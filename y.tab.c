/* A Bison parser, made by GNU Bison 2.7.  */

/* Skeleton implementation for Bison LALR(1) parsers in C++
   
      Copyright (C) 2002-2012 Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

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


/* First part of user declarations.  */
/* Line 279 of lalr1.cc  */
#line 28 "parse.yy"

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <list>
#include <map>

using namespace std;

#include "common-classes.hh"
#include "evaluate.hh"
#include "support.hh"
#include "cfglp-ctx.hh"

#include "reg-alloc.hh"
#include "symtab.hh"
#include "ast.hh"
#include "icode.hh"
#include "program.hh"

#include "options.hh"



/* Line 279 of lalr1.cc  */
#line 64 "y.tab.c"


#include "y.tab.h"

/* User implementation prologue.  */
/* Line 285 of lalr1.cc  */
#line 80 "parse.yy"

  extern int yylex(yy::cfglp::semantic_type *yylval,
       yy::cfglp::location_type* yylloc,
       cfglp_ctx &ctx);

/* Some auxiliary local functions */

  static program_Ptr build_Program(proc_Ptr proc_P, sym_List_Ptr gsym_lp);
  static proc_Ptr build_Procedure (string name, int bb_num, ast_List_Ptr ast_lp,  
                sym_List_Ptr sym_lp, sym_List_Ptr sym_gp);
  static sym_Entry_Ptr redeclaration_Error(string name);
  static ast_Ptr missing_Declaration_Error(bool lhs_d, string lhs_n, bool rhs_d, string rhs_n, int line);
  static ast_Ptr process_Asgn_Name_Name(string lhs, string rhs, int line);
  static ast_Ptr process_Asgn_Name_Num(string lhs, int rhs, int line);

  extern   int yylineno;
/* 
   Currently our program structure supports global declarations
   and a single procedure without any return type (we need to 
   check that it is the main procedure). We also do not support
   control flow or function calls and our RHS of assignments are 
   simple variables or number and have type INT.

   The complete grammar that we currently support is:

   program: decl_Stmt_List procedure
    |  procedure 
    ;
   procedure: ID '(' ')' '{' decl_Stmt_List bb_Decl exe_Stmt_List '}'
    ;
   decl_Stmt_List: decl_Stmt decl_Stmt_List
    |  decl_Stmt
    ;
   exe_Stmt_List: exe_Stmt_List exe_Stmt 
    |  exe_Stmt 
    ;
   decl_Stmt: static_kw INT ID ';'
    ;
   static_kw: 
    |  STATIC
    ;
   exe_Stmt : ID '=' ID ';'
    |  ID '=' NUM ';'
    |  RETURN ';' 
    ;
   bb_Decl: '<' ID NUM '>' ':'
    ;

   Observe that with a left recursive grammar declaration processing requires 
   inherited attributes because we need to compare current declarations with
   past declarations. 

   Building AST without type checking does not require such a check. Hence it
   can use synthesized attributes. However, type checking during AST building
   requires inherited attributes.

   Synthesized attributes are supported well by bison using the $ notation.
   Inherited attributes limited to a grammar rules can be implemented by
   using $i where i is the position of a grammar symbol in RHS but on the 
   left of current symbol.

   Since bison does not support L-attributed definitions, we do not have a 
   direct mechanism of implementing inherited attributes across grammar 
   rules. Hence we store sym_list in symtab_in_current_scope_P extract it 
   wherever required.

*/

/* Line 285 of lalr1.cc  */
#line 141 "y.tab.c"


# ifndef YY_NULL
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULL nullptr
#  else
#   define YY_NULL 0
#  endif
# endif

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* FIXME: INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

# ifndef YYLLOC_DEFAULT
#  define YYLLOC_DEFAULT(Current, Rhs, N)                               \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).begin  = YYRHSLOC (Rhs, 1).begin;                   \
          (Current).end    = YYRHSLOC (Rhs, N).end;                     \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).begin = (Current).end = YYRHSLOC (Rhs, 0).end;      \
        }                                                               \
    while (/*CONSTCOND*/ false)
# endif


/* Suppress unused-variable warnings by "using" E.  */
#define YYUSE(e) ((void) (e))

/* Enable debugging if requested.  */
#if YYDEBUG

/* A pseudo ostream that takes yydebug_ into account.  */
# define YYCDEBUG if (yydebug_) (*yycdebug_)

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)	\
do {							\
  if (yydebug_)						\
    {							\
      *yycdebug_ << Title << ' ';			\
      yy_symbol_print_ ((Type), (Value), (Location));	\
      *yycdebug_ << std::endl;				\
    }							\
} while (false)

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug_)				\
    yy_reduce_print_ (Rule);		\
} while (false)

