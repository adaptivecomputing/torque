#include <pthread.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include "license_pbs.h" /* See here for the software license */
#include "lib_net.h"
#include "test_net_cache.h"
#include "net_cache.h"
#include <stdlib.h>
#include <stdio.h>

#include "pbs_error.h"

#define NUM_THREADS 100

extern const char *getRandomWord(unsigned int *);

void *add_and_lookup_stuff(void *parm);

bool everybody_started  = false;
unsigned int seedp[NUM_THREADS];

START_TEST(test_one)
  {
  pthread_t thread_ids[NUM_THREADS];

    for(int i=0;i < NUM_THREADS;i++)
    {
        pthread_t num;
        //Spin off a bunch of threads that will hammer the cache simultaneously.
        pthread_create(&num,NULL,add_and_lookup_stuff,(void *)&seedp[i]);
        thread_ids[i] = num;
    }

    sleep(3);
    everybody_started = true;

    for(int i = 0; i < NUM_THREADS; i++)
      {
      pthread_join(thread_ids[i], NULL);
      }

    //Exit without shutting down the cache. It turns out that the cache destructor can be called
    //before the threads exit and we want to make sure the exit doesn't create any segfaults
    //or exceptions.
  }
END_TEST

void *add_and_lookup_stuff(void *parm)
{
  int i = 0;

    while((i < 1000) || (everybody_started == false))
    {
      struct addrinfo *pAddr = (struct addrinfo *)calloc(1,sizeof(addrinfo));
      struct sockaddr_in *pINetAddr;
      const char *word = getRandomWord((unsigned int *)parm);

      i++;
      if(NULL == get_cached_addrinfo(word))
      {
        pAddr->ai_addr = (struct sockaddr *)calloc(1,sizeof(struct sockaddr_in));
        pAddr->ai_family = AF_INET;
        pINetAddr = (struct sockaddr_in *)pAddr->ai_addr;

        pAddr->ai_canonname = strdup(word);
        pINetAddr->sin_addr.s_addr = rand_r((unsigned int *)parm);        
        pAddr = insert_addr_name_info(pAddr,pAddr->ai_canonname);
      }
      else
      {
        freeaddrinfo(pAddr);
        pAddr = get_cached_addrinfo_full(word);
      }

      fail_unless((pAddr != NULL));

      if(pAddr != NULL)
        {
        const char *p1;
        const char *p2;
        struct sockaddr_in *p3;
        struct addrinfo *p4;

        pINetAddr = (struct sockaddr_in *)pAddr->ai_addr;
        p1 = get_cached_nameinfo(pINetAddr);
        p2 = get_cached_fullhostname(word,pINetAddr);
        p3 = get_cached_addrinfo(word);
        p4 = get_cached_addrinfo_full(word);
        fail_unless(((p1 != NULL)&&(p2 != NULL)&&(p3 != NULL)&&(p4 != NULL)));
        }
      else
        {
        }
    }
  return NULL;
  }


/*START_TEST(test_two)
  {

  }
END_TEST*/

Suite *get_hostaddr_suite(void)
  {
  Suite *s = suite_create("net_cache_suite methods");
  TCase *tc_core = tcase_create("test_one");
  tcase_set_timeout(tc_core,10);
  tcase_add_test(tc_core, test_one);
  suite_add_tcase(s, tc_core);

/*  tc_core = tcase_create("test_two");
  tcase_add_test(tc_core, test_two);
  suite_add_tcase(s, tc_core);*/

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
