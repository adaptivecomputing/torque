#include <pbs_config.h>

#include <dirent.h>
#include <stdio.h>
#include <sys/types.h>
#include <ctype.h>
#include <signal.h>
#include <sys/param.h>
#include <fcntl.h>
#include <libgen.h>
#include <hwloc.h>
#include <errno.h>
#include <sys/stat.h>
#ifdef USELIBCPUSET
#  include <bitmask.h>
#  include <cpuset.h>
#endif

#include "libpbs.h"
#include "attribute.h"
#include "resource.h"
#include "server_limits.h"
#include "pbs_job.h"
#include "pbs_nodes.h"
#include "log.h"

/* NOTE: move these three things to utils when lib is checked in */
#ifndef MAXPATHLEN
#define MAXPATHLEN 1024
#endif /* MAXPATHLEN */
#ifndef FAILURE
#define FAILURE 0
#endif /* FAILURE */
#ifndef SUCCESS
#define SUCCESS 1
#endif /* SUCCESS */

extern hwloc_topology_t topology;
extern int              MOMConfigUseSMT;
#ifdef NUMA_SUPPORT
extern nodeboard        node_boards[];
extern int              num_node_boards;
#endif /* NUMA_SUPPORT */
extern int              LOGLEVEL;
extern long             system_ncpus;

/* FIXME: TODO:  TTORQUECPUSET_PATH, enabling cpuset support, and correct error
 * checking need a run-time config */






/*
 * Parse ASCII decimal list representation in buf to hwloc_bitmap map.
 * Returns the number of bits set, may be 0.
 * Returns -1 on failure.
 * NOTES:
 * - There is also bitmask_parselist() in SGI libbitmask.
 * - Maybe future hwloc provides similar functionality,
 *   take care on name conflicts.
 *
 * @param buf - (I) - string
 * @param map - (O) - bitmap
 */

int hwloc_bitmap_parselist(

  const char     *buf, /* I */
  hwloc_bitmap_t  map) /* O */

  {
  unsigned a;
  unsigned b;
  int      c = 0;

  if (!map)
    return(-1);

  hwloc_bitmap_zero(map);

  do
    {
    if (!isdigit(*buf))
      return(-1);

    b = a = strtoul(buf, (char **)&buf, 10);

    if (*buf == '-')
      {
      buf++;
      if (! isdigit(*buf))
        return(-1);
      b = strtoul(buf, (char **)&buf, 10);
      }

    if (a > b)
      return(-1);

    while (a <= b)
      {
      hwloc_bitmap_set(map, a++);
      c++;
      }

    if (*buf == ',')
      buf++;

    } while(*buf != '\0' && *buf != '\n');

  return(c);
  } /* END hwloc_bitmap_parselist() */





/*
 * Display ASCII decimal list representation of hwloc_bitmap map in buf.
 * Returns the number of characters written, excluding trailing '\0'.
 *
 * NOTES:
 * - There is also bitmask_displaylist() in SGI libbitmask.
 * - Maybe future hwloc provides similar functionality,
 *   take care on name conflicts.
 *
 * @param buf    - (O) - string
 * @param buflen - (I) - max chars to become written
 * @param map    - (I) - bitmap
 */

int hwloc_bitmap_displaylist(

  char           *buf,    /* O */
  size_t          buflen, /* I */
  hwloc_bitmap_t  map)    /* I */

  {
  int len = 0;
  int fid;
  int lid;
  int id;

  *buf = '\0';

  fid = id = hwloc_bitmap_first(map);
  while (id != -1)
    {
    lid = id;
    id = hwloc_bitmap_next(map, id);
    if ((id == -1) ||
        (id > lid + 1))
      {
      if (len > 0)
        len += snprintf(buf + len, buflen - len, ",");

      if (fid == lid)
        len += snprintf(buf + len, buflen - len, "%d", fid);
      else if (lid > fid + 1)
        len += snprintf(buf + len, buflen - len, "%d-%d", fid, lid);
      else
        len += snprintf(buf + len, buflen - len, "%d,%d", fid, lid);

      fid = id;
      }
    } /* END while (id != -1) */

  return(len);
  } /* END hwloc_bitmap_displaylist() */





/**
 * Initializes cpuset usage.
 *
 * Returns 0 on success.
 * On failure, -1 is returned, error is logged.
 *
 * NOTES:
 * - The current code looks for the root cpuset and
 *   fails if it does not exist. When using libcpuset,
 *   descriptive error messages are constructed from
 *   errno.
 * - Sophisticated implementations may figure
 *   out the mount point of the cpuset VFS, and may
 *   set base paths that are currently hardcoded in
 *   TROOTCPUSET_PATH and the like.
 */

int init_cpusets(void)

  {
  int           rc   = -1;
#ifdef USELIBCPUSET
  struct cpuset *cp  = NULL;
#else
  char           path[MAXPATHLEN + 1];
  char           cmd[MAXPATHLEN + 1];
  FILE          *pipe;
  struct stat    statbuf;
#endif

#ifdef USELIBCPUSET
  /* Allocate a cpuset */
  if ((cp = cpuset_alloc()) == NULL)
    {
    log_err(errno, __func__, "failed to allocate cpuset");
    return(-1);
    }

  /* Check if cpusets are available by querying the root cpuset */
  if ((rc = cpuset_query(cp, TROOTCPUSET_BASE)) == -1)
    {
    if (errno == ENOSYS)
      log_err(errno, __func__, "failed to query root cpuset, cpusets not supported on this system");
    else if (errno == ENODEV)
      log_err(errno, __func__, "failed to query root cpuset, cpusets not enabled on this system");
    else
      log_err(errno, __func__, "failed to query root cpuset");
    }

  cpuset_free(cp);

  return(rc);
#else /* !USELIBCPUSET */

  /* Check if /dev/cpuset/cpus exists */
  sprintf(path, "%s/cpus", TROOTCPUSET_PATH);
  if ((rc = lstat(path, &statbuf)) == -1)
    {
    /* create cpuset base directory */
    mkdir(TROOTCPUSET_PATH,0755);

    /* now mount it */
    sprintf(cmd,"mount -t cpuset none %s", TROOTCPUSET_PATH);

    pipe = popen(cmd,"r");

    if (pipe == NULL)
      {
      fprintf(stderr,"Cannot mount directory '%s'\n",TROOTCPUSET_PATH);
      }
    else
      {
      /* successfully created/mounted cpusets */
      rc = 0;
      pclose(pipe);
      }
    }

  return(rc);
#endif /* USELIBCPUSET */
  } /* END init_cpusets() */





/**
 * Creates/modifies a cpuset.
 *
 * Returns 0 on success.
 * On failure, -1 is returned, errno is set, and log_buffer is populated.
 *
 * @param name  - (I) - string
 * @param cpus  - (I) - hwloc_bitmap_t
 * @param mems  - (I) - hwloc_bitmap_t
 * @param flags - (I) - mode_t
 *
 * NOTES:
 * - If name starts with /, it is used as is. If not, it is
 *   relative to the TORQUE cpuset.
 * - If cpus and/or mems are NULL, the corresponding cpuset
 *   properties are left empty (create new cpuset),
 *   or untouched (modify existing cpuset).
 * - flags may contain ORed O_CREAT, O_EXCL. The meaning is
 *   similar to open(2):
 *   0:                modify cpuset, fail if it does not exist.
 *   O_CREAT:          create cpuset if it does not exist, otherwise modify.
 *   O_CREAT | O_EXCL: create cpuset, fail if it already exists.
 * - The cpuset is created with mode 0755 and owned by ruid.
 * - When using libcpuset, and create fails, the cpuset does not exist.
 * - When using libcpuset, and modify fails, the cpuset remains unmodified.
 * - When not using libcpuset, and writing cpus or mems fails, the
 *   cpuset path remains in an undefined state.
 */