# define YY_STACK_PRINT()		\
do {					\
  if (yydebug_)				\
    yystack_print_ ();			\
} while (false)

#else /* !YYDEBUG */

# define YYCDEBUG if (false) std::cerr
# define YY_SYMBOL_PRINT(Title, Type, Value, Location) YYUSE(Type)
# define YY_REDUCE_PRINT(Rule)        static_cast<void>(0)
# define YY_STACK_PRINT()             static_cast<void>(0)

#endif /* !YYDEBUG */

#define yyerrok		(yyerrstatus_ = 0)
#define yyclearin	(yychar = yyempty_)

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab
#define YYRECOVERING()  (!!yyerrstatus_)


namespace yy {
/* Line 353 of lalr1.cc  */
#line 236 "y.tab.c"

  /// Build a parser object.
  cfglp::cfglp (cfglp_ctx &ctx_yyarg)
    :
#if YYDEBUG
      yydebug_ (false),
      yycdebug_ (&std::cerr),
#endif
      ctx (ctx_yyarg)
  {
  }

  cfglp::~cfglp ()
  {
  }

#if YYDEBUG
  /*--------------------------------.
  | Print this symbol on YYOUTPUT.  |
  `--------------------------------*/

  inline void
  cfglp::yy_symbol_value_print_ (int yytype,
			   const semantic_type* yyvaluep, const location_type* yylocationp)
  {
    YYUSE (yylocationp);
    YYUSE (yyvaluep);
    std::ostream& yyo = debug_stream ();
    std::ostream& yyoutput = yyo;
    YYUSE (yyoutput);
    switch (yytype)
      {
         default:
	  break;
      }
  }


  void
  cfglp::yy_symbol_print_ (int yytype,
			   const semantic_type* yyvaluep, const location_type* yylocationp)
  {
    *yycdebug_ << (yytype < yyntokens_ ? "token" : "nterm")
	       << ' ' << yytname_[yytype] << " ("
	       << *yylocationp << ": ";
    yy_symbol_value_print_ (yytype, yyvaluep, yylocationp);
    *yycdebug_ << ')';
  }
#endif

  void
  cfglp::yydestruct_ (const char* yymsg,
			   int yytype, semantic_type* yyvaluep, location_type* yylocationp)
  {
    YYUSE (yylocationp);
    YYUSE (yymsg);
    YYUSE (yyvaluep);

    if (yymsg)
      YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

    switch (yytype)
      {
  
	default:
	  break;
      }
  }

  void
  cfglp::yypop_ (unsigned int n)
  {
    yystate_stack_.pop (n);
    yysemantic_stack_.pop (n);
    yylocation_stack_.pop (n);
  }

#if YYDEBUG
  std::ostream&
  cfglp::debug_stream () const
  {
    return *yycdebug_;
  }

  void
  cfglp::set_debug_stream (std::ostream& o)
  {
    yycdebug_ = &o;
  }


  cfglp::debug_level_type
  cfglp::debug_level () const
  {
    return yydebug_;
  }

  void
  cfglp::set_debug_level (debug_level_type l)
  {
    yydebug_ = l;
  }
#endif

  inline bool
  cfglp::yy_pact_value_is_default_ (int yyvalue)
  {
    return yyvalue == yypact_ninf_;
  }

  inline bool
  cfglp::yy_table_value_is_error_ (int yyvalue)
  {
    return yyvalue == yytable_ninf_;
  }

