#include <stdio.h>
#include "ffc.h"

void
parse_lines(FILE* fstr, int verbose_flag, 
		char* filter_ts_start, char* filter_ts_end,
		char** filter_components, char** filter_threadname_prefixes, 
		char* filter_severity_vals) {
  int c;
  const size_t bufsize = 256 * 1024;
  char buf[bufsize];
  char* strptime_p;

  c = getc(fstr);
  if (c == EOF)
    return;

  ungetc(c, fstr);
  c = 0;

  char* fgres = NULL;
  while (fgres = fgets(buf, bufsize, fstr)) {
	
	if (filter_components) {
      ;
	}
  }
}

int filter_file(const char* filepath, int verbose_flag, 
		char* filter_ts_start, char* filter_ts_end,
		char** filter_components, char** filter_threadname_prefixes, 
		char* filter_severity_vals) {
  printf("Yay, let's do %s\n", filepath);

  return 0;
}
