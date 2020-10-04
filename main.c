
#include <stdio.h>
#include <stdlib.h>

char look; /* O caracter lido "antecipadamente" (lookahead) */

/* protótipos */
void init();
void nextchar();
void error(char *s);
void fatal(char *s);
void expected(char *s);
void match(char c);
char getname();
char getnum();
int isaddop(char c);
int ismulop(char c);

void prog();
void prolog();
void epilog(char name);
void doblock(char name);
void declarations();
void statements();

void labels();
void constants();
void types();
void variables();
void doprocedure();
void dofunction();

int main()
{
	init();
        prog();

	return 0;
}

void init()
{
	nextchar();
}

void nextchar()
{
	look = getchar();
}

void error(char *s)
{
	fprintf(stderr, "Error: %s\n", s);
}

void fatal(char *s)
{
	error(s);
	exit(1);
}

void expected(char *s)
{
	fprintf(stderr, "Error: %s expected\n", s);
	exit(1);
}

void match(char c)
{
	char s[2];

	if (look == c)
		nextchar();
	else {
		s[0] = c; /* uma conversao rápida (e feia) */
		s[1] = '\0'; /* de um caracter em string */
		expected(s);
	}
}

char getname()
{
	char name;

	if (!isalpha(look))
		expected("Name");
	name = toupper(look);
	nextchar();

	return name;
}

char getnum()
{
	char num;

	if (!isdigit(look))
		expected("Integer");
	num = look;
	nextchar();

        return num;
}

int isaddop(char c)
{
        return (c == '+' || c == '-');
}

int ismulop(char c)
{
        return (c == '*' || c == '/');
}

void skipwhite()
{
	while (look == ' ' || look == '\t')
		nextchar();
}

/* analisa e traduz um programa */
void prog()
{
	char name;
	
	match('p'); /* trata do cabeçalho do programa */
	name = getname();
	prolog();
        doblock(name);
	match('.');
	epilog(name);
}

void prolog()
{
	printf("\t.model small\n");
	printf("\t.stack\n");
	printf("\t.code\n");
        printf("PROG segment byte public\n");
        printf("\tassume cs:PROG,ds:PROG,es:PROG,ss:PROG\n");
}

void epilog(char name)
{
	printf("exit_prog:\n");
        printf("\tmov ax,4C00h\n");
        printf("\tint 21h\n");
        printf("PROG ends\n");
        printf("\tend %c\n", name);
}

/* analisa e traduz um bloco pascal */
void doblock(char name)
{
	declarations();
	printf("%c:\n", name);
	statements();
}

/* analisa e traduz a seção de declaração */
void declarations()
{
	int valid;
	
	do {
		valid = 1;
		switch (look) {
		  case 'l':
		  	labels(); break;
		  case 'c':
		  	constants(); break;
		  case 't':
		  	types(); break;
		  case 'v':
		  	variables(); break;
		  case 'p':
		  	doprocedure(); break;
		  case 'f':
		  	dofunction(); break;
		  default:
		  	valid = 0; break;
		}
	} while (valid);
}

void statements()
{
	match('b');
	while (look != 'e')
		nextchar();
	match('e');
}

void labels()
{
	match('l');
}

void constants()
{
	match('c');
}

void types()
{
	match('t');
}

void variables()
{
	match('v');
}

void doprocedure()
{
	match('p');
}

void dofunction()
{
	match('f');
}


