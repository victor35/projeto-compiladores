#include "input.h"
#include "scanner.h"
#include "codegen.h"
#include "parser.h"

/* analisa e traduz um fator matem�tico */
void factor()
{
	char name[MAXNAME+1], num[MAXNUM+1];

	if (look == '(') {
		match('(');
		expression();
		match(')');
        } else if (isdigit(look)) {
        	getNum(num);
        	asmLoadConstant(num);
        } else if (isalpha(look)) {
                getName(name);
                asmLoadVariable(name);
        } else
                error("Unrecognized character: '%c'", look);
}

/* analisa e traduz um fator com NOT opcional */
void notFactor()
{
	if (look == '!') {
		match('!');
		factor();
		asmNot();
	} else
		factor();
}

/* analisa e traduz uma opera��o de multiplica��o */
void multiply()
{
	match('*');
	asmPush();
	notFactor();
	asmPopMul();
}

/* analisa e traduz uma opera��o de divis�o */
void divide()
{
	match('/');
	asmPush();
	notFactor();
	asmPopDiv();
}

/* analisa e traduz uma opera��o AND */
void boolAnd()
{
	match('&');
	asmPush();
	notFactor();
	asmPopAnd();
}

/* analisa e traduz um termo */
void term()
{
	notFactor();
	while (isMulOp(look)) {
		switch (look) {
		  case '*':
		  	multiply();
		  	break;
		  case '/':
		  	divide();
		  	break;
		  case '&':
		  	boolAnd();
		  	break;
		}
	}
}

/* analisa e traduz um termo com um sinal opcional */
void signedTerm()
{
	char sign = look;
	if (isAddOp(look))
		nextChar();
	term();
	if (sign == '-')
		asmNegate();
}

/* analisa e traduz uma opera��o de soma */
void add()
{
	match('+');
	asmPush();
	term();
	asmPopAdd();
}

/* analisa e traduz uma opera��o de subtra��o */
void subtract()
{
	match('-');
	asmPush();
	term();
	asmPopSub();
}

/* analisa e traduz uma opera��o OU booleana */
void boolOr()
{
	match('|');
	asmPush();
	term();
	asmPopOr();
}

/* analisa e traduz uma opera��o OU-exclusivo booleana */
void boolXor()
{
	match('~');
	asmPush();
	term();
	asmPopXor();
}

/* analisa e traduz uma express�o */
void expression()
{
	signedTerm();
	while (isAddOp(look)) {
		switch (look) {
		  case '+':
		  	add();
		  	break;
		  case '-':
		  	subtract();
		  	break;
		  case '|':
		  	boolOr();
		  	break;
		  case '~':
		  	boolXor();
		  	break;
		}
	}
}

/* analisa e traduz um comando de atribui��o */
void assignment()
{
	char name[MAXNAME+1];
	
	getName(name);
	match('=');
	expression();
	asmStoreVariable(name);
}

