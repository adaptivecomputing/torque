#include "license_pbs.h" /* See here for the software license */

#ifdef __cplusplus
extern "C"
{
#endif
char *pbse_to_txt(int err);

char *pbs_strerror(int err);
#ifdef __cplusplus
}
#endif

const char *reqtype_to_txt(int reqtype);
