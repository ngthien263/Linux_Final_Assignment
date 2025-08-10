#ifndef DATABASE_H
#define DATABASE_H

#include <stdbool.h>

bool db_init(const char *path);
void db_close(void);
bool db_save_state(int id, const char *first_seen_iso, float cur_temp, float cur_humid, float avg_temp);

#endif
