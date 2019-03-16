
#include <stdio.h>
#include "array_list/alist.h"
#include <netdb.h>
#include <memory.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ffi.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <string.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <errno.h>


int main() {

    network_node nn2 = {"kek1", 0, {AF_INET, htons(3180), {10}, 1}};
    array_list_add_file(&nn2, "ffghf");
    array_list_add_file(&nn2, "khhhkk");
    network_node nn3 = {"kek2", 0, {AF_INET, htons(3280), {10}, 2}};
    array_list_add_file(&nn3, "fffgh");
    array_list_add_file(&nn3, "kfghkk");
    array_list_remove_file(&nn3, "fffgh");
    network_node nn4 = {"kek3", 0, {AF_INET, htons(3480), {10}, 3}};
    array_list_add_file(&nn4, "ffggg");
    array_list_add_file(&nn4, "kkfghk");
    network_node nn5 = {"kek4", 0, {AF_INET, htons(3780), {10}, 4}};
    array_list_add_file(&nn5, "ffgfgh");
    array_list_add_file(&nn5, "kkghghk");

    p_array_list alist = create_array_list();

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
        printf("%s ", array_list_get(alist2, iter, &is_shtf)->file_list[0]);
        iter = array_list_next(alist2, iter, &is_shtf);
    }
    printf("\n");

    delete_array_list(alist);
    delete_array_list(alist2);
    return 0;
}
