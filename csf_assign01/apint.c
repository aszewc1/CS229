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

/* Constructor given uint64_t value */
ApInt *apint_create_from_u64(uint64_t val) {
  ApInt * ap = malloc(sizeof(ApInt)); // allocates memory
  assert(ap);
  ap->data = malloc(sizeof(uint64_t)); // allocates memory for data values
  assert(ap->data);
  *(ap->data) = val;
  ap->len = 1;
  ap->sign = 0;
  return ap;
}

/* Constructor given hex value */
ApInt *apint_create_from_hex(const char *hex) {
  /* TODO: implement */
  assert(0);
  return NULL;
}

/* Deconstructor to free dynamically allocated memory */
void apint_destroy(ApInt *ap) {
  free(ap->data); // frees data
  free(ap);
}

/* checks if all data are zero.
 * if true, return 1, else return 0
 */
int apint_is_zero(const ApInt *ap) {
  for(int i = 0; (uint64_t) i < ap->len; i++) {
    if(ap->data[i] != 0UL) {
      return 0;
    }
  }
  return 1;
}

/* returns 0 if non-negative, 1 if negative */
int apint_is_negative(const ApInt *ap) {
  return (int) ap->sign;
}

/* returns bits of given index in data array */
uint64_t apint_get_bits(const ApInt *ap, unsigned n) {
  if(n < ap->len) {
    return ap->data[n];
  }
  return 0;
}

/* searches for most significant set bit index of binary */
int apint_highest_bit_set(const ApInt *ap) {
  // if zero, there are no set bits
  if (apint_is_zero(ap)) {
    return -1;
  }

  int msb = (ap->len - 1) * 64;
  uint64_t n = ap->data[ap->len-1];
  n = n / 2;
  // loop to find msb ends when n==0
  while(n != 0UL) {
    n = n / 2;
    msb++;
  }

  return msb; // returns index of highest set bit
  
}

/* Format uint64_t data into hex*/
char *apint_format_as_hex(const ApInt *ap) {
	/* TODO: implement */
	assert(0);
	return NULL;
}

/* Negates given data by switching sign in copy */
ApInt *apint_negate(const ApInt *ap) {
  ApInt * neg = apint_copy(ap);
  neg->sign = (ap->sign > 0) ? 0 : 1;
  if (apint_is_zero(ap)) { neg->sign = ap->sign; }
  return neg;
}

/* Method to add two uint64_t data values
 * Depending on signs, sub method may be called
 */
ApInt *apint_add(const ApInt *a, const ApInt *b) {
  if(apint_is_negative(a) ^ apint_is_negative(b)) { // only one negative value
    if(apint_compare(a, b) > 0) { // b is negative
      ApInt * temp = apint_negate(b);
      ApInt * ret = apint_sub(a, temp);
      apint_destroy(temp);
      return ret;
    }
    else if (apint_compare(a, b) < 0) { // a is negative
      ApInt * temp = apint_negate(a);
      ApInt * ret = apint_sub(b, temp);
      apint_destroy(temp);
      return ret;
    }
  }

  ApInt * sum = apint_copy(a);
  ApInt * inc = apint_copy(b);
  // ensure both objects have same data length by resizing
  int max_len = apint_resize(sum, inc);
	
  uint64_t carry = 0UL; // value carried over during addition
  uint64_t * s_dat = sum->data;
  uint64_t * i_dat = inc->data;

  for(int i = 0; i < max_len; i++) {
    uint64_t count = 0UL;
    if(*s_dat + carry < *s_dat) {
      count++;
    }
    *s_dat += carry;
    if (*s_dat + *i_dat < *s_dat) {
      count++;
    }
    *s_dat += *i_dat;
    carry = count;
    s_dat++; i_dat++;
  }

  if (carry != 0) {
    sum->data = realloc(sum->data, (sum->len + 1) * sizeof(uint64_t));
    sum->data[sum->len] = carry;
    sum->len += 1; // expansion of length as appropriate
  }
  
  apint_destroy(inc);
  return sum;
}

