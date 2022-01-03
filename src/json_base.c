#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>

#include "json_utils.h"

#include "json_base.h"

#include <jsmn.h>

typedef struct JSONFuncObject{
	bool is_null;
	char *name;
	JSONTokenFunc function;
}JSONFuncObject;

/* --- ERROR CHECKING SETUP --- */
// By default we simply print out errors to stdout
static void BaseErrorFunc(const char *msg){
	printf("%s", msg);
}
static JSONErrorFunc_tf error_func = &BaseErrorFunc;
void Error(const char *msg, ...){
	if(msg != NULL){
		va_list va_format;
		va_list copy;
		
		//Use var args to create formatted text
		va_start(va_format, msg);
		va_copy(copy, va_format);
		int length = vsnprintf(NULL, 0, msg, copy);
		va_end(copy);
		char *formatted_text = malloc(length + 1);
		if(formatted_text != NULL){
			vsnprintf(formatted_text, length + 1, msg, va_format);
			formatted_text[length] = 0;

			error_func(formatted_text);

			free(formatted_text);
		}
		va_end(va_format);
	}
}

void JSONSetErrorFunc(JSONErrorFunc_tf func){
	if(func != NULL){
		error_func = func;
	}
}

/* --- UTILITY FUNCTIONS --- */
int JSONTokenLength(JSONState *json, unsigned int token){
	return (json->tokens[token].end - json->tokens[token].start);
}

static int strntol(char *string, unsigned int length){
	int result = 0;
	int decimal_place = 1;
	for(int i = 0; i < length; i++){
		if(string[length - 1 - i] >= '0' && string[length - 1 - i] <= '9'){
			result += (string[length - 1 - i] - '0') * decimal_place;
			decimal_place *= 10;
		}
	}
	if(string[0] == '-'){
		result *= -1;
	}

	return result;
}

int SkipToken(JSONState *json, unsigned int token){
	// 'token' indexes the name of a property or element
	// 'token + 1' is the data of this property or element
	// and 'token + 2' is the name of the element's first child if it has any children
	// if this element has no children, then 'token + 2' is the name of the next element
	int current_token = token + 1;
	for(int i = 0; i < json->tokens[token].size; i++){
		current_token = SkipToken(json, current_token);
	}
	return current_token;
}

bool CompareToken(JSONState *json, unsigned int token, char *string){
	bool result = true;
	if(string != NULL){
		int string_len = strlen(string);
		if(string_len != JSONTokenLength(json, token)){
			result = false;
		}else{
			for(int i = 0; i < string_len; i++){
				if(json->json_string[json->tokens[token].start + i] != string[i]){
					result = false;
				}
			}
		}
	}else{
		result = false;
	}
	return result;
}

int JSONTokenHash(JSONState *json, unsigned int token, char **dict){
	if(json->tokens[token].type == JSMN_STRING || json->tokens[token].type == JSMN_PRIMITIVE){
		for(int i = 0; dict[i] != NULL; i++){
			if(CompareToken(json, token, dict[i])){
				return i;
			}
		}
	}
	// Cannot get hash from this property
	return -1;
}


