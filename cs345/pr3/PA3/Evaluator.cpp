

#include "Evaluator.h"

#include "ast/expression.h"





/*
 * This skeleton currently only contains code to handle integer constants, print and read. 
 * It is your job to handle all of the rest of the L language.
 */





/*
 * Call this function to report any run-time errors
 * This will abort execution.
 */
void report_error(Expression* e, const string & s)
{
	cout << "Run-time error in expression " << e->to_value() << endl;
	cout << s << endl;
	exit(1);

}



Evaluator::Evaluator()
{
	sym_tab.push();
	c = 0;

}

Expression* Evaluator::eval_unop(AstUnOp* b)
{



	Expression* e = b->get_expression();
	Expression* eval_e = eval(e);

	if(b->get_unop_type() == PRINT) {
		if(eval_e->get_type() == AST_STRING) {
			AstString* s = static_cast<AstString*>(eval_e);
			cout << s->get_string() << endl;
		}
		else cout << eval_e->to_value() << endl;
		return AstInt::make(0);
	}
    if(b->get_unop_type() == ISNIL) {
        if(eval_e->get_type() == AST_NIL){
            return AstInt::make(1);
        }
        else return AstInt::make(0);
    }
    if(b->get_unop_type() == HD){
        if(eval_e->get_type() == AST_LIST){
          AstList* l = static_cast<AstList*>(eval_e);
          return l->get_hd();
        }
        else return eval_e;
    }
    if(b->get_unop_type() == TL){
        if(eval_e->get_type() == AST_LIST){
          AstList* l = static_cast<AstList*>(eval_e);
          return l->get_tl();
        }
        else return AstNil::make();
    }
      //add code to deal with all the other unops
      assert(false);

}
Expression* Evaluator::eval_binop(AstBinOp* b)
{
   Expression* e1 = b->get_first(); 
   Expression* e2 = b->get_second();
   Expression* eval_e1 = eval(e1);
   Expression* eval_e2 = eval(e2);
   binop_type binop = b->get_binop_type(); 
   if(binop == PLUS){
      if( eval_e1->get_type() == AST_INT && eval_e2->get_type() == AST_INT){
	    AstInt* i1 = static_cast<AstInt*>(eval_e1);
	    AstInt* i2 = static_cast<AstInt*>(eval_e2);

        return AstInt::make(i1->get_int()+i2->get_int());
      }
      else if( eval_e1->get_type() == AST_STRING && eval_e2->get_type() == 
              AST_STRING){
	         AstString* s1 = static_cast<AstString*>(eval_e1);
	         AstString* s2 = static_cast<AstString*>(eval_e2);
             return AstString::make(s1->get_string()+s2->get_string());
      }
      else if(eval_e1->get_type() == AST_LIST || eval_e2->get_type() == AST_LIST){
             report_error(b, "Binop @ is the only legal binop for lists");
      }
      else if(eval_e1->get_type() == AST_NIL || eval_e2->get_type() == AST_NIL){
             report_error(b, "Nil can only be used with binop @");
      }
      else{
             report_error(b, "Binop can only be applied to expressions of the same type");
      }
   }
   else if(binop == MINUS){
      if( eval_e1->get_type() == AST_INT && eval_e2->get_type() == AST_INT){
	    AstInt* i1 = static_cast<AstInt*>(eval_e1);
	    AstInt* i2 = static_cast<AstInt*>(eval_e2);

        return AstInt::make(i1->get_int()-i2->get_int());
      }
      else if(eval_e1->get_type() == AST_LIST || eval_e2->get_type() == AST_LIST){
             report_error(b, "Binop @ is the only legal binop for lists");
      }
      else if(eval_e1->get_type() == AST_STRING || eval_e2->get_type() == AST_STRING){
             report_error(b, "Binop X cannot be applied to strings");
      }
      else if(eval_e1->get_type() == AST_NIL || eval_e2->get_type() == AST_NIL){
             report_error(b, "Nil can only be used with binop @");
      }
      else{
             report_error(b, "Binop can only be applied to expressions of the same type");
      }
   }
   else if(binop == TIMES){
      if( eval_e1->get_type() == AST_INT && eval_e2->get_type() == AST_INT){
	    AstInt* i1 = static_cast<AstInt*>(eval_e1);
	    AstInt* i2 = static_cast<AstInt*>(eval_e2);

        return AstInt::make(i1->get_int()*i2->get_int());
      }
      else if(eval_e1->get_type() == AST_LIST || eval_e2->get_type() == AST_LIST){
             report_error(b, "Binop @ is the only legal binop for lists");
      }
      else if(eval_e1->get_type() == AST_STRING || eval_e2->get_type() == AST_STRING){
             report_error(b, "Binop X cannot be applied to strings");
      }
      else if(eval_e1->get_type() == AST_NIL || eval_e2->get_type() == AST_NIL){
             report_error(b, "Nil can only be used with binop @");
      }
      else{
             report_error(b, "Binop can only be applied to expressions of the same type");
      }
   }
   else if(binop == DIVIDE){
      if( eval_e1->get_type() == AST_INT && eval_e2->get_type() == AST_INT){
	    AstInt* i1 = static_cast<AstInt*>(eval_e1);
	    AstInt* i2 = static_cast<AstInt*>(eval_e2);

        return AstInt::make(i1->get_int()/i2->get_int());
      }
      else if(eval_e1->get_type() == AST_LIST || eval_e2->get_type() == AST_LIST){
             report_error(b, "Binop @ is the only legal binop for lists");
      }
      else if(eval_e1->get_type() == AST_STRING || eval_e2->get_type() == AST_STRING){
             report_error(b, "Binop X cannot be applied to strings");
      }
      else if(eval_e1->get_type() == AST_NIL || eval_e2->get_type() == AST_NIL){
             report_error(b, "Nil can only be used with binop @");
      }
      else{
             report_error(b, "Binop can only be applied to expressions of the same type");
      }
   }
   else if(binop == EQ){
      if(eval_e1->get_type() == AST_INT && eval_e2->get_type() == AST_INT){
         AstInt* i1 = static_cast<AstInt*>(eval_e1);
         AstInt* i2 = static_cast<AstInt*>(eval_e2);
          if(i1->get_int() == i2->get_int()){
            return AstInt::make(1);
          }
          else return AstInt::make(0);
      }
      else if(eval_e1->get_type() == AST_STRING && eval_e2->get_type() == AST_STRING){
          AstString* s1 = static_cast<AstString*>(eval_e1);
          AstString* s2 = static_cast<AstString*>(eval_e2);
            if(s1->get_string() == s2->get_string()){
              return AstInt::make(1);
            }
            else return AstInt::make(0);
       }
      else if(eval_e1->get_type() == AST_LIST || eval_e2->get_type() == AST_LIST){
             report_error(b, "Binop @ is the only legal binop for lists");
      }
      else if(eval_e1->get_type() == AST_NIL || eval_e2->get_type() == AST_NIL){
             report_error(b, "Nil can only be used with binop @");
      }
      else{
             report_error(b, "Binop can only be applied to expressions of the same type");
      }
   }
   else if(binop == NEQ){
      if(eval_e1->get_type() == AST_INT && eval_e2->get_type() == AST_INT){
         AstInt* i1 = static_cast<AstInt*>(eval_e1);
         AstInt* i2 = static_cast<AstInt*>(eval_e2);
          if(i1->get_int() == i2->get_int()){
            return AstInt::make(0);
          }
          else return AstInt::make(1);
      }
      else if(eval_e1->get_type() == AST_STRING && eval_e2->get_type() == AST_STRING){
          AstString* s1 = static_cast<AstString*>(eval_e1);
          AstString* s2 = static_cast<AstString*>(eval_e2);
            if(s1->get_string() == s2->get_string()){
              return AstInt::make(0);
            }
            else return AstInt::make(1);
       }
      else if(eval_e1->get_type() == AST_LIST || eval_e2->get_type() == AST_LIST){
             report_error(b, "Binop @ is the only legal binop for lists");
      }
      else if(eval_e1->get_type() == AST_NIL || eval_e2->get_type() == AST_NIL){
             report_error(b, "Nil can only be used with binop @");
      }
      else{
             report_error(b, "Binop can only be applied to expressions of the same type");
      }
   }
    else if(binop == LT || binop == GT || binop == LEQ || binop == GEQ
            || binop == AND || binop == OR){
      AstInt* i1;
      AstInt* i2; 
      if(eval_e1->get_type() == AST_INT && eval_e2->get_type() == AST_INT){
      i1 = static_cast<AstInt*>(eval_e1);
      i2 = static_cast<AstInt*>(eval_e2);
      }
      else if(eval_e1->get_type() == AST_LIST || eval_e2->get_type() == AST_LIST){
             report_error(b, "Binop @ is the only legal binop for lists");
      }
      else if(eval_e1->get_type() == AST_STRING || eval_e2->get_type() == AST_STRING){
             report_error(b, "Binop X cannot be applied to strings");
      }
      else if(eval_e1->get_type() == AST_NIL || eval_e2->get_type() == AST_NIL){
             report_error(b, "Nil can only be used with binop @");
      }
      else if(eval_e1->get_type() == eval_e2->get_type()){
             report_error(b, "Binop can only be applied to expressions of the same type");
      }
      switch(binop)
      {
        case LT:
        {
          if(i1->get_int() < i2->get_int()){
            return AstInt::make(1);
          }
          else return AstInt::make(0);
        }
        case GT:
        {
          if(i1->get_int() > i2->get_int()){
            return AstInt::make(1);
          }
          else return AstInt::make(0);
        }
        case LEQ:
        {
          if(i1->get_int() <= i2->get_int()){
            return AstInt::make(1);
          }
          else return AstInt::make(0);
        }
        case GEQ:
        {
          if(i1->get_int() >= i2->get_int()){
            return AstInt::make(1);
          }
          else return AstInt::make(0);
        }
        case AND:
        {
          if(i1->get_int() & i2->get_int()){
            return AstInt::make(1);
          }
          else return AstInt::make(0);
        }
        case OR:
        {
          if(i1->get_int() | i2->get_int()){
            return AstInt::make(1);
          }
          else return AstInt::make(0);
        }
        default:
        assert(false); 
      }
     
   }
   else if(binop == CONS){
      if(eval_e2->get_type() != AST_NIL){
          return AstList::make(eval_e1, eval_e2); 
      }
      else{
          return eval_e1; 
      }
   }
   assert(false); 
}
Expression* Evaluator::eval_expression_list(AstExpressionList* l){
      vector<Expression*> exp_vector = l->get_expressions(); 
      if(exp_vector.size() == 1){
        return eval(exp_vector.front());
      }
      Expression* e1 = eval(exp_vector.front());
      if(e1->get_type() != AST_LAMBDA){
        report_error(l, "Only lambda expressions can be applied to other expressions");
      }
      else{
        AstLambda* lambda_exp = static_cast<AstLambda*>(e1);
        Expression* e1_prime = lambda_exp->get_body(); 
        Expression* e2 = exp_vector.at(1);
        AstIdentifier* id = lambda_exp->get_formal();
        Expression* e = e1_prime->substitute(id, e2);
        sym_tab.push();
        sym_tab.add(id, e2);
        exp_vector.erase(exp_vector.begin(), exp_vector.begin()+2);
        exp_vector.insert(exp_vector.begin(), e);
        Expression* result = eval_expression_list(AstExpressionList::make(exp_vector));
        return result;
      }
   assert(false);
}




