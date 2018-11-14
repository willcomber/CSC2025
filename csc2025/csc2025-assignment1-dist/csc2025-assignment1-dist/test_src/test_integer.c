#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <assert.h>
#include <errno.h>
#include "test_lib.h"
#include "../integer.h"

#define TEST_CASE_RUNS 100
#define NR_TESTS 15

/* test functions */
int test_newInteger();
int test_deleteInteger();
int test_IntMax();
int test_IntMin(); 
int test_add_norm();
int test_add_err();
int test_subtract_norm();
int test_subtract_err();
int test_multiply_norm();
int test_multiply_err();
int test_divide_norm();
int test_divide_err();
int test_modulo_norm();
int test_modulo_err();
int test_get_value();

struct test_defn test_schedule[NR_TESTS] = {
    { "test_newInteger",    test_newInteger,         17 },   /* test  0 */
    { "test_deleteInteger", test_deleteInteger,       8 },   /* test  1 */
    { "test_IntMax",        test_IntMax,              5 },   /* test  2 */
    { "test_IntMin",        test_IntMin,              5 },   /* test  3 */
    { "test_add_norm",      test_add_norm,          212 },   /* test  4 */
    { "test_add_err",       test_add_err,           113 },   /* test  5 */
    { "test_subtract_norm", test_subtract_norm,     160 },   /* test  6 */
    { "test_subtract_err",  test_subtract_err,      112 },   /* test  7 */
    { "test_multiply_norm", test_multiply_norm,     184 },   /* test  8 */
    { "test_multiply_err",  test_multiply_err,      113 },   /* test  9 */
    { "test_divide_norm",   test_divide_norm,       142 },   /* test 10 */
    { "test_divide_err",    test_divide_err,         10 },   /* test 11 */
    { "test_modulo_norm",   test_modulo_norm,       144 },   /* test 12 */
    { "test_modulo_err",    test_modulo_err,         10 },   /* test 13 */
    { "test_get_value",     test_get_value,         104 },   /* test 14 */
};

/* test helper functions */
Integer assert_newInteger(int test_case, int line_num, int i);
void assert_success(int test_case, int line_num, int lhs_before, 
    int rhs_before, char op, Integer lhs, Integer rhs, Integer act, int exp);
void assert_error(int test_case, int line_num, char op, Integer lhs, 
    Integer rhs, Integer act);
int assert_null_binop(int test_case, int line_num, Integer oi, 
    Integer (*op)(Integer, Integer));
void assert_add(int test_case, int line_num, int l, int r, int exp);
void assert_add_perms(int test_case, int line_num, int i, int j);
void assert_add_err(int test_case, int line_num, int l, int r);
void assert_subtract(int test_case, int line_num, int l, int r, int exp);
void assert_subtract_perms(int test_case, int line_num, int i, int j);
void assert_subtract_err(int test_case, int line_num, int l, int r);
void assert_multiply(int test_case, int line_num, int l, int r, int exp);
void assert_multiply_perms(int test_case, int line_num, int i, int j);
void assert_multiply_err(int test_case, int line_num, int l, int r);
void assert_multiply_err_perms(int test_case, int line_num, int i, int j);
void assert_divide(int test_case, int line_num, int l, int r, int exp);
void assert_divide_perms(int test_case, int line_num, int i, int j);
void assert_divide_err(int test_case, int line_num, int l, int r);
void assert_modulo(int test_case, int line_num, int l, int r, int exp);
void assert_modulo_perms(int test_case, int line_num, int i, int j);
void assert_modulo_err(int test_case, int line_num, int l, int r);

int main(int argc, char** argv) {
    run_tests(argc, argv, NR_TESTS, test_schedule, false);
    
    return 0;
}

