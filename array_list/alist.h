/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <beluckydaf@gmail.com> wrote this file.  As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return. Vladislav Smirnov
 * ----------------------------------------------------------------------------
 */

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netinet/ip.h>
#include <sys/socket.h>

#define NAME_LENGTH 30

typedef struct {
    char name[NAME_LENGTH];
    int no_response_counter;
    struct sockaddr_in node_address;
} network_node;

typedef struct {
    size_t size;
    size_t count;
    network_node* array;
} array_list;

typedef array_list* p_array_list;

p_array_list create_array_list(size_t size);
void delete_array_list(p_array_list a_list);
size_t expand_array_list(p_array_list a_list);
size_t array_list_add(p_array_list a_list, network_node* item);
size_t array_list_remove(p_array_list alist, network_node* item, int * is_error);
size_t array_list_remove_at(p_array_list alist, size_t index, int * is_error);
size_t array_list_iter(p_array_list alist, int * is_error);
size_t array_list_next(p_array_list alist, size_t index, int * is_error);
network_node* array_list_get(p_array_list alist, size_t index, int * is_error);
void* array_list_serialize(p_array_list a_list, size_t * serialized_len);
array_list * array_list_deserialise(void * serialized);
