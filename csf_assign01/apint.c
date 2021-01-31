// aszewc1
// vwen2

/*
 * CSF Assignment 1
 * Arbitrary-precision integer data type
 * Function implementations
 */

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "apint.h"

ApInt *apint_create_from_u64(uint64_t val) {
  ApInt * new = malloc(sizeof(ApInt));
  assert(new);
  new->data = malloc(sizeof(uint64_t));
  assert(new->data);
  *(new->data) = val;
  new->len = 1;
  new->sign = 0;
  return new;
}

ApInt *apint_create_from_hex(const char *hex) {
	/* TODO: implement */
	assert(0);
	return NULL;
}

void apint_destroy(ApInt *ap) {
  free(ap->data);
  free(ap);
  assert(0);
}

int apint_is_zero(const ApInt *ap) {
  uint64_t * curr = ap->data;
  for (int i = 0; i < ap->len; i++) {
    if (*curr != 0UL) {
      return 0;
    }
    curr++;
  }
  return 1;
}

int apint_is_negative(const ApInt *ap) {
  return (int) sign;
}

uint64_t apint_get_bits(const ApInt *ap, unsigned n) {
  if (n < ap->len) {
    return ap->data[n-1];
  }
  return 0;
}

int apint_highest_bit_set(const ApInt *ap) {
  if (apint_is_zero(ap)) {
    return -1;
  }

  int msb = (len - 1) * 64;
  uint64_t n = ap->data[len-1];
  n = n / 2;
  while (n != 0UL) {
    n = n / 2;
    msb++;
  }

  return msb;
  
}

char *apint_format_as_hex(const ApInt *ap) {
	/* TODO: implement */
	assert(0);
	return NULL;
}

ApInt *apint_negate(const ApInt *ap) {
  ApInt * neg = apint_copy(ap);
  neg->sign = 1;
  return neg;
}

ApInt *apint_add(const ApInt *a, const ApInt *b) {
  ApInt * sum = apint_copy(a);
  ApInt * inc = apint_copy(b);
  if(apint_is_negative(a) ^ apint_is_negative(b)) {
    if (apint_compare(a, b) > 0) {
    }
    else if (apint_compare(a, b) < 0) {
    }
    else {
      return sum;
    }
  }
  
}


ApInt *apint_sub(const ApInt *a, const ApInt *b) {
	/* TODO: implement */
	assert(0);
	return NULL;
}

int apint_compare(const ApInt *left, const ApInt *right) {
	/* TODO: implement */
	assert(0);
	return 0;
}

ApInt *apint_copy(const ApInt *ap) {
  ApInt * new = apint_create_from_u64(0UL);
  realloc(neg, ap->len * sizeof(uint64_t));
  uint64_t * old = ap->data;
  uint64_t * new = new->data;
  for (int i = 0; i < ap->len; i++) {
    *old = *new;
  }
  return new;  
}