int test_newInteger() {
    int test_case = 0;
    
    int r = rand();
    Integer oi = newInteger(r);
    assert_notnull(++test_case, __LINE__, oi);
    assert_eq(++test_case, __LINE__, oi->get_value(oi), r);
    
    r  = -rand();
    Integer oj = newInteger(r);
    assert_notnull(++test_case, __LINE__, oj);
    assert_eq(++test_case, __LINE__, oi->get_value(oj), r);
    
    assert_notidentical(++test_case, __LINE__, oi, oj);
    
    deleteInteger(&oi);
    deleteInteger(&oj);
    
    Integer imax1 = newInteger(INT_MAX);
    assert_notnull(++test_case, __LINE__, imax1);
    assert_eq(++test_case, __LINE__, imax1->get_value(imax1), INT_MAX);
    
    assert_identical(++test_case, __LINE__, imax1, IntMax());
    
    Integer imax2 = newInteger(INT_MAX);
    assert_notnull(++test_case, __LINE__, imax2);
    assert_eq(++test_case, __LINE__, imax1->get_value(imax2), INT_MAX);
    
    assert_identical(++test_case, __LINE__, imax1, imax2);
    
    Integer imin1 = newInteger(INT_MIN);
    assert_notnull(++test_case, __LINE__, imin1);
    assert_eq(++test_case, __LINE__, imin1->get_value(imin1), INT_MIN);
    
    assert_identical(++test_case, __LINE__, imin1, IntMin());
    
    Integer imin2 = newInteger(INT_MIN);
    assert_notnull(++test_case, __LINE__, imin2);
    assert_eq(++test_case, __LINE__, imax1->get_value(imin2), INT_MIN);
    
    assert_identical(++test_case, __LINE__, imin1, imin2);   
    
    return test_case;   
}

int test_deleteInteger() { 
    int test_case = 0;

    Integer oi = newInteger(1);
    assert(oi);
    
    deleteInteger(&oi);
    
    assert_null(++test_case, __LINE__, oi);

    deleteInteger(&oi);     /* no-op 
                             * will cause failure if not implemented correctly 
                             */
    ++test_case;            /* passed */
    
    Integer imax = IntMax();
    assert(imax);
    
    deleteInteger(&imax);
    assert_notnull(++test_case, __LINE__, imax);    
    assert_identical(++test_case, __LINE__, imax, IntMax());
    
    Integer imin = IntMin();
    assert(imin);
    
    deleteInteger(&imin);
    assert_notnull(++test_case, __LINE__, imin);    
    assert_identical(++test_case, __LINE__, imin, IntMin());
    
    deleteInteger(NULL);     /* no-op 
                              * will cause failure if not implemented correctly 
                              */
                              
    ++test_case;            /* passed */
    
    return ++test_case;   
}

int test_IntMax() { 
    int test_case = 0;
    
    Integer imax1 = IntMax();
    Integer imax2 = IntMax();
    
    assert_notnull(++test_case, __LINE__, imax1);
    assert_notnull(++test_case, __LINE__, imax2);
    assert_identical(++test_case, __LINE__, imax1, imax2);
    assert_eq(++test_case, __LINE__, imax1->get_value(imax1), INT_MAX);
    assert_eq(++test_case, __LINE__, imax2->get_value(imax2), INT_MAX);
    
    return test_case;
}

int test_IntMin() { 
    int test_case = 0;
    
    Integer imin1 = IntMin();
    Integer imin2 = IntMin();
    
    assert_notnull(++test_case, __LINE__, imin1);
    assert_notnull(++test_case, __LINE__, imin2);
    assert_identical(++test_case, __LINE__, imin1, imin2);
    assert_eq(++test_case, __LINE__, imin1->get_value(imin1), INT_MIN);
    assert_eq(++test_case, __LINE__, imin1->get_value(imin1), INT_MIN);
    
    return test_case;
} 


int test_add_norm() {
    int test_case = 0;
    
    for (int i = 0; i < 10; i++)
        assert_add_perms(++test_case, __LINE__, 0, i);
    
    for (int i = -10; i < 11; i++) {
        assert_add_perms(++test_case, __LINE__, i, i);
        assert_add_perms(++test_case, __LINE__, i, i + 1);
    }
    
    assert_add_perms(++test_case, __LINE__, 0, INT_MAX);
    assert_add_perms(++test_case, __LINE__, 1, INT_MAX - 1);
    
    assert_add(++test_case, __LINE__, -1, INT_MAX, INT_MAX - 1);
    assert_add(++test_case, __LINE__, INT_MAX, -1, INT_MAX - 1);
    assert_add(++test_case, __LINE__, -1, -INT_MAX, INT_MIN);    
    assert_add(++test_case, __LINE__, -INT_MAX, -1, INT_MIN);    

    assert_add(++test_case, __LINE__, 0, INT_MIN, INT_MIN);
    assert_add(++test_case, __LINE__, INT_MIN, 0, INT_MIN);
    assert_add(++test_case, __LINE__, 1, INT_MIN, -INT_MAX);
    assert_add(++test_case, __LINE__, INT_MIN, 1, -INT_MAX);
    
    
    for (int i = 0; i < TEST_CASE_RUNS / 2; i++) {
        int r = rand();
        assert_add_perms(++test_case, __LINE__, r, INT_MAX - r);
        assert_add(++test_case, __LINE__, -r, INT_MIN + r, INT_MIN);
        assert_add(++test_case, __LINE__, INT_MIN + r, -r, INT_MIN);
    }
    
    return test_case;
}

