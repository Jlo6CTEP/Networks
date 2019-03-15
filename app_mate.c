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

#define UDP_SERVER_PORT 8080
#define CMD_LEN 4
#define TRY_COUNT 5
#define DEFAULT_PORT 2000

#define NAME "-n"
#define ADDRESS "-a"
#define PORT "-p"

#define CONNECT "-c"
#define CREATE_NEW "-mk"
#define CONNECT_ID 1
#define CREATE_NEW_ID 0

#define MAX_PORT_NUMBER 65535

#define SHTF(param) {printf("Incorrect parameters: %s\n", param);\
                     printf("Correct usage: %s \n %s\n", SYNTAX_CONNECT, SYNTAX_CREATE_NEW);\
                     return NULL;};

#define SYNTAX_CREATE_NEW "-mk -n[name] in case of creating new network"
#define SYNTAX_CONNECT "-c -a[address] -p[port] -n[name] in case of connecting to existing network"

#define DISCONNECT "bail"
#define ADD_NEW "addn"
#define GET_LIST "glst"
#define PING "ping"
#define PONG "pong"

pthread_mutex_t lock;

p_array_list node_list;
network_node self;

int get_command(char * parameter, void ** line, int line_size, size_t * index) {
    for(size_t i = 0; i < line_size; i++ ){
        if (strcmp(line[i], parameter) == 0 && i < line_size) {
            *index = i;
            return 0;
        }
    }
    return -1;
}


void *tcp_server(void * nothing) {
    printf("server debug 1\n");

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

            while (1) {
                char command[CMD_LEN];
                recvfrom(comm_socket, command, CMD_LEN, 0, (struct sockaddr *) &client_addr, &addr_len);
                if (memcmp(command, (char *)ADD_NEW, CMD_LEN) == 0) {
                    network_node *node = {0};
                    recvfrom(comm_socket, (void *)node, sizeof(network_node), 0,
                            (struct sockaddr *) &client_addr, &addr_len);
                    printf("New node %s:%u was added to list\n",
                            inet_ntoa(node->node_address.sin_addr),
                            ntohs(node->node_address.sin_port));
                    pthread_mutex_lock(&lock);
                    array_list_add(node_list, node);
                    pthread_mutex_unlock(&lock);
                } else if (memcmp(command, (char *)GET_LIST, CMD_LEN) == 0) {
                    size_t serialized_len;
                    void *serialized = array_list_serialize(node_list, &serialized_len);
                    printf("List provided for %s:%u\n",
                           inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
                    sendto(comm_socket, &serialized_len, sizeof(size_t), 0, (const struct sockaddr *) &client_addr,
                           addr_len);
                    sendto(comm_socket, serialized, serialized_len, 0, (const struct sockaddr *) &client_addr,
                           addr_len);
                    sendto(comm_socket, &self, sizeof(network_node), 0, (const struct sockaddr *) &client_addr,
                           addr_len);
                } else if ((memcmp(command, (char *)DISCONNECT, CMD_LEN) == 0)) {
                    close(comm_socket);
                    printf("Client %s:%u is bailing now\n",
                           inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
                    break;
                }
            }
        }
    }
}

void * tcp_client(void * data) {
    self = (network_node){0};
    int cmd_len = 0;
    network_node *nn;
    memcpy(&cmd_len, data, sizeof(int));
    void ** buffer = malloc(sizeof(void *) * cmd_len);
    memcpy(buffer, data + sizeof(int), sizeof(void *) * cmd_len);

    struct sockaddr_in dest;

    size_t index1 = 0;
    size_t index2 = 0;
    int command_counter;
    if (get_command(NAME, buffer, cmd_len, &index1) != -1 && index1 + 1 <= cmd_len) {
        memcpy(&self.name, buffer[index1 + 1], NAME_LENGTH);
        //handle make command
        if (get_command(CREATE_NEW, buffer, cmd_len, &index1) != -1) {
            command_counter = CREATE_NEW_ID;
            self.no_response_counter = 0;
            self.node_address.sin_family = AF_INET;
            self.node_address.sin_port  = htons(DEFAULT_PORT);
        //handle connect command
        } else if (get_command(CONNECT, buffer, cmd_len, &index1) != -1) {
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
                        }else SHTF("port");
                    } else SHTF("address");
                } else SHTF("port");
            } else SHTF("address");
        } else if (1) {
            //some other cmds to handle

            //for this time just leave it alone
        }
    } else SHTF("name");


    switch (command_counter) {
        case CONNECT_ID: {
            socklen_t addr_len = sizeof(struct sockaddr);
            int main_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

            connect(main_socket, (struct sockaddr *) &dest, sizeof(struct sockaddr));

            sendto(main_socket, (char *) GET_LIST, CMD_LEN, 0,
                    (struct sockaddr *) &dest, sizeof(struct sockaddr));

            size_t len = 0;
            recvfrom(main_socket, &len, sizeof(size_t), 0, (struct sockaddr *) &dest, &addr_len);

            void * node_buffer = malloc(len + sizeof(network_node));

            recvfrom(main_socket, node_buffer, len + sizeof(network_node), 0,
                    (struct sockaddr *) &dest, &addr_len);

            pthread_mutex_lock(&lock);
            node_list = array_list_deserialise(node_buffer);
            pthread_mutex_unlock(&lock);

            pthread_mutex_lock(&lock);

            sendto(main_socket, (char *) DISCONNECT, CMD_LEN, 0,
                   (struct sockaddr *) &dest, sizeof(struct sockaddr));
            close(main_socket);

            printf("Client got list and now ready to tell everybody that he arrived\n");

            int is_shtf;
            size_t iter = array_list_iter(node_list, &is_shtf);

            while (is_shtf>= 0) {
                network_node *node = array_list_get(node_list, iter, &is_shtf);
                iter = array_list_next(node_list, iter, &is_shtf);
                main_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

                connect(main_socket, (struct sockaddr *) &node->node_address, sizeof(struct sockaddr));

                sendto(main_socket, (char *) ADD_NEW, CMD_LEN, 0,
                        (struct sockaddr *) &dest, sizeof(struct sockaddr));

                sendto(main_socket, node, sizeof(network_node), 0,
                       (struct sockaddr *) &dest, sizeof(struct sockaddr));

                sendto(main_socket, (char *) DISCONNECT, CMD_LEN, 0,
                       (struct sockaddr *) &dest, sizeof(struct sockaddr));
                close(main_socket);
            }
        }
        case CREATE_NEW_ID:
            return NULL;

        default:
            break;

    }
    return NULL;
}

