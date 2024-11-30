#include "database.h"
#include "linkedList.h"
#include "log.h"
sqlite3* databaseInit(const char* filename) {
    char buffer[BUFF_SIZE];
    sqlite3* db;
    int db_fd = sqlite3_open(filename, &db);
    if(db_fd) { 
        printf("Unable to connect to SQL server: %s\n", sqlite3_errmsg(db)); 
        snprintf(buffer, BUFF_SIZE, "Unable to connect to SQL server\n");
        writetoFifo(buffer);
        //sequenceNum++;
        return NULL; 
    } 
    else { 
        printf("Connection to SQL server established\n");
        snprintf(buffer, BUFF_SIZE, "Connection to SQL server established\n");
        writetoFifo(buffer);
        //sequenceNum++;
    }
    return db;
} 

int createTable(sqlite3* db) {
    const char *sql = "CREATE TABLE IF NOT EXISTS SensorData ("
                      "id INTEGER," 
                      "time TEXT," 
                      "temperature REAL);";
    char* errMsg = 0;
    int rc = sqlite3_exec(db, sql, NULL, 0, &errMsg);
    if (rc != SQLITE_OK) {
        printf("SQL error: %s\n", errMsg); 
        sqlite3_free(errMsg); 
        return rc;
    } else {
        printf("New table SensorData created\n");
    }
    return SQLITE_OK;
}

int insertDataFromList(sqlite3 *db, node *temp) {
    char *errMsg = 0;
    char sql[256];
    int rc;
        printf("%s, %d, %f\n", temp->data.time, temp->data.id,  temp->data.temperature);
        sprintf(sql, "INSERT OR REPLACE INTO SensorData (time, id, temperature) VALUES ('%s', %d, %.2f);",
                temp->data.time, temp->data.id, temp->data.temperature);
        rc = sqlite3_exec(db, sql, NULL, 0, &errMsg);
        if (rc != SQLITE_OK) {
            printf("SQL error: %s\n", errMsg);
            sqlite3_free(errMsg);
        }
    return SQLITE_OK;
}
