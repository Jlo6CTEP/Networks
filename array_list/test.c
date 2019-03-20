
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

    network_node * nn2 = create_network_node();
    strcpy(nn2->name, "kek");
    nn2->node_address = (struct sockaddr_in){AF_INET, htons(3280), {10}, 2};
    size_t ekk1;
    size_t f;


    array_list_add_file(nn2, "ffghf");
    array_list_add_file(nn2, "khhhkk");
    array_list_add_file(nn2, "ffghf");
    array_list_add_file(nn2, "khhhkk");
    array_list_add_file(nn2, "ffghf");
    array_list_add_file(nn2, "khhhkk");

    network_node * nn3 = create_network_node();
    strcpy(nn3->name, "kek2");
    nn3->node_address = (struct sockaddr_in){AF_INET, htons(3280), {10}, 2};
    array_list_add_file(nn3, "fffgh");
    array_list_add_file(nn3, "kfghkk");
    array_list_remove_file(nn3, "fffgh");

    network_node * nn4 = create_network_node();
    strcpy(nn4->name, "kek3");
    nn4->node_address = (struct sockaddr_in){AF_INET, htons(3280), {10}, 2};
    array_list_add_file(nn4, "ffggg");
    array_list_add_file(nn4, "kkfghk");

    network_node * nn5 = create_network_node();
    strcpy(nn5->name, "kek4");
    nn5->node_address = (struct sockaddr_in){AF_INET, htons(3280), {10}, 2};
    array_list_add_file(nn5, "ffgfgh");
    array_list_add_file(nn5, "kkghghk");

    network_node * nn6 = create_network_node();
    strcpy(nn6->name, "kek4");
    nn6->node_address = (struct sockaddr_in){AF_INET, htons(3280), {10}, 2};
    array_list_add_file(nn6, "ffgfgh");
    array_list_add_file(nn6, "kkghghk");

    network_node * nn7 = create_network_node();
    strcpy(nn7->name, "kek4");
    nn7->node_address = (struct sockaddr_in){AF_INET, htons(3280), {10}, 2};
    array_list_add_file(nn7, "ffgfgh");
    array_list_add_file(nn7, "kkghghk");

    p_array_list alist = create_array_list();

    array_list_add(alist, nn2);
    array_list_add(alist, nn3);
    array_list_add(alist, nn4);
    array_list_add(alist, nn5);
    array_list_add(alist, nn6);
    array_list_add(alist, nn7);

    printf("size: %ld, count: %ld\n", alist->size, alist->count);
    int is_shtf = 1;
    size_t length;

    void * serialized = array_list_serialize(alist, &length);
    p_array_list alist2 = array_list_deserialise(serialized);


    size_t iter = array_list_iter(alist2, &is_shtf);
    while(is_shtf >= 0) {
        printf("%s \n", array_list_get(alist2, iter, &is_shtf)->name);
        iter = array_list_next(alist2, iter, &is_shtf);
    }
    printf("kek\n");

    delete_array_list(alist2);

    return 0;
}
