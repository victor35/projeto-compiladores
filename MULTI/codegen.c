#include "output.h"
#include "codegen.h"

/* carrega uma constante no registrador prim�rio */
void asmLoadConstant(char *s)
{
	emit("MOV AX, %s", s);
}

/* carrega uma vari�vel no registrador prim�rio */
void asmLoadVariable(char *s)
{
	emit("MOV AX, [%s]", s);
}

/* inverte sinal de reg. prim. */
void asmNegate()
{
	emit("NEG AX");
}

/* coloca registrador prim�rio na pilha */
void asmPush()
{
	emit("PUSH AX");
}

/* adiciona topo da pilha a reg. prim�rio */
void asmPopAdd()
{
	emit("POP BX");
	emit("ADD AX, BX");
}

/* subtrari do topo da pilha o reg. prim�rio */
void asmPopSub()
{
	emit("POP BX");
	emit("SUB AX, BX");
	asmNegate();
}

/* aplica OU com topo da pilha a reg. prim�rio */
void asmPopOr()
{
	emit("POP BX");
	emit("OR AX, BX");
}

/* aplica OU-exclusivo com topo da pilha a reg. prim�rio */
void asmPopXor()
{
	emit("POP BX");
	emit("XOR AX, BX");
}

/* adiciona topo da pilha a reg. prim�rio */
void asmPopMul()
{
	emit("POP BX");
	emit("IMUL BX");
}

/* subtrari do topo da pilha o reg. prim�rio */
void asmPopDiv()
{
	emit("POP BX");
	emit("XCHG AX, BX");
	emit("CWD");
	emit("IDIV BX");
}

/* aplica AND com topo da pilha a reg. prim�rio */
void asmPopAnd()
{
	emit("POP BX");
	emit("AND AX, BX");
}

/* aplica NOT a reg. prim�rio */
void asmNot()
{
        emit("NOT AX");
}

/* armazena valor do registrador prim�rio em vari�vel */
void asmStoreVariable(char *name)
{
	emit("MOV [%s], AX", name);
}

