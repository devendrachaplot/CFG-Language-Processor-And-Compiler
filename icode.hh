
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


/* 
   This file defines classes for intermediate form of the code generated 
   by our compiler. It also defined classes for machine instructions.

   An intermediate code (IC) statement consists of an instruction,
   possibly two operands and a result.
*/

typedef enum 
{                /* a: assembly format; r: result; o1: opd1; o2: opd2; op: operator */
   a_op,         /* Only the operator, no operand */
   a_op_o1,      /* Only one operand, no result, eg. goto L */
   a_op_r,       /* Only the result. Operand implicit? */ 
   a_op_r_o1,    /* r <- o1 */
   a_op_o1_r,    /* r <- o1 */
   a_op_r_r_o1,  /* r <- r op o1 */
   a_op_r_o1_o2, /* r <- o1 op o2 */ 
   a_op_o1_o2_r, /* r <- o1 op o2 */
   a_nsy         /* not specified yet */
} assembly_Format;

typedef enum 
{                /* i: intermediate format; r: result; o1: opd1; o2: opd2; op: operator */
   i_op,         /* Only the operator, no operand */
   i_op_o1,      /* Only one operand, no result, eg. goto L */
   i_r_op,       /* Only the result. Operand implicit? */ 
   i_op_o1_r,    
   i_op_r_o1,    
   i_r_op_o1,    /* r <- o1 */
   i_r_r_op_o1,  /* r <- r op o1 */
   i_r_o1_op_o2, /* r <- o1 op o2 */ 
   i_nsy         /* not specified yet */
} icode_Format;


/*************************** Int_Code_Stmts ************************/

typedef enum {mem_Addr, reg_Addr, constant, dconstant} opd_Category;  

class ics_Opd;

typedef ics_Opd * ics_Opd_Ptr;

typedef enum { load, load_d, imm_Load, imm_Load_d, store, store_d, add, add_d, sub, sub_d, mult, mult_d, divv, divv_d, uminus, uminus_d, nop } tgt_Op;

class instruction_Descriptor
{
    /* 
       This class stores objects representing machine instructions
       which in our case are assembly instructions supported by spim.
    */

    tgt_Op inst_op;
    string mnemonic;
    string ic_symbol;       /* symbol for printing in intermediate code */
    string name;
    icode_Format ic_format; /* format for printing in intemediate code */
    assembly_Format a_format;

  public:
    instruction_Descriptor (tgt_Op op, string name, string mnn, string ics, icode_Format icf, assembly_Format af);
    ~instruction_Descriptor() {}

    tgt_Op get_Operator();
    string get_Name();
    string get_Mnemonic();
    string get_IC_Symbol();
    icode_Format get_IC_Format();
    assembly_Format get_Assembly_Format();

    void print_Instruction_Descriptor();
};

typedef instruction_Descriptor * inst_Desc_Ptr;

class ics_Opd
{	
    /* 
       Abstract base class for ic operands. From this class we 
       derive classes for three kinds of operands: memory addresses, 
       constants, and registers 
    */
  public:
    opd_Category type;     

    opd_Category get_Opd_Category(); 
    virtual reg_Desc_Ptr get_Reg();
    virtual sym_Entry_Ptr get_Sym_Entry();
    virtual int get_Num();
    virtual double get_Num2();

    /* Operands are printed differently in icode and assembly code */

    virtual void print_ICS_Opd(ostream *st) = 0;
    virtual void print_Asm_Opd(ostream *st) = 0;
};

class mem_Addr_Opd: public ics_Opd
{
    sym_Entry_Ptr sym_entry;

  public:
    mem_Addr_Opd(sym_Entry_Ptr se_P);
    ~mem_Addr_Opd() {}

    sym_Entry_Ptr get_Sym_Entry();
    void print_ICS_Opd(ostream *st);
    void print_Asm_Opd(ostream *st);

    mem_Addr_Opd& operator=(const mem_Addr_Opd& rhs);
};

class reg_Addr_Opd: public ics_Opd
{
    reg_Desc_Ptr reg_desc_ptr;

  public:
    reg_Addr_Opd(reg_Desc_Ptr reg_desc_ptr);
    ~reg_Addr_Opd() {}

    reg_Desc_Ptr get_Reg();
    void print_ICS_Opd(ostream *st);
    void print_Asm_Opd(ostream *st);