  int
  cfglp::parse ()
  {
    /// Lookahead and lookahead in internal form.
    int yychar = yyempty_;
    int yytoken = 0;

    // State.
    int yyn;
    int yylen = 0;
    int yystate = 0;

    // Error handling.
    int yynerrs_ = 0;
    int yyerrstatus_ = 0;

    /// Semantic value of the lookahead.
    static semantic_type yyval_default;
    semantic_type yylval = yyval_default;
    /// Location of the lookahead.
    location_type yylloc;
    /// The locations where the error started and ended.
    location_type yyerror_range[3];

    /// $$.
    semantic_type yyval;
    /// @$.
    location_type yyloc;

    int yyresult;

    // FIXME: This shoud be completely indented.  It is not yet to
    // avoid gratuitous conflicts when merging into the master branch.
    try
      {
    YYCDEBUG << "Starting parse" << std::endl;


/* User initialization code.  */
/* Line 549 of lalr1.cc  */
#line 149 "parse.yy"
{
 // Filename for locations here
 yylloc.begin.filename = yylloc.end.filename = new std::string("stdin");
}
/* Line 549 of lalr1.cc  */
#line 399 "y.tab.c"

    /* Initialize the stacks.  The initial state will be pushed in
       yynewstate, since the latter expects the semantical and the
       location values to have been already stored, initialize these
       stacks with a primary value.  */
    yystate_stack_ = state_stack_type (0);
    yysemantic_stack_ = semantic_stack_type (0);
    yylocation_stack_ = location_stack_type (0);
    yysemantic_stack_.push (yylval);
    yylocation_stack_.push (yylloc);

    /* New state.  */
  yynewstate:
    yystate_stack_.push (yystate);
    YYCDEBUG << "Entering state " << yystate << std::endl;

    /* Accept?  */
    if (yystate == yyfinal_)
      goto yyacceptlab;

    goto yybackup;

    /* Backup.  */
  yybackup:

    /* Try to take a decision without lookahead.  */
    yyn = yypact_[yystate];
    if (yy_pact_value_is_default_ (yyn))
      goto yydefault;

    /* Read a lookahead token.  */
    if (yychar == yyempty_)
      {
        YYCDEBUG << "Reading a token: ";
        yychar = yylex (&yylval, &yylloc, ctx);
      }

    /* Convert token to internal form.  */
    if (yychar <= yyeof_)
      {
	yychar = yytoken = yyeof_;
	YYCDEBUG << "Now at end of input." << std::endl;
      }
    else
      {
	yytoken = yytranslate_ (yychar);
	YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
      }

    /* If the proper action on seeing token YYTOKEN is to reduce or to
       detect an error, take that action.  */
    yyn += yytoken;
    if (yyn < 0 || yylast_ < yyn || yycheck_[yyn] != yytoken)
      goto yydefault;

    /* Reduce or error.  */
    yyn = yytable_[yyn];
    if (yyn <= 0)
      {
	if (yy_table_value_is_error_ (yyn))
	  goto yyerrlab;
	yyn = -yyn;
	goto yyreduce;
      }

    /* Shift the lookahead token.  */
    YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

    /* Discard the token being shifted.  */
    yychar = yyempty_;

    yysemantic_stack_.push (yylval);
    yylocation_stack_.push (yylloc);

    /* Count tokens shifted since error; after three, turn off error
       status.  */
    if (yyerrstatus_)
      --yyerrstatus_;

    yystate = yyn;
    goto yynewstate;

  /*-----------------------------------------------------------.
  | yydefault -- do the default action for the current state.  |
  `-----------------------------------------------------------*/
  yydefault:
    yyn = yydefact_[yystate];
    if (yyn == 0)
      goto yyerrlab;
    goto yyreduce;

  /*-----------------------------.
  | yyreduce -- Do a reduction.  |
  `-----------------------------*/
  yyreduce:
    yylen = yyr2_[yyn];
    /* If YYLEN is nonzero, implement the default value of the action:
       `$$ = $1'.  Otherwise, use the top of the stack.

       Otherwise, the following line sets YYVAL to garbage.
       This behavior is undocumented and Bison
       users should not rely upon it.  */
    if (yylen)
      yyval = yysemantic_stack_[yylen - 1];
    else
      yyval = yysemantic_stack_[0];

    // Compute the default @$.
    {
      slice<location_type, location_stack_type> slice (yylocation_stack_, yylen);
      YYLLOC_DEFAULT (yyloc, slice, yylen);
    }

    // Perform the reduction.
    YY_REDUCE_PRINT (yyn);
    switch (yyn)
      {
          case 2:
/* Line 670 of lalr1.cc  */
#line 156 "parse.yy"
    {
        symtab_in_scope_P = new symtab_In_Scope;
        symtab_in_scope_P->allocate_Sym_List(); 
    }
    break;

  case 3:
/* Line 670 of lalr1.cc  */
#line 161 "parse.yy"
    {
        program_object_P = (yysemantic_stack_[(2) - (2)].prog_P);
        symtab_in_scope_P->deallocate_Sym_List(); 
    }
    break;

  case 4:
/* Line 670 of lalr1.cc  */
#line 169 "parse.yy"
    {
        sym_List_Ptr sym_lp = symtab_in_scope_P->get_Symtab_Top_List(); 
        (yyval.prog_P)=build_Program((yysemantic_stack_[(2) - (2)].proc_P), sym_lp);
    }
    break;

  case 5:
/* Line 670 of lalr1.cc  */
#line 174 "parse.yy"
    {
        (yyval.prog_P)=build_Program((yysemantic_stack_[(1) - (1)].proc_P), NULL);
    }
    break;

  case 6:
/* Line 670 of lalr1.cc  */
#line 181 "parse.yy"
    {
        symtab_in_scope_P->enter_Procedure_Name(*(yysemantic_stack_[(1) - (1)].sval), yylineno);
        symtab_in_scope_P->allocate_Sym_List(); 
    }
    break;

  case 7:
/* Line 670 of lalr1.cc  */
#line 186 "parse.yy"
    {
        string name = *(yysemantic_stack_[(9) - (1)].sval);
        sym_List_Ptr sym_lp = symtab_in_scope_P->get_Symtab_Local_List(); 
        sym_List_Ptr sym_gp = symtab_in_scope_P->get_Symtab_Global_List(); 
        int bb_num = (yysemantic_stack_[(9) - (7)].ival);
        ast_List_Ptr ast_lp = (yysemantic_stack_[(9) - (8)].ast_L);
        (yyval.proc_P) = build_Procedure (name, bb_num, ast_lp, sym_lp, sym_gp);
        symtab_in_scope_P->deallocate_Sym_List(); 
    }
    break;

  case 8:
/* Line 670 of lalr1.cc  */
#line 199 "parse.yy"
    {
        if ((yysemantic_stack_[(2) - (1)].ast_L) && (yysemantic_stack_[(2) - (2)].ast_P))
        {
            ast_List_Ptr ast_lp = (yysemantic_stack_[(2) - (1)].ast_L);
            ast_lp->push_back((yysemantic_stack_[(2) - (2)].ast_P));
            (yyval.ast_L) = ast_lp;
        }
        else if ((yysemantic_stack_[(2) - (1)].ast_L) && !(yysemantic_stack_[(2) - (2)].ast_P))
        {
            (yyval.ast_L) = (yysemantic_stack_[(2) - (1)].ast_L);
        }
        else if (!(yysemantic_stack_[(2) - (1)].ast_L) && (yysemantic_stack_[(2) - (2)].ast_P))
        {   
            ast_List_Ptr ast_lp = new list<ast_Ptr>;
            ast_lp->push_back((yysemantic_stack_[(2) - (2)].ast_P));    
            (yyval.ast_L) = ast_lp;
        }
        else (yyval.ast_L) = NULL;
    }
    break;

  case 9:
/* Line 670 of lalr1.cc  */
#line 220 "parse.yy"
    {
        if ((yysemantic_stack_[(1) - (1)].ast_P))
        { 
            ast_List_Ptr ast_lp = new list<ast_Ptr>;
            ast_lp->push_back((yysemantic_stack_[(1) - (1)].ast_P));    
            (yyval.ast_L) = ast_lp;
        }
        else (yyval.ast_L) = NULL;
    }
    break;

  case 10:
/* Line 670 of lalr1.cc  */
#line 233 "parse.yy"
    {
        sym_List_Ptr sym_lp = symtab_in_scope_P->get_Symtab_Top_List(); 
        if ((yysemantic_stack_[(2) - (2)].sym_P))
            sym_lp->insert_Sym_Entry((yysemantic_stack_[(2) - (2)].sym_P));
    }
    break;

  case 11:
/* Line 670 of lalr1.cc  */
#line 239 "parse.yy"
    {
        sym_List_Ptr sym_lp = symtab_in_scope_P->get_Symtab_Top_List(); 
        if ((yysemantic_stack_[(1) - (1)].sym_P))
            sym_lp->insert_Sym_Entry((yysemantic_stack_[(1) - (1)].sym_P));
    }
    break;

  case 12:
/* Line 670 of lalr1.cc  */
#line 248 "parse.yy"
    {
        string name = *(yysemantic_stack_[(4) - (3)].sval);
        if (symtab_in_scope_P->declared_In_Visible_Scope(name, symtab_Top) == false)
        {
            sym_Entry_Ptr s = new sym_Entry_for_Int_Var(name, yylineno);
            (yyval.sym_P) = s;
        }
        else
            (yyval.sym_P) = redeclaration_Error(name);
    }
    break;

  case 15:
/* Line 670 of lalr1.cc  */
#line 266 "parse.yy"
    {
        CHECK_INVARIANT ((*(yysemantic_stack_[(5) - (2)].sval)) == "bb", "Basic block is expected to be identified by the lexeme bb")
        (yyval.ival) = (yysemantic_stack_[(5) - (3)].ival);
    }
    break;

  case 16:
/* Line 670 of lalr1.cc  */
#line 274 "parse.yy"
    {
        (yyval.ast_P) = process_Asgn_Name_Name(*(yysemantic_stack_[(4) - (1)].sval), *(yysemantic_stack_[(4) - (3)].sval), yylineno);
    }
    break;

  case 17:
/* Line 670 of lalr1.cc  */
#line 278 "parse.yy"
    {
        ast_Ptr a = process_Asgn_Name_Num(*(yysemantic_stack_[(4) - (1)].sval), (yysemantic_stack_[(4) - (3)].ival), yylineno);
        (yyval.ast_P) = a;
    }
    break;

  case 18:
/* Line 670 of lalr1.cc  */
#line 283 "parse.yy"
    {
        (yyval.ast_P) = new ret_Ast();
    }
    break;


/* Line 670 of lalr1.cc  */
#line 684 "y.tab.c"
      default:
        break;
      }

    /* User semantic actions sometimes alter yychar, and that requires
       that yytoken be updated with the new translation.  We take the
       approach of translating immediately before every use of yytoken.
       One alternative is translating here after every semantic action,
       but that translation would be missed if the semantic action
       invokes YYABORT, YYACCEPT, or YYERROR immediately after altering
       yychar.  In the case of YYABORT or YYACCEPT, an incorrect
       destructor might then be invoked immediately.  In the case of
       YYERROR, subsequent parser actions might lead to an incorrect
       destructor call or verbose syntax error message before the
       lookahead is translated.  */
    YY_SYMBOL_PRINT ("-> $$ =", yyr1_[yyn], &yyval, &yyloc);

    yypop_ (yylen);
    yylen = 0;
    YY_STACK_PRINT ();

    yysemantic_stack_.push (yyval);
    yylocation_stack_.push (yyloc);

    /* Shift the result of the reduction.  */
    yyn = yyr1_[yyn];
    yystate = yypgoto_[yyn - yyntokens_] + yystate_stack_[0];
    if (0 <= yystate && yystate <= yylast_
	&& yycheck_[yystate] == yystate_stack_[0])
      yystate = yytable_[yystate];
    else
      yystate = yydefgoto_[yyn - yyntokens_];
    goto yynewstate;

  /*------------------------------------.
  | yyerrlab -- here on detecting error |
  `------------------------------------*/
  yyerrlab:
    /* Make sure we have latest lookahead translation.  See comments at
       user semantic actions for why this is necessary.  */
    yytoken = yytranslate_ (yychar);

    /* If not already recovering from an error, report this error.  */
    if (!yyerrstatus_)
      {
	++yynerrs_;
	if (yychar == yyempty_)
	  yytoken = yyempty_;
	error (yylloc, yysyntax_error_ (yystate, yytoken));
      }

    yyerror_range[1] = yylloc;
    if (yyerrstatus_ == 3)
      {
        /* If just tried and failed to reuse lookahead token after an
           error, discard it.  */
        if (yychar <= yyeof_)
          {
            /* Return failure if at end of input.  */
            if (yychar == yyeof_)
              YYABORT;
          }
        else
          {
            yydestruct_ ("Error: discarding", yytoken, &yylval, &yylloc);
            yychar = yyempty_;
          }
      }

    /* Else will try to reuse lookahead token after shifting the error
       token.  */
    goto yyerrlab1;


  /*---------------------------------------------------.
  | yyerrorlab -- error raised explicitly by YYERROR.  |
  `---------------------------------------------------*/
  yyerrorlab:

    /* Pacify compilers like GCC when the user code never invokes
       YYERROR and the label yyerrorlab therefore never appears in user
       code.  */
    if (false)
      goto yyerrorlab;

    yyerror_range[1] = yylocation_stack_[yylen - 1];
    /* Do not reclaim the symbols of the rule which action triggered
       this YYERROR.  */
    yypop_ (yylen);
    yylen = 0;
    yystate = yystate_stack_[0];
    goto yyerrlab1;

  /*-------------------------------------------------------------.
  | yyerrlab1 -- common code for both syntax error and YYERROR.  |
  `-------------------------------------------------------------*/
  yyerrlab1:
    yyerrstatus_ = 3;	/* Each real token shifted decrements this.  */

    for (;;)
      {
	yyn = yypact_[yystate];
	if (!yy_pact_value_is_default_ (yyn))
	{
	  yyn += yyterror_;
	  if (0 <= yyn && yyn <= yylast_ && yycheck_[yyn] == yyterror_)
	    {
	      yyn = yytable_[yyn];
	      if (0 < yyn)
		break;
	    }
	}

	/* Pop the current state because it cannot handle the error token.  */
	if (yystate_stack_.height () == 1)
	  YYABORT;

	yyerror_range[1] = yylocation_stack_[0];
	yydestruct_ ("Error: popping",
		     yystos_[yystate],
		     &yysemantic_stack_[0], &yylocation_stack_[0]);
	yypop_ ();
	yystate = yystate_stack_[0];
	YY_STACK_PRINT ();
      }

    yyerror_range[2] = yylloc;
    // Using YYLLOC is tempting, but would change the location of
    // the lookahead.  YYLOC is available though.
    YYLLOC_DEFAULT (yyloc, yyerror_range, 2);
    yysemantic_stack_.push (yylval);
    yylocation_stack_.push (yyloc);

    /* Shift the error token.  */
    YY_SYMBOL_PRINT ("Shifting", yystos_[yyn],
		     &yysemantic_stack_[0], &yylocation_stack_[0]);

    yystate = yyn;
    goto yynewstate;

    /* Accept.  */
  yyacceptlab:
    yyresult = 0;
    goto yyreturn;

    /* Abort.  */
  yyabortlab:
    yyresult = 1;
    goto yyreturn;

  yyreturn:
    if (yychar != yyempty_)
      {
        /* Make sure we have latest lookahead translation.  See comments
           at user semantic actions for why this is necessary.  */
        yytoken = yytranslate_ (yychar);
        yydestruct_ ("Cleanup: discarding lookahead", yytoken, &yylval,
                     &yylloc);
      }

    /* Do not reclaim the symbols of the rule which action triggered
       this YYABORT or YYACCEPT.  */
    yypop_ (yylen);
    while (1 < yystate_stack_.height ())
      {
        yydestruct_ ("Cleanup: popping",
                     yystos_[yystate_stack_[0]],
                     &yysemantic_stack_[0],
                     &yylocation_stack_[0]);
        yypop_ ();
      }

    return yyresult;
    }
    catch (...)
      {
        YYCDEBUG << "Exception caught: cleaning lookahead and stack"
                 << std::endl;
        // Do not try to display the values of the reclaimed symbols,
        // as their printer might throw an exception.
        if (yychar != yyempty_)
          {
            /* Make sure we have latest lookahead translation.  See
               comments at user semantic actions for why this is
               necessary.  */
            yytoken = yytranslate_ (yychar);
            yydestruct_ (YY_NULL, yytoken, &yylval, &yylloc);
          }

        while (1 < yystate_stack_.height ())
          {
            yydestruct_ (YY_NULL,
                         yystos_[yystate_stack_[0]],
                         &yysemantic_stack_[0],
                         &yylocation_stack_[0]);
            yypop_ ();
          }
        throw;
      }
  }

