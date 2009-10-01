#ifndef COHSHAESEEGHEEZAIPEK
#define COHSHAESEEGHEEZAIPEK

#include "rfid.h"
#include "config.h"

void db_init(config_DB* config);
void db_write(int antenna, Tags* enter, Tags* leave);
void db_shutdown();

#endif
