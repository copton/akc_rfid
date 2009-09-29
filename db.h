#ifndef COHSHAESEEGHEEZAIPEK
#define COHSHAESEEGHEEZAIPEK

#include "rfid.h"
#include "config.h"

void db_init(DB_Config* config);
void db_write(int antenna, Tags* enter, Tags* leave);
void db_shutdown();

#endif