int create_cpuset(

  const char           *name,  /* I */
  const hwloc_bitmap_t  cpus,  /* I */
  const hwloc_bitmap_t  mems,  /* I */
  mode_t                flags) /* I */

  {
  char            cpuset_path[MAXPATHLEN + 1];
#ifdef USELIBCPUSET
  struct cpuset  *cp   = NULL;
  struct bitmask *mask = NULL;
  int             idx;
#else
  char            path[MAXPATHLEN + 1];
  char            cpuset_buf[MAXPATHLEN];
  struct stat     statbuf;
  FILE           *fd;
#endif

#ifdef USELIBCPUSET

  /* Construct the name of the cpuset.
   * libcpuset does not want the root-cpuset path in it */

  if (name[0] == '/')
    snprintf(cpuset_path, sizeof(cpuset_path), "%s", name);
  else
    snprintf(cpuset_path, sizeof(cpuset_path), "%s/%s", TTORQUECPUSET_BASE, name);

  /* Allocate a cpuset */
  if ((cp = cpuset_alloc()) == NULL)
    {
    sprintf(log_buffer, "(%s) failed to allocate cpuset", __func__);
    return(-1);
    }

  /*
   * Query cpuset.
   * If it exists, fail if O_CREAT | O_EXCL.
   * If it does not exist, fail unless O_CREAT.
   * If query fails for other reasons, fail.
   */
  if (cpuset_query(cp, cpuset_path) == 0)
    {
    if ((flags & (O_CREAT | O_EXCL)) == (O_CREAT | O_EXCL))
      {
      sprintf(log_buffer, "(%s) failed to create cpuset %s", __func__, cpuset_path);
      errno = EEXIST;
      cpuset_free(cp);
      return(-1);
      }

    flags &= ~O_CREAT;
    }
  else if (errno == ENOENT)
    {
    if ((flags & O_CREAT) != O_CREAT)
      {
      sprintf(log_buffer, "(%s) failed to modify cpuset %s", __func__, cpuset_path);
      cpuset_free(cp);
      return(-1);
      }
    }
  else
    {
    sprintf(log_buffer, "(%s) failed to query cpuset %s", __func__, cpuset_path);
    cpuset_free(cp);
    return(-1);
    }

  /* Set cpus */
  if (cpus != NULL)
    {
    if ((mask = bitmask_alloc(cpuset_cpus_nbits())) == NULL)
      {
      sprintf(log_buffer, "(%s) failed to allocate bitmask", __func__);
      cpuset_free(cp);
      return(-1);
      }

    hwloc_bitmap_foreach_begin(idx, cpus)
      bitmask_setbit(mask, idx);
    hwloc_bitmap_foreach_end();

    if (cpuset_setcpus(cp, mask) == -1)
      {
      sprintf(log_buffer, "(%s) failed to set cpus in cpuset %s", __func__, cpuset_path);
      bitmask_free(mask);
      cpuset_free(cp);
      return(-1);
      }

    bitmask_free(mask);
    } /* END cpus != NULL */

  /* Set mems */
  if (mems != NULL)
    {
    if ((mask = bitmask_alloc(cpuset_mems_nbits())) == NULL)
      {
      sprintf(log_buffer, "(%s) failed to allocate bitmask", __func__);
      cpuset_free(cp);
      return(-1);
      }

    hwloc_bitmap_foreach_begin(idx, mems)
      bitmask_setbit(mask, idx);
    hwloc_bitmap_foreach_end();

    if (cpuset_setmems(cp, mask) == -1)
      {
      sprintf(log_buffer, "(%s) failed to set mems in cpuset %s", __func__, cpuset_path);
      bitmask_free(mask);
      cpuset_free(cp);
      return(-1);
      }

    bitmask_free(mask);
    }

  /* Create/modify cpuset */
  if ((flags & O_CREAT) == O_CREAT)
    {
    if (cpuset_create(cpuset_path, cp) == -1)
      {
      sprintf(log_buffer, "(%s) failed to create cpuset %s", __func__, cpuset_path);
      cpuset_free(cp);
      return(-1);
      }

    sprintf(log_buffer, "(%s) successfully created cpuset %s", __func__, cpuset_path);
    }
  else
    {
    if (cpuset_modify(cpuset_path, cp) == -1)
      {
      sprintf(log_buffer, "(%s) failed to modify cpuset %s", __func__, cpuset_path);
      cpuset_free(cp);
      return(-1);
      }
    sprintf(log_buffer, "(%s) successfully modified cpuset %s", __func__, cpuset_path);
    }

  /* Done */
  cpuset_free(cp);

  return(PBSE_NONE);
#else

  /* Construct the name of the cpuset */
  if (name[0] == '/')
    snprintf(cpuset_path, sizeof(cpuset_path), "%s", name);
  else
    snprintf(cpuset_path, sizeof(cpuset_path), "%s/%s", TTORQUECPUSET_PATH, name);

  /*
   * See if cpuset exists.
   * If it exists, fail if O_CREAT | O_EXCL.
   * If it does not exist, fail unless O_CREAT.
   * If query fails for other reasons, fail.
   */

  if (lstat(cpuset_path, &statbuf) == 0)
    {
    if ((flags & (O_CREAT | O_EXCL)) == (O_CREAT | O_EXCL))
      {
      sprintf(log_buffer, "(%s) failed to create cpuset %s", __func__, cpuset_path);
      errno = EEXIST;
      return(-1);
      }

    flags &= ~O_CREAT;
    }
  else if (errno == ENOENT)
    {
    if ((flags & O_CREAT) != O_CREAT)
      {
      sprintf(log_buffer, "(%s) failed to modify cpuset %s", __func__, cpuset_path);
      return(-1);
      }
    }
  else
    {
    sprintf(log_buffer, "(%s) failed to stat cpuset %s", __func__, cpuset_path);
    return(-1);
    }

  /* Create cpuset path, if needed */
  if ((flags & O_CREAT) == O_CREAT)
    {
    if (mkdir(cpuset_path, 0755) == -1)
      {
      sprintf(log_buffer, "(%s) failed to create cpuset %s", __func__, cpuset_path);
      return(-1);
      }
    }

  /* Set cpus */
  if (cpus != NULL)
    {
    sprintf(path, "%s/cpus", cpuset_path);

    if ((fd = fopen(path, "w")) == NULL)
      {
      sprintf(log_buffer, "(%s) failed to open %s", __func__, path);
      return(-1);
      }

    hwloc_bitmap_displaylist(cpuset_buf, sizeof(cpuset_buf), cpus);

    if (fwrite(cpuset_buf, sizeof(char), strlen(cpuset_buf), fd) != strlen(cpuset_buf))
      {
      sprintf(log_buffer, "(%s) failed to write %s", __func__, path);
      fclose(fd);
      return(-1);
      }

    fclose(fd);
    }

  /* Set mems */
  if (mems != NULL)
    {
    sprintf(path, "%s/mems", cpuset_path);

    if ((fd = fopen(path, "w")) == NULL)
      {
      sprintf(log_buffer, "(%s) failed to open %s", __func__, path);
      return(-1);
      }

    hwloc_bitmap_displaylist(cpuset_buf, sizeof(cpuset_buf), mems);

    if (fwrite(cpuset_buf, sizeof(char), strlen(cpuset_buf), fd) != strlen(cpuset_buf))
      {
      sprintf(log_buffer, "(%s) failed to write %s", __func__, path);
      fclose(fd);
      return(-1);
      }

    fclose(fd);
    }

  /* Success */
  if ((flags & O_CREAT) == O_CREAT)
    sprintf(log_buffer, "(%s) successfully created cpuset %s", __func__, cpuset_path);
  else
    sprintf(log_buffer, "(%s) successfully modified cpuset %s", __func__, cpuset_path);

  return(PBSE_NONE);
#endif
  } /* END create_cpuset() */





