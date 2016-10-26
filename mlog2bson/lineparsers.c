#include "datemanip.h"
#include "lineparsers.h"

void parse_line_to_bson(bson_t* bson_doc, char* buf, size_t line_no, char** err_msg_ptr) {

    char* ts;
    char* sev;
    char* cmpn;
    char* thrdnm;
    char* msg; //pointer to message starting after the four fields
    char* p;
    char* strptime_p;
    struct tm tm;


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
        bson_append_timeval(bson_doc, "ts", 2, &tv);
        bson_append_utf8(bson_doc, "s", 1, sev, -1); //should be only char though
        bson_append_utf8(bson_doc, "c", 1, cmpn, -1);
        bson_append_utf8(bson_doc, "p", 1, thrdnm, -1);
        if (conn_id) {
          bson_append_int32(bson_doc, "i", 1, conn_id);
        }

        bson_append_utf8(bson_doc, "x", 1, msg, -1);

      } else {
        bson_append_bool(bson_doc, "not_mongo_log", 13, 1);
        //todo: put all the removed chars terminating first four fields back
        bson_append_utf8(bson_doc, "raw_line", 8, buf, -1);
      }

    } else {
      bson_append_bool(bson_doc, "not_mongo_log", 13, 1);
      bson_append_utf8(bson_doc, "raw_line", 8, buf, -1);
    }
}

void init_lineparsers() {
  init_datemanip();
  auto_set_min_max_ts_strings();
}

