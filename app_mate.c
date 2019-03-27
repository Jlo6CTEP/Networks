/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <JlobCTEP> wrote this file.  As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return. Yuriy Sukhorukov
 * ----------------------------------------------------------------------------
 */

#include "array_list/alist.h"
#include <pthread.h>
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
#include <ifaddrs.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <dirent.h>
#include <fcntl.h>

#define UDP_SERVER_PORT 8080
#define CMD_LEN 4
#define TRY_COUNT 5
#define DEFAULT_PORT 2000

#define NAME "-n"
#define ADDRESS "-a"
#define PORT "-p"

#define GET_CMD "-s"
#define CONNECT_CMD "-c"
#define CREATE_NEW_CMD "-mk"
#define CREATE_NEW_ID 0
#define CONNECT_ID 1
#define GET_ID 2

#define SHARED_FOLDER "../shared_folder/"
#define MAX_PORT_NUMBER 65535

#define SHTF(param) {printf("Incorrect parameters: %s\n", param);\
                     printf("Correct usage: %s \n %s\n", SYNTAX_CONNECT, SYNTAX_CREATE_NEW);\
                     return NULL;};

#define SYNTAX_CREATE_NEW "-mk -n[name] in case of creating new network"
#define SYNTAX_CONNECT "-c -a[address] -p[port] -n[name] (-s[filename]) in case of connecting to existing network"

#define GET_FILE "gtfl"
#define DISCONNECT "bail"
#define ADD_NEW "addn"
#define GET_LIST "glst"
#define PING "ping"
#define PONG "pong"

pthread_mutex_t lock;
pthread_mutex_t init;
pthread_mutex_t lock_file_list;

