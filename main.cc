
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

#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>
#include <list>
#include <stdio.h>

using namespace std;

#include "common-classes.hh"
#include "evaluate.hh"
#include "support.hh"
#include "reg-alloc.hh"
#include "symtab.hh"
#include "ast.hh"
#include "program.hh"
#include "options.hh"
#include "cfglp-ctx.hh"
#include "parse.tab.hh"


extern FILE * yyin;
extern void init_Scanner();

cfglp_ctx::cfglp_ctx() { init_Scanner(); }

int 
main(int argc, char * argv[]) 
{
     /* Process command line options and set up files etc. */

     cmd_options.process_Options(argc, argv);

     /* Set up the contex of parsing, initialize the scanner */
     cfglp_ctx ctx;      

     /* Create the parser object and parse the input program */
     yy::cfglp parser(ctx); 
     int v = parser.parse();  

     /* 
        Ensure that parsing has succeded before 
        proceeding to the rest of compilation 
     */
     report_Violation_and_Abort (v==0, "Cannot parse the input program");

     /* Produce various dumps by checking the command line options */

     if ((error_Status() == false) && (cmd_options.show_Ast()))
          program_object_P->print_AST();

      if ((error_Status() == false) && (cmd_options.show_Symtab()))
          program_object_P->print_Symtab();

      if ((error_Status() == false) && (cmd_options.show_Program()))
          program_object_P->print_Program();

     /****************** Evaluate ******************/
      if ((error_Status() == false) && (cmd_options.do_Eval()))
          program_object_P->evaluate();

     // ***************** Compile *****************

      if ((error_Status() == false) && (cmd_options.do_Compile()))
      {
          program_object_P->compile();
         
          /* 
             Compilation assigns the offsets so we 
             should produce symtab dumps again.
          */
          if ((error_Status() == false) && (cmd_options.show_Program()))
              program_object_P->print_Program();
          if ((error_Status() == false) && (cmd_options.show_Symtab()))
              program_object_P->print_Symtab();
      }

     return 0;
}

namespace yy {
void
cfglp::error(location const &loc, const std::string& s)
{
  std::cerr << "\n     cfglp error at " << loc << ": " << s << "\n";
}
}

