/*
 * Replace the following string of 0s with your student number
 * 170168146
 */
#include <stdbool.h>
#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include "obj_store.h"
#include "string_o.h"

/* Prototype of private _add function prototype for implementation of the add 
 * member of struct integer 
 */
static String _add(String self, String s);

/* Prototype of private _char_at function prototype for implementation of the 
 * char_at member of struct integer 
 */
static char _char_at(String self, int posn);

/* Prototype of private _equals function prototype for implementation of the
 * equals member of struct integer 
 */
static bool _equals(String self, String s);

/* Prototype of private _get_value function prototype for implementation of 
 * the get_value member of struct integer 
 */
static char *_get_value(String self, char *buf);

/* Prototype of private _index_of function prototype for implementation of the
 * index_of member of struct integer 
 */
static int _index_of(String self, char c, int start);

/* Prototype of private _length function prototype for implementation of the
 * length member of struct integer 
 */
static int _length(String self);

/* the string representation of a String for saving to file */
static char *STR_REP_FMT = "str:%d:%s\n";

/* private _new_str_rep to dynamically allocate a new string representation 
 * of a String.
 * Pass a pointer to this function to the store_obj function of the obj_store
 * library.
 * Do NOT change this function.
 */
static char *_new_str_rep(void *obj) {
    char *str_rep = NULL;

    if (obj) {
        String s = (String) obj;
        /* asprintf allocates new string */
        (void) asprintf(&str_rep, STR_REP_FMT, s->_len, s->_val);
    }

    return str_rep;
}

/* 
 * TODO: IMPLEMENT newString
 * See comments to the newString function declaration in string_o.h for the
 * specification of this function. Also see the comments below in the body of
 * the function.
 */
String newString(char *value) {
    String self = NULL;

    if (!value) {
        errno = EINVAL;
        return NULL;
    }
    int size = strnlen(value, STR_LEN_MAX);

    if (malloc(sizeof(struct string)) == 0) {
        errno = ENOMEM;
        return NULL;
    }

    self = (String) malloc(sizeof(struct string));

    if (self) {
        char *c = strndup(value, size + 1);
        self->_val = c;
        self->add = _add;
        self->char_at = _char_at;
        self->equals = _equals;
        self->get_value = _get_value;
        self->index_of = _index_of;
        self->length = _length;
        self->_len = size;
        self->get_value = _get_value;
    }

    if (ostore_is_on() && !store_obj(self, _new_str_rep)) {
        /* ostore is on but failed to store object */
        free(self);
        self = NULL;
    }

    return self;

}

/*
 * TODO:
 * - check validity of parameters
 * - dynamically allocate the struct
 * - assign appropriate values to struct members:
 *      _len - length of string
 *      _val - a new copy of the value parameter
 *      add
 *      char_at
 *      equals
 *      get_value
 *      index_of
 *      length
 * - do conditional object storage (if object store is on, use
 *      store_obj to store an the object, free dynamic allocations
 *      if storage fails)
 * return the new String pointer
 */

/* 
 * TODO: IMPLEMENT deleteString
 * See comments to the deleteString function declaration in string_o.h for the
 * specification of this function. Also see the comments below in the body of
 * the function.
 */
void deleteString(String *as) {
    if (as) {
        if (ostore_is_on())
            unlink_obj(*as);

        free(*as);

        *as = NULL;
    }

    /*
     * TODO:
     * - check validity of parameter
     * - if object storage is on, delete object from object store
     * - free memory allocations for parameter as
     */
}

/* 
 * TODO: IMPLEMENT _add
 * see comments to the add member of struct string in string_o.h for the
 * specification of this function
 */
String _add(String self, String s) {
    if (!self || !s) {
        errno = EINVAL;
        return NULL;
    }

    int bufSize = self->_len + s->_len + 1;

    char buf[bufSize];
    (void) strncpy(buf, self->_val, bufSize);
    (void) strncat(buf, s->_val, sizeof(buf) - 1 - strlen(buf));
    buf[sizeof(buf) - 1] = '\0';

    return newString(buf);
}

/* 
 * TODO: IMPLEMENT _char_at
 * see comments to the char_at member of struct string in string_o.h for the
 * specification of this function
 */
char _char_at(String self, int posn) {
    if (!self || posn < 0 || self->_len < 0) {
        errno = EINVAL;
        return 0;
    } else if (posn >= self->_len && self->_len != 0) {
        errno = EINVAL;
        return 0;
    } else if(self->_len == 0){
        return 0;
    }else {
        return self->_val[posn];
    }
}

/* 
 * TODO: IMPLEMENT _equals
 * see comments to the equals member of struct string in string_o.h for the
 * specification of this function
 */
bool _equals(String self, String s) {
    if (!self || !s) {
        errno = EINVAL;
        return false;
    }
    return strcmp(self->_val, s->_val) == 0;
}

/* 
 * TODO: IMPLEMENT _get_value
 * see comments to the get_value member of struct string in string_o.h for the
 * specification of this function
 */
char *_get_value(String self, char *buf) {
    if (self && buf) {
        char* c = (char *) strncpy(buf, self->_val, self->_len);
        c[sizeof(c)] = "\0";
        return c;
    }
}

/* 
 * TODO: IMPLEMENT _index_of
 * see comments to the index_of member of struct string in string_o.h for the
 * specification of this function
 */
int _index_of(String self, char c, int start) {
    return -1;
}

/* _length: implemented, do NOT change */
int _length(String self) {
    if (!self) {
        errno = EINVAL;
        return -1;
    }

    return self->_len;
}