  // Generate an error message.
  std::string
  cfglp::yysyntax_error_ (int, int)
  {
    return YY_("syntax error");
  }


  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
  const signed char cfglp::yypact_ninf_ = -9;
  const signed char
  cfglp::yypact_[] =
  {
        -9,     2,     0,    -9,    -9,    -9,    -9,    -9,     0,    -9,
       1,     8,    -9,    -9,    10,    11,     5,    12,    -9,    14,
      -2,    15,     9,    20,    13,    16,    -3,    -9,     4,     6,
      -9,    -9,    -9,    17,    18,    19,    -9,    -9,    -9
  };

  /* YYDEFACT[S] -- default reduction number in state S.  Performed when
     YYTABLE doesn't specify something else to do.  Zero means the
     default is an error.  */
  const unsigned char
  cfglp::yydefact_[] =
  {
         2,     0,    13,     1,     6,    14,     3,     5,    13,    11,
       0,     0,     4,    10,     0,     0,     0,     0,    12,    13,
      13,     0,     0,     0,     0,     0,     0,     9,     0,     0,
      18,     7,     8,     0,     0,     0,    15,    17,    16
  };

  /* YYPGOTO[NTERM-NUM].  */
  const signed char
  cfglp::yypgoto_[] =
  {
        -9,    -9,    -9,    -9,    25,    -9,    -9,     7,    -8,    -9,
      -9,    -1
  };

