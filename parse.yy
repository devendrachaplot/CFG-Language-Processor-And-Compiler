
/*********************************************************************************************

                                cfglp : A CFG Language Processor
                                --------------------------------

               About:
               Implemented by Uday Khedker (http://www.cse.iitb.ac.in/~uday) for 
               the courses cs302+cs306: Language Processors (theory and lab) at
               IIT Bombay. Release date Jan 6, 2013. Copyrights reserved by Uday
               Khedker. This implemenation has been made available purely for
               academic purposes without any warranty of any kind.

               Please see the README file for some details. A doxygen generated
               documentation can be found at http://www.cse.iitb.ac.in/~uday/cfglp


***********************************************************************************************/


%skeleton "lalr1.cc"
%language "C++"
%defines
%locations

%define parser_class_name "cfglp"

%{
#include <iostream>
#include <string.h>
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


%}

%parse-param { cfglp_ctx &ctx }
%lex-param   { cfglp_ctx &ctx }

%union 
{
    int ival;
    double dval;
    std::string *sval;
    ast_Ptr ast_P;
    ast_List_Ptr ast_L;
    sym_Entry_Ptr sym_P;
    proc_Ptr proc_P;
    program_Ptr prog_P;
};

/* declare tokens */
%token <ival> I_NUM
%token <dval> D_NUM
%token <sval> BB_ID
%token <sval> ID
%token <sval> ARTIFICIAL_VAR
%token <sval> EXP_VAR
%token INT FLOAT DOUBLE RETURN STATIC 

%left '*' '/'
%left '+' '-'


%type <ast_P> identifiers
%type <ast_P> Expr
%type <ast_P> Expr2
%type <ast_P> Expr3
%type <ast_P> exe_Stmt
%type <ast_L> exe_Stmt_List
%type <sym_P> decl_Stmt 
%type <ival> bb_Decl
%type <proc_P> procedure
%type <prog_P> program

%{
  extern int yylex(yy::cfglp::semantic_type *yylval,
       yy::cfglp::location_type* yylloc,
       cfglp_ctx &ctx);

/* Some auxiliary local functions */

  static program_Ptr build_Program(proc_Ptr proc_P, sym_List_Ptr gsym_lp);
  static proc_Ptr build_Procedure (string name, int bb_num, ast_List_Ptr ast_lp,  
                sym_List_Ptr sym_lp, sym_List_Ptr sym_gp);
  static sym_Entry_Ptr redeclaration_Error(string name);
  static ast_Ptr missing_Declaration_Error(bool lhs_d, string lhs_n, bool rhs_d, string rhs_n, int line);
  static ast_Ptr process_Asgn_Name_Expression(ast_Ptr ast_l, ast_Ptr ast_r, int line);
  static ast_Ptr process_Asgn_Name_Name(string lhs, string rhs, int line);
  static ast_Ptr process_Asgn_Name_Num(string lhs, int rhs, int line);
  static  ast_Ptr process_Asgn_Exp_Exp(ast_Ptr ast_l, ast_Ptr ast_r, int line, char op);

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
%}

%initial-action {
 // Filename for locations here
 @$.begin.filename = @$.end.filename = new std::string("stdin");
}
%%

start: 
    {
        symtab_in_scope_P = new symtab_In_Scope;
        symtab_in_scope_P->allocate_Sym_List(); 
    }
    program[prg]
    {
        program_object_P = $prg;
        symtab_in_scope_P->deallocate_Sym_List(); 
    }
 ;

program: 
    decl_Stmt_List procedure[proc]
    {
        sym_List_Ptr sym_lp = symtab_in_scope_P->get_Symtab_Top_List(); 
        $$=build_Program($proc, sym_lp);
    }
 	|  procedure[proc]
	{
		sym_List_Ptr sym_lp = symtab_in_scope_P->get_Symtab_Top_List();
		$$=build_Program($proc, sym_lp);
	}
 ;

