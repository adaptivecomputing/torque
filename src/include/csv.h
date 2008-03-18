/* Copyright 2008 Cluster Resources */

/* A small library of functions dealing with comma separated variable lists */

#ifndef _CSV_H_
#define _CSV_H_

extern int csv_length A_(( char *str ));
extern char *csv_nth A_(( char *str, int n ));
/* Note that csv_nth returns a pointer to a static buffer */


#endif


