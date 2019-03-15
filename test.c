
#include <stdio.h>
#include "alist.h"
#include "sockutil.h"

int main() {

    network_node nn2 = {"kek1", 0, {AF_INET, htons(3180), {10}, 1}};
    network_node nn3 = {"kek2", 0, {AF_INET, htons(3280), {10}, 2}};
    network_node nn4 = {"kek3", 0, {AF_INET, htons(3480), {10}, 3}};
    network_node nn5 = {"kek4", 0, {AF_INET, htons(3780), {10}, 4}};

    p_array_list alist = create_array_list(2);

    array_list_add(alist, &nn2);
    array_list_add(alist, &nn3);
    array_list_add(alist, &nn4);
    array_list_add(alist, &nn5);

    printf("size: %ld, count: %ld\n", alist->size, alist->count);
    int is_shtf;
    size_t length;

    void * serialized = array_list_serialize(alist, &length);

    p_array_list alist2 = array_list_deserialise(serialized);

    size_t iter = array_list_iter(alist2, &is_shtf);
    while(is_shtf >= 0) {
        printf("%s ", array_list_get(alist2, iter, &is_shtf)->name);
        iter = array_list_next(alist2, iter, &is_shtf);
    }
    printf("\n");

    delete_array_list(alist);
    delete_array_list(alist2);
    return 0;
}
