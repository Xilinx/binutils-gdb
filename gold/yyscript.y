/* yyscript.y -- linker script grammer for gold.  */

/* Copyright 2006, 2007, 2008 Free Software Foundation, Inc.
   Written by Ian Lance Taylor <iant@google.com>.

   This file is part of gold.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston,
   MA 02110-1301, USA.  */

/* This is a bison grammar to parse a subset of the original GNU ld
   linker script language.  */

%{

#include "config.h"

#include <stddef.h>
#include <stdint.h>

#include "script-c.h"

%}

/* We need to use a pure parser because we might be multi-threaded.
   We pass some arguments through the parser to the lexer.  */

%pure-parser

%parse-param {void* closure}
%lex-param {void* closure}

/* Since we require bison anyhow, we take advantage of it.  */

%error-verbose

/* The values associated with tokens.  */

%union {
  /* A string.  */
  struct Parser_string string;
  /* A number.  */
  uint64_t integer;
  /* An expression.  */
  Expression_ptr expr;
  // Used for version scripts and within VERSION {}
  struct Version_dependency_list* deplist;
  struct Version_expression_list* versyms;
  struct Version_tree* versnode;
}

/* Operators, including a precedence table for expressions.  */

%right PLUSEQ MINUSEQ MULTEQ DIVEQ '=' LSHIFTEQ RSHIFTEQ ANDEQ OREQ
%right '?' ':'
%left OROR
%left ANDAND
%left '|'
%left '^'
%left '&'
%left EQ NE
%left '<' '>' LE GE
%left LSHIFT RSHIFT
%left '+' '-'
%left '*' '/' '%'

/* A fake operator used to indicate unary operator precedence.  */
%right UNARY

/* Constants.  */

%token <string> STRING
%token <string> QUOTED_STRING
%token <integer> INTEGER

/* Keywords.  This list is taken from ldgram.y and ldlex.l in the old
   GNU linker, with the keywords which only appear in MRI mode
   removed.  Not all these keywords are actually used in this grammar.
   In most cases the keyword is recognized as the token name in upper
   case.  The comments indicate where this is not the case.  */

%token ABSOLUTE
%token ADDR
%token ALIGN_K		/* ALIGN */
%token ALIGNOF
%token ASSERT_K		/* ASSERT */
%token AS_NEEDED
%token AT
%token BIND
%token BLOCK
%token BYTE
%token CONSTANT
%token CONSTRUCTORS
%token COPY
%token CREATE_OBJECT_SYMBOLS
%token DATA_SEGMENT_ALIGN
%token DATA_SEGMENT_END
%token DATA_SEGMENT_RELRO_END
%token DEFINED
%token DSECT
%token ENTRY
%token EXCLUDE_FILE
%token EXTERN
%token FILL
%token FLOAT
%token FORCE_COMMON_ALLOCATION
%token GLOBAL		/* global */
%token GROUP
%token HLL
%token INCLUDE
%token INFO
%token INHIBIT_COMMON_ALLOCATION
%token INPUT
%token KEEP
%token LENGTH		/* LENGTH, l, len */
%token LOADADDR
%token LOCAL		/* local */
%token LONG
%token MAP
%token MAX_K		/* MAX */
%token MEMORY
%token MIN_K		/* MIN */
%token NEXT
%token NOCROSSREFS
%token NOFLOAT
%token NOLOAD
%token ONLY_IF_RO
%token ONLY_IF_RW
%token ORIGIN		/* ORIGIN, o, org */
%token OUTPUT
%token OUTPUT_ARCH
%token OUTPUT_FORMAT
%token OVERLAY
%token PHDRS
%token PROVIDE
%token PROVIDE_HIDDEN
%token QUAD
%token SEARCH_DIR
%token SECTIONS
%token SEGMENT_START
%token SHORT
%token SIZEOF
%token SIZEOF_HEADERS	/* SIZEOF_HEADERS, sizeof_headers */
%token SORT_BY_ALIGNMENT
%token SORT_BY_NAME
%token SPECIAL
%token SQUAD
%token STARTUP
%token SUBALIGN
%token SYSLIB
%token TARGET_K		/* TARGET */
%token TRUNCATE
%token VERSIONK		/* VERSION */

/* Keywords, part 2.  These are keywords that are unique to gold,
   and not present in the old GNU linker.  As before, unless the
   comments say otherwise, the keyword is recognized as the token
   name in upper case. */

%token OPTION

/* Special tokens used to tell the grammar what type of tokens we are
   parsing.  The token stream always begins with one of these tokens.
   We do this because version scripts can appear embedded within
   linker scripts, and because --defsym uses the expression
   parser.  */
%token PARSING_LINKER_SCRIPT
%token PARSING_VERSION_SCRIPT
%token PARSING_DEFSYM

