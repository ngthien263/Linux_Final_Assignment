#ifndef DATABASE_H
#define DATABASE_H
#include <sqlite3.h>
#include <stdio.h>
#include "linkedList.h"
#define BUFF_SIZE 256
sqlite3* databaseInit(const char* filename);
int createTable(sqlite3* db);
int insertDataFromList(sqlite3 *db, node *temp);

#endif
