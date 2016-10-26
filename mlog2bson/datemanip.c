#include <stdlib.h>
#include <string.h>
//following two defines are for strptime
#define _XOPEN_SOURCE
#define __USE_XOPEN
#include "datemanip.h"

//from https://gmbabar.wordpress.com/2010/12/01/mktime-slow-use-custom-function/
//Seems to be much faster than timegm(). mktime() was a lot slower again.
time_t time_to_epoch (const struct tm *ltm, int utcdiff_hrs, int utcdiff_mins) {
   const int mon_days [] =
      {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
   long tyears, tdays, leaps;
   int i;

   tyears = ltm->tm_year - 70; // tm->tm_year is from 1900.
   leaps = (tyears + 2) / 4; // no of next two lines until year 2100.
   //i = (ltm->tm_year - 100) / 100;
   //leaps -= ( (i/4)*3 + i%4 );
   tdays = 0;
   for (i = 0; i < ltm->tm_mon; i++) tdays += mon_days[i];

   tdays += ltm->tm_mday-1; // days of month passed.
   tdays = tdays + (tyears * 365) + leaps;

   return (tdays * 86400) + ((ltm->tm_hour + utcdiff_hrs) * 3600) + ((ltm->tm_min + utcdiff_mins) * 60) + ltm->tm_sec;
}

void init_datemanip() {
  strcpy(iso_dt_fmt, "%Y-%m-%dT%H:%M:%S");
  iso_dt_fmt[17] = '\0';
}

struct timeval str_to_timeval(const char* str, int* err) {
  const char* strptime_p = str;
  struct tm tm;
  struct timeval tv;
  tv.tv_sec = 0;
  tv.tv_usec = 0;
  memset(&tm, 0, sizeof(struct tm));
  strptime_p = strptime(str, iso_dt_fmt, &tm);
  if (strptime_p != str + iso_fmt_len) {
    *err = 1;
    return tv;
  }

  const char *p = str + 20; //the end of the "YYYY-mm-ddTHH:MM:SS" string, one char after where the decimal for the milliseconds is expected to be
  char *endptr;
  tv.tv_usec = 1000 * strtol(p, &endptr, 10);
  p += 4; //now move to the timezone string
  if (*p == 'Z') {
    tv.tv_sec = time_to_epoch(&tm, 0, 0);
  } else {
    int utc_offset;
    utc_offset = (int)strtol(p, &endptr, 10);
    tv.tv_sec = time_to_epoch(&tm, utc_offset / 100, utc_offset % 100);
  }
  return tv;
}

void auto_set_min_max_ts_strings() {
  time_t t;
  struct tm* tms;
  size_t i;
  t = time(NULL);
  t -= 365 * 86440; //go back ~1 year
  tms = gmtime(&t);
  strftime(min_ts_str, iso_fmt_len + 1, iso_dt_fmt, tms);
  t += 365 * 86440 + 86440; //return back, add 1 day into future
  tms = localtime(&t);
  strftime(max_ts_str, iso_fmt_len + 1, iso_dt_fmt, tms);
  i = 0;
  while (i < iso_fmt_len + 1 && min_ts_str[i] == max_ts_str[i]) {
    common_ts_prefix[i] = min_ts_str[i];
	++i;
  }
  common_ts_prefix_len = i;
  common_ts_prefix[common_ts_prefix_len] = '\0';
}

