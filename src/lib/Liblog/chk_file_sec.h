#include "license_pbs.h" /* See here for the software license */

#ifdef __CYGWIN__
BOOL load_netapi (HANDLE hNetapi,HANDLE hAdvapi);
void uni2ansi (LPWSTR wcs, char *mbs, int size);
void uni2utf8 (LPWSTR wcs, char *mbs, int size);
int enum_local_users (LPWSTR groupname,char *username);
int enum_domain_users (LPWSTR server_name, LPWSTR groupname,char *username);
int check_local_user_privileges (char *username_utf8, int usertype);
int check_domain_user_privileges (LPWSTR servername, char *username_utf8, int usertype);
int IamRoot();
int IamAdminByName(char *userName);
int IamUser();
int IamUserByName(char *userName);
#else
int IamRoot();
#endif

int chk_file_sec( char *path, int isdir, int sticky, int disallow, int fullpath, char *SEMsg);
