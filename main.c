#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char look; /* O caracter lido "antecipadamente" (lookahead) */
int lblcount; /* indica o rótulo atual */

#define VARTBL_SZ 26
char vartbl[VARTBL_SZ];

/* protótipos */
void init();
void nextchar();
void error(char *s);
void fatal(char *s);
void expected(char *s);
void match(char c);
char getname();
int getnum();
int isaddop(char c);
int ismulop(char c);
int isorop(char c);
int isrelop(char c);

int newlabel();

void prog();
void header();
void prolog();
void epilog();
void mainblock();
void topdecls();
void decl();
void allocvar(char name);
int intable(char name);

void block();

void asm_clear();
void asm_negative();
void asm_loadconst(int i);
void asm_loadvar(char name);
void asm_push();
void asm_popadd();
void asm_popsub();
void asm_popmul();
void asm_popdiv();
void asm_store(char name);

void factor();
void negfactor();
void firstfactor();
void multiply();
void term1();
void term();
void firstterm();
void add();
void subtract();
void expression();
void assignment();

void asm_not();
void asm_popand();
void asm_popor();
void asm_popxor();
void asm_popcompare();
void asm_relop(char op);

void relation();
void notfactor();
void boolterm();
void boolor();
void boolxor();
void boolexpression();

void asm_jmp(int label);
void asm_jmpfalse(int label);
void doif();
void dowhile();

int main()
{
	init();
        prog();

        if (look != '\n')
                fatal("Unexpected data after \'.\'");

	return 0;
}

