#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <string.h>
#include "test_lib.h"
#include "strtest_lib.h"
#include "../string_o.h"

#define NR_TESTS 12

/* test functions */
int test_newString();

int test_length();

int test_deleteString();

int test_add_norm();

int test_add_err();

int test_char_at_norm();

int test_char_at_err();

int test_index_of_norm();

int test_index_of_err();

int test_get_value_norm();

int test_get_value_err();

int test_equals();

struct test_defn test_schedule[NR_TESTS] = {
        {"test_newString",      test_newString,      19},   /* test  0 */
        {"test_deleteString",   test_deleteString,   4},   /* test  1 */
        {"test_add_norm",       test_add_norm,       81},   /* test  2 */
        {"test_add_err",        test_add_err,        8},   /* test  3 */
        {"test_char_at_norm",   test_char_at_norm,   12},   /* test  4 */
        {"test_char_at_err",    test_char_at_err,    8},   /* test  5 */
        {"test_equals",         test_equals,         59},   /* test  6 */
        {"test_get_value_norm", test_get_value_norm, 78},   /* test  7 */
        {"test_get_value_err",  test_get_value_err,  8},   /* test  8 */
        {"test_index_of_norm",  test_index_of_norm,  8},   /* test  9 */
        {"test_index_of_err",   test_index_of_err,   8},   /* test 10 */
        {"test_length",         test_length,         14},   /* test 11 */
};

int main(int argc, char **argv) {
    run_tests(argc, argv, NR_TESTS, test_schedule, false);

    return 0;
}

int test_newString() {
    int test_case = 0;

    char *buf;
    int len;

    for (int i = 0; i < 10; i++) {
        len = create_test_buf(-1, &buf);
        assert_newString(++test_case, __LINE__, len, buf);
        free(buf);
    }

    len = create_test_buf(STR_LEN_MAX * 2, &buf);
    assert_newString(++test_case, __LINE__, len, buf);
    free(buf);

    len = create_test_buf(STR_LEN_MAX / 2, &buf);
    assert_newString(++test_case, __LINE__, len, buf);
    free(buf);

    len = create_test_buf(STR_LEN_MAX, &buf);
    assert_newString(++test_case, __LINE__, STR_LEN_MAX, buf);
    free(buf);

    len = create_test_buf(STR_LEN_MAX + 1, &buf);
    assert_newString(++test_case, __LINE__, STR_LEN_MAX + 1, buf);
    free(buf);

    len = create_test_buf(STR_LEN_MAX - 1, &buf);
    assert_newString(++test_case, __LINE__, STR_LEN_MAX - 1, buf);
    free(buf);

    len = create_test_buf(1, &buf);
    assert_newString(++test_case, __LINE__, 1, buf);
    free(buf);

    assert_newString(++test_case, __LINE__, 0, "");

    String null_s = newString(NULL);

    assert_null(++test_case, __LINE__, null_s);
    assert_eq(++test_case, __LINE__, errno, EINVAL);

    return test_case;
}

int test_length() {
    int test_case = 0;

    for (int i = 0; i < 10; i++) {
        char *buf;
        test_case++;

        int len = create_test_buf(-1, &buf);

        int exp_len = len > STR_LEN_MAX ? STR_LEN_MAX : len;

        String s = newString(buf);
        assert(s);

        assert_eq(test_case, __LINE__, s->length(s), exp_len);
        assert_eq(test_case, __LINE__, s->_len, exp_len);

        free(buf);

        deleteString(&s);
    }

    String s = newString(" ");
    assert(s);

    assert_eq(++test_case, __LINE__, s->length(s), 1);

    errno = 0;
    assert_eq(++test_case, __LINE__, s->length(NULL), -1);
    assert_eq(++test_case, __LINE__, errno, EINVAL);
    errno = 0;

    deleteString(&s);

    s = newString("");
    assert(s);
    assert_eq(++test_case, __LINE__, s->length(s), 0);
    deleteString(&s);

    return test_case;
}