void* udp_server(void *nothing) {
    char buffer[CMD_LEN]; 

    struct sockaddr_in servaddr, cliaddr; 
      
    int socket_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    memset(&servaddr, 0, sizeof(servaddr)); 
    memset(&cliaddr, 0, sizeof(cliaddr)); 
      
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(UDP_SERVER_PORT); 

    unsigned int addr_len = sizeof(cliaddr);
    bind(socket_fd, (struct sockaddr *)&servaddr, sizeof(servaddr));
      
    printf("server is ready\n");
    while(1){
        recvfrom(socket_fd, buffer, CMD_LEN, 0, ( struct sockaddr *) &cliaddr, &addr_len);
        printf("I was pinged by %s\n", inet_ntoa(cliaddr.sin_addr));
        if (strcmp(buffer, (char *)PING) == 0) {
            sendto(socket_fd, (const char *)PONG, CMD_LEN, 0, (const struct sockaddr *) &cliaddr, addr_len); 
        }
    }
}

void* udp_client(void *nothing) {
    int sockfd; 
    char buffer[CMD_LEN]; 
    struct sockaddr_in servaddr;

    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 100000;
  
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO,&tv,sizeof(tv));
    memset(&servaddr, 0, sizeof(servaddr)); 
      
    int len;
    int is_error = 0;
    printf("client is ready\n");
    while(1){ 
        size_t iter = array_list_iter(node_list, &is_error);
        while(is_error >= 0) {
            pthread_mutex_lock(&lock);
            network_node *nn = array_list_get(node_list, iter, &is_error);
            iter = array_list_next(node_list, iter, &is_error);
            pthread_mutex_unlock(&lock);
            printf("pingin node with name %s\n", nn->name);
            sendto(sockfd, (const char *)PING, CMD_LEN, 0, (const struct sockaddr *) &nn->node_address,
                    sizeof(nn->node_address));
            ssize_t n = recvfrom(sockfd, (char *)buffer, CMD_LEN, 0, (struct sockaddr *) &nn->node_address,
                                 (socklen_t *) &len);
            if (n >= 0 && strcmp((char *)PONG, (char *)buffer) == 0) {
                printf("Good\n");
                nn->no_response_counter = 0;
            } else {
                nn->no_response_counter++;
            }
            if (nn->no_response_counter >= TRY_COUNT) {
                array_list_remove(node_list, nn, &is_error);
                printf("No response\n");
            }
        }
        sleep(5);
    }
}


int main(int argc, char **argv) {
    pthread_mutex_init(&lock, NULL);
    node_list = create_array_list(2);

    void * data = malloc(sizeof(int) + sizeof(void *) * argc);
    memcpy(data, &argc, sizeof(int));
    memcpy(data + sizeof(int), argv, sizeof(void *) * argc);

    pthread_t server,client, tcp_serv, tcp_cli;
    pthread_create(&server, NULL, udp_server, NULL);
    pthread_create(&client, NULL, udp_client, NULL);
    pthread_create(&tcp_cli, NULL, tcp_client, data);
    pthread_create(&tcp_serv, NULL, tcp_server, NULL);
    pthread_join(server, NULL);
    pthread_join(client, NULL);
    pthread_join(tcp_serv, NULL);
    pthread_join(tcp_cli, NULL);
}