/**
 * Read cpus and mems of a cpuset into hwloc_bitmap structs.
 *
 * Returns 0 on success.
 * On failure, -1 is returned, errno is set, and log_buffer is populated.
 *
 * @param name - (I) - string
 * @param cpus - (O) - hwloc_bitmap_t
 * @param mems - (O) - hwloc_bitmap_t
 *
 * NOTES:
 * - If name starts with /, it is used as is. If not, it is
 *   relative to the TORQUE cpuset.
 * - If cpus and/or mems are NULL, the corresponding cpuset
 *   properties are not read. This can be used as quick check if a
 *   cpuset exists (rc is -1, errno is ENOENT in this case).
 * - When using libcpuset, and reading of the cpuset fails,
 *   cpus and mems have zero content.
 * - When not using libcpuset, and the cpuset does not exist,
 *   cpus and mems have zero content.
 * - When not using libcpuset, and reading mems fails,
 *   cpus is populated but mems has zero content.
 * - When not using libcpuset, cpus and mems of the cpuset
 *   are assumed to be in ASCII list format.
 */

int read_cpuset(

    const char     *name,  /* I */
    hwloc_bitmap_t  cpus,  /* O */
    hwloc_bitmap_t  mems)  /* O */

  {
  char           cpuset_path[MAXPATHLEN + 1];
  char           cpuset_buf[MAXPATHLEN + 1];
  int            rc    = -1;
#ifdef USELIBCPUSET
  struct cpuset  *cp   = NULL;
  struct bitmask *mask = NULL;
#else
  char            path[MAXPATHLEN + 1];
  struct stat     statbuf;
  FILE           *fd;
#endif

#ifdef USELIBCPUSET

  /* Construct the name of the cpuset.
   * libcpuset does not want the root-cpuset path in it */

  if (name[0] == '/')
    snprintf(cpuset_path, sizeof(cpuset_path), "%s", name);
  else
    snprintf(cpuset_path, sizeof(cpuset_path), "%s/%s", TTORQUECPUSET_BASE, name);

  /* Zero bitmaps */
  if (cpus != NULL)
    hwloc_bitmap_zero(cpus);

  if (mems != NULL)
    hwloc_bitmap_zero(mems);

  /* Allocate a cpuset */
  if ((cp = cpuset_alloc()) == NULL)
    {
    sprintf(log_buffer, "(%s) failed to allocate cpuset", __func__);
    return(-1);
    }

  /* Query cpuset */
  if ((rc = cpuset_query(cp, cpuset_path)) == 0)
    {
    /* Read cpus */
    if (cpus != NULL)
      {
      if ((mask = bitmask_alloc(cpuset_cpus_nbits())) == NULL)
        {
        sprintf(log_buffer, "(%s) failed to allocate bitmask", __func__);
        cpuset_free(cp);
        return(-1);
        }

      if (cpuset_getcpus(cp, mask) == -1)
        {
        sprintf(log_buffer, "(%s) failed to read cpus in cpuset %s", __func__, cpuset_path);
        bitmask_free(mask);
        cpuset_free(cp);
        return(-1);
        }

      if (bitmask_weight(mask) > 0)
        {
        bitmask_displaylist(cpuset_buf, sizeof(cpuset_buf), mask);
        hwloc_bitmap_parselist(cpuset_buf, cpus);
        }

      bitmask_free(mask);
      }

    /* Read mems */
    if (mems != NULL)
      {
      if ((mask = bitmask_alloc(cpuset_mems_nbits())) == NULL)
        {
        sprintf(log_buffer, "(%s) failed to allocate bitmask", __func__);
        cpuset_free(cp);
        return(-1);
        }

      if (cpuset_getmems(cp, mask) == -1)
        {
        sprintf(log_buffer, "(%s) failed to read mems in cpuset %s", __func__, cpuset_path);
        bitmask_free(mask);
        cpuset_free(cp);
        return(-1);
        }

      if (bitmask_weight(mask) > 0)
        {
        bitmask_displaylist(cpuset_buf, sizeof(cpuset_buf), mask);
        hwloc_bitmap_parselist(cpuset_buf, mems);
        }

      bitmask_free(mask);
      }
    }

  /* Done */
  cpuset_free(cp);

  return(rc);
#else

  /* Construct the name of the cpuset */
  if (name[0] == '/')
    snprintf(cpuset_path, sizeof(cpuset_path), "%s", name);
  else
    snprintf(cpuset_path, sizeof(cpuset_path), "%s/%s", TTORQUECPUSET_PATH, name);

  /* Zero bitmaps */
  if (cpus != NULL)
    hwloc_bitmap_zero(cpus);

  if (mems != NULL)
    hwloc_bitmap_zero(mems);

  /* Stat cpuset */
  if ((rc = lstat(cpuset_path, &statbuf)) == 0)
    {
    /* Read cpus */
    if (cpus != NULL)
      {
      sprintf(path, "%s/cpus", cpuset_path);

      if ((fd = fopen(path, "r")) == NULL)
        {
        sprintf(log_buffer, "(%s) failed to open %s", __func__, path);
        return(-1);
        }

      if (fscanf(fd, "%s", cpuset_buf) == 1)
        {
        if (hwloc_bitmap_parselist(cpuset_buf, cpus) < 0)
          {
          sprintf(log_buffer, "(%s) failed to parse %s: %s", __func__, path, cpuset_buf);
          fclose(fd);
          errno = EINVAL;
          return(-1);
          }
        }

      fclose(fd);
      }

    /* Read mems */
    if (mems != NULL)
      {
      sprintf(path, "%s/mems", cpuset_path);

      if ((fd = fopen(path, "r")) == NULL)
        {
        sprintf(log_buffer, "(%s) failed to open %s", __func__, path);
        return(-1);
        }

      if (fscanf(fd, "%s", cpuset_buf) == 1)
        {
        if (hwloc_bitmap_parselist(cpuset_buf, mems) < 0)
          {
          sprintf(log_buffer, "(%s) failed to parse %s: %s", __func__, path, cpuset_buf);
          fclose(fd);
          errno = EINVAL;
          return(-1);
          }
        }

      fclose(fd);
      }

    }
  else
    {
    sprintf(log_buffer, "(%s) failed to stat %s", __func__, cpuset_path);
    }

  /* Done */
  return(rc);
#endif
  } /* END read_cpuset() */





/**
 * Deletes a cpuset.
 *
 * Returns 0 on success.
 * On failure, -1 is returned.
 *
 * @param name  - (I) - string
 *
 * NOTES:
 * - If name starts with /, it is used as is. If not, it is
 *   relative to the TORQUE cpuset.
 * - Attempts to delete a non-existing cpuset return -1.
 * - Deleting a cpuset, includes killing tasks in it,
 *   and deleting any descendent cpusets and killing their tasks.
 * - If there are no tasks to kill, the function returns quickly.
 * - If there are tasks to kill, SIGKILL is sent to each of them,
 *   followed by a short sleep. If there still remain tasks,
 *   the procedure is repeated.
 * - When using libcpuset, this is tried max. 5 seconds for
 *   the cpuset including its children.
 * - When not using libcpuset, this is tried max. 5 seconds
 *   for each individual cpuset to delete.
 * - When there still remain tasks, deleting fails with
 *   some errno.
 */

