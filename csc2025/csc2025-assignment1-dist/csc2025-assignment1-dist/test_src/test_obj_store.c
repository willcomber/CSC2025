#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "test_lib.h"
#include "strtest_lib.h"
#include "../string_o.h"
#include "../integer.h"
#include "../obj_store.h"

extern const char* OSTORE_DIR;
extern const char* OFILE_FMT;

#define NR_TESTS 3

/* test functions */
int test_dis_enable_is_on();
int test_store_unlink_norm();
int test_store_unlink_err();

struct test_defn test_schedule[NR_TESTS] = {
    /* test 0 */
    { "test_dis/enable_ostore+ostore_is_on", test_dis_enable_is_on, 6 },
    /* test 1 */
    { "test_store_obj+unlink_obj_norm", test_store_unlink_norm, 169 },
    /* test 2 */
    { "test_store_obj+unlink_obj_err", test_store_unlink_err, 10 },
};

/* helper functions */
int assert_written(int test_case, int line_num, void* oid, char* data);
int assert_unlinked(int test_case, int line_num, char* ofile);
int assert_disabled(int test_case, int line_num);

int main(int argc, char** argv) {
    run_tests(argc, argv, NR_TESTS, test_schedule, true);
    
    return 0;
}

/* create dummy string rep for tests */
char *_new_dummy_str_rep(void* obj) {
    char* str_rep = NULL;
    
    if (obj)
        (void) asprintf(&str_rep, "%s",  "test");

    return str_rep;
}


int test_dis_enable_is_on() {
    int test_case = 0;
    
    disable_ostore();
    
    test_case = assert_disabled(test_case, __LINE__);
    
    if (enable_ostore()) {
        assert_true(++test_case, __LINE__, ostore_is_on());
        struct stat sbuf;

        int r = stat(OSTORE_DIR, &sbuf);
        assert_eq(++test_case, __LINE__, r, 0);
        assert_true(++test_case, __LINE__, S_ISDIR(sbuf.st_mode));
        
        disable_ostore();
        (void) assert_disabled(test_case, __LINE__);
    } else {
        perror("Could not create ostore");
        exit(EXIT_FAILURE);
    }

    return test_case;
}

int test_store_unlink_norm() {
    int test_case = 0;
    char *ofile = NULL;
    char *str_rep = NULL;
    
    errno = 0;
    if (!ostore_is_on() && !enable_ostore()) {
        perror("Could not enable ostore");
        exit(EXIT_FAILURE);
    }        

    bool stored = store_obj(&test_case, _new_dummy_str_rep);
    assert_true(++test_case, __LINE__, stored);
    test_case = assert_written(test_case, __LINE__, &test_case, "test");

    (void) asprintf(&ofile, OFILE_FMT, OSTORE_DIR, &test_case);
    unlink_obj(&test_case);
    test_case = assert_unlinked(test_case, __LINE__, ofile);
    
    for (int i = 0; i < 10; i++) {
        String s = create_test_str(-1);
        assert(s);
    
        char sbuf[s->length(s) + 1];
    
        (void) asprintf(&str_rep, "str:%d:%s\n", s->length(s), 
                s->get_value(s, sbuf)); 
    
        assert(str_rep);
    
        test_case = assert_written(test_case, __LINE__, s, str_rep);
        
        (void) asprintf(&ofile, OFILE_FMT, OSTORE_DIR, s);
    
        free(str_rep);
        deleteString(&s);
        
        test_case = assert_unlinked(test_case, __LINE__, ofile);
    }
    
    for (int i = 0; i < 10; i++) {
        Integer oi = newInteger(rand());
        assert(oi);
    
        (void) asprintf(&str_rep, "int:%d\n", oi->get_value(oi));
    
        assert(str_rep);
    
        test_case = assert_written(test_case, __LINE__, oi, str_rep);
        
        (void) asprintf(&ofile, OFILE_FMT, OSTORE_DIR, oi);
        
        free(str_rep);
        deleteInteger(&oi);
        
        test_case = assert_unlinked(test_case, __LINE__, ofile);
    }
    
    disable_ostore();
    (void) assert_disabled(test_case, __LINE__);

    return test_case;
}

int test_store_unlink_err() {
    int test_case = 0;

    errno = 0;
    if (!ostore_is_on() && !enable_ostore()) {
        perror("Could not enable ostore");
        exit(EXIT_FAILURE);
    }        

    errno = 0;
    assert_false(++test_case, __LINE__, store_obj(&test_case, NULL));
    assert_eq(++test_case, __LINE__, errno, EINVAL);
    
    char *ofile = NULL;
    (void) asprintf(&ofile, OFILE_FMT, OSTORE_DIR, &test_case);
    test_case = assert_unlinked(test_case, __LINE__, ofile);
    
    errno = 0;
    assert_false(++test_case, __LINE__, store_obj(NULL, _new_dummy_str_rep));
    assert_eq(++test_case, __LINE__, errno, EINVAL);

    errno = 0;
    assert_false(++test_case, __LINE__, store_obj(NULL, NULL));
    assert_eq(++test_case, __LINE__, errno, EINVAL);
    
    errno = 0;
    
    unlink_obj(NULL);       /* no-op 
                             * will cause failure if not implemented correctly 
                             */
                             
    disable_ostore();
    (void) assert_disabled(test_case, __LINE__);
    
    errno = 0;
    assert_false(++test_case, __LINE__, 
                            store_obj(&test_case, _new_dummy_str_rep));
    assert_eq(++test_case, __LINE__, errno, ENOENT);
    
    unlink_obj(&test_case); /* no-op 
                             * will cause failure if not implemented correctly 
                             */
                             
    return test_case;
}

/* helper functions */
int assert_written(int test_case, int line_num, void* oid, char* data) {
    char* ofile = NULL;
    size_t data_len = strlen(data);
    struct stat sbuf;
    
    (void) asprintf(&ofile, OFILE_FMT, OSTORE_DIR, oid);
    
    assert_notnull(++test_case, __LINE__, ofile);
    
    int r = stat(ofile, &sbuf);
    
    assert_eq(++test_case, __LINE__, r, 0);
    assert_true(++test_case, __LINE__, S_ISREG(sbuf.st_mode));
    
    int fd = open(ofile, O_RDONLY, 0644);

    free(ofile);
    
    assert_true(++test_case, __LINE__, fd > 0);
    
    char rbuf[data_len + 1];
    
    ssize_t bytes_read = read(fd, rbuf, data_len);

    close(fd);

    assert_eq(++test_case, __LINE__, bytes_read, data_len);
    
    rbuf[bytes_read] = '\0';
    
    assert_eq(++test_case, __LINE__, strncmp(rbuf, data, data_len), 0);
            
    return test_case;
}

int assert_unlinked(int test_case, int line_num, char* ofile) {
    struct stat sbuf;
    
    errno = 0;
    int r = stat(ofile, &sbuf);

    assert_eq(++test_case, line_num, r, -1);
    assert_eq(++test_case, line_num, errno, ENOENT);
    
    free(ofile);
    
    errno = 0;
    
    return test_case;
}

int assert_disabled(int test_case, int line_num) {
    struct stat sbuf;
    
    errno = 0;
    int r = stat(OSTORE_DIR, &sbuf);
    
    assert_eq(++test_case, line_num, r, -1);
    assert_eq(++test_case, line_num, errno, ENOENT);
    
    errno = 0;
    
    assert_false(++test_case, line_num, ostore_is_on());
    
    return test_case;
}