int test_add_err() {
    Integer oi = newInteger(1);
    assert(oi);
    
    int test_case = assert_null_binop(1, __LINE__, oi, oi->add);
    
    deleteInteger(&oi);
    
    assert_add_err(++test_case, __LINE__, 1, INT_MAX);
    assert_add_err(++test_case, __LINE__, INT_MAX, 1);
    assert_add_err(++test_case, __LINE__, -1, INT_MIN);
    assert_add_err(++test_case, __LINE__, INT_MIN, -1);
    assert_add_err(++test_case, __LINE__, -2, -INT_MAX);
    assert_add_err(++test_case, __LINE__, -INT_MAX, -2);
    assert_add_err(++test_case, __LINE__, INT_MIN, INT_MIN);
    assert_add_err(++test_case, __LINE__, INT_MAX, INT_MAX);

    for (int i = 0; i < TEST_CASE_RUNS / 2; i++) {
        int r = rand();
        if (r < 1) r = 1;
        
        assert_add_err(++test_case, __LINE__, r, INT_MAX - (r - 1));
        assert_add_err(++test_case, __LINE__, -r, INT_MIN + (r - 1));
    }
    
    int i = INT_MAX / 2;
    int j = 2 * (INT_MAX / 3);
    
    assert_add_err(++test_case, __LINE__, i, j); 
    assert_add_err(++test_case, __LINE__, -i, -j);
    
    return test_case;
}

int test_subtract_norm() {
    int test_case = 0;
    
    for (int i = 0; i < 10; i++)
        assert_subtract_perms(++test_case, __LINE__, 0, i);
    
    for (int i = -10; i < 11; i++) {
        assert_subtract_perms(++test_case, __LINE__, i, i);
        assert_subtract_perms(++test_case, __LINE__, i, i + 1);
    }
    
    assert_subtract(++test_case, __LINE__, 0, INT_MAX, -INT_MAX);
    assert_subtract(++test_case, __LINE__, INT_MAX, 0, INT_MAX);
    assert_subtract(++test_case, __LINE__, INT_MIN, 0, INT_MIN);
    assert_subtract(++test_case, __LINE__, 1, INT_MAX, -INT_MAX + 1);
    assert_subtract(++test_case, __LINE__, -1, INT_MAX, INT_MIN);
    assert_subtract(++test_case, __LINE__, -1, INT_MIN, INT_MAX);
    assert_subtract(++test_case, __LINE__, INT_MAX, 1, INT_MAX - 1);
    assert_subtract(++test_case, __LINE__, INT_MIN, -1, -INT_MAX);
    
    for (int i = 0; i < TEST_CASE_RUNS / 4; i++) {
        int r = rand();
        assert_subtract(++test_case, __LINE__, INT_MAX, r, INT_MAX - r);
        assert_subtract(++test_case, __LINE__, INT_MIN, -r, INT_MIN + r);
        assert_subtract(++test_case, __LINE__, r, INT_MAX, r - INT_MAX);
        assert_subtract(++test_case, __LINE__, -r, INT_MIN, -(INT_MIN + r));
    }
    
    return test_case;
}

