
#include <stdlib.h>

/* let's write some "effective" algortihm. */
long fib(int n)
  {
  switch (n)
    {

    case 0:

    case 1:
      return n;

    default:
      return fib(n -1) + fib(n - 2);
    }
  }

int main(int argc, char *argv[])
  {
  fib(atoi(argv[1]));
  exit(0);
  }

