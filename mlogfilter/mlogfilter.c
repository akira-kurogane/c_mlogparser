#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mlogfilter_opts.h"
#include "ffc.h"

void print_usage(FILE* fstr) {
  fprintf(fstr, "Usage: mlogfilter [options] <mongod or mongos log file>\n");
}

void print_desc() {
  printf("The fast log filter for mongod and mongos log files. Restricted to simple\n\
  string comparison logic. If there is --ts-start argument a bifurcated search\n\
  will seek the beginning, i.e. avoid scanning the whole file. Without a\n\
  --ts-start argument mlogfilter will create a cache file to make repeated\n\
  searches faster.\n");
}

int main (int argc, char **argv) {

  int opt_err_flag = 0;
  int nonopt_arg_idx = parse_cmd_options(argc, argv, &opt_err_flag);

  if (opt_err_flag || nonopt_arg_idx >= argc) {
    print_usage(stderr);
    exit(EXIT_FAILURE);
  }

  if (verbose_flag) {
    dump_cmd_options();
  }

  if (!filter_ts_start && !filter_ts_end && !filter_components &&
      !filter_threadname_prefixes && !filter_severity_vals) {
	fprintf(stderr, "Aborting. No point to using mlogfilter when there are no filter arguments\n");
	fprintf(stderr, "Try --help for options description\n");
    print_usage(stderr);
    exit(EXIT_FAILURE);
  }

  if (help_flag) {
    print_usage(stdout);
    printf("\n");
    print_desc();
    printf("\n");
    print_options_help();
    free_options;
    exit(EXIT_SUCCESS);
  }
  
  while (nonopt_arg_idx < argc) {
    filter_file(argv[nonopt_arg_idx], verbose_flag, filter_ts_start,
			    filter_ts_end, filter_components, filter_threadname_prefixes, 
		        filter_severity_vals);
    nonopt_arg_idx++;
  }

  free_options;
  exit(EXIT_SUCCESS);
}
