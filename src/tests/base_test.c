#include "../json_base.h"

static char *param3_hash[] = {
	"a",
	"b",
	"c",
	"branch",
	NULL
};

static void tfunc1_param3(JSONState *json, unsigned int token){
	// printf("PARAM3?\n");
	switch(JSONTokenHash(json, token, param3_hash)){
		case 0: // a
			// printf("received a\n");
			break;
		case 1: // b
			// printf("received b\n");
			break;
		case 2: // c
			// printf("received c\n");
			break;
		case 3: // branch
			// printf("received branch\n");
			JSONSetTokenFunc(json, NULL, tfunc1_param3);
			JSONParse(json);
			break;
		default:
			break;
	}
	
}

#include <stdio.h>
static void tfunc0_properties(JSONState *json, unsigned int token){
	// char *prop = malloc(JSONTokenLength(json, token) + 1);
	// memcpy(prop, json->json_string + json->tokens[token].start, JSONTokenLength(json, token));
	// prop[JSONTokenLength(json, token)] = 0;
	// printf("Property: %s\n", prop);
	// free(prop);
	// printf(" -");
	// printf("FOUND PROPERTY!: ");
	// PrintToken(json, token);
	// printf("type: %d\n", JSONTokenValue(json, token).type);
	if(JSONTokenValue(json, token).type == JSON_STRING){
	printf("VALUE!!!!!!: %s\n", JSONTokenValue(json, token)._string);

	}
}

static char *attr_dict[] = {
	"param1",
	"param2",
	"properties",
	"param3",
	NULL,
};
static void tfunc0_attribute(JSONState *json, unsigned int token){ // Called for each child element inside it
	// printf("FOUND ARRTRIBUTE: %d : ", JSONTokenHash(json, token, attr_dict));
	switch(JSONTokenHash(json, token, attr_dict)){
		// properties and param3 are not here since they have seperate functions
		case 0:; // param1
			// printf("PARAM1\n");
			break;
		case 1:; // param2
			JSONToken token_value = JSONTokenValue(json, token + 1);
			if(token_value.type == JSON_FLOAT){
				// printf("%f * 2 = %f\n", token__float, token__float * 2);
			}
			// printf("%d\n", token_value.type);
			// printf("PARAM2\n");
			break;
		case 2: // properties
			JSONSetTokenFunc(json, NULL, tfunc0_properties);
			JSONParse(json);
			break;
		case 3: // param3
			// tfunc1_param3(json, token);
			// printf("Found PARAM3\n");
			JSONSetTokenFunc(json, NULL, tfunc1_param3);
			JSONParse(json);
			break;
		default:
			break;
	}

}

static char *root_dict[] = {
	"properties",
	"attribute",
	"booleans",
	NULL
};

static void tfunc0_default(JSONState *json, unsigned int token){
	// printf("Default function!\n");
	switch(JSONTokenHash(json, token, root_dict)){
		case 0: // properties
		// json->current_token++;
			JSONSetTokenFunc(json, NULL, tfunc0_properties);
			JSONParse(json);
			break;
		case 1: // attribute
			JSONSetTokenFunc(json, NULL, tfunc0_attribute);
			JSONParse(json);
			break;
		case 2: // booleans
			break;
		default:
			break;
	}
}

void base_test(){
	JSONState json_state = JSONOpen("../resources/base_test.json");

	JSONSetTokenFunc(&json_state, NULL, tfunc0_default);
	JSONParse(&json_state);
	JSONFree(&json_state);
}


char json_string[] = "{attribute:{\"param1\":5,param2:45.35406,properties:[false,down,normal],\"param3\":{branch:{thing1,thing2,thing3,thing4},a:1,b:4.5,c:false}},booleans:false,properties:[active,dynamic,3,5.6,\"something unknown\"],attribute:{param2:32.16},random:19}";
void base_test_string(){
	JSONState json = JSONRead(json_string, NULL);

	JSONSetTokenFunc(&json, NULL, tfunc0_default);
	JSONParse(&json);
	JSONFree(&json);
}