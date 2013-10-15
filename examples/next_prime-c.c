/** @example next_prime-c.c
 *  Iterate over primes using C primesieve_iterator. */

#include <primesieve.h>
#include <stdio.h>

int main()
{
  primesieve_iterator pi;
  primesieve_init(&pi);

  uint64_t sum = 0;
  uint64_t prime = 0;

  // iterate over primes below 10^10
  while ((prime = primesieve_next(&pi)) < 10000000000ull)
    sum += prime;

  primesieve_free(&pi);
  printf("Sum of the primes below 10^10 = %llu\n", sum);
  return 0;
}
