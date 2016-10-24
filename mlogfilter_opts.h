static int verbose_flag;
static char* filter_ts_start;
static char* filter_ts_end;
static char** filter_components;
static char** filter_threadname_prefixes;
static char** filter_severity_vals;

void parse_cmd_options(int argc, char **argv);
void dump_cmd_options();

