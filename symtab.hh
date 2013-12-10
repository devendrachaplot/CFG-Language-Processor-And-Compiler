
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

#include <list>
#include <map>

using namespace std;

/*
   This file defines class sym_List which is a map from
   symbol names to pointers to sym_Entry for the symbols.

   sym_Entry is an abstract base class which is derived
   independently for symbol names representing variables
   and symbol names representing procedures.

   Finally, this file also defines a data structure to
   hold symbol table during parsing. This essential because
   while processing a function f, the local variables of
   function g should not be visible. Thus during the parsing
   the visible symbol table changes. 
*/

class sym_List;
class sym_Entry;
class symtab_In_Scope;

typedef enum {entity_Var=1, entity_Proc, entity_Arith, entity_Num} entity_Type;
typedef enum {int_Val=1, void_Val, float_Val } value_Type;
typedef enum {symtab_Top=1, anywhere} scope_for_Search;
typedef enum {empty=0, global, local} symtab_Stack_Top;
typedef enum {is_Global, is_Local} sym_Scope;

typedef sym_Entry * sym_Entry_Ptr;
typedef sym_List * sym_List_Ptr;
typedef map <string,sym_Entry_Ptr> sym_Entry_List; 


class sym_Entry
{
    /* 
       Abstract base class for storing information about names
       appearing in a program. We derive two classes: one for 
       integer variables and the other for procedure names. 
       Later we will need to store variables of other types too.
    */
  public:
  	ast_Ptr node;
  	int typ; //for ID, EXP, ARTIFICIAL
    string   name;
    entity_Type en_type;
    value_Type type;
    int lineno;      /* line number of declaration is used in error reporting */
   
    sym_Entry() {}
    virtual ~sym_Entry() { }

    string get_Name();

    virtual sym_Scope get_Sym_Scope();
    int get_Line_Number ();

    virtual entity_Type get_Entity_Type() = 0;
    virtual value_Type get_Value_Type() = 0;

    /* Functions required for evaluation */
    virtual void set_Value(int num) ;
    virtual void set_Value(double num) {}
    virtual int get_Value() ;
    virtual double get_Value2() {}
    virtual void set_Value_of_Evaluation(eval_Result res) ;
    virtual eval_Result get_Value_of_Evaluation() ;
    virtual void print_Sym_Entry_Eval_Details(ostream * sym_fp) {}

    /* Functions required for compilation */
    virtual void set_Start_Offset(int num) ;
    virtual int get_Start_Offset() ;
    virtual void set_End_Offset(int num) ;
    virtual int get_End_Offset() ;
    virtual void set_Reg(reg_Desc_Ptr reg_desc_ptr);
    virtual reg_Desc_Ptr get_Reg();
    virtual void print_Sym_for_Assembly(ostream * sym_fp);

    /* General dumping functions */ 
    virtual void print_Sym_Entry_Details(ostream *p) = 0;

};

class sym_Entry_for_Int_Var : public sym_Entry
{
    /* data members required for compilation */
    int start_offset;    
    int end_offset;
    sym_Scope sym_scope;
    reg_Desc_Ptr reg_desc_ptr;

    /* data members required for evaluation */
    int value;	          
    bool undefined;   /* A local variable gets defined when a value is assigned to it */

  public:
    sym_Entry_for_Int_Var(string name, int line);
    sym_Entry_for_Int_Var(string name, int line, int typ);
    ~sym_Entry_for_Int_Var() {}

    value_Type get_Value_Type();
    entity_Type get_Entity_Type();

    /* Functions required for compilation */
    void set_Start_Offset(int num);
    int get_Start_Offset();

    void set_Reg(reg_Desc_Ptr reg_desc_ptr);
    reg_Desc_Ptr get_Reg();

    int get_End_Offset();
    void set_End_Offset(int num);

    sym_Scope get_Sym_Scope();

    void print_Sym_for_Assembly(ostream * sym_fp);

    /* Functions required for evaluation */
    void set_Value(int num);
    int get_Value();

    bool is_Undefined();            
    void reset_Undefined_Status();

    void set_Value_of_Evaluation(eval_Result res);
    eval_Result get_Value_of_Evaluation();

    void print_Sym_Entry_Eval_Details(ostream * sym_fp);

    /* General dump functions */
    void print_Sym_Entry_Details(ostream *p);


};

class sym_Entry_for_Float_Var : public sym_Entry
{
    /* data members required for compilation */
    int start_offset;    
    int end_offset;
    sym_Scope sym_scope;
    reg_Desc_Ptr reg_desc_ptr;

    /* data members required for evaluation */
    double value;	          
    bool undefined;   /* A local variable gets defined when a value is assigned to it */

  public:
    sym_Entry_for_Float_Var(string name, int line);
    sym_Entry_for_Float_Var(string name, int line,int typ);
    ~sym_Entry_for_Float_Var() {}

    value_Type get_Value_Type();
    entity_Type get_Entity_Type();

