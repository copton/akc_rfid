#ifndef GOGIENOOCAHZOZOHSHOK
#define GOGIENOOCAHZOZOHSHOK

typedef struct {
    char* host;
    char* user;
    char* passwd;
    char* dbname;
    int port;
} config_DB;

typedef struct {
    
} Logic_Config;

typedef struct {
    int numberofAntennas;
    char* device;
    int timeout;
} config_Reader;

typedef struct {
    config_DB db; 
    config_Reader reader;
    Logic_Config logic;
} Config;

Config* config_get(int argc, char** argv);
void config_delete(Config*);

#endif
