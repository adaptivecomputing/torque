/*
 * credential.h - header file for default authentication system provided
 * with PBS.
 *
 * Other Requrired Header Files:
 * "portability.h"
 * "libpbs.h"
 *
 */
#ifndef CREDENTIAL_H
#define CREDENTIAL_H
#include "license_pbs.h" /* See here for the software license */

/* the credential as decrypted by the server */

#include <sys/time.h>
#include "pbs_ifl.h" /* PBS_MAXUSER, PBS_MAXHOSTNAME */
typedef struct credential
  {
  time_t timestamp;
  int    type;  /* see BATCH-types.h */
  char   username[PBS_MAXUSER + 1];
  char   hostname[PBS_MAXHOSTNAME + 1];
  } pbs_credential;

/*
 * a full ticket (credential) as passed from the client to the server
 * is of the following size: 8 for the pbs_iff key + 8 for the timestamp +
 * space for user and host name rounded up to multiple of 8 which is the
 * sub-credential size
 */
#define PBS_KEY_SIZE  8
#define PBS_TIMESTAMP_SZ 8
#define PBS_SUBCRED_SIZE ((PBS_MAXUSER + PBS_MAXHOSTNAME + 7) / 8 * 8)
#define PBS_SEALED_SIZE (PBS_SUBCRED_SIZE + PBS_TIMESTAMP_SZ)
#define PBS_TICKET_SIZE (PBS_KEY_SIZE + PBS_SEALED_SIZE)

/*
 * a sealed ticket as produced by the server
 */

typedef struct sealed_ticket
  {
  char timestamp[PBS_TIMESTAMP_SZ];
  char subcred[PBS_SUBCRED_SIZE];
  } pbs_sealed_ticket;


#define CREDENTIAL_LIFETIME  3600
#define CREDENTIAL_TIME_DELTA 300

extern void break_cred(char key[PBS_KEY_SIZE], char *ticket, int size, struct credential *pcred);
extern int  get_credent(char *server, unsigned int port, char **credential);
extern int  make_sealed(char key[PBS_KEY_SIZE], char *subcred, int size, char **ticket);
extern void make_svr_key(char key[PBS_KEY_SIZE]);
extern int  make_subcred(char key[PBS_KEY_SIZE], char *user, char **subcred);

#endif /* CREDENTIAL_H */