int delete_cpuset(

    const char *name)  /* I */

  {
  char           cpuset_path[MAXPATHLEN + 1];
#ifndef USELIBCPUSET
  char           path[MAXPATHLEN + 1];
  char           tid[1024];
  struct dirent *pdirent;
  struct stat    statbuf;
  int            npids;
  int            slept;
  FILE          *fd;
  DIR           *dir;
#endif

#ifdef USELIBCPUSET

  /* Construct the name of the cpuset.
   * libcpuset does not want the root-cpuset path in it */
  if (name[0] == '/')
    snprintf(cpuset_path, sizeof(cpuset_path), "%s", name);
  else
    snprintf(cpuset_path, sizeof(cpuset_path), "%s/%s", TTORQUECPUSET_BASE, name);

  /* Nuke the cpuset and all its child cpusets */
  if (cpuset_nuke(cpuset_path, 5) == 0)
    {
    /* Success */
    if (LOGLEVEL >= 6)
      {
      sprintf(log_buffer, "successfully nuked cpuset %s", cpuset_path);
      log_event(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, __func__, log_buffer);
      }

    return(PBSE_NONE);
    }

  /* Failure */
  if (errno != ENOENT)
    {
    sprintf(log_buffer, "failed to nuke cpuset %s", cpuset_path);
    log_err(errno, __func__, log_buffer);
    }

  return(-1);
#else

  /* Construct the name of the cpuset */
  if (name[0] == '/')
    snprintf(cpuset_path, sizeof(cpuset_path), "%s", name);
  else
    snprintf(cpuset_path, sizeof(cpuset_path), "%s/%s", TTORQUECPUSET_PATH, name);

  if ((dir = opendir(cpuset_path)) != NULL)
    {
    while ((pdirent = readdir(dir)) != NULL)
      {
      /* Skip parent and current directory. */
      if ((!strcmp(pdirent->d_name, ".")) ||
          (!strcmp(pdirent->d_name, "..")))
        continue;

      /* Prepend directory name to entry name for lstat. */
      snprintf(path, sizeof(path), "%s/%s", cpuset_path, pdirent->d_name);

      /* Skip entry, if lstat fails. */
      if (lstat(path, &statbuf) == -1)
        continue;

      /* If a directory is found, it is a child cpuset. Try to delete it. */
      if ((statbuf.st_mode & S_IFDIR) == S_IFDIR)
        {
        delete_cpuset(path);
        }

      /*
       * If there are running processes, try to kill them.
       * If this takes more than 5 seconds, give up.
       */
      else if (!strcmp(pdirent->d_name, "tasks"))
        {
        do
          {
          npids = 0;
          slept = 0;
          if ((fd = fopen(path, "r")) != NULL)
            {
            while ((fgets(tid, sizeof(tid), fd)) != NULL)
              {
              kill(atoi(tid), SIGKILL);
              npids++;
              }

            fclose(fd);
            }

          if (npids)
            {
            sleep(1);
            slept++;
            }
          } while ((npids > 0) && (slept <= 5));
        }
      } /* END while((pdirent = readdir(dir)) != NULL) */

    closedir(dir);
    } /* END if (opendir) */

  if (rmdir(cpuset_path) == 0)
    {
    /* Success */
    if (LOGLEVEL >= 6)
      {
      sprintf(log_buffer, "successfully deleted cpuset %s", cpuset_path);
      log_event(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, __func__, log_buffer);
      }

    return(PBSE_NONE);
    }

  /* Failure */
  if (errno != ENOENT)
    {
    sprintf(log_buffer, "failed to delete cpuset %s", cpuset_path);
    log_err(errno, __func__, log_buffer);
    }

  return(-1);
#endif
  } /* END delete_cpuset() */





/**
 * Cleanup TORQUE cpuset from cpusets of jobs that are gone.
 *
 * Called after init_abort_jobs.
 */

void cleanup_torque_cpuset(void)

  {
  char           path[MAXPATHLEN + 1];
  struct dirent *pdirent;
  struct stat    statbuf;
  DIR           *dir;

  if ((dir = opendir(TTORQUECPUSET_PATH)) == NULL)
    {
    log_err(errno, __func__, "failed to open TORQUE cpuset hierarchy");
    return;
    }

  while ((pdirent = readdir(dir)) != NULL)
    {
    /* Skip parent and current directory. */
    if ((!strcmp(pdirent->d_name, ".")) ||
        (!strcmp(pdirent->d_name, "..")))
      continue;

    /* Prepend directory name to entry name for lstat. */
    snprintf(path, sizeof(path), "%s/%s", TTORQUECPUSET_PATH, pdirent->d_name);

    /* Skip entry, if lstat fails. */
    if (lstat(path, &statbuf) == -1)
      continue;

    /* If a directory is found, it is a cpuset. */
    if ((statbuf.st_mode & S_IFDIR) == S_IFDIR)
      {
      /*
       * Check if entry name corresponds to a known job.
       * If not, delete the cpuset.
       */

      if (mom_find_job(pdirent->d_name) == NULL)
        {
        if (LOGLEVEL >= 6)
          {
          sprintf(log_buffer, "about to delete orphaned cpuset %s", path);
          log_event(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, __func__, log_buffer);
          }

        if (delete_cpuset(pdirent->d_name) == 0)
          {
          sprintf(log_buffer, "deleted orphaned cpuset %s", path);
          log_event(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, __func__, log_buffer);
          }

        }
      else
        {
        sprintf(log_buffer, "found active cpuset %s", path);
        log_event(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, __func__, log_buffer);
        }
      }
    }  /* END while((pdirent = readdir(dir)) != NULL) */

  closedir(dir);
  } /* END cleanup_torque_cpuset() */






/**
 * Initializes the TORQUE cpuset.
 *
 * Returns 0 on success.
 * Returns -1 on failure.
 *
 * NOTES:
 * - This function checks if cpusets are supported and available.
 *   So call it before any other cpuset-related things, and abort
 *   pbs_mom if it fails.
 * - This function sets up cpus and mems, only. Other cpuset properties
 *   are not initialized/touched.
 * - With NUMA_SUPPORT, the TORQUE cpuset will contain the ORed
 *   cpusets and nodesets of all nodeboards, as defined in mom.layout.
 *   Initialization will fail, if mom.layout has wrong content
 *   (e.g. defines nodeboards that overlap with a boot cpuset).
 * - With NUMA_SUPPORT, the TOQRUE cpuset cpus and mems are sync'ed
 *   with mom.layout at every call of this function, even when the
 *   TORQUE cpuset already exists.
 * - Without NUMA_SUPPORT, when the TORQUE cpuset does not exist,
 *   it becomes initialized with all cpus and mems of the system,
 *   excluding a possibly existing boot cpuset, and excluding
 *   logical CPUs if $use_smt is off.
 * - Without NUMA_SUPPORT, when the TORQUE cpuset already exists,
 *   it is left untouched. It is not checked if it is in sync
 *   with the current setting of $use_smt (if it contains unwanted
 *   cpus when $use_smt is off, or if one may add additional
 *   cpus when $use_smt is on).
 */

