#include <pbs_config.h>

#include <sys/stat.h>
#include <dirent.h>
#include <stdio.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <sys/param.h>

#include "libpbs.h"
#include "attribute.h"
#include "server_limits.h"
#include "job.h"
#include "log.h"

#define TTORQUECPUSET_PATH "/dev/cpuset/torque"
#define TROOTCPUSET_PATH   "/dev/cpuset"

/* FIXME: TODO:  TTORQUECPUSET_PATH, enabling cpuset support, and correct error
 * checking need a run-time config */

int           *VPToCPUMap = NULL;  /* map of virtual processors to cpus (alloc) */

extern char    mom_host[];
extern char    mom_short_name[];

int cpuset_delete(

  char *cpusetname)  /* I */

  {
  char   path[1024 + 1];
  char   childpath[1024 + 1];
  pid_t  killpids;
  FILE  *fd;
  DIR   *dir;
  struct dirent	*pdirent;
  static char id[] = "cpuset_delete";
  struct stat statbuf;

  /* accept a full path or jobid */
  if (cpusetname[0]=='/')
    strcpy(path,cpusetname);
  else
    sprintf(path,"%s/%s",TTORQUECPUSET_PATH,cpusetname);

  if ((dir = opendir(path)) == NULL)
    {
    /* cpuset does not exist... noone cares! */

    return(0);
    }

  while ((pdirent = readdir(dir)) != NULL)
    {
    /* Skip parent and current directory. */

    if (!strcmp(pdirent->d_name,".") || !strcmp(pdirent->d_name, "..")) 
      continue;
      
    /* Prepend directory name to file name for lstat. */
      
    strcpy(childpath,path);
    strcat(childpath, "/");
    strcat(childpath, pdirent->d_name);
      
    /* Skip file on error. */

    if (!(lstat(childpath,&statbuf)>=0)) 
      continue;

    /* If a directory is found try to get cpuset info about it. */

    if (statbuf.st_mode&S_IFDIR)
      {
      if (!cpuset_delete(childpath))
        {
        sprintf(log_buffer,"Unused cpuset '%s' deleted.", 
          childpath);

        log_err(-1,id,log_buffer);
        }
      else
        {
      	sprintf (log_buffer, "Could not delete unused cpuset %s.", 
          childpath);

        log_err(-1,id,log_buffer);
        }
      }
    else if (!strcmp(pdirent->d_name,"tasks"))
      {
      /* FIXME: need a more careful mechanism here... including a possibly useless sleep */

      if ((fd = fopen(childpath,"r"))!=NULL)
        {
        while(fscanf(fd,"%d",&killpids) == 1)
          kill(killpids,SIGKILL);
        }

      sleep(1);
      }

    /* FIXME: only need when testing with a fake /dev/cpuset */

    if (!(statbuf.st_mode & S_IFDIR))
      unlink(childpath);
    }

  if (rmdir(path) != 0)
    {
    /* FAILURE */

    return(0);
    }

  /* SUCCESS */

  return(1);
  }  /* END cpuset_delete() */





/*
 * Create the root cpuset for Torque if it doesn't already exist.
 * clear out any job cpusets for jobs that no longer exist.
 */

