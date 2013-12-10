
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

/*
   Since we want to use a generic evaluate function that works
   uniformly on all kinds of ASts in the program
   -  In current version of cfglp: 
          assignments, names, numbers. 
   -  In future versions of cfglp: 
          arithmetic operations (integer and float) , control 
          flow, function calls, simple classes.

   Hence we define a generic class eval_Result that
   stores values along with types. All communication between 
   entities happen in terms of objects of this class. The actual 
   computations are performed by extracting underlying values 
   and then creating objects of this class representing the 
   results in domain of underlying values.
*/

typedef enum {int_Res, next_BB_Res, void_Res, float_Res} res_Type;

class eval_Result {
    int int_res;
    double float_res;
    bb_Ptr next_BB;
    res_Type relevant_result;

  public:
    
    eval_Result(int num, bb_Ptr bb, res_Type rt);
    eval_Result(double dnum, bb_Ptr bb, res_Type rt);
    ~eval_Result() {}
    
    int get_Int_Val();
    void  set_Int_Val(int n);
	
	double get_Float_Val();
    void  set_Float_Val(double n);

    bb_Ptr get_Next_BB();
    void set_Next_BB(bb_Ptr bb_p);

    res_Type which_Result();

    void print_Eval_Result();
};

extern eval_Result dummy_result;
