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
}

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

int apint_is_negative(const ApInt *ap) {
  return (int) ap->sign;
}

uint64_t apint_get_bits(const ApInt *ap, unsigned n) {
  uint64_t length = ap->len;
  if(n < length && n >= 0) {
    return ap->data[n];
  }
  return 0;
}

int apint_highest_bit_set(const ApInt *ap) {
  if (apint_is_zero(ap)) {
    return -1;
  }

  int msb = (ap->len - 1) * 64;
  uint64_t n = ap->data[ap->len-1];
  n = n / 2;
  while(n != 0UL) {
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
  if(apint_is_negative(a) ^ apint_is_negative(b)) {
    if(apint_compare(a, b) > 0) {
      return apint_sub(a, apint_negate(b)); // hope this doesnt cause leak
    }
    else if (apint_compare(a, b) < 0) {
      return apint_sub(b, apint_negate(a));
    }
  }

  ApInt * sum = apint_copy(a);
  ApInt * inc = apint_copy(b);
  
  int max_len = 0;
  if(sum->len > inc->len) {
    max_len = sum->len;
    inc->data = realloc(inc->data, max_len * sizeof(uint64_t));
    assert(inc->data);
  }
  else {
    max_len = inc->len;
    sum->data = realloc(sum->data, max_len * sizeof(uint64_t));
    assert(sum->data);
  }
  
  uint64_t carry = 0UL;
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


ApInt *apint_sub(const ApInt *a, const ApInt *b) {
  if(apint_is_negative(a) ^ apint_is_negative(b)) {
    if(apint_compare(a, b) > 0) {
      return apint_add(a, apint_negate(b)); // hope this doesnt cause leak
    }
    else if (apint_compare(a, b) < 0) {
      return apint_add(b, apint_negate(a));
    }
  }

  if(apint_compare(b, a) > 0) {
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
  
  uint64_t carry = 0;
  uint64_t * s_dat = sub->data;
  uint64_t * i_dat = dec->data;

  for(int i = 0; i < max_len; i++) {
    uint64_t count = 0;
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

  for(int i = max_len - 1; i >=0; i--) {
    if(sub->data[i] == 0UL) {
      sub->len -= 1;
    }
  }
  sub->data = realloc(sub->data, sub->len * sizeof(uint64_t));
  apint_destroy(dec);
  return sub;
}

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

  if((left->len > right->len) && both_pos |
     (left->len < right->len) && both_neg) {
    return 1;
  }
  else if((right->len > left->len) && both_pos |
	  (right->len < left->len) && both_neg) {
    return -1;
  }

  for(int i = left->len - 1; i >= 0; i--) {
    if((left->data[i] > right->data[i]) && both_pos |
       (left->data[i] < right->data[i]) && both_neg) {
      return 1;
    }
    if((right->data[i] > left->data[i]) && both_pos |
       (right->data[i] < left->data[i]) && both_neg) {
      return -1;
    }
  }
  return 0;
}

ApInt *apint_copy(const ApInt *ap) {
  ApInt * new = apint_create_from_u64(0UL);
  new->data = realloc(new->data, ap->len * sizeof(uint64_t));
  assert(new->data);
  new->len = ap->len;
  uint64_t * old = ap->data;
  uint64_t * now = new->data;
  for(int i = 0; (uint64_t) i < ap->len; i++) {
    *old = *now;
  }
  return new;  
}
