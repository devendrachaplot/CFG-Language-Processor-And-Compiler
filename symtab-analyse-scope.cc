
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

using namespace std;

#include "common-classes.hh"
#include "evaluate.hh"
#include "reg-alloc.hh"
#include "symtab.hh"
#include "ast.hh"
#include "support.hh"
#include "program.hh"


/************************************ symtab_In_Scope *******************************/

symtab_In_Scope::symtab_In_Scope ()                        
{ 
    global_sym_list = NULL;
    local_sym_list = NULL; 
    stack_top = empty;
}

symtab_Stack_Top symtab_In_Scope::get_Stack_Top()
{
    return stack_top;
}

void symtab_In_Scope::enter_Procedure_Name(string name, int lineno)
{
    if (stack_top == empty)
        global_sym_list = new sym_List;

    /* We need to create a sym_Entry for this name and 
       then we can enter it in the global symtab
    */
    sym_Entry_Ptr se_P = new sym_Entry_for_Proc(name, lineno);
    global_sym_list->insert_Sym_Entry(se_P);
}

void symtab_In_Scope::validate_Symtab_Consistency(string function_name)
{
    string local_mesg = "Inconsistent symtab with stack_top value `local' (function `" + function_name + "')";
    string global_mesg = "Inconsistent symtab with stack_top value `global' (function `" + function_name + "')";
    string empty_mesg = "Inconsistent symtab with stack_top value `empty' (function `" + function_name + "')";
    string none_mesg = "Inconsistent symtab with out of range stack_top value (function `" + function_name + "')";

    if (stack_top == local)
        CHECK_INVARIANT ((local_sym_list) && (global_sym_list), local_mesg)
    else if (stack_top == global)
        CHECK_INVARIANT ((local_sym_list==NULL) && (global_sym_list), global_mesg)
    else if (stack_top == empty)
        CHECK_INVARIANT ((local_sym_list==NULL) && (global_sym_list==NULL), empty_mesg)
    else
        CHECK_INVARIANT(SHOULD_NOT_REACH, empty_mesg)
}

int symtab_In_Scope::get_Line_Number(string name)
{
    sym_Entry_Ptr se_P = get_Sym_Entry(name);

    return se_P->get_Line_Number();
}

sym_Entry_Ptr symtab_In_Scope::get_Sym_Entry(string name)
{

    validate_Symtab_Consistency("get_Sym_Entry");

    /* This function is expected to be called only after checking 
       whether the name has been declared in the visible scope, 
       hence the sym Entry cannot be NULL.

       Note that the get_Sym_Entry function on individual lists 
       may return NULL.
    */

    bool found = false;

    CHECK_INVARIANT ((stack_top != empty), 
             "sym Entry extracted in empty symtab")

    sym_Entry_Ptr se_P = NULL;

    if (stack_top == local) 
        se_P = local_sym_list->get_Sym_Entry(name);

    if (se_P == NULL) 
        se_P = global_sym_list->get_Sym_Entry(name);
    CHECK_INVARIANT (se_P != NULL, 
           "sym Entry cannot be NULL")
    return se_P;
}

bool symtab_In_Scope::declared_In_Visible_Scope(string name, scope_for_Search scope)
{
    validate_Symtab_Consistency("declared_In_Visible_Scope");

    bool found = false;

    if (scope == symtab_Top)
    {
        /* 
           The motivation of this search is not to find out the type
           but to make sure that we avoid re-declaration.
    
           When we are inside a procedure (i.e. stack_top == local), we 
           should search for declaration of the given name only within 
           the procedure. If it has been declared at the global level 
           but not declared locally, we cannot consider the current 
           declaration as re-declaration.
         
           When we are outside a procedure (i.e. stack_top == global), 
           we must search in the global scope only. 
        */
        
    
        if (stack_top == local) 
            found = local_sym_list->find_Name(name);
        else if (stack_top == global) 
            found = global_sym_list->find_Name(name);
        else 
            CHECK_INVARIANT (stack_top==empty, "Stack top should be empty")
    }
    else
    {
        /* 
           Doesn't matter where it is declared. It could be only in the 
           local list, only in the global list, or both of them.
        */

        CHECK_INVARIANT(scope==anywhere, "The value of scope should have been `anywhere'")

        found = (local_sym_list) && (local_sym_list->find_Name(name))
                ||
                (global_sym_list) && (global_sym_list->find_Name(name));
    }
    return found;
}

