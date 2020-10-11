#include <stdio.h>
#include "input.h"
#include "output.h"
#include "errors.h"
#include "scanner1.h"

int main()
{
        initInput();

	printf("%c", getName());
	match('=');
	printf("%c", getNum());
	match('+');
	printf("%c", getName());

	return 0;
}

