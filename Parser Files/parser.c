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
ParserInfo statement();
ParserInfo varDeclarStatement();
ParserInfo letStatement();
ParserInfo ifStatement();
ParserInfo whileStatement();
ParserInfo doStatement();
ParserInfo subroutineCall();
ParserInfo expressionList();
ParserInfo returnStatement();
ParserInfo expression();
ParserInfo relationalExpression();
ParserInfo ArithmeticExpression();
ParserInfo term();
ParserInfo factor();
ParserInfo operand();

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
		t = PeekNextToken();
		if (t.tp == SYMBOL && strcmp(t.lx, "}")  == 0) { // empty
			info.er = none;
			t = GetNextToken(); // get closing brace
			return info;
		}

		// check for statements
		info = statement();
		t = PeekNextToken();
		while (t.tp != SYMBOL && strcmp(t.lx, "}") != 0 && info.er == none) {
			info = statement();
			t = PeekNextToken();
		}

		if (info.er == none) { // no more statements
			t = GetNextToken(); // get closing brace
			if (t.tp == SYMBOL && strcmp(t.lx, "}")  == 0) {
				info.er = none;
			}
			else {
				info.tk = t;
				info.er = closeBraceExpected;
			}
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
	Token t = PeekNextToken();

	while (t.tp == RESWORD && (strcmp(t.lx, "var") == 0 || strcmp(t.lx, "let") == 0) || strcmp(t.lx, "return") == 0 || strcmp(t.lx, "if") == 0) {
		if (t.tp == RESWORD && strcmp(t.lx, "var") == 0) {
			info = varDeclarStatement();
		}
		else if (t.tp == RESWORD && strcmp(t.lx, "let") == 0) {
			info = letStatement();
		}
		else if (t.tp == RESWORD && strcmp(t.lx, "if") == 0) {
			info = ifStatement();
		}
		// else if (t.tp == RESWORD && strcmp(t.lx, "while") == 0) {
		// 	info = whileStatement();
		// }
		// else if (t.tp == RESWORD && strcmp(t.lx, "do") == 0) {
		// 	info = doStatement();
		// }
		else if (t.tp == RESWORD && strcmp(t.lx, "return") == 0) {
			info = returnStatement();
		}
		t = PeekNextToken();
	}

	return info;
}

// varDeclarStatement -> var type identifier { , identifier } ;
ParserInfo varDeclarStatement() {
	ParserInfo info;
	Token t = PeekNextToken();

	if (t.tp == RESWORD && strcmp(t.lx, "var") == 0) {
		GetNextToken(); // get peeked
		info = type(); // will get next if no error
		if (info.er == none) {
			t = GetNextToken(); // get identifier
			if (t.tp == ID) {
				t = GetNextToken(); // get next
				while (t.tp == SYMBOL && strcmp(t.lx, ",") == 0) {
					t = GetNextToken();
					if (t.tp == ID) {
						t = GetNextToken();
					}
					else {
						info.tk = t;
						info.er = idExpected;
						return info;
					}
				}
				if (t.tp == SYMBOL && strcmp(t.lx, ";") == 0) {
					info.er = none;
				}
				else {
					info.tk = t;
					info.er = semicolonExpected;
				}
			}
			else {
				info.tk = t;
				info.er = idExpected;
			}
		}
		else {
			info.tk = t;
			info.er = illegalType;
		}
	}
	else { // no var keyword
		info.er = syntaxError;
	}

	return info;
}

// letStatemnt -> let identifier [ [ expression ] ] = expression ;
ParserInfo letStatement() {
	ParserInfo info;
	Token t = PeekNextToken();

	if (t.tp == RESWORD && strcmp(t.lx, "let") == 0) {
		GetNextToken(); // get peeked
		t = GetNextToken(); // get identifier
		if (t.tp == ID) {
			t = PeekNextToken(); // get next
			if (t.tp == SYMBOL && strcmp(t.lx, "[") == 0) {
				GetNextToken(); // get peeked
				info = expression();
				if (info.er == none) {
					t = GetNextToken(); // get closing bracket
					if (!(t.tp == SYMBOL && strcmp(t.lx, "]") == 0)) { // no close brace
						info.tk = t;
						info.er = closeBracketExpected;
						return info;
					}
				}
			}
			t = GetNextToken(); // get equal sign
			if (t.tp == SYMBOL && strcmp(t.lx, "=") == 0) {
				info = expression();
				if (info.er == none) {
					t = GetNextToken(); // get semicolon
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
				info.er = equalExpected;
			}
		}
		else {
			info.tk = t;
			info.er = idExpected;
		}
	}	
	else { // no let keyword
		info.tk = t;
		info.er = syntaxError;
	}
	return info;
}

// ifStatement -> if ( expression ) { {statement} } [else { {statement} }]
ParserInfo ifStatement() {
	ParserInfo info;
	Token t = PeekNextToken();

	if (t.tp == RESWORD && strcmp(t.lx, "if") == 0) {
		GetNextToken(); // get peeked
		t = GetNextToken();
		if (t.tp == SYMBOL && strcmp(t.lx, "(") == 0) {
			info = expression();
			if (info.er == none) {
				t = GetNextToken();
				if (t.tp == SYMBOL && strcmp(t.lx, ")") == 0) {
					t = GetNextToken();
					if (t.tp == SYMBOL && strcmp(t.lx, "{") == 0) {
						t = PeekNextToken();
						if (t.tp != SYMBOL && strcmp(t.lx, "}") != 0) { // statement here
							info = statement();
							t = PeekNextToken();
							while (info.er == none && t.tp != SYMBOL && strcmp(t.lx, "}") != 0) {
								info = statement();
								t = PeekNextToken();
							}
							if (info.er != none) {
								return info;
							}
						}
						t = GetNextToken(); // get closing brace
						if (t.tp == SYMBOL && strcmp(t.lx, "}") == 0) {
							t = PeekNextToken();
							if (t.tp == RESWORD && strcmp(t.lx, "else") == 0) {
								GetNextToken(); // get peeked
								t = GetNextToken();
								if (t.tp == SYMBOL && strcmp(t.lx, "{") == 0) {
									t = PeekNextToken();
									if (t.tp != SYMBOL && strcmp(t.lx, "}") != 0) { // statement here
										info = statement();
										t = PeekNextToken();
										while (info.er == none && t.tp != SYMBOL && strcmp(t.lx, "}") != 0) {
											info = statement();
											t = PeekNextToken();
										}
										if (info.er != none) {
											return info;
										}
									}
									t = GetNextToken(); // get closing brace
									if (t.tp == SYMBOL && strcmp(t.lx, "}") == 0) {
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
							}
							else {
								info.er = none;
							}
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
				}
				else {
					info.tk = t;
					info.er = closeParenExpected;
				}
			}
		}
		else {
			info.tk = t;
			info.er = openParenExpected;
		}
	}
	return info;
}

// whileStatement -> while ( expression ) { {statement} }
ParserInfo whileStatement();

// doStatement -> do subroutineCall ;
ParserInfo doStatement();


// subroutineCall -> identifier [ . identifier ] ( expressionList )
ParserInfo subroutineCall();

// expressionList -> expression { , expression } | ε
ParserInfo expressionList() {
	ParserInfo info;
	Token t = PeekNextToken();

	if (t.tp == SYMBOL && strcmp(t.lx, ")") == 0 ) { // empty
		info.er = none;
		return info;
	}

	info = expression();
	while (info.er == none) {
		Token t = PeekNextToken();
		if (t.tp == SYMBOL && strcmp(t.lx, ",") == 0) {
			GetNextToken(); // get peeked
			info = expression();
		}
		else {
			break;
		}
	}

	return info;
}

// returnStatemnt -> return [ expression ] ;
ParserInfo returnStatement() {
	ParserInfo info;
	Token t = PeekNextToken();

	if (t.tp == RESWORD && strcmp(t.lx, "return") == 0) {
		GetNextToken(); // get peeked
		
		info = expression();

		if (info.er == none || info.er == syntaxError) {
			t = GetNextToken();
			if (t.tp == SYMBOL && strcmp(t.lx, ";") == 0) {
				info.er = none;
			}
			else {
				info.tk = t;
				printf("%s", t.lx);
				info.er = semicolonExpected;
			}
		}
	}
	return info;
}

// expression -> relationalExpression { ( & | | ) relationalExpression }
ParserInfo expression() { 
	ParserInfo info;

	info = relationalExpression();
	while (info.er == none) {
		Token t = PeekNextToken();
		if (t.tp == SYMBOL && (strcmp(t.lx, "&") == 0 || strcmp(t.lx, "|") == 0)) {
			GetNextToken(); // get peeked
			info = relationalExpression();
		}
		else {
			break;
		}
	}

	return info;
}

// relationalExpression -> ArithmeticExpression { ( = | > | < ) ArithmeticExpression }
ParserInfo relationalExpression() {
	ParserInfo info;

	info = ArithmeticExpression();
	while (info.er == none) {
		Token t = PeekNextToken();
		if (t.tp == SYMBOL && (strcmp(t.lx, "=") == 0 || strcmp(t.lx, ">") == 0 || strcmp(t.lx, "<") == 0)) {
			GetNextToken(); // get peeked
			info = ArithmeticExpression();
		}
		else {
			break;
		}
	}
	return info;
}


// ArithmeticExpression -> term { ( + | - ) term }
ParserInfo ArithmeticExpression() {
	ParserInfo info;

	info = term();
	while (info.er == none) {
		Token t = PeekNextToken();
		if (t.tp == SYMBOL && (strcmp(t.lx, "+") == 0 || strcmp(t.lx, "-") == 0)) {
			GetNextToken(); // get peeked
			info = term();
		}
		else {
			break;
		}
	}
	return info;
}

// term -> factor { ( * | / ) factor }
ParserInfo term() {
	ParserInfo info;

	info = factor();
	while (info.er == none) {
		Token t = PeekNextToken();
		if (t.tp == SYMBOL && (strcmp(t.lx, "*") == 0 || strcmp(t.lx, "/") == 0)) {
			GetNextToken(); // get peeked
			info = factor();
		}
		else {
			break;
		}
	}
	return info;
}

// factor -> ( - | ~ | ε ) operand
ParserInfo factor() {
	ParserInfo info;
	Token t = PeekNextToken();

	if (t.tp == SYMBOL && (strcmp(t.lx, "-") == 0 || strcmp(t.lx, "~") == 0)) {
		GetNextToken(); // get peeked
		info = operand();
	}
	else {
		info = operand();
	}

	return info;
}

// operand -> integerConstant | identifier [.identifier ] [ [ expression ] | (expressionList ) ] | (expression) | stringLiteral | true | false | null | this
ParserInfo operand() {
	ParserInfo info;
	Token t = PeekNextToken();
	
	if (t.tp == INT) {
		GetNextToken(); // get peeked
		info.er = none;
	}
	else if (t.tp == ID) {
		GetNextToken(); // get peeked
		t = PeekNextToken();
		if (t.tp == SYMBOL  && strcmp(t.lx, ".") == 0) {
			GetNextToken(); // get peeked
			t = GetNextToken(); // get identifier
			if (t.tp == ID) {
				info.er = none;
				t = PeekNextToken();
			}
			else {
				info.tk = t;
				info.er = idExpected;
				return info;
			}
		}
		if (t.tp == SYMBOL && strcmp(t.lx, "[") == 0) {
			GetNextToken(); // get peeked
			info = expression();
			if (info.er == none) {
				t = GetNextToken(); // get closing bracket
				if (t.tp == SYMBOL && strcmp(t.lx, "]") == 0) {
					info.er = none;
				}
				else {
					info.tk = t;
					info.er = closeBracketExpected;
				}
			}
		}
		else if (t.tp == SYMBOL && strcmp(t.lx, "(") == 0) {
			GetNextToken(); // get peeked
			info = expressionList();
			if (info.er == none) {
				t = GetNextToken(); // get closing brace
				if (t.tp == SYMBOL && strcmp(t.lx, ")") == 0) {
					info.er = none;
				}
				else {
					info.tk = t;
					info.er = closeBraceExpected;
				}
			}
		}
	}
	else if (t.tp == SYMBOL && strcmp(t.lx, "(") == 0) {
		GetNextToken(); // get peeked
		info = expression();
		if (info.er == none) {
			t = GetNextToken(); // get closing brace
			if (t.tp == SYMBOL && strcmp(t.lx, ")") == 0) {
				info.er = none;
			}
			else {
				info.tk = t;
				info.er = closeBraceExpected;
			}
		}
	}
	else if (t.tp == STRING) {
		info.er = none;
	}
	else if (t.tp == RESWORD && (strcmp(t.lx, "true") == 0 || strcmp(t.lx, "false") == 0 || strcmp(t.lx, "null") == 0 || strcmp(t.lx, "this") == 0)) {
		info.er = none;
	}
	else { // nothing
		info.tk = t;
		info.er = syntaxError;
	}

	return info;
}



int InitParser(char* file_name)
{
	InitLexer(file_name);

	return 1;
}

ParserInfo Parse()
{
	ParserInfo info;

	// implement the function

	// everything is contained in class in jack (i think)
	info = classDeclar();


	return info;
}


int StopParser()
{
	StopLexer();
	return 1;
}

#ifndef TEST_PARSER
int main()
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