void initialize_root_cpuset()

  {
  static char    id[] = "initialize_root_cpuset";

  DIR		*dir;
  struct dirent	*pdirent;
  char           path[MAXPATHLEN + 1];
  struct stat    statbuf;

  char cpuset_buf[1024];
  FILE *fp;

  sprintf(log_buffer,"Init TORQUE cpuset %s.",
    TTORQUECPUSET_PATH);

  log_err(-1,id,log_buffer);

  /* make sure cpusets are available */

  sprintf(path,"%s/cpus",
    TROOTCPUSET_PATH);

  if (lstat(path,&statbuf) != 0) 
    {
    sprintf(log_buffer,"cannot locate %s - cpusets not configured/enabled on host\n",
      path);

    log_err(-1,id,log_buffer);

    /* FAILURE */

    return;
    }

  if (lstat(TTORQUECPUSET_PATH,&statbuf) != 0)
    {
    sprintf(log_buffer,"TORQUE cpuset %s does not exist, creating it now.\n",
      path);

    log_err(-1,id,log_buffer);

    mkdir(path,0755);

    /* load all cpus in root set */

    sprintf(path,"%s/cpus",
      TROOTCPUSET_PATH);

    fp = fopen(path,"r");

    if (fp != NULL) 
      {  
      char *cptr;
      char *dptr;

      char *ptr;

      int   maxindex;
      int   mindex;

      char  tmpBuf[1024];

      /* FORMAT:  <CPU#>[<CPU#>][,<CPU#>[<CPU#>]]... */

      /* read cpus from root cpuset */

      /* FIXME: need proper error checking and response */

      fread(cpuset_buf,sizeof(char),sizeof(cpuset_buf),fp);

      fclose(fp);

      sprintf(log_buffer,"root cpuset %s loaded with value '%s'\n",
        path,
        cpuset_buf);

      log_err(-1,id,log_buffer);

      /* convert string to lookup table */

      strncpy(tmpBuf,cpuset_buf,sizeof(tmpBuf));

      /* extract last cpu index value */

      cptr = strchr(cpuset_buf,',');
      dptr = strchr(cpuset_buf,'-');

      ptr = MAX(cptr,dptr);

      if (ptr == NULL)
        ptr = cpuset_buf;
      else
        ptr++;

      maxindex = strtol(ptr,NULL,10);

      VPToCPUMap = (int *)calloc(1,sizeof(int) * maxindex);

      strncpy(tmpBuf,cpuset_buf,sizeof(tmpBuf));

      mindex = 0;

      ptr = strtok(tmpBuf,",");

      while (ptr != NULL)
        {
        ptr = strtok(ptr,"-");

        while (ptr != NULL)
          {
          }
        }
      /* NOTE:  load 'boot' set */

      sprintf(path,"%s/boot/cpus",
        TROOTCPUSET_PATH);

      fp = fopen(path,"r");

      if (fp != NULL)
        {
        char bootbuf[1024];

        /* what is format of data? */

        /* read cpus from boot cpuset */

        /* FIXME: need proper error checking and response */

        fread(bootbuf,sizeof(char),sizeof(bootbuf),fp);

        fclose(fp);

        /* subtract bootset from rootset */

        /* NYI */
        }  /* END if (fp != NULL) */

      /* create new TORQUE set */

      sprintf(path,"%s/cpus",
        TTORQUECPUSET_PATH);

      fp = fopen(path,"w");

      if (fp != NULL) 
        {
        /* write all root cpus into TORQUE cpuset */

        sprintf(log_buffer,"adding cpus %s to %s",
          cpuset_buf,
          path);

        log_err(-1,id,log_buffer);

        fwrite(cpuset_buf,sizeof(char),strlen(cpuset_buf),fp);

        fclose(fp);
        }

      memset(cpuset_buf,'\0',sizeof(cpuset_buf));
      }  /* END if (fp != NULL) */

    /* add all mems to torqueset */

    sprintf(path,"%s/mems",
      TROOTCPUSET_PATH);

    fp = fopen(path,"r");

    if (fp != NULL) 
      {
      /* what is format of data? */

      /* read all mems from root cpuset */

      fread(cpuset_buf,sizeof(char),1023,fp);

      fclose(fp);

      sprintf(path,"%s/mems",
        TTORQUECPUSET_PATH);

      fp = fopen(path,"w");

      if (fp != NULL) 
        {
        /* write all root mems into TORQUE cpuset */

        sprintf(log_buffer,"adding mems %s to %s",
          cpuset_buf,
          path);

        log_err(-1,id,log_buffer);

        fwrite(cpuset_buf,sizeof(char),strlen(cpuset_buf),fp);

        fclose(fp);
        }

      memset(cpuset_buf,'\0',sizeof(cpuset_buf));
      }  /* END if (fp != NULL) */
    }    /* END if (lstat(path,&statbuf) != 0) */ 
  else 
    {
    /* The cpuset already exists, delete any cpusets for jobs that no longer exist. */

    /* Find all the job cpusets. */

    if ((dir = opendir(TTORQUECPUSET_PATH)) == NULL)
      {
      sprintf(log_buffer,"opendir(%s) failed.\n", 
        TTORQUECPUSET_PATH);

      log_err(-1,id,log_buffer);
      }

    while ((pdirent = readdir(dir)) != NULL)
      {
      	    /* Skip parent and current directory. */
      	    if (!strcmp(pdirent->d_name, ".")||!strcmp(pdirent->d_name, "..")) continue;
      
      	    /* Prepend directory name to file name for lstat. */
      	    strcpy(path, TTORQUECPUSET_PATH);
      
      	    if (path[strlen(path)-1]!='/') strcat(path, "/");
      	    strcat(path, pdirent->d_name);
      
      	    /* Skip file on error. */
      	    if (!(lstat(path, &statbuf)>=0)) continue;

      	    /* If a directory is found try to get cpuset info about it. */
      	    if (statbuf.st_mode&S_IFDIR)
      	    {
                    /* If the job isn't found, delete its cpuset. */
		    if (find_job(pdirent->d_name) == NULL)
                    {
      	                  if (!cpuset_delete(pdirent->d_name))
                            {
                                    sprintf (log_buffer, "Unused cpuset '%s' deleted.", path);
                                    log_err(-1,id,log_buffer);
                            }
                            else
                            {
      	                        sprintf (log_buffer, "Could not delete unused cpuset %s.", path);
                                    log_err(-1,id,log_buffer);
                            }
                    }
      	    }
        }
        closedir(dir);
    }

  return;
  }  /* END initialize_root_cpuset() */





