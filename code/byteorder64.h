/*
  Byte order for 64-bit integers in memory.

  Saleem Bhatti <https://saleem.host.cs.st-andrews.ac.uk/>
  January 2022
  January 2021
*/

/*
  There does not appear to be a standard endian check mechanism,
  or a standard host-to-network / network-to-host API for dealing
  with 64-bit integer values.

  Linux does have a set of functions in:

    #include <endian.h>
    man 3 endian

  but these are not POSIX compliant.

  So, some examples of how to check/convert 64-bit value are below.

  This works for AMD64, Intel64, and Raspberry-Pi/ARM-Cortex-72.

  I did not have a real little-endian machine to check this with!
*/


#ifndef __byteorder64_h__
#define __byteorder64_h__

#include <inttypes.h>

uint64_t hton64(uint64_t v64); /* host to network byte order */
uint64_t ntoh64(uint64_t v64); /* network to host byte order */

#endif /* __byteorder64_h__ */
