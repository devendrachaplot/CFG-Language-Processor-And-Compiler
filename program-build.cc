
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


using namespace std;

/* 
   This is the object which holds the entire program data at 
   the end of parsing. The action associated with the start 
   symbol of the grammar copies the data into this object.
*/
program_Ptr program_object_P;

/******************* Methods for class program *************************/

program::program(sym_List_Ptr sym_lp, proc_Map_Ptr pmp)
{
    global_sym_list = sym_lp;
    proc_list = pmp;
} 


program& program::operator=(const program& rhs)
{
    global_sym_list = rhs.global_sym_list;
    proc_list = rhs.proc_list;
    return *this;
}

/******************* Methods for class procedure *************************/

procedure::procedure(string s, sym_List_Ptr sym_lp, sym_List_Ptr sym_gp, bb_List_Ptr bbl)
{
    name = s;
    ret_val_type = void_Val;
    local_sym_list = sym_lp;
    global_sym_list = sym_gp;
    bb_list = bbl;
}

sym_List_Ptr procedure::get_Local_Sym_List() { return local_sym_list; }

bb_List_Ptr procedure::get_BB_List ()        { return bb_list; }

string procedure::get_Name()                 { return name; }

value_Type procedure::get_Value_Type()       { return ret_val_type; }

   
procedure& procedure::operator=(const procedure& rhs)
{
    name = rhs.name;
    ret_val_type = rhs.ret_val_type;
    local_sym_list = rhs.local_sym_list;
    bb_list = rhs.bb_list;
    return *this;
}
 
/******************* Methods for class basic_Block *************************/

basic_Block::basic_Block(int num, ast_List_Ptr asl)
{
    bb_num = num;
    ast_list = asl;
}

int basic_Block::get_BB_Num()            { return bb_num; }

ast_List_Ptr basic_Block::get_Ast_List() { return ast_list; }

basic_Block& basic_Block::operator=(const basic_Block& rhs)
{
    bb_num = rhs.bb_num;
    ast_list = rhs.ast_list;
    return *this;
}