int create_jobset (
    job *pjob)
{
  static char	id[] = "create_jobset";
  char path[1024+1];
  char rootpath[1024+1];
  char tmppath[1024+1];
  char cpusbuf[1024+1];
  char tasksbuf[1024+1];
  int ix;
  FILE *fd;
  vnodent       *np;
  int j;
  mode_t savemask;

  savemask=(umask(0022));

  sprintf(path,"%s/%s",TTORQUECPUSET_PATH,pjob->ji_qs.ji_jobid);

  if (access(path,F_OK)==0)
    {
    if (!cpuset_delete(path))
      {
      sprintf (log_buffer, "Could not delete cpuset for job %s.\n", pjob->ji_qs.ji_jobid);
      log_err(-1,id,log_buffer);
      umask(savemask);
      return(0);
      }
    }

if (access(TTORQUECPUSET_PATH,F_OK)==0) {

  /* create the jobset */
  mkdir(path,0755);

  /* add all mems to jobset */
  strcpy(rootpath,TTORQUECPUSET_PATH);
  strcat(rootpath,"/mems");
  fd=fopen(rootpath,"r");
if (fd) {
  fread(cpusbuf, sizeof(char), 1023, fd);
  fclose(fd);
  strcpy(tmppath,path);
  strcat(tmppath,"/mems");
  fd=fopen(tmppath,"w");
  fwrite(cpusbuf, sizeof(char), strlen(cpusbuf), fd);
  fclose(fd);
  memset(cpusbuf,'\0',sizeof(cpusbuf));
}
}


  /* Make the string defining the CPUs to add into the jobset */
  np = pjob->ji_vnods;
  cpusbuf[0]='\0';
  ix = 0;
  for (j = 0;j < pjob->ji_numvnod;++j,np++)
    {
    if (pjob->ji_nodeid == np->vn_host->hn_node)
      {
      if (cpusbuf[0]!='\0')
        strcat(cpusbuf,",");

      sprintf(tmppath,"%d",np->vn_index);
      strcat(cpusbuf,tmppath);

      }
    }

  /* add the CPUs to the jobset */
  strcpy(tmppath,path);
  strcat(tmppath,"/cpus");
sprintf (log_buffer, "CPUSET: %s job %s path %s\n", cpusbuf, pjob->ji_qs.ji_jobid,tmppath);
log_err(-1,id,log_buffer);
  fd=fopen(tmppath,"w");
if (fd) {
  fwrite(cpusbuf, sizeof(char), strlen(cpusbuf), fd);
  fclose(fd);
}
	memset(cpusbuf,'\0',sizeof(cpusbuf));
 
  /* Create the vnodesets */
  np = pjob->ji_vnods;
  cpusbuf[0]='\0';
  ix = 0;
  for (j = 0;j < pjob->ji_numvnod;++j,np++)
    {
    if (pjob->ji_nodeid == np->vn_host->hn_node)
      {
      sprintf(tmppath,"%s/%s/%d",TTORQUECPUSET_PATH,pjob->ji_qs.ji_jobid,np->vn_node);
      mkdir(tmppath,0755);
      chmod(tmppath,00755);
      sprintf(tasksbuf,"%d",np->vn_index);
      strcat(tmppath,"/cpus");
sprintf (log_buffer, "TASKSET: %s cpus %s\n", tmppath,tasksbuf );
log_err(-1,id,log_buffer);
      fd=fopen(tmppath,"w");
if (fd) {
      fwrite(tasksbuf, sizeof(char), strlen(tasksbuf), fd);
      fclose(fd);
}
      memset(tasksbuf,'\0',sizeof(tasksbuf));


        /* add all mems to torqueset */
        sprintf(tmppath,"%s/%s/%s",TTORQUECPUSET_PATH,pjob->ji_qs.ji_jobid,"/mems");
        fd=fopen(tmppath,"r");
if (fd) {
        fread(tasksbuf, sizeof(char), 1023, fd);
        fclose(fd);
}
        sprintf(tmppath,"%s/%s/%d/%s",TTORQUECPUSET_PATH,pjob->ji_qs.ji_jobid,np->vn_node,"/mems");
        fd=fopen(tmppath,"w");
if (fd) {
sprintf (log_buffer, "adding %s to %s",tasksbuf,tmppath);
log_err(-1,id,log_buffer);
        fwrite(tasksbuf, sizeof(char), strlen(tasksbuf), fd);
        fclose(fd);
}
        memset(tasksbuf,'\0',sizeof(tasksbuf));


      }
    }


  umask(savemask);

  return 0;
}


