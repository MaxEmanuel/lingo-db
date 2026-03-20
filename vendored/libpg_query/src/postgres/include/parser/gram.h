/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_BASE_YY_SRC_BACKEND_PARSER_GRAM_H_INCLUDED
# define YY_BASE_YY_SRC_BACKEND_PARSER_GRAM_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int base_yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    IDENT = 258,                   /* IDENT  */
    FCONST = 259,                  /* FCONST  */
    SCONST = 260,                  /* SCONST  */
    BCONST = 261,                  /* BCONST  */
    XCONST = 262,                  /* XCONST  */
    Op = 263,                      /* Op  */
    ICONST = 264,                  /* ICONST  */
    PARAM = 265,                   /* PARAM  */
    TYPECAST = 266,                /* TYPECAST  */
    DOT_DOT = 267,                 /* DOT_DOT  */
    COLON_EQUALS = 268,            /* COLON_EQUALS  */
    EQUALS_GREATER = 269,          /* EQUALS_GREATER  */
    LESS_EQUALS = 270,             /* LESS_EQUALS  */
    GREATER_EQUALS = 271,          /* GREATER_EQUALS  */
    NOT_EQUALS = 272,              /* NOT_EQUALS  */
    ABORT_P = 273,                 /* ABORT_P  */
    ABSOLUTE_P = 274,              /* ABSOLUTE_P  */
    ACCESS = 275,                  /* ACCESS  */
    ACTION = 276,                  /* ACTION  */
    ADD_P = 277,                   /* ADD_P  */
    ADMIN = 278,                   /* ADMIN  */
    AFTER = 279,                   /* AFTER  */
    AGGREGATE = 280,               /* AGGREGATE  */
    ALL = 281,                     /* ALL  */
    ALSO = 282,                    /* ALSO  */
    ALTER = 283,                   /* ALTER  */
    ALWAYS = 284,                  /* ALWAYS  */
    ANALYSE = 285,                 /* ANALYSE  */
    ANALYZE = 286,                 /* ANALYZE  */
    AND = 287,                     /* AND  */
    ANY = 288,                     /* ANY  */
    ARRAY = 289,                   /* ARRAY  */
    AS = 290,                      /* AS  */
    ASC = 291,                     /* ASC  */
    ASSERTION = 292,               /* ASSERTION  */
    ASSIGNMENT = 293,              /* ASSIGNMENT  */
    ASYMMETRIC = 294,              /* ASYMMETRIC  */
    AT = 295,                      /* AT  */
    ATTRIBUTE = 296,               /* ATTRIBUTE  */
    AUTHORIZATION = 297,           /* AUTHORIZATION  */
    BACKWARD = 298,                /* BACKWARD  */
    BEFORE = 299,                  /* BEFORE  */
    BEGIN_P = 300,                 /* BEGIN_P  */
    BETWEEN = 301,                 /* BETWEEN  */
    BIGINT = 302,                  /* BIGINT  */
    BINARY = 303,                  /* BINARY  */
    BIT = 304,                     /* BIT  */
    BOOLEAN_P = 305,               /* BOOLEAN_P  */
    BOTH = 306,                    /* BOTH  */
    BY = 307,                      /* BY  */
    CACHE = 308,                   /* CACHE  */
    CALLED = 309,                  /* CALLED  */
    CASCADE = 310,                 /* CASCADE  */
    CASCADED = 311,                /* CASCADED  */
    CASE = 312,                    /* CASE  */
    CAST = 313,                    /* CAST  */
    CATALOG_P = 314,               /* CATALOG_P  */
    CHAIN = 315,                   /* CHAIN  */
    CHAR_P = 316,                  /* CHAR_P  */
    CHARACTER = 317,               /* CHARACTER  */
    CHARACTERISTICS = 318,         /* CHARACTERISTICS  */
    CHECK = 319,                   /* CHECK  */
    CHECKPOINT = 320,              /* CHECKPOINT  */
    CLASS = 321,                   /* CLASS  */
    CLOSE = 322,                   /* CLOSE  */
    CLUSTER = 323,                 /* CLUSTER  */
    COALESCE = 324,                /* COALESCE  */
    COLLATE = 325,                 /* COLLATE  */
    COLLATION = 326,               /* COLLATION  */
    COLUMN = 327,                  /* COLUMN  */
    COMMENT = 328,                 /* COMMENT  */
    COMMENTS = 329,                /* COMMENTS  */
    COMMIT = 330,                  /* COMMIT  */
    COMMITTED = 331,               /* COMMITTED  */
    CONCURRENTLY = 332,            /* CONCURRENTLY  */
    CONFIGURATION = 333,           /* CONFIGURATION  */
    CONFLICT = 334,                /* CONFLICT  */
    CONNECTION = 335,              /* CONNECTION  */
    CONSTRAINT = 336,              /* CONSTRAINT  */
    CONSTRAINTS = 337,             /* CONSTRAINTS  */
    CONTENT_P = 338,               /* CONTENT_P  */
    CONTINUE_P = 339,              /* CONTINUE_P  */
    CONVERSION_P = 340,            /* CONVERSION_P  */
    COPY = 341,                    /* COPY  */
    COST = 342,                    /* COST  */
    CREATE = 343,                  /* CREATE  */
    CROSS = 344,                   /* CROSS  */
    CSV = 345,                     /* CSV  */
    CUBE = 346,                    /* CUBE  */
    CURRENT_P = 347,               /* CURRENT_P  */
    CURRENT_CATALOG = 348,         /* CURRENT_CATALOG  */
    CURRENT_DATE = 349,            /* CURRENT_DATE  */
    CURRENT_ROLE = 350,            /* CURRENT_ROLE  */
    CURRENT_SCHEMA = 351,          /* CURRENT_SCHEMA  */
    CURRENT_TIME = 352,            /* CURRENT_TIME  */
    CURRENT_TIMESTAMP = 353,       /* CURRENT_TIMESTAMP  */
    CURRENT_USER = 354,            /* CURRENT_USER  */
    CURSOR = 355,                  /* CURSOR  */
    CYCLE = 356,                   /* CYCLE  */
    DATA_P = 357,                  /* DATA_P  */
    DATABASE = 358,                /* DATABASE  */
    DAY_P = 359,                   /* DAY_P  */
    DEALLOCATE = 360,              /* DEALLOCATE  */
    DEC = 361,                     /* DEC  */
    DECIMAL_P = 362,               /* DECIMAL_P  */
    DECLARE = 363,                 /* DECLARE  */
    DEFAULT = 364,                 /* DEFAULT  */
    DEFAULTS = 365,                /* DEFAULTS  */
    DEFERRABLE = 366,              /* DEFERRABLE  */
    DEFERRED = 367,                /* DEFERRED  */
    DEFINER = 368,                 /* DEFINER  */
    DELETE_P = 369,                /* DELETE_P  */
    DELIMITER = 370,               /* DELIMITER  */
    DELIMITERS = 371,              /* DELIMITERS  */
    DESC = 372,                    /* DESC  */
    DICTIONARY = 373,              /* DICTIONARY  */
    DISABLE_P = 374,               /* DISABLE_P  */
    DISCARD = 375,                 /* DISCARD  */
    DISTINCT = 376,                /* DISTINCT  */
    DO = 377,                      /* DO  */
    DOCUMENT_P = 378,              /* DOCUMENT_P  */
    DOMAIN_P = 379,                /* DOMAIN_P  */
    DOUBLE_P = 380,                /* DOUBLE_P  */
    DROP = 381,                    /* DROP  */
    EACH = 382,                    /* EACH  */
    ELSE = 383,                    /* ELSE  */
    ENABLE_P = 384,                /* ENABLE_P  */
    ENCODING = 385,                /* ENCODING  */
    ENCRYPTED = 386,               /* ENCRYPTED  */
    END_P = 387,                   /* END_P  */
    ENUM_P = 388,                  /* ENUM_P  */
    ESCAPE = 389,                  /* ESCAPE  */
    EVENT = 390,                   /* EVENT  */
    EXCEPT = 391,                  /* EXCEPT  */
    EXCLUDE = 392,                 /* EXCLUDE  */
    EXCLUDING = 393,               /* EXCLUDING  */
    EXCLUSIVE = 394,               /* EXCLUSIVE  */
    EXECUTE = 395,                 /* EXECUTE  */
    EXISTS = 396,                  /* EXISTS  */
    EXPLAIN = 397,                 /* EXPLAIN  */
    EXTENSION = 398,               /* EXTENSION  */
    EXTERNAL = 399,                /* EXTERNAL  */
    EXTRACT = 400,                 /* EXTRACT  */
    FALSE_P = 401,                 /* FALSE_P  */
    FAMILY = 402,                  /* FAMILY  */
    FETCH = 403,                   /* FETCH  */
    FILTER = 404,                  /* FILTER  */
    FIRST_P = 405,                 /* FIRST_P  */
    FLOAT_P = 406,                 /* FLOAT_P  */
    FOLLOWING = 407,               /* FOLLOWING  */
    FOR = 408,                     /* FOR  */
    FORCE = 409,                   /* FORCE  */
    FOREIGN = 410,                 /* FOREIGN  */
    FORWARD = 411,                 /* FORWARD  */
    FREEZE = 412,                  /* FREEZE  */
    FROM = 413,                    /* FROM  */
    FULL = 414,                    /* FULL  */
    FUNCTION = 415,                /* FUNCTION  */
    FUNCTIONS = 416,               /* FUNCTIONS  */
    GLOBAL = 417,                  /* GLOBAL  */
    GRANT = 418,                   /* GRANT  */
    GRANTED = 419,                 /* GRANTED  */
    GREATEST = 420,                /* GREATEST  */
    GROUP_P = 421,                 /* GROUP_P  */
    GROUPING = 422,                /* GROUPING  */
    HANDLER = 423,                 /* HANDLER  */
    HAVING = 424,                  /* HAVING  */
    HEADER_P = 425,                /* HEADER_P  */
    HOLD = 426,                    /* HOLD  */
    HOUR_P = 427,                  /* HOUR_P  */
    IDENTITY_P = 428,              /* IDENTITY_P  */
    IF_P = 429,                    /* IF_P  */
    ILIKE = 430,                   /* ILIKE  */
    IMMEDIATE = 431,               /* IMMEDIATE  */
    IMMUTABLE = 432,               /* IMMUTABLE  */
    IMPLICIT_P = 433,              /* IMPLICIT_P  */
    IMPORT_P = 434,                /* IMPORT_P  */
    IN_P = 435,                    /* IN_P  */
    INCLUDING = 436,               /* INCLUDING  */
    INCREMENT = 437,               /* INCREMENT  */
    INDEX = 438,                   /* INDEX  */
    INDEXES = 439,                 /* INDEXES  */
    INHERIT = 440,                 /* INHERIT  */
    INHERITS = 441,                /* INHERITS  */
    INITIALLY = 442,               /* INITIALLY  */
    INLINE_P = 443,                /* INLINE_P  */
    INNER_P = 444,                 /* INNER_P  */
    INOUT = 445,                   /* INOUT  */
    INPUT_P = 446,                 /* INPUT_P  */
    INSENSITIVE = 447,             /* INSENSITIVE  */
    INSERT = 448,                  /* INSERT  */
    INSTEAD = 449,                 /* INSTEAD  */
    INT_P = 450,                   /* INT_P  */
    INTEGER = 451,                 /* INTEGER  */
    INTERSECT = 452,               /* INTERSECT  */
    INTERVAL = 453,                /* INTERVAL  */
    INTO = 454,                    /* INTO  */
    INVOKER = 455,                 /* INVOKER  */
    IS = 456,                      /* IS  */
    ISNULL = 457,                  /* ISNULL  */
    ISOLATION = 458,               /* ISOLATION  */
    ITERATIVE = 459,               /* ITERATIVE  */
    JOIN = 460,                    /* JOIN  */
    KEY = 461,                     /* KEY  */
    LABEL = 462,                   /* LABEL  */
    LAMBDA = 463,                  /* LAMBDA  */
    LANGUAGE = 464,                /* LANGUAGE  */
    LARGE_P = 465,                 /* LARGE_P  */
    LAST_P = 466,                  /* LAST_P  */
    LATERAL_P = 467,               /* LATERAL_P  */
    LEADING = 468,                 /* LEADING  */
    LEAKPROOF = 469,               /* LEAKPROOF  */
    LEAST = 470,                   /* LEAST  */
    LEFT = 471,                    /* LEFT  */
    LEVEL = 472,                   /* LEVEL  */
    LIKE = 473,                    /* LIKE  */
    LIMIT = 474,                   /* LIMIT  */
    LISTEN = 475,                  /* LISTEN  */
    LOAD = 476,                    /* LOAD  */
    LOCAL = 477,                   /* LOCAL  */
    LOCALTIME = 478,               /* LOCALTIME  */
    LOCALTIMESTAMP = 479,          /* LOCALTIMESTAMP  */
    LOCATION = 480,                /* LOCATION  */
    LOCK_P = 481,                  /* LOCK_P  */
    LOCKED = 482,                  /* LOCKED  */
    LOGGED = 483,                  /* LOGGED  */
    MAPPING = 484,                 /* MAPPING  */
    MATCH = 485,                   /* MATCH  */
    MATERIALIZED = 486,            /* MATERIALIZED  */
    MAXVALUE = 487,                /* MAXVALUE  */
    MINUTE_P = 488,                /* MINUTE_P  */
    MINVALUE = 489,                /* MINVALUE  */
    MODE = 490,                    /* MODE  */
    MONTH_P = 491,                 /* MONTH_P  */
    MOVE = 492,                    /* MOVE  */
    NAME_P = 493,                  /* NAME_P  */
    NAMES = 494,                   /* NAMES  */
    NATIONAL = 495,                /* NATIONAL  */
    NATURAL = 496,                 /* NATURAL  */
    NCHAR = 497,                   /* NCHAR  */
    NEXT = 498,                    /* NEXT  */
    NO = 499,                      /* NO  */
    NONE = 500,                    /* NONE  */
    NOT = 501,                     /* NOT  */
    NOTHING = 502,                 /* NOTHING  */
    NOTIFY = 503,                  /* NOTIFY  */
    NOTNULL = 504,                 /* NOTNULL  */
    NOWAIT = 505,                  /* NOWAIT  */
    NULL_P = 506,                  /* NULL_P  */
    NULLIF = 507,                  /* NULLIF  */
    NULLS_P = 508,                 /* NULLS_P  */
    NUMERIC = 509,                 /* NUMERIC  */
    OBJECT_P = 510,                /* OBJECT_P  */
    OF = 511,                      /* OF  */
    OFF = 512,                     /* OFF  */
    OFFSET = 513,                  /* OFFSET  */
    OIDS = 514,                    /* OIDS  */
    ON = 515,                      /* ON  */
    ONLY = 516,                    /* ONLY  */
    OPERATOR = 517,                /* OPERATOR  */
    OPTION = 518,                  /* OPTION  */
    OPTIONS = 519,                 /* OPTIONS  */
    OR = 520,                      /* OR  */
    ORDER = 521,                   /* ORDER  */
    ORDINALITY = 522,              /* ORDINALITY  */
    OUT_P = 523,                   /* OUT_P  */
    OUTER_P = 524,                 /* OUTER_P  */
    OVER = 525,                    /* OVER  */
    OVERLAPS = 526,                /* OVERLAPS  */
    OVERLAY = 527,                 /* OVERLAY  */
    OWNED = 528,                   /* OWNED  */
    OWNER = 529,                   /* OWNER  */
    PARSER = 530,                  /* PARSER  */
    PARTIAL = 531,                 /* PARTIAL  */
    PARTITION = 532,               /* PARTITION  */
    PASSING = 533,                 /* PASSING  */
    PASSWORD = 534,                /* PASSWORD  */
    PLACING = 535,                 /* PLACING  */
    PLANS = 536,                   /* PLANS  */
    POLICY = 537,                  /* POLICY  */
    POSITION = 538,                /* POSITION  */
    PRECEDING = 539,               /* PRECEDING  */
    PRECISION = 540,               /* PRECISION  */
    PRESERVE = 541,                /* PRESERVE  */
    PREPARE = 542,                 /* PREPARE  */
    PREPARED = 543,                /* PREPARED  */
    PRIMARY = 544,                 /* PRIMARY  */
    PRIOR = 545,                   /* PRIOR  */
    PRIVILEGES = 546,              /* PRIVILEGES  */
    PROCEDURAL = 547,              /* PROCEDURAL  */
    PROCEDURE = 548,               /* PROCEDURE  */
    PROGRAM = 549,                 /* PROGRAM  */
    QUOTE = 550,                   /* QUOTE  */
    RANGE = 551,                   /* RANGE  */
    READ = 552,                    /* READ  */
    REAL = 553,                    /* REAL  */
    REASSIGN = 554,                /* REASSIGN  */
    RECHECK = 555,                 /* RECHECK  */
    RECURSIVE = 556,               /* RECURSIVE  */
    REF = 557,                     /* REF  */
    REFERENCES = 558,              /* REFERENCES  */
    REFRESH = 559,                 /* REFRESH  */
    REINDEX = 560,                 /* REINDEX  */
    RELATIVE_P = 561,              /* RELATIVE_P  */
    RELEASE = 562,                 /* RELEASE  */
    RENAME = 563,                  /* RENAME  */
    REPEATABLE = 564,              /* REPEATABLE  */
    REPLACE = 565,                 /* REPLACE  */
    REPLICA = 566,                 /* REPLICA  */
    RESET = 567,                   /* RESET  */
    RESTART = 568,                 /* RESTART  */
    RESTRICT = 569,                /* RESTRICT  */
    RETURNING = 570,               /* RETURNING  */
    RETURNS = 571,                 /* RETURNS  */
    REVOKE = 572,                  /* REVOKE  */
    RIGHT = 573,                   /* RIGHT  */
    ROLE = 574,                    /* ROLE  */
    ROLLBACK = 575,                /* ROLLBACK  */
    ROLLUP = 576,                  /* ROLLUP  */
    ROW = 577,                     /* ROW  */
    ROWS = 578,                    /* ROWS  */
    RULE = 579,                    /* RULE  */
    SAVEPOINT = 580,               /* SAVEPOINT  */
    SCHEMA = 581,                  /* SCHEMA  */
    SCROLL = 582,                  /* SCROLL  */
    SEARCH = 583,                  /* SEARCH  */
    SECOND_P = 584,                /* SECOND_P  */
    SECURITY = 585,                /* SECURITY  */
    SELECT = 586,                  /* SELECT  */
    SEQUENCE = 587,                /* SEQUENCE  */
    SEQUENCES = 588,               /* SEQUENCES  */
    SERIALIZABLE = 589,            /* SERIALIZABLE  */
    SERVER = 590,                  /* SERVER  */
    SESSION = 591,                 /* SESSION  */
    SESSION_USER = 592,            /* SESSION_USER  */
    SET = 593,                     /* SET  */
    SETS = 594,                    /* SETS  */
    SETOF = 595,                   /* SETOF  */
    SHARE = 596,                   /* SHARE  */
    SHOW = 597,                    /* SHOW  */
    SIMILAR = 598,                 /* SIMILAR  */
    SIMPLE = 599,                  /* SIMPLE  */
    SKIP = 600,                    /* SKIP  */
    SMALLINT = 601,                /* SMALLINT  */
    SNAPSHOT = 602,                /* SNAPSHOT  */
    SOME = 603,                    /* SOME  */
    SQL_P = 604,                   /* SQL_P  */
    STABLE = 605,                  /* STABLE  */
    STANDALONE_P = 606,            /* STANDALONE_P  */
    START = 607,                   /* START  */
    STATEMENT = 608,               /* STATEMENT  */
    STATISTICS = 609,              /* STATISTICS  */
    STDIN = 610,                   /* STDIN  */
    STDOUT = 611,                  /* STDOUT  */
    STORAGE = 612,                 /* STORAGE  */
    STRICT_P = 613,                /* STRICT_P  */
    STRIP_P = 614,                 /* STRIP_P  */
    SUBSTRING = 615,               /* SUBSTRING  */
    SYMMETRIC = 616,               /* SYMMETRIC  */
    SYSID = 617,                   /* SYSID  */
    SYSTEM_P = 618,                /* SYSTEM_P  */
    TABLE = 619,                   /* TABLE  */
    TABLES = 620,                  /* TABLES  */
    TABLESAMPLE = 621,             /* TABLESAMPLE  */
    TABLESPACE = 622,              /* TABLESPACE  */
    TEMP = 623,                    /* TEMP  */
    TEMPLATE = 624,                /* TEMPLATE  */
    TEMPORARY = 625,               /* TEMPORARY  */
    TEXT_P = 626,                  /* TEXT_P  */
    THEN = 627,                    /* THEN  */
    TIME = 628,                    /* TIME  */
    TIMESTAMP = 629,               /* TIMESTAMP  */
    TO = 630,                      /* TO  */
    TRAILING = 631,                /* TRAILING  */
    TRANSACTION = 632,             /* TRANSACTION  */
    TRANSFORM = 633,               /* TRANSFORM  */
    TREAT = 634,                   /* TREAT  */
    TRIGGER = 635,                 /* TRIGGER  */
    TRIM = 636,                    /* TRIM  */
    TRUE_P = 637,                  /* TRUE_P  */
    TRUNCATE = 638,                /* TRUNCATE  */
    TRUSTED = 639,                 /* TRUSTED  */
    TYPE_P = 640,                  /* TYPE_P  */
    TYPES_P = 641,                 /* TYPES_P  */
    UNBOUNDED = 642,               /* UNBOUNDED  */
    UNCOMMITTED = 643,             /* UNCOMMITTED  */
    UNENCRYPTED = 644,             /* UNENCRYPTED  */
    UNION = 645,                   /* UNION  */
    UNIQUE = 646,                  /* UNIQUE  */
    UNKNOWN = 647,                 /* UNKNOWN  */
    UNLISTEN = 648,                /* UNLISTEN  */
    UNLOGGED = 649,                /* UNLOGGED  */
    UNTIL = 650,                   /* UNTIL  */
    UPDATE = 651,                  /* UPDATE  */
    USER = 652,                    /* USER  */
    USING = 653,                   /* USING  */
    VACUUM = 654,                  /* VACUUM  */
    VALID = 655,                   /* VALID  */
    VALIDATE = 656,                /* VALIDATE  */
    VALIDATOR = 657,               /* VALIDATOR  */
    VALUE_P = 658,                 /* VALUE_P  */
    VALUES = 659,                  /* VALUES  */
    VARCHAR = 660,                 /* VARCHAR  */
    VARIADIC = 661,                /* VARIADIC  */
    VARYING = 662,                 /* VARYING  */
    VERBOSE = 663,                 /* VERBOSE  */
    VERSION_P = 664,               /* VERSION_P  */
    VIEW = 665,                    /* VIEW  */
    VIEWS = 666,                   /* VIEWS  */
    VOLATILE = 667,                /* VOLATILE  */
    WHEN = 668,                    /* WHEN  */
    WHERE = 669,                   /* WHERE  */
    WHITESPACE_P = 670,            /* WHITESPACE_P  */
    WINDOW = 671,                  /* WINDOW  */
    WITH = 672,                    /* WITH  */
    WITHIN = 673,                  /* WITHIN  */
    WITHOUT = 674,                 /* WITHOUT  */
    WORK = 675,                    /* WORK  */
    WRAPPER = 676,                 /* WRAPPER  */
    WRITE = 677,                   /* WRITE  */
    XML_P = 678,                   /* XML_P  */
    XMLATTRIBUTES = 679,           /* XMLATTRIBUTES  */
    XMLCONCAT = 680,               /* XMLCONCAT  */
    XMLELEMENT = 681,              /* XMLELEMENT  */
    XMLEXISTS = 682,               /* XMLEXISTS  */
    XMLFOREST = 683,               /* XMLFOREST  */
    XMLPARSE = 684,                /* XMLPARSE  */
    XMLPI = 685,                   /* XMLPI  */
    XMLROOT = 686,                 /* XMLROOT  */
    XMLSERIALIZE = 687,            /* XMLSERIALIZE  */
    YEAR_P = 688,                  /* YEAR_P  */
    YES_P = 689,                   /* YES_P  */
    ZONE = 690,                    /* ZONE  */
    NOT_LA = 691,                  /* NOT_LA  */
    NULLS_LA = 692,                /* NULLS_LA  */
    WITH_LA = 693,                 /* WITH_LA  */
    POSTFIXOP = 694,               /* POSTFIXOP  */
    UMINUS = 695                   /* UMINUS  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 195 "gram.y"

	core_YYSTYPE		core_yystype;
	/* these fields must match core_YYSTYPE: */
	int					ival;
	char				*str;
	const char			*keyword;

	char				chr;
	bool				boolean;
	JoinType			jtype;
	DropBehavior		dbehavior;
	OnCommitAction		oncommit;
	List				*list;
	Node				*node;
	Value				*value;
	ObjectType			objtype;
	TypeName			*typnam;
	FunctionParameter   *fun_param;
	FunctionParameterMode fun_param_mode;
	FuncWithArgs		*funwithargs;
	DefElem				*defelt;
	SortBy				*sortby;
	WindowDef			*windef;
	JoinExpr			*jexpr;
	IndexElem			*ielem;
	Alias				*alias;
	RangeVar			*range;
	IntoClause			*into;
	WithClause			*with;
	InferClause			*infer;
	OnConflictClause	*onconflict;
	A_Indices			*aind;
	ResTarget			*target;
	struct PrivTarget	*privtarget;
	AccessPriv			*accesspriv;
	struct ImportQual	*importqual;
	InsertStmt			*istmt;
	VariableSetStmt		*vsetstmt;

#line 544 "src_backend_parser_gram.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif

/* Location type.  */
#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE YYLTYPE;
struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
};
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif




int base_yyparse (core_yyscan_t yyscanner);


#endif /* !YY_BASE_YY_SRC_BACKEND_PARSER_GRAM_H_INCLUDED  */
