/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <beluckydaf@gmail.com> wrote this file.  As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return. Vladislav Smirnov
 * ----------------------------------------------------------------------------
 */

#include "alist.h"

p_array_list create_array_list(size_t size) {
    network_node* array = (network_node *)malloc(sizeof(network_node) * size);
    p_array_list alist = (p_array_list)malloc(sizeof(array_list));
    memset(array, 0, sizeof(network_node) * size);
    alist->size = size;
    alist->count = 0;
    alist->array = array;
    return alist;
}

void delete_array_list(p_array_list alist) {
    free(alist->array);
    free(alist);
}

size_t expand_array_list(p_array_list alist) {
    size_t size = alist->size;
    size_t new_size = size * 2;
    network_node* array = (network_node *)malloc(sizeof(network_node) * new_size);
    memset(array, 0, sizeof(network_node) * new_size);
    memcpy(array, alist->array, sizeof(network_node) * size);
    free(alist->array);
    alist->array = array;
    alist->size = new_size;
    return new_size;
}

size_t array_list_add(p_array_list alist, network_node* item) {
    for (size_t i = 0; i < alist->size; i++) {
        if (memcmp(&alist->array[i], &(network_node){0}, sizeof(network_node)) == 0) {
            memcpy(&alist->array[i], item, sizeof(network_node));
            alist->count++;
            return i;
        }
    }
    size_t index = (expand_array_list(alist) / 2) + 1;
    memcpy(&alist->array[index], item, sizeof(network_node));;
    alist->count++;
    return index;
}

size_t array_list_remove(p_array_list alist, network_node* item, int * is_error) {
    for (size_t i = 0; i < alist->size; i++) {
        if (memcmp(&alist->array[i], item, sizeof(network_node)) != 0) {
            memcpy(&alist->array[i], &(network_node){0}, sizeof(network_node));
            alist->count--;
            *is_error = 0;
            return i;
        }
    }
    *is_error = -1;
    return 0;
}

size_t array_list_remove_at(p_array_list alist, size_t index, int * is_error) {
    if (memcmp(&alist->array[index], &(network_node){0}, sizeof(network_node)) == 0) {
        *is_error = -1;
        return 0;
    }
    memcpy(&alist->array[index], &(network_node){0}, sizeof(network_node));
    alist->count--;
    return index;
}

size_t array_list_iter(p_array_list alist, int * is_error) {
    if (alist->count == 0) {
        *is_error = -1;
        return 0;
    }
    for (size_t i = 0; i < alist->size; i++) {
        if (memcmp(&alist->array[i], &(network_node){0}, sizeof(network_node)) != 0) return i;
    }
    *is_error = -1;
    return 0;
}

size_t array_list_next(p_array_list alist, size_t index, int * is_error) {
    for (size_t i = index + 1; i < alist->size; i++) {
        if (memcmp(&alist->array[i], &(network_node){0}, sizeof(network_node)) != 0) return (size_t) i;
    }
    *is_error = -1;
    return 0;
}

network_node* array_list_get(p_array_list alist, size_t index, int * is_error) {
    if (index > alist->size) {
        *is_error = -1;
        return NULL;
    }
    return &alist->array[index];
}


void* array_list_serialize(p_array_list alist, size_t * serialized_len) {
    *serialized_len = 2 * sizeof(size_t) + alist->count * sizeof(network_node);
    void* buffer = malloc(*serialized_len);
    memcpy(buffer, &alist->size, sizeof(size_t));
    memcpy(buffer + sizeof(size_t), &alist->count, sizeof(size_t));
    memcpy(buffer + 2* sizeof(size_t), alist->array, alist->count * sizeof(network_node));
    return buffer;
}

array_list * array_list_deserialise(void * serialized) {
    p_array_list buffer = create_array_list(1);
    memcpy(buffer, serialized, 2* sizeof(size_t));
    network_node * array = (network_node *)malloc(sizeof(network_node) * buffer->count);
    memcpy(array, serialized + 2 * sizeof(size_t) , buffer->count * sizeof(network_node));
    buffer->array = array;
    return buffer;
}

