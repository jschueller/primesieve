///
/// @file  NextPrimes.cpp
///        Fill an array or a vector with primes. This class is
///        used by primesieve::iterator.
///
/// Copyright (C) 2018 Kim Walisch, <kim.walisch@gmail.com>
///
/// This file is distributed under the BSD License. See the COPYING
/// file in the top level directory.
///

#include <primesieve.hpp>
#include <primesieve/Erat.hpp>
#include <primesieve/NextPrimes.hpp>
#include <primesieve/pmath.hpp>
#include <primesieve/PreSieve.hpp>
#include <primesieve/SievingPrimes.hpp>

#include <stdint.h>
#include <algorithm>
#include <array>
#include <iterator>
#include <vector>

using namespace std;

namespace primesieve {

/// First 64 primes
const array<uint64_t, 64> NextPrimes::smallPrimes =
{
      2,   3,   5,   7,  11,  13,  17,  19,
     23,  29,  31,  37,  41,  43,  47,  53,
     59,  61,  67,  71,  73,  79,  83,  89,
     97, 101, 103, 107, 109, 113, 127, 131,
    137, 139, 149, 151, 157, 163, 167, 173,
    179, 181, 191, 193, 197, 199, 211, 223,
    227, 229, 233, 239, 241, 251, 257, 263,
    269, 271, 277, 281, 283, 293, 307, 311
};

/// Number of primes <= n
const array<uint8_t, 312> NextPrimes::primePi =
{
   0,  0,  1,  2,  2,  3,  3,  4,  4,  4,
   4,  5,  5,  6,  6,  6,  6,  7,  7,  8,
   8,  8,  8,  9,  9,  9,  9,  9,  9, 10,
  10, 11, 11, 11, 11, 11, 11, 12, 12, 12,
  12, 13, 13, 14, 14, 14, 14, 15, 15, 15,
  15, 15, 15, 16, 16, 16, 16, 16, 16, 17,
  17, 18, 18, 18, 18, 18, 18, 19, 19, 19,
  19, 20, 20, 21, 21, 21, 21, 21, 21, 22,
  22, 22, 22, 23, 23, 23, 23, 23, 23, 24,
  24, 24, 24, 24, 24, 24, 24, 25, 25, 25,
  25, 26, 26, 27, 27, 27, 27, 28, 28, 29,
  29, 29, 29, 30, 30, 30, 30, 30, 30, 30,
  30, 30, 30, 30, 30, 30, 30, 31, 31, 31,
  31, 32, 32, 32, 32, 32, 32, 33, 33, 34,
  34, 34, 34, 34, 34, 34, 34, 34, 34, 35,
  35, 36, 36, 36, 36, 36, 36, 37, 37, 37,
  37, 37, 37, 38, 38, 38, 38, 39, 39, 39,
  39, 39, 39, 40, 40, 40, 40, 40, 40, 41,
  41, 42, 42, 42, 42, 42, 42, 42, 42, 42,
  42, 43, 43, 44, 44, 44, 44, 45, 45, 46,
  46, 46, 46, 46, 46, 46, 46, 46, 46, 46,
  46, 47, 47, 47, 47, 47, 47, 47, 47, 47,
  47, 47, 47, 48, 48, 48, 48, 49, 49, 50,
  50, 50, 50, 51, 51, 51, 51, 51, 51, 52,
  52, 53, 53, 53, 53, 53, 53, 53, 53, 53,
  53, 54, 54, 54, 54, 54, 54, 55, 55, 55,
  55, 55, 55, 56, 56, 56, 56, 56, 56, 57,
  57, 58, 58, 58, 58, 58, 58, 59, 59, 59,
  59, 60, 60, 61, 61, 61, 61, 61, 61, 61,
  61, 61, 61, 62, 62, 62, 62, 62, 62, 62,
  62, 62, 62, 62, 62, 62, 62, 63, 63, 63,
  63, 64
};

NextPrimes::NextPrimes(uint64_t start, uint64_t stop) :
  Erat(start, stop),
  preSieve_(start, stop)
{ }

size_t NextPrimes::getStartIdx() const
{
  size_t startIdx = 0;

  if (start_ > 1)
    startIdx = primePi[start_ - 1];

  return startIdx;
}

size_t NextPrimes::getStopIdx() const
{
  size_t stopIdx = 0;

  if (stop_ < smallPrimes.back())
    stopIdx = primePi[stop_];
  else
    stopIdx = smallPrimes.size();

  return stopIdx;
}

void NextPrimes::init()
{
  // sieving is only used if stop > max(SmallPrime)
  uint64_t minStart = smallPrimes.back() + 1;
  uint64_t sieveSize = get_sieve_size();
  start_ = max(start_, minStart);

  Erat::init(start_, stop_, sieveSize, preSieve_);
  sievingPrimes_.init(this, preSieve_);
}

void NextPrimes::initSmallPrimes(vector<uint64_t>& primes)
{
  if (start_ > smallPrimes.back())
    return;

  size_t a = getStartIdx();
  size_t b = getStopIdx();

  copy(&smallPrimes[a],
       &smallPrimes[b],
       back_inserter(primes));
}

void NextPrimes::initSmallPrimes(uint64_t* primes, size_t* size)
{
  if (start_ > smallPrimes.back())
    return;

  size_t a = getStartIdx();
  size_t b = getStopIdx();
  *size = b - a;

  copy(&smallPrimes[a], &smallPrimes[b], primes);
}

bool NextPrimes::sieveSegment(vector<uint64_t>& primes)
{
  if (!isInit_)
  {
    isInit_ = true;
    size_t size = prime_count_approx(start_, stop_);
    primes.reserve(size);
    initSmallPrimes(primes);
    init();
    if (!primes.empty())
      return false;
  }

  if (!hasNextSegment())
  {
    finished_ = true;
    return false;
  }

  sieveSegment();
  return true;
}

bool NextPrimes::sieveSegment(uint64_t* primes, size_t* size)
{
  if (!isInit_)
  {
    isInit_ = true;
    initSmallPrimes(primes, size);
    init();
    if (*size > 0)
      return false;
  }

  if (!hasNextSegment())
  {
    *size = 1;
    primes[0] = ~0ull;
    finished_ = (primes[0] > stop_);
    return false;
  }

  sieveSegment();
  return true;
}

void NextPrimes::sieveSegment()
{
  sieveIdx_ = 0;
  low_ = segmentLow_;

  uint64_t high = min(segmentHigh_, stop_);
  uint64_t sqrtHigh = isqrt(high);

  if (!sievingPrime_)
    sievingPrime_ = sievingPrimes_.nextPrime();

  while (sievingPrime_ <= sqrtHigh)
  {
    addSievingPrime(sievingPrime_);
    sievingPrime_ = sievingPrimes_.nextPrime();
  }

  Erat::sieveSegment();
}

void NextPrimes::fill(vector<uint64_t>& primes)
{
  while (!finished())
  {
    if (sieveIdx_ >= sieveSize_)
      if (!sieveSegment(primes))
        continue;

    uint64_t bits = littleendian_cast<uint64_t>(&sieve_[sieveIdx_]);
    sieveIdx_ += 8;

    while (bits != 0)
      primes.push_back(getPrime(&bits, low_));

    low_ += 8 * 30;
  }
}

} // namespace
