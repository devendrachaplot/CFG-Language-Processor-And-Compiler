
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

using namespace std;

/* 
   Invariant condition b must be satisfied at each call to procedure CHECK_INVARIANT. 
   If it is not satisfied, there must be some internal error. Abort the compilation.
   This is different from erroneous input to our compiler.
*/

void check_Invariant_Underlying_Function(bool b, string s)
{
    if (!b)
    {
        cerr <<  "\ncfglp internal error: " << s << "\n";
        exit(1);
    }
} 


/* 
   If the input to our compiler is valid, condition b must be satisfied at call to 
   procedure report_Violation_of_Condition. If it is not satisfied, there must be 
   some error in the input. 

   This is different from an internal inconsistency of our compiler.  Hence we do 
   not abort the compilation but try to report as many errors as possible.
*/

static bool global_error_status = false;

void report_Violation_of_Condition(bool b, string s)
{
    if (!b)
    {
        cerr <<  "     cfglp error: " << s << "\n";
        global_error_status = true;
    }
} 

/*
   In some cases of violation (such as unable to parse the program), 
   we may like to abort the compilation.
*/
void report_Violation_and_Abort(bool b, string s)
{
    if (!b)
    {
        cerr <<  "     cfglp error: " << s << "\n";
        exit(1);
    }
} 


bool error_Status ()   
{ 
   return global_error_status; 
}

