#ifndef GOGIENOOCAHZOZOHSHOK
#define GOGIENOOCAHZOZOHSHOK

typedef struct {
    char* host;
    char* user;
    char* passwd;
    char* dbname;
    int port;
} DB_Config;

typedef struct {
    
} Logic_Config;

typedef struct {
    int numberofAntennas;
    int numberofMultiplexers;
    char* device;
    int timeout;
} Reader_Config;

typedef struct {
    DB_Config db; 
    Reader_Config reader;
    Logic_Config logic;
} Config;

Config* config_get(int argc, char** argv);
void config_delete(Config*);

#endif