p_array_list node_list;
network_node self;
/*
int get_command(char * parameter, void ** line, int line_size, size_t * index) {
    for(size_t i = 0; i < line_size; i++ ){
        if (strcmp(line[i], parameter) == 0 && i < line_size) {
            *index = i;
            return 0;
        }
    }
    return -1;
}

void parse_stuff(const char * src, int len, char ** n_dest, char ** f_dest, size_t * f_count) {
    char buffer[256];
    n_dest = (char **)malloc(3 * sizeof(void *));
    for (int i = 0; i < 3; i ++) {
        n_dest[i] = malloc(256);
        memset(n_dest[i], 0, 256);
    }
    memset(buffer, 0, 256);
    size_t semicolon_count = 0;
    size_t file_count = 0;
    size_t index = 0;
    size_t buffer_count = 0;
    while (semicolon_count < 3 & index < len) {
        if (src[index] == *";") {
            semicolon_count++;
            buffer_count = 0;
            strcpy(n_dest[semicolon_count], buffer);
            memset(buffer, 0, 256);
        } else {
            buffer[buffer_count] = src[index];
            buffer_count++;
        }
        if (src[index] == *"\0")
            break;
        index++;
    }
    size_t next_index = index;
    while (index < len) {
        if(src[index] == *",")
            file_count++;
        if (src[index] == *"\0")
            break;
        index++;
    }
    file_count++;
    *f_count = file_count;

    f_dest = (char **)malloc(file_count * sizeof(void *));
    for (int i = 0; i < file_count; i++) {
        f_dest[i] = malloc(256);
        memset(f_dest[i], 0, 256);
    }

    index  = next_index;
    file_count = 0;
    buffer_count = 0;
    while (index < len) {
        if(src[index] == *",") {
            strcpy(f_dest[file_count],buffer);
            file_count++;
        } else {
            buffer[buffer_count] = src[index];
            buffer_count++;
        }
        if (src[index] == *"\0")
            break;
        index++;
    }
}


void *tcp_server(void * nothing) {
    pthread_mutex_lock(&init);
    pthread_mutex_unlock(&init);
    int main_socket = 0;

    int comm_socket = 0;
    fd_set read_descriptors;
    struct sockaddr_in my_address,client_addr;

    my_address = self.node_address;
    socklen_t addr_len = sizeof(struct sockaddr);

    main_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    bind(main_socket, (struct sockaddr *) &my_address, sizeof(struct sockaddr));

    struct sockaddr_in sin;
    socklen_t len = sizeof(sin);
    getsockname(main_socket, (struct sockaddr *)&sin, &len);
    listen(main_socket, 5);
    printf("Client is ready\n");
    while (1) {
        FD_ZERO(&read_descriptors);
        FD_SET(main_socket, &read_descriptors);
        select(main_socket + 1, &read_descriptors, NULL, NULL, NULL);

        if (FD_ISSET(main_socket, &read_descriptors)) {

            comm_socket = accept(main_socket, (struct sockaddr *) &client_addr, &addr_len);

            printf("Connection accepted from client : %s:%u\n",
                   inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

            char command[CMD_LEN];
            recvfrom(comm_socket, command, CMD_LEN, 0, (struct sockaddr *) &client_addr, &addr_len);
            if (memcmp(command, (char *)ADD_NEW, CMD_LEN) == 0) {
                network_node node;
                memset(&node, 0 , sizeof(network_node));

                size_t node_len = 0;
                recvfrom(comm_socket, &node_len, sizeof(size_t), 0,
                        (struct sockaddr *) &client_addr, &addr_len);
                void * serialized_node = malloc(node_len);
                recvfrom(comm_socket, serialized_node, node_len, 0,
                         (struct sockaddr *) &client_addr, &addr_len);

                node = *network_node_deserialize(serialized_node, &node_len);

                free(serialized_node);

                printf("New node %s:%u was added to list\n",
                        inet_ntoa(node.node_address.sin_addr),
                        ntohs(node.node_address.sin_port));
                pthread_mutex_lock(&lock);
                array_list_add(node_list, &node);
                pthread_mutex_unlock(&lock);
                memset(command, 0, CMD_LEN);
            } else if (memcmp(command, (char *)GET_LIST, CMD_LEN) == 0) {
                size_t serialized_len;

                array_list_add(node_list, &self);
                void *serialized = array_list_serialize(node_list, &serialized_len);
                int is_shtf = 0;
                array_list_remove(node_list, &self, &is_shtf);

                array_list * pl = array_list_deserialise(serialized);

                printf("List provided for %s:%u\n",
                       inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
                sendto(comm_socket, &serialized_len, sizeof(size_t), 0, (const struct sockaddr *) &client_addr,
                       addr_len);
                sendto(comm_socket, serialized, serialized_len, 0, (const struct sockaddr *) &client_addr,
                       addr_len);
                memset(command, 0, CMD_LEN);
            } else if ((memcmp(command, (char *)DISCONNECT, CMD_LEN) == 0)) {
                close(comm_socket);
                printf("Client %s:%u is bailing now\n",
                       inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
                memset(command, 0, CMD_LEN);
                break;
            } else if (memcmp(command, (char *)GET_FILE, CMD_LEN) == 0) {

                char filename[FILENAME_LENGTH];
                recvfrom(comm_socket, filename, FILENAME_LENGTH, 0,
                         (struct sockaddr *) &client_addr, &addr_len);

                char filepath[FILENAME_LENGTH * 2];
                strcpy(filepath, SHARED_FOLDER);
                strcat(filepath, filename);

                FILE *fp;
                char ch;
                long size = 0;
                fp = fopen(filepath, "r");
                fseek(fp, 0, 2);
                size = ftell(fp);
                fclose(fp);

                sendto(comm_socket, &size, sizeof(long), 0, (const struct sockaddr *) &client_addr,
                       addr_len);

                int file = open(filepath, O_RDONLY);
                void * file_content= malloc((size_t) size);
                read(file, file_content, (size_t) size);

                sendto(comm_socket, file_content, size, 0, (const struct sockaddr *) &client_addr,
                       addr_len);
                free(file_content);
                memset(command, 0, CMD_LEN);
            }
        }
    }
}


void tcp_client_connect(struct sockaddr_in dest) {
    socklen_t addr_len = sizeof(struct sockaddr);
    int main_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    connect(main_socket, (struct sockaddr *) &dest, sizeof(struct sockaddr));

    sendto(main_socket, (char *) GET_LIST, CMD_LEN, 0,
           (struct sockaddr *) &dest, sizeof(struct sockaddr));

    size_t len = 0;
    recvfrom(main_socket, &len, sizeof(size_t), 0, (struct sockaddr *) &dest, &addr_len);

    printf("Accepting list with length %zu\n", len + sizeof(network_node));

    void * node_buffer = malloc(len + sizeof(network_node));

    recvfrom(main_socket, node_buffer, len, 0,
             (struct sockaddr *) &dest, &addr_len);

    pthread_mutex_lock(&lock);
    node_list = array_list_deserialise(node_buffer);

    free(node_buffer);

    sendto(main_socket, (char *) DISCONNECT, CMD_LEN, 0,
           (struct sockaddr *) &dest, sizeof(struct sockaddr));
    close(main_socket);

    printf("Got list and now ready to tell everybody that im arrived\n");

    int is_shtf  =0;

    size_t iter = array_list_iter(node_list, &is_shtf);
    while(is_shtf >= 0) {
        printf("%s ", array_list_get(node_list, iter, &is_shtf)->name);
        iter = array_list_next(node_list, iter, &is_shtf);
    }
    printf("\n");

    is_shtf = 0;
    iter = array_list_iter(node_list, &is_shtf);

    while (is_shtf>= 0) {
        network_node *node = array_list_get(node_list, iter, &is_shtf);
        iter = array_list_next(node_list, iter, &is_shtf);
        main_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

        connect(main_socket, (struct sockaddr *) &node->node_address, sizeof(struct sockaddr));

        sendto(main_socket, (char *) ADD_NEW, CMD_LEN, 0,
               (struct sockaddr *) &dest, sizeof(struct sockaddr));

        size_t self_len = 0;
        void *serialized_self = network_node_serialize(&self, &self_len);

        sendto(main_socket, &self_len, sizeof(size_t), 0,
               (struct sockaddr *) &dest, sizeof(struct sockaddr));

        sendto(main_socket, serialized_self, self_len, 0,
               (struct sockaddr *) &dest, sizeof(struct sockaddr));

        sendto(main_socket, (char *) DISCONNECT, CMD_LEN, 0,
               (struct sockaddr *) &dest, sizeof(struct sockaddr));
        close(main_socket);
    }
    pthread_mutex_unlock(&lock);
}

void * tcp_client(void * data) {
    char filename[FILENAME_LENGTH];
    int cmd_len = 0;

    memcpy(&cmd_len, data, sizeof(int));
    void ** buffer = malloc(sizeof(void *) * cmd_len);
    memcpy(buffer, data + sizeof(int), sizeof(void *) * cmd_len);
    free(data);
    struct sockaddr_in dest = {0};

    size_t index1 = 0;
    size_t index2 = 0;
    int command_counter;
    if (get_command(NAME, buffer, cmd_len, &index1) != -1 && index1 + 1 <= cmd_len) {

        char ip_address[15];
        int fd;
        struct ifreq ifr;
        fd = socket(AF_INET, SOCK_DGRAM, 0);
        ifr.ifr_addr.sa_family = AF_INET;
        memcpy(ifr.ifr_name, "eth1", IFNAMSIZ-1);
        ioctl(fd, SIOCGIFADDR, &ifr);
        close(fd);
        strcpy((char *) ip_address, inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));

        strcpy(self.name, buffer[index1 + 1]);
        self.no_response_counter = 0;
        self.node_address.sin_family = AF_INET;
        inet_pton(AF_INET, ip_address, &self.node_address.sin_addr);
        self.node_address.sin_port  = htons(DEFAULT_PORT);

        pthread_mutex_unlock(&init);
        //handle make command
        if (get_command(CREATE_NEW_CMD, buffer, cmd_len, &index1) != -1) {
            command_counter = CREATE_NEW_ID;
        //handle connect command
        } else if (get_command(CONNECT_CMD, buffer, cmd_len, &index1) != -1) {
            char address[20];
            char port[20];
            char *ptr;
            if (get_command(ADDRESS, buffer, cmd_len, &index1) != -1 && index1 + 1 <= cmd_len){
                if (get_command(PORT, buffer, cmd_len, &index2) != -1 && index2 + 1 <= cmd_len){
                    if (inet_pton(AF_INET, buffer[index1 + 1], &dest.sin_addr) != -1) {
                        long port_number = strtol(buffer[index2 + 1], &ptr, 10);
                        if (port_number != 0 | port_number >= MAX_PORT_NUMBER) {
                            dest.sin_port = htons((uint16_t) port_number);
                            dest.sin_family = AF_INET;
                            command_counter = CONNECT_ID;
                            if (get_command(GET_CMD, buffer, cmd_len, &index1) != -1 && index1 + 1 <= cmd_len) {
                                strcpy(filename, buffer[index1 + 1]);
                                command_counter = GET_ID;
                            }
                        }else SHTF("port");
                    } else SHTF("address");
                } else SHTF("port");
            } else SHTF("address");
        } else if (1) {
            //some other cmds to handle

            //for this time just leave it alone
        }
    } else SHTF("name");

    free(buffer);



    switch (command_counter) {
        case CONNECT_ID: {
            tcp_client_connect(dest);
            break;
        }
        case GET_ID: {
            tcp_client_connect(dest);
            printf("execute order file transfer\n");
            int main_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

            connect(main_socket, (struct sockaddr *) &dest, sizeof(struct sockaddr));

            sendto(main_socket, (char *) GET_FILE, CMD_LEN, 0,
                   (struct sockaddr *) &dest, sizeof(struct sockaddr));

            sendto(main_socket, filename, FILENAME_LENGTH, 0,
                   (struct sockaddr *) &dest, sizeof(struct sockaddr));

            printf("file with name %s is transferring\n", filename);
            size_t len = 0;
            socklen_t addr_len = sizeof(struct sockaddr);
            recvfrom(main_socket, &len, sizeof(size_t), 0,
                     (struct sockaddr *) &dest, &addr_len);

            void* file_buffer = malloc(len);

            recvfrom(main_socket, file_buffer, len, 0,
                     (struct sockaddr *) &dest, &addr_len);

            char filepath[FILENAME_LENGTH * 2];
            strcpy(filepath, SHARED_FOLDER);
            strcat(filepath, filename);


            int fp = open(filepath, O_WRONLY | O_CREAT, S_IROTH | S_IWOTH);
            write(fp, file_buffer, len);
            close(fp);

            printf("done file transfer\n");

            sendto(main_socket, (char *) DISCONNECT, CMD_LEN, 0,
                   (struct sockaddr *) &dest, sizeof(struct sockaddr));
            close(main_socket);

            break;
        }
        case CREATE_NEW_ID:
            return NULL;

        default:
            break;

    }
    return NULL;
}*/