int init_torque_cpuset(void)

  {
  hwloc_bitmap_t cpus = NULL;
  hwloc_bitmap_t mems = NULL;
  int            rc   = -1;
  int            i;
#ifndef NUMA_SUPPORT
  hwloc_obj_t    obj;
  hwloc_obj_t    pu;
  hwloc_bitmap_t bootcpus = NULL;
  hwloc_bitmap_t bootmems = NULL;
#endif

#ifdef USELIBCPUSET
  sprintf(log_buffer, "Init cpuset %s", TTORQUECPUSET_BASE);
#else
  sprintf(log_buffer, "Init cpuset %s", TTORQUECPUSET_PATH);
#endif

  log_event(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, __func__, log_buffer);

  /* Check if cpusets are supported, return error if not */
  if ((init_cpusets()) == -1)
    return(-1);

  /* Allocate bitmaps */
  if (((cpus = hwloc_bitmap_alloc()) == NULL) ||
      ((mems = hwloc_bitmap_alloc()) == NULL))
    {
    log_err(ENOMEM, __func__, "failed to allocate bitmap");
    return(-1);
    }

#ifdef NUMA_SUPPORT
  /* Add cpus and mems of all nodeboards */
  for (i = 0; i < num_node_boards; i++)
    {
    hwloc_bitmap_or(cpus, cpus, node_boards[i].cpuset);
    hwloc_bitmap_or(mems, mems, node_boards[i].nodeset);
    }
#else

  /*
   * See if cpuset exists.
   * If it's already there, leave as is, set up otherwise.
   */
#ifdef USELIBCPUSET
  if (read_cpuset(TTORQUECPUSET_BASE, cpus, mems) == -1)
#else
    if (read_cpuset(TTORQUECPUSET_PATH, cpus, mems) == -1)
#endif
      {
      if (errno != ENOENT)
        {
        /* Error */
        log_err(errno, __func__, log_buffer);
        goto finish;
        }
      }
    else
      {
      /* Exists, tell what we have and return */
      sprintf(log_buffer, "cpus = ");
      hwloc_bitmap_displaylist(log_buffer + strlen(log_buffer), sizeof(log_buffer) - strlen(log_buffer), cpus);
      log_event(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, __func__, log_buffer);

      sprintf(log_buffer, "mems = ");
      hwloc_bitmap_displaylist(log_buffer + strlen(log_buffer), sizeof(log_buffer) - strlen(log_buffer), mems);
      log_event(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, __func__, log_buffer);

      rc = 0;
      goto finish;
      }

  /* Add all resources of the root cpuset */
#ifdef USELIBCPUSET
  if (read_cpuset(TROOTCPUSET_BASE, cpus, mems) == -1)
#else
    if (read_cpuset(TROOTCPUSET_PATH, cpus, mems) == -1)
#endif
      {
      log_err(errno, __func__, log_buffer);
      goto finish;
      }

  /*
   * Handle SMT CPUs.
   * If a system has SMT enabled, there are more than one logical CPU per physical core.
   * If MOMConfigUseSMT is off, we only want the first logical CPU of a core in the cpuset.
   * Thus we map the additional logical CPUs out of the cpuset.
   * To be portable among architectures as much as possible, the only assumption that
   * is made here is that the CPUs to become mapped out are HWLOC_OBJ_PU objects that
   * are children of a HWLOC_OBJ_CORE object.
   * If there are no HWLOC_OBJ_CORE objects in the cpuset, we cannot detect if cpuset members
   * are physical or logical. Then the cpuset is left as-is.
   */

  if (!MOMConfigUseSMT)
    {
    for (obj = hwloc_get_next_obj_inside_cpuset_by_type(topology, cpus, HWLOC_OBJ_CORE, NULL);
        obj;
        obj = hwloc_get_next_obj_inside_cpuset_by_type(topology, cpus, HWLOC_OBJ_CORE, obj))
      {
      i = 1;
      while ((pu = hwloc_get_obj_inside_cpuset_by_type(topology, obj->cpuset, HWLOC_OBJ_PU, i++)) != NULL)
        hwloc_bitmap_andnot(cpus, cpus, pu->cpuset);
      }
    }

  /* Allocate bitmaps before querying boot cpuset */
  if ((bootcpus = hwloc_bitmap_alloc()) == NULL)
    {
    log_err(errno, __func__, "failed to allocate bitmap");
    goto finish;
    }
  if ((bootmems = hwloc_bitmap_alloc()) == NULL)
    {
    log_err(errno, __func__, "failed to allocate bitmap");
    goto finish;
    }

  /*
   * Query boot cpuset.
   * If it is there, subtract its cpus and mems.
   */

#ifdef USELIBCPUSET
  if (read_cpuset(TBOOTCPUSET_BASE, bootcpus, bootmems) == -1)
#else
    if (read_cpuset(TBOOTCPUSET_PATH, bootcpus, bootmems) == -1)
#endif
      {
      if (errno != ENOENT)
        {
        /* Error */
        log_err(errno, __func__, log_buffer);
        goto finish;
        }
      }
    else
      {
      sprintf(log_buffer, "subtracting cpus of boot cpuset: ");
      hwloc_bitmap_displaylist(log_buffer + strlen(log_buffer), sizeof(log_buffer) - strlen(log_buffer), bootcpus);
      log_event(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, __func__, log_buffer);

      sprintf(log_buffer, "subtracting mems of boot cpuset: ");
      hwloc_bitmap_displaylist(log_buffer + strlen(log_buffer), sizeof(log_buffer) - strlen(log_buffer), bootmems);
      log_event(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, __func__, log_buffer);

      hwloc_bitmap_andnot(cpus, cpus, bootcpus);
      hwloc_bitmap_andnot(mems, mems, bootmems);
      }

#endif

  sprintf(log_buffer, "setting cpus = ");
  hwloc_bitmap_displaylist(log_buffer + strlen(log_buffer), sizeof(log_buffer) - strlen(log_buffer), cpus);
  log_event(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, __func__, log_buffer);

  sprintf(log_buffer, "setting mems = ");
  hwloc_bitmap_displaylist(log_buffer + strlen(log_buffer), sizeof(log_buffer) - strlen(log_buffer), mems);
  log_event(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, __func__, log_buffer);

#ifdef USELIBCPUSET
  if ((rc = create_cpuset(TTORQUECPUSET_BASE, cpus, mems, O_CREAT)) == -1)
#else
  if ((rc = create_cpuset(TTORQUECPUSET_PATH, cpus, mems, O_CREAT)) == -1)
#endif
    log_err(errno, __func__, log_buffer);

#ifndef NUMA_SUPPORT
finish:
#endif

  if (cpus != NULL)
    hwloc_bitmap_free(cpus);
  if (mems != NULL)
    hwloc_bitmap_free(mems);
#ifndef NUMA_SUPPORT
  if (bootcpus != NULL)
    hwloc_bitmap_free(bootcpus);
  if (bootmems != NULL)
    hwloc_bitmap_free(bootmems);
#endif

  return(rc);
  } /* END init_torque_cpuset */





/**
 * Add cpuset of object at idx in a cpuset to another cpuset.
 *
 * Returns 0 (success) or -1 (failure)
 *
 * @param cpuset  - (I)   - cpuset to search for object
 * @param cpus    - (I/O) - cpuset to add found object
 * @param idx     - (I)   - position in cpuset
 *
 * NOTES:
 * - idx specifies the position of a bit in cpuset (1st bit set is idx = 0).
 * - cpuset is searched for HWLOC_OBJ_CORE and HWLOC_OBJ_PU objects.
 * - The target cpuset cpus is ORed with the cpuset of a found object.
 */

int add_obj_from_cpuset(

  hwloc_bitmap_t cpuset, /* I   */
  hwloc_bitmap_t cpus,   /* I/O */
  unsigned       idx)    /* I   */

  {
  hwloc_obj_t obj = NULL;

  /* Figure out core or pu at position idx in cpuset */
  if ((obj = hwloc_get_obj_inside_cpuset_by_type(topology, cpuset, HWLOC_OBJ_PU, idx)) == NULL)
    if ((obj = hwloc_get_obj_inside_cpuset_by_type(topology, cpuset, HWLOC_OBJ_CORE, idx)) == NULL)
      return(-1);

  /* Add cpuset of found object */
  hwloc_bitmap_or(cpus, cpus, obj->cpuset);
  return(PBSE_NONE);
  } /* END add_obj_from_cpuset() */





/**
 * Creates cpuset for a job.
 *
 * Returns SUCCESS/FAILURE.
 *
 * @param pjob - (I) - job
 *
 * NOTES:
 * - The cpuset is named like the job ID.
 * - The cpuset is created below the TORQUE cpuset.
 * - If a cpuset with the same name already exists, it is
 *   tried to become deleted. If delete fails, create fails.
 * - With NUMA support, the cpuset is constructed by ORing
 *   subsets of the cpus and mems of the nodeboards that are
 *   allocated for the job.
 * - Without NUMA support, the cpuset will contain
 *   a subset of the cpus of the TORQUE cpuset, and all
 *   mems of the TORQUE cpuset.
 */

