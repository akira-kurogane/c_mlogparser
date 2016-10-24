#include <stdio.h>
#include <stdlib.h>
#include "mlogfilter_opts.h"

extern int verbose_flag;
extern char* filter_ts_start;
extern char* filter_ts_end;
extern char** filter_components;
extern char** filter_threadname_prefixes;
extern char** filter_severity_vals;

int main (int argc, char **argv) {
  parse_cmd_options(argc, argv);
dump_cmd_options();
  exit (0);
}
