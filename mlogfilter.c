#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

/* Flag set by ‘--verbose’. */
static int verbose_flag;
static char* filter_ts_start;
static char* filter_ts_end;
static char** filter_components;
static char** filter_threadname_prefixes;
static char** filter_severity_vals;

#include <string.h>
void parse_cmd_options(int argc, char **argv) {

  int c;

  while (1) {
    static struct option long_options[] = {
      {"verbose",     no_argument, &verbose_flag, 1},
      //TODO {"help",     no_argument, &help_flag, 1},
      {"ts-start",    required_argument, 0, 'm'},
      {"ts-end",      required_argument, 0, 'n'},
      {"component",   required_argument, 0, 'c'},
      {"thread-name", required_argument, 0, 'p'},
      {"severity",    required_argument, 0, 's'},
      {0, 0, 0, 0}
    };
    /* getopt_long stores the option index here. */
    int option_index = 0;

    c = getopt_long (argc, argv, "m:n:c:p:s:",
                     long_options, &option_index);

    /* Detect the end of the options. */
    if (c == -1)
      break;

    switch (c) {
      case 0:
        /* If this option set a flag, do nothing else now. */
        if (long_options[option_index].flag != 0)
{
printf("option %s set a flag, breaking\n", long_options[option_index].name);
          break;
}
        printf ("option %s", long_options[option_index].name);
        if (optarg)
          printf (" with arg %s", optarg);
        printf ("\n");
        break;

      case 'm':
        strcpy(filter_ts_start, optarg);
        //printf ("option -m with value `%s'\n", optarg);
        break;

      case 'n':
        strcpy(filter_ts_end, optarg);
        //printf ("option -n with value `%s'\n", optarg);
        break;

      case 'c':
//static char** filter_threadname_prefixes;
        printf ("option -c with value `%s'\n", optarg);
        break;

      case 'p':
//static char** filter_severity_vals;
        printf ("option -p with value `%s'\n", optarg);
        break;

      case 's':
//static char** filter_components;
        printf ("option -s with value `%s'\n", optarg);
        break;

      case '?':
        /* getopt_long already printed an error message. */
        break;

      default:
        abort ();
    }
  }

  /*if (verbose_flag)
    puts ("verbose flag is set"); */

  //TODO confirm there is a valid file, or STDIN if there is not
  /* Print any remaining command line arguments (not options). */
  if (optind < argc)
    {
      printf ("non-option ARGV-elements: ");
      while (optind < argc)
        printf ("%s ", argv[optind++]);
      putchar ('\n');
    }
}

void dump_cmd_options() {
  printf("ts-start    = \"%s\"\n", filter_ts_start);
  printf("ts-end      = \"%s\"\n", filter_ts_end);
  //printf("component   = \"%s\"\n", filter_components);
  //printf("thread-name = \"%s\"\n", filter_threadname_prefixes);
  //printf("severity    = \"%s\"\n", filter_severity_vals);
}

int main (int argc, char **argv) {
  parse_cmd_options(argc, argv);
dump_cmd_options();
  exit (0);
}