int test_subtract_err() {
    Integer oi = newInteger(1);
    assert(oi);
   
    int test_case = assert_null_binop(1, __LINE__, oi, oi->subtract);
   
    deleteInteger(&oi);
    
    assert_subtract_err(++test_case, __LINE__, -2, INT_MAX);
    assert_subtract_err(++test_case, __LINE__, -INT_MAX, 2);
    assert_subtract_err(++test_case, __LINE__, 0, INT_MIN);
    assert_subtract_err(++test_case, __LINE__, 1, INT_MIN + 1);
    assert_subtract_err(++test_case, __LINE__, INT_MIN, 1);
    assert_subtract_err(++test_case, __LINE__, INT_MAX, -1);
    assert_subtract_err(++test_case, __LINE__, INT_MIN, INT_MAX);

    for (int i = 0; i < TEST_CASE_RUNS / 2; i++) {
        int r = rand();
        assert_subtract_err(++test_case, __LINE__, -r - 2, INT_MAX - r);
        assert_subtract_err(++test_case, __LINE__, r, INT_MIN + r);
    }

    int i = INT_MAX / 2;
    int j = 2 * (INT_MAX / 3);
    
    assert_subtract_err(++test_case, __LINE__, i, -j); 
    assert_subtract_err(++test_case, __LINE__, -i, j);

    return test_case;
}

int test_multiply_norm() {
    int test_case = 0;
    
    for (int i = 0; i < 10; i++)
        assert_multiply_perms(++test_case, __LINE__, 0, i);
    
    for (int i = -10; i < 11; i++) {
        assert_multiply_perms(++test_case, __LINE__, i, i);
        assert_multiply_perms(++test_case, __LINE__, i, i + 1);
    }

    assert_multiply(++test_case, __LINE__, 0, 0, 0);
    assert_multiply_perms(++test_case, __LINE__, 0, 1);
    assert_multiply_perms(++test_case, __LINE__, 1, 1);
    
    assert_multiply(++test_case, __LINE__, 0, INT_MAX, 0);
    assert_multiply(++test_case, __LINE__, INT_MAX, 0, 0);
    
    for (int i = 1; i < 10; i++)
        assert_multiply_perms(++test_case, __LINE__, i, INT_MAX / i);
    
    assert_multiply(++test_case, __LINE__, INT_MIN, 0, 0);
    assert_multiply(++test_case, __LINE__, 0, INT_MIN, 0);

    for (int i = 1; i < 10; i++) {
        int exp = (INT_MIN / i) * i;
        assert_multiply(++test_case, __LINE__, INT_MIN / i, i, exp);
        assert_multiply(++test_case, __LINE__, i, INT_MIN / i, exp);
    }
    
    for (int i = 1; i < TEST_CASE_RUNS / 2; i++) {
        int r = rand();
        while (r > INT_MAX / i) r = rand();
        assert_multiply_perms(++test_case, __LINE__, r, i);

        while (r > INT_MAX / (i * i) ) r = rand();
        assert_multiply_perms(++test_case, __LINE__, r, i * i);
    }

    return test_case;
}

int test_multiply_err() {
    Integer oi = newInteger(1);
    assert(oi);
    
    int test_case = assert_null_binop(1, __LINE__, oi, oi->multiply);
     
    deleteInteger(&oi);
    
    assert_multiply_err(++test_case, __LINE__, INT_MIN, -1);
    assert_multiply_err(++test_case, __LINE__, -1, INT_MIN);
    
    assert_multiply_err_perms(++test_case, __LINE__, 2, INT_MAX);

    assert_multiply_err(++test_case, __LINE__, INT_MIN, -2);
    assert_multiply_err(++test_case, __LINE__, 2, INT_MIN);
    assert_multiply_err(++test_case, __LINE__, INT_MIN, 2);
    assert_multiply_err(++test_case, __LINE__, -2, INT_MIN);
    assert_multiply_err(++test_case, __LINE__, INT_MIN / 2, -2);
    assert_multiply_err(++test_case, __LINE__, INT_MAX / 3, 4);
   

    for (int i = 0; i < TEST_CASE_RUNS / 2; i++) {
        int r = rand();
        assert_multiply_err_perms(++test_case, __LINE__, INT_MAX - r, r);
        assert_multiply_err(++test_case, __LINE__, r, INT_MIN + r);        
    }
    
    assert_multiply_err_perms(++test_case, __LINE__, INT_MAX / 2, 
        2 * (INT_MAX / 3));
    
    return test_case;
}

