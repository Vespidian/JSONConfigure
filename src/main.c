#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "debug.h"
#include "json_base.h"

#include "tests/tests.h"

int main(){
	InitDebug();
	
	base_test();
	// printf("---------------------------------\n");
	// nested_test();
	// printf("---------------------------------\n");
	// no_setup_test();
	// printf("---------------------------------\n");
	// label_test();
	// printf("---------------------------------\n");
	// example_test();
	// printf("---------------------------------\n");
	// tag_test();
	// printf("---------------------------------\n");
	// string_test();
	// printf("---------------------------------\n");
	// base_test_string();

	QuitDebug();

	getchar();

	return 0;
}