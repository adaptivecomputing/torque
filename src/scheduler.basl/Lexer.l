/*
*         OpenPBS (Portable Batch System) v2.3 Software License
* 
* Copyright (c) 1999-2000 Veridian Information Solutions, Inc.
* All rights reserved.
* 
* ---------------------------------------------------------------------------
* For a license to use or redistribute the OpenPBS software under conditions
* other than those described below, or to purchase support for this software,
* please contact Veridian Systems, PBS Products Department ("Licensor") at:
* 
*    www.OpenPBS.org  +1 650 967-4675                  sales@OpenPBS.org
*                        877 902-4PBS (US toll-free)
* ---------------------------------------------------------------------------
* 
* This license covers use of the OpenPBS v2.3 software (the "Software") at
* your site or location, and, for certain users, redistribution of the
* Software to other sites and locations.  Use and redistribution of
* OpenPBS v2.3 in source and binary forms, with or without modification,
* are permitted provided that all of the following conditions are met.
* After December 31, 2001, only conditions 3-6 must be met:
* 
* 1. Commercial and/or non-commercial use of the Software is permitted
*    provided a current software registration is on file at www.OpenPBS.org.
*    If use of this software contributes to a publication, product, or
*    service, proper attribution must be given; see www.OpenPBS.org/credit.html
* 
* 2. Redistribution in any form is only permitted for non-commercial,
*    non-profit purposes.  There can be no charge for the Software or any
*    software incorporating the Software.  Further, there can be no
*    expectation of revenue generated as a consequence of redistributing
*    the Software.
* 
* 3. Any Redistribution of source code must retain the above copyright notice
*    and the acknowledgment contained in paragraph 6, this list of conditions
*    and the disclaimer contained in paragraph 7.
* 
* 4. Any Redistribution in binary form must reproduce the above copyright
*    notice and the acknowledgment contained in paragraph 6, this list of
*    conditions and the disclaimer contained in paragraph 7 in the
*    documentation and/or other materials provided with the distribution.
* 
* 5. Redistributions in any form must be accompanied by information on how to
*    obtain complete source code for the OpenPBS software and any
*    modifications and/or additions to the OpenPBS software.  The source code
*    must either be included in the distribution or be available for no more
*    than the cost of distribution plus a nominal fee, and all modifications
*    and additions to the Software must be freely redistributable by any party
*    (including Licensor) without restriction.
* 
* 6. All advertising materials mentioning features or use of the Software must
*    display the following acknowledgment:
* 
*     "This product includes software developed by NASA Ames Research Center,
*     Lawrence Livermore National Laboratory, and Veridian Information 
*     Solutions, Inc.
*     Visit www.OpenPBS.org for OpenPBS software support,
*     products, and information."
* 
* 7. DISCLAIMER OF WARRANTY
* 
* THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND. ANY EXPRESS
* OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
* OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND NON-INFRINGEMENT
* ARE EXPRESSLY DISCLAIMED.
* 
* IN NO EVENT SHALL VERIDIAN CORPORATION, ITS AFFILIATED COMPANIES, OR THE
* U.S. GOVERNMENT OR ANY OF ITS AGENCIES BE LIABLE FOR ANY DIRECT OR INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
* LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
* OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
* NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
* EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
* 
* This license will be governed by the laws of the Commonwealth of Virginia,
* without reference to its choice of law rules.
*/
/*************************************************************
*      myname           Lexer.fl
*      created          7/2/1996
*      updated          8/30/1996
*      works            xx/1996
*      designed			rrs/1996
*************************************************************/

/***********************************************************
*
*	               OPERATION OVERVIEW
*
* The flex will generate a scanner that looks for tokens
* in the language of NASL.
* The sentences are parsed by parser in the other files.
*
************************************************************/
 