int test_divide_norm() {
    int test_case = 0;
    
    for (int i = -10; i < -1; i++) {
        assert_divide_perms(++test_case, __LINE__, i, i);
        assert_divide_perms(++test_case, __LINE__, i, i + 1);
    }

    for (int i = 1; i < 11; i++) {
        assert_divide_perms(++test_case, __LINE__, i, i);
        assert_divide_perms(++test_case, __LINE__, i, i + 1);
    }
    
    assert_divide_perms(++test_case, __LINE__, INT_MAX, 1);
    assert_divide_perms(++test_case, __LINE__, INT_MAX, 2);
    assert_divide_perms(++test_case, __LINE__, INT_MIN, 2);
   
    assert_divide(++test_case, __LINE__, INT_MIN, 1, INT_MIN);

    
    for (int i = 0; i < TEST_CASE_RUNS; i++) {
        int r = rand();
        while (!r) r = rand();
        assert_divide_perms(++test_case, __LINE__, r, i + 1);
    }

    return test_case;
}

int test_divide_err() {
    Integer oi = newInteger(1);
    assert(oi);
   
    int test_case = assert_null_binop(1, __LINE__, oi, oi->divide);
   
    deleteInteger(&oi);
    
    assert_divide_err(++test_case, __LINE__, INT_MIN, -1);
    assert_divide_err(++test_case, __LINE__, -INT_MAX - 1, -1);
    assert_divide_err(++test_case, __LINE__, INT_MAX, 0);
    assert_divide_err(++test_case, __LINE__, INT_MIN, 0);
    assert_divide_err(++test_case, __LINE__, 1, 0);
    assert_divide_err(++test_case, __LINE__, 0, 0);
    assert_divide_err(++test_case, __LINE__, -1, 0);   
    
    return test_case;
}

int test_modulo_norm() {
    int test_case = 0;
    
    for (int i = -10; i < -1; i++) {
        assert_modulo_perms(++test_case, __LINE__, i, i);
        assert_modulo_perms(++test_case, __LINE__, i, i + 1);
    }

    for (int i = 1; i < 11; i++) {
        assert_modulo_perms(++test_case, __LINE__, i, i);
        assert_modulo_perms(++test_case, __LINE__, i, i + 1);
    }
    
    assert_modulo_perms(++test_case, __LINE__, INT_MAX, 1);
    assert_modulo_perms(++test_case, __LINE__, INT_MAX, 2);
    assert_modulo_perms(++test_case, __LINE__, INT_MAX, 3);
    assert_modulo_perms(++test_case, __LINE__, INT_MIN, 2);
    assert_modulo_perms(++test_case, __LINE__, INT_MIN, 3);
   
    assert_modulo(++test_case, __LINE__, INT_MIN, 1, 0);

    
    for (int i = 0; i < TEST_CASE_RUNS; i++) {
        int r = rand();
        while (!r) r = rand();
        assert_modulo_perms(++test_case, __LINE__, r, i + 1);
    }

    return test_case;
}

int test_modulo_err() {
    Integer oi = newInteger(1);
    assert(oi);
   
    int test_case = assert_null_binop(1, __LINE__, oi, oi->modulo);
   
    deleteInteger(&oi);
    
    assert_modulo_err(++test_case, __LINE__, INT_MIN, -1);
    assert_modulo_err(++test_case, __LINE__, -INT_MAX - 1, -1);
    assert_modulo_err(++test_case, __LINE__, INT_MAX, 0);
    assert_modulo_err(++test_case, __LINE__, INT_MIN, 0);
    assert_modulo_err(++test_case, __LINE__, 1, 0);
    assert_modulo_err(++test_case, __LINE__, 0, 0);
    assert_modulo_err(++test_case, __LINE__, -1, 0);   
    
    return test_case;
}

int test_get_value() {
    int test_case = 0;

    Integer oi = newInteger(1);
    
    assert_notnull(++test_case, __LINE__, oi);
    assert_eq(++test_case, __LINE__, oi->get_value(oi), 1);
    
    errno = 0;
    assert_eq(++test_case, __LINE__, oi->get_value(NULL), 0);
    assert_eq(++test_case, __LINE__, errno, EINVAL);
    
    errno = 0;

    deleteInteger(&oi);
    assert(!oi);

    for (int i = 0; i < 25; i++) {
        int r = rand();
        Integer oi = newInteger(r);
        
        assert_notnull(++test_case, __LINE__, oi);
        assert_eq(++test_case, __LINE__, oi->get_value(oi), r);
        
        deleteInteger(&oi);
        assert(!oi);
        
        oi = newInteger(-r);
        
        assert_notnull(++test_case, __LINE__, oi);
        assert_eq(++test_case, __LINE__, oi->get_value(oi), -r);
        
        deleteInteger(&oi);
        assert(!oi);
    }    


    return test_case;
}

