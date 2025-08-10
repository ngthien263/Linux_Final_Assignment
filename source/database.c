#include "database.h"
#include <sqlite3.h>
#include <stdio.h>
#include <stdbool.h>

static sqlite3 *g_db = NULL;

bool db_init(const char *path) {
    if (sqlite3_open(path, &g_db) != SQLITE_OK) {
        fprintf(stderr, "sqlite open: %s\n", sqlite3_errmsg(g_db));
        return false;
    }

    const char *DDL =
        "CREATE TABLE IF NOT EXISTS sensors_state ("
        "  id         INTEGER PRIMARY KEY,"
        "  first_seen TEXT    NOT NULL,"
        "  cur_temp   REAL,"
        "  cur_humid  REAL,"
        "  avg_temp   REAL"
        ");";

    char *errmsg = NULL;
    int rc = sqlite3_exec(g_db, DDL, NULL, NULL, &errmsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "sqlite exec(DDL): %s\n", errmsg ? errmsg : "?");
        sqlite3_free(errmsg);
        return false;
    }
    return true;
}

void db_close(void) {
    if (g_db) {
        sqlite3_close(g_db);
        g_db = NULL;
    }
}

bool db_save_state(int id,
                   const char *first_seen_iso,
                   float cur_temp,
                   float cur_humid,
                   float avg_temp)
{
    if (!g_db) return false;

    char *sql = sqlite3_mprintf(
        "INSERT INTO sensors_state(id, first_seen, cur_temp, cur_humid, avg_temp) "
        "VALUES(%d, %Q, %f, %f, %f) "
        "ON CONFLICT(id) DO UPDATE SET "
        " cur_temp=excluded.cur_temp,"
        " cur_humid=excluded.cur_humid,"
        " avg_temp=excluded.avg_temp;",
        id,
        first_seen_iso,
        (double)cur_temp,
        (double)cur_humid,
        (double)avg_temp
    );

    char *errmsg = NULL;
    int rc = sqlite3_exec(g_db, sql, NULL, NULL, &errmsg);
    sqlite3_free(sql);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "sqlite exec(UPSERT): %s\n", errmsg ? errmsg : "?");
        sqlite3_free(errmsg);
        return false;
    }
    return true;
}