int test_deleteString() {
    int test_case = 0;

    String s = create_test_str(-1);

    assert_notnull(++test_case, __LINE__, s);

    deleteString(&s);

    assert_null(++test_case, __LINE__, s);

    deleteString(&s);       /* no-op, will cause segmentation fault if not 
                             * implemented correctly 
                             */
    ++test_case;            /* passed */

    deleteString(NULL);     /* no-op 
                             * will cause failure if not implemented correctly 
                             */
    ++test_case;            /* passed */

    return test_case;
}

void assert_add_success(int test_case, int line_num, String lhs, String rhs,
                        String result) {
    assert_notnull(test_case, line_num, result);
    assert_notidentical(test_case, line_num, lhs, result);
    assert_notidentical(test_case, line_num, rhs, result);

    int exp_len = lhs->length(lhs) + rhs->length(rhs);

    exp_len = exp_len > STR_LEN_MAX ? STR_LEN_MAX : exp_len;

    assert_eq(test_case, line_num, result->length(result), exp_len);

    char buf[STR_LEN_MAX + STR_LEN_MAX + 1];

    strncpy(buf, lhs->_val, lhs->length(lhs));

    buf[lhs->length(lhs)] = '\0';

    strncat(buf, rhs->_val, rhs->length(rhs));

    buf[result->length(result)] = '\0';

    assert_eq(test_case, __LINE__,
              strncmp(buf, result->_val, STR_LEN_MAX), 0);
}

