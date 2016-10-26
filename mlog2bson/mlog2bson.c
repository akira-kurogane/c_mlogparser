#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lineparsers.h"

void
parse_lines(FILE* fstr) {
  int c;
  const size_t bufsize = 256 * 1024;
  char buf[bufsize];
  size_t line_ctr = 0;
  char* p;
  char* lend_p;

  c = getc(fstr);
  if (c == EOF)
    return;

  ungetc(c, fstr);
  c = 0;

  bson_t b;
  bson_init(&b);

  char* fgres = NULL;
  while (fgres = fgets(buf, bufsize, fstr)) {
    line_ctr++;

    lend_p = strchr(buf, '\n');
    if (!lend_p) {
      fprintf(stderr, "No newline read into buffer at 'line' %zu\n", line_ctr);
      continue;
    }
    while (*lend_p == '\n' || *lend_p == '\r') {
      *lend_p-- = '\0';
    }
    lend_p++; //finally set it one past new end
    
    bson_reinit(&b);
    int parse_line_err;
    char* parse_err_msg = NULL;
    parse_line_to_bson(&b, buf, line_ctr, &parse_err_msg);
    if (parse_err_msg) {
      fprintf(stderr, parse_err_msg);
      if (parse_err_msg[strlen(parse_err_msg) - 1] != '\n') {
        fprintf(stderr, "\n");
      }
      free(parse_err_msg);
    }

    if (fwrite(bson_get_data(&b), 1, b.len, stdout) != b.len) { 
       fprintf(stderr, "Failed to write to stdout, exiting.\n"); 
       exit(1); 
    } 

  }
  bson_destroy(&b);
}

void parse_file(char const *filepath) {
  FILE *fstr;

  if (strcmp(filepath, "-") == 0) {
    fstr = stdin;
  } else {
    fstr = fopen(filepath, "r");
    if (fstr == NULL) {
      fprintf(stderr, "fopen(\"%s\") failed with error code %d\n", filepath, errno);
      exit(EXIT_FAILURE);
    }
  }

  posix_fadvise(fileno(fstr), 0, 0, POSIX_FADV_SEQUENTIAL);

  parse_lines(fstr);

  if (ferror(fstr)) {
    fprintf(stderr, "reading stream of \"%s\" failed with error code %d\n", filepath, errno);
    exit(EXIT_FAILURE);
  }
}

int main(int argc, char** argv) {

  init_lineparsers();

  if (argc < 2) {
    parse_file("-");
  } else {
    parse_file(argv[1]);
  }

  exit(EXIT_SUCCESS);
}
