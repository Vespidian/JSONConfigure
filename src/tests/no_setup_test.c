#include "../json_base.h"

void no_setup_test(){
	JSONState json = JSONOpen("../resources/no_setup_test.json");
	JSONParse(&json); // Since we havent set any token functions, this call will do nothing
	JSONFree(&json);
}