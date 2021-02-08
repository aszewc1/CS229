// aszewc1
// vwen2

/*
 * Unit tests for arbitrary-precision integer data type
 *
 * These tests are by no means comprehensive.  You will need to
 * add more tests of your own!  In particular, make sure that
 * you have tests for more challenging situations, such as
 *
 * - large values
 * - adding/subtracting/comparing values with different lengths
 * - special cases (carries when adding, borrows when subtracting, etc.)
 * - etc.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "apint.h"
#include "tctest.h"

typedef struct {
  ApInt *ap0;
  ApInt *ap1;
  ApInt *ap110660361;
  ApInt *max1;
  ApInt *minus1;
  ApInt *ap11204;
  ApInt *apNA;
  ApInt *apSUB;
  ApInt *apCARRY;
  ApInt *apADD;
  ApInt *apHex20;
  ApInt *apHex174281552;
  ApInt *apHex0;
  ApInt *apHex6694288348987732;
  ApInt *apHexLarge1;
  ApInt *apHexNeg;
} TestObjs;

TestObjs *setup(void);
void cleanup(TestObjs *objs);

void testCreateFromU64(TestObjs *objs);
void testCreateFromHex(TestObjs *objs);
void testHighestBitSet(TestObjs *objs);
void testCompare(TestObjs *objs);
void testAddNoHex(TestObjs *objs);
void testSubNoHex(TestObjs *objs);
void testFormatAsHex(TestObjs *objs);
void testAdd(TestObjs *objs);
void testSub(TestObjs *objs);
void testlShift(TestObjs *objs);

int main(int argc, char **argv) {
	TEST_INIT();

	if (argc > 1) {
		/*
		 * name of specific test case to execute was provided
		 * as a command line argument
		 */
		tctest_testname_to_execute = argv[1];
	}

	TEST(testCreateFromU64); // tests for negative & zero throughout
	TEST(testCreateFromHex);
	TEST(testHighestBitSet);
	TEST(testCompare);
	TEST(testAddNoHex);
	TEST(testSubNoHex);
	TEST(testFormatAsHex);
	TEST(testAdd);
	TEST(testSub);
	TEST(testlShift);

	TEST_FINI();
}

TestObjs *setup(void) {
  TestObjs *objs = malloc(sizeof(TestObjs));
  objs->ap0 = apint_create_from_u64(0UL);
  objs->ap1 = apint_create_from_u64(1UL);
  objs->ap110660361 = apint_create_from_u64(110660361UL);
  objs->max1 = apint_create_from_u64(0xFFFFFFFFFFFFFFFFUL);
  objs->minus1 = apint_negate(objs->ap1);
  objs->ap11204 = apint_create_from_u64(11204UL);
  objs->apNA = apint_negate(objs->max1);
  objs->apSUB = apint_sub(objs->max1, objs->max1);
  objs->apCARRY = apint_create_from_u64(99999999UL);
  objs->apADD = apint_add(objs->apCARRY, objs->apCARRY);
  objs->apHex20 = apint_create_from_hex("14");
  objs->apHex174281552 = apint_create_from_hex("174281552");
  objs->apHex0 = apint_create_from_hex("0");
  objs->apHex6694288348987732 = apint_create_from_hex("17C86B7710C154");
  objs->apHexLarge1 = apint_create_from_hex("7FFFFFFFFFFFFFFFFFEE1");
  objs->apHexNeg = apint_create_from_hex("-255D566DE");
  return objs;
}

void cleanup(TestObjs *objs) {
  apint_destroy(objs->ap0);
  apint_destroy(objs->ap1);
  apint_destroy(objs->ap110660361);
  apint_destroy(objs->max1);
  apint_destroy(objs->minus1);
  apint_destroy(objs->ap11204);
  apint_destroy(objs->apNA);
  apint_destroy(objs->apSUB);
  apint_destroy(objs->apCARRY);
  apint_destroy(objs->apADD);
  apint_destroy(objs->apHex20);
  apint_destroy(objs->apHex174281552);
  apint_destroy(objs->apHex0);
  apint_destroy(objs->apHex6694288348987732);
  apint_destroy(objs->apHexLarge1);
  apint_destroy(objs->apHexNeg);
  free(objs);
}

