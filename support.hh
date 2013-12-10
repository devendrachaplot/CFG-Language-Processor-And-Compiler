
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

#include <sstream>
#include <string>

using namespace std;

#define SHOULD_NOT_REACH false

void check_Invariant_Underlying_Function(bool b, string s);
void report_Violation_of_Condition(bool b, string s);
bool error_Status ();
void report_Violation_and_Abort(bool b, string s);

#define CHECK_INVARIANT(x,y) \
     {  stringstream ___new___string___; \
        ___new___string___ << y << " (Invariant at line " << __LINE__ <<  ", file " << __FILE__ << ").\n"; \
        check_Invariant_Underlying_Function(x, ___new___string___.str()); \
     }

        
