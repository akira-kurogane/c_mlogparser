#include <stdio.h>
#include <stdlib.h>
#include "mlogfilter_opts.h"

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

  if (help_flag) {
    print_usage(stdout);
    printf("\n");
    print_desc();
    printf("\n");
    print_options_help();
    free_options;
    exit(EXIT_SUCCESS);
  }
//dump_cmd_options();

  //TODO confirm there is a valid file, or STDIN if there is not
  while (nonopt_arg_idx < argc) {
    FILE *fstr;
    printf("Yay, let's filter %s\n", argv[nonopt_arg_idx]);
    //fstr = fopen(argv[nonopt_arg_idx], "r");
    //fclose(fstr);
    nonopt_arg_idx++;
  }

  free_options;
  exit(EXIT_SUCCESS);
}