  /* YYDEFGOTO[NTERM-NUM].  */
  const signed char
  cfglp::yydefgoto_[] =
  {
        -1,     1,     2,     6,     7,    11,    26,     8,     9,    10,
      22,    27
  };

  /* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule which
     number is the opposite.  If YYTABLE_NINF_, syntax error.  */
  const signed char cfglp::yytable_ninf_ = -1;
  const unsigned char
  cfglp::yytable_[] =
  {
        13,    24,     3,    25,     4,     5,    14,     5,    31,    34,
      35,    21,    13,    24,    16,    25,    15,    18,    33,    23,
      17,     5,    19,    28,     0,    32,    20,     0,    30,    29,
      37,    38,    36,    12
  };

  /* YYCHECK.  */
  const signed char
  cfglp::yycheck_[] =
  {
         8,     4,     0,     6,     4,     7,     5,     7,    11,     3,
       4,    13,    20,     4,     4,     6,     8,    12,    14,     4,
       9,     7,    10,     3,    -1,    26,    19,    -1,    12,    16,
      12,    12,    15,     8
  };

  /* STOS_[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
  const unsigned char
  cfglp::yystos_[] =
  {
         0,    18,    19,     0,     4,     7,    20,    21,    24,    25,
      26,    22,    21,    25,     5,     8,     4,     9,    12,    10,
      24,    13,    27,     4,     4,     6,    23,    28,     3,    16,
      12,    11,    28,    14,     3,     4,    15,    12,    12
  };

#if YYDEBUG
  /* TOKEN_NUMBER_[YYLEX-NUM] -- Internal symbol number corresponding
     to YYLEX-NUM.  */
  const unsigned short int
  cfglp::yytoken_number_[] =
  {
         0,   256,   257,   258,   259,   260,   261,   262,    40,    41,
     123,   125,    59,    60,    62,    58,    61
  };
#endif

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
  const unsigned char
  cfglp::yyr1_[] =
  {
         0,    17,    19,    18,    20,    20,    22,    21,    23,    23,
      24,    24,    25,    26,    26,    27,    28,    28,    28
  };

