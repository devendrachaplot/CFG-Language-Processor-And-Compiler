
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
#include <string>
#include <list>
#include <vector>
#include <map>
#include <algorithm>

using namespace std;

#include "common-classes.hh"
#include "evaluate.hh"
#include "support.hh"
#include "reg-alloc.hh"
#include "symtab.hh"
#include "ast.hh"
#include "program.hh"
#include "options.hh"
#include "icode.hh"


static ostream * compile_fp = NULL;

void program::compile()
{
     
    /* Set up the machine details */
    initialize_Reg_Table();
    initialize_Inst_Table();

    /* We need to assign offsets to global variables */
    global_sym_list->set_Start_Offset_of_First_Sym(0);
    global_sym_list->set_Size(0);
    global_sym_list->assign_Offsets();

   /* Compile the program by visiting each procedure */
    map<string, proc_Ptr>::iterator i;
    for (i = proc_list->begin(); i != proc_list->end(); i++)
    {
         i->second->compile();  
         i->second->print_Icode();
    }

    /* Emity assembly program */

    print_Assembly();
}

void procedure::compile()
{

    /* We need to assign offsets to local variables */
    local_sym_list->set_Start_Offset_of_First_Sym(4);
    local_sym_list->set_Size(4);
    local_sym_list->assign_Offsets();

   /* Compile the program by visiting each basic block */
    list<bb_Ptr>::iterator i;
    for (i = bb_list->begin(); i != bb_list->end(); i++)
    { 
        bb_Ptr bbp = *i;

        icode_Stmt_List bb_code = bbp->compile(); 
     
        if (icode_list.empty())
            icode_list = bb_code;
        else 
            icode_list.splice(icode_list.end(), bb_code);
    }
}


icode_Stmt_List basic_Block::compile()
{
    ast_Code_Ptr ast_code = NULL;

    validate_Init_Local_Reg_Mappings();

   /* Compile the program by visiting each AST in the block */
    list <ast_Ptr>::iterator i;
    for (i = ast_list->begin(); i != ast_list->end(); i++)
    {
        ast_Ptr ast = *i;
        if (ast->get_Tree_Op() != ret)
        {
             /* Right now there is no code corresponding to Ret Node */
             ast_code = ast->compile(); 
             if (ast_code)
             {
                 icode_Stmt_List ic_ast = ast_code->get_Icode();
                 if (ic_ast.empty() == false)
                 { 
                     if (icode_list.empty())
                         icode_list = ic_ast;
                     else 
                         icode_list.splice(icode_list.end(), ic_ast);
                 }
             }
        }
    }
    clear_Local_Reg_Mappings();

    return icode_list;
}

