#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include "strtest_lib.h"
#include "test_lib.h"


/* implementation of utility functions for string (and obj_store) tests */

char alphabet[] = "abcdefghijklmnopqrstuvwxyz";

int create_test_buf(int len, char **out_buf) {
    if (len == -1) {
        int r = rand();
        len = r % 2 ? r % (STR_LEN_MAX * 2) : r % (STR_LEN_MAX / 2);
    } else if (len < 1) {
        len = STR_LEN_MAX;
    }

    char *buf = (char *) calloc(len + 1, 1);

    for (int i = 0; i < len; i++)
        buf[i] = alphabet[rand() % 26];

    buf[len] = '\0';

    *out_buf = buf;

    return len;
}

String create_test_str(int len) {
    char *buf;

    len = create_test_buf(len, &buf);

    String s = newString(buf);

    int exp_len = len > STR_LEN_MAX ? STR_LEN_MAX : len;

    assert(s);
    assert(exp_len == s->length(s));
    assert(buf != s->_val);
    assert(strncmp(buf, s->_val, exp_len) == 0);

    free(buf);

    return s;
}

void assert_newString(int test_case, int line_num, int len, char *buf) {

    int exp_len = len > STR_LEN_MAX ? STR_LEN_MAX : len;

    String s = newString(buf);

    assert_notnull(test_case, line_num, s);
    assert_notnull(test_case, line_num, s->_val);
    assert_eq(test_case, line_num, s->length(s), exp_len);
    assert_eq(test_case, line_num, strncmp(buf, s->_val, exp_len), 0);
    assert_notidentical(test_case, line_num, buf, s->_val);

    deleteString(&s);
}

void assert_cmp(int test_case, int line_num, String act, int exp_len,
                String exp, int cmp_len) {
    assert_notnull(test_case, line_num, act);
    assert_eq(test_case, line_num, act->length(act), exp_len);
    assert_eq(test_case, line_num, strncmp(act->_val, exp->_val, cmp_len), 0);
}