procedure: 
    ID[id] 
    {
        string name = *$id;

        if (symtab_in_scope_P->declared_In_Visible_Scope(name, symtab_Top) == false)
            symtab_in_scope_P->enter_Procedure_Name(name, yylineno);
        else
            redeclaration_Error(name);
        symtab_in_scope_P->allocate_Sym_List(); 
    }
    '(' ')' '{' decl_Stmt_List bb_Decl[bbn] exe_Stmt_List[slist] '}'
    {
        
        /*
        ast_List_Ptr ls = $slist;
        list<ast_Ptr>::iterator i;
        for(i = ls->begin(); i != ls->end(); i++)
        {
        	cout<<"B";
        	ast_Ptr asp = *i;
        	cout<<asp->t_op;
        	if(asp->t_op == asgn)
        	{
        		cout<<"A";
        		sym_Entry_Ptr sp = ((asgn_Ast*)asp)->left->get_Sym_Entry();
        		if(sp->typ == 2)
        		{
        			if(sp->node)
        				report_Violation_of_Condition(false,"not used");
        		}
        	}
        	
        }
        */
        string name = *$id;
        sym_List_Ptr sym_lp = symtab_in_scope_P->get_Symtab_Local_List(); 
        
        /*
        ///////
        stringstream sn;
        sn<<yylineno;
        map<string, sym_Entry_Ptr>::iterator a;
    	for(a=(sym_lp->sym_list).begin(); a!=(sym_lp->sym_list).end(); a++)
    	{
			string s = a->first;
			sym_Entry_Ptr se = a->second;
			if(se->typ==2 && se->node)
			{
				ast_Ptr as=se->node;
				report_Violation_of_Condition(false, "Temporary variable "+s+" is not used in the block");
    		}
    	}
        ///////
        */
        sym_List_Ptr sym_gp = symtab_in_scope_P->get_Symtab_Global_List(); 
        int bb_num = $bbn;
        ast_List_Ptr ast_lp = $slist;
        $$ = build_Procedure (name, bb_num, ast_lp, sym_lp, sym_gp);
        symtab_in_scope_P->deallocate_Sym_List(); 
    }
 ;

exe_Stmt_List:   
    exe_Stmt_List[list] exe_Stmt[item] 
    {
        if ($list && $item)
        {
            ast_List_Ptr ast_lp = $list;
            ast_lp->push_back($item);
            $$ = ast_lp;
        }
        else if ($list && !$item)
        {
            $$ = $list;
        }
        else if (!$list && $item)
        {   
            ast_List_Ptr ast_lp = new list<ast_Ptr>;
            ast_lp->push_back($item);    
            $$ = ast_lp;
        }
        else $$ = NULL;
    }
                
 |  exe_Stmt[stmt] 
    {
        if ($stmt)
        { 
            ast_List_Ptr ast_lp = new list<ast_Ptr>;
            ast_lp->push_back($stmt);    
            $$ = ast_lp;
        }
        else $$ = NULL;
    }
 ;

decl_Stmt_List: 
    decl_Stmt_List decl_Stmt[item]
    {
        sym_List_Ptr sym_lp = symtab_in_scope_P->get_Symtab_Top_List(); 
        if ($item)
            sym_lp->insert_Sym_Entry($item);
    }
 |  decl_Stmt[item]
    {
        sym_List_Ptr sym_lp = symtab_in_scope_P->get_Symtab_Top_List(); 
        if ($item)
            sym_lp->insert_Sym_Entry($item);
    }
 ;

