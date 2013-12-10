
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
#include <ostream>
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
#include "options.hh"

eval_Result asgn_Ast::evaluate()
{
    CHECK_INVARIANT (right != NULL, "Right child of an assignment tree node cannot be NULL")
    eval_Result res = right->evaluate(); 

    CHECK_INVARIANT (left != NULL, "Left child of an assignment tree node cannot be NULL")
    CHECK_INVARIANT (left->get_Tree_Op() == name_Leaf, "LHS of an assignment should be a name")
    left->set_Value_of_Evaluation(res);

    /* 
       Here we really do not need to return the result but since 
       the type of the evaluate function has to remain identical 
       across all derived classes, we return a dummy object which
       has been declared globally.
    */
    print_Eval_Result(this);
    return dummy_result;
}

eval_Result arith_Expr_Ast::evaluate()
{
	stringstream s;
	int line;
	//cout<<"Hello"<<endl;
    CHECK_INVARIANT (right != NULL, "Right child of an arith tree node cannot be NULL")
    CHECK_INVARIANT (left != NULL, "Right child of an arith tree node cannot be NULL")
    eval_Result r = right->evaluate(); 
    eval_Result l = left->evaluate();
    int rs=0;
    double rf=0.0;
    switch(a_op)
    {
    	case plus_op:
    		rs = r.get_Int_Val()+l.get_Int_Val();
    		rf = r.get_Float_Val()+l.get_Float_Val();
    		break;
    	case minus_op:
    		rs = l.get_Int_Val()-r.get_Int_Val();
    		rf = l.get_Float_Val()-r.get_Float_Val();
    		break;
    	case mult_op:
    		rs = r.get_Int_Val()*l.get_Int_Val();
    		rf = r.get_Float_Val()*l.get_Float_Val();
    		break;
    	case div_op:
    		s<<"Divisor on line "<<lineno<<" has value 0";
    		if(r.which_Result()==l.which_Result() && r.which_Result() == float_Res) {
    			if(r.get_Float_Val() == 0.0)
    				report_Violation_and_Abort(false, s.str());
    			rf = l.get_Float_Val()/r.get_Float_Val();
    		}
    		if(r.which_Result()==l.which_Result() && r.which_Result() == int_Res) {
    			if(r.get_Int_Val()==0)
    				report_Violation_and_Abort(false, s.str());
    			rs = l.get_Int_Val()/r.get_Int_Val();
    		}
    		//rs = r.get_Int_Val()/l.get_Int_Val();
    		
    		break;
    	case uminus_op:
    		rs = - l.get_Int_Val();
    		rf = - l.get_Float_Val();
    		break;
    	case typecast_op:
    		rs = l.get_Int_Val();
    		rf = l.get_Float_Val();
    		break;
    		
    }
    if(r.which_Result()==l.which_Result() && r.which_Result() == int_Res) {
    	eval_Result res(rs,NULL,int_Res);
    	return res;
    }
    else if(r.which_Result()==l.which_Result() && r.which_Result() == float_Res) {
    	eval_Result res(rf,NULL,float_Res);
    	return res;
    }
}

eval_Result uminus_Ast::evaluate()
{
    CHECK_INVARIANT (left != NULL, "Left child of a unary minus tree node cannot be NULL")
    eval_Result l = left->evaluate();
    int rs=0;
    double rf=0.0;
    rs = - l.get_Int_Val();
    rf = - l.get_Float_Val();
    if(l.which_Result() == int_Res) {
    	eval_Result res(rs,NULL,int_Res);
    	return res;
    }
    else if(l.which_Result() == float_Res) {
    	eval_Result res(rf,NULL,float_Res);
    	return res;
    }
}

eval_Result type_converter_Ast::evaluate()
{
	//stringstream s;
	//int line;
	//cout<<"Hello"<<endl;
    CHECK_INVARIANT (left != NULL, "Left child of a typecast tree node cannot be NULL")
    eval_Result l = left->evaluate();
    int rs=0;
    double rf=0.0;
    rs = l.get_Int_Val();
    rf = l.get_Float_Val();
    if(get_Val_Type() == float_Val) {
    	if(l.which_Result() == int_Res) {
    		eval_Result res((float)rs,NULL,float_Res);
    		return res;
    	}
    	else if(l.which_Result() == float_Res) {
    		eval_Result res(rf,NULL,float_Res);
    		return res;
    	}
    }
}

eval_Result name_Ast::evaluate()
{
    CHECK_INVARIANT (sym_entry != NULL, "Left child of an assignment tree node cannot be NULL")
    return this->get_Value_of_Evaluation();
}

eval_Result num_Ast::evaluate()
{
    eval_Result res(num, NULL, int_Res);
    return res;
}

eval_Result dnum_Ast::evaluate()
{
    eval_Result res(num, NULL, float_Res);
    return res;
}


eval_Result ret_Ast::evaluate()
{
    /* 
       In this version, we have void procedures only
       hence we return the dummy value which is void.
        
    */
    return dummy_result;
}

eval_Result name_Ast::get_Value_of_Evaluation()
{
    CHECK_INVARIANT(sym_entry, "Sym entry of symbol cannot be NULL")
    return sym_entry->get_Value_of_Evaluation();
}

void name_Ast::set_Value_of_Evaluation(eval_Result res)
{
    CHECK_INVARIANT(sym_entry, "Sym entry of symbol cannot be NULL")
    sym_entry->set_Value_of_Evaluation(res);
}

eval_Result ast_Node::get_Value_of_Evaluation()
{
    CHECK_INVARIANT(SHOULD_NOT_REACH, "get_Value_of_Evaluation cannot be called on a non-name-Ast")
}

void ast_Node::set_Value_of_Evaluation(eval_Result res)
{
    CHECK_INVARIANT(SHOULD_NOT_REACH, "set_Value_of_Evaluation cannot be called on a non-name-Ast")
}
