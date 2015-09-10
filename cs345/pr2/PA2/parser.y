%{
#include "parser-defs.h"

int yylex();
extern int yy_scan_string(const char* c);
int yyerror(const char* p)
{ 
  if(parser_error_fn != NULL) {
    parser_error_fn("At line " + int_to_string(curr_lineno) + ": " + string(p));
  }
  return 1;
};

 



%}
/* BISON Declarations */
%token 
TOKEN_READSTRING 
TOKEN_READINT 
TOKEN_PRINT 
TOKEN_ISNIL
TOKEN_HD 
TOKEN_TL 
TOKEN_CONS 
TOKEN_NIL 
TOKEN_DOT 
TOKEN_WITH 
TOKEN_LET 
TOKEN_PLUS 
TOKEN_MINUS 
TOKEN_IDENTIFIER 
TOKEN_TIMES 
TOKEN_DIVIDE 
TOKEN_INT 
TOKEN_LPAREN 
TOKEN_RPAREN 
TOKEN_AND 
TOKEN_OR 
TOKEN_EQ 
TOKEN_NEQ 
TOKEN_GT 
TOKEN_GEQ 
TOKEN_LT 
TOKEN_LEQ 
TOKEN_IF 
TOKEN_THEN 
TOKEN_ELSE 
TOKEN_LAMBDA 
TOKEN_FUN 
TOKEN_COMMA 
TOKEN_STRING 
TOKEN_ERROR 
TOKEN_IN


%nonassoc EXPR

%left TOKEN_PRINT
%left TOKEN_EQ TOKEN_NEQ TOKEN_GT TOKEN_GEQ TOKEN_LT TOKEN_LEQ
%left TOKEN_AND TOKEN_OR
%left TOKEN_PLUS TOKEN_MINUS
%left TOKEN_TIMES TOKEN_DIVIDE
%left TOKEN_ISNIL
%right TOKEN_CONS
%left TOKEN_TL TOKEN_HD
%nonassoc funEQ

%%







program: 
expression
{
	res_expr = $$;
}



