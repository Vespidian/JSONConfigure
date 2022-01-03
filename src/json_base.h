#ifndef JSON_BASE_H_
#define JSON_BASE_H_

#ifndef bool
	#define bool _Bool
#endif

#define JSMN_HEADER
#include <jsmn.h>

typedef struct JSONFuncObject JSONFuncObject;

/**
 * 
 */
typedef struct JSONState{
	char *path;
	bool is_loaded;
	JSONFuncObject **funcs;
	unsigned int depth;

	char **parsed_strings; // Strings parsed by 'JSONTokenValue' function are stored here
	unsigned int num_strings;

	char *json_string;
	jsmntok_t *tokens;
	int num_tokens;
	unsigned int current_token;
}JSONState;

typedef enum JSONConvertTypes{
	JSON_UNDEFINED = -1,
	JSON_BOOL,
	JSON_INT,
	JSON_FLOAT,
	JSON_STRING,
}JSONConvertTypes;

/**
 *  @brief test
 */
typedef struct JSONToken{
	JSONConvertTypes type;
	union{
		bool _bool;
		int _int;
		float _float;
		char *_string;
	}value;
}JSONToken;


void Error(const char *msg, ...);
/** --- Error Checking --- */

/**
 * Function pointer for the function to be used as an error callback
 */
typedef void (*JSONErrorFunc_tf)(const char *msg);

/**
 *  @brief Set the function which will be called when an error occurs within the JSON library
 */
void JSONSetErrorFunc(JSONErrorFunc_tf func);

/* --- Utility --- */

/**
 *  @brief Get the length of a token
 *  @param json pointer to the 'JSONState' your token is in
 *  @param token ID of your token
 *  @return The length of the specified token 
 */
int JSONTokenLength(JSONState *json, unsigned int token);

/**
 *  @brief
 *  @param json pointer to the 'JSONState' your token is in
 *  @param token ID of your token
 *  @return
 */
int JSONTokenHash(JSONState *json, unsigned int token, char **dict);

/**
 *  @brief
 *  @param json pointer to the 'JSONState' your token is in
 *  @param token ID of your token
 *  @return
 */
JSONToken JSONTokenValue(JSONState *json, unsigned int token);

/* --- State --- */

/**
 *  Function pointer to be called by the parser when encoutering a certain token (Set by JSONSetTokenFunc)
 */
typedef void (*JSONTokenFunc)(JSONState *json, unsigned int token);

/**
 *  @brief 
 */
void JSONSetTokenFunc(JSONState *json, char *type, JSONTokenFunc func_ptr);

/**
 * 
 */
JSONState JSONOpen(char *path);

/**
 * 
 */
void JSONParse(JSONState *json);

/**
 * 
 */
void JSONFree(JSONState *json);

#endif