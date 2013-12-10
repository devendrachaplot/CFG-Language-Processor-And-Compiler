
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


static ostream * eval_fp = NULL;

eval_Result program::evaluate()
{
    proc_Ptr main_p = get_Main_Proc_Ptr();
    return main_p->evaluate();
}

eval_Result procedure::evaluate()
{
   eval_Result res(0,NULL,void_Res);

   eval_fp = cmd_options.output_File();

   *eval_fp << "\nEvaluating Procedure " << name << "\n";

   *eval_fp << " Symtab values before evaluating procedure " << name << "\n";
   print_Symtab_for_Eval(eval_fp);

   *eval_fp << " Evaluating statements \n";
   bb_Ptr cur_BB = get_Start_BB();
   while (cur_BB)
   {
       res = cur_BB->evaluate();
       cur_BB = res.get_Next_BB();
   }

   *eval_fp << " Final symtab values after evaluating procedure " << name << "\n";
   print_Symtab_for_Eval(eval_fp);

   return res;
}
   
eval_Result basic_Block::evaluate()
{
   eval_Result res(0,NULL,void_Res);

    list <ast_Ptr>::iterator i;
    for (i = ast_list->begin(); i != ast_list->end(); i++)
    {
         CHECK_INVARIANT (*i, "Ast pointer seems to be NULL")
       	 res = (*i)->evaluate(); 
    }
    return res;
}

proc_Ptr program::get_Main_Proc_Ptr()
{
     map<string, proc_Ptr>::iterator i;
     for (i = proc_list->begin(); i != proc_list->end(); i++)
     {
          proc_Ptr pp = i->second;
          CHECK_INVARIANT (pp, "Procedure pointer seems to be NULL")
          if (pp->get_Name() == "main")
              return pp;  
     }
}

bb_Ptr procedure::get_Start_BB()
{
    list<bb_Ptr>::iterator i = bb_list->begin(); 
    return (*i);
}