void init()
{
	int i = 0;
	
	for (i = 0; i < VARTBL_SZ; i++)
		vartbl[i] = ' ';
	
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

/* avisa a respeito de um identificador desconhecido */
void undefined(char name)
{
	fprintf(stderr, "Error: Undefined identifier %c\n", name);
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

void skipwhite()
{
	while (look == ' ' || look == '\t')
		nextchar();
}

void prog()
{
	match('p');
	header();
        topdecls();
        mainblock();
	match('.');
}

void header()
{
	printf("\t.model small\n");
	printf("\t.stack\n");
	printf("\t.code\n");
        printf("PROG segment byte public\n");
        printf("\tassume cs:PROG,ds:PROG,es:PROG,ss:PROG\n");
}

void prolog()
{
        printf("MAIN:\n");
}

void epilog()
{
        printf("\tmov ax,4C00h\n");
        printf("\tint 21h\n");
        printf("PROG ends\n");
        printf("\tend MAIN\n");
}

void mainblock()
{
	match('b');
	prolog();
        block();
	match('e');
	epilog();
}

void topdecls()
{
        while (look != 'b') {
                switch (look) {
                  case 'v':
                        decl();
                        break;
                  default:
                        error("Unrecognized keyword.");
                        expected("BEGIN");
                        break;
                }
        }
}

void decl()
{
        match('v');
	for (;;) {
	        allocvar(getname());
	        if (look != ',')
	        	break;
	        match(',');
	}
}

void allocvar(char name)
{
	int value = 0, signal = 1;

        if (intable(name)) {
                fprintf(stderr, "Duplicate variable name: %c", name);
                exit(1);
        } else
                vartbl[name - 'A'] = 'v';

	
	if (look == '=') {
		match('=');
                if (look == '-') {
                        match('-');
                        signal = -1;
                }
		value = signal * getnum();
	}	
	
        printf("%c:\tdw %d\n", name, value);
}

int intable(char name)
{
	return (vartbl[name - 'A'] != ' ');
}

void block()
{
	int follow = 0;
	
	while (!follow) {
		switch (look) {
		  case 'i':
		  	doif();
		  	break;
		  case 'w':
		  	dowhile();
		  	break;
		  case 'e':
		  case 'l':
		  	follow = 1;
		  	break;
		  default:
		  	assignment();
		  	break;
		}
	}
}


/* zera o registrador primário */
void asm_clear()
{
	printf("\txor ax, ax\n");
}

/* negativa o reg. primário */
void asm_negative()
{
	printf("\tneg ax\n");
}

/* carrega uma constante numérica no reg. prim. */
void asm_loadconst(int i)
{
	printf("\tmov ax, %d\n", i);
}

/* carrega uma variável no reg. prim. */
void asm_loadvar(char name)
{
	if (!intable(name))
		undefined(name);
	printf("\tmov ax, [%c]\n", name);
}

/* coloca reg. prim. na pilha */
void asm_push()
{
	printf("\tpush ax\n");
}

/* adiciona o topo da pilha ao reg. prim. */
void asm_popadd()
{
	printf("\tpop bx\n");
	printf("\tadd ax, bx\n");
}

/* subtrai o reg. prim. do topo da pilha */
void asm_popsub()
{
	printf("\tpop bx\n");
	printf("\tsub ax, bx\n");
	printf("\tneg ax\n");
}

/* multiplica o topo da pilha pelo reg. prim. */
void asm_popmul()
{
	printf("\tpop bx\n");
	printf("\tmul bx\n");
}

/* divide o topo da pilha pelo reg. prim. */
void asm_popdiv()
{
	printf("\tpop bx\n");
	printf("\txchg ax, bx\n");
	printf("\tdiv bx\n");
}

/* armazena reg. prim. em variável */
void asm_store(char name)
{
	if (!intable(name))
		undefined(name);
	printf("\tlea bx, [%c]\n", name);
   	printf("\tmov [bx], ax\n");
}

/* analisa e traduz um fator matemático */
void factor()
{
	if (look == '(') {
		match('(');
		boolexpression();
		match(')');
	} else if (isalpha(look))
		asm_loadvar(getname());
	else
		asm_loadconst(getnum());
}

/* analisa e traduz um fator negativo */
void negfactor()
{
	match('-');
	if (isdigit(look))
		asm_loadconst(-getnum());
	else {
		factor();
		asm_negative();
	}
}

/* analisa e traduz um fator inicial */
void firstfactor()
{
	switch (look) {
	  case '+':
		match('+');
		factor();
		break;
	  case '-':
		negfactor();
		break;
	  default:
		factor();
		break;
	}
}

/* reconhece e traduz uma multiplicação */
void multiply()
{
	match('*');
	factor();
	asm_popmul();
}

/* reconhece e traduz uma divisão */
void divide()
{
	match('/');
	factor();
	asm_popdiv();
}

/* código comum usado por "term" e "firstterm" */
void term1()
{
	while (ismulop(look))  {
		asm_push();
		switch (look) {
		  case '*':
			multiply();
			break;
		  case '/':
			divide();
			break;
		}
	}
}

/* analisa e traduz um termo matemático */
void term()
{
	factor();
	term1();
}

/* analisa e traduz um termo inicial */
void firstterm()
{
	firstfactor();
	term1();
}

/* reconhece e traduz uma adição */
void add()
{
	match('+');
	term();
	asm_popadd();
}



/* reconhece e traduz uma subtração*/
void subtract()
{
	match('-');
	term();
	asm_popsub();
}

/* analisa e traduz uma expressão matemática */
void expression()
{
	firstterm();
	while (isaddop(look))  {
		asm_push();
		switch (look) {
		  case '+':
			add();
			break;
		  case '-':
			subtract();
			break;
		}
	}
}

/* analisa e traduz um comando de atribuição */
void assignment()
{
	char name;

	name = getname();
	match('=');
	boolexpression();
	asm_store(name);
}


/* inverte reg. prim. */
void asm_not()
{
	 printf("\tnot ax\n");
}

/* "E" do topo da pilha com reg. prim. */
void asm_popand()
{
	 printf("\tpop bx\n");
	 printf("\tand ax, bx\n");
}

/* "OU" do topo da pilha com reg. prim. */
void asm_popor()
{
	 printf("\tpop bx\n");
	 printf("\tor ax, bx\n");
}

/* "OU-exclusivo" do topo da pilha com reg. prim. */
void asm_popxor()
{
	 printf("\tpop bx\n");
	 printf("\txor ax, bx\n");
}

/* compara topo da pilha com reg. prim. */
void asm_popcompare()
{
	 printf("\tpop bx\n");
	 printf("\tcmp bx, ax\n");
}

/* altera reg. primário (e flags, indiretamente) conforme a comparação */
void asm_relop(char op)
{
	char *jump;
        int l1, l2;

        l1 = newlabel();
        l2 = newlabel();

	switch (op) {
	  case '=': jump = "je"; break;
	  case '#': jump = "jne"; break;
	  case '<': jump = "jl"; break;
	  case '>': jump = "jg"; break;
	}

	printf("\t%s L%d\n", jump, l1);
	printf("\txor ax, ax\n");
	printf("\tjmp L%d\n", l2);
	printf("L%d:\n", l1);
	printf("\tmov ax, -1\n");
	printf("L%d:\n", l2);
}

/* analisa e traduz uma relação */
void relation()
{
	char op;

	expression();
	if (isrelop(look)) {
		op = look;
		match(op); /* só para remover o operador do caminho */
		asm_push();
		expression();
		asm_popcompare();
		asm_relop(op);
	}
}

/* analisa e traduz um fator booleano com NOT inicial */
void notfactor()
{
	if (look == '!') {
		match('!');
		relation();
		asm_not();
	} else
		relation();
}

/* analisa e traduz um termo booleano */
void boolterm()
{
	notfactor();
	while (look == '&') {
		asm_push();
		match('&');
		notfactor();
		asm_popand();
	}
}

/* reconhece e traduz um "OR" */
void boolor()
{
	match('|');
	boolterm();
	asm_popor();
}

/* reconhece e traduz um "xor" */
void boolxor()
{
	match('~');
	boolterm();
	asm_popxor();
}

/* analisa e traduz uma expressão booleana */
void boolexpression()
{
	boolterm();
	while (isorop(look)) {
		asm_push();
		switch (look) {
		  case '|':
		  	boolor();
		  	break;
		  case '~':
		  	boolxor();
		  	break;
		}
	}
}

/* desvio incondicional */
void asm_jmp(int label)
{
	printf("\tjmp L%d\n", label);
}

/* desvio se falso (0) */
void asm_jmpfalse(int label)
{
	printf("\tjz L%d\n", label);
}

void doif()
{
	int l1, l2;
	
	match('i');
	boolexpression();
	l1 = newlabel();
	l2 = l1;
	asm_jmpfalse(l1);
	block();
	if (look == 'l') {
		match('l');
		l2 = newlabel();
		asm_jmp(l2);
		printf("L%d:\n", l1);
		block();
	}
	printf("L%d:\n", l2);
	match('e');
}

void dowhile()
{
	int l1, l2;

	match('w');
	l1 = newlabel();
	l2 = newlabel();
	printf("L%d:\n", l1);
	boolexpression();
	asm_jmpfalse(l2);
	block();
	match('e');
	asm_jmp(l1);
	printf("L%d:\n", l2);
}