Expression* Evaluator::eval(Expression* e)
{
	Expression* res_exp = NULL;
	switch(e->get_type())
	{
	
	case AST_UNOP:
	{
		AstUnOp* b = static_cast<AstUnOp*>(e);
		res_exp = eval_unop(b);
		break;
	}
	case AST_READ:
	{
		AstRead* r = static_cast<AstRead*>(e);
		string input;
		getline(cin, input);
		if(r->read_integer()) {
			return AstInt::make(string_to_int(input));
		}
		return AstString::make(input);


		break;
	}
	case AST_INT:
	{
		res_exp = e;
		break;
	}
	case AST_STRING:
	{
		res_exp = e;
		break;
	}
    case AST_IDENTIFIER:
    {
       assert(e->get_type() == AST_IDENTIFIER);
       AstIdentifier* i = static_cast<AstIdentifier*>(e);
       res_exp = sym_tab.find(i);
       if(res_exp == NULL){
           report_error(e, "Identifier ID is not bound in current context");
       }
       break; 
    }
    case AST_LET:
    {
      sym_tab.push();
      assert(e->get_type() == AST_LET);
      AstLet* l = static_cast<AstLet*>(e);
      Expression* e1 = l->get_val();
      Expression* eval_e = eval(e1); 
      sym_tab.add(l->get_id(), eval_e);
      Expression* e2 = l->get_body();
      Expression* eval_e2 = eval(e2);
      res_exp = eval_e2;
      sym_tab.pop(); 
      break;
    }
    case AST_BRANCH:
    {
       AstBranch* b = static_cast<AstBranch*>(e);
       Expression* eval_e = eval(b->get_pred());
       if(eval_e->get_type() != AST_INT){
          report_error(e, "Predicate in conditional must be an integer");
       }
       AstInt* i = static_cast<AstInt*>(eval_e);
       Expression* eval_e2; 
       if(i->get_int() == 0){
          eval_e2 = eval(b->get_else_exp());
       }
       else{
          eval_e2 = eval(b->get_then_exp());
       }
       res_exp = eval_e2; 
       break;
    }
    case AST_BINOP:
    {
		AstBinOp* b = static_cast<AstBinOp*>(e);
		res_exp = eval_binop(b);
		break;
    }
    case AST_NIL:
    {
        res_exp = e;
        break;
    }
    case AST_LAMBDA:
    {
        res_exp = e;
        break;
    }
    case AST_EXPRESSION_LIST:
    {
		AstExpressionList* l = static_cast<AstExpressionList*>(e);
		res_exp = eval_expression_list(l);
		break;

    }
	//ADD CASES FOR ALL EXPRESSIONS!!
	default:
		assert(false);


	}
	return res_exp;
}
