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

#define DEFAULT_PORT "8080"

#define NAME "-n"
#define ADDRESS "-a"
#define PORT "-p"
#define NODE_NAME "Tango\0"

#define REQUEST_CMD "-r"
#define SYN_CMD "-s"
#define CREATE_NEW_CMD "-mk"
#define FILENAME "-f"
#define CREATE_NEW 2
#define SYN 1
#define REQUEST 0
#define MAX_ATTEMPTS 3

#define SHARED_FOLDER "../shared_folder/"

#define SHTF(param) {printf("Incorrect parameters: %s\n", param);\
                     printf("Correct usage: %s\n %s\n %s\n", SYNTAX_CREATE_NEW, SYNTAX_SYN, SYNTAX_REQUEST);\
                     return NULL;};

#define SYNTAX_CREATE_NEW "-mk -n[name] in case of creating new network"
#define SYNTAX_SYN "-s -a[address] -p[port] -n[name] in case of syncing with existing network"
#define SYNTAX_REQUEST "-r -a[address] -p[port] -n[name] -f[filename] in case of syncing with existing network"

pthread_mutex_t lock_node_list, client, lock_file_list, server , lock_black_list, lock_current;

p_array_list node_list;
p_array_list black_list;
p_array_list current;
network_node * self;

void *connection_handler(void * data) {
    // unpack all needed data
    struct sockaddr_in client_addr;
    memcpy(&client_addr, data, sizeof(struct sockaddr_in));

    socklen_t addr_len;
    memcpy(&addr_len, data + sizeof(struct sockaddr_in), sizeof(socklen_t));

    int comm_socket;
    memcpy(&comm_socket, data + sizeof(struct sockaddr_in) + sizeof(socklen_t), sizeof(int));

    size_t hashed;
    memcpy(&hashed, data + sizeof(struct sockaddr_in) + sizeof(socklen_t) + sizeof(int), sizeof(size_t));

    printf("Connection accepted from client : %s:%u\n",
           inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

    unsigned int command = 100500;
    // now it is time to receive command
    recvfrom(comm_socket, &command, sizeof(int), 0, (struct sockaddr *) &client_addr, &addr_len);
    command = ntohl(command);
    printf("command %d\n", command);
    if (command == SYN) {
        //first receive info about requesting node
        char * buffer = malloc(MSG_LEN);
        memset(buffer, 0, MSG_LEN);
        recvfrom(comm_socket, buffer, MSG_LEN, 0,
                 (struct sockaddr *) &client_addr, &addr_len);

        network_node * nn = (network_node *)malloc(sizeof(network_node));
        memset(nn, 0, sizeof(network_node));
        split_msg(nn, buffer);
        free(buffer);

        //and add it into my node list
        pthread_mutex_lock(&lock_node_list);
        array_list_add(node_list, nn);
        pthread_mutex_unlock(&lock_node_list);
        int length = 0;
        //receive number of known nodes IN NETWORK FORMAT
        recvfrom(comm_socket, &length, sizeof(int), 0,
                 (struct sockaddr *) &client_addr, &addr_len);
        length = ntohl((uint32_t) length);
        printf("%d\n", length);
        //receive nodes one by one
        for (int i = 0; i < length; i++) {
            network_node *nn2 = (network_node *) malloc(sizeof(network_node));
            memset(nn2, 0, sizeof(network_node));
            char *buffer2 = malloc(MSG_LEN);
            memset(buffer2, 0, MSG_LEN);
            recvfrom(comm_socket, buffer2, MSG_LEN, 0,
                     (struct sockaddr *) &client_addr, &addr_len);
            split_msg(nn2, buffer2);
            // sometimes other nodes send you info about yourself
            // if so, do nothing
            if (memcmp(nn2, self, NODE_LENGTH) != 0) {
                pthread_mutex_lock(&lock_node_list);
                array_list_add(node_list, nn2);
                pthread_mutex_unlock(&lock_node_list);
            }
            free(buffer2);
        }
        pthread_mutex_unlock(&lock_node_list);

    } else if (command == REQUEST) {
        // in case of request, we first receive filename
        char * filename = malloc(MSG_LEN);
        memset(filename, 0, MSG_LEN);
        printf("Receiving filename\n");
        recvfrom(comm_socket, filename, MSG_LEN, 0,
                 (struct sockaddr *) &client_addr, &addr_len);

        // construct filepath to get this file
        char filepath[MSG_LEN];
        memset(filepath, 0, MSG_LEN);
        strcpy(filepath, SHARED_FOLDER);
        strcat(filepath, filename);
        printf("filepath is %s\n", filepath);

        // this stores number of words
        int word_count = 0;
        // now file can be parsed into buffer word by word by utility function
        char ** parsed_file = parse_file(filepath, &word_count);
        printf("file was %d long\n", word_count);
        word_count = htonl((uint32_t) word_count);
        // send number of words
        sendto(comm_socket, &word_count, sizeof(int), 0,
               (struct sockaddr *) &client_addr, sizeof(struct sockaddr));

        //and then send file content word by word
        for (int i = 0; i < word_count; i ++) {
            printf("word %s\n", parsed_file[i]);
            sendto(comm_socket, parsed_file[i], MSG_LEN, 0,
                   (struct sockaddr *) &client_addr, sizeof(struct sockaddr));
            usleep(50000);
        }
        // prevent memory leaks
        printf("Done file transfer\n");
        for (int i = 0; i < word_count; i++)
            free(parsed_file[i]);
        free(parsed_file);
    }

    close(comm_socket);
    usleep(10000);
    pthread_mutex_lock(&lock_current);
    get_by_hash(current, hashed)->counter--;
    pthread_mutex_unlock(&lock_current);

    return NULL;
}


//done
void *tcp_server(void * nothing) {
    // this makes the server wait unblocking by client
    // again, race condition is evil
    pthread_mutex_lock(&server);
    pthread_mutex_unlock(&server);

    // this will setup server (like in previous examples on labs)
    int main_socket = 0;
    int comm_socket = 0;
    fd_set read_descriptors;
    struct sockaddr_in my_address,client_addr;

    my_address = *get_sockadrr(self);
    socklen_t addr_len = sizeof(struct sockaddr);

    main_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    bind(main_socket, (struct sockaddr *) &my_address, sizeof(struct sockaddr));

    struct sockaddr_in sin;
    socklen_t len = sizeof(sin);
    getsockname(main_socket, (struct sockaddr *)&sin, &len);
    listen(main_socket, 5);
    printf("Server is ready\n");
    while (1) {
        FD_ZERO(&read_descriptors);
        FD_SET(main_socket, &read_descriptors);
        select(main_socket + 1, &read_descriptors, NULL, NULL, NULL);
        // if there is connection
        if (FD_ISSET(main_socket, &read_descriptors)) {
            // we accept it, and check if it is spammer
            comm_socket = accept(main_socket, (struct sockaddr *) &client_addr, &addr_len);

            // this flag determine if we process this connection or not
            int flag = 1;
            //construct temporary network node for accepted connection
            network_node * nn = (network_node *)malloc(sizeof(network_node));
            sprintf(nn->node, ":%s:%d:", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
            // calculate its hash
            size_t hashed = hash_nn(nn);

            //if node in blacklist - do nothing
            if (contains_by_hash(black_list, hashed) != 0) {
                flag = 0;
                // if it is in current list, which means that there are requests from this
                // node being processed right now
            } else if (contains_by_hash(current, hashed)) {
                // first check, if this node reached maximum number of concurrent connections
                if (get_by_hash(current, hashed)->counter >= MAX_ATTEMPTS) {
                    // if so, black-list this node and print message
                    pthread_mutex_lock(&lock_black_list);
                    array_list_add(black_list, nn);
                    pthread_mutex_unlock(&lock_black_list);
                    printf("Node %s be black-listed\n", nn->node);
                    pthread_mutex_lock(&lock_current);
                    array_list_remove(current, nn);
                    pthread_mutex_unlock(&lock_current);
                    flag = 0;
                } else {
                    // if not, just increase connections counter
                    pthread_mutex_lock(&lock_current);
                    get_by_hash(current, hashed)->counter++;
                    pthread_mutex_unlock(&lock_current);
                }
            } else {
                // if node is not in current list
                // add it and increase counter
                pthread_mutex_lock(&lock_current);
                array_list_add(current, nn);
                get_by_hash(current, hashed)->counter++;
                pthread_mutex_unlock(&lock_current);
            }

            // now if node is not blacklisted, we can process connection from it
            if (flag) {
                // pack all needed data for handler thread
                size_t size = sizeof(struct sockaddr_in) + sizeof(socklen_t) + sizeof(int) + sizeof(size_t);
                void *data = malloc(size);
                memset(data, 0, size);
                memcpy(data, &client_addr, sizeof(struct sockaddr_in));
                memcpy(data + sizeof(struct sockaddr_in), &addr_len, sizeof(socklen_t));
                memcpy(data + sizeof(struct sockaddr_in) + sizeof(socklen_t), &comm_socket, sizeof(int));
                memcpy(data + sizeof(struct sockaddr_in) + sizeof(socklen_t) + sizeof(int), &hashed, sizeof(size_t));

                //invoke new thread to process this connection
                pthread_t request;
                pthread_create(&request, NULL, connection_handler, data);
            }
        }
    }
}

int cl_parse(char *parameter, void **line, int line_size, size_t *index) {
    for(size_t i = 0; i < line_size; i++ ){
        if (strcmp(line[i], parameter) == 0 && i < line_size) {
            *index = i;
            return 0;
        }
    }
    return -1;
}

void * tcp_client(void * data) {
    // this hold client locked until unlocked by file daemon
    pthread_mutex_lock(&client);
    pthread_mutex_unlock(&client);

    // unpack argv and argc (CMD line arguments) from main function
    int argc = 0;

    //this gonna be argc
    memcpy(&argc, data, sizeof(int));
    //and this argv
    void ** argv = malloc(sizeof(void *) * argc);
    memcpy(argv, data + sizeof(int), sizeof(void *) * argc);
    free(data);

    // target node created and initialised
    network_node * dest = (network_node *)malloc(sizeof(network_node));
    memset(dest, 0, sizeof(network_node));

    // indexes of different command line arguments
    size_t index0 = 0;
    size_t index1 = 0;
    size_t index2 = 0;

    //this will be filled with node info
    char node[NODE_LENGTH];
    // this with filename from CMD line arguments
    char file_name[MSG_LEN];


    int command_counter;
    // try to find name key and its argument in argv
    if (cl_parse(NAME, argv, argc, &index0) != -1 && index0 + 1 <= argc) {

        //this will obtain your IP on the corresponding communication media (eth0, eth1 etc)
        char ip_address[15];
        int fd;
        struct ifreq ifr;
        fd = socket(AF_INET, SOCK_DGRAM, 0);
        ifr.ifr_addr.sa_family = AF_INET;
        memcpy(ifr.ifr_name, "wifi0", IFNAMSIZ-1);

        ioctl(fd, SIOCGIFADDR, &ifr);
        close(fd);
        //this will construct info about your node and put it into self network_node
        strcpy((char *) ip_address, inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));
        memset(node, 0, NODE_LENGTH);
        strcat(node, NODE_NAME);
        strcat(node, ":\0");
        strcat(node, ip_address);
        strcat(node, ":\0");
        strcat(node, DEFAULT_PORT);
        strcat(node, ":\0");

        strcpy(self->node, node);

        //try to find MAKE command, which is used to create first ndoe
        if (cl_parse(CREATE_NEW_CMD, argv, argc, &index1) != -1) {
            command_counter = CREATE_NEW;
        //handle connect command
        } else {
            //otherwise we have to parse address and port of target node
            // they are needed for both request and syn commands
            // if there is such parameters, they will be parsed and put into destination node
            if (cl_parse(ADDRESS, argv, argc, &index1) != -1 && index1 + 1 <= argc){
                if (cl_parse(PORT, argv, argc, &index2) != -1 && index2 + 1 <= argc){
                    memset(node, 0, NODE_LENGTH);
                    strcat(node, argv[index0 + 1]);
                    strcat(node, ":\0");
                    strcat(node, argv[index1 + 1]);
                    strcat(node, ":\0");
                    strcat(node, argv[index2 + 1]);
                    strcat(node, ":\0");

                    strcpy(dest->node, node);
                    // if there is no such parameters, program will show error message and quit
                }else SHTF("port");
            } else SHTF("address");

            //if there is SYN parameter, set command_counter to appropriate command
            if (cl_parse(SYN_CMD, argv, argc, &index1) != -1 && index1 + 1 <= argc)
                command_counter = SYN;
            else
                // last option is request command
                //here must be appropriate key for command itself
                if (cl_parse(REQUEST_CMD, argv, argc, &index1) != -1 && index1 + 1 <= argc) {
                    // and key for file, followed by filename
                    if (cl_parse(FILENAME, argv, argc, &index1) != -1 && index1 + 1 <= argc) {
                        memset(file_name, 0, MSG_LEN);
                        strcpy(file_name, argv[index1 + 1]);
                        command_counter = REQUEST;
                    } else SHTF("filename")
                } else SHTF("file")
        }
    } else SHTF("name");
    //now time to unlock server
    pthread_mutex_unlock(&server);

    free(argv);


    // in previous stage we only set command_counter, filename and dest address
    // according to arguments from CMD
    switch (command_counter) {
        //in case of syn command, do all appropriate stuff from lab
        case SYN: {
            // warning, network format!
            int command = htonl(SYN);
            int main_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
            connect(main_socket, (struct sockaddr *) get_sockadrr(dest), sizeof(struct sockaddr));
            // first we send command to remote server
            sendto(main_socket, &command , sizeof(int), 0,
                   (struct sockaddr *) get_sockadrr(dest), sizeof(struct sockaddr));
            // then send info about this host
            sendto(main_socket, concat_msg(self), MSG_LEN, 0,
                   (struct sockaddr *) get_sockadrr(dest), sizeof(struct sockaddr));
            // Then send number of known nodes
            int known_nodes_count = htonl((uint32_t) node_list->count);
            sendto(main_socket, &known_nodes_count, sizeof(int), 0,
                   (struct sockaddr *) get_sockadrr(dest), sizeof(struct sockaddr));

            // and send them one by one
            int is_shtf2 = 0;
            size_t iter2 = array_list_iter(node_list, &is_shtf2);
            while (is_shtf2 >= 0) {
                char * buffer = malloc(MSG_LEN);
                memset(buffer, 0, MSG_LEN);
                strcpy(buffer, array_list_get(node_list, iter2, &is_shtf2)->node);
                sendto(main_socket, buffer, MSG_LEN, 0, (struct sockaddr *) &dest, sizeof(struct sockaddr));
                iter2 = array_list_next(node_list, iter2, &is_shtf2);
            }

            printf("done sync\n");
            break;
        }
        case REQUEST: {
            // if we want to perform request command
            printf("execute order file transfer\n");

            int main_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

            connect(main_socket, (struct sockaddr *) get_sockadrr(dest), sizeof(struct sockaddr));
            // first send command in network format
            int command = htonl(REQUEST);
            sendto(main_socket, &command, sizeof(int), 0,
                   (struct sockaddr *) get_sockadrr(dest), sizeof(struct sockaddr));
            // then send filename
            sendto(main_socket, file_name, MSG_LEN, 0,
                   (struct sockaddr *) get_sockadrr(dest), sizeof(struct sockaddr));
            // and then receive word count of file
            int32_t len = 0;
            socklen_t addr_len = sizeof(struct sockaddr);
            recvfrom(main_socket, &len, sizeof(int32_t), 0,
                     (struct sockaddr *) get_sockadrr(dest), &addr_len);

            len = ntohl((uint32_t) len);
            printf("word count is %d\n", len);
            // construct filepath
            char filepath[FILENAME_LENGTH * 2];
            strcpy(filepath, SHARED_FOLDER);
            strcat(filepath, file_name);
            FILE *fp;
            fp = fopen(filepath, "w");

            char file_content[MSG_LEN];
            memset(file_content, 0, MSG_LEN);
            //receive file word by word and write it into file
            for (int i = 0; i < len; i++) {
                char word[MSG_LEN];
                memset(word, 0, MSG_LEN);
                recvfrom(main_socket, word, MSG_LEN, 0,
                         (struct sockaddr *) get_sockadrr(dest), &addr_len);
                printf("word %s\n", word);
                fprintf(fp,"%s ", word);
            }
            // transfer is done
            fclose(fp);
            printf("done file transfer\n");
            close(main_socket);

            break;
        }
        // in case of creating new file we should do nothing
        case CREATE_NEW:
            return NULL;
        default:break;
    }
    return NULL;
}

void * syncher(void * nothing) {
    // lock node_list for all the time of operations
    // to prevent race conditions
    pthread_mutex_lock(&lock_node_list);
    while (1) {
        // if there is no nodes in my node, do nothing
        int is_shtf = 0;
        size_t iter = array_list_iter(node_list, &is_shtf);

        while (is_shtf >= 0) {
            // construct all appropriate data
            int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
            struct sockaddr_in *dest = get_sockadrr(array_list_get(node_list, iter, &is_shtf));
            connect(sock, (struct sockaddr *) dest, sizeof(struct sockaddr));
            int command = htonl(1);
            // send command to the host
            sendto(sock, &command, sizeof(int), 0, (struct sockaddr *) &dest, sizeof(struct sockaddr));
            // send info about this machine
            sendto(sock, concat_msg(self), MSG_LEN, 0, (struct sockaddr *) &dest, sizeof(struct sockaddr));
            int length = htonl((uint32_t) node_list->count);
            // send number of known nodes
            sendto(sock, &length, sizeof(int), 0, (struct sockaddr *) &dest, sizeof(struct sockaddr));
            // send nodes one by one, if there are any nodes
            if (node_list->count != 0) {
                // this is error counter for node_list, it will be set to -1
                // if there are no nodes/next node doesn't exist
                int is_shtf2 = 0;
                size_t iter2 = array_list_iter(node_list, &is_shtf);
                while (is_shtf2 >= 0) {
                    char * buffer = malloc(MSG_LEN);
                    memset(buffer, 0, MSG_LEN);
                    strcpy(buffer, array_list_get(node_list, iter2, &is_shtf2)->node);
                    sendto(sock, buffer, MSG_LEN, 0, (struct sockaddr *) &dest, sizeof(struct sockaddr));
                    iter2 = array_list_next(node_list, iter2, &is_shtf2);
                }
            }
            iter = array_list_next(node_list, iter, &is_shtf);
        }
        pthread_mutex_unlock(&lock_node_list);
        sleep(10);
    }
}


void * file_daemon(void *nothing) {
    // put files in ../shared_folder/ into self->files
    while (1) {
        // scans directory
        DIR *d;
        struct dirent *dir;
        d = opendir(SHARED_FOLDER);
        // locks file list
        pthread_mutex_lock(&lock_file_list);
        if (d) {
            // add file by file into self->file
            while ((dir = readdir(d)) != NULL) {
                // ignore .. and . files
                if (strcmp(dir->d_name, ".") != 0 && strcmp(dir->d_name, "..") != 0) {
                    array_list_add_file(self, dir->d_name);
                    //printf("%s added\n", dir->d_name);
                }
            }
            closedir(d);
        }
        // unlocks file list
        pthread_mutex_unlock(&lock_file_list);
        // after the first pass client will be unblocked
        pthread_mutex_unlock(&client);
        sleep(10);
    }
}


int main(int argc, char **argv) {

    // mutex for main node list
    pthread_mutex_init(&lock_node_list, NULL);
    // mutex for thread in the process of startup
    pthread_mutex_init(&client, NULL);
    // mutex for list of network_node self
    pthread_mutex_init(&lock_file_list, NULL);
    // mutex for thread in the process of startup
    pthread_mutex_init(&server, NULL);

    // mutex for for current requests and blacklist
    pthread_mutex_init(&lock_current, NULL);
    pthread_mutex_init(&lock_black_list, NULL);

    // server and client are initially locked (only file_daemon works)
    pthread_mutex_lock(&client);
    pthread_mutex_lock(&server);

    // create lists for databases
    node_list = create_array_list();
    black_list = create_array_list();
    current = create_array_list();

    // create self node (which represents this machine)
    self = (network_node *)malloc(sizeof(network_node));
    memset(self, 0, sizeof(network_node));

    //prepare argc and argv for pushing to client node
    void * data = malloc(sizeof(int) + sizeof(void *) * argc);
    memcpy(data, &argc, sizeof(int));
    memcpy(data + sizeof(int), argv, sizeof(void *) * argc);

    // invoke threads for all functionality
    // file daemon starts first, then unblocks client
    // and client then unblocks server
    pthread_t sync, tcp_serv, tcp_cli, daemon;
    pthread_create(&tcp_cli, NULL, tcp_client, data);
    pthread_create(&tcp_serv, NULL, tcp_server, NULL);
    pthread_create(&daemon, NULL, file_daemon, NULL);
    pthread_create(&sync, NULL, syncher, NULL);
    pthread_join(sync, NULL);
    pthread_join(tcp_serv, NULL);
    pthread_join(tcp_cli, NULL);
    pthread_join(daemon, NULL);
}