#include <pbs_config.h>

#include <bitmask.h>
#include <cpuset.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdio.h>

#include "libpbs.h"
#include "attribute.h"
#include "server_limits.h"
#include "job.h"
#include "log.h"

/*
 * Create the root cpuset for Torque if it doesn't already exist.
 * clear out any job cpusets for jobs that no longer exist.
 */
void initialize_root_cpuset()
{
    static char	id[] = "initialize_root_cpuset";
    DIR		*dir;
    struct dirent	*pdirent;
    char           path[MAXPATHLEN + 1];
    struct stat    statbuf;
    char *dir_path = "/dev/cpuset/torque";
    char cpuset_name[MAXPATHLEN+1];
    struct cpuset *root_set;
    struct cpuset *torque_set;
    int cpu_map_size;
    int mem_map_size;
    struct bitmask *available_cpus;
    struct bitmask *available_mems;

    /* Create the top level torque cpuset if it doesn't already exist. */
    torque_set = cpuset_alloc();

    if (cpuset_query(torque_set, "/torque") == -1)
    {
        sprintf (log_buffer, "Torque cpuset %s does not exist, creating it now.\n",dir_path);
        log_err(-1,id,log_buffer);

        cpu_map_size = cpuset_cpus_nbits();
        if (cpu_map_size < 1)
        {
            sprintf (log_buffer, "cpuset_cpus_nbits() failed.\n");
            log_err(-1,id,log_buffer);
        }
    
        available_cpus = bitmask_alloc(cpu_map_size);
        if (available_cpus == NULL)
        {
            sprintf (log_buffer, "bitmask_alloc() failed.\n");
            log_err(-1,id,log_buffer);
        }
    
        mem_map_size = cpuset_mems_nbits();
        if (mem_map_size < 1)
        {
            sprintf (log_buffer, "cpuset_mems_nbits() failed.\n");
            log_err(-1,id,log_buffer);
        }
    
        available_mems = bitmask_alloc(mem_map_size);
        if (available_mems == NULL)
        {
            sprintf (log_buffer, "bitmask_alloc() failed.\n");
            log_err(-1,id,log_buffer);
        }

        root_set = cpuset_alloc();

        if (cpuset_query(root_set, "/") == 0)
        {
            /* Determine all cpus and mems in the root cpuset. */
            cpuset_getcpus(root_set, available_cpus);

            if (bitmask_isallclear(available_cpus))
            {
                sprintf (log_buffer, "Root cpuset has no cpus.\n");
                log_err(-1,id,log_buffer);
            }
    
            cpuset_getmems(root_set, available_mems);
            if (bitmask_isallclear(available_mems))
            {
                sprintf (log_buffer, "Root cpuset has no mems.\n");
                log_err(-1,id,log_buffer);
            }
        }

        /* Find all cpus and mems available in the root cpuset. */
        if (find_free_cpuset_space(available_cpus, available_mems, "/") == 0)
        {
            /* Create the torque root cpuset with the available cpu and mems bitmaps. */
            if (cpuset_setcpus(torque_set, available_cpus) != 0)
            {
                sprintf (log_buffer, "cpuset_setcpus() failed.\n");
                log_err(-1,id,log_buffer);
            }
    
            if (cpuset_setmems(torque_set, available_mems) != 0)
            {
                sprintf (log_buffer, "cpuset_setmems() failed.\n");
                log_err(-1,id,log_buffer);
            }
    
            if (cpuset_create("/torque", torque_set) != 0)
            {
                sprintf (log_buffer, "cpuset_create() failed.\n");
                log_err(-1,id,log_buffer);
            }
        }

        bitmask_free(available_cpus);
        bitmask_free(available_mems);
        cpuset_free(torque_set);
        cpuset_free(root_set);

    /* The cpuset already exists, delete any cpusets for jobs that no longer exist. */
    } else {
        /* Find all the job cpusets. */
        if ((dir = opendir(dir_path)) == NULL)
        {
            sprintf(log_buffer, "opendir(%s) failed.\n", dir_path);
            log_err(-1,id,log_buffer);
        }
        while ((pdirent = readdir(dir)) != NULL)
        {
      	    /* Skip parent and current directory. */
      	    if (!strcmp(pdirent->d_name, ".")||!strcmp(pdirent->d_name, "..")) continue;
      
      	    /* Prepend directory name to file name for lstat. */
      	    strcpy(path, dir_path);
      
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
                          sprintf (cpuset_name, "torque/%s", pdirent->d_name);
      	                  if (cpuset_delete(cpuset_name) != 0)
                            {
                                    sprintf (log_buffer, "Unused cpuset '%s' deleted.", path);
                                    log_err(0,id,log_buffer);
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
}

int create_job_set (
    char *job_id,
    char *cpuset_name,
    int num_cpus,
    int num_mems)
{
  static char	id[] = "create_job_set";
  struct cpuset *root_set;
  struct cpuset *new_cpuset;

  struct bitmask *available_cpus;
  struct bitmask *available_mems;
  int index;
  int count;
  int cpu_map_size;
  int mem_map_size;
  int max_cpus;
  int max_mems;

    /* Check if the cpuset for the job already exists. */
    new_cpuset = cpuset_alloc();
    if (new_cpuset == NULL)
    {
        sprintf (log_buffer, "Cpuset_alloc() failed.\n");
        log_err(-1,id,log_buffer);
        return -1;
    }
    if (cpuset_query(new_cpuset, cpuset_name) == 0) 
    {
	sprintf (log_buffer, "Cpuset for job %s already exists, deleting it now.\n", job_id);
        log_err(0,id,log_buffer);

	if (cpuset_delete(cpuset_name) != 0)
        {
	    sprintf (log_buffer, "Could not delete cpuset for job %s.\n", job_id);
            log_err(-1,id,log_buffer);
            return -1;
        }
    }

    /* Get information about the root cpuset. */
    root_set = cpuset_alloc();
    if (root_set == NULL)
    {
        sprintf (log_buffer, "cpuset_alloc() failed.\n");
        log_err(-1,id,log_buffer);
        return -1;
    }

    if (cpuset_query(root_set, "/torque") != 0)
    {
        sprintf (log_buffer, "cpuset_query() of root cpuset failed.\n");
        log_err(-1,id,log_buffer);
        return -1;
    }

    cpu_map_size = cpuset_cpus_nbits();
    if (cpu_map_size < 1)
    {
        sprintf (log_buffer, "cpuset_cpus_nbits() failed.\n");
        log_err(-1,id,log_buffer);
        return -1;
    }

    available_cpus = bitmask_alloc(cpu_map_size);
    if (available_cpus == NULL)
    {
        sprintf (log_buffer, "bitmask_alloc() failed.\n");
        log_err(-1,id,log_buffer);
        return -1;
    }

    mem_map_size = cpuset_mems_nbits();
    if (mem_map_size < 1)
    {
        sprintf (log_buffer, "cpuset_mems_nbits() failed.\n");
        log_err(-1,id,log_buffer);
        return -1;
    }

    available_mems = bitmask_alloc(mem_map_size);
    if (available_mems == NULL)
    {
        sprintf (log_buffer, "bitmask_alloc() failed.\n");
        log_err(-1,id,log_buffer);
        return -1;
    }

    /* Sanity check */
    if (num_cpus < 1)
    {
        sprintf (log_buffer, "Job %s requested %d cpus, it needs to request at least one.\n",
            job_id, num_cpus);
        log_err(-1,id,log_buffer);
        return -1;
    }

    /* If more cpus are being requested than exist on the machine exit. */
    max_cpus = cpuset_cpus_weight(root_set);
    if (num_cpus > max_cpus)
    {
        sprintf (log_buffer, "Job %s requests %d cpus, there are only %d on this machine.\n",
            job_id, num_cpus, max_cpus);
        log_err(-1,id,log_buffer);
        return -1;
    }

    /* If more mems are being requested than exist on the machine exit. */
    max_mems = cpuset_mems_weight(root_set);
    if (num_mems > max_mems)
    {
        sprintf (log_buffer, "Job %s requests %d mems, there are only %d on this machine.\n",
            job_id, num_mems, max_mems);
        log_err(-1,id,log_buffer);
        return -1;
    }

    /* Determine all cpus and mems in the root cpuset. */
    cpuset_getcpus(root_set, available_cpus);
    if (bitmask_isallclear(available_cpus))
    {
        sprintf (log_buffer, "Root cpuset has no cpus.\n");
        log_err(-1,id,log_buffer);
        return -1;
    }

    cpuset_getmems(root_set, available_mems);
    if (bitmask_isallclear(available_mems))
    {
        sprintf (log_buffer, "Root cpuset has no mems.\n");
        log_err(-1,id,log_buffer);
        return -1;
    }
    cpuset_free(root_set);

    /* Find what cpus and mems are available. */
    if (find_free_cpuset_space(available_cpus, available_mems, "/torque") != 0)
    {
        return -1;
    }

    /* Walk through the bitmap and find the number of needed cpus in the bitmap. */
    for (index = bitmask_first(available_cpus), count = 0;
        index < cpu_map_size && count < num_cpus;
        index = bitmask_next(available_cpus, index+1), count++)
    {
/* PME!! Only output on high log level. */
        sprintf(log_buffer, "CPU bit set: %d, count: %d\n", index, count);
        log_err(0,id,log_buffer);
    }

    /* If we are not past the end of the bitmap there are extra, unneeded cpus, clear their bits. */
    if (index != cpu_map_size)
    {
        bitmask_clearrange(available_cpus, index, cpu_map_size-1);
    }
    else if (count != num_cpus)
    {
        sprintf (log_buffer, "Job %s requested %d cpus, found %d.\n", job_id, num_cpus, count);
        log_err(-1,id,log_buffer);
        return -1;
    }

    /* Walk through the bitmap and find the number of needed mems in the bitmap. */
    for (index = bitmask_first(available_mems), count = 0;
        index < mem_map_size && count < num_mems;
        index = bitmask_next(available_mems, index+1), count++)
    {
/* PME!! Only output on high log level. */
        sprintf(log_buffer, "Mem bit set: %d, count: %d\n", index, count);
        log_err(0,id,log_buffer);
    }

    /* If we are not past the end of the bitmap there are extra, unneeded mems, clear their bits. */
    if (index != mem_map_size)
    {
        bitmask_clearrange(available_mems, index, mem_map_size-1);
    }
    else if (count != num_mems)
    {
        sprintf (log_buffer, "Job %s requested %d mems, found %d.\n", job_id, num_mems, count);
        log_err(-1,id,log_buffer);
        return -1;
    }

    /* Create the cpuset with the job's name and the available cpu and mems bitmaps. */
    if (cpuset_setcpus(new_cpuset, available_cpus) != 0)
    {
        sprintf (log_buffer, "cpuset_setcpus() failed.\n");
        log_err(-1,id,log_buffer);
        return -1;
    }

    if (cpuset_setmems(new_cpuset, available_mems) != 0)
    {
        sprintf (log_buffer, "cpuset_setmems() failed.\n");
        log_err(-1,id,log_buffer);
        return -1;
    }

    if (cpuset_create(cpuset_name, new_cpuset) != 0)
    {
        sprintf (log_buffer, "cpuset_create() failed.\n");
        log_err(-1,id,log_buffer);
        return -1;
    }

    bitmask_free(available_cpus);
    bitmask_free(available_mems);
    cpuset_free(new_cpuset);
    return 0;
}

/* Look through all of the cpusets in the root cpuset and determine cpus and memory that are available. */
int find_free_cpuset_space(
    struct bitmask *available_cpus,
    struct bitmask *available_mems,
    char *parent_path)
{

  static char	id[] = "find_free_cpuset_space";
  struct cpuset *child_cpuset;
  struct bitmask *child_cpus;
  struct bitmask *child_mems;
  struct bitmask *result_cpus;
  struct bitmask *result_mems;
  DIR *cpu_root;
  struct dirent *dir_entry;
  struct stat stat_buf;
  char path[MAXPATHLEN + 1];
  char dir_path[MAXPATHLEN + 1];
  char cpuset_name[MAXPATHLEN + 1];

    child_cpuset = cpuset_alloc();
    if (child_cpuset == NULL)
    {
        sprintf (log_buffer, "cpuset_alloc() failed.\n");
        log_err(-1,id,log_buffer);
        return -1;
    }

    child_cpus = bitmask_alloc(cpuset_cpus_nbits());
    if (child_cpus == NULL)
    {
        sprintf (log_buffer, "bitmask_alloc() failed.\n");
        log_err(-1,id,log_buffer);
        return -1;
    }

    child_mems = bitmask_alloc(cpuset_mems_nbits());
    if (child_mems == NULL)
    {
        sprintf (log_buffer, "bitmask_alloc() failed.\n");
        log_err(-1,id,log_buffer);
        return -1;
    }

    result_cpus = bitmask_alloc(cpuset_cpus_nbits());
    if (result_cpus == NULL)
    {
        sprintf (log_buffer, "bitmask_alloc() failed.\n");
        log_err(-1,id,log_buffer);
        return -1;
    }

    result_mems = bitmask_alloc(cpuset_mems_nbits());
    if (result_mems == NULL)
    {
        sprintf (log_buffer, "bitmask_alloc() failed.\n");
        log_err(-1,id,log_buffer);
        return -1;
    }

    /* Find all cpusets directly under the cpuset root directory. */
    sprintf (dir_path, "/dev/cpuset%s", parent_path);
    cpu_root = opendir(dir_path);
    if (cpu_root == NULL)
    {
        sprintf(log_buffer, "opendir(%s) failed.\n", dir_path);
        log_err(-1,id,log_buffer);
    }

    while ((dir_entry = readdir(cpu_root)) != NULL)
    {
	/* Skip parent and current directory. */
	if (!strcmp(dir_entry->d_name, ".")||!strcmp(dir_entry->d_name, "..")) continue;

	/* Prepend directory name to file name for lstat. */
	strcpy(path, dir_path);
	if (path[strlen(path)-1]!='/') strcat(path, "/");
	strcat(path, dir_entry->d_name);

	/* Skip file on error. */
	if (!(lstat(path, &stat_buf)>=0)) continue;

	/* If a directory is found try to get cpuset info about it. */
	if (stat_buf.st_mode&S_IFDIR)
	{
            sprintf (cpuset_name, "%s/%s", parent_path, dir_entry->d_name);
            if (cpuset_query(child_cpuset, cpuset_name) != 0)
            {
                sprintf (log_buffer, "cpuset_query(%s) failed.\n", cpuset_name);
                log_err(-1,id,log_buffer);
                return -1;
            }

            /* Determine all cpus and mems in the cpuset. */
            cpuset_getcpus(child_cpuset, child_cpus);
            cpuset_getmems(child_cpuset, child_mems);

            /* Mask child_cpus bits off of available_cpus. */
            if (bitmask_subset(child_cpus, available_cpus))
            {
                bitmask_andnot(result_cpus, available_cpus, child_cpus);
                bitmask_copy(available_cpus, result_cpus);
            }
            else
            {
                sprintf (log_buffer, "cpuset %s not a subset of parent cpuset '%s'.\n", cpuset_name, dir_path);
                log_err(-1,id,log_buffer);
                return -1;
            }

            /* Mask child_mems bits off of available_mems. */
            if (bitmask_subset(child_mems, available_mems))
            {
                bitmask_andnot(result_mems, available_mems, child_mems);
                bitmask_copy(available_mems, result_mems);
            }
            else
            {
                sprintf (log_buffer, "memset %s not a subset of parent memset '%s'.\n", cpuset_name, dir_path);
                log_err(-1,id,log_buffer);
                return -1;
            }
	}
    }

    closedir(cpu_root);
    bitmask_free(child_cpus);
    bitmask_free(child_mems);
    cpuset_free(child_cpuset);
    return 0;
}


