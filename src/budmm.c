/*
 * All functions you make for the assignment must be implemented in this file.
 * Do not submit your assignment with a main function in this file.
 * If you submit with a main function in this file, you will get a zero.
 */
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "debug.h"
#include "budmm.h"

/*
 * You should store the heads of your free lists in these variables.
 * Doing so will make it accessible via the extern statement in budmm.h
 * which will allow you to pass the address to sf_snapshot in a different file.
 */
extern bud_free_block free_list_heads[NUM_FREE_LIST];

void *bud_malloc(uint32_t rsize) {
    if (rsize == 0 || rsize > (MAX_BLOCK_SIZE - sizeof(bud_header))){
        //printf("invalid size\n");
        errno = EINVAL;
        return NULL;
    }
    uint32_t order = ORDER_MIN;
    while (rsize + sizeof(bud_header) > ORDER_TO_BLOCK_SIZE(order)){
        order++;
    }
    int notFound = 1;

    int i = order - ORDER_MIN;
    bud_free_block tester = free_list_heads[i];
    while(notFound && (i < NUM_FREE_LIST)){
        //printf("next and prev and test: %p, %p, %p ", tester.next, tester.prev, &free_list_heads[i]);

        if (tester.next == tester.prev && tester.next == &free_list_heads[i]){
            //printf("no block at order %d\n", i + ORDER_MIN);
            i++;
            tester = free_list_heads[i];
            continue;
        }
        else{
            notFound = 0;
           // printf("found block at order %d\n", i + ORDER_MIN);
        }
    }

    // no free blocks of proper size
    if (i == NUM_FREE_LIST){
        void *newBlock = bud_sbrk();
        bud_free_block *newFreeBlock;

        // check if sbrk failed
        if (newBlock == (void *)-1){
            errno = ENOMEM;
            return NULL;
        }
       // printf("new mem\n");
        int j = order - ORDER_MIN;
        int k = NUM_FREE_LIST - 1;
        while (k > j){

            k--;

            newFreeBlock = newBlock + ORDER_TO_BLOCK_SIZE(k + ORDER_MIN);

            (*newFreeBlock).next = free_list_heads[k].next;
            (*newFreeBlock).prev = &free_list_heads[k];
            ((*newFreeBlock).header).allocated = 0;
            ((*newFreeBlock).header).order = k + ORDER_MIN;

            free_list_heads[k].next = newFreeBlock;
            //printf("next and prev: %p, %p", free_list_heads[k].next, free_list_heads[k].prev);

            if (free_list_heads[k].prev == &free_list_heads[k]){
                free_list_heads[k].prev = newFreeBlock;
            }
            //printf(" added block to order %d\n", k + ORDER_MIN);

        }
        bud_header *allocatedBlock = newBlock;
        allocatedBlock->allocated = 1;
        allocatedBlock->order = order;
        allocatedBlock->rsize = rsize;
        if (rsize + sizeof(bud_header) < ORDER_TO_BLOCK_SIZE(order)){
            allocatedBlock->padded = 1;
        }
        else{
            allocatedBlock->padded = 0;
        }
       /* bud_free_block *allocatedBlock = newBlock;
        ((*allocatedBlock).header).allocated = 1;
        ((*allocatedBlock).header).order = k + ORDER_MIN;
        ((*allocatedBlock).header).rsize = rsize;
        if (rsize + sizeof(bud_header) < ORDER_TO_BLOCK_SIZE(k + ORDER_MIN)){
            ((*allocatedBlock).header).padded = 1;
        }
        else{
            ((*allocatedBlock).header).padded = 0;
        }*/

        newBlock = newBlock + sizeof(bud_header);

        return newBlock;
    }
    else if (i+ORDER_MIN == order){
       // printf("here\n");
        bud_free_block *freeBlock = free_list_heads[i].next;
        //bud_header allocatedBlock = freeBlock->header;
        // remove this block from the free list
        free_list_heads[i].next = (*free_list_heads[i].next).next;
        if (free_list_heads[i].next == &free_list_heads[i]){
            free_list_heads[i].prev = &free_list_heads[i];
        }

        //bud_header *allocatedBlock = blockToSplit;
        (freeBlock->header).allocated = 1;
        (freeBlock->header).order = i + ORDER_MIN;
        (freeBlock->header).rsize = rsize;
        if (rsize + sizeof(bud_header) < ORDER_TO_BLOCK_SIZE(i + ORDER_MIN)){
            (freeBlock->header).padded = 1;
        }
        else{
            (freeBlock->header).padded = 0;
        }

      /*  ((*allocatedBlock).header).allocated = 1;
        ((*allocatedBlock).header).order = i + ORDER_MIN;
        ((*allocatedBlock).header).rsize = rsize;
        if (rsize + sizeof(bud_header) < ORDER_TO_BLOCK_SIZE(i + ORDER_MIN)){
            ((*allocatedBlock).header).padded = 1;
        }
        else{
            ((*allocatedBlock).header).padded = 0;
        }*/

        void *newBlock = freeBlock;
        newBlock = newBlock + sizeof(bud_header);

       // printf("here\n");
        return newBlock;

    }
    else if (i + ORDER_MIN > order){
        bud_free_block *newFreeBlock;
        void *blockToSplit = free_list_heads[i].next;

        int j = order - ORDER_MIN;
        int k = i;
        while (k > j){

            // remove block to split from the free list
            free_list_heads[i].next = (*free_list_heads[i].next).next;
            if (free_list_heads[i].next == &free_list_heads[i]){
                free_list_heads[i].prev = &free_list_heads[i];
            }
            k--;

            newFreeBlock =  blockToSplit + ORDER_TO_BLOCK_SIZE(k + ORDER_MIN);

            (*newFreeBlock).next = free_list_heads[k].next;
            (*newFreeBlock).prev = &free_list_heads[k];
            ((*newFreeBlock).header).allocated = 0;
            ((*newFreeBlock).header).order = k + ORDER_MIN;

            free_list_heads[k].next = newFreeBlock;
         //   printf("next and prev: %p, %p", free_list_heads[k].next, free_list_heads[k].prev);

            if (free_list_heads[k].prev == &free_list_heads[k]){
                free_list_heads[k].prev = newFreeBlock;
            }
          //  printf("added block to order %d\n", k + ORDER_MIN);

        }

        bud_header *allocatedBlock = blockToSplit;
        allocatedBlock->allocated = 1;
        allocatedBlock->order = order;
        allocatedBlock->rsize = rsize;
        if (rsize + sizeof(bud_header) < ORDER_TO_BLOCK_SIZE(order)){
            allocatedBlock->padded = 1;
        }
        else{
            allocatedBlock->padded = 0;
        }
        //bud_free_block *allocatedBlock = blockToSplit;

       // ((*allocatedBlock).header).allocated = 1;
       // ((*allocatedBlock).header).order = i + ORDER_MIN;
       // ((*allocatedBlock).header).rsize = rsize;
      //  if (rsize + sizeof(bud_header) < ORDER_TO_BLOCK_SIZE(i + ORDER_MIN)){
      //      ((*allocatedBlock).header).padded = 1;
      //  }
      //  else{
     //       ((*allocatedBlock).header).padded = 0;
      //  }

        void *newBlock = allocatedBlock;
        newBlock = newBlock + sizeof(bud_header);

        //printf("header order: %d\n", (*allocatedBlock).header)
        return newBlock;
    }
    //printf("Order: %d\n", order);

    return NULL;
}

