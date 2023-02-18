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

// File pointer and character iterator
FILE *f;
int c;

const char *ReservedWords[] = {"class", "constructor", "method", "function:",
							   "int", "boolean", "char", "void:",
							   "var", "static", "field:",
							   "let", "do", "if", "else", "while", "return:",
							   "true", "false", "null:", "this:"};

const char *Symbols[] = {"(", ")", "[", "]", "{", "}", ",", ";", "=", ".",
						 "+", "-", "*", "/", "&", "|", "~", "<", ">"};

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
	while (isspace(c))
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
			else
			{
				// Skip newline
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
					if (c == '/')
						break;
				}
				else if (c == EOF)
				{
					t.tp = ERR;
					t.ec = EofInStr;
					return t;
				}
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

	// Temp lexeme storage
    char lexeme[128];

	// Check for string (surrounded with double quotes) EofInStr & NewLnInStr, rememember string lex error codes
	if (c == '"'){

	}

	// Check for reserved words and id (identifier must start with _ or letter)
	if (isalpha(c) || c == '_') {

	}

	// Check for symbol

	// Else must be illegal symbol

	// Check for
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
	return 0;
}

// do not remove the next line
#ifndef TEST
int main()
{
	// implement your main function here
	// NOTE: the autograder will not use your main function

	return 0;
}
// do not remove the next line
#endif
