/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

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

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     IDENT = 258,
     FCONST = 259,
     SCONST = 260,
     BCONST = 261,
     XCONST = 262,
     Op = 263,
     ICONST = 264,
     PARAM = 265,
     TYPECAST = 266,
     DOT_DOT = 267,
     COLON_EQUALS = 268,
     EQUALS_GREATER = 269,
     LESS_EQUALS = 270,
     GREATER_EQUALS = 271,
     NOT_EQUALS = 272,
     ABORT_P = 273,
     ABSOLUTE_P = 274,
     ACCESS = 275,
     ACTION = 276,
     ADD_P = 277,
     ADMIN = 278,
     AFTER = 279,
     AGGREGATE = 280,
     ALL = 281,
     ALSO = 282,
     ALTER = 283,
     ALWAYS = 284,
     ANALYSE = 285,
     ANALYZE = 286,
     AND = 287,
     ANY = 288,
     ARRAY = 289,
     AS = 290,
     ASC = 291,
     ASSERTION = 292,
     ASSIGNMENT = 293,
     ASYMMETRIC = 294,
     AT = 295,
     ATTRIBUTE = 296,
     AUTHORIZATION = 297,
     BACKWARD = 298,
     BEFORE = 299,
     BEGIN_P = 300,
     BETWEEN = 301,
     BIGINT = 302,
     BINARY = 303,
     BIT = 304,
     BOOLEAN_P = 305,
     BOTH = 306,
     BY = 307,
     CACHE = 308,
     CALLED = 309,
     CASCADE = 310,
     CASCADED = 311,
     CASE = 312,
     CAST = 313,
     CATALOG_P = 314,
     CHAIN = 315,
     CHAR_P = 316,
     CHARACTER = 317,
     CHARACTERISTICS = 318,
     CHECK = 319,
     CHECKPOINT = 320,
     CLASS = 321,
     CLOSE = 322,
     CLUSTER = 323,
     COALESCE = 324,
     COLLATE = 325,
     COLLATION = 326,
     COLUMN = 327,
     COMMENT = 328,
     COMMENTS = 329,
     COMMIT = 330,
     COMMITTED = 331,
     CONCURRENTLY = 332,
     CONFIGURATION = 333,
     CONFLICT = 334,
     CONNECTION = 335,
     CONSTRAINT = 336,
     CONSTRAINTS = 337,
     CONTENT_P = 338,
     CONTINUE_P = 339,
     CONVERSION_P = 340,
     COPY = 341,
     COST = 342,
     CREATE = 343,
     CROSS = 344,
     CSV = 345,
     CUBE = 346,
     CURRENT_P = 347,
     CURRENT_CATALOG = 348,
     CURRENT_DATE = 349,
     CURRENT_ROLE = 350,
     CURRENT_SCHEMA = 351,
     CURRENT_TIME = 352,
     CURRENT_TIMESTAMP = 353,
     CURRENT_USER = 354,
     CURSOR = 355,
     CYCLE = 356,
     DATA_P = 357,
     DATABASE = 358,
     DAY_P = 359,
     DEALLOCATE = 360,
     DEC = 361,
     DECIMAL_P = 362,
     DECLARE = 363,
     DEFAULT = 364,
     DEFAULTS = 365,
     DEFERRABLE = 366,
     DEFERRED = 367,
     DEFINER = 368,
     DELETE_P = 369,
     DELIMITER = 370,
     DELIMITERS = 371,
     DESC = 372,
     DICTIONARY = 373,
     DISABLE_P = 374,
     DISCARD = 375,
     DISTINCT = 376,
     DO = 377,
     DOCUMENT_P = 378,
     DOMAIN_P = 379,
     DOUBLE_P = 380,
     DROP = 381,
     EACH = 382,
     ELSE = 383,
     ENABLE_P = 384,
     ENCODING = 385,
     ENCRYPTED = 386,
     END_P = 387,
     ENUM_P = 388,
     ESCAPE = 389,
     EVENT = 390,
     EXCEPT = 391,
     EXCLUDE = 392,
     EXCLUDING = 393,
     EXCLUSIVE = 394,
     EXECUTE = 395,
     EXISTS = 396,
     EXPLAIN = 397,
     EXTENSION = 398,
     EXTERNAL = 399,
     EXTRACT = 400,
     FALSE_P = 401,
     FAMILY = 402,
     FETCH = 403,
     FILTER = 404,
     FIRST_P = 405,
     FLOAT_P = 406,
     FOLLOWING = 407,
     FOR = 408,
     FORCE = 409,
     FOREIGN = 410,
     FORWARD = 411,
     FREEZE = 412,
     FROM = 413,
     FULL = 414,
     FUNCTION = 415,
     FUNCTIONS = 416,
     GLOBAL = 417,
     GRANT = 418,
     GRANTED = 419,
     GREATEST = 420,
     GROUP_P = 421,
     GROUPING = 422,
     HANDLER = 423,
     HAVING = 424,
     HEADER_P = 425,
     HOLD = 426,
     HOUR_P = 427,
     IDENTITY_P = 428,
     IF_P = 429,
     ILIKE = 430,
     IMMEDIATE = 431,
     IMMUTABLE = 432,
     IMPLICIT_P = 433,
     IMPORT_P = 434,
     IN_P = 435,
     INCLUDING = 436,
     INCREMENT = 437,
     INDEX = 438,
     INDEXES = 439,
     INHERIT = 440,
     INHERITS = 441,
     INITIALLY = 442,
     INLINE_P = 443,
     INNER_P = 444,
     INOUT = 445,
     INPUT_P = 446,
     INSENSITIVE = 447,
     INSERT = 448,
     INSTEAD = 449,
     INT_P = 450,
     INTEGER = 451,
     INTERSECT = 452,
     INTERVAL = 453,
     INTO = 454,
     INVOKER = 455,
     IS = 456,
     ISNULL = 457,
     ISOLATION = 458,
     JOIN = 459,
     KEY = 460,
     LABEL = 461,
     LAMBDA = 462,
     LANGUAGE  = 463,
     LARGE_P  = 464,
     LAST_P  = 465,
     LATERAL_P  = 466,
     LEADING  = 467,
     LEAKPROOF  = 468,
     LEAST  = 469,
     LEFT  = 470,
     LEVEL  = 471,
     LIKE  = 472,
     LIMIT  = 473,
     LISTEN  = 474,
     LOAD  = 475,
     LOCAL  = 476,
     LOCALTIME  = 477,
     LOCALTIMESTAMP  = 478,
     LOCATION  = 479,
     LOCK_P  = 480,
     LOCKED  = 481,
     LOGGED  = 482,
     MAPPING  = 483,
     MATCH  = 484,
     MATERIALIZED  = 485,
     MAXVALUE  = 486,
     MINUTE_P  = 487,
     MINVALUE  = 488,
     MODE  = 489,
     MONTH_P  = 490,
     MOVE  = 491,
     NAME_P  = 492,
     NAMES  = 493,
     NATIONAL  = 494,
     NATURAL  = 495,
     NCHAR  = 496,
     NEXT  = 497,
     NO  = 498,
     NONE  = 499,
     NOT  = 500,
     NOTHING  = 501,
     NOTIFY  = 502,
     NOTNULL  = 503,
     NOWAIT  = 504,
     NULL_P  = 505,
     NULLIF  = 506,
     NULLS_P  = 507,
     NUMERIC  = 508,
     OBJECT_P  = 509,
     OF  = 510,
     OFF  = 511,
     OFFSET  = 512,
     OIDS  = 513,
     ON  = 514,
     ONLY  = 515,
     OPERATOR  = 516,
     OPTION  = 517,
     OPTIONS  = 518,
     OR  = 519,
     ORDER  = 520,
     ORDINALITY  = 521,
     OUT_P  = 522,
     OUTER_P  = 523,
     OVER  = 524,
     OVERLAPS  = 525,
     OVERLAY  = 526,
     OWNED  = 527,
     OWNER  = 528,
     PARSER  = 529,
     PARTIAL  = 530,
     PARTITION  = 531,
     PASSING  = 532,
     PASSWORD  = 533,
     PLACING  = 534,
     PLANS  = 535,
     POLICY  = 536,
     POSITION  = 537,
     PRECEDING  = 538,
     PRECISION  = 539,
     PRESERVE  = 540,
     PREPARE  = 541,
     PREPARED  = 542,
     PRIMARY  = 543,
     PRIOR  = 544,
     PRIVILEGES  = 545,
     PROCEDURAL  = 546,
     PROCEDURE  = 547,
     PROGRAM  = 548,
     QUOTE  = 549,
     RANGE  = 550,
     READ  = 551,
     REAL  = 552,
     REASSIGN  = 553,
     RECHECK  = 554,
     RECURSIVE  = 555,
     REF  = 556,
     REFERENCES  = 557,
     REFRESH  = 558,
     REINDEX  = 559,
     RELATIVE_P  = 560,
     RELEASE  = 561,
     RENAME  = 562,
     REPEATABLE  = 563,
     REPLACE  = 564,
     REPLICA  = 565,
     RESET  = 566,
     RESTART  = 567,
     RESTRICT  = 568,
     RETURNING  = 569,
     RETURNS  = 570,
     REVOKE  = 571,
     RIGHT  = 572,
     ROLE  = 573,
     ROLLBACK  = 574,
     ROLLUP  = 575,
     ROW  = 576,
     ROWS  = 577,
     RULE  = 578,
     SAVEPOINT  = 579,
     SCHEMA  = 580,
     SCROLL  = 581,
     SEARCH  = 582,
     SECOND_P  = 583,
     SECURITY  = 584,
     SELECT  = 585,
     SEQUENCE  = 586,
     SEQUENCES  = 587,
     SERIALIZABLE  = 588,
     SERVER  = 589,
     SESSION  = 590,
     SESSION_USER  = 591,
     SET  = 592,
     SETS  = 593,
     SETOF  = 594,
     SHARE  = 595,
     SHOW  = 596,
     SIMILAR  = 597,
     SIMPLE  = 598,
     SKIP  = 599,
     SMALLINT  = 600,
     SNAPSHOT  = 601,
     SOME  = 602,
     SQL_P  = 603,
     STABLE  = 604,
     STANDALONE_P  = 605,
     START  = 606,
     STATEMENT  = 607,
     STATISTICS  = 608,
     STDIN  = 609,
     STDOUT  = 610,
     STORAGE  = 611,
     STRICT_P  = 612,
     STRIP_P  = 613,
     SUBSTRING  = 614,
     SYMMETRIC  = 615,
     SYSID  = 616,
     SYSTEM_P  = 617,
     TABLE  = 618,
     TABLES  = 619,
     TABLESAMPLE  = 620,
     TABLESPACE  = 621,
     TEMP  = 622,
     TEMPLATE  = 623,
     TEMPORARY  = 624,
     TEXT_P  = 625,
     THEN  = 626,
     TIME  = 627,
     TIMESTAMP  = 628,
     TO  = 629,
     TRAILING  = 630,
     TRANSACTION  = 631,
     TRANSFORM  = 632,
     TREAT  = 633,
     TRIGGER  = 634,
     TRIM  = 635,
     TRUE_P  = 636,
     TRUNCATE  = 637,
     TRUSTED  = 638,
     TYPE_P  = 639,
     TYPES_P  = 640,
     UNBOUNDED  = 641,
     UNCOMMITTED  = 642,
     UNENCRYPTED  = 643,
     UNION  = 644,
     UNIQUE  = 645,
     UNKNOWN  = 646,
     UNLISTEN  = 647,
     UNLOGGED  = 648,
     UNTIL  = 649,
     UPDATE  = 650,
     USER  = 651,
     USING  = 652,
     VACUUM  = 653,
     VALID  = 654,
     VALIDATE  = 655,
     VALIDATOR  = 656,
     VALUE_P  = 657,
     VALUES  = 658,
     VARCHAR  = 659,
     VARIADIC  = 660,
     VARYING  = 661,
     VERBOSE  = 662,
     VERSION_P  = 663,
     VIEW  = 664,
     VIEWS  = 665,
     VOLATILE  = 666,
     WHEN  = 667,
     WHERE  = 668,
     WHITESPACE_P  = 669,
     WINDOW  = 670,
     WITH  = 671,
     WITHIN  = 672,
     WITHOUT  = 673,
     WORK  = 674,
     WRAPPER  = 675,
     WRITE  = 676,
     XML_P  = 677,
     XMLATTRIBUTES  = 678,
     XMLCONCAT  = 679,
     XMLELEMENT  = 680,
     XMLEXISTS  = 681,
     XMLFOREST  = 682,
     XMLPARSE  = 683,
     XMLPI  = 684,
     XMLROOT  = 685,
     XMLSERIALIZE  = 686,
     YEAR_P  = 687,
     YES_P  = 688,
     ZONE  = 689,
     NOT_LA  = 690,
     NULLS_LA  = 691,
     WITH_LA  = 692,
     POSTFIXOP  = 693,
     UMINUS  = 694
   };
