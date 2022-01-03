#include <stdio.h>

#include "../json_base.h"

static char *dict[] = {
	"nest",
	NULL
};

static void tfunc0_nest(JSONState *json, unsigned int token){
	switch(JSONTokenHash(json, token, dict)){
		case 0:
			JSONSetTokenFunc(json, NULL, tfunc0_nest);
			JSONParse(json);
			break;
		default:;
			JSONToken token_value = JSONTokenValue(json, token + 1);
			if(token_value.type == JSON_INT){
				printf("\nWOO: %d\n", token_value.value._int);
			}else{
				printf("\nAWW\n");
			}
			// printf("TokenType: %d\n", token_value.type);
			break;
	}
}

void nested_test(){
	JSONState state = JSONOpen("../resources/nested_test.json");
	JSONSetTokenFunc(&state, NULL, tfunc0_nest);
	JSONParse(&state);
	JSONFree(&state);
}