/* --- JSON PARSING --- */
JSONState JSONOpen(char *path){
	JSONState json;
	if(path != NULL){
		json.path = malloc(strlen(path) + 1);
		memcpy(json.path, path, strlen(path));
		json.path[strlen(path)] = 0;
	}

	json.json_string = NULL;
	FILE *fp = fopen(path, "rb");
	if(fp == NULL){
		Error("%s: error opening JSON file", path);
		goto exit_error;
	}
	fseek(fp, 0, SEEK_END);
	long file_length = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	if(file_length == -1){
		Error("%s: error reading length of JSON file", path);
		goto exit_error;
	}
	// printf("file length: %ld\n", file_length);
	json.json_string = malloc(file_length + 1);
	if(json.json_string == NULL){
		Error("%s: error: Could not allocate space for JSON file string", json.path);
		goto exit_error;
	}
	size_t read_error_check = fread(json.json_string, 1, file_length, fp);

	// Null terminate the copied string
	json.json_string[file_length] = 0;

	if(read_error_check != file_length){
		Error("%s: error reading data from file", path);
		goto exit_error;
	}

	// ----- Begin parsing -----

	jsmn_parser parser;

	// Initialize the jsmn parser
	jsmn_init(&parser);

	// Parse once to get number of tokens
	json.num_tokens = jsmn_parse(&parser, json.json_string, strlen(json.json_string), NULL, 0);

	// Error checking
	switch(json.num_tokens){
		case JSMN_ERROR_INVAL:
			Error("%s: error: JSMN_ERROR_INVAL\n", path);
			goto exit_error;
			break;
		case JSMN_ERROR_NOMEM:
			Error("%s: error: JSMN_ERROR_NOMEM\n", path);
			goto exit_error;
			break;
		case JSMN_ERROR_PART:
			Error("%s: error: JSMN_ERROR_PART\n", path);
			goto exit_error;
			break;
	}
	if(json.num_tokens <= 0){
		goto exit_error;
	}

	// Allocate space for tokens array
	json.tokens = malloc(sizeof(jsmntok_t) * (json.num_tokens + 1));

	// The jsmn parser needs to be re-initialized after having called 'jsmn_parse'
	jsmn_init(&parser);

	// Now actually parse the file and put tokens in token array
	jsmn_parse(&parser, json.json_string, strlen(json.json_string), json.tokens, json.num_tokens);


	if(json.tokens[0].type == JSMN_OBJECT){
		// Define values
		json.parsed_strings = NULL;
		json.num_strings = 0;
		json.is_loaded = true;
		json.funcs = malloc(sizeof(JSONFuncObject*));
		// if(json.funcs == NULL){

		// }
		json.funcs[0] = malloc(sizeof(JSONFuncObject) * 2);
		json.funcs[0][0].is_null = true; // Set the default function to null since it hasnt been set yet
		json.funcs[0][1].is_null = true;
		json.depth = 0;
		json.current_token = 1;

	}else{
		Error("%s: error: The entire JSON file must be surrounded by {} to be valid", path);
		exit_error:
		json.is_loaded = false;
		free(json.path);
		json.path = NULL;
		
		free(json.json_string);
		json.json_string = NULL;
	}

	fclose(fp);

	return json;
}

void JSONSetTokenFunc(JSONState *json, char *type, JSONTokenFunc func_ptr){
	if(json->is_loaded && json != NULL && func_ptr != NULL){
		// Count the number of functions at the current depth
		unsigned int num_funcs_at_depth = 1; // 0 index is the default function

		if(type == NULL){
			json->funcs[json->depth][0].function = func_ptr;
			json->funcs[json->depth][0].name = NULL;
			json->funcs[json->depth][0].is_null = false;
		}else{

			int function_exists = -1;
			while(!json->funcs[json->depth][num_funcs_at_depth].is_null){
				if(strcmp(json->funcs[json->depth][num_funcs_at_depth].name, type) == 0){
					function_exists = num_funcs_at_depth;
				}
				num_funcs_at_depth++;
				// Check if a function already exists with the specified 'type'
			}

			if(function_exists != -1){
				JSONFuncObject *state_func = &json->funcs[json->depth][function_exists];
				state_func->function = func_ptr;
			}else{
				JSONFuncObject *tmp = realloc(json->funcs[json->depth], sizeof(JSONFuncObject) * (num_funcs_at_depth + 2)); // TODO: Make sure there arent buffer overruns here
				
				if(tmp != NULL){
					json->funcs[json->depth] = tmp;
					JSONFuncObject *state_func = &json->funcs[json->depth][num_funcs_at_depth];
					state_func->is_null = false;
					state_func->name = malloc(strlen(type) + 1);
					memcpy(state_func->name, type, strlen(type));
					state_func->name[strlen(type)] = 0;
					state_func->function = func_ptr;
					num_funcs_at_depth++;

					json->funcs[json->depth][num_funcs_at_depth].is_null = true;
					json->funcs[json->depth][num_funcs_at_depth].name = NULL;
					json->funcs[json->depth][num_funcs_at_depth].function = NULL;
				}else{
					Error("%s: error: Could not allocate space for a new json token type\n");
				}

				// printf("Set function for type: '%s'\n", type);
			}
		}
	}
}

