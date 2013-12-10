
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

/****************************** Class ics_Opd *****************************/

opd_Category ics_Opd::get_Opd_Category() { return type;} 

reg_Desc_Ptr ics_Opd::get_Reg()              
{
    CHECK_INVARIANT(SHOULD_NOT_REACH, "get_Reg method should not be called for a non-reg operand")
}

sym_Entry_Ptr ics_Opd::get_Sym_Entry() 
{
    CHECK_INVARIANT(SHOULD_NOT_REACH, "get_Sym_Entry method should not be called for a non-address operand")
}

int ics_Opd::get_Num() 
{
    CHECK_INVARIANT(SHOULD_NOT_REACH, "get_Num method should not be called for a non-constant operand")
}

double ics_Opd::get_Num2() 
{
    CHECK_INVARIANT(SHOULD_NOT_REACH, "get_Num2 method should not be called for a non-constant operand")
}
/****************************** Class mem_Addr_Opd *****************************/

mem_Addr_Opd::mem_Addr_Opd(sym_Entry_Ptr se_P) 
{
   sym_entry = se_P;
   type = mem_Addr;
}

sym_Entry_Ptr mem_Addr_Opd::get_Sym_Entry() 
{
    CHECK_INVARIANT(sym_entry, "Sym Entry of an address in an instruction cannot be NULL")
    return sym_entry;
}

mem_Addr_Opd& mem_Addr_Opd::operator=(const mem_Addr_Opd& rhs)
{
    type = rhs.type;     
    sym_entry = rhs.sym_entry;
    return *this;
}

/****************************** Class reg_Addr_Opd *****************************/

reg_Addr_Opd::reg_Addr_Opd(reg_Desc_Ptr reg) 
{
   type = reg_Addr;
   reg_desc_ptr = reg;
}

reg_Desc_Ptr reg_Addr_Opd::get_Reg()    { return reg_desc_ptr; }

reg_Addr_Opd& reg_Addr_Opd::operator=(const reg_Addr_Opd& rhs)
{
    type = rhs.type;     
    reg_desc_ptr = rhs.reg_desc_ptr ;
    return *this;
}


/****************************** Class const_Opd *****************************/

const_Opd::const_Opd(int n) 
{
   type = constant;
   num = n;
}

int const_Opd::get_Num()     { return num; }

const_Opd& const_Opd::operator=(const const_Opd& rhs)
{
    type = rhs.type;     
    num = rhs.num;
    return *this;
}

/****************************** Class dconst_Opd *****************************/

dconst_Opd::dconst_Opd(double n) 
{
   type = dconstant;
   num = n;
}

double dconst_Opd::get_Num2()     { return num; }

dconst_Opd& dconst_Opd::operator=(const dconst_Opd& rhs)
{
    type = rhs.type;     
    num = rhs.num;
    return *this;
}
/****************************** Class icode_stmt *****************************/

inst_Desc_Ptr icode_Stmt::get_Operator()        { return op_desc_ptr; }

ics_Opd_Ptr icode_Stmt::get_Opd1()
{
    CHECK_INVARIANT(SHOULD_NOT_REACH, "virtual method get_Opd1 not implemented")
}

ics_Opd_Ptr icode_Stmt::get_Opd2()
{
    CHECK_INVARIANT(SHOULD_NOT_REACH, "virtual method get_Opd2 not implemented")
}

ics_Opd_Ptr icode_Stmt::get_Result()
{
    CHECK_INVARIANT(SHOULD_NOT_REACH, "virtual method get_Result not implemented")
}

void icode_Stmt::set_Opd1(ics_Opd_Ptr io_P)
{
    CHECK_INVARIANT(SHOULD_NOT_REACH, "virtual method set_Opd1 not implemented")
}

void icode_Stmt::set_Opd2(ics_Opd_Ptr io_P)
{
    CHECK_INVARIANT(SHOULD_NOT_REACH, "virtual method set_Opd2 not implemented")
}

