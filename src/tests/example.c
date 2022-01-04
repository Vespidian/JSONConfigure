#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../json_base.h"

// Dictionary items should ideally be ordered from most to least commonly occuring
static char *item_dict[] = {
	"value",
	"onclick",
	NULL // Dictionaries must ALWAYS be NULL terminated
};
static void tfunc_menuitem(JSONState *json, unsigned int token){
	if(json->tokens[token].type != JSMN_OBJECT){
		switch(JSONTokenHash(json, token, item_dict)){
			case 0: // value
				printf("Value: ");
				JSONPrint(json, token + 1);
				printf("\n");
				break;
			case 1: // onclick
				printf("OnClick: ");
				JSONPrint(json, token + 1);
				printf("\n");
				break;
			default:
				break;
		}
	}else{
		printf("\n-- Menu Item: --\n");
		JSONSetTokenFunc(json, NULL, tfunc_menuitem);
		JSONParse(json);
	}
}

// Dictionary items should ideally be ordered from most to least commonly occuring
static char *menu_dict[] = {
	"id",
	"popup",
	"menuitem",
	"text",
	NULL // Dictionaries must ALWAYS be NULL terminated
};
static void tfunc_menu(JSONState *json, unsigned int token){
	switch(JSONTokenHash(json, token, menu_dict)){
		case 0:; // id (int, float, bool)
			JSONToken id = JSONTokenValue(json, token + 1); // Get the value of the token (token + 1)
			if(id.type == JSON_INT){ // Make sure the type is what we think it is
				// id.value._int; // Do whatever with it..
			}
			break;
		case 1: // popup (nested items)
			JSONSetTokenFunc(json, "menuitem", &tfunc_menu);
			JSONParse(json);
			break;
		case 2: // menuitem (object / array)
			JSONSetTokenFunc(json, NULL, &tfunc_menuitem);
			JSONParse(json);
			break;
		case 3:; // text (string)
			JSONToken text = JSONTokenValue(json, token + 1); // Get the value of the token (token + 1)
			if(text.type == JSON_STRING){ // Make sure the type is what we think it is
				{ // Copy the string into our own string if we intend to use it after the current 'JSONState' gets freed
					char *string = malloc(strlen(text.value._string) + 1);
					strcpy(string, text.value._string);
					string[strlen(text.value._string)] = 0;
				}
				// OR
				{ // Simply use it temporarily (not keeping any pointers to it after 'JSONState' is freed)
					printf("%s\n", text.value._string);
				}
			}
			break;
		default: // Any other tokens
			break;
	}
}

static void tfunc_example(JSONState *json, unsigned int token){
	JSONSetTokenFunc(json, NULL, tfunc_menu);
	JSONParse(json);
}

void example_test(){
		JSONState json = JSONOpen("../resources/example.json");
		JSONSetTokenFunc(&json, "menu", tfunc_example); // Assign a function only to 'menu', we dont want anything else
		JSONParse(&json); // Increments the json depth for any deeper tokens and decrements it before returning
		JSONFree(&json);
}