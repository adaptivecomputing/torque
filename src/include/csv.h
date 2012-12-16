/* Copyright 2008 Cluster Resources */

/* A small library of functions dealing with comma separated variable lists */

#ifndef _CSV_H_
#define _CSV_H_

extern int csv_length(const char *csv_str);
extern char *csv_nth(const char *csv_str, int n);
extern char *csv_find_string(const char *csv_str, const char *search_str);
extern char *csv_find_value(const char *csv_str, const char *search_str);
/* Note that csv_nth returns a pointer to a static buffer */


#endif


