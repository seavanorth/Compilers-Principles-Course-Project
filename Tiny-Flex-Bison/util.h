/****************************************************/
/* File: util.h                                     */
/* Utility functions for the TINY compiler          */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#ifndef _UTIL_H_
#define _UTIL_H_

/* MAXTOKENLEN is the maximum size of a token */
#define MAXTOKENLEN 40

/* tokenString array stores the lexeme of each token */
extern char tokenString[MAXTOKENLEN+1];

TokenType getToken(void);

void printToken( TokenType, const char* );

void TokenOut();

TreeNode * newStmtNode(StmtKind);

TreeNode * newExpNode(ExpKind);

char * copyString( char * );

void printTree( TreeNode * );

TreeNode * parse(void);
#endif