int move_to_jobset(pid_t pid,job *pjob)
  {
  char pidbuf[1024];
  char taskspath[1024];
  FILE *fd;
  mode_t savemask;

  savemask=(umask(0022));

  sprintf(pidbuf,"%d",pid);
  sprintf(taskspath,"%s/%s/tasks",TTORQUECPUSET_PATH,pjob->ji_qs.ji_jobid);
sprintf (log_buffer, "CPUSET MOVE: %s  %s\n", taskspath, pidbuf);
log_err(-1,"move_to_jobset",log_buffer);
  
  fd=fopen(taskspath,"w");
if (fd) {
  fwrite(pidbuf,sizeof(char),strlen(pidbuf),fd);
  fclose(fd);
}
  memset(pidbuf,'\0',sizeof(pidbuf));

  umask(savemask);

  return 0;
  }

int move_to_taskset(pid_t pid,job *pjob,char * vnodeid)
  {

  char pidbuf[1024];
  char taskspath[1024];
  FILE *fd;
  mode_t savemask;

  savemask=(umask(0022));

  sprintf(pidbuf,"%d",pid);
  sprintf(taskspath,"%s/%s/%s/tasks",TTORQUECPUSET_PATH,pjob->ji_qs.ji_jobid,vnodeid);
sprintf (log_buffer, "TASKSET MOVE: %s  %s\n", taskspath, pidbuf);
log_err(-1,"move_to_taskset",log_buffer);
  
  fd=fopen(taskspath,"w");
if (fd) {
  fwrite(pidbuf,sizeof(char),strlen(pidbuf),fd);
  fclose(fd);
}
  memset(pidbuf,'\0',sizeof(pidbuf));

  umask(savemask);

  return 0;
  }
