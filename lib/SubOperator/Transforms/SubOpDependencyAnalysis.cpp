#include "llvm/Support/Debug.h"

#include "mlir/Dialect/SubOperator/SubOperatorOps.h"
#include "mlir/Dialect/SubOperator/Transforms/SubOpDependencyAnalysis.h"
#include "mlir/Dialect/SCF/IR/SCF.h"

#include <queue>

// Get the directly-accessed state Value for a SubOp, if any.
// Returns the first non-TupleStream operand whose type implements subop::State.
// For ref-based SubOps (ReduceOp, GatherOp, ScatterOp), returns null.
static mlir::Value getDirectStateValue(mlir::subop::SubOperator subop) {
   for (auto operand : subop->getOperands()) {
      if (operand.getType().isa<mlir::tuples::TupleStreamType>()) continue;
      if (operand.getType().isa<mlir::subop::State>()) return operand;
   }
   return {};
}
mlir::subop::SubOpRootAnalysis::SubOpRootAnalysis(mlir::Operation* op) {
   op->walk([&](mlir::subop::SubOperator subop) {
      for (auto x : subop->getOperands()) {
         if (auto pred = mlir::dyn_cast_or_null<mlir::subop::SubOperator>(x.getDefiningOp())) {
            if (x.getType().isa<mlir::tuples::TupleStreamType>()) {
               this->roots[subop].insert(this->roots[subop].end(), this->roots[pred].begin(), this->roots[pred].end());
            }
         }
      }
      if (this->roots[subop].empty()) {
         this->roots[subop].push_back(subop.getOperation());
      }
   });
}
bool mlir::subop::SubOpDependencyAnalysis::isDependentOn(mlir::Operation* curr, mlir::Operation* other) {
   std::unordered_set<mlir::Operation*> visited;
   std::function<bool(mlir::Operation*)> visit = [&](mlir::Operation* op) -> bool {
      if (!visited.insert(op).second) return false; // cycle detection
      for (auto* d : getDependenciesOf(op)) {
         if (d == other) return true;
         if (visit(d)) return true;
      }
      return false;
   };
   return visit(curr);
}
bool mlir::subop::SubOpDependencyAnalysis::areIndependent(mlir::Operation* op, mlir::Operation* op2) {
   return !isDependentOn(op, op2) && !isDependentOn(op2, op);
}
void mlir::subop::SubOpDependencyAnalysis::addToRoot(mlir::Operation* root, mlir::Operation* previousRoot) {
   for (auto* dep : dependencies[previousRoot]) {
      addDependency(root, dep, {});
   }
   for (auto* dep : inverseDependencies[previousRoot]) {
      addDependency(dep, root, {});
   }
}
mlir::subop::SubOpDependencyAnalysis::SubOpDependencyAnalysis(mlir::Operation* op, AnalysisManager& am) {
   SubOpRootAnalysis& rootAnalysis = am.getAnalysis<SubOpRootAnalysis>();
   std::unordered_map<mlir::Operation*, std::vector<mlir::Operation*>> pipelines;
   std::unordered_map<mlir::Operation*, std::vector<mlir::Operation*>> pipelineRequirements;
   std::unordered_map<mlir::Operation*, size_t> dependCount;
   std::queue<mlir::Operation*> queue;
   op->walk([&](mlir::subop::SubOperator subop) {

      auto roots = rootAnalysis.getRoots(subop);
      auto currentState = getDirectStateValue(subop);
      for (auto* subopRoot : roots) {
         for (auto x : subop->getOperands()) {
            if (!x.getType().isa<mlir::tuples::TupleStreamType>()) {
               if (auto* definingOp = x.getDefiningOp()) {
                  if (mlir::dyn_cast_or_null<mlir::subop::SubOperator>(definingOp)) {
                     addDependency(subopRoot, definingOp, roots);
                  } else {
                     if (subopRoot->getBlock() == definingOp->getBlock()) {
                        if (auto getLocal = mlir::dyn_cast_or_null<mlir::subop::GetLocal>(definingOp)) {
                           if (auto* createTLOp = getLocal.getThreadLocal().getDefiningOp()) {
                              pipelineRequirements[subopRoot].push_back(createTLOp);
                           }
                        }
                        pipelineRequirements[subopRoot].push_back(definingOp);
                     }
                  }
               }
            }
         }
         for (auto& region : subop->getRegions()) {
            for (auto& op : region.getOps()) {
               for (auto operand : op.getOperands()) {
                  if (operand.getParentRegion() == subopRoot->getParentRegion()) {
                     if (auto* definingOp = operand.getDefiningOp()) {
                        pipelineRequirements[subopRoot].push_back(definingOp);
                     }
                  }
               }
            }
         }

         for (auto readMember : subop.getReadMembers()) {
            for (auto& record : writtenMembers[readMember]) {
               if (currentState && record.state && currentState != record.state) continue;
               addDependency(subopRoot, record.root, roots);
            }
         }
         for (auto writtenMember : subop.getWrittenMembers()) {
            for (auto& record : writtenMembers[writtenMember]) {
               if (currentState && record.state && currentState != record.state) continue;
               addDependency(subopRoot, record.root, roots);
            }
            for (auto& record : readMembers[writtenMember]) {
               if (currentState && record.state && currentState != record.state) continue;
               addDependency(subopRoot, record.root, roots);
            }
         }
         for (auto readMember : subop.getReadMembers()) {
            readMembers[readMember].push_back({currentState, subopRoot});
         }
         for (auto writtenMember : subop.getWrittenMembers()) {
            writtenMembers[writtenMember].push_back({currentState, subopRoot});
         }

         pipelines[subopRoot].push_back(subop);
      }
   });
   for (auto x : pipelines) {
      dependCount[x.first] = dependencies[x.first].size();
      if (dependCount[x.first] == 0) {
         queue.push(x.first);
      }
   }
   std::unordered_set<mlir::Operation*> availableRequirements;
   while (!queue.empty()) {
      auto* currRoot = queue.front();
      availableRequirements.insert(currRoot);
      queue.pop();
      for (auto* otherRoot : inverseDependencies[currRoot]) {
         if (dependCount[otherRoot] > 0 && otherRoot != currRoot) {
            dependCount[otherRoot]--;
            if (dependCount[otherRoot] == 0) {
               queue.push(otherRoot);
            }
         }
      }
      auto& localOrdering = validOrder[currRoot->getBlock()];
      // Add pipeline requirements and transitively their non-SubOp operand-defining ops.
      // Use depth-first recursion so dependencies are added before their dependents.
      std::function<void(mlir::Operation*)> addRequirement = [&](mlir::Operation* req) {
         if (availableRequirements.contains(req)) return;
         if (!mlir::isa<mlir::subop::SubOperator>(req)) {
            for (auto operand : req->getOperands()) {
               if (auto* defOp = operand.getDefiningOp()) {
                  if (defOp->getBlock() == currRoot->getBlock())
                     addRequirement(defOp);
               }
            }
         }
         availableRequirements.insert(req);
         localOrdering.push_back(req);
      };
      for (auto* requirement : pipelineRequirements[currRoot])
         addRequirement(requirement);
      localOrdering.insert(localOrdering.end(), pipelines[currRoot].begin(), pipelines[currRoot].end());
   }
   // Handle cycles gracefully: if any roots remain with unresolved dependencies,
   // add them in their original block order.
   bool hasCycles = false;
   for (auto [root, c] : dependCount) {
      if (c != 0) {
         hasCycles = true;
         break;
      }
   }
   if (hasCycles) {
      // Collect cyclic roots grouped by block, in original block order
      std::unordered_map<mlir::Block*, std::vector<mlir::Operation*>> cyclicByBlock;
      for (auto& [root, c] : dependCount) {
         if (c != 0) {
            cyclicByBlock[root->getBlock()].push_back(root);
         }
      }
      for (auto& [block, roots] : cyclicByBlock) {
         // Sort by original position in block
         std::sort(roots.begin(), roots.end(), [](mlir::Operation* a, mlir::Operation* b) {
            return a->isBeforeInBlock(b);
         });
         auto& localOrdering = validOrder[block];
         for (auto* root : roots) {
            std::function<void(mlir::Operation*)> addReq = [&](mlir::Operation* req) {
               if (availableRequirements.contains(req)) return;
               if (!mlir::isa<mlir::subop::SubOperator>(req)) {
                  for (auto operand : req->getOperands()) {
                     if (auto* defOp = operand.getDefiningOp()) {
                        if (defOp->getBlock() == root->getBlock())
                           addReq(defOp);
                     }
                  }
               }
               availableRequirements.insert(req);
               localOrdering.push_back(req);
            };
            for (auto* requirement : pipelineRequirements[root])
               addReq(requirement);
            localOrdering.insert(localOrdering.end(), pipelines[root].begin(), pipelines[root].end());
         }
      }
   }
}