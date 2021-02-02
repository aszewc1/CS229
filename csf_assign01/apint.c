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

/*Constructor given uint64_t value */
ApInt *apint_create_from_u64(uint64_t val) {
  ApInt * ap = malloc(sizeof(ApInt)); //allocates memory
  assert(ap);
  ap->data = malloc(sizeof(uint64_t)); //allocates memory for data values
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

/* Deconstructor */
void apint_destroy(ApInt *ap) {
  free(ap->data); // frees data
  free(ap);
}

/* checks if data is zero.
*if true, return 1, else return 0
*/
int apint_is_zero(const ApInt *ap) {
  uint64_t * curr = ap->data;
  for(int i = 0; (uint64_t) i < ap->len; i++) {
    if(*curr != 0UL) {
      return 0;
    }
    curr++;
  }
  return 1;
}

/* returns 0 if non-negative, 1 if negative */
int apint_is_negative(const ApInt *ap) {
  return (int) ap->sign;
}


uint64_t apint_get_bits(const ApInt *ap, unsigned n) {
  uint64_t length = ap->len;
  uint64_t * bits = ap->data;
  if(n < length) {
    return bits[n];
  }
  return 0;
}

/* searches for most significant bit with 1*/
int apint_highest_bit_set(const ApInt *ap) {
  //base case: if zero, there are no significant 1 bits
  if (apint_is_zero(ap)) {
    return -1;
  }

  int msb = (ap->len - 1) * 64;
  uint64_t n = ap->data[ap->len-1];
  n = n / 2;
  /* Loops to find most significant bit
  *when n=0 loop ends; if n < 2, as integer division rounds down
  */
  while(n != 0UL) {
    n = n / 2;
    msb++;
  }

  return msb; //returns location of significant bit
  
}

/* formats uint64_t data into hex*/
char *apint_format_as_hex(const ApInt *ap) {
	/* TODO: implement */
	assert(0);
	return NULL;
}

/* Negates given data and switches signs*/
ApInt *apint_negate(const ApInt *ap) {
  ApInt * neg = apint_copy(ap);
  neg->sign = (ap->sign > 0) ? 0 : 1;
  if (apint_is_zero(ap)) { neg->sign = ap->sign; }
  return neg;
}

/* Method to add two uint64_t data values*/
ApInt *apint_add(const ApInt *a, const ApInt *b) {
  if(apint_is_negative(a) ^ apint_is_negative(b)) { // bitwise XOR
    if(apint_compare(a, b) > 0) { //case where a > b
      ApInt * temp = apint_negate(b);
      ApInt * ret = apint_sub(a, temp);
      apint_destroy(temp);
      return ret;
    }
    else if (apint_compare(a, b) < 0) { //case where a < b
      ApInt * temp = apint_negate(a);
      ApInt * ret = apint_sub(b, temp);
      apint_destroy(temp);
      return ret;
    }
  }

  ApInt * sum = apint_copy(a);
  ApInt * inc = apint_copy(b);
  
  int max_len = 0;
  if(sum->len > inc->len) {
    max_len = sum->len;
    inc->data = realloc(inc->data, max_len * sizeof(uint64_t)); //resizes memory of data
    assert(inc->data);
  }
  else {
    max_len = inc->len;
    sum->data = realloc(sum->data, max_len * sizeof(uint64_t));//resizes memory of data
    assert(sum->data);
  }
	
  uint64_t carry = 0UL; //bit carried over during add operation
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
    sum->data[sum->len] += carry;
    sum->len += 1;
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

  if(
     ((apint_compare(b, a) > 0)
      && !apint_is_negative(a)
      && !apint_is_negative(b)) ||
     ((apint_compare(a, b) > 0)
      && apint_is_negative(a)
      && apint_is_negative(b))) {
    return apint_negate(apint_sub(b, a));
  }

  ApInt * sub = apint_copy(a);
  ApInt * dec = apint_copy(b);
  
  int max_len = 0;
  if(sub->len > dec->len) {
    max_len = sub->len;
    dec-> data = realloc(dec->data, max_len * sizeof(uint64_t));
    assert(dec->data);
  }
  else {
    max_len = dec->len;
    sub->data = realloc(sub->data, max_len * sizeof(uint64_t));
    assert(sub->data);
  }
  
  uint64_t carry = 0UL; //bit carried over during operation
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
      sub->len -= 1;
    }
  }
  sub->data = realloc(sub->data, sub->len * sizeof(uint64_t));
  apint_destroy(dec);
  return sub;
}

/* Compares 2 given data values 
*if result is less than 1, *right is greater than * left.
*if result is greater than 1, *left is greater than *right.
*if result is zero, returns 0.
*/
int apint_compare(const ApInt *left, const ApInt *right) {
  int left_neg = apint_is_negative(left);
  int righ_neg = apint_is_negative(right);
  int both_pos = !left_neg && !righ_neg;
  int both_neg = left_neg && righ_neg;

  if(left_neg && !righ_neg) {
    return -1;
  }
  else if(!left_neg && righ_neg) {
    return 1;
  }
  
  /* uses length to determine which is larger. Value with greater length is larger*/
  if(((left->len > right->len) && both_pos) ||
     ((left->len < right->len) && both_neg)) {
    return 1;
  }
  else if(((right->len > left->len) && both_pos) ||
	  ((right->len < left->len) && both_neg)) {
    return -1;
  }

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

/* method to clone/copy data values*/
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
