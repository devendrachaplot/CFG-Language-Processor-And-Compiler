
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
#include <sstream>
#include <string>
#include <vector>
#include <list>
#include <map>

using namespace std;

#include "common-classes.hh"
#include "evaluate.hh"
#include "support.hh"

#include "reg-alloc.hh"
#include "symtab.hh"
#include "ast.hh"
#include "program.hh"
#include "icode.hh"


/****************************** Class mem_Addr_Opd *****************************/

void mem_Addr_Opd::print_ICS_Opd(ostream * st) 
{
    string name = sym_entry->get_Name();

    *st << name;
}

void mem_Addr_Opd::print_Asm_Opd(ostream * st) 
{
    stringstream opd_string;

    sym_Scope sym_scope = sym_entry->get_Sym_Scope();
 
    CHECK_INVARIANT(sym_scope==is_Local || sym_scope==is_Global, "Wrong scope value")

    if (sym_scope == is_Local)
    {
        int offset = sym_entry->get_Start_Offset();
        opd_string << offset << "($fp)";
    }
    else
        opd_string << sym_entry->get_Name();
          
    *st << opd_string.str();
}

/****************************** Class reg_Addr_Opd *****************************/

void reg_Addr_Opd::print_ICS_Opd(ostream * st) 
{
 
    string name = reg_desc_ptr->get_Name();

    *st << name;
}

void reg_Addr_Opd::print_Asm_Opd(ostream * st) 
{
 
    string name = reg_desc_ptr->get_Name();

    *st << "$" << name;
}



/****************************** Class const_Opd *****************************/

void const_Opd::print_ICS_Opd(ostream * st) 
{
    *st << num;
}

void const_Opd::print_Asm_Opd(ostream * st) 
{
    *st << num;
}

/****************************** Class dconst_Opd *****************************/

void dconst_Opd::print_ICS_Opd(ostream * st) 
{
    *st << num;
}

void dconst_Opd::print_Asm_Opd(ostream * st) 
{
    *st << num;
}

/*************************** Class icode_Stmt *****************************/

void move_IC_Stmt::print_Icode(ostream *st)
{
    CHECK_INVARIANT (opd1, "Opd1 cannot be NULL for a move IC Stmt")
    CHECK_INVARIANT (result, "Result cannot be NULL for a move IC Stmt")
    CHECK_INVARIANT (op_desc_ptr, "Instruction descriptor format cannot be NULL for a move IC Stmt")

    string ops = op_desc_ptr->get_Name();

    icode_Format icf = op_desc_ptr->get_IC_Format();

    switch (icf)
    {
        case i_r_op_o1: 
                *st << " " << ops << ": ";
                result->print_ICS_Opd(st);
                *st << " <- ";
                opd1->print_ICS_Opd(st);
                *st << "\n";
            break; 
        default: CHECK_INVARIANT(SHOULD_NOT_REACH, "Intermediate code format not supported")
            break;
    }
}

void move_IC_Stmt::print_Assembly(ostream *st)
{
    CHECK_INVARIANT (opd1, "Opd1 cannot be NULL for a move IC Stmt")
    CHECK_INVARIANT (result, "Result cannot be NULL for a move IC Stmt")
    CHECK_INVARIANT (op_desc_ptr, "Instruction descriptor format cannot be NULL for a move IC Stmt")

    string ops = op_desc_ptr->get_Mnemonic();

    assembly_Format acf = op_desc_ptr->get_Assembly_Format();

    switch (acf)
    {
        case a_op_r_o1: 
                *st << "\t" << ops << ", ";
                result->print_Asm_Opd(st);
                *st << ", ";
                opd1->print_Asm_Opd(st);
                *st << "\n";
            break; 
        case a_op_o1_r: 
                *st << "\t" << ops << ", ";
                opd1->print_Asm_Opd(st);
                *st << ", ";
                result->print_Asm_Opd(st);
                *st << "\n";
            break; 
        default: CHECK_INVARIANT(SHOULD_NOT_REACH, "Intermediate code format not supported")
            break;
    }
}

/*************************** Class icode_Stmt *****************************/

void arith_IC_Stmt::print_Icode(ostream *st)
{
    CHECK_INVARIANT (opd1, "Opd1 cannot be NULL for a arith IC Stmt")
    CHECK_INVARIANT (opd2, "Opd2 cannot be NULL for a arith IC Stmt")
    CHECK_INVARIANT (op_desc_ptr, "Instruction descriptor format cannot be NULL for a arith IC Stmt")

    string ops = op_desc_ptr->get_Name();

    icode_Format icf = op_desc_ptr->get_IC_Format();

    switch (icf)
    {
        case i_r_o1_op_o2: 
                *st << " " << ops << ": ";
                result->print_ICS_Opd(st);
                *st << " <- ";
                opd1->print_ICS_Opd(st);
                *st << " , ";
                opd2->print_ICS_Opd(st);
                *st << "\n";
            break; 
        default: CHECK_INVARIANT(SHOULD_NOT_REACH, "Intermediate code format not supported")
            break;
    }
}

void arith_IC_Stmt::print_Assembly(ostream *st)
{
    
}
/******************************* Class code_for_Ast ****************************/

void code_for_Ast::print_Icode(ostream *st)
{
    list <icode_Stmt_Ptr>::iterator i;
    for (i = ics_List.begin(); i != ics_List.end(); i++)
    {
         CHECK_INVARIANT (*i, "List pointer seems to be NULL")
       	    (*i)->print_Icode(st); 
    }
}


