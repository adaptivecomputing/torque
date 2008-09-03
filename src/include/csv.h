/* Copyright 2008 Cluster Resources */

/* A small library of functions dealing with comma separated variable lists */

#ifndef _CSV_H_
#define _CSV_H_

extern int csv_length(char *csv_str);
extern char *csv_nth(char *csv_str, int n);
extern char *csv_find_string(char *csv_str, char *search_str);
extern char *csv_find_value(char *csv_str, char *search_str);
/* Note that csv_nth returns a pointer to a static buffer */


#endif


