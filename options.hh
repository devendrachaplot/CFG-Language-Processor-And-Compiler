
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
#include <stdio.h>
#include <fstream>
#include <ostream>

using namespace std;

class options
{
    /* 
       Command line options controlling the behaviour of cfglp 
       are recorded in the following flag. See the usage string 
       in function options::process_Options in options.cc.
     */

    bool show_tokens; 
    bool show_ast;
    bool show_symtab;
    bool show_program;
    bool show_ic;
    bool do_eval; 
    bool do_lra; 
    bool do_compile;
    bool demo_mode;

    /* Output streams */

    ostream* os_tokens;
    ostream* os_ast;
    ostream* os_sym;
    ostream* os_prog;
    ostream* os_output;

    /* 
       Privately, we also need objects of ofstream which can be opened
       if required and their addresses set to the pointers above.
    */
 
    ofstream ofs_tokens;
    ofstream ofs_ast;
    ofstream ofs_sym;
    ofstream ofs_prog;
    ofstream ofs_ic;
    ofstream ofs_output;

  public:
    options(); 
    ~options();

    bool show_Tokens();
    bool show_Ast();
    bool show_Symtab();
    bool show_Program();
    bool do_Eval();
    bool do_Compile();
    bool do_Lra();
    bool show_IC();
    bool demo_Mode();
    void process_Options(int argc, char * argv[]);

    /*
       Our accessor stream functions below return pointers to objects 
       of ostream class instead of objects of ofstream class because 
       we want to choose between the following two possibilities:

       - If an input file has been provided, the output stream should be 
         files with appropriate extensions attached to the input file name.
       - If no input file has been povided, the output stream should be
         the standard output cout.

       A file out stream is an object of class ofstream which is a derived
       class of ostream which is the class of cout. Since we would like to 
       use object of a derived class or base class, we need to use pointers
       to the base class. 
    */

    ostream* tokens_File();
    ostream* ast_File();
    ostream* sym_File();
    ostream* prog_File();
    ostream* output_File();
    ostream* ic_File();
};

extern options cmd_options;
