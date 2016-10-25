#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "ffc.h"

void seek_start_ts(FILE* fstr, int verbose_flag, const char* filter_ts_start) {
  off_t close_enough = 10 * 1024 * 1024; //the size we stop seeking, just scan all
  off_t fpos = 0;
  
  struct stat st;
  int st_status;
  st_status = fstat(fileno(fstr), &st);
  off_t file_sz = st.st_size;
  off_t jump_sz = file_sz / 2;
  fpos = jump_sz; //start in the middle of the file
  fseek(fstr, fpos, SEEK_SET);

  int c;
  const size_t bufsize = 256 * 1024;
  char buf[bufsize];
  unsigned int line_ctr = 0;
  unsigned int escape_line_no = 50; //how many lines to look through before giving up on finding a timestamped one

  c = getc(fstr);
  if (c == EOF)
    return;

  ungetc(c, fstr);
  c = 0;

  int dt_str_cmp;
  while (jump_sz > close_enough) {
    char* fgres = NULL;
    while (fgres = fgets(buf, bufsize, fstr)) { //looking for first valid timestamped line
      line_ctr++;
      if (line_ctr >= escape_line_no) {
        break;
      }
      if (buf[0] == '2' && buf[1] == '0' && buf[10] == 'T' && buf[19] == '.') { //looks like an ISO datetime at the front
        jump_sz = jump_sz / 2;
        buf[19] = '\0';
        dt_str_cmp = strcmp(buf, filter_ts_start);
        fpos += (dt_str_cmp <= 0) ? jump_sz : -jump_sz;
        break;
      }
    }
    if (fgres == NULL || line_ctr >= escape_line_no) {
      fprintf(stderr, "Error: this file doesn't seem to have any ISO 8069 timestamps\n  (At least not in %u lines of one part)", escape_line_no);
    }
    fseek(fstr, fpos, SEEK_SET);
  }
  if (dt_str_cmp <= 0) { //if the last comparison was the line's ts > filter_ts_start, step back once
    fpos -= jump_sz;
    fseek(fstr, fpos, SEEK_SET);
  }

  //todo? finish seeking to the exact spot.

  return;
}

void
filter_lines(FILE* fstr, int verbose_flag, 
                  const char* filter_ts_start, const char* filter_ts_end,
                  char** filter_components, char** filter_threadname_prefixes, 
                  const char* filter_severity_vals) {
  int c;
  const size_t bufsize = 256 * 1024;
  char buf[bufsize];
  char* ts;
  char* sev;
  char* cmpn;
  char* thrdnm;
  int r; //line should be filtered out
  int i;
  size_t line_ctr = 0;
  char* p;
  int ts_start_check = filter_ts_start ? 1 : 0; //optimization to avoid needless strcmp calls once ts times > filter_ts_start time

  c = getc(fstr);
  if (c == EOF)
    return;

  ungetc(c, fstr);
  c = 0;

  char* fgres = NULL;
  while (fgres = fgets(buf, bufsize, fstr)) {
    line_ctr++;
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
      if (*thrdnm != '[') { //this is not a properly formatted log line
        r = 1;
      } else {
        thrdnm++; // move from the '[' char to one after
        p = thrdnm;
        while (*p != ']' && *p != '\0' && (p - thrdnm) < 32) {
          p++;
        }
        if (*p != ']') { //this is not a properly formatted log line
          r = 1;
        } else {
          *p = '\0';
        }
      }
//printf("FIELDS: %s %s %s %s\n", ts, sev, cmpn, thrdnm);

      if (!r && ts_start_check) {
        if (strcmp(filter_ts_start, ts) >= 0) {
          r = 1;
        } else {
          ts_start_check = 0;
        }
      }

      //todo optimization: once it becomes false exit func
      if (!r && filter_ts_end && strcmp(filter_ts_end, ts) < 0) {
        break;
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
      *(thrdnm + strlen(thrdnm) + 1) = ' '; //put the space back
      *(thrdnm + strlen(thrdnm)) = ']'; //put the ] back

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
  if (filter_ts_start) {
    seek_start_ts(fstr, verbose_flag, filter_ts_start);
  }
  filter_lines(fstr, verbose_flag, filter_ts_start, filter_ts_end,
               filter_components, filter_threadname_prefixes, filter_severity_vals);
  fclose(fstr);

  return 0;
}
