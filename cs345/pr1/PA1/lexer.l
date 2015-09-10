%{


#include "parser-defs.h"
#include "parser.tab.h"
/*
 * You may want to use this string to assemble the lexeme
 * of string constants by adding the current matched character from yytext to it.
 * You can do this as buffer += yytext;
 */
string buffer;


/*
 * You may want to track the comment desting depth with this variable.
 */
int comment_depth = 0;

%}


%option noyywrap
%option stack
WHITESPACE [ \t\f\r\v]+
NEWLINE [\n]
COMMA [,]
PLUS [+]
MINUS [-]
TIMES [*]
DIVIDE [/]
LPAREN [(]
RPAREN [)]
INTEGER [0-9]*
AND [&]
OR [|]
EQ [=]
NEQ ("<>")
GT [>]
GEQ (">=")
LT [<]
LEQ ("<=")
HD [!]
TL [#]
CONS [@]
LET (?i:let)
IN (?i:in)
FUN (?i:fun)
WITH (?i:with)
LAMBDA (?i:lambda)
IF (?i:if)
THEN (?i:then)
ELSE (?i:else)
PRINT (?i:print)   
READINT (?i:readint)
READSTRING (?i:readstring)
ISNIL (?i:isnil)
NIL (?i:nil)
DOT (".")


IDENTIFIER [_[:alpha:]]+[_[:alnum:]]*


%x STRING
%x COMMENT 
%x KEYWORD

%%
"(*" yy_push_state(COMMENT);
"*)" {
	return TOKEN_ERROR;
	yyterminate();
}
\"   BEGIN(STRING);

<STRING>\" {
  SET_LEXEME(buffer); 
  BEGIN(INITIAL);
  buffer = "";
  return TOKEN_STRING;
}
<STRING>. {
 buffer += yytext[0];
}
<STRING>\n {
 buffer += yytext[0];
 curr_lineno++;
}
<STRING><<EOF>> {
  return TOKEN_ERROR;
  yyterminate();
}
<COMMENT>"(*" { 
  yy_push_state(COMMENT);
}
<COMMENT>"*"+")" {
  yy_pop_state();
}
<COMMENT>.
<COMMENT>\n curr_lineno++;
<COMMENT><<EOF>> {
  return TOKEN_ERROR;
  yyterminate();
}
{WHITESPACE} {
  /* Do nothing */
}

{NEWLINE} {
/* Do nothing, but increment line numbers */
 curr_lineno++;
}

{COMMA} {
  return TOKEN_COMMA;
}
{PLUS} {
  return TOKEN_PLUS;
}
{MINUS} {
  return TOKEN_MINUS;
}
{TIMES} {
  return TOKEN_TIMES;
}
{DIVIDE} {
  return TOKEN_DIVIDE;
}
{INTEGER} {
  SET_LEXEME(yytext);
  return TOKEN_INT;
}
{LPAREN} {
  return TOKEN_LPAREN;
}
{RPAREN} {
  return TOKEN_RPAREN;
}
{AND} {
  return TOKEN_AND;
}
{OR} {
  return TOKEN_OR;
}
{EQ} {
  return TOKEN_EQ;
}
{NEQ} {
  return TOKEN_NEQ;
}
{GEQ} {
  return TOKEN_GEQ;
}
{GT} {
  return TOKEN_GT;
}
{LEQ} {
  return TOKEN_LEQ;
}
{LT} {
  return TOKEN_LT;
}
{CONS} {
  return TOKEN_CONS;
}
{HD} {
  return TOKEN_HD;
}
{TL} {
  return TOKEN_TL;
}
{DOT} {
  return TOKEN_DOT;
}
{READSTRING} {
  return TOKEN_READSTRING;
}
{READINT} {
  return TOKEN_READINT;
}
{LET} {
  return TOKEN_LET;
}
{IN} {
  return TOKEN_IN;
}
{FUN} {
  return TOKEN_FUN;
}
{WITH} {
  return TOKEN_WITH;
}
{LAMBDA} {
  return TOKEN_LAMBDA;
}
{IF} {
  return TOKEN_IF;
}
{THEN} {
  return TOKEN_THEN;
}
{ELSE} {
  return TOKEN_ELSE;
}
{PRINT} {
  return TOKEN_PRINT;
}
{ISNIL} {
  return TOKEN_ISNIL;
}
{NIL} {
  return TOKEN_NIL;
}

{IDENTIFIER} {
  SET_LEXEME(yytext);
  return TOKEN_IDENTIFIER;
}
. {
  /* Leave this rule at the end of our lexer to "eat up" all illegal strings */
  SET_LEXEME("Unexpected character in input");
  return TOKEN_ERROR;
}



