
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
#include <fstream>
#include <ostream>
#include <string>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

#include "common-classes.hh"
#include "options.hh"
#include "support.hh"


options cmd_options;

options::options() 
{
    show_tokens = show_ast = show_symtab = show_program = show_ic
    = do_lra = do_eval = do_compile = demo_mode = false;
} 

options::~options()
{
    if (demo_mode == false )
    {
        if (show_tokens) ofs_tokens.close(); 
        if (show_ast) ofs_ast.close(); 
        ofs_output.close(); 
    }
}

bool options::show_Tokens()      { return show_tokens; }
bool options::show_Ast()         { return show_ast; }
bool options::show_Symtab()      { return show_symtab; }
bool options::show_Program()     { return show_program; }
bool options::do_Eval()          { return do_eval; }
bool options::do_Compile()       { return do_compile; }
bool options::do_Lra()           { return do_lra; }
bool options::demo_Mode()        { return demo_mode; }
bool options::show_IC()          { return show_ic; }

ostream * options::tokens_File() 
{ 
   CHECK_INVARIANT(show_tokens, "Show tokens option not set")
   if (demo_mode) return &cout;
   else return &ofs_tokens; 
}

ostream * options::ast_File()    
{   
   CHECK_INVARIANT(show_ast, "Show AST option not set")
   if (demo_mode) return &cout;
   else return &ofs_ast; 
}

ostream * options::sym_File()    
{   
   CHECK_INVARIANT(show_symtab, "Show symtab option not set")
   if (demo_mode) return &cout;
   else return &ofs_sym; 
}

ostream * options::prog_File()    
{   
   CHECK_INVARIANT(show_program, "Show program option not set")
   if (demo_mode) return &cout;
   else return &ofs_prog; 
}

ostream * options::ic_File()    
{   
   CHECK_INVARIANT(show_ic , "Show ic option not set")
   if (demo_mode) return &cout;
   else return &ofs_ic; 
}



ostream * options::output_File() 
{ 
   if (demo_mode) return &cout;
   else return &ofs_output; 
}

/* 
   Lex generated scanner reads from a C style file pointer called yyin which is
   set to stdin by default. If an input file name has been given on the command 
   line, we need to open it and set its pointer to yyin.
*/

extern FILE * yyin;

