#include <stdio.h>
#include <stdlib.h>
#include "tctest.h"

#include "calc.h"

typedef struct {
	struct Calc *calc;
} TestObjs;

TestObjs *setup(void) {
	TestObjs *objs = malloc(sizeof(TestObjs));
	objs->calc = calc_create();
	return objs;
}

void cleanup(TestObjs *objs) {
	calc_destroy(objs->calc);
	free(objs);
}

void testEvalLiteral(TestObjs *objs);
void testAssignment(TestObjs *objs);
void testComputation(TestObjs *objs);
void testComputationAndAssignment(TestObjs *objs);
void testUpdate(TestObjs *objs);
void testInvalidExpr(TestObjs *objs);

int main(void) {
	TEST_INIT();

	TEST(testEvalLiteral);
	TEST(testAssignment);
	TEST(testComputation);
	TEST(testComputationAndAssignment);
	TEST(testUpdate);
	TEST(testInvalidExpr);

	TEST_FINI();
}

void testEvalLiteral(TestObjs *objs) {
	int result;

	result = 0;
	ASSERT(0 != calc_eval(objs->calc, "42", &result));
	ASSERT(42 == result);
	result = 0;
        ASSERT(0 != calc_eval(objs->calc, "1024", &result));
        ASSERT(1024 == result);
}

void testAssignment(TestObjs *objs) {
	int result;

	result = 0;
	ASSERT(0 != calc_eval(objs->calc, "a = 4", &result));
	ASSERT(4 == result);

	result = 0;
	ASSERT(0 != calc_eval(objs->calc, "a", &result));
	ASSERT(4 == result);

	result = 0;
	ASSERT(0 != calc_eval(objs->calc, "b = a", &result));
	ASSERT(4 == result);

	result = 0;
	ASSERT(0 != calc_eval(objs->calc, "c = b", &result));
	ASSERT(4 == result);
}

void testComputation(TestObjs *objs) {
	int result;

	result = 0;
	ASSERT(0 != calc_eval(objs->calc, "33 + 15", &result));
	ASSERT(48 == result);

	result = 0;
	ASSERT(0 != calc_eval(objs->calc, "33 - 15", &result));
	ASSERT(18 == result);

	result = 0;
	ASSERT(0 != calc_eval(objs->calc, "33 * 15", &result));
	ASSERT(495 == result);

	result = 0;
	ASSERT(0 != calc_eval(objs->calc, "33 / 15", &result));
	ASSERT(2 == result);

	result = 0;
        ASSERT(0 != calc_eval(objs->calc, "33 * 0", &result));
        ASSERT(0 == result);

	result = 0;
        ASSERT(0 != calc_eval(objs->calc, "87 / 15", &result));
        ASSERT(5 == result);
	/*Test that program can handle negative results */
	result = 0;
        ASSERT(0 != calc_eval(objs->calc, "33 - 34", &result));
        ASSERT(-1 == result);
	/*Test that program can handle calculation if first number is negative */
	result = 0;
        ASSERT(0 != calc_eval(objs->calc, "-33 + 34", &result));
        ASSERT(1 == result);
	/*Test that program can handle negative multiplication */
        result = 0;
        ASSERT(0 != calc_eval(objs->calc, "-40 * -1", &result));
        ASSERT(40 == result);
	/*Test that program can handle negative subtraction*/
        result = 0;
        ASSERT(0 != calc_eval(objs->calc, "-5 - -35", &result));
        ASSERT(30 == result);
}

void testComputationAndAssignment(TestObjs *objs) {
	int result;

	result = 0;
	ASSERT(0 != calc_eval(objs->calc, "a = 33 + 15", &result));
	ASSERT(48 == result);
	result = 0;
	ASSERT(0 != calc_eval(objs->calc, "a", &result));
	ASSERT(48 == result);

	result = 0;
	ASSERT(0 != calc_eval(objs->calc, "b = 33 - 15", &result));
	ASSERT(18 == result);
	result = 0;
	ASSERT(0 != calc_eval(objs->calc, "b", &result));
	ASSERT(18 == result);

	result = 0;
	ASSERT(0 != calc_eval(objs->calc, "c = 33 * 15", &result));
	ASSERT(495 == result);
	result = 0;
	ASSERT(0 != calc_eval(objs->calc, "c", &result));
	ASSERT(495 == result);

	result = 0;
	ASSERT(0 != calc_eval(objs->calc, "d = 33 / 15", &result));
	ASSERT(2 == result);
	result = 0;
	ASSERT(0 != calc_eval(objs->calc, "d", &result));
	ASSERT(2 == result);
	/*Test that program can correctly caculate with assigned variables only */
	result = 0;
        ASSERT(0 != calc_eval(objs->calc, "e = a + b", &result));
        ASSERT(66 == result);
	result = 0;
        ASSERT(0 != calc_eval(objs->calc, "e = d * c", &result));
        ASSERT(990 == result);
        result = 0;
        ASSERT(0 != calc_eval(objs->calc, "e = b - c", &result));
        ASSERT(-477 == result);

}

void testUpdate(TestObjs *objs) {
	int result;

	result = 0;
	ASSERT(0 != calc_eval(objs->calc, "a = 2", &result));
	ASSERT(2 == result);
	result = 0;
	ASSERT(0 != calc_eval(objs->calc, "a = a + 1", &result));
	ASSERT(3 == result);
	ASSERT(0 != calc_eval(objs->calc, "b = a * 10", &result));
	ASSERT(30 == result);
	ASSERT(0 != calc_eval(objs->calc, "b = a + b", &result));
	ASSERT(33 == result);
}

void testInvalidExpr(TestObjs *objs) {
	int result;

	/* bad syntax */
	ASSERT(0 == calc_eval(objs->calc, "+ 4", &result));
	/* bad syntax */
	ASSERT(0 == calc_eval(objs->calc, "4 +", &result));
	/* undefined variable */
	ASSERT(0 == calc_eval(objs->calc, "a", &result));
	/* attempt to divide by 0 */
	ASSERT(0 == calc_eval(objs->calc, "4 / 0", &result));
	/* bad syntax */
        ASSERT(0 == calc_eval(objs->calc, "1 -1", &result));
	/* bad syntax */
        ASSERT(0 == calc_eval(objs->calc, ".5 / 4", &result));
}
