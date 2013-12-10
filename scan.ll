
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


%option noyywrap
%{
# include <cstdlib>

using namespace std;

#include "common-classes.hh"
#include "evaluate.hh"
# include "cfglp-ctx.hh"
# include "options.hh"
# include "reg-alloc.hh"
# include "symtab.hh"
# include "ast.hh"
# include "program.hh"
# include "parse.tab.hh"
# include "support.hh"
# include "icode.hh"


/* 
   Our scanner specification allows us to trap 

   - the tokens 
   - the tokan codes, and
   - the lexemes

   being passed from the scanner to the parser for the
   purpose of testing the scanner. 

   This is enabled by renaming the lex generated function to 
   lexscan and defining a wrapper function yylex that 
   - receives the information from lexscan, 
   - prints the information (if -tokens option has been provided) 
   - and makes it available to the parser in a transparent manner.

*/
   

void store_Token_Name(const char * tok_name);
void ignore_Token();
static char * token_name;

static bool show_tokens = false;
static ostream * tokens_fp = NULL;

#define YY_DECL int lexscan(yy::cfglp::semantic_type *yylval, \
    yy::cfglp::location_type *yylloc, cfglp_ctx &ctx)

// make location include the current token
# define YY_USER_ACTION  yylloc->columns (yyleng);

typedef yy::cfglp::token token;

%}

digit [0-9]
letter [a-z_A-Z_] 
dot [.]
exp [eE]
id {letter}({letter}|{digit})*
%%

%{
     // start where previous token ended
     yylloc->step ();
%}

[<>:{}();] { 
                 store_Token_Name ("META CHAR");
                 return yytext[0]; 
            }
[-]?{digit}+    { 
                 store_Token_Name ("I_NUM");
                 yylval->ival = atoi(yytext); 
                 return token::I_NUM; 
            }
[-]?(({digit}+[\.]?{digit}*)|({digit}*[\.]?{digit}+)|({digit}+[.]{digit}+[eE][+-]{digit}+)) {
                 store_Token_Name ("D_NUM");
                 yylval->dval = atof(yytext); 
                 return token::D_NUM; 
            }

[-/+*=]   {
        store_Token_Name ("OPERATOR");
        return yytext[0];
      }

int         {    
                 store_Token_Name("INT");
                 return token::INT; 
            }

float       {    
                 store_Token_Name("FLOAT");
                 return token::FLOAT; 
            }
  
double      {    
                 store_Token_Name("DOUBLE");
                 return token::DOUBLE; 
            }


return      { 
                 store_Token_Name("RETURN");
                 return token::RETURN; 
            }

static      { 
                 store_Token_Name("STATIC");
                 return token::STATIC; 
            }


{id} {
                 store_Token_Name("ID");
                 yylval->sval = new std::string (yytext);
                 return token::ID; 
             }

{id}({letter}|{digit})*{dot}{digit}{digit}{digit}{digit} {
        store_Token_Name("EXP_VAR");
        yylval->sval = new std::string (yytext);  
        return token::EXP_VAR;

        }
{id}{dot}({digit})* {
        store_Token_Name("ARTIFICIAL_VAR");
        yylval->sval = new std::string (yytext);  
        return token::ARTIFICIAL_VAR;
}

[<][b][b][ ]{digit}+[>] {
        store_Token_Name("BB_ID");
        yylval->sval = new std::string (yytext);
        return token::BB_ID;

}
\n           { 
                 yylloc->lines(1); 
                 yylineno++;
/*
                 cout << "scanned line " << yylineno << "\n";
*/
                 ignore_Token();
             }    

  /* skip over comments and white space */
";;".*  |
[ \t]   {  yylloc->step (); 
           ignore_Token();
        }

.       { 
            stringstream ss;
            ss <<  "Illegal character `" << yytext << "' on line " << yylineno;
            report_Violation_of_Condition(false, ss.str());
        }
%%

void init_Scanner ()
{
    show_tokens = cmd_options.show_Tokens();
    if (show_tokens)
        tokens_fp = cmd_options.tokens_File(); 
}

void store_Token_Name(const char * tok_name)
{     
    if (token_name) delete token_name;
    token_name = strdup (tok_name);
}

int yylex(yy::cfglp::semantic_type *yylval, 
    yy::cfglp::location_type *yylloc, cfglp_ctx &ctx)
{    int token_code;

     token_code = lexscan(yylval, yylloc, ctx);

     if (show_tokens)
     {
          if (token_code)
          {
               string mesg = " Token name has not been set for the lexeme `" + string(yytext) +"'";
               CHECK_INVARIANT (token_name, mesg)
               *tokens_fp << "Line: " << yylineno << " Token name:" << token_name << "\ttoken code:" << token_code << "\tlexeme: `" << yytext << "'\n";
               delete token_name;
               token_name = NULL;
          }
    }
    return token_code;
}

void ignore_Token()
{
    if (show_tokens)
    {
       if (yytext[0] == '\n')
           *tokens_fp << "Line: "<< yylineno << " Ignored NEWLINE character\n";
       else
           *tokens_fp << "Line: "<< yylineno << " Ignored lexeme: '" << yytext << "'\n";
    }
}
