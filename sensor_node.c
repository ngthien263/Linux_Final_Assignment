#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/mman.h>
#include <fcntl.h>
#include "sensor_sender.h"
#include "common.h"
#include "shared_data.h"
void print_menu() {
    printf("\n  ╔═════════════════════════════════════╗\n");
    printf("  ║           SENSOR MAIN MENU          ║\n");
    printf("  ╠═════════════════════════════════════╣\n");
    printf("  ║ 1. Connect to server                ║\n");
    printf("  ║ 2. Add new sensor                   ║\n");
    printf("  ║ 3. List sensors                     ║\n");
    printf("  ║ 4. Remove sensor                    ║\n");
    printf("  ║ 5. Exit                             ║\n");
    printf("  ╚═════════════════════════════════════╝\n");
}



int main(int argc, const char* argv[]){
    int choice;
    int port_no;
    //char ip[64];
    int server_fd;
    int sensor_id;
    int status;
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0,sizeof(serv_addr));
    if (argc < 3) {
        printf("command : ./client <server address> <port number>\n");
        exit(1);
    }
    port_no = atoi(argv[2]);
    serv_addr.sin_family =  AF_INET;
    serv_addr.sin_port   = htons(port_no);
    if (inet_pton(AF_INET, argv[1], &serv_addr.sin_addr) == -1) 
        handle_error("inet_pton()");
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(connect(server_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
        handle_error("connect()");
    int shm_fd = shm_open("/temp", O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd, sizeof(shared_struct_t));
    shared_struct_t* shr = mmap(0, sizeof(shared_struct_t), PROT_WRITE | PROT_READ, MAP_SHARED, shm_fd, 0);
    sem_init(&shr->sem, 1, 0);
    pid_t child_pid = fork();
    if (child_pid < 0) {
        handle_error("fork");
    }
    else if (child_pid == 0){
        sensor_node_t* head = NULL;
        while(1){
            if(!sem_trywait(&shr->sem) && shr->has_command){
                if(shr->cmd == CMD_ADD){
                    add_sensor(&head, shr->sensor_id);
                } else if(shr->cmd == CMD_REMOVE){
                    rm_sensor(&head, shr->sensor_id);
                } else {
                    //do nothing
                }
                shr->has_command = 0;
            }
            sensor_send_message(&head, server_fd);
            sleep(1);
        }
    } else {
        while(1){
            print_menu();
            printf("Input an option: ");
            scanf("%d", &choice);
            if(choice == 1) {
                printf("Invalid choice\n");
            } else if (choice == 2) {
                printf("Input sensor ID: ");
                scanf("%d", &sensor_id);
                shr->sensor_id = sensor_id;
                shr->cmd = CMD_ADD;
                shr->has_command = 1;
                sem_post(&shr->sem);
                printf("New sensor with ID: %d created\n", sensor_id);
            } else if (choice == 3) {
                //dump_sensor(&head);
            } else if (choice == 4) {
                //dump_sensor(&head);
                printf("Input sensor ID: ");
                shr->sensor_id = sensor_id;
                shr->cmd = CMD_REMOVE;
                shr->has_command = 1;
                sem_post(&shr->sem);
            } else if (choice == 5) {
                printf("Exiting...\n");
                kill(child_pid, SIGTERM);
                wait(&status);
                close(server_fd);
                return 0;
            } else {
                printf("Invalid choice\n");
            }
        }
        close(server_fd);
        wait(&status);
    }
}