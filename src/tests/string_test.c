#include <stdio.h>
#include <stdlib.h>
#include "../json_base.h"

static void tfunc_string(JSONState *json, unsigned int token){
	JSONToken str_token = JSONTokenValue(json, token + 1);
	if(str_token.type == JSON_STRING){
		char *string = NULL;
		JSONTokenToString(json, token + 1, &string);
		printf("%s\n", string);
		free(string);
	}
}

void string_test(){
	JSONState json = JSONOpen("../resources/string_test.json");
	JSONSetTokenFunc(&json, "string", tfunc_string);
	JSONParse(&json);
	JSONFree(&json);
}