decl_Stmt: 
    static_kw INT ID[id] ';'
    {
        string name = *$id;
        if (symtab_in_scope_P->declared_In_Visible_Scope(name, symtab_Top) == false)
        {
            sym_Entry_Ptr s = new sym_Entry_for_Int_Var(name, yylineno, 0);
            $$ = s;
        }
        else
            $$ = redeclaration_Error(name);
    }
    
    | static_kw INT ARTIFICIAL_VAR[id] ';'
    {
        string name = *$id;
        unsigned pos = name.find(".");
        string id_name = name.substr(0,pos);
        if (symtab_in_scope_P->declared_In_Visible_Scope(id_name, anywhere) == true)
        {
        	sym_Entry_Ptr s = new sym_Entry_for_Int_Var(name, yylineno, 1);
            $$ = s; 
        }
        else
        {   
         	stringstream ss;
         	ss<<"Declaration of artificial version of variable "<<id_name<<" on line "<<yylineno<<" should have been preceded by a global declaration.";
            report_Violation_of_Condition(false,ss.str());
            $$=NULL;
        }
    }
    | static_kw INT EXP_VAR[id] ';'
    {
        string name = *$id;
        if (symtab_in_scope_P->declared_In_Visible_Scope(name, symtab_Top) == false)
        {
        	sym_Entry_Ptr s = new sym_Entry_for_Int_Var(name, yylineno, 2);
            $$ = s; 
        }
        else
            $$ = redeclaration_Error(name);
    }
    
    | static_kw FLOAT ID[id] ';'
    {
        string name = *$id;
        if (symtab_in_scope_P->declared_In_Visible_Scope(name, symtab_Top) == false)
        {
            sym_Entry_Ptr s = new sym_Entry_for_Float_Var(name, yylineno,0);
            $$ = s;
        }
        else
            $$ = redeclaration_Error(name);
    }
    
    | static_kw FLOAT ARTIFICIAL_VAR[id] ';'
   {
    	string name = *$id;
        unsigned pos = name.find(".");
        string id_name = name.substr(0,pos);//strtok(name, '.');
        if (symtab_in_scope_P->declared_In_Visible_Scope(id_name, anywhere) == true)
        {
        	sym_Entry_Ptr s = new sym_Entry_for_Float_Var(name, yylineno, 1);
            $$ = s; 
        }
        else
        {   
         	stringstream ss;
         	ss<<"Declaration of artificial version of variable "<<id_name<<" on line "<<yylineno<<" should have been preceded by a global declaration.";
            report_Violation_of_Condition(false,ss.str());
            $$=NULL;
        }
    }
    | static_kw FLOAT EXP_VAR[id] ';'
    {
        string name = *$id;
        if (symtab_in_scope_P->declared_In_Visible_Scope(name, symtab_Top) == false)
        {
           sym_Entry_Ptr s = new sym_Entry_for_Float_Var(name, yylineno, 2);
            $$ = s;
        }
        else
            $$ = redeclaration_Error(name);
    }
    
    | static_kw DOUBLE ID[id] ';'
    {
        string name = *$id;
        if (symtab_in_scope_P->declared_In_Visible_Scope(name, symtab_Top) == false)
        {
            sym_Entry_Ptr s = new sym_Entry_for_Float_Var(name, yylineno,0);
            $$ = s;
        }
        else
            $$ = redeclaration_Error(name);
    }
    
    | static_kw DOUBLE ARTIFICIAL_VAR[id] ';'
    {
    	string name = *$id;
        unsigned pos = name.find(".");
        string id_name = name.substr(0,pos);//strtok(name, '.');
        if (symtab_in_scope_P->declared_In_Visible_Scope(id_name, anywhere) == true)
        {
        	sym_Entry_Ptr s = new sym_Entry_for_Float_Var(name, yylineno, 1);
            $$ = s; 
        }
        else
         {   
         	stringstream ss;
         	ss<<"Declaration of artificial version of variable "<<id_name<<" on line "<<yylineno<<" should have been preceded by a global declaration.";
            report_Violation_of_Condition(false,ss.str());
            $$=NULL;
        }
    }
    | static_kw DOUBLE EXP_VAR[id] ';'
    {
        string name = *$id;
        if (symtab_in_scope_P->declared_In_Visible_Scope(name, symtab_Top) == false)
        {
           sym_Entry_Ptr s = new sym_Entry_for_Float_Var(name, yylineno,2);
            $$ = s;
        }
        else
            $$ = redeclaration_Error(name);
    }
 ;

static_kw:  /* empty */
 | STATIC
 ;

bb_Decl: 
    BB_ID[str] ':'
    {
    	stringstream s(*$str);
    	string s1,s2;
    	s>>s1;
    	s>>s2;
        $$=atoi(s2.c_str());
    }
 ;