expression: 
TOKEN_LET expression TOKEN_EQ expression TOKEN_IN expression %prec EXPR
{
	Expression* identifier = $2;
	assert(identifier->get_type() == AST_IDENTIFIER);       
	AstIdentifier* id = static_cast<AstIdentifier*>(identifier);
	Expression* e = AstLet::make(id, $4, $6);
        $$ = e;
}
|
TOKEN_FUN expression TOKEN_WITH expression TOKEN_EQ expression TOKEN_IN expression %prec EXPR  
{
	Expression* id_or_list = $4;
	assert(id_or_list->get_type() == AST_IDENTIFIER);
	AstIdentifier* id = static_cast<AstIdentifier*>(id_or_list);
	AstLambda* lambda = AstLambda::make(id, $6); 
	Expression* e = $2;
	assert(e->get_type() == AST_IDENTIFIER);
	AstIdentifier* id2 = static_cast<AstIdentifier*>(e);
	$$ = AstLet::make(id2, lambda, $8);
}
|
TOKEN_FUN expression TOKEN_WITH id_list TOKEN_EQ expression TOKEN_IN expression %prec EXPR  
{
	Expression* id_or_list = $4;
	assert(id_or_list->get_type() == AST_IDENTIFIER_LIST);
	AstIdentifierList* list = static_cast<AstIdentifierList*>(id_or_list);
	AstLambda* lambda = AstLambda::make(list, $6); 
	Expression* e = $2;
	assert(e->get_type() == AST_IDENTIFIER);
	AstIdentifier* id2 = static_cast<AstIdentifier*>(e);
	$$ = AstLet::make(id2, lambda, $8);
}
|
TOKEN_LAMBDA id_list TOKEN_DOT expression %prec EXPR
{
  Expression* id_or_list = $2;
  assert(id_or_list->get_type() == AST_IDENTIFIER_LIST);
  AstIdentifierList* list = static_cast<AstIdentifierList*>(id_or_list);

   $$ = AstLambda::make(list, $4); 		
}
|
TOKEN_LAMBDA expression TOKEN_DOT expression %prec EXPR
{
  Expression* id_or_list = $2;
  assert(id_or_list->get_type() == AST_IDENTIFIER);
  AstIdentifier* id = static_cast<AstIdentifier*>(id_or_list);

   $$ = AstLambda::make(id, $4); 		
}
|
TOKEN_IF expression TOKEN_THEN expression TOKEN_ELSE expression %prec EXPR
{
  $$ = AstBranch::make($2, $4, $6); 
}
|
TOKEN_INT 
{
  	string lexeme = GET_LEXEME($1);
  	long int val = string_to_int(lexeme);
  	Expression* e = AstInt::make(val);
	$$ = e;
} 
|
TOKEN_STRING 
{
	string lexeme = GET_LEXEME($1);
  	Expression* e = AstString::make(lexeme);
  	$$ = e;
}
|
TOKEN_IDENTIFIER
{
	string lexeme = GET_LEXEME($1);
  	$$ =  AstIdentifier::make(lexeme);
}  
|
TOKEN_ISNIL expression  
{
       $$ = AstUnOp::make(ISNIL, $2);
}
|
expression TOKEN_PLUS expression 
{
	$$ = AstBinOp::make(PLUS, $1, $3);
}
|  
expression TOKEN_AND expression
{
	$$ = AstBinOp::make(AND, $1, $3);
}
|
expression TOKEN_OR expression
{
	$$ = AstBinOp::make(OR, $1, $3);
}
|
expression TOKEN_MINUS expression
{
	$$ = AstBinOp::make(MINUS, $1, $3);
}
|
expression TOKEN_TIMES expression
{
	$$ = AstBinOp::make(TIMES, $1, $3);
}
|
expression TOKEN_DIVIDE expression
{
	$$ = AstBinOp::make(DIVIDE, $1, $3);
}
|
expression TOKEN_EQ expression
{
	$$ = AstBinOp::make(EQ, $1, $3);
}
|
expression TOKEN_NEQ expression
{
	$$ = AstBinOp::make(NEQ, $1, $3);
}
|
expression TOKEN_LT expression
{
	$$ = AstBinOp::make(LT, $1, $3);
}
|
expression TOKEN_LEQ expression
{
	$$ = AstBinOp::make(LEQ, $1, $3);
}
|
expression TOKEN_GT expression
{
	$$ = AstBinOp::make(GT, $1, $3);
}
|
expression TOKEN_GEQ expression
{
	$$ = AstBinOp::make(GEQ, $1, $3);
}
|
TOKEN_PRINT expression
{
       $$ = AstUnOp::make(PRINT, $2); 
}
|
TOKEN_READINT
{
       $$ = AstRead::make(true);
}
|
TOKEN_READSTRING
{
       $$ = AstRead::make(false);
}
|
TOKEN_HD expression
{
       $$ = AstUnOp::make(HD, $2);
}
|
TOKEN_TL expression
{
       $$ = AstUnOp::make(TL, $2);
}
|
expression TOKEN_CONS expression 
{
       $$ = AstBinOp::make(CONS, $1, $3);
}
|
TOKEN_NIL
{
       $$ = AstNil::make();
}
|
TOKEN_LPAREN expression_application TOKEN_RPAREN
{
	$$ = $2;
}
|
TOKEN_LPAREN expression TOKEN_RPAREN
{
	$$ = $2;
}
|
TOKEN_ERROR 
{
   // do not change the error rule
   string lexeme = GET_LEXEME($1);
   string error = "Lexing error: ";
   if(lexeme != "") error += lexeme;
   yyerror(error.c_str());
   YYERROR;
}




expression_application:
expression expression
{
	AstExpressionList *l = AstExpressionList::make($1);
	l = l->append_exp($2);
	$$ = l;
}
|
expression_application expression
{
	Expression* _l = $1;
	assert(_l->get_type() == AST_EXPRESSION_LIST);
	AstExpressionList* l = static_cast<AstExpressionList*>(_l);
	$$ = l->append_exp($2);
}


id_list:
expression TOKEN_COMMA expression %prec funEQ
{
  
  Expression* e = $1;
  assert(e->get_type() == AST_IDENTIFIER);       
  AstIdentifier* id = static_cast<AstIdentifier*>(e);
  AstIdentifierList* l = AstIdentifierList::make(id);
  Expression* e2 = $3;
  assert(e2->get_type() == AST_IDENTIFIER);       
  AstIdentifier* id2 = static_cast<AstIdentifier*>(e2);
  l = l->append_id(id2);
  

  $$ = l; 
}
|
id_list TOKEN_COMMA expression %prec funEQ
{
  Expression* _l = $1;
  assert(_l->get_type() == AST_IDENTIFIER_LIST);
  AstIdentifierList* l = static_cast<AstIdentifierList*>(_l);
  Expression* e = $3;  
  assert(e->get_type() == AST_IDENTIFIER);
  AstIdentifier* id = static_cast<AstIdentifier*>(e);
  $$ = l->append_id(id);
} 
