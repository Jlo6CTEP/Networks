/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <beluckydaf@gmail.com> wrote this file.  As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return. Vladislav Smirnov
 * ----------------------------------------------------------------------------
 */

#include "alist.h"
#define NODES_INIT_SIZE 4

p_array_list create_array_list() {
    network_node* array = (network_node *)malloc(sizeof(network_node) * NODES_INIT_SIZE);
    p_array_list list = (p_array_list)malloc(sizeof(array_list));
    memset(array, 0, sizeof(network_node) * NODES_INIT_SIZE);
    list->size = NODES_INIT_SIZE;
    list->count = 0;
    list->nodes = array;
    return list;
}

void delete_array_list(p_array_list list) {
    free(list->nodes);
    free(list);
}

size_t expand_array_list(p_array_list list) {
    size_t size = list->size;
    size_t new_size = size * 2;
    network_node* array = (network_node *)malloc(sizeof(network_node) * new_size);
    memset(array, 0, sizeof(network_node) * new_size);
    memcpy(array, list->nodes, sizeof(network_node) * size);
    free(list->nodes);
    list->nodes = array;
    list->size = new_size;
    return new_size;
}

size_t array_list_add(p_array_list list, network_node* item) {
    for (size_t i = 0; i < list->size; i++) {
        if (memcmp(&list->nodes[i], &(network_node){0}, sizeof(network_node)) == 0) {
            memcpy(&list->nodes[i], item, sizeof(network_node));
            list->count++;
            return i;
        }
    }
    size_t index = (expand_array_list(list) / 2) + 1;
    memcpy(&list->nodes[index], item, sizeof(network_node));;
    list->count++;
    return index;
}

void array_list_add_file(network_node* item, char * file) {
    for (int i = 0; i < FILE_NUMBER; i++) {
        if (strcmp(item->file_list[i], "") == 0)
            strcpy(item->file_list[i], file);
    }
}

void array_list_remove_file(network_node* item, char * file) {
    for (int i = 0; i < FILE_NUMBER; i++) {
        if (strcmp(item->file_list[i], file) == 0)
            strcpy(item->file_list[i], "");
    }
}

int array_list_exists_file(network_node* item, char * file) {
    for (int i = 0; i < FILE_NUMBER; i++) {
        if (strcmp(item->file_list[i], file) == 0)
            return 1;
    }
    return 0;
}

void array_list_clear(network_node* item) {
    for (int i = 0; i < FILE_NUMBER; i++)
        strcpy(item->file_list[i], "");
}

size_t array_list_remove(p_array_list list, network_node* item, int * is_error) {
    for (size_t i = 0; i < list->size; i++) {
        if (memcmp(&list->nodes[i], item, sizeof(network_node)) == 0) {
            memset(&list->nodes[i], 0, sizeof(network_node));
            list->count--;
            *is_error = 0;
            return i;
        }
    }
    *is_error = -1;
    return 0;
}

size_t array_list_iter(p_array_list list, int * is_error) {
    if (list->count == 0) {
        *is_error = -1;
        return 0;
    }
    for (size_t i = 0; i < list->count; i++) {
        if (memcmp(&list->nodes[i], &(network_node){0}, sizeof(network_node)) != 0) return i;
    }
    *is_error = -1;
    return 0;
}

size_t array_list_next(p_array_list list, size_t index, int * is_error) {
    for (size_t i = index + 1; i < list->count; i++) {
        if (memcmp(&list->nodes[i], &(network_node){0}, sizeof(network_node)) != 0) return (size_t) i;
    }
    *is_error = -1;
    return 0;
}

network_node* array_list_get(p_array_list list, size_t index, int * is_error) {
    if (index > list->size) {
        *is_error = -1;
        return NULL;
    }
    return &list->nodes[index];
}

void* array_list_serialize(p_array_list list, size_t * serialized_len) {
    *serialized_len = 2 * sizeof(size_t) + list->count * sizeof(network_node);
    void* buffer = malloc(*serialized_len);
    memcpy(buffer, &list->size, sizeof(size_t));
    memcpy(buffer + sizeof(size_t), &list->count, sizeof(size_t));
    memcpy(buffer + 2* sizeof(size_t), list->nodes, list->count * sizeof(network_node));
    return buffer;
}

array_list * array_list_deserialise(void * serialized) {
    p_array_list buffer = create_array_list();
    memcpy(buffer, serialized, 2* sizeof(size_t));
    network_node * array = (network_node *)malloc(sizeof(network_node) * buffer->count);
    memcpy(array, serialized + 2 * sizeof(size_t) , buffer->count * sizeof(network_node));
    buffer->nodes = array;
    return buffer;
}

