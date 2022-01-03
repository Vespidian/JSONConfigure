#ifndef JSON_UTILS_H_
#define JSON_UTILS_H_

// #include <string.h>
#include "json_base.h"



// static Vector3 HexToColor(char *hex){
// 	if(hex[0] == '#'){
// 		hex = hex + 1;
// 	}
// 	short v[3] = {0};
// 	char o1, o2;
// 	o1 = o2 = 0;
// 	for(int i = 0; i < 3; i++){
// 		if(hex[i * 2] <= '9'){
// 			o1 = '0';
// 		}else if(hex[i * 2] <= 'F'){
// 			o1 = 'A' - 10;
// 		}else if(hex[i * 2] <= 'f'){
// 			o1 = 'a' - 10;
// 		}
// 		if(hex[i * 2 + 1] <= '9'){
// 			o2 = '0';
// 		}else if(hex[i * 2 + 1] <= 'F'){
// 			o2 = 'A' - 10;
// 		}else if(hex[i * 2 + 1] <= 'f'){
// 			o2 = 'a' - 10;
// 		}
// 		v[i] = ((hex[i * 2] - o1) << 4) + (hex[i * 2 + 1] - o2);
// 	}
// 	return  (Vector3){v[0] / 255.0, v[1] / 255.0, v[2] / 255.0};
// }




// void SkipToken(JSONState *json){
// 	// 'token' indexes the name of a property or element
// 	// 'token + 1' is the data of this property or element
// 	// and 'token + 2' is the name of the element's first child if it has any children
// 	// if this element has no children, then 'token + 2' is the name of the next element
// 	json->current_token++;
// 	for(int i = 0; i < json->tokens[token].size; i++){
// 		SkipToken(json);
// 	}
// }




#endif