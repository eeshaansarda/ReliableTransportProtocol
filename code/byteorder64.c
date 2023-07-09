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

#ifdef DEBUG
#include <stdio.h>
#endif

#include <inttypes.h>
#include <arpa/inet.h>

#define FOUND_LITTLE_ENDIAN 0
#define FOUND_BIG_ENDIAN    1

/* comment the line below to use bit-fiddling instead of struct/memory */
#define USE_UNION

#ifdef USE_UNION

typedef union uint_32_4x8_u {
  uint32_t v32;
  uint8_t  v8[4];
} uint_32_4x8_t;

typedef union uint_64_2x32_u {
  uint64_t v64;
  uint32_t v32[2];
} uint_64_2x32_t;

int
endian() /* check memory contents for a known value to check endianness */
{
  uint_32_4x8_t v;
  int e = -1;

  v.v32 = 0xabcd1234;

#if defined(DEBUG)
  printf("\n** endian() check on 32-bit word ...\n");
  printf("**   v32: 0x%08"PRIx32"\n", v.v32);
  printf("** v8[0]: 0x%02"PRIx8"\n", v.v8[0]);
  printf("** v8[1]: 0x%02"PRIx8"\n", v.v8[1]);
  printf("** v8[2]: 0x%02"PRIx8"\n", v.v8[2]);
  printf("** v8[3]: 0x%02"PRIx8"\n", v.v8[3]);
#endif

  /* little-endian does not have bytes as written above, v = 0xabcd1234 */
  /* really, checking one of these bytes is probably enough */
  if ( (uint8_t) (((v.v32 & 0xff000000) >> 24) == v.v8[0]) &&
       (uint8_t) (((v.v32 & 0x00ff0000) >> 16) == v.v8[1]) &&
       (uint8_t) (((v.v32 & 0x0000ff00) >>  8) == v.v8[2]) &&
       (uint8_t) (( v.v32 & 0x000000ff       ) == v.v8[3])
     ) {
#if defined(DEBUG)
    printf("** big-endian **\n");
#endif
    e = FOUND_BIG_ENDIAN;
  }
  else {
#if defined(DEBUG)
    printf("** little-endian **\n");
#endif
    e = FOUND_LITTLE_ENDIAN;
  }
#if defined(DEBUG)
  printf("** endian() check finished.\n\n");
#endif

  return e;
}


uint64_t
hton64(uint64_t v64)
{
  if (endian() == FOUND_BIG_ENDIAN) return v64;

  uint_64_2x32_t v;
  uint32_t v32tmp;

  v.v64 = v64;
  v.v32[0] = htonl(v.v32[0]); // reorder bytes in 32-bit word
  v.v32[1] = htonl(v.v32[1]); // reorder bytes in 32-bit word

  /* reorder 32-bit words */
  v32tmp = v.v32[0];
  v.v32[0] = v.v32[1];
  v.v32[1] = v32tmp;

  return v.v64;
}

uint64_t
ntoh64(uint64_t v64)
{
  if (endian() == FOUND_BIG_ENDIAN) return v64;

  uint_64_2x32_t v;
  uint32_t v32tmp;

  v.v64 = v64;
  v.v32[0] = ntohl(v.v32[0]); // reorder bytes in 32-bit word
  v.v32[1] = ntohl(v.v32[1]); // reorder bytes in 32-bit word

  /* reorder 32-bit words */
  v32tmp = v.v32[0];
  v.v32[0] = v.v32[1];
  v.v32[1] = v32tmp;

  return v.v64;
}


#else /* USE_UNION : use bit-fillding insetad of union */


int
endian() /* check memory contents for a known value to check endianness */
{
  uint32_t v32 = 0xabcd1234;
  int e = -1;
  unsigned char *v32p = (unsigned char *) &v32;
  /* little-endian does not have bytes as written above, v = 0xabcd1234 */
  uint16_t hi16 = ((((uint16_t) v32p[0]) << 8) & 0xff00) |
                   (((uint16_t) v32p[1])       & 0x00ff);
  uint16_t lo16 = ((((uint16_t) v32p[2]) << 8) & 0xff00) |
                   (((uint16_t) v32p[3])       & 0x00ff);

#if defined(DEBUG)
  printf("\n** endian() check on 32-bit word ...\n");
  printf("**  v32: 0x%08"PRIx32"\n", v32);
  printf("** lo16: 0x%04"PRIx16"\n", lo16);
  printf("** hi16: 0x%04"PRIx16"\n", hi16);
#endif
  if (hi16 == 0xabcd && lo16 == 0x1234) {
#if defined(DEBUG)
    printf("** big-endian **\n");
#endif
    e = FOUND_BIG_ENDIAN;
  }
  else {
#if defined(DEBUG)
    printf("** little-endian **\n");
#endif
    e = FOUND_LITTLE_ENDIAN;
  }
#if defined(DEBUG)
  printf("** endian() check finished.\n");
#endif

  return e;
}

#define SPLIT_64_2x32(v64_, lo32_, hi32_) \
  lo32_ = (uint32_t) ( v64_ & 0x00000000ffffffff); \
  hi32_ = (uint32_t) ((v64_ & 0xffffffff00000000) >> 32)

#define COMBINE_2x32_64(lo32_, hi32_, v64_) \
  v64_ = ((((uint64_t) hi32_) << 32) & 0xffffffff00000000) | (((uint64_t) lo32_) & 0x00000000ffffffff)

uint64_t
hton64(uint64_t v64)
{
  if (endian() == FOUND_BIG_ENDIAN) return v64;

  uint32_t lo32, hi32;
  SPLIT_64_2x32(v64, lo32, hi32);
  lo32 = htonl(lo32); // reorder bytes in 32-bit word
  hi32 = htonl(hi32); // reorder bytes in 32-bit word
  COMBINE_2x32_64(hi32, lo32, v64); // re-order 32-bit words

  return v64;
}

uint64_t
ntoh64(uint64_t v64)
{
  if (endian() == FOUND_BIG_ENDIAN) return v64;

  uint32_t lo32, hi32;
  SPLIT_64_2x32(v64, lo32, hi32);
  lo32 = ntohl(lo32); // reorder bytes in 32-bit word
  hi32 = ntohl(hi32); // reorder bytes in 32-bit word
  COMBINE_2x32_64(hi32, lo32, v64); // re-order 32-bit words

  return v64;
}

#endif /* USE_UNION */
