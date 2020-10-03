/*
The Cradle - O Berço

O código abaixo foi escrito por Felipo Soranz e é uma adaptação
do código original em Pascal escrito por Jack E. Crenshaw em sua
série "Let's Build a Compiler".

Este código é de livre distribuição e uso.
*/

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

int main()
{
	init();

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