void testCreateFromU64(TestObjs *objs) {
  ASSERT(0UL == apint_get_bits(objs->ap0, 0));
  ASSERT(1 == apint_is_zero(objs->ap0));
  ASSERT(1UL == apint_get_bits(objs->ap1, 0));
  ASSERT(110660361UL == apint_get_bits(objs->ap110660361, 0));
  ASSERT(0xFFFFFFFFFFFFFFFFUL == apint_get_bits(objs->max1, 0));
  ASSERT(0 == apint_is_negative(objs->max1));
  ASSERT(11204UL == apint_get_bits(objs->ap11204, 0));
  ASSERT(0xFFFFFFFFFFFFFFFFUL == apint_get_bits(objs->apNA, 0));
  ASSERT(1 == apint_is_negative(objs->apNA));
  ASSERT(0UL == apint_get_bits(objs->apSUB, 0));
  ASSERT(1 == apint_is_zero(objs->apSUB));
  ASSERT(99999999UL == apint_get_bits(objs->apCARRY, 0));
  ASSERT(199999998UL == apint_get_bits(objs->apADD, 0));
}
void testCreateFromHex(TestObjs * objs) {
  ApInt *a;
  char *s;
  ASSERT(20UL == apint_get_bits(objs->apHex20, 0));
  ASSERT(6243751250ULL == apint_get_bits(objs->apHex174281552, 0));
  ASSERT(0 == apint_get_bits(objs->apHex0, 0));
  ASSERT(apint_is_zero(objs->apHex0));
  ASSERT(!apint_is_negative(objs->apHex0));
  ASSERT(6694288348987732UL == apint_get_bits(objs->apHex6694288348987732, 0));
  ASSERT(10029983454UL == apint_get_bits(objs->apHexNeg, 0));
  ASSERT(apint_is_negative(objs->apHexNeg));

  a = apint_create_from_hex("-0000");
  ASSERT(0 == apint_get_bits(a, 0));
  ASSERT(!apint_is_negative(a));
  ASSERT(apint_is_zero(a));
  apint_destroy(a);

  a = apint_create_from_hex("-F");
  ASSERT(15 == apint_get_bits(a, 0));
  ASSERT(apint_is_negative(a));
  apint_destroy(a);
  
  a = apint_create_from_hex("-0000101");
  ASSERT(257 == apint_get_bits(a, 0));
  ASSERT(apint_is_negative(a));
  ASSERT(!apint_is_zero(a));
  apint_destroy(a);

  a = apint_create_from_hex("-17c86B7710C154"); // mixed cases
  ASSERT(6694288348987732 == apint_get_bits(a, 0));
  ASSERT(apint_is_negative(a));
  apint_destroy(a);

  a = apint_create_from_hex("-7FfFffFffFfFfffFfFeE1");
  ASSERT(strcmp("-7fffffffffffffffffee1", (s = apint_format_as_hex(a))) == 0);
  ASSERT(18446744073709551329UL == apint_get_bits(a, 0));
  ASSERT(524287UL == apint_get_bits(a, 1));
  apint_destroy(a);
  free(s);
  
  ASSERT(18446744073709551329UL == apint_get_bits(objs->apHexLarge1, 0));
}

void testHighestBitSet(TestObjs *objs) {
  ASSERT(-1 == apint_highest_bit_set(objs->ap0));
  ASSERT(0 == apint_highest_bit_set(objs->ap1));
  ASSERT(26 == apint_highest_bit_set(objs->ap110660361));
  ASSERT(63 == apint_highest_bit_set(objs->max1));
  ASSERT(13 == apint_highest_bit_set(objs->ap11204));
  ASSERT(63 == apint_highest_bit_set(objs->apNA));
  ASSERT(-1 == apint_highest_bit_set(objs->apSUB));
  ASSERT(26 == apint_highest_bit_set(objs->apCARRY));
  ASSERT(27 == apint_highest_bit_set(objs->apADD));
}

void testCompare(TestObjs *objs) {
  //ApInt *a, *b;
  /* 1 > 0 */
  ASSERT(apint_compare(objs->ap1, objs->ap0) > 0);
  /* 0 < 1 */
  ASSERT(apint_compare(objs->ap0, objs->ap1) < 0);
  /* 110660361 > 0 */
  ASSERT(apint_compare(objs->ap110660361, objs->ap0) > 0);
  /* 110660361 > 1 */
  ASSERT(apint_compare(objs->ap110660361, objs->ap1) > 0);
  /* 0 < 110660361 */
  ASSERT(apint_compare(objs->ap0, objs->ap110660361) < 0);
  /* 1 < 110660361 */
  ASSERT(apint_compare(objs->ap1, objs->ap110660361) < 0);

  ASSERT(apint_compare(objs->ap0, objs->apHex0) == 0);  
   
   /* larger cases */
   ApInt *a = apint_create_from_hex("5b6a7d127b4007e7b95a2e0b6f9c281ebafc45911b348a28015d811d335b5909c939f2b6986e409f");
   ApInt *b = apint_create_from_hex("3d6ab25858932");
   ASSERT(apint_compare(a, b) > 0);
   apint_destroy(b);
   apint_destroy(a);
   
}

