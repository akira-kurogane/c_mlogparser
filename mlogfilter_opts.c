#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include "mlogfilter_opts.h"

void init_options() {
  filter_ts_start = malloc(1);
  filter_ts_start = NULL;
  filter_ts_end = malloc(1);
  filter_ts_end = NULL;
  filter_components = calloc(sizeof(char*), 1);
  filter_components[0] = NULL;
  filter_threadname_prefixes = calloc(sizeof(char*), 1);
  filter_threadname_prefixes[0] = NULL;
  filter_severity_vals = malloc(1);
  filter_severity_vals = NULL;
}

void free_options() {
  int i;
  char* p;

  free(filter_ts_start);
  free(filter_ts_end);

  i = 0;
  p = filter_components[i];
  while (p) {
    free(p);
    p = filter_components[i++];
  }
  free(filter_components);

  printf("thread-name = [ ");
  i = 0;
  p = filter_threadname_prefixes[i++];
  while (p) {
    free(p);
    p = filter_threadname_prefixes[++i];
  }
  free(filter_components);

  free(filter_severity_vals);
}

int parse_cmd_options(int argc, char **argv, int* err_flag) {

  int c;
  char* tmp_str;
  char* p;
  int tnp_len;
  int i;

  init_options();

  while (1) {
    static struct option long_options[] = {
      {"verbose",     no_argument, &verbose_flag, 1},
      {"help",        no_argument, &help_flag,    1},
      {"ts-start",    required_argument, 0, 'm'},
      {"ts-end",      required_argument, 0, 'n'},
      {"component",   required_argument, 0, 'c'},
      {"thread-name", required_argument, 0, 'p'},
      {"severity",    required_argument, 0, 's'},
      {0, 0, 0, 0}
    };
    /* getopt_long stores the option index here. */
    int option_index = 0;

    c = getopt_long(argc, argv, "m:n:c:p:s:", long_options, &option_index);

    /* Detect the end of the options. */
    if (c == -1)
      break;

    switch (c) {
      case 0:
        /* If this option set a flag, do nothing else now. */
        if (long_options[option_index].flag != 0)
          break;

        fprintf(stderr, "Unexpected option %s", long_options[option_index].name);
        if (optarg) {
          fprintf(stderr, " with arg %s", optarg);
        }
        fprintf(stderr, "\n");
        *err_flag = 1;
        break;

      case 'm':
        filter_ts_start = realloc(filter_ts_start, strlen(optarg) + 1);
        strcpy(filter_ts_start, optarg);
        //sanity enforcement
        if (filter_ts_start && strlen(filter_ts_start) == 0) {
          free(filter_ts_start);
          filter_ts_start = NULL;
        }
        break;

      case 'n':
        filter_ts_end = realloc(filter_ts_end, strlen(optarg) + 1);
        strcpy(filter_ts_end, optarg);
        //sanity enforcement
        if (filter_ts_end && strlen(filter_ts_end) == 0) {
          free(filter_ts_end);
          filter_ts_end = NULL;
        }
        break;

      case 'c':
        tmp_str = malloc(strlen(optarg) + 1);
        strcpy(tmp_str, optarg);
        p = tmp_str;
        tnp_len = 1;
        while (*p) {
          if (*p == ',') {
            *p = '\0';
            tnp_len++;
          }
		  p++;
        }
        filter_components = realloc(filter_components, sizeof(char*) * (tnp_len + 1));
        p = tmp_str;
        for (i = 0; i < tnp_len; i++) {
          filter_components[i] = malloc(strlen(p));
          strcpy(filter_components[i], p);
          p += strlen(p) + 1;
        }
        filter_components[tnp_len] = NULL;
        free(tmp_str);
        break;

      case 'p':
        tmp_str = malloc(strlen(optarg) + 1);
        strcpy(tmp_str, optarg);
        p = tmp_str;
        tnp_len = 1;
        while (*p) {
          if (*p == ',') {
            *p = '\0';
            tnp_len++;
          }
		  p++;
        }
        filter_threadname_prefixes = realloc(filter_threadname_prefixes, sizeof(char*) * (tnp_len + 1));
        p = tmp_str;
        for (i = 0; i < tnp_len; i++) {
          filter_threadname_prefixes[i] = malloc(strlen(p));
printf("threadname str being added: %s\n", p);
          strcpy(filter_threadname_prefixes[i], p);
          p += strlen(p) + 1;
        }
        filter_threadname_prefixes[tnp_len] = NULL;
        free(tmp_str);
        break;

      case 's':
    //N.b. this one is to be used an array of single chars, each their own filter value. As opposed to filter_ts_start or filter_ts_start, which are strings, where the whole string will be one filter value.
        filter_severity_vals = realloc(filter_severity_vals, strlen(optarg) + 1);
        strcpy(filter_severity_vals, optarg);
        //sanity enforcement
        if (filter_severity_vals && strlen(filter_severity_vals) == 0) {
          free(filter_severity_vals);
          filter_components = NULL;
        }
        break;

      case '?':
        /* getopt_long already printed an error message. */
        *err_flag = 1;
        break;

      default:
        *err_flag = 1;
    }
  }

  //sanity enforcement
  if (filter_components[0] == NULL) {
    filter_components = NULL;
  }
  //sanity enforcement
  if (filter_threadname_prefixes[0] == NULL) {
   filter_threadname_prefixes = NULL;
  }

  return optind; //return idx to non-option argv argument (would have been 
    //moved to end by getopt() functions if it wasn't already).
}

