#ifndef KIEZAHHEJEIKAISHAESH
#define KIEZAHHEJEIKAISHAESH

#include "config.h"
#include "rfid.h"

typedef struct {
    unsigned int size;
    Tags* antenna_results;
} reader_Result;

void reader_init(Reader_Config*);

reader_Result* reader_read();
void reader_free(reader_Result*);

int reader_numberofAntennas();

#endif