void * file_daemon(void *nothing) {
    pthread_mutex_lock(&init);
    pthread_mutex_unlock(&init);

    while (1) {
        DIR *d;
        struct dirent *dir;
        d = opendir(SHARED_FOLDER);
        pthread_mutex_lock(&lock_file_list);
        if (d) {
            while ((dir = readdir(d)) != NULL) {
                if (strcmp(dir->d_name, ".") != 0 && strcmp(dir->d_name, "..") != 0) {
                    array_list_add_file(&self, dir->d_name);
                    printf("%s added\n", dir->d_name);
                }
            }
            closedir(d);
        }
        pthread_mutex_unlock(&lock_file_list);
        sleep(10);
        printf("done");
    }
}


int main(int argc, char **argv) {
    pthread_mutex_init(&lock, NULL);
    pthread_mutex_init(&init, NULL);
    pthread_mutex_init(&lock_file_list, NULL);
    //pthread_mutex_lock(&init);
    node_list = create_array_list();
    self = *(network_node *)malloc(sizeof(network_node));

    pthread_t /*server,client, tcp_serv, tcp_cli,*/ daemon;
    //pthread_create(&tcp_cli, NULL, tcp_client, NULL);
    //pthread_create(&tcp_serv, NULL, tcp_server, NULL);
    pthread_create(&daemon, NULL, file_daemon, NULL);
    //pthread_join(server, NULL);
    //pthread_join(client, NULL);
    //pthread_join(tcp_serv, NULL);
    //pthread_join(tcp_cli, NULL);
    pthread_join(daemon, NULL);
}