int test_add_norm() {
    int test_case = 0;

    String empty = newString("");
    assert(empty);
    assert(empty->length(empty) == 0);

    for (int i = 0; i < 20; i++) {
        String s1 = create_test_str(-1);
        String s2 = create_test_str(-1);

        String s3 = s1->add(s1, s2);
        assert_add_success(++test_case, __LINE__, s1, s2, s3);
        deleteString(&s3);

        s3 = s2->add(s2, s1);
        assert_add_success(++test_case, __LINE__, s2, s1, s3);
        deleteString(&s3);

        s3 = s1->add(s1, s1);
        assert_add_success(test_case, __LINE__, s1, s1, s3);
        deleteString(&s3);

        s3 = s1->add(s1, empty);
        assert_add_success(test_case, __LINE__, s1, empty, s3);
        deleteString(&s3);

        s3 = empty->add(empty, s1);
        assert_add_success(test_case, __LINE__, empty, s1, s3);

        deleteString(&s1);
        deleteString(&s2);
        deleteString(&s3);
    }

    String s1 = create_test_str(STR_LEN_MAX);
    String s2 = create_test_str(10);

    String s3 = s1->add(s1, s2);
    assert_cmp(++test_case, __LINE__, s3, STR_LEN_MAX, s1, STR_LEN_MAX);
    assert_notidentical(++test_case, __LINE__, s1, s3);
    assert_notidentical(++test_case, __LINE__, s1->_val, s3->_val);
    assert_notidentical(++test_case, __LINE__, s2, s3);

    deleteString(&s3);

    s3 = s2->add(s2, s1);
    assert_cmp(++test_case, __LINE__, s3, STR_LEN_MAX, s2, 10);
    assert_notidentical(++test_case, __LINE__, s2->_val, s3->_val);
    assert_notidentical(++test_case, __LINE__, s1, s3);
    assert_notidentical(++test_case, __LINE__, s2, s3);

    deleteString(&s3);

    s3 = empty->add(empty, s2);
    assert_cmp(++test_case, __LINE__, s3, 10, s2, 10);
    assert_notidentical(++test_case, __LINE__, s2->_val, s3->_val);
    assert_notidentical(++test_case, __LINE__, empty, s3);
    assert_notidentical(++test_case, __LINE__, s2, s3);

    deleteString(&s3);

    s3 = s2->add(s2, empty);
    assert_cmp(++test_case, __LINE__, s3, 10, s2, 10);
    assert_notidentical(++test_case, __LINE__, s2->_val, s3->_val);
    assert_notidentical(++test_case, __LINE__, empty, s3);
    assert_notidentical(++test_case, __LINE__, s2, s3);

    deleteString(&s2);
    deleteString(&s3);

    s2 = newString("");
    assert(s2);

    s3 = empty->add(empty, s2);
    assert_cmp(++test_case, __LINE__, s3, 0, s2, 1);
    assert_cmp(++test_case, __LINE__, s3, 0, empty, 1);
    assert_notidentical(++test_case, __LINE__, s2->_val, s3->_val);
    assert_notidentical(++test_case, __LINE__, empty->_val, s3->_val);
    assert_notidentical(++test_case, __LINE__, empty, s3);
    assert_notidentical(++test_case, __LINE__, s2, s3);
    deleteString(&s3);

    s3 = s2->add(s2, empty);
    assert_cmp(++test_case, __LINE__, s3, 0, s2, 1);
    assert_cmp(++test_case, __LINE__, s3, 0, empty, 1);
    assert_notidentical(++test_case, __LINE__, s2->_val, s3->_val);
    assert_notidentical(++test_case, __LINE__, empty->_val, s3->_val);
    assert_notidentical(++test_case, __LINE__, empty, s3);
    assert_notidentical(++test_case, __LINE__, s2, s3);
    deleteString(&s3);

    s3 = empty->add(empty, empty);
    assert_cmp(++test_case, __LINE__, s3, 0, empty, 1);
    assert_notidentical(++test_case, __LINE__, empty, s3);
    assert_notidentical(++test_case, __LINE__, empty->_val, s3->_val);

    deleteString(&s1);
    deleteString(&s2);
    deleteString(&s3);

    s1 = newString("a");
    s2 = newString("b");
    s3 = s1->add(s1, s2);
    String s4 = newString("ab");
    assert_cmp(++test_case, __LINE__, s3, 2, s4, 2);
    assert_notidentical(++test_case, __LINE__, s3->_val, s4->_val);
    assert_notidentical(++test_case, __LINE__, s1, s3);
    assert_notidentical(++test_case, __LINE__, s2, s3);
    assert_notidentical(++test_case, __LINE__, s3, s4);

    deleteString(&s3);
    deleteString(&s4);

    s3 = s2->add(s2, s1);
    s4 = newString("ba");
    assert_cmp(++test_case, __LINE__, s3, 2, s4, 2);
    assert_notidentical(++test_case, __LINE__, s3->_val, s4->_val);
    assert_notidentical(++test_case, __LINE__, s1, s3);
    assert_notidentical(++test_case, __LINE__, s2, s3);
    assert_notidentical(++test_case, __LINE__, s3, s4);

    deleteString(&s1);
    deleteString(&s2);
    deleteString(&s3);
    deleteString(&s4);
    deleteString(&empty);

    return test_case;
}

int test_add_err() {
    int test_case = 0;
    String s = newString("test");
    assert_notnull(++test_case, __LINE__, s);
    assert_eq(++test_case, __LINE__, s->length(s), 4);

    errno = 0;
    String t = s->add(s, NULL);
    assert_null(++test_case, __LINE__, t);
    assert_eq(++test_case, __LINE__, errno, EINVAL);

    errno = 0;
    t = s->add(NULL, s);
    assert_null(++test_case, __LINE__, t);
    assert_eq(++test_case, __LINE__, errno, EINVAL);

    errno = 0;
    t = s->add(NULL, NULL);
    assert_null(++test_case, __LINE__, t);
    assert_eq(++test_case, __LINE__, errno, EINVAL);

    errno = 0;

    deleteString(&s);

    return test_case;
}

int test_char_at_norm() {
    int test_case = 0;

    char *buf;

    for (int i = 0; i < 10; i++) {
        ++test_case;
        int len = create_test_buf(-1, &buf);

        int len_s = len > STR_LEN_MAX ? STR_LEN_MAX : len;

        String s = newString(buf);

        assert(s);
        assert(len_s == s->length(s));
        for (int j = 0; j < len_s; j++)
            assert_eq(test_case, __LINE__, buf[j], s->char_at(s, j));

        free(buf);
        deleteString(&s);
    }

    String s = newString("");

    assert(s);

    errno = 0;
    char c = s->char_at(s, 0);

    assert_eq(++test_case, __LINE__, c, 0);
    assert_eq(++test_case, __LINE__, errno, 0);

    deleteString(&s);

    return test_case;
}