int create_job_cpuset(

  job *pjob) /* I */

  {
  vnodent        *np = pjob->ji_vnods;
  hwloc_bitmap_t  cpus = NULL;
  hwloc_bitmap_t  mems = NULL;
  int             rc   = FAILURE;
  int             j;
#ifdef NUMA_SUPPORT
  int             numa_idx;
#else
  hwloc_bitmap_t  tcpus = NULL;
#  ifdef GEOMETRY_REQUESTS
  resource       *presc = NULL;
  resource_def   *prd   = NULL;
  hwloc_obj_t     obj   = NULL;
  hwloc_obj_t     core  = NULL;
#  endif
#endif

  /* Delete cpuset, if it exists */
  delete_cpuset(pjob->ji_qs.ji_jobid);

  /* Allocate bitmaps for cpus and mems */
  if (((cpus = hwloc_bitmap_alloc()) == NULL) ||
      ((mems = hwloc_bitmap_alloc()) == NULL))
    {
    log_err(errno, __func__, "failed to allocate bitmap");
    goto finish;
    }

#ifdef NUMA_SUPPORT
  /* Walk through job's vnodes, add corresponding cpus */
  for (j = 0; j < pjob->ji_numvnod; ++j, np++)
    {
    /* Figure out numa_node for this vnode */
    char *dash = strchr(np->vn_host->hn_host, '-');

    if (dash)
      {
      while (strchr(dash + 1, '-'))
        dash = strchr(dash + 1, '-');

      numa_idx = atoi(dash + 1);
      }
    else
      {
      sprintf(log_buffer, "failed to parse node number from nodeboard name %s", np->vn_host->hn_host);
      log_err(-1, __func__, log_buffer);
      continue;
      }

    if ((pjob->ji_wattr[JOB_ATR_node_exclusive].at_flags & ATR_VFLAG_SET) &&
        (pjob->ji_wattr[JOB_ATR_node_exclusive].at_val.at_long != 0))
      {
      /* If job's node_usage is singlejob, simply add all cpus/mems of this vnode */
      hwloc_bitmap_or(cpus, cpus, node_boards[numa_idx].cpuset);
      hwloc_bitmap_or(mems, mems, node_boards[numa_idx].nodeset);
      }
    else
      {
      /* Add core at position vn_index in nodeboard cpuset */
      if (add_obj_from_cpuset(node_boards[numa_idx].cpuset, cpus, np->vn_index) == -1)
        {
        sprintf(log_buffer, "nodeboard %s cpuset contains no CPU at index %d", np->vn_host->hn_host, np->vn_index);
        log_err(-1, __func__, log_buffer);
        }

      /* Set mems to all memory nodes covered by cpus */
      hwloc_cpuset_to_nodeset_strict(topology, cpus, mems);
      }
    } /* END for(j) */

#else /* ndef NUMA_SUPPORT follows */
  /* Allocate bitmap for cpus of TORQUE cpuset */
  if ((tcpus = hwloc_bitmap_alloc()) == NULL)
    {
    log_err(errno, __func__, "failed to allocate bitmap");
    goto finish;
    }

  /* Read TORQUE cpuset */

#ifdef USELIBCPUSET
  if (read_cpuset(TTORQUECPUSET_BASE, tcpus, mems) == -1)
#else
    if (read_cpuset(TTORQUECPUSET_PATH, tcpus, mems) == -1)
#endif
      {
      /* Error */
      log_err(errno, __func__, log_buffer);
      goto finish;
      }

#ifdef GEOMETRY_REQUESTS
  /* Check if job requested procs_bitmap */
  prd   = find_resc_def(svr_resc_def,"procs_bitmap",svr_resc_size);
  presc = find_resc_entry(&pjob->ji_wattr[JOB_ATR_resource],prd);

  /* If so, walk through job's vnodes, add corresponding cpus */
  if ((presc != NULL) && (presc->rs_value.at_flags & ATR_VFLAG_SET) == TRUE)
    {
    for (j = 0; j < pjob->ji_numvnod; ++j, np++)
      {
      /* Figure out cpu with os_index vn_index */
      if ((obj = hwloc_get_pu_obj_by_os_index(topology, np->vn_index)) == NULL)
        {
        sprintf(log_buffer, "topology contains no CPU at os-index %d", np->vn_index);
        log_err(-1, __func__, log_buffer);
        continue;
        }

      /* Check if this cpu is part of the TORQUE cpuset */
      if (!hwloc_bitmap_isincluded(obj->cpuset, tcpus))
        {
        sprintf(log_buffer, "TORQUE cpuset contains no CPU at os-index %d", np->vn_index);
        log_err(-1, __func__, log_buffer);
        continue;
        }

      /* If $use_smt is on, look for parent core */
      if (MOMConfigUseSMT)
        if ((core = hwloc_get_ancestor_obj_by_type(topology, HWLOC_OBJ_CORE, obj)) != NULL)
          if (hwloc_bitmap_isincluded(core->cpuset, tcpus))
            obj = core;

      /* Add cpuset of found object */
      hwloc_bitmap_or(cpus, cpus, obj->cpuset);
      } /* END for(j) */
    }
  else
#endif /* GEOMETRY REQUESTS */
    {
    if ((pjob->ji_wattr[JOB_ATR_node_exclusive].at_flags & ATR_VFLAG_SET) &&
        (pjob->ji_wattr[JOB_ATR_node_exclusive].at_val.at_long != 0))
      /* If job's node_usage is singlejob, simply add all cpus */
      {
      hwloc_bitmap_or(cpus, cpus, tcpus);
      }
    else
      {
      /* Walk through job's vnodes, add corresponding cpus */
      for (j = 0; j < pjob->ji_numvnod; ++j, np++)
        {
        /* Add core at position vn_index in TORQUE cpuset */
        if (pjob->ji_nodeid == np->vn_host->hn_node)
          {
          if (add_obj_from_cpuset(tcpus, cpus, np->vn_index) == -1)
            {
            sprintf(log_buffer, "TORQUE cpuset contains no CPU at index %d", np->vn_index);
            log_err(-1, __func__, log_buffer);
            }
          }
        } /* END for(j) */

      }
    }

  /* give this job the mems that these cpus cover */
  hwloc_cpuset_to_nodeset_strict(topology, cpus, mems);

#endif /* NUMA_SUPPORT (first section def, second sectoin ndef */

  /* Now create cpuset for job */
  snprintf(log_buffer, sizeof(log_buffer),
      "creating cpuset for job %s: %d cpus (",
      pjob->ji_qs.ji_jobid,
      hwloc_bitmap_weight(cpus));

  hwloc_bitmap_displaylist(log_buffer + strlen(log_buffer),
      sizeof(log_buffer) - strlen(log_buffer),
      cpus);

  snprintf(log_buffer + strlen(log_buffer), sizeof(log_buffer) - strlen(log_buffer),
      "), %d mems (",
      hwloc_bitmap_weight(mems));

  hwloc_bitmap_displaylist(log_buffer + strlen(log_buffer),
      sizeof(log_buffer) - strlen(log_buffer),
      mems);

  snprintf(log_buffer + strlen(log_buffer), sizeof(log_buffer) - strlen(log_buffer), ")");
  log_ext(-1, __func__, log_buffer, LOG_INFO);

  if (create_cpuset(pjob->ji_qs.ji_jobid, cpus, mems, O_CREAT) == 0)
    {
    /* Success */
    if (LOGLEVEL >= 4)
      log_ext(-1, __func__, log_buffer, LOG_DEBUG);

    rc = SUCCESS;
    }
  else
    {
    /* Failure */
    log_err(errno, __func__, log_buffer);
    rc = FAILURE;
    }

finish:

  if (cpus != NULL)
    hwloc_bitmap_free(cpus);
  if (mems != NULL)
    hwloc_bitmap_free(mems);
#ifndef NUMA_SUPPORT
  if (tcpus != NULL)
    hwloc_bitmap_free(tcpus);
#endif

  return(rc);
  } /* END create_job_cpuset() */






/**
 * Bind a process id to the cpuset of a job.
 *
 * Returns SUCCESS on success.
 * Returns FAILURE on failure.
 *
 * @param pid  - (I) - pid
 * @param pjob - (I) - job
 *
 * NOTES:
 * - If pid is zero, the current process is bound to the cpuset.
 * - If binding fails, the process stays in the cpuset where
 *   it was created (where pbs_mom runs). In usual cases,
 *   this is the root cpuset of the system.
 */

