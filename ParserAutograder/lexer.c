/************************************************************************
University of Leeds
School of Computing
COMP2932- Compiler Design and Construction
Lexer Module

I confirm that the following code has been developed and written by me and it is
entirely the result of my own work. I also confirm that I have not copied any
parts of this program from another person or any other source or facilitated
someone to copy this program from me. I confirm that I will not publish the
program online or share it with anyone without permission of the module leader.

Student Name: Josh Goundry
Student ID:
Email: sc21j2lg@leeds.ac.uk
Date Work Commenced:
*************************************************************************/

#include "lexer.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// YOU CAN ADD YOUR OWN FUNCTIONS, DECLARATIONS AND VARIABLES HERE

#define RESERVED_SIZE 21
#define SYMBOL_SIZE 19

// File pointer and character iterator
FILE *f;
int c;
int LineNumber;
_Bool startOfFile;
char *fileName;

const char *ReservedWords[] = {
    "class", "constructor", "method", "function", "int",   "boolean", "char",
    "void",  "var",         "static", "field",    "let",   "do",      "if",
    "else",  "while",       "return", "true",     "false", "null",    "this"};

const char Symbols[] = {'(', ')', '[', ']', '{', '}', ',', ';', '=', '.',
                        '+', '-', '*', '/', '&', '|', '~', '<', '>'};

// IMPLEMENT THE FOLLOWING functions
//***********************************

// Initialise the lexer to read from source file
// file_name is the name of the source file
// This requires opening the file and making any necessary initialisations of
// the lexer If an error occurs, the function should return 0 if everything goes
// well the function should return 1
int InitLexer(char *file_name) {
  f = fopen(file_name, "r");
  if (f == 0) { // failure
    return 0;
  }
  LineNumber = 1;
  startOfFile = 1;

  fileName = file_name;

  return 1;
}

_Bool BreakLoop = 0;

// Get the next token from the source file
Token GetNextToken() {
  Token t;
  strcpy(t.fl, fileName);
  t.tp = ERR;

  if (startOfFile) {
    c = getc(f);
    startOfFile = 0;
  }

  BreakLoop = 0; // used to break out of double loop

  while (isspace(c) || c == '\n' || c == '/') {
    // Remove whitespace
    while (isspace(c) || c == '\n') {
      if (c == '\n') {
        LineNumber++;
      }
      c = getc(f);
    }

    if (BreakLoop) {
      break;
    }

    // Check for comment, remember EOF comment
    while (c == '/') {
      // Save current pos
      fpos_t pos;
      fgetpos(f, &pos);

      c = getc(f);

      // Comment to end of line
      if (c == '/') {
        while (c != '\n' && c != EOF) {
          c = getc(f);
        }
        if (c == EOF) {
          t.tp = ERR;
          t.ec = EofInStr;
          t.ln = LineNumber;
          return t;
        } else {
          // New Line
          LineNumber++;
          c = getc(f);
        }
      }

      // Comment until closing
      else if (c == '*') {
        c = getc(f);
        if (c == '*') {
          // API documentation comment
          c = getc(f);
        }

        while (1) {
          if (c == '*') {
            c = getc(f);
            if (c == '/') { // end of comment
              c = getc(f);
              break;
            } else {
              continue;
            }
          } else if (c == EOF) {
            t.tp = ERR;
            t.ec = EofInCom;
            strcpy(t.lx, "Error: unexpected eof in comment\0");
            t.ln = LineNumber;
            return t;
          }
          if (c == '\n')
            LineNumber++;

          c = getc(f);
        }
      } else {
        // Not a comment, move iterator back
        c = '/';
        fsetpos(f, &pos);
        BreakLoop = 1;
        break;
      }
    }
  }

  // Temp lexeme storage and array iterator
  char lexeme[128];
  unsigned int i = 0;
  fpos_t *pos;
  int temp;

  // Check for string
  if (c == '"') {
    c = getc(f);
    while (c != '"') {
      if (c == '\n') {
        t.tp = ERR;
        t.ec = NewLnInStr;
        strcpy(t.lx, "Error: new line in string constant\0");
        t.ln = LineNumber;
        LineNumber++;
        return t;
      } else if (c == EOF) {
        t.tp = ERR;
        t.ec = EofInStr;
        strcpy(t.lx, "Error: unexpected eof in string constant\0");
        t.ln = LineNumber;
        return t;
      }
      lexeme[i++] = c;
      c = getc(f);
    }
    c = getc(f); // skip "
    lexeme[i] = '\0';
    t.tp = STRING;
    strcpy(t.lx, lexeme);
    t.ln = LineNumber;

    return t;
  }

  // Check for reserved words and id
  if (isalpha(c) || c == '_') {

    // Read in string
    while (isalnum(c) || c == '_') {
      lexeme[i++] = c;
      c = getc(f);
    }
    lexeme[i] = '\0';

    // Check for keyword, return if is
    for (int j = 0; j < RESERVED_SIZE; j++) {
      if (strcmp(ReservedWords[j], lexeme) == 0) {
        t.tp = RESWORD;
        strcpy(t.lx, lexeme);
        t.ln = LineNumber;
        return t;
      }
    }

    // String is identifier
    t.tp = ID;
    strcpy(t.lx, lexeme);
    t.ln = LineNumber;
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
    t.ln = LineNumber;
    return t;
  }

  // Check for symbol
  lexeme[0] = c;
  lexeme[1] = '\0';
  strcpy(t.lx, lexeme);

  _Bool isSymbol = 0;
  for (int j = 0; j < SYMBOL_SIZE; j++) {
    if (Symbols[j] == c) {
      c = getc(f);
      t.tp = SYMBOL;
      t.ln = LineNumber;
      return t;
    }
  }

  if (c == EOF) {
    strcpy(t.lx, "End of File\0");
    t.tp = EOFile;
    t.ln = LineNumber;
    return t;
  }

  // Else it must be illegal symbol
  t.ec = IllSym;
  c = getc(f);
  strcpy(t.lx, "Error: illegal symbol in source file\0");
  t.ln = LineNumber;
  return t;
}

// peek (look) at the next token in the source file without removing it from the
// stream
Token PeekNextToken() {
  Token t;

  int tempC = c;
  int tempLineNumber = LineNumber;
  _Bool tempStartOfFile = startOfFile;

  // Save current pos
  fpos_t pos;
  fgetpos(f, &pos);

  t = GetNextToken();

  fsetpos(f, &pos);

  c = tempC;
  LineNumber = tempLineNumber;
  startOfFile = tempStartOfFile;

  return t;
}

// clean out at end, e.g. close files, free memory, ... etc
int StopLexer() {
  fclose(f);
  // free(pos);
  return 0;
}

// // do not remove the next line
// #ifndef TEST

// const char *TokenTypeArr[] = { "RESWORD", "ID", "INT", "SYMBOL", "STRING",
// "EOFile", "ERR" };

// int main(int argc, char *argv[])
// {
// 	// NOTE: the autograder will not use your main function

// 	if (argc != 2) {
//         printf("Usage: myjc source\n");
//         return 1;
//     }

//     InitLexer(argv[1]);

//     while (1) {
// 		Token t = GetNextToken();
//         printf("< %s, %d, %s, %s >\n", t.fl, LineNumber, t.lx,
//         TokenTypeArr[t.tp]);
// 		if (t.tp == EOFile)
//             break;
// 	}

// 	return 0;
// }
// // do not remove the next line
// #endif