void dump_cmd_options() {
  char* p;
  size_t i;

  printf("verbose     = %s\n", verbose_flag ? "true" : "false");
  printf("help        = %s\n", help_flag ? "true" : "false");
  printf("ts-start    = \"%s\"\n", filter_ts_start);
  printf("ts-end      = \"%s\"\n", filter_ts_end);

  printf("component   = ");
  if (filter_components) {
    printf("[ ");
    i = 0;
    p = filter_components[i];
    while (p) {
      printf("\"%s\", ", p);
      p = filter_components[++i];
    }
    printf("]");
  } else {
    printf("NULL");
  }
  printf("\n");

  printf("thread-name = ");
  if (filter_threadname_prefixes) {
    printf("[ ");
    i = 0;
    p = filter_threadname_prefixes[i];
    while (p) {
      printf("\"%s\", ", p);
      p = filter_threadname_prefixes[++i];
    }
    printf(" ]");
  } else {
    printf("NULL");
  }
  printf("\n");

  printf("severity    = [ ");
  if (filter_severity_vals) {
    for (i = 0; i < strlen(filter_severity_vals); i++) {
      printf("%c, ", filter_severity_vals[i]);
    }
  }
  printf(" ]\n");
}

void print_options_help() {
  printf("Options:\n\
  --help This message\n\
  -m, --ts-start\n\
    Filter out log lines before this time. YYYY-MM-DDTHH:MM:SS format, \n\
    timezone of log file assumed.\n\
  -n, --ts-end\n\
    Filter out log lines after this time. YYYY-MM-DDTHH:MM:SS format, \n\
    timezone of log file assumed. Is inclusive, e.g '2016-09-14' will really\n\
    mean < 2016-09-15T00:00:00\n\
  -c, --component \n\
    Log component: (e.g. COMMAND, REPL, SHARDING, NETWORK). Comma-delimit to\n\
    include multiple.\n\
  -p, --thread-name\n\
    Starting string for '[threadName]' filtering. Comma-delimit to include\n\
    multiple. Case-sensitive. E.g. \"conn,repl,Repl,rs\" for both connection and\n\
    replication threads.\n\
  -s, --severity\n\
    One or more of the single-character values I, W, E, D. Join without spaces\n\
    if multiple wanted.\n\
");
}