void testAddNoHex(TestObjs *objs) {
  ApInt *sum;

  /* 1 = 1 */
  sum = apint_add(objs->ap1, objs->ap0);
  ASSERT(apint_compare(objs->ap1, sum) == 0);
  apint_destroy(sum);

  /* 0 = 0 */
  sum = apint_add(objs->apNA, objs->max1);
  ASSERT(apint_compare(objs->ap0, sum) == 0);
  ASSERT(!apint_is_negative(sum));
  ASSERT(apint_is_zero(sum));
  apint_destroy(sum);
  
  /*0 = 1 + (-1) */
  sum = apint_add(objs->ap1, objs->minus1);
  ASSERT(apint_compare(objs->ap0, sum) == 0);
  ASSERT(!apint_is_negative(sum));
  ASSERT(apint_is_zero(sum));
  apint_destroy(sum);

  /* -1 = 0 + (-1) */
  sum = apint_add(objs->ap0, objs->minus1);
  ASSERT(apint_compare(objs->minus1, sum) == 0);
  ASSERT(apint_is_negative(sum));
  apint_destroy(sum);
  
  sum = apint_add(objs->apADD, objs->apADD);
  ASSERT(apint_compare(sum, objs->apADD) > 0);
  ASSERT(!apint_is_zero(sum));
  ASSERT(28 == apint_highest_bit_set(sum));
  apint_destroy(sum);

  sum = apint_add(objs->max1, objs->ap1);
  ASSERT(apint_compare(sum, objs->max1) > 0);
  ASSERT(!apint_is_zero(sum));
  ASSERT(64 == apint_highest_bit_set(sum));
  apint_destroy(sum);
}

void testSubNoHex(TestObjs *objs) {
  ApInt *sub, *neg;

  /* 0 = 0 */
  sub = apint_sub(objs->ap1, objs->ap1);
  ASSERT(apint_compare(sub, objs->ap0) == 0);
  ASSERT(apint_is_zero(sub));
  apint_destroy(sub);

  /* -1 = -1 */
  sub = apint_sub(objs->ap0, objs->ap1);
  ASSERT(apint_compare(sub, objs->minus1) == 0);
  ASSERT(apint_is_negative(sub));
  apint_destroy(sub);
  
  /* 0 = -1 - (-1) */
  sub = apint_sub(objs->minus1, objs->minus1);
  ASSERT(apint_compare(sub, objs->ap0) == 0);
  ASSERT(apint_is_zero(sub));
  apint_destroy(sub);
  

  sub = apint_sub(objs->max1, objs->ap1);
  neg = apint_sub(sub, objs->max1);
  ASSERT(apint_compare(sub, neg) > 0);
  ASSERT(apint_is_negative(neg));
  ASSERT(0 == apint_highest_bit_set(neg));
  ASSERT(apint_compare(objs->minus1, neg) == 0);
  apint_destroy(sub);
  apint_destroy(neg);
}

void testFormatAsHex(TestObjs *objs) {
  char *s; ApInt *a;
  
  ASSERT(0 == strcmp("0", (s = apint_format_as_hex(objs->ap0))));
  free(s);

  a = apint_create_from_hex("-0000");
  ASSERT(0 == strcmp("0", (s = apint_format_as_hex(a))));
  free(s);
  apint_destroy(a);

  ASSERT(0 == strcmp("1", (s = apint_format_as_hex(objs->ap1))));
  free(s);
   
  ASSERT(0 == strcmp("-1", (s = apint_format_as_hex(objs->minus1))));
  free(s);

  ASSERT(0 == strcmp("6988b09", (s = apint_format_as_hex(objs->ap110660361))));
  free(s);

  ASSERT(0 == strcmp("ffffffffffffffff", (s = apint_format_as_hex(objs->max1))));
  free(s);
   
  ASSERT(0 == strcmp("2bc4", (s = apint_format_as_hex(objs->ap11204))));
  free(s); 
}