/* token types by bison */
%p 20000
%n 4000
%e 8000
%a 16000
%k 20000
%o 24000
%{
/*must be here*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "Lexer.h"
#include "y.tab.h"
extern int linenum;
char   linebuf[500];
int    tokenReturned = -1;
%}

/* rules */
%%
sched_main	{ 
			LexerCondPrint("flex main"); 
			LexerPrintToken(linenum, yytext, yyleng);
			LexerTokenPut(yytext, linenum, yyleng, KEYWORDTYPE, OTHER);
			tokenReturned = MAIN;
			return(MAIN); 
		}

Void	{ 
			LexerCondPrint("flex Void"); 
			LexerPrintToken(linenum, yytext, yyleng);
			LexerTokenPut(yytext, linenum, yyleng, KEYWORDTYPE, OTHER);
			tokenReturned = VOID;
			return(VOID); 
		}

Int		{ 
			LexerCondPrint("flex int"); 
			LexerPrintToken(linenum, yytext, yyleng);
			LexerTokenPut(yytext, linenum, yyleng, KEYWORDTYPE, OTHER);
			tokenReturned = INT;
			return(INT); 
		}

Float	{ 
			LexerCondPrint("flex float"); 
			LexerPrintToken(linenum, yytext, yyleng);
			LexerTokenPut(yytext, linenum, yyleng, KEYWORDTYPE, OTHER);
			tokenReturned = FLOAT;
			return(FLOAT); 
		}

Dayofweek		{ 
			LexerCondPrint("flex Dayofweek"); 
			LexerPrintToken(linenum, yytext, yyleng);
			LexerTokenPut(yytext, linenum, yyleng, KEYWORDTYPE, OTHER);
			tokenReturned = DAYOFWEEK;
			return(DAYOFWEEK); 
		}

DateTime		{ 
			LexerCondPrint("flex DateTime"); 
			LexerPrintToken(linenum, yytext, yyleng);
			LexerTokenPut(yytext, linenum, yyleng, KEYWORDTYPE, OTHER);
			tokenReturned = DATETIME;
			return(DATETIME); 
		}

String		{ 
			LexerCondPrint("flex String"); 
			LexerPrintToken(linenum, yytext, yyleng);
			LexerTokenPut(yytext, linenum, yyleng, KEYWORDTYPE, OTHER);
			tokenReturned = STRING;
			return(STRING); 
		}

Server		{ 
			LexerCondPrint("flex Server"); 
			LexerPrintToken(linenum, yytext, yyleng);
			LexerTokenPut(yytext, linenum, yyleng, KEYWORDTYPE, OTHER);
			tokenReturned = SERVER;
			return(SERVER); 
		}

Que		{ 
			LexerCondPrint("flex Que"); 
			LexerPrintToken(linenum, yytext, yyleng);
			LexerTokenPut(yytext, linenum, yyleng, KEYWORDTYPE, OTHER);
			tokenReturned = QUE;
			return(QUE); 
		}

Job		{ 
			LexerCondPrint("flex Job"); 
			LexerPrintToken(linenum, yytext, yyleng);
			LexerTokenPut(yytext, linenum, yyleng, KEYWORDTYPE, OTHER);
			tokenReturned = JOB;
			return(JOB); 
		}

CNode		{ 
			LexerCondPrint("flex CNode"); 
			LexerPrintToken(linenum, yytext, yyleng);
			LexerTokenPut(yytext, linenum, yyleng, KEYWORDTYPE, OTHER);
			tokenReturned = CNODE;
			return(CNODE); 
		}

Size		{
			LexerCondPrint("flex Size"); 
			LexerPrintToken(linenum, yytext, yyleng);
			LexerTokenPut(yytext, linenum, yyleng, KEYWORDTYPE, OTHER);
			tokenReturned = SIZE;
			return(SIZE);
		}

Set		{ 
			LexerCondPrint("flex Set"); 
			LexerPrintToken(linenum, yytext, yyleng);
			LexerTokenPut(yytext, linenum, yyleng, KEYWORDTYPE, OTHER);
			tokenReturned = SET;
			return(SET); 
		}

Range		{ 
			LexerCondPrint("flex Range"); 
			LexerPrintToken(linenum, yytext, yyleng);
			LexerTokenPut(yytext, linenum, yyleng, KEYWORDTYPE, OTHER);
			tokenReturned = RANGE;
			return(RANGE); 
		}

while	{ 
			LexerCondPrint("flex while"); 
			LexerPrintToken(linenum, yytext, yyleng);
			LexerTokenPut(yytext, linenum, yyleng, KEYWORDTYPE, OTHER);
			tokenReturned = WHILE;
			return(WHILE); 
		}

if		{ 
			LexerCondPrint("flex if"); 

			LexerPrintToken(linenum, yytext, yyleng);
			LexerTokenPut(yytext, linenum, yyleng, KEYWORDTYPE, OTHER);
			tokenReturned = IF;

			return(IF); 
		}

else		{ 
			LexerCondPrint("flex else"); 

			LexerPrintToken(linenum, yytext, yyleng);
			LexerTokenPut(yytext, linenum, yyleng, KEYWORDTYPE, OTHER);
			tokenReturned = ELSE;

			return(ELSE); 
		}

return		{ 
			LexerCondPrint("flex return"); 

			LexerPrintToken(linenum, yytext, yyleng);
			LexerTokenPut(yytext, linenum, yyleng, KEYWORDTYPE, OTHER);
			tokenReturned = RETURN;

			return(RETURN); 
		}

print	{ 
			LexerCondPrint("flex print"); 
			LexerPrintToken(linenum, yytext, yyleng);
			LexerTokenPut(yytext, linenum, yyleng, KEYWORDTYPE, OTHER);
			tokenReturned = PRINT;
			return(PRINT); 
		}

for		{ 
			LexerCondPrint("flex for"); 
			LexerPrintToken(linenum, yytext, yyleng);
			LexerTokenPut(yytext, linenum, yyleng, KEYWORDTYPE, OTHER);
			tokenReturned = FOR;
			return(FOR); 
		}

foreach	{ 
			LexerCondPrint("flex foreach"); 
			LexerPrintToken(linenum, yytext, yyleng);
			LexerTokenPut(yytext, linenum, yyleng, KEYWORDTYPE, OTHER);
			tokenReturned = FOREACH;
			return(FOREACH); 
		}

in		{ 
			LexerCondPrint("flex in"); 
			LexerPrintToken(linenum, yytext, yyleng);
			LexerTokenPut(yytext, linenum, yyleng, KEYWORDTYPE, OTHER);
			tokenReturned = IN;
			return(IN); 
		}

switch		{ 
			LexerCondPrint("flex switch"); 
			LexerPrintToken(linenum, yytext, yyleng);
			LexerTokenPut(yytext, linenum, yyleng, KEYWORDTYPE, OTHER);
			tokenReturned = SWITCH;
			return(SWITCH); 
		}

case		{ 
			LexerCondPrint("flex case"); 
			LexerPrintToken(linenum, yytext, yyleng);
			LexerTokenPut(yytext, linenum, yyleng, KEYWORDTYPE, OTHER);
			tokenReturned = CASE;
			return(CASE); 
		}

break		{ 
			LexerCondPrint("flex break"); 

			LexerPrintToken(linenum, yytext, yyleng);
			LexerTokenPut(yytext, linenum, yyleng, KEYWORDTYPE, OTHER);
			tokenReturned = BREAK;

			return(BREAK); 
		}

continue		{
			LexerCondPrint("flex continue"); 

			LexerPrintToken(linenum, yytext, yyleng);
			LexerTokenPut(yytext, linenum, yyleng, KEYWORDTYPE, OTHER);
			tokenReturned = CONTINUE;

			return(CONTINUE); 
		}

exit			{
			LexerCondPrint("flex exit");

			LexerPrintToken(linenum, yytext, yyleng);
			LexerTokenPut(yytext, linenum, yyleng, KEYWORDTYPE, OTHER);
			tokenReturned = EXIT;

			return(EXIT);
		}

Fun		{ 
			LexerCondPrint("flex case"); 
			LexerPrintToken(linenum, yytext, yyleng);
			LexerTokenPut(yytext, linenum, yyleng, KEYWORDTYPE, OTHER);
			tokenReturned = FUN;
			return(FUN); 
		}

EQ		{ 
			LexerCondPrint("flex EQ"); 

			LexerPrintToken(linenum, yytext, yyleng);
			LexerTokenPut(yytext, linenum, yyleng, KEYWORDTYPE, OTHER);
			tokenReturned = EQ;

			return(EQ); 
		}


NEQ		{ 
			LexerCondPrint("flex NEQ"); 

			LexerPrintToken(linenum, yytext, yyleng);
			LexerTokenPut(yytext, linenum, yyleng, KEYWORDTYPE, OTHER);
			tokenReturned = NEQ;

			return(NEQ); 
		}

LT		{ 
			LexerCondPrint("flex LT<"); 

			LexerPrintToken(linenum, yytext, yyleng);
			LexerTokenPut(yytext, linenum, yyleng, KEYWORDTYPE, OTHER);
			tokenReturned = LT;

			return(LT); 
		}

LE		{ 
			LexerCondPrint("flex LE"); 

			LexerPrintToken(linenum, yytext, yyleng);
			LexerTokenPut(yytext, linenum, yyleng, KEYWORDTYPE, OTHER);
			tokenReturned = LE;

			return(LE); 
		}

GT		{ 
			LexerCondPrint("flex GT"); 

			LexerPrintToken(linenum, yytext, yyleng);
			LexerTokenPut(yytext, linenum, yyleng, KEYWORDTYPE, OTHER);
			tokenReturned = GT;

			return(GT); 
		}

GE		{ 
			LexerCondPrint("flex GE"); 

			LexerPrintToken(linenum, yytext, yyleng);
			LexerTokenPut(yytext, linenum, yyleng, KEYWORDTYPE, OTHER);
			tokenReturned = GE;

			return(GE); 
		}

MIN		{ 
			LexerCondPrint("flex MIN"); 

			LexerPrintToken(linenum, yytext, yyleng);
			LexerTokenPut(yytext, linenum, yyleng, KEYWORDTYPE, OTHER);
			tokenReturned = MIN;

			return(MIN); 
		}

MAX		{ 
			LexerCondPrint("flex MAX"); 

			LexerPrintToken(linenum, yytext, yyleng);
			LexerTokenPut(yytext, linenum, yyleng, KEYWORDTYPE, OTHER);
			tokenReturned = MAX;

			return(MAX); 
		}

AND		{ 
			LexerCondPrint("flex AND"); 

			LexerPrintToken(linenum, yytext, yyleng);
			LexerTokenPut(yytext, linenum, yyleng, KEYWORDTYPE, OTHER);
			tokenReturned = AND;

			return(AND); 
		}

OR		{ 
			LexerCondPrint("flex OR"); 

			LexerPrintToken(linenum, yytext, yyleng);
			LexerTokenPut(yytext, linenum, yyleng, KEYWORDTYPE, OTHER);
			tokenReturned = OR;

			return(OR); 
		}

default		{ 
			LexerCondPrint("flex default"); 

			LexerPrintToken(linenum, yytext, yyleng);
			LexerTokenPut(yytext, linenum, yyleng, KEYWORDTYPE, OTHER);
			tokenReturned = DEFAULT;

			return(DEFAULT);
		}

MON|TUE|WED|THU|FRI|SAT|SUN		{ 
			LexerCondPrint("flex DAYOFWEEKCONST"); 

			LexerPrintToken(linenum, yytext, yyleng);
			LexerTokenPut(yytext, linenum, yyleng, KEYWORDTYPE, OTHER);
			tokenReturned = DAYOFWEEKCONST;

			return(DAYOFWEEKCONST); 
		}

\+\+|\-\-	{
			LexerCondPrint("flex POSTOP"); 
			if( tokenReturned != IDENT ) {	/* previous token returned is an IDENT */	
				yyerror("illegal operand to postfix operator");
			}
			LexerPrintToken(linenum, yytext, yyleng);
			LexerTokenPut(yytext, linenum, yyleng, KEYWORDTYPE, OTHER);
			tokenReturned = POSTOP;
			return(POSTOP);
		}

[+-]?[0-9]+	{ 

			LexerCondPrint("flex intconstant"); 
			LexerPrintToken(linenum, yytext, yyleng);

			LexerTokenPut(yytext, linenum, yyleng, UNKNOWN, CONST);

			/* Rule: var+6 or <number>+6, the + should be treated separately. */
			if( yytext[0] == '+' || yytext[0] == '-' ) {

				if( tokenReturned == INTCONST || tokenReturned == IDENT || tokenReturned == FLOATCONST ) {
					tokenReturned = -1;
					yyless(1);	/* return '+' or '-' sign to reparse */
					return(yytext[0]);
				}
			}

			/* Otherwise, return +6 as one token */
			tokenReturned = INTCONST;
			return(INTCONST); 
		}

[+-]?[0-9]+[kmgtpKMGTP]?[bwBW] {
			LexerCondPrint("flex sizeconstant"); 
			LexerPrintToken(linenum, yytext, yyleng);

			LexerTokenPut(yytext, linenum, yyleng, UNKNOWN, CONST);
			/* Rule: var+23kb or <number>+23kb, the + and 23kb should be returned separately. */
			if( yytext[0] == '+' || yytext[0] == '-' ) {

				if( tokenReturned == SIZECONST || tokenReturned == IDENT ) {
					tokenReturned = -1;
					yyless(1);	/* return '+' or '-' sign to reparse */
					return(yytext[0]);
				}
			}
			/* Otherwise, return -23kb as one token */
			tokenReturned = SIZECONST;
			return(SIZECONST); 
		}

OP_EQ|OP_NEQ|OP_LE|OP_LT|OP_GE|OP_GT|OP_MAX|OP_MIN|SYNCRUN|ASYNCRUN|DELETE|RERUN|HOLD|RELEASE|SIGNAL|MODIFYATTR|MODIFYRES|SUCCESS|FAIL|SERVER_ACTIVE|SERVER_IDLE|SERVER_SCHED|SERVER_TERM|SERVER_TERMDELAY|QTYPE_E|QTYPE_R|SCHED_DISABLED|SCHED_ENABLED|FALSE|TRUE|TRANSIT|QUEUED|HELD|WAITING|RUNNING|EXITING|CNODE_OFFLINE|CNODE_DOWN|CNODE_FREE|CNODE_RESERVE|CNODE_INUSE_EXCLUSIVE|CNODE_INUSE_SHARED|CNODE_TIMESHARED|CNODE_CLUSTER|CNODE_UNKNOWN|ASC|DESC {	/* SUCCESS=1, FAIL=0 */
			LexerCondPrint("flex intconstant"); 
			LexerPrintToken(linenum, yytext, yyleng);

			LexerTokenPut(yytext, linenum, yyleng, UNKNOWN, CONST);
			tokenReturned = INTCONST;

			return(INTCONST); 
		}

NULLSTR		{
			LexerCondPrint("flex stringconstant"); 
			LexerPrintToken(linenum, yytext, yyleng);

			LexerTokenPut(yytext, linenum, yyleng, UNKNOWN, CONST);
			tokenReturned = STRINGCONST;

			return(STRINGCONST); 
		}

NOSERVER	{
			LexerCondPrint("flex serverconstant"); 
			LexerPrintToken(linenum, yytext, yyleng);

			LexerTokenPut(yytext, linenum, yyleng, UNKNOWN, CONST);
			tokenReturned = SERVERCONST;

			return(SERVERCONST);
		}

NOQUE		{
			LexerCondPrint("flex queconstant"); 
			LexerPrintToken(linenum, yytext, yyleng);

			LexerTokenPut(yytext, linenum, yyleng, UNKNOWN, CONST);
			tokenReturned = QUECONST;

			return(QUECONST);
		}

NOJOB		{
			LexerCondPrint("flex jobconstant"); 
			LexerPrintToken(linenum, yytext, yyleng);

			LexerTokenPut(yytext, linenum, yyleng, UNKNOWN, CONST);
			tokenReturned = JOBCONST;

			return(JOBCONST);
		}

NOCNODE		{
			LexerCondPrint("flex cnodeconstant"); 
			LexerPrintToken(linenum, yytext, yyleng);

			LexerTokenPut(yytext, linenum, yyleng, UNKNOWN, CONST);
			tokenReturned = CNODECONST;

			return(CNODECONST);
		}

EMPTYSETSERVER	{
			LexerCondPrint("flex empty set server constant"); 
			LexerPrintToken(linenum, yytext, yyleng);

			LexerTokenPut(yytext, linenum, yyleng, UNKNOWN, CONST);
			tokenReturned = SETSERVERCONST;

			return(SETSERVERCONST);
		}

EMPTYSETQUE	{
			LexerCondPrint("flex empty set que constant"); 
			LexerPrintToken(linenum, yytext, yyleng);

			LexerTokenPut(yytext, linenum, yyleng, UNKNOWN, CONST);
			tokenReturned = SETQUECONST;

			return(SETQUECONST);
		}

EMPTYSETJOB	{
			LexerCondPrint("flex empty set job constant"); 
			LexerPrintToken(linenum, yytext, yyleng);

			LexerTokenPut(yytext, linenum, yyleng, UNKNOWN, CONST);
			tokenReturned = SETJOBCONST;

			return(SETJOBCONST);
		}

EMPTYSETCNODE	{
			LexerCondPrint("flex empty set cnode constant"); 
			LexerPrintToken(linenum, yytext, yyleng);

			LexerTokenPut(yytext, linenum, yyleng, UNKNOWN, CONST);
			tokenReturned = SETCNODECONST;

			return(SETCNODECONST);
		}

[+-]?[0-9]+[.][0-9]*	{ 
			LexerCondPrint("flex floatconst"); 
			LexerPrintToken(linenum, yytext, yyleng);

			LexerTokenPut(yytext, linenum, yyleng, UNKNOWN, CONST);
			/* Rule: var+6.1 or <number>+6.1, the + and 6.1 should be treated separately. */
			if( yytext[0] == '+' || yytext[0] == '-' ) {

				if( tokenReturned == FLOATCONST || tokenReturned == IDENT || tokenReturned == INTCONST ) {
					tokenReturned = -1;
					yyless(1);	/* return '+' or '-' sign to reparse */
					return(yytext[0]);
				}
			}
			/* Otherwise, return +6.1 as one token */
			tokenReturned = FLOATCONST;
			return(FLOATCONST); 
		}

["][^"\n()]*["]	{
			LexerCondPrint("flex stringconst"); 
			LexerPrintToken(linenum, yytext, yyleng);

			LexerTokenPut(yytext, linenum, yyleng, UNKNOWN, CONST);
			tokenReturned = STRINGCONST;

			return(STRINGCONST); 
		}

[a-zA-Z]+[a-zA-Z0-9_]*	{ 
			LexerCondPrint("flex ident"); 
			LexerPrintToken(linenum, yytext, yyleng);

			LexerTokenPut(yytext, linenum, yyleng, UNKNOWN, VAR);
			tokenReturned = IDENT;

			return(IDENT); 
		}

"//".*	{
			LexerCondPrint("flex comment -- eat it -- don't return"); 
			LexerPrintToken(linenum, yytext, yyleng);
			tokenReturned = -1;	/* misc return */

			/*
			LexerTokenPut(yytext, linenum, yyleng, UNKNOWN, OTHER);

			return(COMMENT); 
			*/
		}

\n.*		{
			strcpy(linebuf, yytext+1); /* save the next line */
			linenum++; 
			yyless(1);	/* give back all by the \n to rescan */
			tokenReturned = -1;	/* misc return */
		}

[ \t]+  {    
			; /* eat space */
		/* these tokens are ignored so need to set tokenReturned */
		}

.	{ 
		
			LexerCondPrint("flex single char");
			
			LexerPrintToken(linenum, yytext, yyleng);
			LexerTokenPut(yytext, linenum, yyleng, yytext[0], OTHER);
			tokenReturned = -1;	/* misc return */
			return(yytext[0]); 

			/* mostly operators, 
			but other junk that will be rejected by parser */
		} 
%%
#include <pbs_config.h>   /* the master config generated by configure */

#ifndef _POSIX_SOURCE
#define _POSIX_SOURCE 1
#endif  /* _POSIX_SOURCE */
/*
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include "Lexer.h"
#include "y.tab.h"
*/

static char ident[] = "@(#) $RCSfile$ $Revision$";
/* Global Variables */
FILE *LexerFpOut;

/* File Scope Variables */ 
static char * LexerErrors[] =
{
        "0 no such error msg",
        "1 yytext = NULL",
        "2 lexem ptr = NULL",
        "3 str ptr = NULL",
        "4 LexerFpOut ptr = NULL",
        ""
};

static int LexerMaxErrors = 6;

static char *LexerName = "Lexer";

static int LexerDF = 0;

void LexerInit(void)
{
	LexerCondPrint("LexerInit");

/*	LexerDF = 1; */
}
 
void LexerTokenPut(lexem, lin, len, typ, varFlag)
char	*lexem;
int	lin;
int	len;
int	typ;
int	varFlag;
{

	LexerCondPrint("LexerTokenPut");
	if(lexem == NULL)
		LexerErr(2);

	strcpy((char *) yylval.mytok.lexeme, lexem);
	yylval.mytok.line = lin;
	yylval.mytok.len = len;
	yylval.mytok.type = typ;
	yylval.mytok.varFlag = varFlag;	/*yes/no*/
}

void LexerPrintToken(linenum, yytext, yylen)
int	linenum;
char	*yytext;
int	yylen;
{
	LexerCondPrint("LexerPrintToken");
	if(yytext == NULL)
		LexerErr(1);

	if(LexerFpOut == NULL)
		LexerErr(4);

	if (LexerDF == 1)
	{
	fprintf(LexerFpOut, "linenum=%d, yytext=%s, yylen=%d\n", linenum, yytext, yylen);
	}
}


 
/*
 * Print Token in Lexer
 */
void LexerPrintToken2(tok)
struct	MYTOK	tok;
{
	LexerCondPrint("LexerPrintToken2");

	if(LexerFpOut == NULL)
		LexerErr(4);

	fprintf(LexerFpOut, "lexem=%s, lin=%d, len=%d, typ=%d, varFlag=%d\n", tok.lexeme, tok.line, tok.len, tok.type, tok.varFlag);
}

 
void LexerPutDF(df)
int	df;
{
	LexerCondPrint("LexerPutDF");

	LexerDF = df;
}
 
void LexerCondPrint(str)
char	*str;
{
	if(str == NULL)
		LexerErr(3);

	if(LexerFpOut == NULL)
		LexerErr(4);

	if (LexerDF ==1)
	{
		fprintf(LexerFpOut, "%s\t", LexerName);
		fprintf(LexerFpOut, "%s\n", str);
	}
}

void LexerPrint(void)
{
	if(LexerFpOut == NULL)
		LexerErr(4);

	LexerCondPrint("LexerPrint");
	fprintf(LexerFpOut, "no state to print\n");

	/*no state to print*/
}

void LexerErr(e)
int	e;
{
	fprintf(LexerFpOut, "LexerErr\n");

	if(LexerFpOut == NULL)
		LexerErr(4);

	if (e >= LexerMaxErrors)
		e = 0;

	fprintf(LexerFpOut, "rs: %s\n", LexerErrors[e]);
	exit(1);
}
