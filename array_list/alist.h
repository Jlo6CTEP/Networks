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

#define NODENAME_LENGTH 30
#define FILENAME_LENGTH 64
#define FILE_NUMBER 64
#define FILES_INIT_SIZE 4
#define NODES_INIT_SIZE 4


typedef struct {
    char name[NODENAME_LENGTH];
    int no_response_counter;
    struct sockaddr_in node_address;
    size_t file_list_size;
    size_t file_list_count;
    char ** file_list;
} network_node;

typedef struct {
    size_t size;
    size_t count;
    network_node* nodes;
} array_list;

typedef array_list* p_array_list;

p_array_list create_array_list();
network_node * create_network_node();
void delete_array_list(p_array_list list);
size_t enlarge_array_list(p_array_list list);
size_t array_list_add(p_array_list list, network_node* item);
size_t array_list_remove(p_array_list list, network_node* item, int * is_error);
size_t array_list_iter(p_array_list list, int * is_error);
size_t array_list_next(p_array_list list, size_t index, int * is_error);
network_node* array_list_get(p_array_list list, size_t index, int * is_error);
void* array_list_serialize(p_array_list list, size_t * serialized_len);
array_list * array_list_deserialise(void * serialized);
void array_list_add_file(network_node* item, char * file);
size_t enlarge_file_list(network_node *item);
void array_list_remove_file(network_node* item, char * file);
void array_list_clear_files(network_node *item);
int array_list_exists_file(network_node* item, char * file);