/* The following are assertion helper functions for tests */

Integer assert_newInteger(int test_case, int line_num, int i) {
    Integer oi = newInteger(i);
    
    if (!oi || oi->get_value(oi) != i) {
        char* msg = NULL;
        (void) asprintf(&msg, "newInteger(%d) failed", i);
        print_test_case_msg(test_case, line_num, msg);
        free(msg);
    }
    
    assert(oi && oi->get_value(oi) == i);
    
    return oi;
}

void assert_success(int test_case, int line_num, int lhs_before, 
    int rhs_before, char op, Integer lhs, Integer rhs, Integer act, int exp) {
    char *msg = NULL;
    int lhs_after = lhs->get_value(lhs);
    int rhs_after = rhs->get_value(rhs);
    
    if (!act) {
        (void) asprintf(&msg, "%d %c %d, actual: NULL, expected: %d",
            lhs_before, op, rhs_before, exp);
        print_test_case_msg(test_case, line_num, msg);
        free(msg);
    } else {
        int act_val = act->get_value(act);
        
        if (act_val != INT_MAX && act_val != INT_MIN) {
            assert_notidentical(test_case, line_num, lhs, act);
            assert_notidentical(test_case, line_num, lhs, act);
        }
        
        if (act->get_value(act) != exp) {
            (void) asprintf(&msg, "%d %c %d, actual: %d, expected: %d",
                lhs_before, op, rhs_before, act_val, exp);
            print_test_case_msg(test_case, line_num, msg);
            free(msg);
        } else if (lhs_before != lhs_after) {
            (void) asprintf(&msg, "%c op changed lhs operand, before: %d, after: %d", op, lhs_before, lhs_after);
            print_test_case_msg(test_case, line_num, msg);
            free(msg);
        } else if (rhs_before != rhs_after) {
            (void) asprintf(&msg, "%c op changed lhs operand, before: %d, after: %d", op, rhs_before, rhs_after);
            print_test_case_msg(test_case, line_num, msg);
            free(msg);
        }
    }
            
    assert(act && act->get_value(act) == exp);
    assert(lhs_before == lhs_after);
    assert(rhs_before == rhs_after);
    
    deleteInteger(&lhs); 
    deleteInteger(&rhs);
    deleteInteger(&act);
}

void assert_error(int test_case, int line_num, char op, Integer lhs, 
    Integer rhs, Integer act) {
    if (act) { 
        char* msg = NULL;
        (void) asprintf(&msg, "%d %c %d, actual: %d, expected: NULL, actual errno: %d, expected errno: %d", lhs->get_value(lhs), op, rhs->get_value(rhs),
        act->get_value(act), errno, ERANGE);
        print_test_case_msg(test_case, line_num, msg);
        free(msg);
    } else if (errno != ERANGE) {
        char* msg = NULL;
        (void) asprintf(&msg, "%d %c %d, actual errno: %d, expected errno: %d",
            lhs->get_value(lhs), op, rhs->get_value(rhs), errno, ERANGE);
        print_test_case_msg(test_case, line_num, msg);
        free(msg);
    }

    assert(!act && errno == ERANGE);
    
    deleteInteger(&lhs);
    deleteInteger(&rhs);
    errno = 0;
}

int assert_null_binop(int test_case, int line_num, Integer oi, 
    Integer (*op)(Integer, Integer)) {    
    errno = 0;
    if (op(oi, NULL) != NULL || errno != EINVAL) {
        char* msg = NULL;
        (void) asprintf(&msg, "actual: not NULL, expected: NULL, actual errno: %d, expected errno: %d", errno, EINVAL);
        print_test_case_msg(test_case, line_num, msg);
        free(msg);
    }
    assert(!op(oi, NULL) && errno == EINVAL);
    ++test_case;
    
    errno = 0;
    if (op(NULL, oi) != NULL || errno != EINVAL) {
        char* msg = NULL;
        (void) asprintf(&msg, "actual: not NULL, expected: NULL, actual errno: %d, expected errno: %d", errno, EINVAL);
        print_test_case_msg(test_case, line_num, msg);
        free(msg);
    }
    assert(!op(NULL, oi) && errno == EINVAL);
    ++test_case;
    
    errno = 0;
    if (op(NULL, NULL) != NULL || errno != EINVAL) {
        char* msg = NULL;
        (void) asprintf(&msg, "actual: not NULL, expected: NULL, actual errno: %d, expected errno: %d", errno, EINVAL);
        print_test_case_msg(test_case, line_num, msg);
        free(msg);
    }
    assert(!op(NULL, NULL) && errno == EINVAL);
    
    errno = 0;
    
    return test_case;
}

