/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <beluckydaf@gmail.com> wrote this file.  As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return. Vladislav Smirnov
 * ----------------------------------------------------------------------------
 */

#include <stdio.h>
#include "alist.h"

p_array_list create_array_list() {
    network_node* array = (network_node *)malloc(sizeof(network_node) * NODES_INIT_SIZE);
    p_array_list list = (p_array_list)malloc(sizeof(array_list));
    memset(array, 0, sizeof(network_node) * NODES_INIT_SIZE);
    list->size = NODES_INIT_SIZE;
    list->count = 0;
    list->nodes = array;
    return list;
}

char ** parse_files(network_node * nn, size_t * count, size_t * last_pos) {
    size_t i = 0;
    size_t file_count = 0;
    while (i < FILE_LIST_LENGTH && nn->files[i] != '\0') {
        if (nn->files[i] == ',')
            file_count++;
        i++;
    }
    char ** array = (char **)malloc(file_count * sizeof(void *));
    for (int x = 0; x < file_count; x++) {
        array[x] = malloc(FILENAME_LENGTH);
        memset(array[x], 0, FILENAME_LENGTH);
    }

    char buffer[FILENAME_LENGTH];
    memset(buffer, 0, FILENAME_LENGTH);
    size_t buffer_count = 0;
    size_t array_count = 0;
    i = 0;
    while (i < FILE_LIST_LENGTH && nn->files[i] != '\0') {
        if (nn->files[i] == ',') {
            strcpy(array[array_count], buffer);
            memset(buffer, 0, FILENAME_LENGTH);
        } else {
            buffer[buffer_count] = nn->files[i];
            buffer_count++;
        }
        i++;
    }
    *count = file_count;
    *last_pos = i;
    return array;
}

char ** parse_nodes(network_node * nn, size_t * last_pos) {

}



void delete_array_list(p_array_list list) {
    free(list->nodes);
    free(list);
}


size_t enlarge_array_list(p_array_list list) {
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
        if (strcmp(list->nodes[i].node, item->node) == 0 && strcmp(item->files, "") != 0) {
            memcpy(&list->nodes[i], item, sizeof(network_node));
            return i;
        }
    }
    size_t index = (enlarge_array_list(list) / 2);
    memcpy(&list->nodes[index], item, sizeof(network_node));;
    list->count++;
    return index;
}

void array_list_add_file(network_node* item, char * file) {
    size_t file_count = 0;
    size_t last_pos = 0;
    char ** existing_files = parse_files(item, &file_count, &last_pos);
    for (size_t i = 0; i < file_count; i ++) {
        if (strcmp(existing_files[i], file) == 0)
            return;
    }
    size_t offset = 0;
    if (last_pos != 0) {
        item->files[last_pos] = ',';
        offset = 1;
    }
    strcpy(&item->files[last_pos + offset], file);
}

void array_list_clear_files(network_node *item) {
    memset(item->files, 0, FILE_LIST_LENGTH);
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
    for (size_t i = index + 1; i < list->size; i++) {
        if (memcmp(&list->nodes[i], &(network_node){0}, sizeof(network_node) - 3*sizeof(void *)) != 0) return (size_t) i;
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