void testAdd(TestObjs *objs) {
  ApInt *a, *b, *sum;
  char *s;
  
  /* 0 + 0 = 0 */
  sum = apint_add(objs->ap0, objs->ap0);
  ASSERT(0 == strcmp("0", (s = apint_format_as_hex(sum))));
  apint_destroy(sum);
  free(s);
  
  /* 1 + 0 = 1 */
  sum = apint_add(objs->ap1, objs->ap0);
  ASSERT(0 == strcmp("1", (s = apint_format_as_hex(sum))));
  apint_destroy(sum);
  free(s);
  
  /* 1 + 1 = 2 */
  sum = apint_add(objs->ap1, objs->ap1);
  ASSERT(0 == strcmp("2", (s = apint_format_as_hex(sum))));
  apint_destroy(sum);
  free(s);
  
  /* 110660361 + 1 = 110660362 */
  sum = apint_add(objs->ap110660361, objs->ap1);
  ASSERT(0 == strcmp("6988b0a", (s = apint_format_as_hex(sum))));
  apint_destroy(sum);
  free(s);
  
  /* FFFFFFFFFFFFFFFF + 1 = 10000000000000000 */
  sum = apint_add(objs->max1, objs->ap1);
  ASSERT(0 == strcmp("10000000000000000", (s = apint_format_as_hex(sum))));
  apint_destroy(sum);
  free(s);
	
  /* 110660361 + 11204 = 110,671,565*/
  sum = apint_add(objs->ap110660361, objs->ap11204);
  ASSERT(0 == strcmp("698b6cd", (s = apint_format_as_hex(sum))));
  apint_destroy(sum);
  free(s);
   
   /* test involving larger values */
  a = apint_create_from_hex("c16e712e5d2794a3496ff5c7e0106cac9b2949d462a21de27254d8414d04142446c8e841ddd3b8564f0ee06e9a5");
  b = apint_create_from_hex("95fdce7a109e35fff4bd");
  sum = apint_add(a, b);
  ASSERT(0 == strcmp("c16e712e5d2794a3496ff5c7e0106cac9b2949d462a21de27254d8414d04142446c8e84b3db09ff758f2406de62",
		     (s = apint_format_as_hex(sum))));
  apint_destroy(sum);
  apint_destroy(b);
  apint_destroy(a);
  free(s);
  
  /* test involving negative */
  sum = apint_add(objs->ap1, objs->minus1);
  ASSERT(0 == strcmp("0", (s = apint_format_as_hex(sum))));
  apint_destroy(sum);
  free(s);
   
   
  /* test involving larger values */
  a = apint_create_from_hex("6ffc4d9d8e7");
  b = apint_create_from_hex("6190496725a798c966df1638b83b4c67493be375");
  sum = apint_add(a, b);
  ASSERT(0 == strcmp("6190496725a798c966df1638b83b53670e15bc5c",
		     (s = apint_format_as_hex(sum))));
  apint_destroy(sum);
  apint_destroy(b);
  apint_destroy(a);
  free(s); 
  
  a = apint_create_from_hex("4edcdf339283");
  b = apint_create_from_hex("-2CBF");
  sum = apint_add(a, b);
  ASSERT(0 == strcmp("4edcdf3365c4",
		     (s = apint_format_as_hex(sum))));
  apint_destroy(sum);
  apint_destroy(b);
  apint_destroy(a);
  free(s); 

  
  a = apint_create_from_hex("abdd4433");
  b = apint_create_from_hex("-7eeaf44444444");
  sum = apint_add(a, b);
  ASSERT(0 == strcmp("-7eeae98670011",
		     (s = apint_format_as_hex(sum))));
  apint_destroy(sum);
  apint_destroy(b);
  apint_destroy(a);
  free(s);     
}

