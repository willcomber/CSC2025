//
// Created by Will on 02/11/2018.
//
#include <stdlib.h>
#include <stdio.h>


int main(void) {

    int r, len;

    for (int i = 0; i <10 ; i++) {
        r = rand();
        len = r % 2 ? r % (1023 * 2) : r % (1023 / 2);
        printf("%d\n",len);
    }

    return 0;
}