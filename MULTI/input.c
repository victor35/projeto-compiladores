#include <stdio.h>
#include "input.h"

char look; /* caracter "lookahead */

/* l� pr�ximo caracter da entrada */
void nextChar()
{
	look = getchar();
}

/* inicializa o m�dulo de entrada */
void initInput()
{
	nextChar();
}

