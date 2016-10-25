#include <stdio.h>
#include <string.h>
#include <strings.h>
#include "ffc.h"

void
filter_lines(FILE* fstr, int verbose_flag, 
    char* filter_ts_start, char* filter_ts_end,
    char** filter_components, char** filter_threadname_prefixes, 
    char* filter_severity_vals) {
  int c;
  const size_t bufsize = 256 * 1024;
  char buf[bufsize];
  char* ts;
  char* sev;
  char* cmpn;
  char* thrdnm;
  int r; //line should be filtered out
  int i;

  c = getc(fstr);
  if (c == EOF)
    return;

  ungetc(c, fstr);
  c = 0;

  char* fgres = NULL;
  while (fgres = fgets(buf, bufsize, fstr)) {
    r = 0;
    if (buf[0] == '2' && buf[1] == '0' && buf[10] == 'T' && buf[19] == '.') { //looks like an ISO datetime at the front

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
      //todo ? confirm this current thrdnm points to a '[' char
      thrdnm++; // move from the '[' char to one after
      *(index(thrdnm, ' ')) = '\0'; //just in case
      *(index(thrdnm, ']')) = '\0'; //todo ? confirm there was a ']' char on the end
//printf("FIELDS: %s %s %s %s\n", ts, sev, cmpn, thrdnm);

      if (!r && filter_ts_start && strcmp(filter_ts_start, ts) >= 0) {
        r = 1;
      }

      if (!r && filter_ts_end && strcmp(filter_ts_end, ts) < 0) {
        r = 1;
      }

      if (!r && filter_components) {
        int cmpn_match = 0;
        i = 0;
        while (filter_components[i] != NULL) {
          if (strncmp(filter_components[i], cmpn, strlen(filter_components[i])) == 0) {
            cmpn_match = 1;
          }
          i++;
        }
        if (!cmpn_match) {
          r = 1;
        }
      }

      if (!r && filter_threadname_prefixes) {
        int thrdnm_match = 0;
        i = 0;
        while (filter_threadname_prefixes[i] != NULL) {
          if (strncmp(filter_threadname_prefixes[i], thrdnm, strlen(filter_threadname_prefixes[i])) == 0) {
            thrdnm_match = 1;
          }
          i++;
        }
        if (!thrdnm_match) {
          r = 1;
        }
      }

      if (!r && filter_severity_vals) {
        int sev_match = 0;
        int i = 0;
        while (filter_severity_vals[i] != '\0') {
          if (filter_severity_vals[i] == *sev) {
            sev_match = 1;
          }
					i++;
        }
        if (!sev_match) {
          r = 1;
        }
      }

    } else {
      r = 1; // this is non-standard line
    }

    if (!r) {
      //put the spaces and braces back
      *(ts + strlen(ts)) = ' ';
      *(sev + strlen(sev)) = ' ';
      *(cmpn + strlen(cmpn)) = ' ';
      *(thrdnm + strlen(thrdnm)) = ']'; //put the ] back
      *(thrdnm + strlen(thrdnm)) = ' '; //put the space back

      printf(buf);
    }
  }
}

int filter_file(const char* filepath, int verbose_flag, 
                char* filter_ts_start, char* filter_ts_end,
                char** filter_components, char** filter_threadname_prefixes, 
                char* filter_severity_vals) {

  FILE* fstr;
  fstr = fopen(filepath, "r");
  filter_lines(fstr, verbose_flag, filter_ts_start, filter_ts_end,
               filter_components, filter_threadname_prefixes, filter_severity_vals);
  fclose(fstr);

  return 0;
}