int test_char_at_err() {
    int test_case = 0;

    String s = create_test_str(-1);

    errno = 0;
    assert_eq(++test_case, __LINE__, s->char_at(s, -1), 0);
    assert_eq(++test_case, __LINE__, errno, EINVAL);

    errno = 0;
    assert_eq(++test_case, __LINE__, s->char_at(s, s->length(s)), 0);
    assert_eq(++test_case, __LINE__, errno, EINVAL);

    errno = 0;
    assert_eq(++test_case, __LINE__, s->char_at(s, s->length(s) + 1), 0);
    assert_eq(++test_case, __LINE__, errno, EINVAL);

    errno = 0;
    assert_eq(++test_case, __LINE__, s->char_at(NULL, 0), 0);
    assert_eq(++test_case, __LINE__, errno, EINVAL);

    errno = 0;

    deleteString(&s);

    return test_case;
}

int test_index_of_norm() {
    int test_case = 0;

    String s = newString("hello");
    assert(s);

    assert_eq(++test_case, __LINE__, s->index_of(s, 'h', 0), 0);
    errno = 0;
    assert_eq(++test_case, __LINE__, s->index_of(s, 'h', 1), -1);
    assert_eq(++test_case, __LINE__, errno, 0);
    assert_eq(++test_case, __LINE__, s->index_of(s, 'l', 0), 2);
    assert_eq(++test_case, __LINE__, s->index_of(s, 'l', 2), 2);
    assert_eq(++test_case, __LINE__, s->index_of(s, 'l', 3), 3);
    errno = 0;
    assert_eq(++test_case, __LINE__, s->index_of(s, 'l', 4), -1);
    assert_eq(++test_case, __LINE__, errno, 0);

    deleteString(&s);

    return test_case;
}

int test_index_of_err() {
    int test_case = 0;

    String s = create_test_str(-1);

    errno = 0;
    assert_eq(++test_case, __LINE__, s->index_of(s, 'a', -1), -1);
    assert_eq(++test_case, __LINE__, errno, EINVAL);

    errno = 0;
    assert_eq(++test_case, __LINE__, s->index_of(s, 'a', s->length(s)), -1);
    assert_eq(++test_case, __LINE__, errno, EINVAL);

    errno = 0;
    assert_eq(++test_case, __LINE__, s->index_of(s, 'a', s->length(s) + 1), -1);
    assert_eq(++test_case, __LINE__, errno, EINVAL);

    errno = 0;
    assert_eq(++test_case, __LINE__, s->index_of(NULL, '1', 0), -1);
    assert_eq(++test_case, __LINE__, errno, EINVAL);

    errno = 0;

    deleteString(&s);

    return test_case;
}

