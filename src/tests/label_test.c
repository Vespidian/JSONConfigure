#include <stdio.h>
#include "../json_base.h"

void tfunc_f(JSONState *json, unsigned int token){
	printf("WE GOT AN F!!\n");
}

void tfunc_test(JSONState *json, unsigned int token){
	JSONSetTokenFunc(json, "f", tfunc_f);
	JSONParse(json);
}

void label_test(void){
	JSONState json = JSONOpen("../resources/label_test.json");
	JSONSetTokenFunc(&json, "test", tfunc_test);
	JSONParse(&json);
	JSONFree(&json);
}