static void FreeFuncDepth(JSONState *json, unsigned int depth){
	if(json->is_loaded){
		for(int i = 0; !json->funcs[depth][i].is_null; i++){
			free(json->funcs[depth][i].name);
			json->funcs[depth][i].name = NULL;
		}
		free(json->funcs[depth]);
		json->funcs[depth] = NULL;
	}
}

static void PrintToken(JSONState *json, unsigned int token){
	char *str = malloc(JSONTokenLength(json, token) + 1);
	memcpy(str, json->json_string + json->tokens[token].start, JSONTokenLength(json, token));
	str[JSONTokenLength(json, token)] = 0;

	// strupr(str);

	// printf("%s:{", str);
	free(str);
	str = NULL;
}

void JSONParse(JSONState *json){
	if(json->is_loaded && json != NULL){
		// Count how many functions we have at the current depth
		unsigned int num_funcs_at_depth = 1;
		while(!json->funcs[json->depth][num_funcs_at_depth].is_null){
			num_funcs_at_depth++;
		}
		// If there are no functions to be called AND there is no default function, there is no reason to parse anything
		if(num_funcs_at_depth != 1 || !json->funcs[json->depth][0].is_null){
			// Allocate space for any new 'JSONSetFunc' calls from the next depth
			JSONFuncObject **tmp = realloc(json->funcs, sizeof(JSONFuncObject*) * (json->depth + 2)); // TODO: Make sure there arent buffer overruns here
			if(tmp != NULL){
				json->funcs = tmp;
			}else{
				Error("%s: error: Could not allocate space for another nested 'JSONParse'", json->path);
				return;
			}

			json->depth++;
			json->funcs[json->depth] = malloc(sizeof(JSONFuncObject) * 2);
			
			// Default function
			json->funcs[json->depth][0].is_null = true;
			json->funcs[json->depth][0].name = NULL;
			json->funcs[json->depth][0].function = NULL;

			// Start of custom functions
			json->funcs[json->depth][1].is_null = true;
			json->funcs[json->depth][1].name = NULL;
			json->funcs[json->depth][1].function = NULL;

			// Loop through the tokens at the current depth and compare them to the functions that have been set using 'JSONSetFunc'
			// If they do match, call them (go down to the next depth / level)
			int num_objects;
			if(json->depth == 1){
				num_objects = json->tokens[0].size;
			}else{
				num_objects = json->tokens[json->current_token - 1].size;
			}
			int current_token = json->current_token;
			for(int i = 0; i < num_objects; i++){
				// for(int t = 0; t < json->depth; t++){
				// 	printf("	");
				// }
				// PrintToken(json, current_token);
				// if(json->tokens[current_token+1].size >= 2){
				// 	printf("\n");
				// }
				for(int j = 1; j < num_funcs_at_depth; j++){
					if(CompareToken(json, current_token, json->funcs[json->depth - 1][j].name)){
						// Go through each object nested in the current token and pass that token to the function
						if(json->tokens[current_token].type == JSMN_ARRAY || json->tokens[current_token].type == JSMN_OBJECT){
								json->current_token = current_token + 2;
								json->funcs[json->depth - 1][j].function(json, current_token);
						}
						break;
					}
				}
				if(num_funcs_at_depth == 1 && !json->funcs[json->depth - 1][0].is_null){ // Call the default function if its the only function
					json->current_token = current_token + 2;
					json->funcs[json->depth - 1][0].function(json, current_token);
				}
				// if(json->tokens[current_token+1].size >= 2){
				// 	for(int t = 0; t < json->depth; t++){
				// 		printf("	");
				// 	}
				// }
				// printf("}\n");

				// Jump to the next token at the current depth
				if(current_token != json->num_tokens){
					current_token = SkipToken(json, current_token);
				}
			}
			FreeFuncDepth(json, json->depth);
			json->depth--;
		}
	}
}

