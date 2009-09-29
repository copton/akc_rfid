#include "logic.h"
#include "reader.h"
#include "db.h"

#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <stdlib.h>

static Logic_Config* cfg;

void logic_init(Logic_Config* config)
{
	cfg = config;
}

/*
static void print(reader_Result* result)
{
	int i, j;
	static int count = 0;	
	printf("\n %d ------------------------\n", count++);
	for(i=0; i<result->size; i++) {
		printf("\n");
		Tags* cur = result->antenna_results + i;
		for (j=0; j<cur->size; j++) {
			printf("%llX ", (long long unsigned) cur->tags[j]);
		}
	}
	printf("\n");
}
*/

static Tags** oldTags = NULL;
static reader_Result* oldResult = NULL;

void logic_shutdown()
{
	free(oldTags);
	reader_free(oldResult);
}

static int find(Tag what, Tags* where)
{
	int i;
	for (i=0; i<where->size; i++) {
		if (where->tags[i] == what) {
			return 1;	
		}
	}
	return 0;
}	

void logic_run()
{
	int numberofAntennas = reader_numberofAntennas();
	oldTags = malloc(numberofAntennas * sizeof(Tags*));
	{
		int i;
		for (i=0; i<numberofAntennas; i++) {
			oldTags[i] = NULL;
		}
	}

	while(1) {	
		reader_Result* result = reader_read();
		assert(numberofAntennas == result->size);
		
		int antenna;
		for (antenna=0; antenna<numberofAntennas; antenna++) {
			Tags* new = result->antenna_results + antenna;
			Tags* old = oldTags[antenna];

			Tags enter;
			enter.tags = malloc(new->size * sizeof(Tag));
			enter.size = 0;
			{
				int i;
				for (i=0; i<new->size; i++) {
					if (old == NULL || ! find(new->tags[i], old)) {
						enter.tags[enter.size++] = new->tags[i];	
					}
				}	
			}

			Tags leave;
			leave.size = 0;
			if (old) {
				leave.tags = malloc(old->size * sizeof(Tag));
				{
/*
					int j;
					for (j=0; j<old->size; j++) printf("old[%d] = %llX ", j, (long long unsigned) old->tags[j]);
					printf("\n");
*/
					int i;
					for (i=0; i<old->size; i++) {
						if (! find(old->tags[i], new)) {
							leave.tags[leave.size++] = old->tags[i];
						}
					}
				}
			} else {
				leave.tags = NULL;
			}

			db_write(antenna, &enter, &leave);

			free(enter.tags);
			free(leave.tags);

			oldTags[antenna] = new;
		}

		if (oldResult) reader_free(oldResult);
		oldResult = result;
	
		usleep(250 * 1000);
	}	
}
