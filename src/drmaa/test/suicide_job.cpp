
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <stdlib.h>

int main(int argc, char *argv[])
  {
  kill(getpid(), atoi(argv[1]));
  exit(1);
  }


