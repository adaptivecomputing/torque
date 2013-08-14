#include <pthread.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "license_pbs.h" /* See here for the software license */
#include "lib_net.h"
#include "test_net_cache.h"
#include "net_cache.h"
#include <stdlib.h>
#include <stdio.h>

#include "pbs_error.h"

#define NUMTHREADS 20

extern const char *getRandomWord(unsigned int *);

void *add_and_lookup_stuff(void *parm);

int          exited = FALSE;
unsigned int seedp[NUMTHREADS];

START_TEST(test_one)
  {

  for (int i=0; i < NUMTHREADS; i++)
    {
    pthread_t num;
    //Spin off a bunch of threads that will hammer the cache simultaneously.
    pthread_create(&num,NULL,add_and_lookup_stuff,(void *)&seedp[i]);
    }

  sleep(2);
  exited = TRUE;
  //Exit without shutting down the cache. It turns out that the cache destructor can be called
  //before the threads exit and we want to make sure the exit doesn't create any segfaults
  //or exceptions.
  }
END_TEST

void *add_and_lookup_stuff(void *parm)
  {

  while(1)
    {
    struct addrinfo *pAddr = (struct addrinfo *)calloc(1,sizeof(addrinfo));
    struct sockaddr_in *pINetAddr;
    const char *word = getRandomWord((unsigned int *)parm);
        
    if (pAddr == NULL)
      {
      //We're out of memory, just bail on this thread.
      return NULL;
      }
    
    if (get_cached_addrinfo(word) == NULL)
      {
      pAddr->ai_addr = (struct sockaddr *)calloc(1,sizeof(struct sockaddr_in));
            
      if (pAddr->ai_addr == NULL)
        {
        //Out of memory so bail out on this thread.
        return(NULL);
        }

      pAddr->ai_family = AF_INET;
      pINetAddr = (struct sockaddr_in *)pAddr->ai_addr;
      
      pAddr->ai_canonname = strdup(word);
      if (pAddr->ai_canonname == NULL)
        {
        //Out of memory so bail out on this thread.
        return(NULL);
        }

      pINetAddr->sin_addr.s_addr = rand_r((unsigned int *)parm);
      pAddr = insert_addr_name_info(pAddr,pAddr->ai_canonname);
      }
    else
      {
      freeaddrinfo(pAddr);
      pAddr = get_cached_addrinfo_full(word);
      }
    
    fail_unless((pAddr != NULL)||(exited == TRUE));
    
    if (pAddr != NULL)
      {
      char *p1;
      char *p2;
      struct sockaddr_in *p3;
      struct addrinfo *p4;
      
      pINetAddr = (struct sockaddr_in *)pAddr->ai_addr;
      p1 = get_cached_nameinfo(pINetAddr);
      p2 = get_cached_fullhostname(word,pINetAddr);
      p3 = get_cached_addrinfo(word);
      p4 = get_cached_addrinfo_full(word);
      fail_unless(((p1 != NULL)&&(p2 != NULL)&&(p3 != NULL)&&(p4 != NULL))||(exited == TRUE));
      }
    else
      {
      }
    }
  return NULL;
  }


START_TEST(test_two)
  {

  }
END_TEST

Suite *get_hostaddr_suite(void)
  {
  Suite *s = suite_create("net_cache_suite methods");
  TCase *tc_core = tcase_create("test_one");
  tcase_add_test(tc_core, test_one);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_two");
  tcase_add_test(tc_core, test_two);
  suite_add_tcase(s, tc_core);

  return s;
  }

void rundebug()
  {
  }

int main(void)
  {
  int number_failed = 0;
  SRunner *sr = NULL;
  rundebug();
  sr = srunner_create(get_hostaddr_suite());
  srunner_set_log(sr, "net_cache_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
