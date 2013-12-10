
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
#include "support.hh"


/* 
    Please see the documentation in file ast.hh for a description of the
    classes. Here we provide an implementation of the class methods.
*/


int ast_Node::get_Num()
{
    CHECK_INVARIANT(SHOULD_NOT_REACH, "get_Num method cannot be called for a non-num ast node")
}

double ast_Node::get_dNum()
{
    CHECK_INVARIANT(SHOULD_NOT_REACH, "get_dNum method cannot be called for a non-num ast node")
}


/************* Methods for class asgn_Ast ******************/

asgn_Ast::asgn_Ast(ast_Ptr  l, ast_Ptr  r, int line)
{
    t_op = asgn;
    node_arity = binary;
    CHECK_INVARIANT(l != NULL, "Left child of an assignment tree node cannot be NULL")
    CHECK_INVARIANT(l->get_Tree_Op() != num_Leaf, "Left child of an assignment tree node cannot be a number")
    //CHECK_INVARIANT(r != NULL, "Right child of an assignment tree node cannot be NULL")
    CHECK_INVARIANT(l->get_Tree_Op() != asgn, "Left child cannot be an assignment node")
    if(r)
    	CHECK_INVARIANT(r->get_Tree_Op() != asgn, "Right child cannot be an assignment node")
    left = l;
    right = r;
    lineno = line;
}

asgn_Ast& asgn_Ast::operator=(const asgn_Ast& rhs)
{
    t_op = rhs.t_op;          
    node_arity = rhs.node_arity;
    left = rhs.left;
    right = rhs.right;
    lineno = rhs.lineno;
    return *this;
}

/************* Methods for class name_Ast ******************/

name_Ast::name_Ast(string n)
{
        t_op = name_Leaf;
        name = n;
        node_arity = zero_Arity;
        sym_entry = symtab_in_scope_P->get_Sym_Entry(n);
}

sym_Entry_Ptr name_Ast::get_Sym_Entry()
{
        return sym_entry;
}

/************* Methods for class num_Ast ******************/

num_Ast::num_Ast(int n)
{
    t_op = num_Leaf;
    num = n;
    node_arity = zero_Arity;
}

int num_Ast::get_Num()
{
    return num;
}

dnum_Ast::dnum_Ast(double n)
{
    t_op = num_Leaf;
    num = n;
    node_arity = zero_Arity;
}

double dnum_Ast::get_dNum()
{
    return num;
}

/************* Methods for class ret_Ast ******************/

ret_Ast::ret_Ast() 
{
    t_op = ret;
    node_arity = zero_Arity;
}


/************* Methods for class arith ******************/
plus_Ast::plus_Ast(ast_Ptr  l, ast_Ptr  r, int line)
{
    t_op = arith_op;
    a_op = plus_op;
    node_arity = binary;
    //CHECK_INVARIANT(l != NULL, "Left child of an assignment tree node cannot be NULL")
    //CHECK_INVARIANT(r != NULL, "Right child of an assignment tree node cannot be NULL")
    CHECK_INVARIANT(l->get_Tree_Op() != asgn, "Left child cannot be an assignment node")
    CHECK_INVARIANT(r->get_Tree_Op() != asgn, "Right child cannot be an assignment node")
    left = l;
    right = r;
    lineno = line;
}

minus_Ast::minus_Ast(ast_Ptr  l, ast_Ptr  r, int line)
{
    t_op = arith_op;
    a_op = minus_op;
    node_arity = binary;
    //CHECK_INVARIANT(l != NULL, "Left child of an assignment tree node cannot be NULL")
    //CHECK_INVARIANT(r != NULL, "Right child of an assignment tree node cannot be NULL")
    CHECK_INVARIANT(l->get_Tree_Op() != asgn, "Left child cannot be an assignment node")
    CHECK_INVARIANT(r->get_Tree_Op() != asgn, "Right child cannot be an assignment node")
    left = l;
    right = r;
    lineno = line;
}

uminus_Ast::uminus_Ast(ast_Ptr  l, int line)
{
    t_op = arith_op;
    a_op = uminus_op;
    node_arity = unary;
    //CHECK_INVARIANT(l != NULL, "Left child of an assignment tree node cannot be NULL")
    CHECK_INVARIANT(l->get_Tree_Op() != asgn, "Left child cannot be an assignment node")
    left = l;
    right = NULL;
    lineno = line;
}

mult_Ast::mult_Ast(ast_Ptr  l, ast_Ptr  r,  int line)
{
    t_op = arith_op;
    a_op = mult_op;
    node_arity = binary;
    //CHECK_INVARIANT(l != NULL, "Left child of an assignment tree node cannot be NULL")
    //CHECK_INVARIANT(r != NULL, "Right child of an assignment tree node cannot be NULL")
    CHECK_INVARIANT(l->get_Tree_Op() != asgn, "Left child cannot be an assignment node")
    CHECK_INVARIANT(r->get_Tree_Op() != asgn, "Right child cannot be an assignment node")
    left = l;
    right = r;
    lineno = line;
}

div_Ast::div_Ast(ast_Ptr  l, ast_Ptr  r,  int line)
{
    t_op = arith_op;
    a_op = div_op;
    node_arity = binary;
    //CHECK_INVARIANT(l != NULL, "Left child of an assignment tree node cannot be NULL")
    //CHECK_INVARIANT(r != NULL, "Right child of an assignment tree node cannot be NULL")
    CHECK_INVARIANT(l->get_Tree_Op() != asgn, "Left child cannot be an assignment node")
    CHECK_INVARIANT(r->get_Tree_Op() != asgn, "Right child cannot be an assignment node")
    left = l;
    right = r;
    lineno = line;
}

type_converter_Ast::type_converter_Ast(ast_Ptr l, value_Type x, int line){
      
      CHECK_INVARIANT(l->get_Tree_Op() != asgn, "Left child cannot be an assignment node")
      left = l;
      v = x;
      lineno = line;
      a_op = typecast_op;
      t_op = arith_op;
      node_arity = unary;
    }
