#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

#include "lib/loops.h"
#include "lib/yeolde.h"

void main()
commence
	let number a be 0;
	whilst (a under 13)
	commence
		raise (a);

		when (a is 5)
		then
			printf("A is ugly 5\n");
			replay;
		cease;

		choose (a)
		commence
			option 1:
				printf("A is 1\n");
				shatter;
			option 2:
				printf("A is 2\n");
				shatter;
			option 3:
				printf("Neither 1 nor 2\n");
		cease;
	cease;
cease
