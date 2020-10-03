#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char look; /* O caracter lido "antecipadamente" (lookahead) */
int lblcount; /* indica o rótulo atual */

/* protótipos */
void init();
void nextchar();
void error(char *s);
void fatal(char *s);
void expected(char *s);
void match(char c);
char getname();
char getnum();
int getboolean();
int isaddop(char c);
int ismulop(char c);
int isboolean(char c);
int isorop(char c);
int isrelop(char c);

int newlabel();

void notfactor();
void boolfactor();
void boolterm();
void boolexpression();
void boolor();
void boolxor();
void do_equals();
void do_notequals();
void do_greater();
void do_less();
void relation();

int main()
{
	init();
        boolexpression();

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

int getboolean()
{
	int boolean;
	
	if (!isboolean(look))
		expected("Boolean Literal");
	boolean = (toupper(look) == 'T');
	nextchar();
	
	return boolean;
}

int isaddop(char c)
{
        return (c == '+' || c == '-');
}

int ismulop(char c)
{
        return (c == '*' || c == '/');
}

int isboolean(char c)
{
        char uc;

        uc = toupper(c);

	return (uc == 'T' || uc == 'F');
}

int isorop(char c)
{
        return (c == '|' || c == '~');
}

int isrelop(char c)
{
	return (strchr("=#<>", c) != NULL);
}

int newlabel()
{
	return lblcount++;
}

void boolfactor()
{
	if (isboolean(look)) {
		if (getboolean())
			printf("\tmov ax, -1\n");
		else
			printf("\txor ax, ax\n");
	} else
		relation();
}

void notfactor()
{
        if (look == '!') {
                match('!');
                boolfactor();
                printf("\tnot ax\n");
        } else
                boolfactor();
}

void boolterm()
{
	notfactor();
	while (look == '&') {
		printf("\tpush ax\n");
		match('&');
		notfactor();
		printf("\tpop bx\n");
		printf("\tand ax, bx\n");
	}
}

void boolor()
{
	match('|');
	boolterm();
	printf("\tpop bx\n");
	printf("\tor ax, bx\n");
}

void boolxor()
{
	match('~');
	boolterm();
	printf("\tpop bx\n");
	printf("\txor ax, bx\n");
}

void boolexpression()
{
	boolterm();
	while (isorop(look)) {
		printf("\tpush ax\n");
		switch (look) {
		  case '|':
		  	boolor(); break;
		  case '~' :
		  	boolxor(); break;
		}
	}
}

void expression()
{
        // dummy
}

void do_equals()
{
        int l1, l2;

	match('=');
        l1 = newlabel();
        l2 = newlabel();
	expression();
	printf("\tpop bx\n");
	printf("\tcmp bx, ax\n");
	printf("\tje L%d\n", l1);
	printf("\txor ax, ax\n");
	printf("\tjmp L%d\n", l2);
	printf("L%d:\n", l1);
	printf("\tmov ax, -1\n");
	printf("L%d:\n", l2);
}

void do_notequals()
{
        int l1, l2;

	match('#');
        l1 = newlabel();
        l2 = newlabel();
	expression();
	printf("\tpop bx\n");
	printf("\tcmp bx, ax\n");
	printf("\tjne L%d\n", l1);
	printf("\txor ax, ax\n");
	printf("\tjmp L%d\n", l2);
	printf("L%d:\n", l1);
	printf("\tmov ax, -1\n");
	printf("L%d:\n", l2);
}

void do_greater()
{
        int l1, l2;

	match('>');
        l1 = newlabel();
        l2 = newlabel();
	expression();
	printf("\tpop bx\n");
	printf("\tcmp bx, ax\n");
	printf("\tjg L%d\n", l1);
	printf("\txor ax, ax\n");
	printf("\tjmp L%d\n", l2);
	printf("L%d:\n", l1);
	printf("\tmov ax, -1\n");
	printf("L%d:\n", l2);
}

void do_less()
{
        int l1, l2;

	match('<');
        l1 = newlabel();
        l2 = newlabel();
	expression();
	printf("\tpop bx\n");
	printf("\tcmp bx, ax\n");
	printf("\tjl L%d\n", l1);
	printf("\txor ax, ax\n");
	printf("\tjmp L%d\n", l2);
	printf("L%d:\n", l1);
	printf("\tmov ax, -1\n");
	printf("L%d:\n", l2);
}

void relation()
{
	expression();
	if (isrelop(look)) {
		printf("\tpush ax\n");
		switch (look) {
		  case '=':
		  	do_equals(); break;
		  case '#':
		  	do_notequals(); break;
		  case '>':
		  	do_greater(); break;
		  case '<':
		  	do_less(); break;
		}
	}
}