void assert_add(int test_case, int line_num, int l, int r, int exp) {
    Integer lhs = assert_newInteger(test_case, line_num, l);
    Integer rhs = assert_newInteger(test_case, line_num, r);
    
    int lhs_before = lhs->get_value(lhs);
    int rhs_before = rhs->get_value(rhs);
    
    Integer act = lhs->add(lhs, rhs);
    
    assert_success(test_case, line_num, lhs_before, rhs_before, '+', lhs, rhs,
        act, exp);
}

void assert_add_perms(int test_case, int line_num, int i, int j) {
    assert_add(test_case, line_num, i, j, i + j);
    assert_add(test_case, line_num, i, -j, i - j);
    assert_add(test_case, line_num, -i, j, j - i);
    assert_add(test_case, line_num, -i, -j, -(i + j));
    
    assert_add(test_case, line_num, j, i, i + j);
    assert_add(test_case, line_num, j, -i, j - i);
    assert_add(test_case, line_num, -j, i, i - j);
    assert_add(test_case, line_num, -j, -i, -(i + j));
}

void assert_add_err(int test_case, int line_num, int l, int r) {
    Integer lhs = assert_newInteger(test_case, line_num, l);
    Integer rhs = assert_newInteger(test_case, line_num, r);
    
    errno = 0;
    
    Integer act = lhs->add(lhs, rhs);
    
    assert_error(test_case, line_num, '+', lhs, rhs, act);
}

void assert_subtract(int test_case, int line_num, int l, int r, int exp) {
    Integer lhs = assert_newInteger(test_case, line_num, l);
    Integer rhs = assert_newInteger(test_case, line_num, r);
    
    int lhs_before = lhs->get_value(lhs);
    int rhs_before = rhs->get_value(rhs);
    
    Integer act = lhs->subtract(lhs, rhs);
    
    assert_success(test_case, line_num, lhs_before, rhs_before, '-', lhs, rhs,
        act, exp);
}

void assert_subtract_perms(int test_case, int line_num, int i, int j) {
    assert_subtract(test_case, line_num, i, j, i - j);
    assert_subtract(test_case, line_num, i, -j, i + j);
    assert_subtract(test_case, line_num, -i, j, -i - j);
    assert_subtract(test_case, line_num, -i, -j, -i + j);
    
    assert_subtract(test_case, line_num, j, i, j - i);
    assert_subtract(test_case, line_num, j, -i, j + i);
    assert_subtract(test_case, line_num, -j, i, -j - i);
    assert_subtract(test_case, line_num, -j, -i, -j + i);
}

void assert_subtract_err(int test_case, int line_num, int l, int r) {
    Integer lhs = assert_newInteger(test_case, line_num, l);
    Integer rhs = assert_newInteger(test_case, line_num, r);
    
    errno = 0;
    
    Integer act = lhs->subtract(lhs, rhs);
    
    assert_error(test_case, line_num, '-', lhs, rhs, act);
}

void assert_multiply(int test_case, int line_num, int l, int r, int exp) {
    Integer lhs = assert_newInteger(test_case, line_num, l);
    Integer rhs = assert_newInteger(test_case, line_num, r);
    
    int lhs_before = lhs->get_value(lhs);
    int rhs_before = rhs->get_value(rhs);
    
    Integer act = lhs->multiply(lhs, rhs);
    
    assert_success(test_case, line_num, lhs_before, rhs_before, '*', lhs, rhs,
        act, exp);
}

