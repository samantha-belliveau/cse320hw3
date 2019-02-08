#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "budmm.h"

int main(int argc, char const *argv[]) {
    int i;

    bud_mem_init();

    char* ptr1 = (char*)bud_malloc(10); // 64

    ptr1[0] = 'a';
    ptr1[1] = 'b';
    ptr1[2] = 'c';
    ptr1[3] = 'd';
    ptr1[4] = 'e';
    ptr1[5] = 'f';
    ptr1[6] = '\0';
    printf("ptr1 string: %s\n", ptr1);


    char* ptr11 = (char*)bud_malloc(10); // 64

    ptr11[0] = 'g';
    ptr11[1] = 'h';
    ptr11[2] = 'i';
    ptr11[3] = 'j';
    ptr11[4] = 'k';
    ptr11[5] = 'l';
    ptr11[6] = '\0';
    printf("ptr1 string: %s\n", ptr1);
    printf("ptr11 string: %s\n", ptr11);


    char* ptr12 = (char*)bud_malloc(10); // 64

    ptr12[0] = 'm';
    ptr12[1] = 'n';
    ptr12[2] = 'o';
    ptr12[3] = 'p';
    ptr12[4] = 'q';
    ptr12[5] = 'r';
    ptr12[6] = '\0';
    printf("ptr1 string: %s\n", ptr1);
    printf("ptr11 string: %s\n", ptr11);
    printf("ptr12 string: %s\n", ptr12);


    int* ptr2 = bud_malloc(sizeof(int) * 100); // 512
    for(i = 0; i < 100; i++){
        //printf("%d\n", i);
        ptr2[i] = i;
    }

    for (i = 0; i < 100; i++){
        //printf("array[%d]: %d\n", i, ptr2[i]);
    }


    void* ptr3 = bud_malloc(3000); // 4192
    printf("ptr3: %p\n", ptr3);

    //bud_free(ptr2);

    ptr2 = bud_realloc(ptr2, 124); // 128

    ptr1 = bud_malloc(200); // 256

    ptr1 = bud_realloc(ptr1, 100); // 128

    // intentional error (errno = EINVAL)
    ptr3 = bud_malloc(20000);
    printf("errno: %d (%s)\n", errno, strerror(errno));

    bud_mem_fini();

    return EXIT_SUCCESS;
}