int move_to_job_cpuset(

    pid_t  pid,   /* I */
    job   *pjob)  /* I */

  {
  char          cpuset_path[MAXPATHLEN + 1];
#ifndef USELIBCPUSET
  char          path[MAXPATHLEN + 1];
  char          cpuset_buf[MAXPATHLEN];
  FILE         *fd;
#endif

#ifdef USELIBCPUSET

  /* Construct the name of the cpuset.
   * libcpuset does not want the root-cpuset path in it */
  sprintf(cpuset_path, "%s/%s", TTORQUECPUSET_BASE, pjob->ji_qs.ji_jobid);

  if (cpuset_migrate(pid, cpuset_path) == 0)
    {
    /* Success */
    if (LOGLEVEL >= 4)
      {
      sprintf(log_buffer, "successfully moved pid %d to cpuset %s", pid, cpuset_path);
      log_ext(-1, __func__, log_buffer, LOG_DEBUG);
      }
    return(SUCCESS);
    }

  /* Failure */
  sprintf(log_buffer, "failed to move pid %d to cpuset %s", pid, cpuset_path);
  log_err(errno, __func__, log_buffer);
  return(FAILURE);

#else

  /* Construct the name of the cpuset */
  sprintf(cpuset_path, "%s/%s", TTORQUECPUSET_PATH, pjob->ji_qs.ji_jobid);

  /* If pid is 0, set it to current pid */
  if (pid == 0)
    pid = getpid();

  /* Write pid to tasks file */
  sprintf(path, "%s/tasks", cpuset_path);
  if ((fd = fopen(path, "w")) == NULL)
    {
    /* Failure */
    sprintf(log_buffer, "failed to move pid %d to cpuset %s", pid, cpuset_path);
    log_err(errno, __func__, log_buffer);
    return(FAILURE);
    }

  sprintf(cpuset_buf, "%d", pid);

  if (fwrite(cpuset_buf, sizeof(char), strlen(cpuset_buf), fd) != strlen(cpuset_buf))
    {
    sprintf(log_buffer, "failed to move pid %d to cpuset %s", pid, cpuset_path);
    log_err(errno, __func__, log_buffer);
    fclose(fd);
    return(FAILURE);
    }

  /* Success */
  fclose(fd);

  if (LOGLEVEL >= 4)
    {
    sprintf(log_buffer, "successfully moved pid %d to cpuset %s", pid, cpuset_path);
    log_ext(-1, __func__, log_buffer, LOG_DEBUG);
    }

  return(SUCCESS);
#endif
  }  /* END move_to_job_cpuset() */



int get_cpuset_size(char *cpusetStr)
  {
  int val1 = -1;
  int val2 = -2;
  int j;
  int range = FALSE;
  int len = 0;
  char *ptr;

  val1 = atoi(cpusetStr);
  range = TRUE;

  len = strlen(cpusetStr);
  for (j=0; j<len; j++)
    {
    if (cpusetStr[j] == '-')
      {
      range = TRUE;
      ptr = cpusetStr;
      ptr += j + 1;
      val2 = atoi(ptr);
      break;
      }
    }
  
  if (val2 != -1)
    return(val2+1);
  else
    return(val1);
  
  }


/**
 * adjust_root_map
 * @see remove_boot_set() - parent
 *
 * @param cpusetStr - (I) the cpuset string
 * @param cpusetMap - (I/O) the cpuset map
 * @param add - (I) True to add cpuset to map else we remove cpuset from map
 */

void adjust_root_map(

    char *cpusetStr, /* I */
    int   cpusetMap[], /* I/O */
    int   add)       /* I */

  {
  int   val1 = -1;
  int   val2 = -1;
  int   i;
  int   j;
  int   len;
  int   range;
  char *ptr;

  val1 = atoi(cpusetStr);
  range = FALSE;
  len = strlen(cpusetStr);

  for (j=0; j<len; j++)
    {
    if (cpusetStr[j] == '-')
      {
      range = TRUE;
      ptr = cpusetStr;
      ptr += j + 1;
      val2 = atoi(ptr);
      }
    else if (cpusetStr[j] == ',')
      {
      if (val2 > -1)
        {
        for (i=val1; i<=val2; i++)
          {
          cpusetMap[i] = add? 1: 0;
          }
        range = FALSE;
        }
      else
        {
        cpusetMap[val1] = add? 1: 0;
        }
      ptr = cpusetStr;
      ptr += j + 1;
      val1 = atoi(ptr);      val2 = -1;
      }
    }

  if (val2 > -1)
    {
    for (i=val1; i<=val2; i++)
      {
      cpusetMap[i] = add? 1: 0;
      }
    }
  else
    {
    cpusetMap[val1] = add? 1: 0;
    }

  return;
  } /* END adjust_root_map() */





/**
 * remove_boot_set
 * @see initialize_root_cpuset() - parent
 *
 * @param rootStr - (I/O) the root cpuset string
 * @param bootStr - (I) the boot cpuset string
 */

void remove_boot_set(

  char *rootStr, /* I/O */
  char *bootStr) /* I */

  {
  int          j;
  int          first;
  int          *cpusetMap;
  int          cpuset_size;
  char         tmpBuf[MAXPATHLEN];

  if ((rootStr == NULL) ||
      (bootStr == NULL))
    return;

  cpuset_size = get_cpuset_size(rootStr);
  if (cpuset_size <= 0)
    {
    return;
    }

    cpusetMap = (int *)calloc(1, cpuset_size + 1);
    if(cpusetMap == NULL)
      return;

  if (LOGLEVEL >= 7)
    {
    sprintf(log_buffer,
        "removing boot cpuset (%s) from root cpuset (%s)",
        bootStr, rootStr);
    log_ext(-1, __func__, log_buffer, LOG_DEBUG);
    }

  /* add the root cpuset to the map */
  adjust_root_map(rootStr, cpusetMap, TRUE);

  /* now remove the boot cpuset from the map */
  adjust_root_map(bootStr, cpusetMap, FALSE);

  /* convert the cpuset map back into the root cpuset string */
  rootStr[0] = '\0';
  first = TRUE;
  for (j=0; j<cpuset_size; j++)
    {
    if (cpusetMap[j] > 0 )
      {
      if (first)
        {
        sprintf (rootStr, "%d", j);
        first = FALSE;
        }
      else
        {
        sprintf (tmpBuf, ",%d", j);
        strcat (rootStr, tmpBuf);
        }
      }
    }

  if (LOGLEVEL >= 7)
    {
    sprintf(log_buffer,
        "resulting root cpuset (%s)",
        rootStr);
    log_ext(-1, __func__, log_buffer, LOG_DEBUG);
    }

  free(cpusetMap);

  return;
  } /* END remove_boot_set() */





/**
 * Check if a pid_t number is a process ID or thread ID.
 *
 * Return 1 if it is a pid.
 * Return 0 if it is a tid.
 * Return -1 if error.
 *
 * The check compares the tread group ID from /proc/<id>/status with pid.
 * If both are equal, id is the master thread of the thread group, and is regarded as "process ID".
 * If they are not, id is a child thread in the thread group, and is regarded as "thread ID".
 */

static int PidIsPid(

  pid_t pid)

  {
  char    path[1024];
  char    readbuf[1024];
  FILE   *fd;
  pid_t   tgid;
  int     rc = -1;

  sprintf(path, "/proc/%d/status", pid);

  if ((fd = fopen(path, "r")) != NULL)
    {
    while ((fgets(readbuf, sizeof(readbuf), fd)) != NULL)
      {
      if ((strncmp(readbuf, "Tgid:",5)) == 0)
        {
        if ((sscanf(readbuf + 5, " %d", &tgid)) == 1)
          rc = tgid == pid;
        break;
        }
      }
    fclose(fd);
    }

  return(rc);
  } /* END PidIsPid() */