entity_Type symtab_In_Scope::get_Entity_Type (string name, scope_for_Search scope)
{
    /* 
       See the commend in the beginning of the procedure above
       symtab_In_Scope::declared (string name, scope_for_Search scope).
    */
    validate_Symtab_Consistency("get_Entity_Type");

    entity_Type en_t;

    if (scope == symtab_Top)
    {
        if (stack_top == local) 
        {   if (local_sym_list->find_Name(name))
                en_t = local_sym_list->get_Entity_Type(name);
            else 
                 CHECK_INVARIANT(SHOULD_NOT_REACH, "Entity type should not be searched for a non-existent symbol")
        }
        else if (stack_top == global) 
        {    if (global_sym_list->find_Name(name))
                 en_t = global_sym_list->get_Entity_Type(name);
            else 
                 CHECK_INVARIANT(SHOULD_NOT_REACH, "Entity type should not be searched for a non-existent symbol")
        }
        else 
        {
            CHECK_INVARIANT(stack_top==empty, "Stack top should be empty")
            CHECK_INVARIANT(SHOULD_NOT_REACH, "Entity type should not be searched in a non-existent symbol table")
        }
    }
    else
    {
        /* In this case, we need to find en_t first in the local list */
        CHECK_INVARIANT(scope==anywhere, "The value of scope should have been `anywhere;")

        if ((local_sym_list) && (local_sym_list->find_Name(name)))
            en_t = local_sym_list->get_Entity_Type(name);
        else if ((global_sym_list) && (global_sym_list->find_Name(name)))
            en_t = global_sym_list->get_Entity_Type(name);
    }
    return en_t;
}

value_Type symtab_In_Scope::get_Value_Type (string name, scope_for_Search scope)
{
    /* 
       See the commend in the beginning of the procedure above
       symtab_In_Scope::declared (string name, scope_for_Search scope).
    */
    validate_Symtab_Consistency("get_Value_Type");

    value_Type val_t;

    if (scope == symtab_Top)
    {
        if (stack_top == local) 
        {   if (local_sym_list->find_Name(name))
                val_t = local_sym_list->get_Value_Type(name);
            else 
                 CHECK_INVARIANT(SHOULD_NOT_REACH, "Value type should not be searched for a non-existent symbol")
        }
        else if (stack_top == global) 
        {    if (global_sym_list->find_Name(name))
                 val_t = global_sym_list->get_Value_Type(name);
            else 
                 CHECK_INVARIANT(SHOULD_NOT_REACH, "Value type should not be searched for a non-existent symbol")
        }
        else 
        {
            CHECK_INVARIANT(stack_top==empty, "Stack top should be empty")
            CHECK_INVARIANT(SHOULD_NOT_REACH, "Value type should not be searched in a non-existent symbol table")
        }
    }
    else
    {
        /* In this case, we need to find val_t first in the local list */
        CHECK_INVARIANT(scope==anywhere, "The value of scope should have been `anywhere;")

        if ((local_sym_list) && (local_sym_list->find_Name(name)))
            val_t = local_sym_list->get_Value_Type(name);
        else if ((global_sym_list) && (global_sym_list->find_Name(name)))
            val_t = global_sym_list->get_Value_Type(name);
    }
    return val_t;
}

void symtab_In_Scope::allocate_Sym_List()
{
    validate_Symtab_Consistency("allocate_Sym_List");

    if (stack_top == empty)
    {
        global_sym_list = new sym_List();;
        stack_top = global;
    }
    else if (stack_top == global)
    {
        local_sym_list = new sym_List();;
        stack_top = local;
    }
    else if (stack_top == local)
        CHECK_INVARIANT(SHOULD_NOT_REACH, "Attempt to allocate sym_list in a full stack")
    else
        CHECK_INVARIANT(SHOULD_NOT_REACH, "Wrong value of stack_top")
}

