
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

#include <string>
#include <vector>
#include <list>
#include <map>

class ast_Node;
typedef ast_Node* ast_Ptr;

class sym_List;
class sym_Entry;
class symtab_In_Scope;
typedef sym_Entry * sym_Entry_Ptr;
typedef sym_List * sym_List_Ptr;
typedef map <string,sym_Entry_Ptr> sym_Entry_List; 

class program;
class procedure;
class basic_Block;
typedef basic_Block * bb_Ptr;

class eval_Result; 

class ics_Opd;
typedef ics_Opd * ics_Opd_Ptr;
class code_for_Ast;
class code_for_BB;

typedef code_for_Ast * ast_Code_Ptr;
typedef code_for_BB * bb_Code_Ptr;

class icode_Stmt;
typedef icode_Stmt * icode_Stmt_Ptr;

typedef list<icode_Stmt_Ptr> icode_Stmt_List;

