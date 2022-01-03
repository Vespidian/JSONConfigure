#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <jsmn.h>

#include "debug.h"
#include "json_base.h"

#include "tests/tests.h"

int main(){
	base_test();
	base_test();
	printf("---------------------------------\n");
	nested_test();
	printf("---------------------------------\n");
	no_setup_test();
	printf("---------------------------------\n");

	getchar();

	return 0;
}