  /* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
  const unsigned char
  cfglp::yyr2_[] =
  {
         0,     2,     0,     2,     2,     1,     0,     9,     2,     1,
       2,     1,     4,     0,     1,     5,     4,     4,     2
  };

#if YYDEBUG
  /* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
     First, the terminals, then, starting at \a yyntokens_, nonterminals.  */
  const char*
  const cfglp::yytname_[] =
  {
    "$end", "error", "$undefined", "NUM", "ID", "INT", "RETURN", "STATIC",
  "'('", "')'", "'{'", "'}'", "';'", "'<'", "'>'", "':'", "'='", "$accept",
  "start", "$@1", "program", "procedure", "$@2", "exe_Stmt_List",
  "decl_Stmt_List", "decl_Stmt", "static_kw", "bb_Decl", "exe_Stmt", YY_NULL
  };


  /* YYRHS -- A `-1'-separated list of the rules' RHS.  */
  const cfglp::rhs_number_type
  cfglp::yyrhs_[] =
  {
        18,     0,    -1,    -1,    19,    20,    -1,    24,    21,    -1,
      21,    -1,    -1,     4,    22,     8,     9,    10,    24,    27,
      23,    11,    -1,    23,    28,    -1,    28,    -1,    24,    25,
      -1,    25,    -1,    26,     5,     4,    12,    -1,    -1,     7,
      -1,    13,     4,     3,    14,    15,    -1,     4,    16,     4,
      12,    -1,     4,    16,     3,    12,    -1,     6,    12,    -1
  };