/* Non-terminal types, where needed.  */

%type <expr> parse_exp exp
%type <versyms> vers_defns
%type <versnode> vers_tag
%type <deplist> verdep
%type <string> string

%%

/* Read the special token to see what to read next.  */
top:
	  PARSING_LINKER_SCRIPT linker_script
	| PARSING_VERSION_SCRIPT version_script
	| PARSING_DEFSYM defsym_expr
	;

/* A file contains a list of commands.  */
linker_script:
	  linker_script file_cmd
	| /* empty */
	;

/* A command which may appear at top level of a linker script.  */
file_cmd:
	  GROUP
	    { script_start_group(closure); }
	  '(' input_list ')'
	    { script_end_group(closure); }
        | OPTION '(' string ')'
	    { script_parse_option(closure, $3.value, $3.length); }
        | VERSIONK '{'
            { script_push_lex_into_version_mode(closure); }
          version_script '}'
            { script_pop_lex_mode(closure); }
	| file_or_sections_cmd
	| ignore_cmd
	;

/* Top level commands which we ignore.  The GNU linker uses these to
   select the output format, but we don't offer a choice.  Ignoring
   these is more-or-less OK since most scripts simply explicitly
   choose the default.  */
ignore_cmd:
	  OUTPUT_FORMAT '(' string ')'
	| OUTPUT_FORMAT '(' string ',' string ',' string ')'
	| OUTPUT_ARCH '(' string ')'
	;

/* A list of input file names.  */
input_list:
	  input_list_element
	| input_list opt_comma input_list_element
	;

/* An input file name.  */
input_list_element:
	  string
	    { script_add_file(closure, $1.value, $1.length); }
	| AS_NEEDED
	    { script_start_as_needed(closure); }
	  '(' input_list ')'
	    { script_end_as_needed(closure); }
	;

/* A command which may appear at the top level of a linker script, or
   within a SECTIONS block.  */
file_or_sections_cmd:
	  ENTRY '(' string ')'
	    { script_set_entry(closure, $3.value, $3.length); }
	| assignment end
	;

/* Set a symbol to a value.  */
assignment:
	  string '=' parse_exp
	    { script_set_symbol(closure, $1.value, $1.length, $3, 0, 0); }
	| string PLUSEQ parse_exp
	    {
	      Expression_ptr s = script_exp_string($1.value, $1.length);
	      Expression_ptr e = script_exp_binary_add(s, $3);
	      script_set_symbol(closure, $1.value, $1.length, e, 0, 0);
	    }
	| string MINUSEQ parse_exp
	    {
	      Expression_ptr s = script_exp_string($1.value, $1.length);
	      Expression_ptr e = script_exp_binary_sub(s, $3);
	      script_set_symbol(closure, $1.value, $1.length, e, 0, 0);
	    }
	| string MULTEQ parse_exp
	    {
	      Expression_ptr s = script_exp_string($1.value, $1.length);
	      Expression_ptr e = script_exp_binary_mult(s, $3);
	      script_set_symbol(closure, $1.value, $1.length, e, 0, 0);
	    }
	| string DIVEQ parse_exp
	    {
	      Expression_ptr s = script_exp_string($1.value, $1.length);
	      Expression_ptr e = script_exp_binary_div(s, $3);
	      script_set_symbol(closure, $1.value, $1.length, e, 0, 0);
	    }
	| string LSHIFTEQ parse_exp
	    {
	      Expression_ptr s = script_exp_string($1.value, $1.length);
	      Expression_ptr e = script_exp_binary_lshift(s, $3);
	      script_set_symbol(closure, $1.value, $1.length, e, 0, 0);
	    }
	| string RSHIFTEQ parse_exp
	    {
	      Expression_ptr s = script_exp_string($1.value, $1.length);
	      Expression_ptr e = script_exp_binary_rshift(s, $3);
	      script_set_symbol(closure, $1.value, $1.length, e, 0, 0);
	    }
	| string ANDEQ parse_exp
	    {
	      Expression_ptr s = script_exp_string($1.value, $1.length);
	      Expression_ptr e = script_exp_binary_bitwise_and(s, $3);
	      script_set_symbol(closure, $1.value, $1.length, e, 0, 0);
	    }
	| string OREQ parse_exp
	    {
	      Expression_ptr s = script_exp_string($1.value, $1.length);
	      Expression_ptr e = script_exp_binary_bitwise_or(s, $3);
	      script_set_symbol(closure, $1.value, $1.length, e, 0, 0);
	    }
	| PROVIDE '(' string '=' parse_exp ')'
	    { script_set_symbol(closure, $3.value, $3.length, $5, 1, 0); }
	| PROVIDE_HIDDEN '(' string '=' parse_exp ')'
	    { script_set_symbol(closure, $3.value, $3.length, $5, 1, 1); }
	;

