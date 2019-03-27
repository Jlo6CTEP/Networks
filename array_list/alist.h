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

#define NODE_LENGTH 256
#define FILE_LIST_LENGTH 768
#define FILENAME_LENGTH 64
#define NODES_INIT_SIZE 4


typedef struct {
    char node[NODE_LENGTH];
    char files[FILE_LIST_LENGTH];
} network_node;

typedef struct {
    size_t size;
    size_t count;
    network_node* nodes;
} array_list;

typedef array_list* p_array_list;

p_array_list create_array_list();
void delete_array_list(p_array_list list);
size_t enlarge_array_list(p_array_list list);
size_t array_list_add(p_array_list list, network_node* item);
size_t array_list_iter(p_array_list list, int * is_error);
size_t array_list_next(p_array_list list, size_t index, int * is_error);
network_node* array_list_get(p_array_list list, size_t index, int * is_error);
void array_list_add_file(network_node* item, char * file);