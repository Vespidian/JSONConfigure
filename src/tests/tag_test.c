#include <stdio.h>
#include "../json_base.h"

static char *object_dict[] = {
	"name",
	"tags",
	"objects",
	NULL
};
void tfunc_object(JSONState *json, unsigned int token){
	if(json->tokens[token].type == JSMN_OBJECT){ // Parse each object {name: ..., tags: [...]}
		JSONSetTokenFunc(json, NULL, tfunc_object); // And call this function again
		JSONParse(json);
	}else{
		switch(JSONTokenHash(json, token, object_dict)){
			case 0: // name
				printf("name: ");
				JSONPrint(json, token + 1);
				printf("\n");
				break;
			case 1: // tags (Loop over each tag in the array and print them)
				printf("tags: \n");
				for(int i = 0; i < json->tokens[token + 1].size; i++){
					printf("	");
					JSONPrint(json, token + 2 + i);
					printf("\n");
				}
				printf("\n");
				break;
			case 2: // objects
				JSONSetTokenFunc(json, NULL, tfunc_object);
				JSONParse(json);
				break;
			default:
				break;
		}
	}
}

void tag_test(){
	JSONState json = JSONOpen("../resources/tag_test.json");
	JSONSetTokenFunc(&json, "objects", tfunc_object);
	JSONParse(&json);
	JSONFree(&json);
}