void testSub(TestObjs *objs) {
  ApInt *a, *b, *diff;
  char *s;

  /* subtracting 1 from ffffffffffffffff is fffffffffffffffe */
  diff = apint_sub(objs->max1, objs->ap1);
  ASSERT(0 == strcmp("fffffffffffffffe", (s = apint_format_as_hex(diff))));
  apint_destroy(diff);
  free(s);

  /* subtracting 0 from 1 is 1 */
  diff = apint_sub(objs->ap1, objs->ap0);
  ASSERT(0 == strcmp("1", (s = apint_format_as_hex(diff))));
  ASSERT(0 == apint_compare(diff, objs->ap1));
  apint_destroy(diff);
  free(s);

  /* subtracting 1 from 1 is 0 */
  diff = apint_sub(objs->ap1, objs->ap1);
  ASSERT(0 == strcmp("0", (s = apint_format_as_hex(diff))));
  ASSERT(0 == apint_compare(diff, objs->ap0));
  apint_destroy(diff);
  free(s);
  
  /* subtracting 1 from 0 is -1 */
  diff = apint_sub(objs->ap0, objs->ap1);
  ASSERT(0 == (strcmp("-1", (s = apint_format_as_hex(diff)))));
  ASSERT(0 == apint_compare(diff, objs->minus1));
  apint_destroy(diff);
  free(s);

  /* test involving larger values */
  a = apint_create_from_hex("7e35207519b6b06429378631ca460905c19537644f31dc50114e9dc90bb4e4ebc43cfebe6b86d");
  b = apint_create_from_hex("9fa0fb165441ade7cb8b17c3ab3653465e09e8078e09631ec8f6fe3a5b301dc");
  diff = apint_sub(a, b);
  ASSERT(0 == strcmp("7e35207519b6afc4883c6fdd8898213a367d73b918de95f20766963b0251c622cd3ec4633b691",
		     (s = apint_format_as_hex(diff))));
  apint_destroy(diff);
  apint_destroy(b);
  apint_destroy(a);
  free(s);
  
  /* test involving larger values (with a negative difference) */
  a = apint_create_from_hex("9fa0fb165441ade7cb8b17c3ab3653465e09e8078e09631ec8f6fe3a5b301dc");
  b = apint_create_from_hex("7e35207519b6b06429378631ca460905c19537644f31dc50114e9dc90bb4e4ebc43cfebe6b86d");
  diff = apint_sub(a, b);
  ASSERT(0 == strcmp("-7e35207519b6afc4883c6fdd8898213a367d73b918de95f20766963b0251c622cd3ec4633b691",
		     (s = apint_format_as_hex(diff))));
  apint_destroy(diff);
  apint_destroy(b);
  apint_destroy(a);
  free(s);
  
   /* more larger cases */
  a = apint_create_from_hex("a6ea18dd7b00fd0b389934b7cfe2d9be4b66211b10249ed517b2243ea009418c6c20ded77e");
  b = apint_create_from_hex("562b41ce730753683b2c555337874c870655bbc7f38661b37e996328419546bbde82");
  diff = apint_sub(a, b);
  ASSERT(0 == strcmp("a6ea18874fbf2e983145cc7ca38d8686c4199a14ba68d6e1915070c006a6194ad6da22f8fc",
		     (s = apint_format_as_hex(diff))));
  apint_destroy(diff);
  apint_destroy(b);
  apint_destroy(a);
  free(s);
  
  a = apint_create_from_hex("6ffc4d9d8e7");
  b = apint_create_from_hex("-6190496725a798c966df1638b83b4c67493be375");
  diff = apint_sub(a, b);
  ASSERT(0 == strcmp("6190496725a798c966df1638b83b53670e15bc5c",
		     (s = apint_format_as_hex(diff))));
  apint_destroy(diff);
  apint_destroy(b);
  apint_destroy(a);
  free(s); 
  
  a = apint_create_from_hex("4edcdf339283");
  b = apint_create_from_hex("2CBF");
  diff = apint_sub(a, b);
  ASSERT(0 == strcmp("4edcdf3365c4",
		     (s = apint_format_as_hex(diff))));
  apint_destroy(diff);
  apint_destroy(b);
  apint_destroy(a);
  free(s); 

  
  a = apint_create_from_hex("abdd4433");
  b = apint_create_from_hex("7eeaf44444444");
  diff = apint_sub(a, b);
  ASSERT(0 == strcmp("-7eeae98670011",
		     (s = apint_format_as_hex(diff))));
  apint_destroy(diff);
  apint_destroy(b);
  apint_destroy(a);
  free(s);     

}

void testlShift(TestObjs *objs) {
  ApInt *a, *shift;
  char * s;
  (void) objs;
  a = apint_create_from_hex("3");
  shift = apint_lshift(a);
  ASSERT(0 == strcmp("6", (s = apint_format_as_hex(shift))));
  apint_destroy(shift);
  apint_destroy(a);
  free(s);     
  
  a = apint_create_from_hex("2eedccd4453");
  shift = apint_lshift_n(a, 4);
  ASSERT(0 == strcmp("2eedccd44530", (s = apint_format_as_hex(shift))));
  apint_destroy(shift);
  apint_destroy(a);
  free(s); 
  
  a = apint_create_from_hex("-7eefccd22445099");
  shift = apint_lshift_n(a, 15);
  ASSERT(0 == strcmp("-3f77e669122284c8000", (s = apint_format_as_hex(shift))));
  apint_destroy(shift);
  apint_destroy(a);
  free(s); 
}