void assert_multiply_perms(int test_case, int line_num, int i, int j) {
    int exp = i * j;
    assert_multiply(test_case, line_num, i, j, exp);
    assert_multiply(test_case, line_num, i, -j, -exp);
    assert_multiply(test_case, line_num, -i, j, -exp);
    assert_multiply(test_case, line_num, -i, -j, exp);
    
    assert_multiply(test_case, line_num, j, i, exp);
    assert_multiply(test_case, line_num, j, -i, -exp);
    assert_multiply(test_case, line_num, -j, i, -exp);
    assert_multiply(test_case, line_num, -j, -i, exp);
}

void assert_multiply_err(int test_case, int line_num, int l, int r) {
    Integer lhs = assert_newInteger(test_case, line_num, l);
    Integer rhs = assert_newInteger(test_case, line_num, r);
    
    errno = 0;
    
    Integer act = lhs->multiply(lhs, rhs);
    
    assert_error(test_case, line_num, '*', lhs, rhs, act);
}

void assert_multiply_err_perms(int test_case, int line_num, int i, int j) {
    assert_multiply_err(test_case, line_num, i, j);
    assert_multiply_err(test_case, line_num, i, -j);
    assert_multiply_err(test_case, line_num, -i, j);
    assert_multiply_err(test_case, line_num, -i, -j);
    
    assert_multiply_err(test_case, line_num, j, i);
    assert_multiply_err(test_case, line_num, j, -i);
    assert_multiply_err(test_case, line_num, -j, i);
    assert_multiply_err(test_case, line_num, -j, -i);
}

void assert_divide(int test_case, int line_num, int l, int r, int exp) {
    Integer lhs = assert_newInteger(test_case, line_num, l);
    Integer rhs = assert_newInteger(test_case, line_num, r);
    
    int lhs_before = lhs->get_value(lhs);
    int rhs_before = rhs->get_value(rhs);

    Integer act = lhs->divide(lhs, rhs);
    
    assert_success(test_case, line_num, lhs_before, rhs_before, '/', lhs, rhs,
        act, exp);
}

void assert_divide_perms(int test_case, int line_num, int i, int j) {
    assert_divide(test_case, line_num, i, j, i / j);
    assert_divide(test_case, line_num, i, -j, i / -j);
    assert_divide(test_case, line_num, -i, j, -i / j);
    assert_divide(test_case, line_num, -i, -j, -i / -j);
    
    assert_divide(test_case, line_num, j, i, j / i);
    assert_divide(test_case, line_num, j, -i, j / -i);
    assert_divide(test_case, line_num, -j, i, -j / i);
    assert_divide(test_case, line_num, -j, -i, -j / -i);
}

void assert_divide_err(int test_case, int line_num, int l, int r) {
    Integer lhs = assert_newInteger(test_case, line_num, l);
    Integer rhs = assert_newInteger(test_case, line_num, r);
    
    errno = 0;
    
    Integer act = lhs->divide(lhs, rhs);
    
    assert_error(test_case, line_num, '/', lhs, rhs, act);
}

void assert_modulo(int test_case, int line_num, int l, int r, int exp) {
    Integer lhs = assert_newInteger(test_case, line_num, l);
    Integer rhs = assert_newInteger(test_case, line_num, r);
    
    int lhs_before = lhs->get_value(lhs);
    int rhs_before = rhs->get_value(rhs);

    Integer act = lhs->modulo(lhs, rhs);
    
    assert_success(test_case, line_num, lhs_before, rhs_before, '%', lhs, rhs,
        act, exp);
}

void assert_modulo_perms(int test_case, int line_num, int i, int j) {
    assert_modulo(test_case, line_num, i, j, i % j);
    assert_modulo(test_case, line_num, i, -j, i % -j);
    assert_modulo(test_case, line_num, -i, j, -i % j);
    assert_modulo(test_case, line_num, -i, -j, -i % -j);
    
    assert_modulo(test_case, line_num, j, i, j % i);
    assert_modulo(test_case, line_num, j, -i, j % -i);
    assert_modulo(test_case, line_num, -j, i, -j % i);
    assert_modulo(test_case, line_num, -j, -i, -j % -i);
}

void assert_modulo_err(int test_case, int line_num, int l, int r) {
    Integer lhs = assert_newInteger(test_case, line_num, l);
    Integer rhs = assert_newInteger(test_case, line_num, r);
    
    errno = 0;
    
    Integer act = lhs->modulo(lhs, rhs);
    
    assert_error(test_case, line_num, '%', lhs, rhs, act);
}
