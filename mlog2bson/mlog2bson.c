#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "datemanip.h"

void
parse_lines(FILE* fstr) {
  int c;
  const size_t bufsize = 256 * 1024;
  char buf[bufsize];
  char* strptime_p;

  struct tm tm;

  c = getc(fstr);
  if (c == EOF)
    return;

  ungetc(c, fstr);
  c = 0;

  char* fgres = NULL;
  while (fgres = fgets(buf, bufsize, fstr)) {

    if (buf[10] == 'T' && buf[19] == '.') { //assume ISO datetime
      //fprintf(stdout, "%s", buf);

//      memset(&tm, 0, sizeof(struct tm));
//      strptime_p = strptime(buf, iso_dt_fmt, &tm);
//      if (strptime_p != buf + iso_fmt_len) {
//        fprintf(stderr, "bad format line (not ISO date string): %s", buf);
//        continue;
//      }
//
//      char *p = buf + 20; //the end of the "YYYY-mm-ddTHH:MM:SS" string, where the decimal for the milliseconds is expected to be
//      le.ts.tv_usec = 1000 * strtol(p, &p, 10);
//      if (*endptr == 'Z') {
//        le.ts.tv_sec = time_to_epoch(&tm, 0, 0);
//      } else {
//        int utc_offset;
//        utc_offset = (int)strtol(p, &p, 10);
//        le.ts.tv_sec = time_to_epoch(&tm, utc_offset / 100, utc_offset % 100);
//      }


    } else {
      fprintf(stderr, "bad format line: %s", buf);
      continue;
    }
  }
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
