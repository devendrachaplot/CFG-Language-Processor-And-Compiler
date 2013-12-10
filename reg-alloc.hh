
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

typedef enum
{
    none,                                       /* dummy to indicate no register */
    zero,         /* constant register */
    v0,                                         /* expression result register */
    v1,                                         /* function result register */
    a0, a1, a2, a3,                             /* argument register */
    t0, t1, t2, t3, t4, t5, t6, t7, t8, t9,     /* temporary caller-save registers */ 
    s0, s1, s2, s3, s4, s5, s6, s7,             /* temporary callee-save registers */
    f0, f2, f4, f6, f8, f10, f12, f14, f16,
    f18, f20, f22, f24, f26, f28, f30, f32,
    gp,                                         /* global data pointer register */
    sp,                                         /* stack pointer register */
    fp,                                         /* frame pointer register */
    ra                                          /* return address register */
} spim_Reg;

typedef enum { int_Num,float_Num } reg_Val_Type;

typedef enum { fixed_reg, gp_data, fn_result, argument, pointer, ret_address } reg_Use_Category;

class register_Descriptor
{
    spim_Reg reg_id;
    string reg_name;
    
    reg_Use_Category reg_use;  

  public:
    list<sym_Entry_Ptr> lra_sym_list;
    reg_Val_Type value_type;
    bool used_for_expr_result;
    register_Descriptor (spim_Reg reg, string s, reg_Val_Type vt, reg_Use_Category uc);
    ~register_Descriptor() {}

    bool is_Sym_List_Empty();
    void update_Sym_Info(sym_Entry_Ptr sep);

    bool find_Sym_Entry_in_List(sym_Entry_Ptr sep);
    void remove_Sym_Entry_from_List(sym_Entry_Ptr sep);

    reg_Use_Category get_Use_Category(); 
    spim_Reg get_Reg();

    string get_Name();
    void clear_lra_Sym_List();

    bool used_for_Expr_Result();
    void reset_Use_for_Expr_Result();

    bool is_Free();

    void print_Register_Descriptor();
};

typedef register_Descriptor * reg_Desc_Ptr;

typedef  map<spim_Reg, reg_Desc_Ptr> reg_Table;

extern reg_Table spim_reg_table;

void initialize_Reg_Table();

/*   
    The local register allocation (lra) restricts its attention to a basic block
    and maintains the following two pieces of information

    - for each name encountered, the register in which current value of name
      can be found 
    - for each register, the set of names whose values are held in the
      register (they all have the same value).

    At the start of a block each name has register "none" and the name list of
    each register is empty.
     Local register allocation should consider the following cases described 
     in terms of "source to destination" moves where source and destination
     could be memory (m), register (r), or constant (c).

     For each case we outline a step-wise decision process for identifying
     (a) The register, (b) Load, if any

     1. m2m is a typical assginment statemenmt involving memory on lhs
        as well as RHS. Decision steps
               if (rhs is in a register)
               {
                   use the register. 
                   no load required.
               }
               else if (lhs is in a register)
               {
                   use the register.
                   load rhs into the register.
               }
               else
               {
                   use a new register.
                   load rhs into the register.
               }
     2. m2r is a typical load for a compute statemenmt. As a policy
        we use a new register because the value may be updated by the
        computation. This can be minimized by traversing the tree using
        Sethi-Ullman lablelling, but we do not do it in this version.
               if (rhs is in a register)
               {
                   use a new register. 
                   move from source register into the new register.
               }
               else
               {
                   use a new register.
                   load rhs into the register.
               }
    3. r2m. This is a typical store for a compute statemenmt. It does not need a 
       new register 
    4. c2m. This is a store that needs an intermediate register.
               if (lhs is in a register)
               {
                   use the register. 
                   load the rhs constant into the register.
               }
               else
               {
                   use a new register.
                   load rhs constant into the new register.
               }
    5. c2r. This is typical load immediate for computation. 
               unconditionally
               {
                   use a new register.
                   load rhs constant into the new register.
               } 
    6. r2r does not need to bring in register allocation.

    Given the 3 candidates (m,r,c) and 2 positions (source and destination)
    we have 3^2 = 9 possibility. Apart from the 6 possibilities
    outlined above, the remaining three possibilities (m2c, r2c, c2c)
    do not arise on common architectures.


    In each of the above case, the following side-effects are noted:
    - the register of the destination mem location is updated to 
      the new register by removing the earlier register.
    - the destination is added to the name list of the new register. 

    An invariant of our policy is that that a for given location there 
    is only one register although for a given register, there may be 
    multiple locations. This happens because of a call to register 
    allocation 
    - NEVER adds a source location to any register, and
    - overwrites the previous register of the destination of the 
      new register.


    At the end of a block both these information are nullified by setting
    register to "none" for each name and by setting the name list of each 
    register to empty.
*/

typedef enum { m2m=1, m2r, r2m, c2m, c2r, r2r } lra_Scenario;

class lra_Outcome
{
    reg_Desc_Ptr reg_desc_ptr;

    bool is_a_new_reg;
    bool is_same_as_source;
    bool is_same_as_destination;
    bool reg_move_needed; 
    bool load_needed;

  public:
    lra_Outcome(reg_Desc_Ptr rdp, bool nr, bool sr, bool dr, bool mv, bool ld);
    ~lra_Outcome() {}
    
    reg_Desc_Ptr get_Reg();
    bool is_New_Reg();
    bool is_Source_Reg ();
    bool is_Dest_Reg();
    bool is_Mov_Needed();
    bool is_Load_Needed();
};

lra_Outcome allocate_Local_Registers(lra_Scenario lcase, ast_Ptr dest_mem, ast_Ptr source_mem);
reg_Desc_Ptr get_New_Reg();
reg_Desc_Ptr get_New_Reg(reg_Val_Type val);
reg_Desc_Ptr get_New_Reg(reg_Val_Type val, spim_Reg sreg);
void update_Reg_Sym_Mappings(reg_Desc_Ptr res_reg, sym_Entry_Ptr dest_sym);
void free_Reg(reg_Desc_Ptr dest_rdp, sym_Entry_Ptr dest_sym);
void validate_Init_Local_Reg_Mappings();
void clear_Local_Reg_Mappings();