/**
 * Lists tasks currently attached to a cpuset incl. its child cpusets.
 *
 * Returns a new allocated list of pids.
 * Returns NULL on error, or if no pids are found.
 *
 * @param name - (I) - string
 * @param pids - (I) - pid list
 *
 * NOTES:
 * - If name starts with /, it is used as is. If not, it is
 *   relative to the TORQUE cpuset.
 * - The returned pid list must be freed with free_pidlist().
 * - The parameter pids is used for recursive calls. Found
 *   pids are appended to this list. The top-level call
 *   should call with pids = NULL.
 * - pids will contain process IDs, only. Thread IDs are
 *   not stored.
 */

struct pidl *get_cpuset_pidlist(

  const char  *name, /* I */
  struct pidl *pids) /* I */

  {
  char                   cpuset_path[MAXPATHLEN + 1];
  char                   path[MAXPATHLEN + 1];
  struct pidl           *pl, *pp;
  int                    npids = 0;
  pid_t                  pid;
#ifdef USELIBCPUSET
  struct cpuset_pidlist *plist;
  int                    i;
#else
  char                   tid[1024];
  struct stat            statbuf;
  struct dirent         *pdirent;
  DIR                   *dir;
  FILE                  *fd;
#endif

#ifdef USELIBCPUSET

  /* Construct the name of the cpuset.
   * libcpuset does not want the root-cpuset path in it */

  if (name[0] == '/')
    snprintf(cpuset_path, sizeof(cpuset_path), "%s", name);
  else
    snprintf(cpuset_path, sizeof(cpuset_path), "%s/%s", TTORQUECPUSET_BASE, name);

  /* Get the list of PIDs attached to the cpuset,
   * do not care if the cpuset does not exist */

  if ((plist = cpuset_init_pidlist(cpuset_path, 1)) == NULL)
    {
    if (errno != ENOENT)
      {
      sprintf(log_buffer, "%s: cpuset_init_pidlist", path);
      log_err(errno, __func__, log_buffer);
      }
    return(NULL);
    }

  /* Transform the PID list into what we return */

  pl = NULL;

  for (i = 0; i < cpuset_pidlist_length(plist); i++)
    {
    pid = cpuset_get_pidlist(plist, i);

    /* Do not store IDs of individual threads */
    if ((PidIsPid(pid)) != 1)
      continue;

    if ((pp = (struct pidl *)calloc(1,sizeof(struct pidl))) == NULL)
      {
      log_err(errno, __func__, "calloc");
      break;
      }

    pp->pid  = pid;
    pp->next = NULL;
    npids++;
    if (pl)
      pl->next = pp;
    else
      pids = pp;
    pl = pp;
    } /* END for(i) */

  /* Free the initial PID list */
  cpuset_freepidlist(plist);

#else
  /* Construct the name of the cpuset */
  if (name[0] == '/')
    snprintf(cpuset_path, sizeof(cpuset_path), "%s", name);
  else
    snprintf(cpuset_path, sizeof(cpuset_path), "%s/%s", TTORQUECPUSET_PATH, name);

  /* Try to open cpuset directory, don't care if it does not exist */

  if ((dir = opendir(cpuset_path)) == NULL)
    {
    if (errno != ENOENT)
      {
      sprintf(log_buffer, "%s: opendir", path);
      log_err(errno, __func__, log_buffer);
      }
    }
  else
    {
    /* Dive into child cpusets, if they exist */

    while ((pdirent = readdir(dir)) != NULL)
      {
      /* Skip parent and current directory. */
      if (! strcmp(pdirent->d_name, "."))
        continue;

      if (! strcmp(pdirent->d_name, ".."))
        continue;

      /* Prepend directory name to entry name for lstat. */
      snprintf(path, sizeof(path), "%s/%s", cpuset_path, pdirent->d_name);

      /* Skip entry, if lstat fails. */
      if (lstat(path, &statbuf) == -1)
        continue;

      /* If a directory is found, it is a child cpuset. Parse its content. */
      if ((statbuf.st_mode & S_IFDIR) == S_IFDIR)
        {
        pids = get_cpuset_pidlist(path, pids);
        }

      /* Read tasks list of this cpuset */
      else if (!strcmp(pdirent->d_name, "tasks"))
        {
        /* Find last pidl entry in pids */

        if (pids != NULL)
          {
          pl = pids;
          while(pl->next != NULL)
            pl = pl->next;
          }
        else
          {
          pl = NULL;
          }

        if ((fd = fopen(path, "r")) != NULL)
          {
          /* Read tasks list line by line, store */
          while ((fgets(tid, sizeof(tid), fd)) != NULL)
            {
            pid = atoi(tid);

            /* Do not store IDs of individual threads */
            if ((PidIsPid(pid)) != 1)
              continue;

            if ((pp = (struct pidl *)calloc(1, sizeof(struct pidl))) == NULL)
              {
              log_err(errno, __func__, "calloc");
              break;
              }
            else
              {
              pp->pid  = pid;
              pp->next = NULL;
              npids++;

              if (pl)
                pl->next = pp;
              else
                pids = pp;
              pl = pp;
              }
            } /* END while(fgets) */

          fclose(fd);
          }
        }
      } /* END while(readdir) */

    closedir(dir);
    }  /* END if (opendir) */

#endif

  if (LOGLEVEL >= 6)
    {
    sprintf(log_buffer, "%s contains %d PIDs", cpuset_path, npids);
    log_event(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, __func__, log_buffer);
    }

  return(pids);
  } /* END get_cpuset_pidlist() */




/**
 * Returns memory pressure of a cpuset.
 *
 * Returns a non-negative number on success.
 * Returns -1 on failure.

 * @param name - (I) - string
 *
 * NOTES:
 * - If name starts with /, it is used as is. If not, it is
 *   relative to the TORQUE cpuset.
 * - Child cpusets are not checked.
 */

int get_cpuset_mempressure(

  const char *name) /* I */

  {
  char        path[MAXPATHLEN + 1];
  int         rc;
#ifdef USELIBCPUSET
  int         fd;
#else
  FILE       *fd;
#endif

#ifdef USELIBCPUSET

  /* Construct the name of the cpuset.
   * libcpuset does not want the root-cpuset path in it */
  if (name[0] == '/')
    snprintf(path, sizeof(path), "%s", name);
  else
    snprintf(path, sizeof(path), "%s/%s", TTORQUECPUSET_BASE, name);

  /* Open, read, close */
  if ((fd = cpuset_open_memory_pressure(path)) == -1)
    {
    if (errno != ENOENT)
      {
      sprintf(log_buffer, "%s: cpuset_open_memory_pressure", path);
      log_err(errno, __func__, log_buffer);
      }

    return(-1);
    }

  if ((rc = cpuset_read_memory_pressure(fd)) == -1)
    {
    sprintf(log_buffer, "%s: cpuset_read_memory_pressure", path);
    log_err(errno, __func__, log_buffer);
    }

  cpuset_close_memory_pressure(fd);

  if (LOGLEVEL >= 6)
    {
    sprintf(log_buffer, "%s/memory_pressure=%d", path, rc);
    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, __func__, log_buffer);
    }

#else

  /* Construct the name of the cpuset's memory_pressure file */
  if (name[0] == '/')
    snprintf(path, sizeof(path), "%s/memory_pressure", name);
  else
    snprintf(path, sizeof(path), "%s/%s/memory_pressure", TTORQUECPUSET_PATH, name);

  /* Open, read, close */
  if ((fd = fopen(path, "r")) == NULL)
    {
    if (errno != ENOENT)
      {
      sprintf(log_buffer, "%s: fopen", path);
      log_err(errno, __func__, log_buffer);
      }
    return(-1);
    }

  if ((fscanf(fd, "%d", &rc)) != 1)
    {
    sprintf(log_buffer, "%s: fscanf", path);
    rc = -1;
    }

  fclose(fd);

  if (LOGLEVEL >= 6)
    {
    sprintf(log_buffer, "%s=%d", path, rc);
    log_event(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, __func__, log_buffer);
    }

#endif

  return(rc);
  } /* END get_cpuset_mempressure() */

