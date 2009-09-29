#include "db.h"

#include <mysql/mysql.h>
#include <stdio.h>
#include <stdlib.h>

MYSQL* mysql;

void db_init(DB_Config* config)
{
    mysql = mysql_init(NULL);
    if (mysql == NULL) {
        fprintf(stderr, "failed to init mysql\n");
        exit(1);
    }

    mysql = mysql_real_connect(mysql, config->host, config->user, config->passwd, config->dbname, config->port, NULL, 0);
    if (mysql == NULL) {
        fprintf(stderr, "failed to connect: %s\n", mysql_error(mysql));
        exit(1);
    }

	
}

void db_shutdown()
{
    mysql_close(mysql);
}

/*
 create table events (
 		id bigint unsigned auto_increment not null, 
 		antenna tinyint unsigned not null, 
 		time timestamp not null, 
 		tag bigint unsigned not null, 
 		type enum('enter', 'leave') not null, 
 		primary key(id)
 );
*/

static void write(int antenna, Tag tag, const char* type)
{
	char stmt[] = "INSERT INTO events VALUES(NULL, %d, NULL, %lld, '%s');";
	char buffer[sizeof(stmt) + 3 + 20 + 5];

	int res = snprintf(buffer, sizeof(buffer)-1, stmt, antenna, tag, type);
	if (res >= sizeof(buffer)-1) {
		fprintf(stderr, "fatal: not enough buffer to create query string\n");
		exit(1);
	}
	
	int mres = mysql_query(mysql, buffer);
	if (mres != 0) {
		fprintf(stderr, "query failed: %s\n", mysql_error(mysql));
		exit(1);
	}
}

void db_write(int antenna, Tags* enter, Tags* leave)
{
//	printf("db_write(%d, %d, %d)\n", antenna, enter->size, leave->size);
	int i;
	if (enter) {
		for (i=0; i < enter->size; i++) {
			write(antenna, enter->tags[i], "enter");
		}
	}

	if (leave) {
		for (i=0; i < leave->size; i++) {
			write(antenna, leave->tags[i], "leave");
		}
	}
}