int test_get_value_norm() {
    int test_case = 0;

    for (int i = 0; i < 10; i++) {
        String s = create_test_str(-1);

        char buf[s->length(s) + 1];
        char *val = s->get_value(s, buf);

        assert_notidentical(++test_case, __LINE__, val, s->_val);
        assert_notidentical(++test_case, __LINE__, buf, s->_val);
        assert_identical(++test_case, __LINE__, val, buf);
        assert_eq(++test_case, __LINE__, strnlen(buf, s->length(s) + 1),
                  s->length(s));

        assert_eq(++test_case, __LINE__, strncmp(val, s->_val, s->length(s)),
                  0);

        String t = newString(val);
        assert_true(++test_case, __LINE__, s->equals(s, t));

        deleteString(&s);
        deleteString(&t);
    }

    String s = newString("");
    assert(s);

    char buf[1];

    char *val = s->get_value(s, buf);

    assert_eq(++test_case, __LINE__, val[0], 0);

    deleteString(&s);

    char hello[] = "hello";

    s = newString(hello);
    assert(s);

    val = s->get_value(s, hello);

    assert_identical(++test_case, __LINE__, val, hello);
    assert_notidentical(++test_case, __LINE__, s->_val, val);

    assert_eq(++test_case, __LINE__, val[0], 'h');
    assert_eq(++test_case, __LINE__, val[1], 'e');
    assert_eq(++test_case, __LINE__, val[2], 'l');
    assert_eq(++test_case, __LINE__, val[3], 'l');
    assert_eq(++test_case, __LINE__, val[4], 'o');

    deleteString(&s);

    char goodbye[] = "goodbye";

    s = newString(goodbye);
    assert(s);

    char gb_buf[strlen(goodbye) + 1];

    val = s->get_value(s, gb_buf);

    assert_identical(++test_case, __LINE__, val, gb_buf);
    assert_notidentical(++test_case, __LINE__, s->_val, val);
    assert_notidentical(++test_case, __LINE__, val, goodbye);

    assert_eq(++test_case, __LINE__, val[0], 'g');
    assert_eq(++test_case, __LINE__, val[1], 'o');
    assert_eq(++test_case, __LINE__, val[2], 'o');
    assert_eq(++test_case, __LINE__, val[3], 'd');
    assert_eq(++test_case, __LINE__, val[4], 'b');
    assert_eq(++test_case, __LINE__, val[5], 'y');
    assert_eq(++test_case, __LINE__, val[6], 'e');

    deleteString(&s);

    return test_case;
}

int test_get_value_err() {
    int test_case = 0;

    String s = create_test_str(-1);
    assert(s);

    char buf[s->length(s) + 1];

    errno = 0;
    assert_notnull(++test_case, __LINE__, s->get_value(s, buf));
    assert_eq(++test_case, __LINE__, errno, 0);

    errno = 0;
    assert_null(++test_case, __LINE__, s->get_value(NULL, buf));
    assert_eq(++test_case, __LINE__, errno, EINVAL);

    errno = 0;
    assert_null(++test_case, __LINE__, s->get_value(s, NULL));
    assert_eq(++test_case, __LINE__, errno, EINVAL);

    errno = 0;
    assert_null(++test_case, __LINE__, s->get_value(NULL, NULL));
    assert_eq(++test_case, __LINE__, errno, EINVAL);

    errno = 0;

    deleteString(&s);

    return test_case;
}

int test_equals() {
    int test_case = 0;

    for (int i = 0; i < 10; i++) {
        String s = create_test_str(-1);
        String t = newString(s->_val);

        assert(s);
        assert(t);

        assert_cmp(++test_case, __LINE__, s, s->length(s), t, t->length(t));

        assert_true(++test_case, __LINE__, s->equals(s, t));
        assert_true(++test_case, __LINE__, t->equals(t, s));

        assert_true(++test_case, __LINE__, s->equals(s, s));
        assert_true(++test_case, __LINE__, t->equals(t, t));

        deleteString(&s);
        deleteString(&t);
    }

    String s = newString("hello");
    String t = newString("goodbye");
    String u = newString("hellohello");

    assert(s);
    assert(t);
    assert(u);
    assert_false(++test_case, __LINE__, s->equals(s, t));
    assert_false(++test_case, __LINE__, t->equals(t, s));
    assert_false(++test_case, __LINE__, s->equals(s, u));
    assert_false(++test_case, __LINE__, s->equals(u, s));

    assert_false(++test_case, __LINE__, s->equals(s, NULL));
    assert_false(++test_case, __LINE__, s->equals(NULL, s));

    assert_false(++test_case, __LINE__, s->equals(NULL, NULL));


    deleteString(&s);
    deleteString(&t);
    deleteString(&u);

    s = newString("");
    t = newString("");

    assert(s);
    assert(t);
    assert_true(++test_case, __LINE__, s->equals(s, t));
    assert_true(++test_case, __LINE__, t->equals(t, s));

    deleteString(&s);
    deleteString(&t);

    return test_case;
}




