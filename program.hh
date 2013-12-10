
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
#include <ostream>
#include <vector>
#include <list>
#include <map>
#include <string>

using namespace std;

/* 
   This is the top level representation of our programs.

   - Program consists of  global symtab and list of procedures. 
   - Procedures consist of local symtab and list of basic blocks.
   - Basic blocks consist of list of ASTs.

*/

class program;
class procedure;
class basic_Block;
typedef program * program_Ptr;
typedef procedure * proc_Ptr;
typedef basic_Block * bb_Ptr;
typedef list <bb_Ptr> * bb_List_Ptr;
typedef map <string, proc_Ptr> proc_Map; 
typedef proc_Map * proc_Map_Ptr;

extern program_Ptr program_object_P;

class program
{
    sym_List_Ptr global_sym_list;
    proc_Map_Ptr proc_list;
  public:
    program(sym_List_Ptr, proc_Map_Ptr); 
    ~program() {}

    program& operator=(const program& rhs);

    /* General dump functions */
    void print_Program();
    void print_Symtab();
    void print_AST();
    void print_Icode();

    /* Functions for evaluation */
    proc_Ptr get_Main_Proc_Ptr();
    eval_Result evaluate();

    /* Functions for compilaton */
    void compile();
    void print_Assembly();

};

class procedure
{   
    string name;
    value_Type ret_val_type;

    sym_List_Ptr local_sym_list;

    /* 
       We also maintain a pointer to the global sym list
       for accessing the global variables during printing
    */
    sym_List_Ptr global_sym_list;
    bb_List_Ptr bb_list;

    icode_Stmt_List icode_list;

  public: 
    procedure(string name, sym_List_Ptr sym_lp, sym_List_Ptr sym_gp, bb_List_Ptr bbl);
    ~procedure() {}

    procedure& operator=(const procedure& rhs);

    sym_List_Ptr get_Local_Sym_List();
    bb_List_Ptr get_BB_List ();
    string get_Name();
    value_Type get_Value_Type(); 
    bb_Ptr get_Start_BB();

    /* General dump functions */
    void print_Proc(ostream *p);
    void print_Symtab(ostream *p);
    void print_AST(ostream *p);
    void print_Symtab_for_Eval(ostream * sym_fp);
    void print_Icode();

    /* Function for evaluation */
    eval_Result evaluate();

    /* Function for compilation */
    void compile();

    /* Functions for printing the generated code */
    void print_Assembly(ostream * opfp);
    void print_Prologue(ostream * opfp);
    void print_Epilogue(ostream * opfp);
};
    
class basic_Block
{
    int bb_num;
    ast_List_Ptr ast_list;
    icode_Stmt_List icode_list;

  public:
    basic_Block(int num, ast_List_Ptr asl);
    ~basic_Block() {}

    basic_Block& operator=(const basic_Block& rhs);

    int get_BB_Num();
    ast_List_Ptr get_Ast_List();

    void print_BB(ostream *p);
    void print_Icode(ostream * ic_fp);

    eval_Result evaluate();
    icode_Stmt_List compile();

};