identifiers :
    ID[name]
    {
    	ast_Ptr asgn;
        bool lhs_d = symtab_in_scope_P->declared_In_Visible_Scope(*$name, anywhere);
        if(lhs_d)
        	asgn = new name_Ast(*$name);
        else
        	asgn = missing_Declaration_Error(lhs_d, *$name, true, "operator", yylineno);
        $$ = asgn; 
    }
    | ARTIFICIAL_VAR[name]
    {
    	ast_Ptr asgn;
        string name2 = *$name;
        unsigned pos = name2.find(".");
        string id_name = name2.substr(0,pos);
        if (symtab_in_scope_P->declared_In_Visible_Scope(id_name, anywhere) == true)
        {
        	$$ = new name_Ast(id_name);
        }
        else
        {   
         	stringstream ss;
         	ss<<"Undeclared variable "<<id_name<<" on line "<<yylineno<<".";
            report_Violation_of_Condition(false,ss.str());
            $$=NULL;
        }
    }
    | EXP_VAR[name]
    {
    	ast_Ptr asgn;
        bool lhs_d = symtab_in_scope_P->declared_In_Visible_Scope(*$name, anywhere);
        if(lhs_d)
        	asgn = new name_Ast(*$name);
        else
        	asgn = missing_Declaration_Error(lhs_d, *$name, true, "operator", yylineno);
        $$ = asgn; 
    }
    ;

exe_Stmt : 
    identifiers[lhs] '=' Expr[rhs] ';'{
        $$ = process_Asgn_Name_Expression($lhs, $rhs, yylineno);
    }
    | RETURN ';' 
    {
        $$ = new ret_Ast();
    }
    ;


Expr:
    Expr2[lhs] '+' Expr2[rhs]{
        $$ = process_Asgn_Exp_Exp($lhs, $rhs, yylineno, '+');
    }
    |
    Expr2[lhs] '-' Expr2[rhs]{
        $$ = process_Asgn_Exp_Exp($lhs, $rhs, yylineno, '-');
    }
    |
    Expr2[lhs] '*' Expr2[rhs]{
        $$ = process_Asgn_Exp_Exp($lhs, $rhs, yylineno, '*');
    }
    |
    Expr2[lhs] '/' Expr2[rhs]{
        $$ = process_Asgn_Exp_Exp($lhs, $rhs, yylineno, '/');
    }
    | Expr2[var] {
        $$ = $var;
    }
    
    ;
Expr2:
    Expr3[var] {
        $$=$var;
        //$$ = new type_converter_Ast($var,float_Val,yylineno);
    }
    |
     '-' Expr2[var] {
     	if($var)
        	$$ = new uminus_Ast($var, yylineno); 
        else
        	$$=NULL;
    }
    | '(' FLOAT ')' Expr3[var] {
        //$$ = new type_converter_Ast($var,float_Val,yylineno);
        $$=$var;
    }
    | '(' DOUBLE ')' Expr3[var] {
        //$$ = new type_converter_Ast($var,float_Val,yylineno);
        $$=$var;
    }
    ;
Expr3:
    ID[name] { 
        ast_Ptr asgn;
        bool lhs_d = symtab_in_scope_P->declared_In_Visible_Scope(*$name, anywhere);
        if(lhs_d)
            asgn = new name_Ast(*$name);
        else
            asgn = missing_Declaration_Error(lhs_d, *$name, true, "operator", yylineno);
        $$ = asgn; 
    }
    | EXP_VAR[name] {
    
    	stringstream sn;
    	sn << yylineno;
        ast_Ptr asgn=NULL;
        bool lhs_d = symtab_in_scope_P->declared_In_Visible_Scope(*$name, anywhere);
        if(lhs_d)
        {
            sym_Entry_Ptr lsym = NULL;
            lsym=symtab_in_scope_P->get_Sym_Entry(*$name);
            asgn = lsym->node;
            lsym->node = NULL;
            if(!asgn)
                //asgn = new name_Ast(*$name);
                report_Violation_of_Condition(false,"Use of temporary variable "+*$name+" on line "+sn.str()+" is not preceded by a definition in the block");
            
        }
        else
            asgn = missing_Declaration_Error(lhs_d, *$name, true, "operator", yylineno);
        $$ = asgn; 
        
        
    }
    | ARTIFICIAL_VAR[name] { 
        string name2 = *$name;
        unsigned pos = name2.find(".");
        string id_name = name2.substr(0,pos);
        if (symtab_in_scope_P->declared_In_Visible_Scope(id_name, anywhere) == true)
        {
            $$ = new name_Ast(id_name);
        }
        else
        {   
            stringstream ss;
            ss<<"Undeclared variable "<<id_name<<" on line "<<yylineno<<".";
            report_Violation_of_Condition(false,ss.str());
            $$=NULL;
        }
    }
    | I_NUM[id] {
        $$ = new num_Ast($id);
    }
    | D_NUM[id] {
        $$ = new dnum_Ast($id);
    }
    | '(' Expr[var] ')' {
        $$ = $var;
    }
    ;