void *bud_realloc(void *ptr, uint32_t rsize) {
    if (rsize == 0){
        bud_free(ptr);
        return NULL;
    }
    if(ptr == NULL){
        return bud_malloc(rsize);
    }

    // check if address in heap address range
    if (ptr < bud_heap_start() || ptr >= bud_heap_end()){
        abort();
    }

    // check if address is multiple of sizeof(bud_header)

    int temp = -1;
    int temp1 = 0;
    uintptr_t temp2 = sizeof(bud_header);
    while (temp2 > 1){
        temp2 = temp2/2;
        temp1++;
    }
    temp2 = temp >> temp1;
    temp2 = temp2 << temp1;
    temp2 = ~temp2;
 //   printf("temp: %d\n", temp);
 //   printf("size of ptr: %zu\n", sizeof(ptr));
 //   printf("size of int: %zu\n", sizeof(unsigned int));
 //   printf("pointer: %p\n", ptr);


    void *pointer = ptr;
    //pointer = pointer - sizeof(bud_header);
    temp2 = (uintptr_t)temp2 & (uintptr_t)pointer;
 //   printf("temp: %zu\n", temp2);

    if (temp2 > 0){
 //       printf("invalid pointer\n");
        abort();
    }

    pointer = pointer - sizeof(bud_header);

    // check if order is within range
    bud_header *headerPtr = pointer;
    int order = headerPtr->order;
 //   printf("order: %d\n", order);
    if (order < ORDER_MIN || order >= ORDER_MAX){
  //      printf("invalid order\n");
        abort();
    }


    // check if alloated flag == 0
    if (headerPtr->allocated == 0){
   //     printf("not allcated\n");
        abort();
    }

    // check if correct padding flag
    int checkPadding = headerPtr->rsize + sizeof(bud_header);
    if (checkPadding == ORDER_TO_BLOCK_SIZE(order) && headerPtr->padded == 1){
     //   printf("Header says there is padding, but no padding found\n");
        abort();
    }
    if (checkPadding != ORDER_TO_BLOCK_SIZE(order) && headerPtr->padded == 0){
    //    printf("Header says there is no padding, but padding is needed\n");
        abort();
    }

    // check rsize corresponds to order
    // rsize + sizeof(bud_header) <= ORDER_TO_BLOCK_SIZE(order)
    // rsize + sizeof(bud_header) > ORDER_TO_BLOCK_SIZE(order-1);
    if (headerPtr->rsize + sizeof(bud_header) > ORDER_TO_BLOCK_SIZE(order)){
    //    printf("order is too small\n");
        abort();
    }
    if (headerPtr->rsize + sizeof(bud_header) <= ORDER_TO_BLOCK_SIZE(order-1)){
        if (order != 5){
    //        printf("order is too big\n");
            abort();
        }

    }

    // check if valid size
    if (rsize > (MAX_BLOCK_SIZE - sizeof(bud_header))){
        //printf("invalid size\n");
        errno = EINVAL;
        return NULL;
    }

    int prevOrder = headerPtr->order;
    // calculate required order for rsize
    order = ORDER_MIN;
    while (rsize + sizeof(bud_header) > ORDER_TO_BLOCK_SIZE(order)){
        order++;
    }

    if (order == prevOrder){
        headerPtr->rsize = rsize;
        //printf("padding?: %lu < %d\n", rsize + sizeof(bud_header), ORDER_TO_BLOCK_SIZE(order));
        if (rsize + sizeof(bud_header) < ORDER_TO_BLOCK_SIZE(order)){
            headerPtr->padded = 1;
        }
        else{

            headerPtr->padded = 0;
        }
        return ptr;
    }
    void *newPtr = ptr;
    if (order > prevOrder){
        newPtr = bud_malloc(rsize);
        if (newPtr == NULL){
            errno = ENOMEM;
            return NULL;
        }
        memcpy(newPtr, ptr, headerPtr->rsize);

        bud_free(ptr);
        return newPtr;

    }
    // pointer = addr of the header (void *)
    // headerPtr = addr of the header (bud_header *)
    if (order < prevOrder){
        int counter = prevOrder;

        while (counter > order){
            counter--;
            bud_free_block *newFree = pointer + ORDER_TO_BLOCK_SIZE(counter);

            int k = counter - ORDER_MIN;

            (*newFree).next = free_list_heads[k].next;
            (*newFree).prev = &free_list_heads[k];
            ((*newFree).header).allocated = 0;
            ((*newFree).header).order = k + ORDER_MIN;


            (free_list_heads[k].next)->prev = newFree;
            free_list_heads[k].next = newFree;

        //    printf("next and prev: %p, %p", free_list_heads[k].next, free_list_heads[k].prev);

            if (free_list_heads[k].prev == &free_list_heads[k]){
                free_list_heads[k].prev = newFree;
            }
        //    printf("added block to order %d\n", k + ORDER_MIN);
        }

        headerPtr->order = order;
        headerPtr->rsize = rsize;
        headerPtr->allocated = 1;

        if (rsize + sizeof(bud_header) < ORDER_TO_BLOCK_SIZE(order)){
            headerPtr->padded = 1;
        }
        else{
            headerPtr->padded = 0;
        }

        return ptr;
    }

    return NULL;
}

