#include <time.h>

char iso_dt_fmt[18]; //"%Y-%m-%dT%H:%M:%S";
#define iso_fmt_len 19
char min_ts_str[iso_fmt_len + 1];
char max_ts_str[iso_fmt_len + 1];
char common_ts_prefix[iso_fmt_len + 1];
size_t common_ts_prefix_len;
 
void init_datemanip();
void auto_set_min_max_ts_strings();

//from https://gmbabar.wordpress.com/2010/12/01/mktime-slow-use-custom-function/
time_t time_to_epoch (const struct tm *ltm, int utcdiff_hrs, int utcdiff_mins);
