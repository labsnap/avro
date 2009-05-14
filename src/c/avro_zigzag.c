#include "avro.h"

/* TODO: add avro_int32 */

static avro_status_t
avro_int64_encode (AVRO * avro, int64_t * lp)
{
  avro_status_t status;
  int64_t n = *lp;
  uint8_t b;

  /* move sign to low-order bit */
  n = (n << 1) ^ (n >> 63);
  while ((n & ~0x7F) != 0)
    {
      b = ((((uint8_t) n) & 0x7F) | 0x80);
      status = AVRO_PUTBYTES (avro, (caddr_t) & b, 1);
      CHECK_ERROR (status);
      n >>= 7;
    }
  b = (uint8_t) n;
  return AVRO_PUTBYTES (avro, (caddr_t) & b, 1);
}

static avro_status_t
avro_int64_decode (AVRO * avro, int64_t * lp)
{
  avro_status_t status;
  int64_t value = 0;
  int offset = 0;
  uint8_t b;
  const int MAX_VARINT_BUF_SIZE = 10;
  do
    {
      if (offset == MAX_VARINT_BUF_SIZE)
	{
	  return AVRO_FAILURE;
	}
      status = AVRO_GETBYTES (avro, &b, 1);
      CHECK_ERROR (status);
      value |= (int64_t) (b & 0x7F) << (7 * offset);
      ++offset;
    }
  while (b & 0x80);
  /* back to two's-complement value; */
  *lp = (value >> 1) ^ -(value & 1);
  return AVRO_OK;
}

/**
* Function for encoding/decoding 64-bit signed integers
* @param avro An initialized avro handle
* @param lp Pointer to a 64-bit signed integer
*/
avro_status_t
avro_int64 (AVRO * avro, int64_t * lp)
{
  if (!avro || !lp)
    {
      return AVRO_FAILURE;
    }
  switch (avro->a_op)
    {
    case AVRO_ENCODE:
      return avro_int64_encode (avro, lp);
    case AVRO_DECODE:
      return avro_int64_decode (avro, lp);
    default:
      return AVRO_FAILURE;
    }
  return AVRO_OK;
}