/* Parse an expression, putting the lexer into the right mode.  */
parse_exp:
	    { script_push_lex_into_expression_mode(closure); }
	  exp
	    {
	      script_pop_lex_mode(closure);
	      $$ = $2;
	    }
	;

/* An expression.  */
exp:
	  '(' exp ')'
	    { $$ = $2; }
	| '-' exp %prec UNARY
	    { $$ = script_exp_unary_minus($2); }
	| '!' exp %prec UNARY
	    { $$ = script_exp_unary_logical_not($2); }
	| '~' exp %prec UNARY
	    { $$ = script_exp_unary_bitwise_not($2); }
	| '+' exp %prec UNARY
	    { $$ = $2; }
	| exp '*' exp
	    { $$ = script_exp_binary_mult($1, $3); }
	| exp '/' exp
	    { $$ = script_exp_binary_div($1, $3); }
	| exp '%' exp
	    { $$ = script_exp_binary_mod($1, $3); }
	| exp '+' exp
	    { $$ = script_exp_binary_add($1, $3); }
	| exp '-' exp
	    { $$ = script_exp_binary_sub($1, $3); }
	| exp LSHIFT exp
	    { $$ = script_exp_binary_lshift($1, $3); }
	| exp RSHIFT exp
	    { $$ = script_exp_binary_rshift($1, $3); }
	| exp EQ exp
	    { $$ = script_exp_binary_eq($1, $3); }
	| exp NE exp
	    { $$ = script_exp_binary_ne($1, $3); }
	| exp LE exp
	    { $$ = script_exp_binary_le($1, $3); }
	| exp GE exp
	    { $$ = script_exp_binary_ge($1, $3); }
	| exp '<' exp
	    { $$ = script_exp_binary_lt($1, $3); }
	| exp '>' exp
	    { $$ = script_exp_binary_gt($1, $3); }
	| exp '&' exp
	    { $$ = script_exp_binary_bitwise_and($1, $3); }
	| exp '^' exp
	    { $$ = script_exp_binary_bitwise_xor($1, $3); }
	| exp '|' exp
	    { $$ = script_exp_binary_bitwise_or($1, $3); }
	| exp ANDAND exp
	    { $$ = script_exp_binary_logical_and($1, $3); }
	| exp OROR exp
	    { $$ = script_exp_binary_logical_or($1, $3); }
	| exp '?' exp ':' exp
	    { $$ = script_exp_trinary_cond($1, $3, $5); }
	| INTEGER
	    { $$ = script_exp_integer($1); }
	| STRING
	    { $$ = script_exp_string($1.value, $1.length); }
	| QUOTED_STRING
	    { $$ = script_exp_string($1.value, $1.length); }
	| MAX_K '(' exp ',' exp ')'
	    { $$ = script_exp_function_max($3, $5); }
	| MIN_K '(' exp ',' exp ')'
	    { $$ = script_exp_function_min($3, $5); }
	| DEFINED '(' string ')'
	    { $$ = script_exp_function_defined($3.value, $3.length); }
	| SIZEOF_HEADERS
	    { $$ = script_exp_function_sizeof_headers(); }
	| ALIGNOF '(' string ')'
	    { $$ = script_exp_function_alignof($3.value, $3.length); }
	| SIZEOF '(' string ')'
	    { $$ = script_exp_function_sizeof($3.value, $3.length); }
	| ADDR '(' string ')'
	    { $$ = script_exp_function_addr($3.value, $3.length); }
	| LOADADDR '(' string ')'
	    { $$ = script_exp_function_loadaddr($3.value, $3.length); }
	| ORIGIN '(' string ')'
	    { $$ = script_exp_function_origin($3.value, $3.length); }
	| LENGTH '(' string ')'
	    { $$ = script_exp_function_length($3.value, $3.length); }
	| CONSTANT '(' string ')'
	    { $$ = script_exp_function_constant($3.value, $3.length); }
	| ABSOLUTE '(' exp ')'
	    { $$ = script_exp_function_absolute($3); }
	| ALIGN_K '(' exp ')'
	    { $$ = script_exp_function_align(script_exp_string(".", 1), $3); }
	| ALIGN_K '(' exp ',' exp ')'
	    { $$ = script_exp_function_align($3, $5); }
	| BLOCK '(' exp ')'
	    { $$ = script_exp_function_align(script_exp_string(".", 1), $3); }
	| DATA_SEGMENT_ALIGN '(' exp ',' exp ')'
	    { $$ = script_exp_function_data_segment_align($3, $5); }
	| DATA_SEGMENT_RELRO_END '(' exp ',' exp ')'
	    { $$ = script_exp_function_data_segment_relro_end($3, $5); }
	| DATA_SEGMENT_END '(' exp ')'
	    { $$ = script_exp_function_data_segment_end($3); }
	| SEGMENT_START '(' string ',' exp ')'
	    {
	      $$ = script_exp_function_segment_start($3.value, $3.length, $5);
	    }
	| ASSERT_K '(' exp ',' string ')'
	    { $$ = script_exp_function_assert($3, $5.value, $5.length); }
	;

