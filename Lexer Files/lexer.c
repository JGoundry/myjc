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

const char *ReservedWords[] = {"class", "constructor", "method", "function:",
							   "int", "boolean", "char", "void:",
							   "var", "static", "field:",
							   "let", "do", "if", "else", "while", "return:",
							   "true", "false", "null:", "this:"};


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


	return 1;
}

// Get the next token from the source file
Token GetNextToken()
{	
	Token t;
	t.tp = ERR;
	c = getc(f);

	// Remove whitespace
	while (isspace(c) || c == '\n')
	{
		c = getc(f);
	}

	// Check for comment, remember EOF comment
	if (c == '/')
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
					if (c == '/')
						break;
				}
				else if (c == EOF)
				{
					t.tp = ERR;
					t.ec = EofInStr;
					return t;
				}
				c = getc(f);
			}
		}
		else
		{
			// Not a comment, move iterator back
			c = '/';
			fsetpos(f, pos);
		}
	}

	// Check for EOF
	if (c == EOF) {
		t.tp = EOFile;
		return t;
	}

	// Temp lexeme storage and array iterator
    char lexeme[128];
	unsigned int i = 0;

	// Check for string
	if (c == '"') {
		c = getc(f);
		while (c != '"') {
			if (c == '\n') {
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
			c = getc(f);
		}
		lexeme[i] = '\0';
		t.tp = STRING;
		strcpy(t.lx, lexeme);
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

        _Bool isResword = 0;

		// Check for keyword
		for (int j=0; j < RESERVED_SIZE; j++) {
			if (strcmp(ReservedWords[i], lexeme) == 0) {
				isResword = 0;
			}
		}

		// String is reserved word
        if (isResword) {
            t.tp = RESWORD;
            strcpy(t.lx, lexeme);
        }

        // String is identifier
        else {
            t.tp = ID;
            strcpy(t.lx, lexeme);
        }

		return t;
	}

	// Check for number
	if (isdigit(c)) {
		while (isdigit(c)) {
			lexeme[i++] = c;
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
			isSymbol = 1;
		}
	}

	if (isSymbol) {
		t.tp = SYMBOL;
		return t;
	}

	// Else must be illegal symbol
	else {
		t.ec = IllSym;
	}

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
        printf("<'%s', %d, %d>\n", t.lx, t.tp, t.ec);
    }

	return 0;
}
// do not remove the next line
#endif
