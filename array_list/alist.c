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
    for (int i = 0; i < NODES_INIT_SIZE; i++) {
        array[i].file_list = (char **) malloc(FILES_INIT_SIZE * sizeof(void *));
        for (int j = 0; j < FILES_INIT_SIZE; j ++) {
            array[i].file_list[j] = (char *) malloc(FILENAME_LENGTH);
            memset(array[i].file_list[j], 0, FILENAME_LENGTH);
        }
        array[i].file_list_size = FILES_INIT_SIZE;
    }
    list->size = NODES_INIT_SIZE;
    list->count = 0;
    list->nodes = array;
    return list;
}

network_node * create_network_node() {
    network_node *nn = (network_node *) malloc(sizeof(network_node));
    memset(nn, 0, sizeof(network_node));
    nn->file_list = (char **) malloc(FILES_INIT_SIZE * sizeof(void *));
    for (int j = 0; j < FILES_INIT_SIZE; j++) {
        nn->file_list[j] = (char *) malloc(FILENAME_LENGTH);
        memset(nn->file_list[j], 0, FILENAME_LENGTH);
    }
    nn->file_list_size = FILES_INIT_SIZE;
    nn->file_list_count = 0;
    return nn;
}

void delete_array_list(p_array_list list) {
    for (int i = 0; i < list->count; i++) {
        for (int j = 0; j < list->nodes[i].file_list_count; j++)
            free(list->nodes[i].file_list[j]);
        free(list->nodes[i].file_list);
    }
    free(list->nodes);
    free(list);
}

void delete_network_node(network_node * nn) {
    for (int j = 0; j < nn->file_list_count; j++)
        free(nn->file_list[j]);
    free(nn->file_list);
    free(nn);
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
        if (memcmp(&list->nodes[i], &(network_node){0}, sizeof(network_node) - 3*sizeof(void *)) == 0) {
            memcpy(&list->nodes[i], item, sizeof(network_node));
            list->count++;
            return i;
        }
    }
    size_t index = (enlarge_array_list(list) / 2);
    memcpy(&list->nodes[index], item, sizeof(network_node));;
    list->count++;
    return index;
}

void array_list_add_file(network_node* item, char * file) {
    for (int i = 0; i < item->file_list_size; i++) {
        if (strcmp(item->file_list[i], "") == 0) {
            strcpy(item->file_list[i], file);
            item->file_list_count++;
            return;
        }
    }
    size_t index = enlarge_file_list(item)/2;
    strcpy(item->file_list[index], file);
    item->file_list_count++;
}

size_t enlarge_file_list(network_node *item) {
    size_t new_size = item->file_list_size * 2;

    char **array = (char **) malloc(new_size * sizeof(void *));
    for (int i = 0; i < new_size; i++) {
        array[i] = malloc(FILENAME_LENGTH);
        memset(array[i], 0, FILENAME_LENGTH);
    }

    for (int i = 0; i < item->file_list_count; i++)
        strcpy(array[i], item->file_list[i]);

    for (int i = 0; i < item->file_list_count; i++)
        free(item->file_list[i]);
    free(item->file_list);
    item->file_list = array;
    item->file_list_size = new_size;
    return new_size;
}

void array_list_remove_file(network_node* item, char * file) {
    for (int i = 0; i < item->file_list_size; i++) {
        if (strcmp(item->file_list[i], file) == 0) {
            memset(item->file_list[i], 0, FILENAME_LENGTH);
            item->file_list_count--;
        }
    }
}

int array_list_exists_file(network_node* item, char * file) {
    for (int i = 0; i < item->file_list_size; i++) {
        if (strcmp(item->file_list[i], file) == 0)
            return 1;
    }
    return 0;
}

void array_list_clear_files(network_node *item) {
    for (int i = 0; i < item->file_list_size; i++)
        memset(item->file_list[i], 0, FILENAME_LENGTH);
    item->file_list_count = 0;
}

size_t array_list_remove(p_array_list list, network_node* item, int * is_error) {
    for (size_t i = 0; i < list->size; i++) {
        if (memcmp(&list->nodes[i], item, sizeof(network_node) - sizeof(void *)) == 0) {
            for (int j = 0; j < list->nodes[i].file_list_size; j++)
                free(list->nodes[i].file_list[j]);
            free(list->nodes[i].file_list);
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
void* network_node_serialize(network_node * nn, size_t * serialized_len) {
    *serialized_len = sizeof(network_node) - sizeof(void *) + nn->file_list_count*FILENAME_LENGTH;

    void* buffer = malloc(*serialized_len);
    size_t  offset = 0;
    memset(buffer, 0, *serialized_len);
    memcpy(buffer, nn, sizeof(network_node) - sizeof(void *));
    offset += sizeof(network_node) - sizeof(void *);
    for (int j = 0; j < nn->file_list_count; j++) {
        memcpy(buffer + offset, nn->file_list[j], FILENAME_LENGTH);
        offset += FILENAME_LENGTH;
    }
    return buffer;
}

network_node * network_node_deserialize(void * buff, size_t * shift) {
    network_node *nn = (network_node *)malloc(sizeof(network_node));
    *shift = sizeof(network_node) - sizeof(void *);
    memcpy(nn, buff, *shift);
    char **file_list = (char **) malloc(nn->file_list_size * sizeof(void *));
    for (int i = 0; i < nn->file_list_count; i++) {
        file_list[i] = malloc(FILENAME_LENGTH);
        memset(file_list[i], 0, FILENAME_LENGTH);
        memcpy(file_list[i], buff + *shift, FILENAME_LENGTH);
        *shift += FILENAME_LENGTH;
        nn->file_list = file_list;
    }
    return nn;
}

void* array_list_serialize(p_array_list list, size_t * serialized_len) {
    *serialized_len = 2 * sizeof(size_t) + list->count * (sizeof(network_node) - sizeof(void *));
    for (int i = 0; i < list->count; i ++)
        *serialized_len += list->nodes[i].file_list_count * FILENAME_LENGTH;

    void* buffer = malloc(*serialized_len);
    memset(buffer, 0, *serialized_len);
    size_t offset = 0;
    memcpy(buffer, list, sizeof(size_t) * 2);
    offset += sizeof(size_t) * 2;
    for (int i = 0; i < list->count; i ++) {
        size_t length = 0;
        void * buff = network_node_serialize(&list->nodes[i], &length);
        memcpy(buffer + offset, buff, length);
        offset += length;
        free(buff);
    }
    return buffer;
}

array_list * array_list_deserialise(void * serialized) {
    p_array_list buffer = create_array_list();
    memcpy(buffer, serialized, 2* sizeof(size_t));
    size_t offset = 2* sizeof(size_t);
    network_node * array = (network_node *)malloc(sizeof(network_node) * buffer->size);
    memset(array, 0, sizeof(network_node) * buffer->size);
    for (int i = 0; i < buffer->count; i ++) {
        size_t shift = 0;
        array[i] = *network_node_deserialize(serialized + offset, &shift);
        offset += shift;
    }
    buffer->nodes = array;
    return buffer;
}

