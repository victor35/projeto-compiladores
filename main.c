#include <stdio.h>
#include <stdlib.h>

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
int isaddop(char c);
int ismulop(char c);

int newlabel();

void other();
void program();
void condition();
void expression();

void doif(int l);
void dowhile();
void doloop();
void dorepeat();
void dofor();
void dodo();
void dobreak(int l);
void block(int l);

int main()
{
	init();
        program();

	return 0;
}

void init()
{
        lblcount = 0;
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

int newlabel()
{
	return lblcount++;
}

void other()
{
        printf("\t## %c ##\n", getname());
}

void program()
{
	block(-1);
        match('e');
	printf("\tint 20h\n");
}

void condition()
{
	printf("\t## condition ##\n");
}

void expression()
{
	printf("\t## EXPR ##\n");
}

void doif(int l)
{
	int l1, l2;
	
	match('i');
	condition();
	l1 = newlabel();
	l2 = l1;
	printf("\tjz L%d\n", l1);
	block(l);
	if (look == 'l') {
		match('l');
		l2 = newlabel();
		printf("\tjmp L%d\n", l2);
		printf("L%d:\n", l1);
		block(l);
	}
	match('e');
	printf("L%d:\n", l2);
}

void dowhile()
{
	int l1, l2;
	
	match('w');
	l1 = newlabel();
	l2 = newlabel();
	printf("L%d:\n", l1);
	condition();
	printf("\tjz L%d\n", l2);
	block(l2);
	match('e');
	printf("\tjmp L%d\n", l1);
	printf("L%d:\n", l2);
}

void doloop()
{
	int l1, l2;
	
	match('p');
	l1 = newlabel();
	l2 = newlabel();
	printf("L%d:\n", l1);
	block(l2);
	match('e');
	printf("\tjmp L%d\n", l1);
	printf("L%d:\n", l2);
}


void dorepeat()
{
	int l1, l2;
	
	match('r');
	l1 = newlabel();
	l2 = newlabel();
	printf("L%d:\n", l1);
	block(l2);
	match('u');
	condition();
	printf("\tjz L%d\n", l1);
        printf("L%d:\n", l2);
}

void dofor()
{
	int l1, l2;
	char name;
	
	match('f');
	l1 = newlabel();
	l2 = newlabel();
	name = getname();
	match('=');
	expression();
	printf("\tdec ax\n");
	printf("\tlea bx, [%c]\n", name);
	printf("\tmov [bx], ax\n");
	expression();
	printf("\tpush ax\n");
	printf("L%d:\n", l1);
	printf("\tlea bx, [%c]\n", name);
	printf("\tmov ax, [bx]\n");
	printf("\tinc ax\n");
	printf("\tmov [bx], ax\n");
	printf("\tpop bx\n");
	printf("\tpush bx\n");
	printf("\tcmp ax, bx\n");
	printf("\tjg L%d\n", l2);
	block(l2);
	printf("\tjmp L%d\n", l1);
	printf("L%d:\n", l2);	
	printf("\tpop ax\n");
}

void dodo()
{
	int l1, l2;
	
	match('d');
	l1 = newlabel();
	l2 = newlabel();
	expression();
	printf("\tmov cx, ax\n");
	printf("L%d:\n", l1);
	printf("\tpush cx\n");
	block(l2);
	printf("\tpop cx\n");	
	printf("\tloop L%d\n", l1);
	printf("\tpush cx\n");
	printf("L%d:\n", l2);
	printf("\tpop cx\n");	
}

void dobreak(int l)
{
        match('b');
        if (l == -1)
                fatal("No loop to break from");
        printf("\tjmp L%d\n", l);
}

void block(int l)
{
	int follow;
	
	follow = 0;
	
	while (!follow) {
		switch (look) {
		  case 'i':
		   	doif(l); break;
		  case 'w':
		   	dowhile(); break;
                  case 'p':
                        doloop(); break;
                  case 'r':
                   	dorepeat(); break;
                  case 'f':
                        dofor(); break;
                  case 'd':
                        dodo(); break;
                  case 'b':
                        dobreak(l); break;
		  case 'e':
		  case 'l':
		  case 'u':
		   	follow = 1;
		   	break;
		  default:
		   	other(); break;
                }
	}
}
