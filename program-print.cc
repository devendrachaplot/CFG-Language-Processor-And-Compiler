
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

static ostream * prog_fp = NULL;

/******************* Print functions for dumping the program read *********************/

void program::print_Program()
{
    bool show_program = cmd_options.show_Program();
    if (show_program)
    {
        prog_fp = cmd_options.prog_File();
        *prog_fp << "PROGRAM:\n Global Declarations:\n";

        if (global_sym_list)
            global_sym_list->print_Sym_Node_List(prog_fp);
        else
            *prog_fp << "None\n";

        *prog_fp << " List of Procedures:\n";

        map<string, proc_Ptr>::iterator i;
        for (i = proc_list->begin(); i != proc_list->end(); i++)
            i->second->print_Proc(prog_fp);  
    }
}

void program::print_Symtab()
{
    bool show_symtab = cmd_options.show_Symtab();
    if (show_symtab)
    {
        ostream * sym_fp = cmd_options.sym_File();
        *sym_fp << " Global Declarations: \n";

        if (global_sym_list)
            global_sym_list->print_Sym_Node_List(sym_fp);
        else
            *sym_fp << "None\n";

        map<string, proc_Ptr>::iterator i;
        for (i = proc_list->begin(); i != proc_list->end(); i++)
            i->second->print_Symtab(sym_fp);  
    }
}

void program::print_AST()
{
    bool show_ast = cmd_options.show_Ast();
    if (show_ast)
    {
        ostream * ast_fp = cmd_options.ast_File();

        map<string, proc_Ptr>::iterator i;
        for (i = proc_list->begin(); i != proc_list->end(); i++)
            i->second->print_AST(ast_fp);  
    }
}

void procedure::print_Proc(ostream * proc_fp)
{
    string type_name = "VOID";

    CHECK_INVARIANT (ret_val_type == void_Val, "In this version, only void functions are supported");
    *proc_fp << " Procedure: " << name << ", Return Type: " << type_name << "\n"; 
    *proc_fp << " Local Declarations \n";
    local_sym_list->print_Sym_Node_List(proc_fp);
    *proc_fp << " AST lists of Basic Blocks\n";

    list<bb_Ptr>::iterator i;
    for (i = bb_list->begin(); i != bb_list->end(); i++)
        (*i)->print_BB(proc_fp); 
}

void procedure::print_Symtab(ostream * sym_fp)
{

    *sym_fp << " Procedure: " << name << "\n"; 
    *sym_fp << " Local Declarations \n";
    local_sym_list->print_Sym_Node_List(sym_fp);
    *sym_fp << "\n";
}

void procedure::print_AST(ostream * ast_fp)
{
    *ast_fp << " Procedure: " << name << "\n"; 
    *ast_fp << " AST lists of Basic Blocks\n";

    list<bb_Ptr>::iterator i;
    for (i = bb_list->begin(); i != bb_list->end(); i++)
       (*i)->print_BB(ast_fp); 
}

void basic_Block::print_BB(ostream * bb_fp)
{
    *bb_fp << " Basic Block Number " << bb_num <<"\n";

    list <ast_Ptr>::iterator i;
    for (i = ast_list->begin(); i != ast_list->end(); i++)
       (*i)->print_Node(bb_fp); 
}


/******************* Print functions for evaluation *********************/

void procedure::print_Symtab_for_Eval(ostream * sym_fp)
{
    *sym_fp << " Values of Global Variables \n";
    global_sym_list->print_Sym_Node_List_for_Evaluation(sym_fp);

    *sym_fp << " Values of Local Variables \n";
    local_sym_list->print_Sym_Node_List_for_Evaluation(sym_fp);
}

/******************* Print functions for intermediate code *********************/

void procedure::print_Icode()
{
    if (cmd_options.show_IC())
    {
        ostream * ic_fp = cmd_options.ic_File();

        *ic_fp << " Procedure: " << name << "\n"; 
        *ic_fp << " Intermediate Code Statements\n";
        list <icode_Stmt_Ptr>::iterator i;
    
        for (i = icode_list.begin(); i != icode_list.end(); i++)
        {
            (*i)->print_Icode(ic_fp); 
        }
    }
}

/* 
   basic_Block::print_Icode method is not required in the 
   bigger scheme of things but has been retained because 
   it may be useful for debugging the later versions.
*/
void basic_Block::print_Icode(ostream * ic_fp)
{
    list <icode_Stmt_Ptr>::iterator i;

    for (i = icode_list.begin(); i != icode_list.end(); i++)
        (*i)->print_Icode(ic_fp); 
}

/******************* Print functions for assembly code *********************/

void program::print_Assembly()
{

    ostream * op_fp = cmd_options.output_File();

    /* print global variables */
    if (global_sym_list)
    {
        *op_fp << "\n\t.data\n";

        global_sym_list->print_Sym_for_Assembly(op_fp);
    }

    /* Print each procedure */
    map<string, proc_Ptr>::iterator i;
    for (i = proc_list->begin(); i != proc_list->end(); i++)
         i->second->print_Assembly(op_fp);  
}


void procedure::print_Assembly(ostream * op_fp)
{

    /* First print procedure prologue */
    print_Prologue(op_fp);

    /* Then print the body of the procedure */
    list <icode_Stmt_Ptr>::iterator i;

    for (i = icode_list.begin(); i != icode_list.end(); i++)
        (*i)->print_Assembly(op_fp); 

    /* Finally, print procedure epilogue */
    print_Epilogue(op_fp);
}


void procedure::print_Prologue(ostream * op_fp)
{

    stringstream prologue;

    prologue << "\n\
       .text  \t\t\t# The .text assembler directive indicates\n\
       .globl " << name <<"\t\t# the following is code (as oppose to data).\n";

    prologue << name <<":\t\t\t\t# .globl makes main known to the \n\t\t\t\t# outside of the program.\n\
# Prologue begins \n\
        sw  $fp, 0($sp)\t\t# Save the frame pointer \n\
        sub $fp, $sp, 4\t\t# Update the frame pointer\n";

        
    int size = local_sym_list->get_Size();

    if (size > 0 )
        prologue << "\tsub $sp, $sp, " << size << "\t# Make space for locals\n";
    else 
        CHECK_INVARIANT(SHOULD_NOT_REACH, "Size of activation record cannot be negative");

    prologue << "# Prologue ends\n\n";
   
    *op_fp  << prologue.str();
}

void procedure::print_Epilogue(ostream * op_fp)
{
    
    stringstream epilogue;

    int size = local_sym_list->get_Size();
  
    epilogue <<  "\n# Epilogue Begins\n\tadd $sp, $sp, " << size << "\n";

    epilogue << "\tlw $fp, 0($sp)  \n\tj        $31\t\t# Jump back to operating system.\n# Epilogue Ends\n\n";


    *op_fp  << epilogue.str();
}