void bud_free(void *ptr) {
    // check if address in heap address range
    if (ptr < bud_heap_start() || ptr >= bud_heap_end()){
        abort();
    }

    // check if address is multiple of sizeof(bud_header)

    int temp = -1;
    int temp1 = 0;
    uintptr_t temp2 = sizeof(bud_header);
    while (temp2 > 1){
        temp2 = temp2/2;
        temp1++;
    }
    temp2 = temp >> temp1;
    temp2 = temp2 << temp1;
    temp2 = ~temp2;
  //  printf("temp: %d\n", temp);
  //  printf("size of ptr: %zu\n", sizeof(ptr));
  //  printf("size of int: %zu\n", sizeof(unsigned int));
  //  printf("pointer: %p\n", ptr);


    void *pointer = ptr;
    //pointer = pointer - sizeof(bud_header);
    temp2 = (uintptr_t)temp2 & (uintptr_t)pointer;
  //  printf("temp: %zu\n", temp2);

    if (temp2 > 0){
    //    printf("invalid pointer\n");
        abort();
    }

    pointer = pointer - sizeof(bud_header);

    // check if order is within range
    bud_header *headerPtr = pointer;
    int order = headerPtr->order;
  //  printf("order: %d\n", order);
    if (order < ORDER_MIN || order >= ORDER_MAX){
    //    printf("invalid order\n");
        abort();
    }


    // check if alloated flag == 0
    if (headerPtr->allocated == 0){
    //    printf("not allcated\n");
        abort();
    }

    // check if correct padding flag
    int checkPadding = headerPtr->rsize + sizeof(bud_header);
    if (checkPadding == ORDER_TO_BLOCK_SIZE(order) && headerPtr->padded == 1){
    //    printf("Header says there is padding, but no padding found\n");
        abort();
    }
    if (checkPadding != ORDER_TO_BLOCK_SIZE(order) && headerPtr->padded == 0){
    //    printf("Header says there is no padding, but padding is needed\n");
        abort();
    }

    // check rsize corresponds to order
    // rsize + sizeof(bud_header) <= ORDER_TO_BLOCK_SIZE(order)
    // rsize + sizeof(bud_header) > ORDER_TO_BLOCK_SIZE(order-1);
    if (headerPtr->rsize + sizeof(bud_header) > ORDER_TO_BLOCK_SIZE(order)){
    //    printf("order is too small\n");
        abort();
    }
    if (headerPtr->rsize + sizeof(bud_header) <= ORDER_TO_BLOCK_SIZE(order-1)){
        if (order != 5){
    //        printf("order is too big\n");
            abort();
        }

    }

    // add this block to the free list
    headerPtr->allocated = 0;
    bud_free_block *newFree = pointer;
    int listPos = headerPtr->order - ORDER_MIN;
   // printf("listPos: %d\n", listPos);
    //bud_free_block sentinel = free_list_heads[listPos];

    /*newFree->prev = &free_list_heads[listPos];
    newFree->next = free_list_heads[listPos].next;

    free_list_heads[listPos].next = newFree;

    if (free_list_heads[listPos].prev == &free_list_heads[listPos]){
        free_list_heads[listPos].prev = newFree;
    }*/

    int notDoneCoal = 1;
    int notDoneCoal1 = 1;
    //check for coalescing
   // printf("newFree: %p\n", newFree);

    while(listPos < ORDER_MAX - ORDER_MIN && (notDoneCoal == 1)){


        bud_free_block *oldFree = free_list_heads[listPos].next;//newFree->next;
        notDoneCoal1 = 1;
        while(oldFree != &free_list_heads[listPos]){ //&& notDoneCoal1){
            //printf("notdone1: %d\n", notDoneCoal1);
            void *temp1 = newFree;
            void *temp2 = oldFree;
            temp1 = (void *)((uintptr_t)temp1^(uintptr_t)ORDER_TO_BLOCK_SIZE((newFree->header).order)); // newFree + size
            temp2 = (void *)((uintptr_t)temp2^(uintptr_t)ORDER_TO_BLOCK_SIZE((newFree->header).order)); // oldFree + size

    //        printf("(newFree->header).order: %d\n", (newFree->header).order);
    //        printf("newFree + blockSize: %p\n", temp1);
    //        printf("oldFree: %p\n", oldFree);
    //        printf("oldFree + blockSize: %p\n", temp2);
    //        printf("newFree: %p\n", newFree);


            // check if they are buddies
            if (temp1 == oldFree || temp2 == newFree){
                notDoneCoal1 = 0;
                //printf("notdone1: %d\n", notDoneCoal1);

                // if newFree + size == oldFree i.e., newFree is left buddy
                if (newFree < oldFree){
                    // remove oldFree from list
                    bud_free_block *prev = oldFree->prev;
                    bud_free_block *next = oldFree->next;
                    prev->next = next;
                    next->prev = prev;
            //        printf("oldFree prev: %p\n", oldFree->prev);
            //        printf("oldFree next: %p\n", oldFree->next);
                    // increase order of newFree
                    (newFree->header).order = (newFree->header).order + 1;

                    listPos++;
            //        printf("coalescing and adding block to order %d\n", listPos+ORDER_MIN);

                    /*// add newFree to next pos in free list
                    newFree->prev = &free_list_heads[listPos];
                    newFree->next = free_list_heads[listPos].next;

                    free_list_heads[listPos].next = newFree;

                    if (free_list_heads[listPos].prev == &free_list_heads[listPos]){
                        free_list_heads[listPos].prev = newFree;
                    }*/
                }
                // if (oldFree + size == newFree) i.e., oldFree is left buddy
                else if (newFree > oldFree){
                    // remove oldFree from list

                    bud_free_block *prev = oldFree->prev;
                    bud_free_block *next = oldFree->next;
                    (oldFree->prev)->next = oldFree->next;
                    (oldFree->next)->prev = oldFree->prev;
                    //printf("oldFree prev: %p\n", oldFree->prev);

                    // increase order of oldFree
                    (oldFree->header).order = (oldFree->header).order + 1;

                    listPos++;

                //    printf("coalescing and adding block to order%d\n", listPos+ORDER_MIN);


                    newFree = oldFree;

                    /*// add oldFree to next pos in free list
                    oldFree->prev = &free_list_heads[listPos];
                    oldFree->next = free_list_heads[listPos].next;

                    free_list_heads[listPos].next = oldFree;

                    if (free_list_heads[listPos].prev == &free_list_heads[listPos]){
                        free_list_heads[listPos].prev = oldFree;
                    }*/

                    //newFree = oldFree;
                }
                // coalesce

                break;
            }
            oldFree = oldFree->next;
        //    printf("oldFree: %p\n", oldFree);
            //printf("notdone1: %d\n", notDoneCoal1);
        }
        //break;
        // if there was no buddy found on this level, don't go to the next, add this block to this level
        if (notDoneCoal1 == 1 || listPos == ORDER_MAX - ORDER_MIN - 1){
            notDoneCoal = 0;
            // add newFree to next pos in free list
            newFree->prev = &free_list_heads[listPos];
            newFree->next = free_list_heads[listPos].next;

            (free_list_heads[listPos].next)->prev = newFree;
            free_list_heads[listPos].next = newFree;

            if (free_list_heads[listPos].prev == &free_list_heads[listPos]){
                free_list_heads[listPos].prev = newFree;
            }
            if (notDoneCoal1 == 1){
        //        printf("adding to order %d\n", listPos + ORDER_MIN);
            }
        }
        //newFree = oldFree;
    }

    return;
}
