#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "lexer.h"
#include "parser.h"


// you can declare prototypes of parser functions below

// each grammar rule is a function

// classDeclar
// memberDeclar
// classVarDeclar
// type
// subroutineDeclar
// paramList
// subroutineBody
// statement
// varDeclarStatement
// letStatement
// ifStatement
// whileStatement
// doStatement
// subroutineCall
// expressionList
// returnStatement
// expression
// relationalExpression
// ArithmeticExpression
// term
// factor
// operand


int InitParser (char* file_name)
{
	InitLexer(file_name);

	return 1;
}

ParserInfo Parse ()
{
	ParserInfo pi;

	// implement the function

	Token t;
	t = GetNextToken();

	printf("Token: %s\n", t.lx);


	pi.er = none;
	return pi;
}


int StopParser ()
{
	StopLexer();
	return 1;
}

#ifndef TEST_PARSER
int main ()
{
	InitParser("Ball.jack");
	Parse();
	StopParser();

	return 1;
}
#endif
