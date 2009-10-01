#include <stdio.h>

#include "config.h"
#include "db.h"
#include "reader.h"
#include "logic.h"

int main(int argc, char** argv)
{
    Config* config = config_get(argc, argv);
	
	printf("initializing database connection... "); fflush(stdout);
	db_init(&config->db);	
	printf("done\n");

	printf("initializing rfid reader... "); fflush(stdout);
    reader_init(&config->reader);
	printf("done\n");

	printf("initializing logic... "); fflush(stdout);
    logic_init(&config->logic);
	printf("done\n");

	printf("running\n");
    logic_run();

	printf("shutdown sequence\n");
    logic_shutdown();
	db_shutdown();	
    config_delete(config);
    return 0; 
}
