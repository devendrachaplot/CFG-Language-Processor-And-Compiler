
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
#include <cstdlib>
#include <string>
#include <vector>

using namespace std;

#include "common-classes.hh"
#include "evaluate.hh"
#include "reg-alloc.hh"
#include "symtab.hh"
#include "ast.hh"
#include "program.hh"
#include "support.hh"


/* 
    Please see the documentation in file ast.hh for a description of the
    classes. Here we provide an implementation of the class methods.
*/

eval_Result::eval_Result(int num, bb_Ptr bb, res_Type rt)
{
    int_res = num;
    next_BB = bb;
    relevant_result = rt;
}
eval_Result::eval_Result(double num, bb_Ptr bb, res_Type rt)
{
    float_res = num;
    next_BB = bb;
    relevant_result = rt;
}

int eval_Result::get_Int_Val()             { return int_res; }
double eval_Result::get_Float_Val()             { return float_res; }
bb_Ptr eval_Result::get_Next_BB()          { return next_BB; }
res_Type eval_Result::which_Result()       { return relevant_result; }
void  eval_Result::set_Int_Val(int n)      { int_res = n; }
void  eval_Result::set_Float_Val(double n)      { float_res = n; }
void eval_Result::set_Next_BB(bb_Ptr bb_p) { next_BB = bb_p; }

void eval_Result::print_Eval_Result()
{
   stringstream ss;

   switch (relevant_result)
   {
       case int_Res:  ss << "Number"; break;
       case next_BB_Res:  ss << "Next BB"; break;
       case void_Res:  ss << "Void"; break;
       default: ss << "Some thing strange"; break;
   }

   cout << "\nResult Object: Number is " << int_res << " Next BB is " << next_BB << " Relevant result is " << ss.str();
   
}

eval_Result dummy_result (0, NULL, void_Res);