void icode_Stmt::set_Result(ics_Opd_Ptr io_P)
{
    CHECK_INVARIANT(SHOULD_NOT_REACH, "virtual methos set_Result not implemented")
}



/*************************** Class move_IC_Stmt *****************************/

move_IC_Stmt::move_IC_Stmt(tgt_Op op, ics_Opd_Ptr o1, ics_Opd_Ptr res)
{ 
    op_desc_ptr = spim_inst_table[op];
    opd1 = o1;   
    result = res; 
}

ics_Opd_Ptr move_IC_Stmt::get_Opd1()          { return opd1; }
ics_Opd_Ptr move_IC_Stmt::get_Result()        { return result; }

void move_IC_Stmt::set_Opd1(ics_Opd_Ptr io_P)   { opd1 = io_P; }
void move_IC_Stmt::set_Result(ics_Opd_Ptr io_P) { result = io_P; }

move_IC_Stmt& move_IC_Stmt::operator=(const move_IC_Stmt& rhs)
{
    op_desc_ptr = rhs.op_desc_ptr;         
    opd1 = rhs.opd1;   
    result = rhs.result; 
    return *this;
}

/*************************** Class arith_IC_Stmt *****************************/

arith_IC_Stmt::arith_IC_Stmt(tgt_Op op, ics_Opd_Ptr o1,ics_Opd_Ptr o2, ics_Opd_Ptr res)
{ 
    op_desc_ptr = spim_inst_table[op];
    opd1 = o1;   
    opd2 = o2;
    result = res; 
}

ics_Opd_Ptr arith_IC_Stmt::get_Opd1()          { return opd1; }
ics_Opd_Ptr arith_IC_Stmt::get_Opd2()          { return opd2; }
ics_Opd_Ptr arith_IC_Stmt::get_Result()        { return result; }

void arith_IC_Stmt::set_Opd1(ics_Opd_Ptr io_P)   { opd1 = io_P; }
void arith_IC_Stmt::set_Opd2(ics_Opd_Ptr io_P)   { opd2 = io_P; }
void arith_IC_Stmt::set_Result(ics_Opd_Ptr io_P) { result = io_P; }

arith_IC_Stmt& arith_IC_Stmt::operator=(const arith_IC_Stmt& rhs)
{
    op_desc_ptr = rhs.op_desc_ptr;         
    opd1 = rhs.opd1;   
    opd2 = rhs.opd2;
    result = rhs.result; 
    return *this;
}

/******************************* Class code_for_Ast ****************************/

code_for_Ast::code_for_Ast(reg_Desc_Ptr reg, icode_Stmt_List ic_L)
{
    ics_List = ic_L;
    result_reg = reg;
}

void code_for_Ast::append_ICS(icode_Stmt_Ptr ics_P)
{
    ics_List.push_back(ics_P);

    ics_Opd_Ptr result = ics_P->get_Result();

    result_reg = result->get_Reg();
}

icode_Stmt_List code_for_Ast::get_Icode()  { return ics_List; }

code_for_Ast& code_for_Ast::operator=(const code_for_Ast& rhs)
{
    ics_List = rhs.ics_List;
    result_reg = rhs.result_reg;
    return *this;
}

/************************ class instruction_Descriptor ********************************/

tgt_Op instruction_Descriptor::get_Operator()                   { return inst_op; }
string instruction_Descriptor::get_Name()                       { return name; }
string instruction_Descriptor::get_Mnemonic()                   { return mnemonic; }
string instruction_Descriptor::get_IC_Symbol()                  { return ic_symbol; }
icode_Format instruction_Descriptor::get_IC_Format()            { return ic_format; }
assembly_Format instruction_Descriptor::get_Assembly_Format()   { return a_format; }

