#include "sensor.h"
#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)
node* head = NULL;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
int sequenceNum = 1;
pthread_t connect_thr, datamanager_thr, storagemanager_thr;
int portNo, opt;
pid_t child_pid;
/*Hàm xử lý Connecting thread*/
void* connectThrHandler(void* args) {
    int server_fd = *((int*)args);
    sensor_data_t recv_data;
    char buffer[BUFF_SIZE];
    int new_socket_fd = connectSocket(server_fd, portNo);
    open(FIFO_FILE, O_WRONLY);
    while (1) {  
        pthread_mutex_lock(&lock);
        recv_data = readSensorData(new_socket_fd); 
        if (recv_data.id == 0) {
            pthread_mutex_unlock(&lock);
            pthread_exit(NULL);
        }
		pushBack(&head, recv_data);
        //log thông báo có sensor tạo kết nối
        snprintf(buffer, BUFF_SIZE, "%d. %s A sensor node with ID %d has opened a new connection\n", sequenceNum, 
                                                                                                    recv_data.time, 
                                                                                                    recv_data.id);
        printf("%s\n", buffer);
        writetoFifo(buffer);
        sequenceNum++;  
        pthread_mutex_unlock(&lock);
    }
    pthread_exit(NULL);
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

        // Viết log vào Fifo 
        temp = head; 
        while (1) {     
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
    sqlite3* db = databaseInit("sensordata.db");
    createTable(db);
    node* temp = head;
    while (1) {
        pthread_mutex_lock(&lock);
        insertDataFromList(db, temp);
        temp = temp->next;
        if(temp->next == NULL) {
            insertDataFromList(db, temp);
            pthread_mutex_unlock(&lock);
            pthread_exit(NULL);
        }
        pthread_mutex_unlock(&lock);     
    }
    sqlite3_close(db); 
    pthread_exit(NULL);
}

int main(int argc, const char* argv[]) {
    int server_fd;
    int status;
    struct sockaddr_in serv_addr;
    char buffer[BUFF_SIZE];
    mkfifo(FIFO_FILE, 0666);
    child_pid = fork();
    if (child_pid >= 0) {
        if (child_pid == 0) {
            int ffd = open(FIFO_FILE, O_RDONLY);
            if (ffd == -1) {
                handle_error("open()");
            }
            // Tạo file log
            int log_fd = open(LOG_FILE, O_CREAT | O_WRONLY | O_APPEND | O_TRUNC, 0666);
            if (log_fd == -1) {
                handle_error("open()");
            }

            while (1) {
                //Viết vào  file log
                ssize_t bytes_read = read(ffd, buffer, sizeof(buffer) - 1);
                if (bytes_read > 0) {
                    buffer[bytes_read] = '\0';
                    if (write(log_fd, buffer, bytes_read) == -1) {
                        handle_error("write()");
                    }
                }
            }
            close(log_fd);
            close(ffd);
            exit(status);
        } else if (child_pid > 0) {
            /* Đọc portnumber trên command line */
            if (argc < 2) {
                printf("No port provided\ncommand: ./server <port number>\n");
                exit(EXIT_FAILURE);
            } else {
                portNo = atoi(argv[1]);
            }

            /* Khởi tạo socket */
            memset(&serv_addr, '0', sizeof(struct sockaddr_in));
            serv_addr.sin_family = AF_INET;
            serv_addr.sin_port = htons(portNo);
            serv_addr.sin_addr.s_addr = INADDR_ANY;

            server_fd = socketInit(server_fd, serv_addr, opt);
            /* Khởi tạo Thread */
            int ret;
            if ((ret = pthread_create(&connect_thr, NULL, &connectThrHandler, &server_fd))) {
                printf("pthread_create() error number=%d\n", ret);
                return -1;
            }
            
            pthread_join(connect_thr, NULL);
            if ((ret = pthread_create(&datamanager_thr, NULL, &dataMngThrHandler, NULL))) {
                printf("pthread_create() error number=%d\n", ret);
                return -1;
            }
            pthread_join(datamanager_thr, NULL);
            if ((ret = pthread_create(&storagemanager_thr, NULL, &storageMngThrHandler, NULL))) {
                printf("pthread_create() error number=%d\n", ret);
                return -1;
            }
            pthread_join(storagemanager_thr, NULL);  
            wait(&status);
        }           
    }
    return 0; 
}