    reg_Addr_Opd& operator=(const reg_Addr_Opd& rhs);
};


class const_Opd: public ics_Opd
{
    int num;

  public:
    const_Opd (int num);
    ~const_Opd() {}

    int get_Num();

    void print_ICS_Opd(ostream *st);
    void print_Asm_Opd(ostream *st);

    const_Opd& operator=(const const_Opd& rhs);
};

class dconst_Opd: public ics_Opd
{
    double num;

  public:
    dconst_Opd (double num);
    ~dconst_Opd() {}

    double get_Num2();

    void print_ICS_Opd(ostream *st);
    void print_Asm_Opd(ostream *st);

    dconst_Opd& operator=(const dconst_Opd& rhs);
};

class icode_Stmt
{
    /* 
       Abstract base class for generated ic statements. From this 
       class, we derive three classes: move, compute, control_Flow.
       In this version, we need move sub class only
    */

  public:
    inst_Desc_Ptr op_desc_ptr;

    icode_Stmt() {}
    ~icode_Stmt() {}  

    inst_Desc_Ptr get_Operator();
    virtual ics_Opd_Ptr get_Opd1();
    virtual ics_Opd_Ptr get_Opd2();
    virtual ics_Opd_Ptr get_Result();

    virtual void set_Opd1(ics_Opd_Ptr io_P);
    virtual void set_Opd2(ics_Opd_Ptr io_P);
    virtual void set_Result(ics_Opd_Ptr io_P);

    virtual void print_Icode(ostream *st) = 0;
    virtual void print_Assembly(ostream *st) = 0;
};

class move_IC_Stmt: public icode_Stmt
{ 
    ics_Opd_Ptr opd1;   
    ics_Opd_Ptr result; 

  public:
    move_IC_Stmt(tgt_Op inst_op, ics_Opd_Ptr opd1, ics_Opd_Ptr result); 
    ~move_IC_Stmt() {} 
    move_IC_Stmt& operator=(const move_IC_Stmt& rhs);

    inst_Desc_Ptr get_Inst_Op_of_ICS();

    ics_Opd_Ptr get_Opd1();
    void set_Opd1(ics_Opd_Ptr io_P);

    ics_Opd_Ptr get_Result();
    void set_Result(ics_Opd_Ptr io_P);

    void print_Icode(ostream *st);
    void print_Assembly(ostream *st);
};

class arith_IC_Stmt: public icode_Stmt
{ 
    ics_Opd_Ptr opd1;
    ics_Opd_Ptr opd2;   
    ics_Opd_Ptr result; 

  public:
    arith_IC_Stmt(tgt_Op inst_op, ics_Opd_Ptr opd1,ics_Opd_Ptr opd2, ics_Opd_Ptr result); 
    ~arith_IC_Stmt() {} 
    arith_IC_Stmt& operator=(const arith_IC_Stmt& rhs);

    inst_Desc_Ptr get_Inst_Op_of_ICS();

    ics_Opd_Ptr get_Opd1();
    void set_Opd1(ics_Opd_Ptr io_P);
  
    ics_Opd_Ptr get_Opd2();
    void set_Opd2(ics_Opd_Ptr io_P);

    ics_Opd_Ptr get_Result();
    void set_Result(ics_Opd_Ptr io_P);

    void print_Icode(ostream *st);
    void print_Assembly(ostream *st);
};

typedef icode_Stmt * icode_Stmt_Ptr;
typedef list<icode_Stmt_Ptr> icode_Stmt_List;

class code_for_Ast;

typedef code_for_Ast * ast_Code_Ptr;

class code_for_Ast
{
    /* 
       This class describes the code generated for a given
       AST in a basic block. Note that a single AST would
       in general need multiple statements to implement it.
       We also need to remember the intermediate results of
       an AST because it could be a subtree of a larger AST.
    */

    

  public:
    icode_Stmt_List ics_List;
    reg_Desc_Ptr result_reg;
    code_for_Ast(reg_Desc_Ptr reg, icode_Stmt_List ic_L);
    ~code_for_Ast() {}

    void append_ICS(icode_Stmt_Ptr ics_P);
    icode_Stmt_List get_Icode();

    void print_Icode(ostream *st);

    code_for_Ast& operator=(const code_for_Ast& rhs);
};


typedef  map<tgt_Op, inst_Desc_Ptr> inst_Table;

extern inst_Table spim_inst_table;

void initialize_Inst_Table();
