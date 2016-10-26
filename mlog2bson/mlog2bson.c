#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "datemanip.h"
#include <bson.h>

void
parse_lines(FILE* fstr) {
  int c;
  const size_t bufsize = 256 * 1024;
  char buf[bufsize];
  char* ts;
  char* sev;
  char* cmpn;
  char* thrdnm;
  size_t line_ctr = 0;
  size_t llen;
  char* p;
  char* lend_p;
  char* strptime_p;
  char* msg; //pointer to message starting after the four fields

  struct tm tm;

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
    //todo: strip line breaks from end
    lend_p = strchr(buf, '\n');
    if (!lend_p) {
      fprintf(stderr, "No newline read into buffer at 'line' %zu\n", line_ctr);
      continue;
    }
    while (*lend_p == '\n' || *lend_p == '\r') {
      *lend_p-- = '\0';
    }
    lend_p++; //finally set it one past end
    
    bson_reinit(&b);

    struct timeval tv;
    if (buf[0] == '2' && buf[1] == '0' && buf[10] == 'T' && buf[19] == '.') { //looks like an ISO datetime at the front

      long int conn_id = 0;
      int valid_ll = 1; //valid log line
      //Take pointers and null-terminate the first four fields in the line
      ts = buf;
      sev = index(ts, ' ') + 1;  
      *(sev - 1) = '\0';
      cmpn = index(sev, ' ') + 1;  
      *(cmpn - 1) = '\0';
      thrdnm = index(cmpn, ' ') + 1;  
      *(thrdnm - 1) = '\0';
      while (*thrdnm == ' ') { //keep on sliding up further space chars
        thrdnm++;
      }
      if (*thrdnm != '[') { //this is not a properly formatted log line
        valid_ll = 0;
      } else {
        thrdnm++; // move from the '[' char to one after
        p = thrdnm;
        while (*p != ']' && *p != '\0' && (p - thrdnm) < 32) {
          p++;
        }
        if (*p != ']') { //this is not a properly formatted log line
          valid_ll = 0;
        } else {
          *p = '\0';
          msg = p + 1;
        }
      }
      while (*msg == ' ') {
        msg++;
      }

      char* strtol_endptr;
      if (strncmp(thrdnm, "conn", 4) == 0 && *(thrdnm + 4) >= '0' && *(thrdnm + 4) <= '9') {
        conn_id = strtol(thrdnm + 4, &strtol_endptr, 10);
        thrdnm[4] = '\0';
      }

      int timeval_parse_err = 0;
      tv = str_to_timeval(buf, &timeval_parse_err);
      if (timeval_parse_err) {
        valid_ll = 0;
      }

      if (valid_ll) {
//fprintf(stderr, "FIELDS: %s %s %s %s\n", ts, sev, cmpn, thrdnm);
        bson_append_timeval(&b, "ts", 2, &tv);
        bson_append_utf8(&b, "s", 1, sev, -1); //should be only char though
        bson_append_utf8(&b, "c", 1, cmpn, -1);
        bson_append_utf8(&b, "p", 1, thrdnm, -1);
        if (conn_id) {
          bson_append_int32(&b, "i", 1, conn_id);
        }

        bson_append_utf8(&b, "x", 1, msg, -1);

      } else {
        bson_append_bool(&b, "not_mongo_log", 13, 1);
        //todo: put all the removed chars terminating first four fields back
        bson_append_utf8(&b, "raw_line", 8, buf, -1);
      }

    } else {
      bson_append_bool(&b, "not_mongo_log", 13, 1);
      bson_append_utf8(&b, "raw_line", 8, buf, -1);
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

  init_datemanip();
  auto_set_min_max_ts_strings();

  if (argc < 2) {
    parse_file("-");
  } else {
    parse_file(argv[1]);
  }

  exit(EXIT_SUCCESS);
}
