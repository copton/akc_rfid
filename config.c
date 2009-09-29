#include "config.h"

#include <mxml.h>

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

static void usage(char** argv)
{
	fprintf(stderr, "usage: %s [config_file]\n", argv[0]); 	
	exit(1);
}

static FILE* get_config_file(int argc, char** argv)
{
	if (argc > 2) {
		usage(argv);
	}

	const char* filename;
	if (argc == 2) {
		filename = argv[1];
	} else {
		filename = "./rfid_cfg.xml";
	}
	
	FILE* stream = fopen(filename, "r");
	if (stream == NULL) {
		fprintf(stderr, "failed to open '%s': %s\n", filename, strerror(errno));
		exit(1);
	}

	return stream;
}

static void copy_text(const char* path, mxml_node_t* current, const char* name, char** dst)
{
    mxml_node_t* node = mxmlFindElement(current, current, name, NULL, NULL, MXML_DESCEND_FIRST);

	if (node == NULL) {
		fprintf(stderr, "missing config item '%s/%s'\n", path, name);
		exit(1);
	}
	if (node->child == NULL) {
		fprintf(stderr, "missing text for item '%s/%s'\n", path, name);
		exit(1);
	}

	char* src = node->child->value.text.string;
	*dst = malloc(strlen(src) + 1);
	strcpy(*dst, src);
}

static void copy_int(const char* path, mxml_node_t* current, const char* name, int* dst)
{
    char* buffer;
    copy_text(path, current, name, &buffer);
    *dst = atoi(buffer);
    free(buffer);
}

static void load_database_config(mxml_node_t* current, DB_Config* config)
{
	if (current == NULL) {
		fprintf(stderr, "missing config item 'database'\n");
		exit(1);
	}

	copy_text("database", current, "host", &config->host);	
	copy_text("database", current, "user", &config->user);	
	copy_text("database", current, "passwd", &config->passwd);	
	copy_text("database", current, "dbname", &config->dbname);	
    copy_int("database", current, "port", &config->port);
}

static void load_reader_config(mxml_node_t* current, Reader_Config* config)
{
	if (current == NULL) {
		fprintf(stderr, "missing config item 'reader'\n");
		exit(1);
	}

    copy_text("reader", current, "device", &config->device); 
    copy_int("reader", current, "numberofAntennas", &config->numberofAntennas);
    copy_int("reader", current, "numberofMultiplexers", &config->numberofMultiplexers);
}

static void load_logic_config(mxml_node_t* current, Logic_Config* config)
{
	if (current == NULL) {
		fprintf(stderr, "missing config item 'logic'\n");
		exit(1);
	}
}

static void load(FILE* file, Config* config)
{
	mxml_node_t* tree = mxmlLoadFile(NULL, file, MXML_TEXT_CALLBACK);
	mxml_node_t* cfg = mxmlFindElement(tree, tree, "config", NULL, NULL, MXML_DESCEND_FIRST);
	if (cfg == NULL) {
		fprintf(stderr, "missing config item 'config'\n");
		exit(1);
	}
	
	load_database_config(mxmlFindElement(cfg, cfg, "database", NULL, NULL, MXML_DESCEND_FIRST), &config->db);
	load_logic_config(mxmlFindElement(cfg, cfg, "logic", NULL, NULL, MXML_DESCEND_FIRST), &config->logic);
    load_reader_config(mxmlFindElement(cfg, cfg, "reader", NULL, NULL, MXML_DESCEND_FIRST), &config->reader);

	mxmlDelete(tree);
}

Config* config_get(int argc, char** argv)
{
	Config* config = (Config*) malloc(sizeof(Config));
	FILE* file = get_config_file(argc, argv);
	
	load(file, config);
	fclose(file);

	return config;
}

void config_delete(Config* config)
{
	free (config->db.host);
	free (config->db.user);
	free (config->db.passwd);
	free (config->db.dbname);
	free (config);
}