%%

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

ast_Ptr process_Asgn_Name_Expression(ast_Ptr ast_l, ast_Ptr ast_r, int line)
{

    ast_Ptr asgn=NULL;
    if(ast_l)
    {
		string lhs = ast_l->get_Name();
		stringstream sn;
		sn << line;
		//string rhs = ast_l->get_Name();
		bool lhs_d = symtab_in_scope_P->declared_In_Visible_Scope(lhs, anywhere); 
		//bool rhs_d = symtab_in_scope_P->declared_In_Visible_Scope(rhs, anywhere); 
	   
		//cout<<"hoohaa " << rhs<<endl;
		sym_Entry_Ptr lsym = NULL;
		lsym=ast_l->get_Sym_Entry();
		if(lsym->typ != 0){
			if(!lsym->node)
				lsym->node = ast_r;
			else
				report_Violation_of_Condition (false, "Redefinition of EXP_VAR "+lhs+" on line "+sn.str());
			asgn = new asgn_Ast(ast_l, ast_r, line);
            asgn->type_Check();
            return NULL;
		}
		

		if (lhs_d)
		{
			if(ast_l && ast_r)
			{
				if(ast_r->get_Entity_Type() == ast_l->get_Entity_Type() && ast_l->get_Entity_Type()==entity_Var)
				{
					if(strcmp(ast_l->get_Name().c_str(),ast_r->get_Name().c_str())==0)
						return NULL;
				}
			}
		    // ast_Ptr ast_l = new name_Ast(lhs);
		    // ast_Ptr ast_r = new name_Ast(rhs);
		    asgn = new asgn_Ast(ast_l, ast_r, line);
		    asgn->type_Check(); 
		}
	 
		else 
		    asgn = missing_Declaration_Error(lhs_d, lhs, true, "operator", line);
	
	}
		return asgn;
}


ast_Ptr process_Asgn_Exp_Exp(ast_Ptr ast_l, ast_Ptr ast_r, int line, char op)
{

    ast_Ptr asgn=NULL;
    /*string lhs = ast_l->get_Name();
    string rhs = ast_l->get_Name();
    bool lhs_d = symtab_in_scope_P->declared_In_Visible_Scope(lhs, anywhere); 
    bool rhs_d = true;//symtab_in_scope_P->declared_In_Visible_Scope(rhs, anywhere); 

    if (lhs_d)
    {*/
    if(ast_l && ast_r)
    {
        switch(op)
        {
		    // ast_Ptr ast_l = new name_Ast(lhs);
		    // ast_Ptr ast_r = new name_Ast(rhs);
		    case '+':
				asgn = new plus_Ast(ast_l, ast_r, line);
				asgn->type_Check(); 
				break;
			case '-':
				asgn = new minus_Ast(ast_l, ast_r, line);
				asgn->type_Check(); 
				break;
			case '*':
				asgn = new mult_Ast(ast_l, ast_r, line);
				asgn->type_Check(); 
				break;
			case '/':
				asgn = new div_Ast(ast_l, ast_r, line);
				asgn->type_Check(); 
				break;
		}
	}    
    else 
        asgn = NULL;//missing_Declaration_Error(true, "bb", true, "aa", line);

	
    return asgn;
}


ast_Ptr process_Name(string name){
    ast_Ptr asgn = new name_Ast(name);
    return asgn;
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

ast_Ptr process_Asgn_Name_Num(string lhs, float rhs, int line)
{

    ast_Ptr asgn = NULL;

    bool lhs_d = symtab_in_scope_P->declared_In_Visible_Scope(lhs, anywhere); 

    if (lhs_d)
    {
        ast_Ptr ast_l = new name_Ast(lhs);
        ast_Ptr ast_r = new dnum_Ast(rhs);
        asgn = new asgn_Ast(ast_l, ast_r, line);
        asgn->type_Check(); 
    }
    else 
       asgn = missing_Declaration_Error(lhs_d, lhs, true, "dummy_string", line);

    return asgn;
}