void JSONFree(JSONState *json){
	if(json->is_loaded){
		// for(int i = 0; i < json->depth; i++){
			FreeFuncDepth(json, 0);
		// }
		free(json->funcs);
		json->funcs = NULL;

		for(int i = 0; i < json->num_strings; i++){
			free(json->parsed_strings[i]);
			json->parsed_strings[i] = NULL;
		}

		free(json->path);
		json->path = NULL;

		free(json->parsed_strings);
		json->parsed_strings = NULL;

		free(json->json_string);
		json->json_string = NULL;

		free(json->tokens);
		json->tokens = NULL;
	}
}

char *bool_dict[] = {
	"true",
	"false",
	NULL
};

JSONToken JSONTokenValue(JSONState *json, unsigned int token){
	if(json->is_loaded){
		bool is_bool = false;
		bool is_int = false;
		bool is_float = false;
		bool is_string = false;
		if(json->tokens[token].type == JSMN_PRIMITIVE){
			if(JSONTokenHash(json, token, bool_dict) != -1){
				// Boolean
				is_bool = true;

			}else{
				char *str = json->json_string;
				for(int i = json->tokens[token].start; i < json->tokens[token].end; i++){
					if(!is_string){
						if(str[i] == '.'){
							if(is_float){ // Can only be a float if there is a single decimal point '.'
								is_float = false;
								is_string = true;
								break;
							}else{
								// Float
								is_float = true;
							}
						}else if(!(str[i] <= '9' && str[i] >= '0')){
							// String
							is_float = false;
							is_string = true;
							break;
						}
					}
				}
				if(!is_string && !is_float){
					// Int
					is_int = true;
				}
			}

		}else if(json->tokens[token].type == JSMN_STRING){
			// String
			is_string = true;
		}else{
			// Undefined / Array / Object
		}

		JSONToken token_object;
		token_object.type = JSON_UNDEFINED;
		token_object.value._string = NULL;

		char *token_string = json->json_string + json->tokens[token].start;
		int token_string_length = JSONTokenLength(json, token) + 1;

		// Convert the data to the specified type
		if(is_string){
			token_object.type = JSON_STRING;
			char **tmp = realloc(json->parsed_strings, sizeof(char*) * (json->num_strings + 1));
			if(tmp != NULL){
				json->parsed_strings = tmp;
				json->parsed_strings[json->num_strings] = malloc(token_string_length);
				if(json->parsed_strings[json->num_strings] != NULL){
					strncpy(json->parsed_strings[json->num_strings], token_string, token_string_length);
					json->parsed_strings[json->num_strings][token_string_length - 1] = 0;
					token_object.value._string = json->parsed_strings[json->num_strings];
				}else{
					Error("%s: error: Could not allocate return string for 'JSONTokenValue'", json->path);
				}
				json->num_strings++;
			}else{
				Error("%s: error: Could not reallocate strings array for 'JSONTokenValue'", json->path);
			}
			// printf("STRING!\n");
		}else if(is_int){
			token_object.type = JSON_INT;
			token_object.value._int = strntol(token_string, token_string_length);
			// printf("INT!\n");
		}else if(is_bool){
			token_object.type = JSON_BOOL;
			token_object.value._bool = JSONTokenHash(json, token, bool_dict);
			// printf("BOOL!\n");
		}else if(is_float){
			token_object.type = JSON_FLOAT;
			char *tmp_float_string = malloc(token_string_length + 1);
			if(tmp_float_string != NULL){
				strncpy(tmp_float_string, token_string, token_string_length);
				token_object.value._float = strtod(tmp_float_string, NULL);
				free(tmp_float_string);
				tmp_float_string = NULL;
			}else{
				Error("%s: error: Could not allocate string for float conversion", json->path);
			}
			// printf("FLOAT!\n");
		}else{
			// printf("OTHER!\n");
		}
		return token_object;
	}else{
		Error("%s: error: Attempting to get value from an uninitialized 'JSONState'", json->path);
		return (JSONToken){JSON_UNDEFINED};
	}
}