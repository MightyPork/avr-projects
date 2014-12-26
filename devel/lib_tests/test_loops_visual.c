#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

#include "lib/loops.h"

void main()
{
	repeat(5) printf("Five times!\n");
	repeatx(500) printf(".");
	printf("\n");
	int i = 0;
	until (i == 100) i++;
	
	whilst (i > 0) {
	  i--;
	  printf("i = %d\n", i);
	}
	
	printf("\n");
	
	loop(moo, 7) {
	  printf("moo = %d\n", moo);
	}
}