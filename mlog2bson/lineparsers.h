#include <bson.h>

void init_lineparsers();
void parse_line_to_bson(bson_t* bson_doc, char* str, size_t line_no, char** err_msg_p);
