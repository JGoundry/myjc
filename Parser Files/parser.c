#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "lexer.h"
#include "parser.h"


// you can declare prototypes of parser functions below

ParserInfo classDeclar();
ParserInfo memberDeclar();
ParserInfo classVarDeclar();
ParserInfo type();
ParserInfo subroutineDeclar();
ParserInfo paramList();
ParserInfo subroutineBody();
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

// classDeclar -> class identifier { {memberDeclar} }
ParserInfo classDeclar() {
	ParserInfo info;
	Token t = PeekNextToken();
	
	
	if (t.tp == RESWORD && strcmp(t.lx, "class") == 0) {
		GetNextToken();
		t = GetNextToken();
		if (t.tp == ID) { // Identifier
			t = GetNextToken();
			if (t.tp == SYMBOL && strcmp(t.lx, "{") == 0) { // Open bracket

				// Zero or more memberDeclar
				info = memberDeclar();
				while (info.er == none) {
					info = memberDeclar();
				}
				if (info.er == memberDeclarErr) {
					t = GetNextToken();
					if (t.tp == SYMBOL && strcmp(t.lx, "}") == 0) { // Close bracket
						info.er = none;
					}
					else {
						info.tk = t;
						info.er = closeBraceExpected;
					}
				}
			}
			else { // No open bracket
				info.tk = t;
				info.er = openBraceExpected;
			}
		}
		else { // No identifier
			info.tk = t;
			info.er = idExpected;
		}
	}
	else { // No class
		info.tk = t;
		info.er = classExpected;
	}

	return info;
}

// memberDeclar -> classVarDeclar | subroutineDeclar
ParserInfo memberDeclar() {

	ParserInfo info;


	// Temp until subroutineDeclar implemented
	// info = classVarDeclar();
	// 	if (info.er == classVarErr) {
	// 		info.er = memberDeclarErr;
	// 	}

	info = classVarDeclar();
	if (info.er == classVarErr) {
		info = subroutineDeclar();
		if (info.er == subroutineDeclarErr) {
			info.er = memberDeclarErr;
		}
	}

	return info;
}

// classVarDeclar -> (static | field) type identifier {, identifier} ;
ParserInfo classVarDeclar() {
	ParserInfo info;
	Token t = PeekNextToken();

	if (t.tp == RESWORD && (strcmp(t.lx, "static") == 0 ||  (strcmp(t.lx, "field") == 0))) {
		GetNextToken();
		info = type();
		if (info.er == none) {
			t = GetNextToken();
			if (t.tp == ID) {
				t = GetNextToken();
				// Need to check for other IDS
				while (t.tp == SYMBOL && strcmp(t.lx, ",") == 0) {
					t = GetNextToken();
					if (t.tp == ID) {
						t = GetNextToken();
					}
					else { // Expected identifier
						info.tk = t;
						info.er = idExpected;
					}
				}
				if (info.er != idExpected) {
					if (t.tp == SYMBOL && strcmp(t.lx, ";") == 0) {
						info.er = none;
					}
					else {
						info.tk = t;
						info.er = semicolonExpected;
					}
				}
			}
			else {
				info.tk = t;
				info.er = idExpected;
			}
		}
	}
	else { // No static or field
		info.tk = t;
		info.er = classVarErr;
	}
	return info;
}

// type -> int | char | boolean | identifier
ParserInfo type() {
	ParserInfo info;
	Token t = PeekNextToken();

	if (t.tp == ID || strcmp(t.lx, "int") == 0 || strcmp(t.lx, "char") == 0 || strcmp(t.lx, "boolean") == 0) {
			GetNextToken();
			info.er = none;
		}
	else {
		info.tk = t;
		info.er = illegalType;
	}

	return info;
}

// subroutineDeclar -> (constructor | function | method) (type|void) identifier (paramList) subroutineBody
ParserInfo subroutineDeclar() {
	ParserInfo info;
	Token t = PeekNextToken();

	if (t.tp == RESWORD && (strcmp(t.lx, "constructor") == 0 || strcmp(t.lx, "function") == 0 || strcmp(t.lx, "method") == 0)) {
		GetNextToken();	// get peeked token
		info = type();
		if (info.er == illegalType) { // no type
			t = PeekNextToken();
			if (strcmp(t.lx, "void") == 0) { // check for void
				info.er = none;
			}
		}
		if (info.er == none) { // no errors
			GetNextToken(); // get (type|void) that was peeked
			t = GetNextToken(); // get identifier
			if (t.tp == ID) { // id
				t = GetNextToken();
				if (t.tp == SYMBOL && strcmp(t.lx, "(") == 0) {	// open bracket
					info = paramList(); // param list
					if (info.er == none) { // no param list error
						t = GetNextToken();
						if (t.tp == SYMBOL && strcmp(t.lx, ")") == 0) { // close bracket
							info = subroutineBody(); // subroutine body
							if (info.er == none) {
								info.er = none;
							}
						}
						else {
							info.tk = t;
							info.er = closeParenExpected;
						}
					}
				}
				else { // no open bracket
					info.tk = t;
					info.er = openParenExpected;
				}
			}
			else {
				info.tk = t;
				info.er = idExpected;
			}
		}

	}
	else {
		info.tk = t;
		info.er = subroutineDeclarErr;
	}

	return info;

}

// paramList -> type identifier {, type identifier} | ε
ParserInfo paramList() {
	ParserInfo info;
	Token t = PeekNextToken();

	if (t.tp == SYMBOL && strcmp(t.lx, ")") == 0 ) { // empty
		info.er = none;
		return info;
	}

	info = type();
	if (info.er == none) {
		Token t = GetNextToken();
		if (t.tp == ID) {
			t = GetNextToken();
			while (t.tp == SYMBOL && strcmp(t.lx, ",") == 0) {
				info = type();
				if (info.er == none) {
					t = GetNextToken();
					if (t.tp == ID) {
						t = GetNextToken();
					}
					else {
						info.tk = t;
						info.er = idExpected;
						break;
					}
				}
				else {
					break;
				}
			}
		}
		else {
			info.tk = t;
			info.er = idExpected;
		}
	}
	return info;
}

// subroutineBody -> { {statement} }
ParserInfo subroutineBody() {
	ParserInfo info;
	Token t = PeekNextToken();

	if (t.tp == SYMBOL && strcmp(t.lx, "{")  == 0) {
		GetNextToken(); // get peeked

		// statement zero or more loop
		
		t = GetNextToken(); // get closing brace
		if (t.tp == SYMBOL && strcmp(t.lx, "}")  == 0) {
			info.er = none;
		}
		else {
			info.tk = t;
			info.er = closeBraceExpected;
		}
	}
	else {
		info.tk = t;
		info.er = openBraceExpected;
	}
	
	return info;
}

// statement -> varDeclarStatement | letStatemnt | ifStatement | whileStatement | doStatement | returnStatemnt
ParserInfo statement() {
	ParserInfo info;
	info.er = none;

	return info;
}

int InitParser (char* file_name)
{
	InitLexer(file_name);

	return 1;
}

ParserInfo Parse ()
{
	ParserInfo info;

	// implement the function

	// everything is contained in class in jack (i think)
	info = classDeclar();


	return info;
}


int StopParser ()
{
	StopLexer();
	return 1;
}

#ifndef TEST_PARSER
int main ()
{
	InitParser("myTest.jack");

	ParserInfo info;

	info = Parse();

	if (info.er == none)
		printf("good\n");
	else
		printf("Error: %d", info.er);

	StopParser();

	return 1;
}
#endif