/* Handle the --defsym option.  */
defsym_expr:
	  string '=' parse_exp
	    { script_set_symbol(closure, $1.value, $1.length, $3, 0, 0); }
	;

/* A version script.  */
version_script:
	  vers_nodes
	;

vers_nodes:
	  vers_node
	| vers_nodes vers_node
	;

vers_node:
	  '{' vers_tag '}' ';'
	    {
	      script_register_vers_node (closure, NULL, 0, $2, NULL);
	    }
	| string '{' vers_tag '}' ';'
	    {
	      script_register_vers_node (closure, $1.value, $1.length, $3,
					 NULL);
	    }
	| string '{' vers_tag '}' verdep ';'
	    {
	      script_register_vers_node (closure, $1.value, $1.length, $3, $5);
	    }
	;

verdep:
	  string
	    {
	      $$ = script_add_vers_depend (closure, NULL, $1.value, $1.length);
	    }
	| verdep string
	    {
	      $$ = script_add_vers_depend (closure, $1, $2.value, $2.length);
	    }
	;

vers_tag:
	  /* empty */
	    { $$ = script_new_vers_node (closure, NULL, NULL); }
	| vers_defns ';'
	    { $$ = script_new_vers_node (closure, $1, NULL); }
	| GLOBAL ':' vers_defns ';'
	    { $$ = script_new_vers_node (closure, $3, NULL); }
	| LOCAL ':' vers_defns ';'
	    { $$ = script_new_vers_node (closure, NULL, $3); }
	| GLOBAL ':' vers_defns ';' LOCAL ':' vers_defns ';'
	    { $$ = script_new_vers_node (closure, $3, $7); }
	;

/* Here is one of the rare places we care about the distinction
   between STRING and QUOTED_STRING.  For QUOTED_STRING, we do exact
   matching on the pattern, so we pass in true for the exact_match
   parameter.  For STRING, we do glob matching and pass in false.  */
vers_defns:
	  STRING
	    {
	      $$ = script_new_vers_pattern (closure, NULL, $1.value,
					    $1.length, 0);
	    }
	| QUOTED_STRING
	    {
	      $$ = script_new_vers_pattern (closure, NULL, $1.value,
					    $1.length, 1);
	    }
	| vers_defns ';' STRING
	    {
	      $$ = script_new_vers_pattern (closure, $1, $3.value,
                                            $3.length, 0);
	    }
	| vers_defns ';' QUOTED_STRING
	    {
	      $$ = script_new_vers_pattern (closure, $1, $3.value,
                                            $3.length, 1);
	    }
        | /* Push string on the language stack. */
          EXTERN string '{'
	    { version_script_push_lang (closure, $2.value, $2.length); }
	  vers_defns opt_semicolon '}'
	    {
	      $$ = $5;
	      version_script_pop_lang(closure);
	    }
        | /* Push string on the language stack.  This is more complicated
             than the other cases because we need to merge the linked-list
             state from the pre-EXTERN defns and the post-EXTERN defns.  */
          vers_defns ';' EXTERN string '{'
	    { version_script_push_lang (closure, $4.value, $4.length); }
	  vers_defns opt_semicolon '}'
	    {
	      $$ = script_merge_expressions ($1, $7);
	      version_script_pop_lang(closure);
	    }
        | EXTERN  // "extern" as a symbol name
	    {
	      $$ = script_new_vers_pattern (closure, NULL, "extern",
					    sizeof("extern") - 1, 1);
	    }
	| vers_defns ';' EXTERN
	    {
	      $$ = script_new_vers_pattern (closure, $1, "extern",
					    sizeof("extern") - 1, 1);
	    }
	;

/* A string can be either a STRING or a QUOTED_STRING.  Almost all the
   time we don't care, and we use this rule.  */
string:
          STRING
	    { $$ = $1; }
	| QUOTED_STRING
	    { $$ = $1; }
	;

/* Some statements require a terminator, which may be a semicolon or a
   comma.  */
end:
	  ';'
	| ','
	;

/* An optional semicolon.  */
opt_semicolon:
	  ';'
	|  /* empty */
	;

/* An optional comma.  */
opt_comma:
	  ','
	| /* empty */
	;

%%
