
/*
 * header file supporting PBS accounting information
 */

#define TOKEN_ACCT_MAX_RCD 131072 /* increased from 4095 */

#define TOKEN_ACCT_POOL   (int)'P' /* Tokens available in the pool */
#define TOKEN_ACCT_RUN    (int)'A' /* Tokens allocated to a job */
#define TOKEN_ACCT_ABT   (int)'U' /* Tokens total usage */

extern int  token_acct_open(char *filename);
extern void token_account_record(int acctype, char *jobid, char *text);
extern void token_acct_close();
