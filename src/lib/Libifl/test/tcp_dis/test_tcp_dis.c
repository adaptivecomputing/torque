#include "license_pbs.h" /* See here for the software license */
#include "lib_ifl.h"
#include "test_tcp_dis.h"
#include "dis.h"
#include "tcp.h"
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "pbs_error.h"

/* Note: must set MAXCHUNK in tcp_dis.c to 30 for testing all possible combinations 
 * of length, (less than MAXCHUNK, same as MAXCHUNK, larger than MAXCHUNK.  I could 
 * not find a way to overwrite 50000 with 30 for our unit
 * test even if I had defined MAXCHUNK in a header file and 
 * undefinining in this file and redefine it to 30.
*/

#ifndef MAXCHUNK
/* #define MAXCHUNK 30 */
#define MAXCHUNK 50000
#endif

char *setup_tcp_chan_for_test(struct tcp_chan *chan, const char *buf)
  {
  memset(chan, 0, sizeof(struct tcp_chan));

  chan->writebuf.tdis_bufsize = 100;
  chan->writebuf.tdis_thebuf = (char *) calloc(chan->writebuf.tdis_bufsize, sizeof(char));
  strcpy(chan->writebuf.tdis_thebuf, buf);
  chan->writebuf.tdis_leadp = chan->writebuf.tdis_thebuf + strlen(buf); /* not relevant to the unit test */
  chan->writebuf.tdis_trailp = chan->writebuf.tdis_thebuf + strlen(buf); 

  char *tempfilename = tempnam("/tmp", "test");
  fail_unless((tempfilename != NULL), "Failed to create a temporary filename");
  chan->sock = open(tempfilename, O_CREAT | O_WRONLY | O_TRUNC, 0777);
  fail_unless((chan->sock > 2), "Failed to create a temporary filename for write");

  return tempfilename;
  }

char *setupAndDoBreakup(const char *strToBreak)
 {
 struct tcp_chan chan;
 char *tempfilename = setup_tcp_chan_for_test(&chan, strToBreak);

 DIS_tcp_wflush(&chan);

 close(chan.sock);
 free(chan.writebuf.tdis_thebuf);
 return tempfilename;
}

/* 
 * Read back the file and make sure that they were broken in chunks of the right size
 */
void test_chunk_breakups(const char *filename, const char *str)
  {
  FILE *fp = fopen(filename, "r");
  fail_unless(fp != NULL, "unable to open written file with the breaks");

  char strToBreak[256];
  snprintf(strToBreak, sizeof(strToBreak), "%s", str);
  size_t len = strlen(strToBreak);
  char *buf = strToBreak;
  char saveChar;
  size_t processed_chars = 0;
  char readingBuf[MAXCHUNK + 10];

  int lineRead = 0;
  int nlines = len/MAXCHUNK + 1; 

  char *endBufStr = buf + strlen(buf);

  while (processed_chars < len)
    {
    char *x = fgets(readingBuf, sizeof(readingBuf), fp);
    fail_unless(x != NULL, "unexpected end of file");
    if (!x)
      break;

    size_t l = strlen(readingBuf);
    if (readingBuf[l - 1] == '\n')
      readingBuf[l - 1] = '\0';
    saveChar = '\0';
    if ((buf + MAXCHUNK) < endBufStr) 
      {
      saveChar = *(buf + MAXCHUNK);
      *(buf + MAXCHUNK) = '\0';
      }
    lineRead += 1;
    int r = strcmp(readingBuf, buf);
    processed_chars += strlen(buf);
    if (saveChar) 
      *(buf + MAXCHUNK) = saveChar;
    fail_unless(r == 0, "Did not write the chunks we expected", buf);
    buf += MAXCHUNK;
    }

    if (lineRead != nlines)
      {
      char err_buf[512];
      snprintf(err_buf, sizeof(err_buf), "Did not write the number of chunks we expected %d %d: %s", lineRead, nlines, str);
      fail(err_buf);
      }
  }

START_TEST(test_DIS_tcp_wflush_writing_chunks)
  {
  char buf_len_larger_than_chunk_max[] = "hello there, this is a test of writing a chunk of data. In this case, our chunk lenght is 30";
  char buf_len_larger_as_much_as_chunk_max[] = "Something of 30, only thirty ch";
  char buf_len_less_than_chunk_max[] = "shortly - less than 30";

  char *tempfilename = setupAndDoBreakup(buf_len_larger_than_chunk_max);
  test_chunk_breakups(tempfilename, buf_len_larger_than_chunk_max);
  unlink(tempfilename);

  tempfilename = setupAndDoBreakup(buf_len_larger_as_much_as_chunk_max);
  test_chunk_breakups(tempfilename, buf_len_larger_as_much_as_chunk_max);
  unlink(tempfilename);

  tempfilename = setupAndDoBreakup(buf_len_less_than_chunk_max);
  test_chunk_breakups(tempfilename, buf_len_less_than_chunk_max);
  unlink(tempfilename);
  }
END_TEST

Suite *tcp_dis_suite(void)
  {
  Suite *s = suite_create("tcp_dis_suite methods");
  TCase *tc_core = tcase_create("test_DIS_tcp_wflush_writing_chunks");
  tcase_add_test(tc_core, test_DIS_tcp_wflush_writing_chunks);
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
  sr = srunner_create(tcp_dis_suite());
  srunner_set_log(sr, "tcp_dis_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
