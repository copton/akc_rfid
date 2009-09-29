#include <stdio.h>

#include "config.h"
#include "db.h"
#include "reader.h"
#include "logic.h"

int main(int argc, char** argv)
{
    Config* config = config_get(argc, argv);
	
	db_init(&config->db);	
    reader_init(&config->reader);
    logic_init(&config->logic);

    logic_run();

    logic_shutdown();
	db_shutdown();	
    config_delete(config);
    return 0; 
}