  /* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
     YYRHS.  */
  const unsigned char
  cfglp::yyprhs_[] =
  {
         0,     0,     3,     4,     7,    10,    12,    13,    23,    26,
      28,    31,    33,    38,    39,    41,    47,    52,    57
  };

  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
  const unsigned short int
  cfglp::yyrline_[] =
  {
         0,   156,   156,   156,   168,   173,   181,   180,   198,   219,
     232,   238,   247,   260,   261,   265,   273,   277,   282
  };

  // Print the state stack on the debug stream.
  void
  cfglp::yystack_print_ ()
  {
    *yycdebug_ << "Stack now";
    for (state_stack_type::const_iterator i = yystate_stack_.begin ();
	 i != yystate_stack_.end (); ++i)
      *yycdebug_ << ' ' << *i;
    *yycdebug_ << std::endl;
  }

  // Report on the debug stream that the rule \a yyrule is going to be reduced.
  void
  cfglp::yy_reduce_print_ (int yyrule)
  {
    unsigned int yylno = yyrline_[yyrule];
    int yynrhs = yyr2_[yyrule];
    /* Print the symbols being reduced, and their result.  */
    *yycdebug_ << "Reducing stack by rule " << yyrule - 1
	       << " (line " << yylno << "):" << std::endl;
    /* The symbols being reduced.  */
    for (int yyi = 0; yyi < yynrhs; yyi++)
      YY_SYMBOL_PRINT ("   $" << yyi + 1 << " =",
		       yyrhs_[yyprhs_[yyrule] + yyi],
		       &(yysemantic_stack_[(yynrhs) - (yyi + 1)]),
		       &(yylocation_stack_[(yynrhs) - (yyi + 1)]));
  }
#endif // YYDEBUG

