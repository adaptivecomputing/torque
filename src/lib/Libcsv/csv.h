#include "license_pbs.h" /* See here for the software license */

int csv_length(const char *csv_str);

char *csv_nth(const char *csv_str, int n);

char * csv_find_string(const char *csv_str, const char *search_str);

char * csv_find_value(const char *csv_str, const char *search_str);
