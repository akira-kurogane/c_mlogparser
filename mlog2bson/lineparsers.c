#include "datemanip.h"
#include "lineparsers.h"

//"connection accepted from 10.10.2.211:48114 #1069642 (698 connections now open)"
void lp_connection_accepted_from(bson_t* bson_doc, char* str, char** err_msg_ptr) {
   char from_ip[13];
   char from_port[7];
   long conn_id;
   long curr_incoming_conn_cnt;
   char* strtol_endptr;
   char* p = str + strlen("connection accepted from ");
   char* e;
   e = strchr(p, ':');
   strncpy(from_ip, p, (e - p));
   from_ip[e - p] = '\0';
   p = e + 1;
   e = strchr(p, ' ');
   strncpy(from_port, p, (e - p));
   from_port[e - p] = '\0';
   p = e + 2;
   conn_id = strtol(p, &strtol_endptr, 10);
   p = strtol_endptr + 2; //mv past " ("
   curr_incoming_conn_cnt = strtol(p, &strtol_endptr, 10);
   bson_append_utf8(bson_doc, "ca", 2, from_ip, -1);
   bson_append_utf8(bson_doc, "cap", 3, from_port, -1);
   bson_append_int32(bson_doc, "i", 1, conn_id);
   bson_append_int32(bson_doc, "iccnt", 5, curr_incoming_conn_cnt);
}

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

        if (strncmp(msg, "connection accepted from ", strlen("connection accepted from ")) == 0) {
          lp_connection_accepted_from(bson_doc, msg, err_msg_ptr);
        } else {
          bson_append_utf8(bson_doc, "x", 1, msg, -1);
        }

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

