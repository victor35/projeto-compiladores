#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SYMTBL_SZ 1000
#define KWLIST_SZ 4

char *symtbl[SYMTBL_SZ];
char *kwlist[KWLIST_SZ] = {"IF", "ELSE", "ENDIF", "END"};

char *kwcode = "ileex";

#define MAXTOKEN 10
#define MAXNAME 8
#define MAXNUM 5
#define MAXOP 2

char look; /* O caracter lido "antecipadamente" (lookahead) */

char token;
char value[MAXTOKEN+1];


/* protótipos */
void init();
void nextchar();
void error(char *s);
void fatal(char *s);
void expected(char *s);
void skipwhite();
void skipcomma();
void newline();
void match(char c);
void getname();
void getnum();
void getop();
int isaddop(char c);
int ismulop(char c);
int isop(char c);
void scan();

int lookup(char *s, char *list[], int size);

int main()
{
	init();

        do {
                scan();
                switch (token) {
                  case 'x':
                        printf("Ident: ");
                        break;
                  case '#':
                        printf("Number: ");
                        break;
                  case 'i':
                  case 'l':
                  case 'e':
                  	printf("Keyword: ");
                        break;
		  default:
                        printf("Operator: ");
                        break;
                }
                printf("%s\n", value);
                if (value[0] == '\n')
                	newline();
        } while (strcmp(value, "END") != 0);

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

void skipwhite()
{
	while (look == ' ' || look == '\t')
		nextchar();
}

void skipcomma()
{
	skipwhite();
	if (look == ',') {
		nextchar();
		skipwhite();
	}
}

void newline()
{
	if (look == '\n')
		nextchar();
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

int isaddop(char c)
{
        return (c == '+' || c == '-');
}

int ismulop(char c)
{
        return (c == '*' || c == '/');
}

int isop(char c)
{
	return (strchr("+-*/<>:=", c) != NULL);
}

void getname()
{
	int i, kw;
	
	if (!isalpha(look))
		expected("Name");
	for (i = 0; isalnum(look) && i < MAXNAME; i++) {
		value[i] = toupper(look);
		nextchar();
	}
	value[i] = '\0';
	kw = lookup(value, kwlist, KWLIST_SZ);
	if (kw == -1)
                token = 'x';
        else
                token = kwcode[kw];
}

void getnum()
{
	int i;
	
	if (!isdigit(look))
		expected("Integer");
	for (i = 0; isdigit(look) && i < MAXNUM; i++) {
		value[i] = look;
		nextchar();
	}
	value[i] = '\0';
        token = '#';
}

void getop()
{
        int i;

        if (!isop(look))
                expected("Operator");
        for (i = 0; isop(look) && i < MAXOP; i++) {
                value[i] = look;
                nextchar();
        }
        value[i] = '\0';
        if (strlen(value) == 1)
        	token = value[0];
        else
        	token = '?';
}

void scan()
{
        int kw;

        while (look == '\n')
                newline();
	if (isalpha(look))
		getname();
	else if (isdigit(look))
		getnum();
	else if (isop(look))
		getop();
	else {
		value[0] = look;
		value[1] = '\0';
		token = '?';
		nextchar();
	}
	skipwhite();
}

int lookup(char *s, char *list[], int size)
{
	int i;

	for (i = 0; i < size; i++) {
		if (strcmp(list[i], s) == 0)
			return i;
	}

	return -1;
}
