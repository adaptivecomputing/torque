#ifndef _CYGWIN_QUOTA_H
#define _CYGWIN_QUOTA_H 1



#define MAXQUOTAS 2
#define USRQUOTA  0		/* element used for user quotas */
#define GRPQUOTA  1		/* element used for group quotas */
#define INITQFNAMES { "user" /* USRQUOTA */, "group" /* GRPQUOTA */, "undefined", };


#define SUBCMDSHIFT 8
#define SUBCMDMASK  0x00ff
#define QCMD(cmd, type)  (((cmd) << SUBCMDSHIFT) | ((type) & SUBCMDMASK))


# define Q_GETQUOTA 0x800007	/* get user quota structure */


struct dqblk
  {
    u_int64_t dqb_bhardlimit;	/* absolute limit on disk quota blocks alloc */
    u_int64_t dqb_bsoftlimit;	/* preferred limit on disk quota blocks */
    u_int64_t dqb_curspace;	/* current quota block count */
    u_int64_t dqb_ihardlimit;	/* maximum # allocated inodes */
    u_int64_t dqb_isoftlimit;	/* preferred inode limit */
    u_int64_t dqb_curinodes;	/* current # allocated inodes */
    u_int64_t dqb_btime;	/* time limit for excessive disk use */
    u_int64_t dqb_itime;	/* time limit for excessive files */
    u_int32_t dqb_valid;	/* bitmask of QIF_* constants */
  };


/* May be it is required in the future */
int quotactl (int __cmd, const char *__special, int __id,
		     caddr_t __addr)
{
	return(0);
}



#endif /* cygwin/quota.h */