void instruction_Descriptor::print_Instruction_Descriptor()
{
    string af, icf, ops;

    switch (a_format)
    {
        case a_op_r_o1: af = "a_op_r_o1"; break; 
        default: CHECK_INVARIANT(SHOULD_NOT_REACH, "Assembly format not supported") break;
    }

    switch (ic_format)
    {
        case i_r_op_o1: icf = "i_op_r_o1"; break; 
        default: CHECK_INVARIANT(SHOULD_NOT_REACH, "Intermediate code format not supported") break;
    }

    switch(inst_op)
    {
        case load : ops = "load"; break;
        case store : ops = "store"; break;
        case imm_Load : ops = "imm_Load"; break;
        default: CHECK_INVARIANT(SHOULD_NOT_REACH, "Target operator not supported") break;
    }
    cout << "Operator " << ops << " Mnemonic " << mnemonic << " IC symbol " 
         << ic_symbol << " IC format " << icf << " Assembly format " << af;
}

instruction_Descriptor::instruction_Descriptor (tgt_Op op, string nm, string mnc, 
                                  string ics, icode_Format icf, assembly_Format af)
{
    inst_op = op;
    name = nm; 
    mnemonic = mnc;
    ic_symbol = ics;
    ic_format = icf;
    a_format = af;
}

/******************************* global functions and data for instructions **********************************/

void initialize_Inst_Table()
{
    spim_inst_table[load] = 		
          new instruction_Descriptor(load,	"load", 	"lw", "",	i_r_op_o1, 	a_op_r_o1);
    spim_inst_table[load_d] =     
          new instruction_Descriptor(store_d, "load.d",   "lw", "", i_r_op_o1,  a_op_r_o1);
    spim_inst_table[imm_Load] =   
          new instruction_Descriptor(imm_Load,  "iLoad",  "li", "", i_r_op_o1,  a_op_r_o1);
    spim_inst_table[imm_Load_d] = 	
          new instruction_Descriptor(imm_Load_d,	"iLoad.d",	"li", "",	i_r_op_o1, 	a_op_r_o1);
    spim_inst_table[store] = 		
          new instruction_Descriptor(store,	"store", 	"sw", "", 	i_r_op_o1, 	a_op_o1_r);
    spim_inst_table[store_d] =     
          new instruction_Descriptor(load_d, "store.d",  "sw", "",   i_r_op_o1,  a_op_o1_r);
    spim_inst_table[add] =   
          new instruction_Descriptor(add,  "add",  "add", "", i_r_o1_op_o2,  a_op_o1_o2_r);
    spim_inst_table[add_d] =   
          new instruction_Descriptor(add_d,  "add.d",  "add", "", i_r_o1_op_o2,  a_op_o1_o2_r);
    spim_inst_table[sub] =   
          new instruction_Descriptor(add,  "sub",  "sub", "", i_r_o1_op_o2,  a_op_o1_o2_r);
    spim_inst_table[sub_d] =   
          new instruction_Descriptor(add,  "sub.d",  "sub", "", i_r_o1_op_o2,  a_op_o1_o2_r);
    spim_inst_table[mult] =   
          new instruction_Descriptor(add,  "mult",  "mult", "", i_r_o1_op_o2,  a_op_o1_o2_r);
    spim_inst_table[mult_d] =   
          new instruction_Descriptor(add,  "mult.d",  "mult", "", i_r_o1_op_o2,  a_op_o1_o2_r);
    spim_inst_table[divv] =   
          new instruction_Descriptor(add,  "div",  "div", "", i_r_o1_op_o2,  a_op_o1_o2_r);
    spim_inst_table[divv_d] =   
          new instruction_Descriptor(add,  "div.d",  "div", "", i_r_o1_op_o2,  a_op_o1_o2_r);
    spim_inst_table[uminus] =   
          new instruction_Descriptor(add,  "uminus",  "uminus", "", i_r_op_o1,  a_op_r_o1);
    spim_inst_table[uminus_d] =   
          new instruction_Descriptor(add,  "uminus.d",  "uminus", "", i_r_op_o1,  a_op_r_o1);    
    
}

void print_Inst_Table()
{
    map<tgt_Op, inst_Desc_Ptr>::iterator i;
    for (i = spim_inst_table.begin(); i != spim_inst_table.end(); i++)
            i->second->print_Instruction_Descriptor();
}

inst_Table spim_inst_table;