void options::process_Options(int argc, char * argv[])
{    

    bool input_file_given = false;

    string input_file_name, tokens_file_name, ast_file_name, sym_file_name, prog_file_name, 
           ic_file_name, output_file_name;

    char * input_file_c_string = NULL;

    const string usage = "\n     Usage: cfglp [options] [file]\n\
     Options:\n\
          -help     Show this help\n\
          -tokens   Show the tokens in file.toks (or out.toks)\n\
          -ast      Show abstract syntax trees in file.ast (or out.ast)\n\
          -symtab   Show the symbol table of declarations in file.sym (or out.sym)\n\
          -program  Show the complete program read by cfglp in file.prog (or out.prog)\n\
                    (-program option cannot be given with -tokens, -ast, or -symtab) \n\
          -eval     Interpret the program and show a trace of the execution in file.eval (or out.eval)\n\
          -compile  Compile the program and generate spim code in file.spim (or out.spim)\n\
                    (-eval and -compile options are mutually exclusive)\n\
                    (-compile is the default option)\n\
          -lra      Do local register allocation to avoid redundant loads within a basic block\n\
          -icode    Compile the program and show the intermediate code in file.ic (or out.ic)\n\
                    (-eval and -icode options are mutually exclusive)\n\
          -d        Demo version. Use stdout for the outputs instead of files\n\n";


    /* First scan the command line to discover the options and set appropriate flags.  */

    for (int i = 1; i < argc; i++)
    {    
        char * option = strdup(argv[i]);
        if (strncmp(option,"-",1) == 0)
        {
            if (strcmp(option,"-d") == 0) demo_mode = true;
            else if (strcmp(option,"-tokens") == 0) 
            {
                string mesg = "-program option cannot be given with -tokens, -ast or -symtab options" + usage;
                report_Violation_and_Abort (!show_program, mesg);
                show_tokens = true;
            }
            else if (strcmp(option,"-symtab") == 0) 
            {
                string mesg = "-program option cannot be given with -tokens, -ast or -symtab options" + usage;
                report_Violation_and_Abort (!show_program, mesg);
                show_symtab = true;
            }
            else if (strcmp(option,"-ast") == 0) 
            {
                string mesg = "-program option cannot be given with -tokens, -ast or -symtab options" + usage;
                report_Violation_and_Abort (!show_program, mesg);
                show_ast = true;
            }
            else if (strcmp(option,"-program") == 0) 
            {
                string mesg = "-program option cannot be given with -tokens, -ast or -symtab options" + usage;
                report_Violation_and_Abort (!(show_ast || show_tokens || show_symtab), mesg);
                show_program = true;
            }
            else if (strcmp(option,"-eval") == 0)
            {
                string mesg = "-compile and -eval options are mutually exclusive" + usage;
                report_Violation_and_Abort (!do_compile, mesg);

                mesg = "-icode and -eval options are mutually exclusive" + usage;
                report_Violation_and_Abort (!show_ic, mesg);

                mesg = "-lra and -eval options are mutually exclusive" + usage;
                report_Violation_and_Abort (!do_lra, mesg);

                do_eval = true;
            }
            else if (strcmp(option,"-icode") == 0)
            {
                string mesg = "-icode and -eval options are mutually exclusive" + usage;
                report_Violation_and_Abort (!do_eval, mesg);
                show_ic = true;
            }
            else if (strcmp(option,"-lra") == 0)
            {
                string mesg = "-lra and -eval options are mutually exclusive" + usage;
                report_Violation_and_Abort (!do_eval, mesg);
                do_lra = true;
            }
            else if (strcmp(option,"-compile") == 0)
            {
                string mesg = "-compile and -eval options are mutually exclusive" + usage;
                report_Violation_and_Abort (!do_eval, mesg);
                do_compile = true;
            }
            else if ((strcmp(option,"--help") == 0) || (strcmp(option,"-help") == 0))
            {        
                cerr << usage;
                exit(0);
            }
            else 
            {    
                string mesg = "Unknown option `" + string(option) + "'" + usage;
                report_Violation_and_Abort(SHOULD_NOT_REACH, mesg); /* Force reporting of the error */
            }
       }
       else if (input_file_given)
       {
           string mesg = "Only one input file name can be provided" + usage;
           report_Violation_and_Abort(SHOULD_NOT_REACH, mesg); /* Force reporting of the error */
       }
       else    
       {    
           input_file_given = true;
           input_file_c_string = strdup(option);
       }
    }

    /* 
       The default option is to compile the program. If the eval 
       option has not been given we should set this option.
    */

    do_compile = (do_eval)? false : true;


    /* Having set the option flags, its time to set the input and output streams. */

    if (input_file_given == true)
    {
        CHECK_INVARIANT (input_file_c_string, "Input file string cannot be NULL")
        input_file_name = string (input_file_c_string);

        yyin = fopen(input_file_c_string,"r");
        string mesg = "Cannot open file `" + input_file_name + "' for input";
        report_Violation_and_Abort (yyin, mesg);
    }
    else input_file_name = "out";

    
    CHECK_INVARIANT(input_file_name.c_str(), "Input file name cannot be NULL")

    tokens_file_name = input_file_name + ".toks"; 
    ast_file_name = input_file_name + ".ast"; 
    sym_file_name = input_file_name + ".sym"; 
    prog_file_name = input_file_name + ".prog"; 
    ic_file_name = input_file_name + ".ic"; 
    if (do_eval) output_file_name = input_file_name + ".eval"; 
    else output_file_name = input_file_name + ".spim"; 

    /* 
       Remove old dump files. Function remove is equivalent of `rm -f' except that it
       return a non-zero status if the file did not exist, or could not be removed
       (eg. because of permissions). But the return status does not matter to us
    */

    remove (tokens_file_name.c_str()); 
    remove (ast_file_name.c_str()); 
    remove (sym_file_name.c_str()); 
    remove (prog_file_name.c_str()); 
    remove (output_file_name.c_str()); 
    remove (ic_file_name.c_str()); 
    remove ("out.toks");
    remove ("out.ast");
    remove ("out.sym");
    remove ("out.prog");
    remove ("out.eval");
    remove ("out.ic");

    /* Open new dump files if demo_mode is false. Warn if a file could not be opened. */

    if (demo_mode == false)
    {
        if (show_tokens) ofs_tokens.open(tokens_file_name.c_str()); 
        if (show_ast) ofs_ast.open(ast_file_name.c_str()); 
        if (show_symtab) ofs_sym.open(sym_file_name.c_str()); 
        if (show_program) ofs_prog.open(prog_file_name.c_str()); 
        if (show_ic) ofs_ic.open(ic_file_name.c_str()); 
        ofs_output.open(output_file_name.c_str()); 
        report_Violation_and_Abort (ofs_tokens, "Unable to open output file for tokens");
        report_Violation_and_Abort (ofs_ast, "Unable to open output file for AST");
        report_Violation_and_Abort (ofs_sym, "Unable to open output file for symbol table");
        report_Violation_and_Abort (ofs_prog, "Unable to open output file for program");
        report_Violation_and_Abort (ofs_ic, "Unable to open output file for program");
        report_Violation_and_Abort (ofs_output, "Unable to open output file");
    }
}