void symtab_In_Scope::deallocate_Sym_List ()
{
    validate_Symtab_Consistency("deallocate_Sym_List");

    if (stack_top == empty)
        CHECK_INVARIANT(SHOULD_NOT_REACH, "Attempt to deallocate sym_list in an empty stack")
    else if (stack_top == global)
    {
        /* 
           We do not `delete' but merely set pointer to NULL because 
           the list is copied elsewhere. 

           We have overloaded the assignment operator `=' for the classes
           containing pointers. This ensures deep copy of objects. However,
           our sym_lists are not objects but pointers to objects. Thus they
           undergo a shallow copy.
        */
        global_sym_list = NULL;
        stack_top = empty;
    }
    else if (stack_top == local)
    {
        local_sym_list = NULL;
        stack_top = global;
    }
    else
        CHECK_INVARIANT(SHOULD_NOT_REACH, "Wrong value of stack_top")
}

void symtab_In_Scope::set_Symtab_Top_List (sym_List_Ptr sym_lp)
{
    validate_Symtab_Consistency("set_Symtab_Top_List");

    if (stack_top == empty)
    {
        global_sym_list = sym_lp;
        stack_top = global;
    }
    else if (stack_top == global)
    {
        local_sym_list = sym_lp;
        stack_top = local;
    }
    else if (stack_top == local)
        CHECK_INVARIANT(SHOULD_NOT_REACH, "Attempt to set top sym_list in a full stack")
    else
        CHECK_INVARIANT(SHOULD_NOT_REACH, "Wrong value of stack_top")
}

sym_List_Ptr symtab_In_Scope::get_Symtab_Top_List ()
{
    validate_Symtab_Consistency("get_Symtab_Top_List");

    if (stack_top == empty)
        CHECK_INVARIANT(SHOULD_NOT_REACH, "Attempt to get sym_list in an empty stack")
    else if (stack_top == global)
        return global_sym_list;
    else if (stack_top == local)
        return local_sym_list;
    else
        CHECK_INVARIANT(SHOULD_NOT_REACH, "Wrong value of stack_top")
}

sym_List_Ptr symtab_In_Scope::get_Symtab_Global_List ()
{
    validate_Symtab_Consistency("get_Symtab_Global_List");
    return global_sym_list;
}

sym_List_Ptr symtab_In_Scope::get_Symtab_Local_List ()
{
    validate_Symtab_Consistency("get_Symtab_Local_List");
    return local_sym_list;
}


symtab_In_Scope& symtab_In_Scope::operator=(symtab_In_Scope& rhs)
{
    global_sym_list = rhs.global_sym_list;
    local_sym_list = rhs.local_sym_list;
    return *this;
}

symtab_In_Scope * symtab_in_scope_P;

/******************************* symtab use for evaluation ****************************/

void sym_Entry_for_Int_Var::set_Value_of_Evaluation(eval_Result res)
{
    CHECK_INVARIANT (res.which_Result() == int_Res, "Integer object can get integer result only")
    set_Value(res.get_Int_Val());
    reset_Undefined_Status();
}

eval_Result sym_Entry_for_Int_Var::get_Value_of_Evaluation()
{
    string mesg = "Variable " + name + " has not been defined before its use";
    report_Violation_and_Abort(undefined==false, mesg);
    eval_Result res(value,NULL,int_Res);
    return res;
}
void sym_Entry_for_Float_Var::set_Value_of_Evaluation(eval_Result res)
{
    CHECK_INVARIANT (res.which_Result() == float_Res, "Integer object can get integer result only")
    set_Value(res.get_Float_Val());
    reset_Undefined_Status();
}

eval_Result sym_Entry_for_Float_Var::get_Value_of_Evaluation()
{
    string mesg = "Variable " + name + " has not been defined before its use";
    report_Violation_and_Abort(undefined==false, mesg);
    eval_Result res(value,NULL,float_Res);
    return res;
}
