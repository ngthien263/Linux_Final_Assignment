#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>     //  Chứa cấu trúc cần thiết cho socket. 
#include <netinet/in.h>     //  Thư viện chứa các hằng số, cấu trúc khi sử dụng địa chỉ trên internet
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#define LISTEN_BACKLOG 50
#define BUFF_SIZE 256
#define FIFO_FILE   "./logfifo"
#define LOG_FILE    "gateway.log"
#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

#define TEMP_THRESHOLD 30
pthread_t connect_thr, datamanager_thr, storagemanager_thr;
struct sockaddr_in clientAddr;
int portNo, opt;
pid_t child_pid;
int sequenceNum = 1;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
typedef struct data {
    int id;
    float temperature;
    char time[30];
} data;

/*Khởi tạo danh sách liên kết*/
typedef struct node {
    data data;
    struct node* prev;
    struct node* next;
} node;
node* head = NULL;
node* makeNode(data inputData) {
    node* new_node = (node*)malloc(sizeof(node));
    if (new_node == 0){
        handle_error("malloc()");
    }
    new_node->data = inputData;
    new_node->next = NULL;
    new_node->prev = NULL;
    return new_node;
}

void pushBack(node** fhead, data inputData) {
    node* new_node = makeNode(inputData);
    if(*fhead == NULL) {
        *fhead = new_node; 
        return;
    }
    node* temp = *fhead;
    while(temp->next != NULL) {
        temp = temp->next;
    }
    temp->next = new_node;
    new_node->prev = temp;
    new_node->next = NULL;
}

/*Đọc data từ socket*/
data readSensorData(int newSocketFd) {
    int numb_read;
    char recvbuff[BUFF_SIZE];
    data recvData;
    memset(recvbuff, '0', BUFF_SIZE);
    if (read(newSocketFd, recvbuff, BUFF_SIZE) == -1)
            handle_error("read()");
    //Phân tích chuỗi đọc được từ data thành các trường trong recvData
    sscanf(recvbuff, "%[^-] - ID: %d\nTemp: %f\n", recvData.time, 
                                                   &recvData.id, 
                                                   &recvData.temperature);
    return recvData;
}

void writetoFifo (char* buffer) {
    int ffd = open(FIFO_FILE, O_WRONLY);
    if (ffd == -1)
        handle_error("open()");
    write(ffd, buffer, strlen(buffer));
    close(ffd);
}

int connectSocket(int server_fd) {
    if (listen(server_fd, LISTEN_BACKLOG) == -1)
        handle_error("listen()");
    socklen_t clientLen  = sizeof(clientAddr);
    printf("Server is listening at port : %d \n....\n",portNo);
    int newSocketFd = accept(server_fd, (struct sockaddr*)&clientAddr, &clientLen);
    if (newSocketFd == -1)
        handle_error("accept()");
    printf("Connected");
    return newSocketFd;
}



/*Hàm xử lý Connecting thread*/
void* connectThrHandler(void* args) {
    int server_fd = *((int*)args);
    data recv_data;
    char buffer[BUFF_SIZE];
    int newSocketFd = connectSocket(server_fd);
    open(FIFO_FILE, O_WRONLY);
    while (1) {  
        pthread_mutex_lock(&lock);
        recv_data = readSensorData(newSocketFd); 
        if (recv_data.id == 0) {
            pthread_mutex_unlock(&lock);
            pthread_exit(NULL);
        }
		pushBack(&head, recv_data);
        //log thông báo có sensor tạo kết nối
        snprintf(buffer, BUFF_SIZE, "%d. %s A sensor node with ID %d has opened a new connection", sequenceNum, 
                                                                                                    recv_data.time, 
                                                                                                    recv_data.id);
        printf("%s\n", buffer);
        writetoFifo(buffer);
        sequenceNum++;  
        pthread_mutex_unlock(&lock);
    }
    pthread_exit(NULL);
}

void dataMngLog(float avgTemp, node* temp) {
    char buffer[BUFF_SIZE];
    if (temp->data.id < 0) { 
        // log nếu nhận được sensor id không hợp lệ
        snprintf(buffer, BUFF_SIZE, "%d. %s Received sensor data with invalid sensor node ID %d", sequenceNum, 
                                                                                                   temp->data.time, 
                                                                                                   temp->data.id);
        printf("%s\n", buffer); 
        writetoFifo(buffer); 
        sequenceNum++;
        
    } else {
        if (temp->data.temperature < avgTemp) { 
            // log thông báo nhiệt độ quá lạnh
            snprintf(buffer, BUFF_SIZE, "%d. %s The sensor node with ID %d reports it's too cold (avgtemperature = %.2f)",  sequenceNum, 
                                                                                                                            temp->data.time, 
                                                                                                                            temp->data.id,
                                                                                                                            avgTemp);
            printf("%s\n", buffer);
            writetoFifo(buffer);
            sequenceNum++;  
        } else if (temp->data.temperature > avgTemp) { 
            // log thông báo nhiệt độ quá nóng
            snprintf(buffer, BUFF_SIZE, "%d. %s The sensor node with ID %d reports it's too hot (avgtemperature = %.2f)",   sequenceNum, 
                                                                                                                            temp->data.time, 
                                                                                                                            temp->data.id,
                                                                                                                            avgTemp);
            printf("%s\n", buffer); 
            writetoFifo(buffer);
            sequenceNum++;
        }
    }
}