/* Method to subtract 2 data values */
ApInt *apint_sub(const ApInt *a, const ApInt *b) {
  if(apint_is_negative(a) ^ apint_is_negative(b)) { // only one number negative
    if(apint_compare(a, b) > 0) { // case where b is negative
      ApInt * temp = apint_negate(b);
      ApInt * ret = apint_add(a, temp);
      apint_destroy(temp);
      return ret;
    }
    else if (apint_compare(a, b) < 0) { // case where a is negative
      ApInt * temp = apint_negate(a);
      ApInt * ret = apint_add(b, temp);
      apint_destroy(temp);
      return ret;
    }
  }

  if(((apint_compare(b, a) > 0)
      && !apint_is_negative(a) && !apint_is_negative(b)) ||
     ((apint_compare(a, b) > 0)
      && apint_is_negative(a) && apint_is_negative(b))) {
    ApInt * temp = apint_sub(b, a);
    ApInt * ret = apint_negate(temp);
    apint_destroy(temp);
    return ret;
  }

  ApInt * sub = apint_copy(a);
  ApInt * dec = apint_copy(b);
  
  int max_len = apint_resize(sub, dec);
  
  uint64_t carry = 0UL; // value carried over during subtraction
  uint64_t * s_dat = sub->data;
  uint64_t * i_dat = dec->data;

  for(int i = 0; i < max_len; i++) {
    uint64_t count = 0UL;
    if(*s_dat - carry > *s_dat) {
      count++;
    }
    *s_dat -= carry;
    if (*s_dat - *i_dat > *s_dat) {
      count++;
    }
    *s_dat -= *i_dat;
    carry = count;
    s_dat++; i_dat++;
  }

  for(int i = max_len - 1; i > 0; i--) {
    if(sub->data[i] == 0UL) {
      sub->len -= 1; // reduction of length as appropriate
    }
  }
  sub->data = realloc(sub->data, sub->len * sizeof(uint64_t));
  apint_destroy(dec);
  return sub;
}

/* Compares given data values
 * if result is less than 1, right is greater than left.
 * if result is greater than 1, left is greater than right.
 * if data are equivalent, returns 0.
 */
int apint_compare(const ApInt *left, const ApInt *right) {
  int left_neg = apint_is_negative(left);
  int righ_neg = apint_is_negative(right);
  int both_pos = !left_neg && !righ_neg;
  int both_neg = left_neg && righ_neg;

  // this method checks sign, length, and then value in order
  // to minimize runtime

  // checking sign
  if(left_neg && !righ_neg) {
    return -1;
  }
  else if(!left_neg && righ_neg) {
    return 1;
  }
  
  // checks length with sign
  if(((left->len > right->len) && both_pos) ||
     ((left->len < right->len) && both_neg)) {
    return 1;
  }
  else if(((right->len > left->len) && both_pos) ||
	  ((right->len < left->len) && both_neg)) {
    return -1;
  }

  // if lengths same, checks values
  for(int i = left->len - 1; i >= 0; i--) {
    if(((left->data[i] > right->data[i]) && both_pos) ||
       ((left->data[i] < right->data[i]) && both_neg)) {
      return 1;
    }
    if(((right->data[i] > left->data[i]) && both_pos) ||
       ((right->data[i] < left->data[i]) && both_neg)) {
      return -1;
    }
  }
  return 0;
}

/* method to duplicate ApInt object */
ApInt *apint_copy(const ApInt *ap) {
  ApInt * new = apint_create_from_u64(0UL);
  new->data = realloc(new->data, ap->len * sizeof(uint64_t));
  assert(new->data);
  new->len = ap->len;
  uint64_t * old = ap->data;
  uint64_t * now = new->data;
  for(int i = 0; (uint64_t) i < ap->len; i++) {
    now[i] = old[i];
  }
  return new;  
}

/* resizes apints to equivalent length */
int apint_resize(ApInt *a, ApInt *b) {
  if(a->len > b->len) {
    b->data = realloc(b->data, a->len * sizeof(uint64_t));
    assert(b->data);
    b->len = a->len;
    return a->len;
  }

  a->data = realloc(a->data, b->len * sizeof(uint64_t));
  assert(a->data);
  a->len = b->len;
  return b->len;
}