  /* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
  cfglp::token_number_type
  cfglp::yytranslate_ (int t)
  {
    static
    const token_number_type
    translate_table[] =
    {
           0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       8,     9,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    15,    12,
      13,    16,    14,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    10,     2,    11,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7
    };
    if ((unsigned int) t <= yyuser_token_number_max_)
      return translate_table[t];
    else
      return yyundef_token_;
  }

  const int cfglp::yyeof_ = 0;
  const int cfglp::yylast_ = 33;
  const int cfglp::yynnts_ = 12;
  const int cfglp::yyempty_ = -2;
  const int cfglp::yyfinal_ = 3;
  const int cfglp::yyterror_ = 1;
  const int cfglp::yyerrcode_ = 256;
  const int cfglp::yyntokens_ = 17;

  const unsigned int cfglp::yyuser_token_number_max_ = 262;
  const cfglp::token_number_type cfglp::yyundef_token_ = 2;


} // yy
/* Line 1141 of lalr1.cc  */
#line 1123 "y.tab.c"
/* Line 1142 of lalr1.cc  */
#line 287 "parse.yy"


/* Auxiliary functions called in the parser script alone. */

program_Ptr build_Program (proc_Ptr proc_P, sym_List_Ptr gsym_lp) 
{
    /*
       We make a pointer to a list of procedure (in this version 
       we have a single procedure).
    */
    proc_Map_Ptr proc_list = new map <string, proc_Ptr>;
    string proc_name = proc_P->get_Name();
    (*proc_list)[proc_name] = proc_P;
    /*
       Now we create a new program object. In this rule of the
       grammar, we have no global declarations.
    */
    program_Ptr prog_P = new program(gsym_lp, proc_list);

    return prog_P;
}

proc_Ptr build_Procedure (string name, int bb_num, ast_List_Ptr ast_lp,  sym_List_Ptr sym_lp, sym_List_Ptr sym_gp) 
{
     /*  
         We create a basic block and then add it to a list. In 
         this version the list contains a single basic block.
     */

     bb_Ptr bbp = new basic_Block(bb_num, ast_lp);
     bb_List_Ptr bblp = new list<bb_Ptr>;
     bblp->push_back(bbp);

     /*
         Then we use the procedure name and declaration list to 
         create a procedure object.
     */ 
    
     proc_Ptr proc_P = new procedure (name, sym_lp, sym_gp, bblp);

     return proc_P;
    }


sym_Entry_Ptr redeclaration_Error(string name)
{
    entity_Type en_t = symtab_in_scope_P->get_Entity_Type(name, anywhere);
    value_Type val_t = symtab_in_scope_P->get_Value_Type(name, anywhere);

    string en_s = "";
    string val_s = "";
    en_s = (en_t == entity_Var)? "variable" : "procedure";
    val_s = (val_t == int_Val)? "INT" : ((val_t == float_Val)? "FLOAT" : "VOID");

    stringstream snum1; 
    snum1 << yylineno;

    string mesg = "Redeclaration of name `" + name + "' in the same scope on line " + snum1.str() + ". ";

    int old_line_no = symtab_in_scope_P->get_Line_Number(name);

    stringstream snum2; 
    snum2 << old_line_no;

    mesg = mesg + "Earlier declaration was as a " + en_s + " of type " + val_s + " on line " + snum2.str() + ". ";
    report_Violation_of_Condition(SHOULD_NOT_REACH, mesg);

    return NULL; 
}

ast_Ptr missing_Declaration_Error(bool lhs_d, string lhs_n, bool rhs_d, string rhs_n, int line)
{
        stringstream sn1; 
        stringstream mesg; 
        sn1 << line;
 
        if ((lhs_d == false) && (rhs_d == false))
                mesg << "Undeclared variables " << lhs_n << "and " << rhs_n << " on line " << line << ".";
        if (lhs_d == false)
        	mesg << "Undeclared variable " << lhs_n << " on line " << line <<  ".";
        else if (rhs_d == false)
        	mesg << "Undeclared variable " << rhs_n << " on line " << line <<  ".";
        else
             CHECK_INVARIANT(SHOULD_NOT_REACH, "Both LHS and RHS variables seem to be declared")
    
        report_Violation_of_Condition (SHOULD_NOT_REACH, mesg.str());

        return NULL; 
}

ast_Ptr process_Asgn_Name_Name(string lhs, string rhs, int line)
{

    ast_Ptr asgn;

    bool lhs_d = symtab_in_scope_P->declared_In_Visible_Scope(lhs, anywhere); 
    bool rhs_d = symtab_in_scope_P->declared_In_Visible_Scope(rhs, anywhere); 

    if (lhs_d && rhs_d)
    {
        ast_Ptr ast_l = new name_Ast(lhs);
        ast_Ptr ast_r = new name_Ast(rhs);
        asgn = new asgn_Ast(ast_l, ast_r, line);
        asgn->type_Check(); 
    }
    else 
        asgn = missing_Declaration_Error(lhs_d, lhs, rhs_d, rhs, line);

    return asgn;
}

ast_Ptr process_Asgn_Name_Num(string lhs, int rhs, int line)
{

    ast_Ptr asgn = NULL;

    bool lhs_d = symtab_in_scope_P->declared_In_Visible_Scope(lhs, anywhere); 

    if (lhs_d)
    {
        ast_Ptr ast_l = new name_Ast(lhs);
        ast_Ptr ast_r = new num_Ast(rhs);
        asgn = new asgn_Ast(ast_l, ast_r, line);
        asgn->type_Check(); 
    }
    else 
       asgn = missing_Declaration_Error(lhs_d, lhs, true, "dummy_string", line);

    return asgn;
}