#endif
/* Tokens.  */
#define IDENT 258
#define FCONST 259
#define SCONST 260
#define BCONST 261
#define XCONST 262
#define Op 263
#define ICONST 264
#define PARAM 265
#define TYPECAST 266
#define DOT_DOT 267
#define COLON_EQUALS 268
#define EQUALS_GREATER 269
#define LESS_EQUALS 270
#define GREATER_EQUALS 271
#define NOT_EQUALS 272
#define ABORT_P 273
#define ABSOLUTE_P 274
#define ACCESS 275
#define ACTION 276
#define ADD_P 277
#define ADMIN 278
#define AFTER 279
#define AGGREGATE 280
#define ALL 281
#define ALSO 282
#define ALTER 283
#define ALWAYS 284
#define ANALYSE 285
#define ANALYZE 286
#define AND 287
#define ANY 288
#define ARRAY 289
#define AS 290
#define ASC 291
#define ASSERTION 292
#define ASSIGNMENT 293
#define ASYMMETRIC 294
#define AT 295
#define ATTRIBUTE 296
#define AUTHORIZATION 297
#define BACKWARD 298
#define BEFORE 299
#define BEGIN_P 300
#define BETWEEN 301
#define BIGINT 302
#define BINARY 303
#define BIT 304
#define BOOLEAN_P 305
#define BOTH 306
#define BY 307
#define CACHE 308
#define CALLED 309
#define CASCADE 310
#define CASCADED 311
#define CASE 312
#define CAST 313
#define CATALOG_P 314
#define CHAIN 315
#define CHAR_P 316
#define CHARACTER 317
#define CHARACTERISTICS 318
#define CHECK 319
#define CHECKPOINT 320
#define CLASS 321
#define CLOSE 322
#define CLUSTER 323
#define COALESCE 324
#define COLLATE 325
#define COLLATION 326
#define COLUMN 327
#define COMMENT 328
#define COMMENTS 329
#define COMMIT 330
#define COMMITTED 331
#define CONCURRENTLY 332
#define CONFIGURATION 333
#define CONFLICT 334
#define CONNECTION 335
#define CONSTRAINT 336
#define CONSTRAINTS 337
#define CONTENT_P 338
#define CONTINUE_P 339
#define CONVERSION_P 340
#define COPY 341
#define COST 342
#define CREATE 343
#define CROSS 344
#define CSV 345
#define CUBE 346
#define CURRENT_P 347
#define CURRENT_CATALOG 348
#define CURRENT_DATE 349
#define CURRENT_ROLE 350
#define CURRENT_SCHEMA 351
#define CURRENT_TIME 352
#define CURRENT_TIMESTAMP 353
#define CURRENT_USER 354
#define CURSOR 355
#define CYCLE 356
#define DATA_P 357
#define DATABASE 358
#define DAY_P 359
#define DEALLOCATE 360
#define DEC 361
#define DECIMAL_P 362
#define DECLARE 363
#define DEFAULT 364
#define DEFAULTS 365
#define DEFERRABLE 366
#define DEFERRED 367
#define DEFINER 368
#define DELETE_P 369
#define DELIMITER 370
#define DELIMITERS 371
#define DESC 372
#define DICTIONARY 373
#define DISABLE_P 374
#define DISCARD 375
#define DISTINCT 376
#define DO 377
#define DOCUMENT_P 378
#define DOMAIN_P 379
#define DOUBLE_P 380
#define DROP 381
#define EACH 382
#define ELSE 383
#define ENABLE_P 384
#define ENCODING 385
#define ENCRYPTED 386
#define END_P 387
#define ENUM_P 388
#define ESCAPE 389
#define EVENT 390
#define EXCEPT 391
#define EXCLUDE 392
#define EXCLUDING 393
#define EXCLUSIVE 394
#define EXECUTE 395
#define EXISTS 396
#define EXPLAIN 397
#define EXTENSION 398
#define EXTERNAL 399
#define EXTRACT 400
#define FALSE_P 401
#define FAMILY 402
#define FETCH 403
#define FILTER 404
#define FIRST_P 405
#define FLOAT_P 406
#define FOLLOWING 407
#define FOR 408
#define FORCE 409
#define FOREIGN 410
#define FORWARD 411
#define FREEZE 412
#define FROM 413
#define FULL 414
#define FUNCTION 415
#define FUNCTIONS 416
#define GLOBAL 417
#define GRANT 418
#define GRANTED 419
#define GREATEST 420
#define GROUP_P 421
#define GROUPING 422
#define HANDLER 423
#define HAVING 424
#define HEADER_P 425
#define HOLD 426
#define HOUR_P 427
#define IDENTITY_P 428
#define IF_P 429
#define ILIKE 430
#define IMMEDIATE 431
#define IMMUTABLE 432
#define IMPLICIT_P 433
#define IMPORT_P 434
#define IN_P 435
#define INCLUDING 436
#define INCREMENT 437
#define INDEX 438
#define INDEXES 439
#define INHERIT 440
#define INHERITS 441
#define INITIALLY 442
#define INLINE_P 443
#define INNER_P 444
#define INOUT 445
#define INPUT_P 446
#define INSENSITIVE 447
#define INSERT 448
#define INSTEAD 449
#define INT_P 450
#define INTEGER 451
#define INTERSECT 452
#define INTERVAL 453
#define INTO 454
#define INVOKER 455
#define IS 456
#define ISNULL 457
#define ISOLATION 458
#define ITERATIVE 459
#define JOIN 460
#define KEY 461
#define LABEL 462
#define LAMBDA 463
#define LANGUAGE 464
#define LARGE_P 465
#define LAST_P 466
#define LATERAL_P 467
#define LEADING 468
#define LEAKPROOF 469
#define LEAST 470
#define LEFT 471
#define LEVEL 472
#define LIKE 473
#define LIMIT 474
#define LISTEN 475
#define LOAD 476
#define LOCAL 477
#define LOCALTIME 478
#define LOCALTIMESTAMP 479
#define LOCATION 480
#define LOCK_P 481
#define LOCKED 482
#define LOGGED 483
#define MAPPING 484
#define MATCH 485
#define MATERIALIZED 486
#define MAXVALUE 487
#define MINUTE_P 488
#define MINVALUE 489
#define MODE 490
#define MONTH_P 491
#define MOVE 492
#define NAME_P 493
#define NAMES 494
#define NATIONAL 495
#define NATURAL 496
#define NCHAR 497
#define NEXT 498
#define NO 499
#define NONE 500
#define NOT 501
#define NOTHING 502
#define NOTIFY 503
#define NOTNULL 504
#define NOWAIT 505
#define NULL_P 506
#define NULLIF 507
#define NULLS_P 508
#define NUMERIC 509
#define OBJECT_P 510
#define OF 511
#define OFF 512
#define OFFSET 513
#define OIDS 514
#define ON 515
#define ONLY 516
#define OPERATOR 517
#define OPTION 518
#define OPTIONS 519
#define OR 520
#define ORDER 521
#define ORDINALITY 522
#define OUT_P 523
#define OUTER_P 524
#define OVER 525
#define OVERLAPS 526
#define OVERLAY 527
#define OWNED 528
#define OWNER 529
#define PARSER 530
#define PARTIAL 531
#define PARTITION 532
#define PASSING 533
#define PASSWORD 534
#define PLACING 535
#define PLANS 536
#define POLICY 537
#define POSITION 538
#define PRECEDING 539
#define PRECISION 540
#define PRESERVE 541
#define PREPARE 542
#define PREPARED 543
#define PRIMARY 544
#define PRIOR 545
#define PRIVILEGES 546
#define PROCEDURAL 547
#define PROCEDURE 548
#define PROGRAM 549
#define QUOTE 550
#define RANGE 551
#define READ 552
#define REAL 553
#define REASSIGN 554
#define RECHECK 555
#define RECURSIVE 556
#define REF 557
#define REFERENCES 558
#define REFRESH 559
#define REINDEX 560
#define RELATIVE_P 561
#define RELEASE 562
#define RENAME 563
#define REPEATABLE 564
#define REPLACE 565
#define REPLICA 566
#define RESET 567
#define RESTART 568
#define RESTRICT 569
#define RETURNING 570
#define RETURNS 571
#define REVOKE 572
#define RIGHT 573
#define ROLE 574
#define ROLLBACK 575
#define ROLLUP 576
#define ROW 577
#define ROWS 578
#define RULE 579
#define SAVEPOINT 580
#define SCHEMA 581
#define SCROLL 582
#define SEARCH 583
#define SECOND_P 584
#define SECURITY 585
#define SELECT 586
#define SEQUENCE 587
#define SEQUENCES 588
#define SERIALIZABLE 589
#define SERVER 590
#define SESSION 591
#define SESSION_USER 592
#define SET 593
#define SETS 594
#define SETOF 595
#define SHARE 596
#define SHOW 597
#define SIMILAR 598
#define SIMPLE 599
#define SKIP 600
#define SMALLINT 601
#define SNAPSHOT 602
#define SOME 603
#define SQL_P 604
#define STABLE 605
#define STANDALONE_P 606
#define START 607
#define STATEMENT 608
#define STATISTICS 609
#define STDIN 610
#define STDOUT 611
#define STORAGE 612
#define STRICT_P 613
#define STRIP_P 614
#define SUBSTRING 615
#define SYMMETRIC 616
#define SYSID 617
#define SYSTEM_P 618
#define TABLE 619
#define TABLES 620
#define TABLESAMPLE 621
#define TABLESPACE 622
#define TEMP 623
#define TEMPLATE 624
#define TEMPORARY 625
#define TEXT_P 626
#define THEN 627
#define TIME 628
#define TIMESTAMP 629
#define TO 630
#define TRAILING 631
#define TRANSACTION 632
#define TRANSFORM 633
#define TREAT 634
#define TRIGGER 635
#define TRIM 636
#define TRUE_P 637
#define TRUNCATE 638
#define TRUSTED 639
#define TYPE_P 640
#define TYPES_P 641
#define UNBOUNDED 642
#define UNCOMMITTED 643
#define UNENCRYPTED 644
#define UNION 645
#define UNIQUE 646
#define UNKNOWN 647
#define UNLISTEN 648
#define UNLOGGED 649
#define UNTIL 650
#define UPDATE 651
#define USER 652
#define USING 653
#define VACUUM 654
#define VALID 655
#define VALIDATE 656
#define VALIDATOR 657
#define VALUE_P 658
#define VALUES 659
#define VARCHAR 660
#define VARIADIC 661
#define VARYING 662
#define VERBOSE 663
#define VERSION_P 664
#define VIEW 665
#define VIEWS 666
#define VOLATILE 667
#define WHEN 668
#define WHERE 669
#define WHITESPACE_P 670
#define WINDOW 671
#define WITH 672
#define WITHIN 673
#define WITHOUT 674
#define WORK 675
#define WRAPPER 676
#define WRITE 677
#define XML_P 678
#define XMLATTRIBUTES 679
#define XMLCONCAT 680
#define XMLELEMENT 681
#define XMLEXISTS 682
#define XMLFOREST 683
#define XMLPARSE 684
#define XMLPI 685
#define XMLROOT 686
#define XMLSERIALIZE 687
#define YEAR_P 688
#define YES_P 689
#define ZONE 690
#define NOT_LA 691
#define NULLS_LA 692
#define WITH_LA 693
#define POSTFIXOP 694
#define UMINUS 695




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 194 "gram.y"
{
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
}
/* Line 1489 of yacc.c.  */
#line 965 "src_backend_parser_gram.h"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
} YYLTYPE;
# define yyltype YYLTYPE /* obsolescent; will be withdrawn */
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif


