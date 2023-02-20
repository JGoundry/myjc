/************************************************************************
University of Leeds
School of Computing
COMP2932- Compiler Design and Construction
Lexer Module

I confirm that the following code has been developed and written by me and it is entirely the result of my own work.
I also confirm that I have not copied any parts of this program from another person or any other source or facilitated someone to copy this program from me.
I confirm that I will not publish the program online or share it with anyone without permission of the module leader.

Student Name: Josh Goundry
Student ID:
Email: sc21j2lg@leeds.ac.uk
Date Work Commenced:
*************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "lexer.h"

// YOU CAN ADD YOUR OWN FUNCTIONS, DECLARATIONS AND VARIABLES HERE

#define RESERVED_SIZE 21
#define SYMBOL_SIZE 19

// File pointer and character iterator
FILE *f;
int c;
int LineNumber;

const char *ReservedWords[] = {"class", "constructor", "method", "function:",
							   "int", "boolean", "char", "void",
							   "var", "static", "field",
							   "let", "do", "if", "else", "while", "return",
							   "true", "false", "null", "this"};


const char Symbols[] = {'(', ')', '[', ']', '{', '}', ',', ';', '=', '.',
						'+', '-', '*', '/', '&', '|', '~', '<', '>'};


// IMPLEMENT THE FOLLOWING functions
//***********************************

// Initialise the lexer to read from source file
// file_name is the name of the source file
// This requires opening the file and making any necessary initialisations of the lexer
// If an error occurs, the function should return 0
// if everything goes well the function should return 1
int InitLexer(char *file_name)
{
	f = fopen(file_name, "r");
	if (f == 0)
	{ // failure
		return 0;
	}
	LineNumber = 1;

	return 1;
}

_Bool BreakLoop = 0;

// Get the next token from the source file
Token GetNextToken()
{	
	Token t;
	t.tp = ERR;

	c = getc(f);

	BreakLoop = 0;

	while (isspace(c) || c == '\n' || c == '/') {		
		// Remove whitespace
		while (isspace(c) || c == '\n')
		{
			if (c == '\n') {
				LineNumber++;
			}

			c = getc(f);
		}

		if (BreakLoop) {
			break;
		}

		// Check for comment, remember EOF comment
		while (c == '/')
		{
			// Save current pos
			fpos_t *pos;
			fgetpos(f, pos);

			c = getc(f);

			// Comment to end of line
			if (c == '/')
			{
				while (c != '\n' && c != EOF)
				{
					c = getc(f);
				}
				if (c == EOF)
				{
					t.tp = ERR;
					t.ec = EofInStr;
					return t;
				}
				else {
					// New Line
					LineNumber++;
					c = getc(f);
				}
			}

			// Comment until closing
			else if (c == '*')
			{
				c = getc(f);

				if (c == '*')
				{
					// API documentation comment
					c = getc(f);
				}


				while (1)
				{
					if (c == '*')
					{
						c = getc(f);
						if (c == '/') { // end of comment
							c = getc(f);
							BreakLoop = 1;
							break;
						}
						else {
							continue;
						}
					}
					else if (c == EOF)
					{
						t.tp = ERR;
						t.ec = EofInCom;
						return t;
					}
					if (c == '\n')
						LineNumber++;

					c = getc(f);	
				}
			}
			else
			{
				// Not a comment, move iterator back
				c = '/';
				fsetpos(f, pos);
				BreakLoop = 1;
				break;
			}
		}
	}

		// Temp lexeme storage and array iterator
		char lexeme[128];
		unsigned int i = 0;

		// Check for string
		if (c == '"') {
			c = getc(f);
			while (c != '"') {
				if (c == '\n') {
					LineNumber++;
					t.tp = ERR;
					t.ec = NewLnInStr;
					return t;
				}
				else if (c == EOF) {
					t.tp = ERR;
					t.ec =EofInStr;
					return t;
				}
				lexeme[i++] = c;
				// c = getc(f);
			}
			lexeme[i] = '\0';
			t.tp = STRING;
			strcpy(t.lx, lexeme);
			
			return t;
		}

		// Check for reserved words and id
		// **** NO CHECK FOR EOF OR NEWLN ****
		if (isalpha(c) || c == '_') {

			// Read in string
			while (isalnum(c) || c == '_') {
				lexeme[i++] = c;
				c = getc(f);
			}
			lexeme[i] = '\0';


			// Check for keyword, return if is
			for (int j=0; j < RESERVED_SIZE; j++) {
				if (strcmp(ReservedWords[j], lexeme) == 0) {
					t.tp = RESWORD;
					strcpy(t.lx, lexeme);
					return t;
				}
			}

			// String is identifier
			t.tp = ID;
			strcpy(t.lx, lexeme);
			return t;
		}
		// Check for number
		if (isdigit(c)) {
			while (isdigit(c)) {
				lexeme[i++] = c;
				c = getc(f);
			}
			lexeme[i] = '\0';
			t.tp = INT;
			strcpy(t.lx, lexeme);
			return t;
		}


		// Check for symbol
		lexeme[0] = c;
		lexeme[1] = '\0';
		strcpy(t.lx, lexeme);

		_Bool isSymbol = 0;
		for (int j=0; j < SYMBOL_SIZE; j++) {
			if (Symbols[j] == c) {
				t.tp = SYMBOL;
				c = getc(f);
				return t;
			}
		}

		// Else must be illegal symbol

		if (c == EOF) {
			t.tp = EOFile;
			return t;
		}

	// Else it must be illegal symbol
	t.ec = IllSym;
	c = getc(f);
	return t;
	
}

// peek (look) at the next token in the source file without removing it from the stream
Token PeekNextToken()
{
	Token t;
	t.tp = ERR;

	// Peek current pos in file

	// Call GetNextToken

	// Reset to old pos in file

	return t;
}

// clean out at end, e.g. close files, free memory, ... etc
int StopLexer()
{
	fclose(f);
	return 0;
}

// do not remove the next line
#ifndef TEST

const char *TokenTypeArr[] = { "RESWORD", "ID", "INT", "SYMBOL", "STRING", "EOFile", "ERR" };

int main(int argc, char *argv[])
{
	// NOTE: the autograder will not use your main function

	if (argc != 2) {
        printf("Usage: myjc source\n");
        return 1;
    }

    InitLexer(argv[1]);

    while (1) {
        Token t = GetNextToken();
        if (t.tp == EOFile)
            break;
        printf("< Ball.jack, %d, %s, %s >\n", LineNumber, t.lx, TokenTypeArr[t.tp]);
    }

	return 0;
}
// do not remove the next line
#endif

/*

	1. Remove white space (+ newline)

	2. Check for comments

	3. Look for strings (can return)

	4. Check for reserved words and id (can return)

	5. check for symbols (can return)

	5.

*/