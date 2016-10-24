int help_flag;
int verbose_flag;
char* filter_ts_start;
char* filter_ts_end;
char** filter_components;
char** filter_threadname_prefixes;
char* filter_severity_vals; //N.b. this one is meant to be an array of single chars, each their own filter value. As opposed to filter_ts_start or filter_ts_start, which are strings, where the whole string will be one filter value.

void init_options();
void free_options();
int  parse_cmd_options(int argc, char **argv, int* err_flag);
void dump_cmd_options();
void print_options_help();

