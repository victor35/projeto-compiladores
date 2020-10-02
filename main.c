#include <stdio.h>
#include <stdlib.h>

#define MAXVAR 26

char look; /* O caracter lido "antecipadamente" (lookahead) */

int var[MAXVAR];

/* protótipos */
void init();
void nextchar();
void error(char *s);
void fatal(char *s);
void expected(char *s);
void match(char c);
char getname();
int getnum();

void initvar();
void newline();

int factor();
int term();
int expression();
void assignment();
void input();
void output();

int main()
{
	init();
        do {
		switch (look) {
		  case '?':
		  	input();
		  	break;
		  case '!':
		  	output();
		  	break;
		  default:
		       	assignment();
		       	break;
		}
        	newline();
        } while (look != '.');

	return 0;
}

void init()
{
        initvar();
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

int getnum()
{
	int i;

	i = 0;

	if (!isdigit(look))
		expected("Integer");

	while (isdigit(look)) {
		i *= 10;
		i += look - '0';
		nextchar();
	}
	
        return i;
}

int isaddop(char c)
{
        return (c == '+' || c == '-');
}

int ismulop(char c)
{
        return (c == '*' || c == '/');
}

void initvar()
{
	int i;

	for (i = 0; i < MAXVAR; i++)
		var[i] = 0;
}

void newline()
{
	if (look == '\n')
		nextchar();
}

int factor()
{
        int i;

        if (look == '(') {
                match('(');
                i = expression();
                match(')');
        } else if (isalpha(look))
        	i = var[getname() - 'A'];
        else
                i = getnum();

        return i;
}

int term()
{
        int i;

        i = factor();
        while (ismulop(look)) {
                switch (look) {
                  case '*':
                        match('*');
                        i *= factor();
                        break;
                  case '/':
                        match('/');
                        i /= factor();
                }
        }

        return i;
}

int expression()
{
	int i;

	if (isaddop(look))
		i = 0;
	else
		i = term();

	while (isaddop(look)) {
		switch (look) {
		  case '+':
		  	match('+');
		  	i += term();
		  	break;
		  case '-':
		  	match('-');
		  	i -= term();
		  	break;
		}
	}
	
	return i;
}

void assignment()
{
	char name;
	
	name = getname();
	match('=');
	var[name - 'A'] = expression();
}

void input()
{
	char name;
        char buffer[20];
	
	match('?');
	name = getname();
	printf("%c? ", name);
        fgets(buffer, 20, stdin);
        var[name - 'A'] = atoi(buffer);
}

void output()
{
	char name;
	
	match('!');
	name = getname();
	printf("%c -> %d\n", name, var[name - 'A']);
}