/* Hàm xử lý Data Manager thread */
void* dataMngThrHandler(void* args) {
    float avgTemp = 0;
    node* temp;
    int division;
    open(FIFO_FILE, O_WRONLY);
    while (1) {
        pthread_mutex_lock(&lock);
        temp = head;
        avgTemp = 0; 
        division = 0;
        // Tính toán nhiệt độ trung bình
        while (temp != NULL) {
            avgTemp += temp->data.temperature;
            division++;
            temp = temp->next;
        }
        avgTemp = avgTemp / division;

        // Viết log vào Fifo cho từng node
        temp = head; 
        while (temp != NULL) {     
            dataMngLog(avgTemp, temp);
            temp = temp->next;
            if (temp->next == NULL) {
                dataMngLog(avgTemp, temp);
                pthread_mutex_unlock(&lock);
                pthread_exit(NULL);
            }
        }
        pthread_mutex_unlock(&lock);
    }   
    pthread_exit(NULL);
}


/*Hàm xử lý Storage Manager thread*/
void* storageMngThrHandler(void* args) {
    // while (1) {
    //     pthread_mutex_lock(&lock);
        
    //     pthread_mutex_unlock(&lock);
    // }
}

int main(int argc, const char* argv[]) {
    int server_fd, newSocketFd;
    struct sockaddr_in serv_addr;
    char buffer[BUFF_SIZE];
    mkfifo(FIFO_FILE, 0666);
    child_pid = fork();
    if (child_pid >= 0) {
        if (child_pid == 0) {
            int ffd = open(FIFO_FILE, O_RDONLY);
            if (ffd == -1) {
                handle_error("open FIFO");
            }

            // Tạo file log
            int log_fd = open(LOG_FILE, O_CREAT | O_WRONLY | O_APPEND | O_TRUNC, 0666);
            if (log_fd == -1) {
                handle_error("open()");
            }

            while (1) {
                ssize_t bytes_read = read(ffd, buffer, sizeof(buffer) - 1);
                if (bytes_read > 0) {
                    buffer[bytes_read] = '\0';
                    if (write(log_fd, buffer, bytes_read) == -1) {
                        handle_error("write()");
                    }
                    if (write(log_fd, "\n", 1) == -1) {
                        handle_error("write()");
                    }
                }
            }
            close(log_fd);
            close(ffd);

        } else if (child_pid > 0) {
            /* Đọc portnumber trên command line */
            if (argc < 2) {
                printf("No port provided\ncommand: ./server <port number>\n");
                exit(EXIT_FAILURE);
            } else {
                portNo = atoi(argv[1]);
            }

            memset(&serv_addr, '0', sizeof(struct sockaddr_in));
            memset(&clientAddr, '0', sizeof(struct sockaddr_in));

            /* Khởi tạo socket */
            server_fd = socket(AF_INET, SOCK_STREAM, 0);
            if (server_fd == -1)
                handle_error("socket()");

            // Ngăn lỗi: “address already in use”
            if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) == -1)
                handle_error("setsockopt()");

            serv_addr.sin_family = AF_INET;
            serv_addr.sin_port = htons(portNo);
            serv_addr.sin_addr.s_addr = INADDR_ANY;

            if (bind(server_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
                handle_error("bind()");

            /* Khởi tạo Thread */
            int ret;
            if (ret = pthread_create(&connect_thr, NULL, &connectThrHandler, &server_fd)) {
                printf("pthread_create() error number=%d\n", ret);
                return -1;
            }
            pthread_join(connect_thr, NULL);
            if (ret = pthread_create(&datamanager_thr, NULL, &dataMngThrHandler, NULL)) {
                printf("pthread_create() error number=%d\n", ret);
                return -1;
            }
            pthread_join(datamanager_thr, NULL);
            if (ret = pthread_create(&storagemanager_thr, NULL, &storageMngThrHandler, NULL)) {
                printf("pthread_create() error number=%d\n", ret);
                return -1;
            }
            pthread_join(storagemanager_thr, NULL);  
        }           
    }
    return 0; 
}
