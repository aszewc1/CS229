// aszewc1
// vwen2

/*
 * Arbitrary-precision integer data type
 */

#ifndef APINT_H
#define APINT_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Representation: the data field is a little-endian bitstring ---
 * data[0] is bits 0..63, data[1] is bits 64..127, etc.
 */
typedef struct {
  uint64_t * data;  // data array for int
  uint64_t len;     // length of data array
  uint8_t sign;     // sign flag for int
} ApInt;

/* Constructors and destructors */
ApInt *apint_create_from_u64(uint64_t val);
ApInt *apint_create_from_hex(const char *hex);
void apint_destroy(ApInt *ap);

/* Operations */
int apint_is_zero(const ApInt *ap);
int apint_is_negative(const ApInt *ap);
uint64_t apint_get_bits(const ApInt *ap, unsigned n);
int apint_highest_bit_set(const ApInt *ap);
char *apint_format_as_hex(const ApInt *ap);
ApInt *apint_negate(const ApInt *ap);
ApInt *apint_add(const ApInt *a, const ApInt *b);
ApInt *apint_sub(const ApInt *a, const ApInt *b);
int apint_compare(const ApInt *left, const ApInt *right);
ApInt *apint_copy(const ApInt *ap);
#ifdef __cplusplus
}
#endif

#endif /* APINT_H */
