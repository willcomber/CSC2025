#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include "../string_o.h"

/* 
 * Program to demonstrate example usage of String (as defined in string_o.h).
 */

int main(void) {
    String s = newString("hello");  /* strictly, should check this succeeds */
    String t = newString("again");
    String sp = newString(" ");
    printf("s length: %d\n", s->length(s));
    printf("t length: %d\n", t->length(t));
    
    String u = s->add(s, sp);
    printf("u length: %d\n", u->length(u));
    
    String v = u->add(u, t);
    printf("v length: %d\n", v->length(v));
    
    char sv[s->length(s)+1];
    char tv[t->length(t)+1];
    char vv[v->length(v)+1];
    
    s->get_value(s, sv);
    t->get_value(t, tv);
    v->get_value(v, vv);
    
    printf("sv (%d): \"%s\"\n", (int) strnlen(sv, STR_LEN_MAX), sv);
    printf("tv (%d): \"%s\"\n", (int) strnlen(tv, STR_LEN_MAX), tv);
    printf("vv (%d): \"%s\"\n", (int) strnlen(vv, STR_LEN_MAX), vv);
    
    for (int i = 0; i < s->length(s); i++) 
        printf("%c\n", s->char_at(s, i));
            
    char c = s->char_at(s, s->length(s));
    
    if (!c)
        perror("char_at error");
    
    int idx = 0;
    c = 'l';
    int start = idx;
    int count =0;
    while ((idx = s->index_of(s, c, start)) != -1) {
        count++;
        printf("index of number %d %c in %s is %d\n", count, c,
            sv, idx);
        start = idx + 1;
    }
    
    String s2 = newString("hello");
    
    printf("s.equals(s2) is %s\n", s->equals(s, s2) ? "true" : "false");
        
    deleteString(&s);
    deleteString(&s2);
    deleteString(&t);
    deleteString(&sp);
    deleteString(&u);
    deleteString(&v);    

    return 0;
} 