    /* Functions required for compilation */
    void set_Start_Offset(int num);
    int get_Start_Offset();

    void set_Reg(reg_Desc_Ptr reg_desc_ptr);
    reg_Desc_Ptr get_Reg();

    int get_End_Offset();
    void set_End_Offset(int num);

    sym_Scope get_Sym_Scope();

    void print_Sym_for_Assembly(ostream * sym_fp);

    /* Functions required for evaluation */
    void set_Value(double num);
    double get_Value2();

    bool is_Undefined();            
    void reset_Undefined_Status();

    void set_Value_of_Evaluation(eval_Result res);
    eval_Result get_Value_of_Evaluation();

    void print_Sym_Entry_Eval_Details(ostream * sym_fp);

    /* General dump functions */
    void print_Sym_Entry_Details(ostream *p);


};


class sym_Entry_for_Proc : public sym_Entry
{
    sym_List_Ptr sym_lp;

  public:
    sym_Entry_for_Proc(string name, int line);
    ~sym_Entry_for_Proc() {}

    sym_Entry_for_Proc& operator=(sym_Entry_for_Proc& rhs);

    entity_Type get_Entity_Type();
    value_Type get_Value_Type();
    
    /* General dump function */
    virtual void print_Sym_Entry_Details(ostream *p);
};


class sym_List 
{
    
    int start_offset_of_first_sym;
    int size_in_bytes;

  public:
    sym_Entry_List sym_list;
    sym_List(); 
    ~sym_List() {}

    sym_List& operator=(sym_List& rhs);

    void insert_Sym_Entry (sym_Entry_Ptr se);
    bool find_Name (string name);
    sym_Entry_Ptr get_Sym_Entry (string name);
    entity_Type get_Entity_Type (string name);
    value_Type get_Value_Type (string name);


    /* Functions required for evaluation */
    void set_Int_Val (string name, int value);
    void set_Float_Val (string name, double value);
    int get_Int_Val (string name);
    double get_Float_Val (string name);
    void print_Sym_Node_List_for_Evaluation(ostream * sym_fp);

    /* Functions required for compilation */
    void set_Start_Offset_of_First_Sym (int n);
    int get_Start_Offset_of_First_Sym ();    

    void assign_Offsets();
    int get_Size();      
    void set_Size(int n);

    void print_Sym_for_Assembly(ostream * sym_fp);

    /* General dump function */
    void print_Sym_Node_List(ostream *p);

};

/*  
   An object of class symtab_In_Scope is a data structure that 
   is required only during the parsing of a procedure to find 
   out the symbols visible in the scope and their type. 
  
   There are two pieces of information: 
   - The global symbol table which is always visible. 
   - A local symbol table for a given procedure. 

   Note that if we allow nest compound statements, we will
   need a stack of symbol tables with the global symbol table
   at the bottom of the stack. However, our input is the cfg 
   dump produced by GCC in which all nestings are opened out
   into a flat structure. Hence a combinaton of global and
   local symbol tables serve our purpose.

   Once the parsing is over, the symbol tables are available
   in a persistent repository through program object (global 
   variables) and procedure object (local variables).
*/


class symtab_In_Scope
{
    sym_List_Ptr global_sym_list;
    sym_List_Ptr local_sym_list;
    symtab_Stack_Top stack_top;

/*     
    For our input program a symtab_In_Scope object can 
    be in one of the following three configurations as 
    far as the visible scope is concerned.

                                          Top -> | Local  |    
                                                 ---------- 
        |        |    Top -> | Global |          | Global |       
 Top -> ---------             ---------          ---------- 
          Empty                

     The stack_top variable remembers this configuration.
     It is used to direct searches in the symtab.
*/

  public:
    symtab_In_Scope(); 
    ~symtab_In_Scope() {}

    symtab_In_Scope& operator=(symtab_In_Scope& rhs);

    void allocate_Sym_List();
    void deallocate_Sym_List ();

    void set_Symtab_Top_List (sym_List_Ptr sym_lp);
    sym_List_Ptr get_Symtab_Top_List();
    sym_List_Ptr get_Symtab_Global_List();
    sym_List_Ptr get_Symtab_Local_List();

    bool declared_In_Visible_Scope (string name, scope_for_Search scope);
    sym_Entry_Ptr get_Sym_Entry (string name);
    int get_Line_Number(string name);

    entity_Type get_Entity_Type (string name, scope_for_Search scope);
    value_Type get_Value_Type (string name, scope_for_Search scope);

    void validate_Symtab_Consistency(string function_name);

    /* 
       Right now we are not insisting on a prototype
       declaration of procedures. Hence we need to enter
       a procedure name explicitly in the global symtab.
       Otherwise it will not be entered and hence will not
       be available which processing procedure calls in a
       future version.
    */
    void enter_Procedure_Name(string name, int line);

    symtab_Stack_Top get_Stack_Top();
};

extern symtab_In_Scope * symtab_in_scope_P;

