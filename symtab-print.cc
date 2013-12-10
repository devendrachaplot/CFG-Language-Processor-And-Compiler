
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
#include <iomanip>
using namespace std;

#include "common-classes.hh"
#include "evaluate.hh"
#include "reg-alloc.hh"
#include "symtab.hh"
#include "ast.hh"
#include "support.hh"
#include "program.hh"

void sym_Entry_for_Int_Var::print_Sym_Entry_Details(ostream * sym_fp)
{

    string k_mesg = "The entity type of name " + name + " should have been variable";
    CHECK_INVARIANT (en_type == entity_Var, k_mesg)
    const string en_type_name = "VAR";

    string t_mesg = "The type of entity_Var " + name + " should have been int_Val";
    CHECK_INVARIANT (type == int_Val, t_mesg)
    const string type_name = "INT";

    if(typ==0)
    {
		*sym_fp << " Name: " << name << " Type: " << type_name << " Entity Type: " << en_type_name;
		if (start_offset == end_offset)
		    *sym_fp << " (No offset assigned yet)\n"; 
		else
		    *sym_fp << " Start Offset: " << start_offset << " End Offset: " << end_offset << "\n"; 
	}
}

void sym_Entry_for_Float_Var::print_Sym_Entry_Details(ostream * sym_fp)
{

    string k_mesg = "The entity type of name " + name + " should have been variable";
    CHECK_INVARIANT (en_type == entity_Var, k_mesg)
    const string en_type_name = "VAR";

    string t_mesg = "The type of entity_Var " + name + " should have been float_Val";
    CHECK_INVARIANT (type == float_Val, t_mesg)
    const string type_name = "FLOAT";

    if(typ==0)
    {
		*sym_fp << " Name: " << name << " Type: " << type_name << " Entity Type: " << en_type_name;
		if (start_offset == end_offset)
			*sym_fp << " (No offset assigned yet)\n"; 
		else
			*sym_fp << " Start Offset: " << start_offset << " End Offset: " << end_offset << "\n"; 
	}
}

void sym_List::print_Sym_Node_List(ostream * sym_fp)
{ 
    map<string, sym_Entry_Ptr>::iterator i;

    for (i = sym_list.begin(); i != sym_list.end(); i++)
        i->second->print_Sym_Entry_Details(sym_fp); 
}

void sym_Entry_for_Proc::print_Sym_Entry_Details(ostream * sym_fp)
{

    string mesg = "Wrong value of entity type field for a symbol entry representing a procedure " + name;
    CHECK_INVARIANT (en_type == entity_Proc, mesg)
    const string en_type_name = "FUNC";

    const string type_name = (type == int_Val)? "INT" : ((type == float_Val)? "FLOAT" : "VOID");

    *sym_fp << " Name: " << name << " Type: " << type_name << " Entity Type: " << en_type_name << "\n"; 
}

void sym_List::print_Sym_Node_List_for_Evaluation(ostream * sym_fp)
{ 
    map<string, sym_Entry_Ptr>::iterator i;

    for (i = sym_list.begin(); i != sym_list.end(); i++)
    {
        /* Check if the sym list entry is a name and then print it */
        sym_Entry_Ptr se_P = i->second;

        if (se_P->get_Entity_Type() == entity_Var)
            se_P->print_Sym_Entry_Eval_Details(sym_fp); 
    }
}

void sym_Entry_for_Int_Var::print_Sym_Entry_Eval_Details(ostream * sym_fp)
{

    string k_mesg = "The entity type of name " + name + " should have been variable";
    CHECK_INVARIANT (en_type == entity_Var, k_mesg)
    const string en_type_name = "VAR";

    string t_mesg = "The type of entity_Var " + name + " should have been int_Val";
    CHECK_INVARIANT (type == int_Val, t_mesg)
    const string type_name = "INT";
    
    stringstream ss;
    if (undefined)
        ss << "Undefined";
    else
        ss << value;

    if(typ==0)
    	*sym_fp << " Name: " << name << " Value " << ss.str() << "\n";
}

void sym_Entry_for_Float_Var::print_Sym_Entry_Eval_Details(ostream * sym_fp)
{

    string k_mesg = "The entity type of name " + name + " should have been variable";
    CHECK_INVARIANT (en_type == entity_Var, k_mesg)
    const string en_type_name = "VAR";

    string t_mesg = "The type of entity_Var " + name + " should have been float_Val";
    CHECK_INVARIANT (type == float_Val, t_mesg)
    const string type_name = "FLOAT";
    
    stringstream ss;
    if (undefined)
        ss << "Undefined";
    else{
    	ss.setf(ios::fixed, ios::floatfield);
		ss.precision(6);
        ss << value;
	}
	(*sym_fp).setf(ios::fixed, ios::floatfield);
	(*sym_fp).precision(6);
	if(typ==0)
	{
		*sym_fp << fixed;
    	*sym_fp << " Name: " << name << " Value " << ss.str() << "\n";
	}
}

/**************************** functions for generating assembly ******************/

void sym_List::print_Sym_for_Assembly(ostream * sym_fp)
{ 
    map<string, sym_Entry_Ptr>::iterator i;

    for (i = sym_list.begin(); i != sym_list.end(); i++)
    {
        /* Check if the sym list entry is a name and then print it */
        sym_Entry_Ptr se_P = i->second;

        if (se_P->get_Entity_Type() == entity_Var)
            se_P->print_Sym_for_Assembly(sym_fp); 
    }
}

void sym_Entry_for_Int_Var::print_Sym_for_Assembly(ostream * sym_fp)
{

    string k_mesg = "The entity type of name " + name + " should have been variable";
    CHECK_INVARIANT (en_type == entity_Var, k_mesg)
    const string en_type_name = "VAR";

    string t_mesg = "The type of entity_Var " + name + " should have been int_Val";
    CHECK_INVARIANT (type == int_Val, t_mesg)
    const string type_name = "INT";

    CHECK_INVARIANT(sym_scope == is_Global, "Global scope value expected")
    *sym_fp << name << ":\t.word 0\n";
}

void sym_Entry_for_Float_Var::print_Sym_for_Assembly(ostream * sym_fp)
{

    string k_mesg = "The entity type of name " + name + " should have been variable";
    CHECK_INVARIANT (en_type == entity_Var, k_mesg)
    const string en_type_name = "VAR";

    string t_mesg = "The type of entity_Var " + name + " should have been float_Val";
    CHECK_INVARIANT (type == float_Val, t_mesg)
    const string type_name = "FLOAT";

    CHECK_INVARIANT(sym_scope == is_Global, "Global scope value expected")
    *sym_fp << name << ":\t.word 0\n";
}

void sym_Entry::print_Sym_for_Assembly(ostream * sym_fp)
{
    CHECK_INVARIANT (SHOULD_NOT_REACH, "Symbol does not qualify as global data in assembly output")
}
