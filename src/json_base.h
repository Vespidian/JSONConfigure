#ifndef JSON_BASE_H_
#define JSON_BASE_H_

#ifndef bool
	#define bool _Bool
#endif

#define JSMN_HEADER
#include <jsmn.h>

typedef struct JSONFuncObject JSONFuncObject;

/**
 *  @brief State which stores all the information necessary to parse a file
 */
typedef struct JSONState{
	// Describes whether or not this state currently has a file loaded
	bool is_loaded;

	// Path to the file loaded with 'JSONOpen'
	char *path;

	// Internal
	JSONFuncObject **funcs;

	// Current depth of the json parser
	unsigned int depth;

	// Strings parsed by 'JSONTokenValue' function are stored here (Internal)
	char **parsed_strings;
	// Internal
	unsigned int num_strings;

	// Internal
	char *json_string;

	// Internal
	jsmntok_t *tokens;

	// Internal
	int num_tokens;

	// Internal
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
 *  @brief Stores the value of a json token
 */
typedef struct JSONToken{
	// The type of the token (int, float, string, ..)
	JSONConvertTypes type;
	
	// The actual value of the token (only the one specified by 'type' should be accessed)
	union{
		bool _bool;
		int _int;
		float _float;
		char *_string;
	}value;
}JSONToken;


// void Error(const char *msg, ...);
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
 *  @param json - pointer to the 'JSONState' your token is in
 *  @param token - ID of your token
 *  @return The length of the specified token 
 */
int JSONTokenLength(JSONState *json, unsigned int token);

/**
 *  @brief Loops through the provided dictionary looking for a string that matches the provided token's string (See usage in example source file)
 *  @param json - pointer to the 'JSONState' your token is in
 *  @param token - ID of your token
 *  @param dict - Dictionary (array of strings) of possible token strings (NULL terminated)
 *  @return The index of the string in the dictionary or -1 if the string is not found
 */
int JSONTokenHash(JSONState *json, unsigned int token, char **dict);

/**
 *  @brief Get the value of a token
 *  brief JSONToken.value.string should be copied if you want to use it later, as calling 'JSONFree' will free the string
 *  @param json - pointer to the 'JSONState' your token is in
 *  @param token - ID of your token
 *  @return A struct containing the type of the token and a union containing the value
 */
JSONToken JSONTokenValue(JSONState *json, unsigned int token);


/* --- State --- */

/**
 *  Function pointer to be called by the parser when encoutering a certain token (Set by JSONSetTokenFunc)
 */
typedef void (*JSONTokenFunc)(JSONState *json, unsigned int token);

/**
 *  @brief Set a function to be called when the parser encounters a certain token string
 *  @param json - Pointer to the 'JSONState' to be used
 *  @param type - Specific string to trigger the calling of 'func_ptr' (or NULL to specify 'func_ptr' as the default function, i.e. called for any undefined tokens)
 *  @param func_ptr - Pointer to the 'JSONTokenFunc' to be called when 'type' is encountered
 */
void JSONSetTokenFunc(JSONState *json, char *type, JSONTokenFunc func_ptr);

/**
 *  @brief Open, read, and tokenize the specified file
 *  @param path - Path to file to be read
 *  @return A JSONState ready to be passed to 'JSONSetTokenFunc' and 'JSONParse'
 */
JSONState JSONOpen(char *path);

/**
 *  @brief Loop through tokens only at the current depth and call any 'JSONTokenFunc' that have been set
 *  @param json - Pointer to the 'JSONState' to be used
 */
void JSONParse(JSONState *json);

/**
 *  @brief Free a JSONState
 *  @param json - Pointer to JSONState to be freed
 */
void JSONFree(JSONState *json);

#endif