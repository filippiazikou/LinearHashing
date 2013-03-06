#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "BF.h"
#include "LH.h"

#define MODE "r"
#define LINE_SIZE 256
#define DELIMITERS " \t"

#define FILENAME_HASH_ID "hash_id1"
#define FILENAME_HASH_SML "hash_id2"
#define FILENAME_HASH_SUR "hash_sur"
#define FILENAME_HASH_NAM "hash_nam"
#define FILENAME_HASH_CIT "hash_cit"

int main(int argc, char** argv) {

	int i;
	LH_info *hashId, *hashSur, *hashNm, *hashCt, *hashSml;
	char line [LINE_SIZE];
	char* token;
	Record record;

	BF_Init();

	/* create hash index on id */
	if ( LH_CreateIndex(FILENAME_HASH_ID, ID, 'i', sizeof(int), 4, (float)0.9) < 0 ) {
		fprintf(stderr, "Error creating hash index\n");
		exit(EXIT_FAILURE);
	}

	if ( LH_CreateIndex(FILENAME_HASH_SML, ID, 'i', sizeof(int), 16, (float)0.9) < 0 ) {
		fprintf(stderr, "Error creating 2nd hash index for ids\n");
		exit(EXIT_FAILURE);
	}

	/* create hash index on surname */
	if ( LH_CreateIndex(FILENAME_HASH_SUR, SURNAME, 'c', SURNAME_SIZE, 64, (float)0.8) < 0) {
		fprintf(stderr, "Error creating hash index for surname\n");
		exit(EXIT_FAILURE);
	}

	/* create hash index on city */
	if ( LH_CreateIndex(FILENAME_HASH_CIT, CITY, 'c', CITY_SIZE, 32, (float)0.85) < 0 ) {
		fprintf(stderr, "Error creating hash index for city \n");
		exit(EXIT_FAILURE);
	}

	/* create hash index on name */
	if ( LH_CreateIndex(FILENAME_HASH_NAM, NAME, 'c', NAME_SIZE, 32, (float)0.85) < 0 ) {
		fprintf(stderr, "Error creating hash index for name\n");
		exit(EXIT_FAILURE);
	}

	/* ************************************* OPEN HASH INDEXES ************************************* */
	if ( (hashId = LH_OpenIndex(FILENAME_HASH_ID)) == NULL) { /* open hash index based on id */
		fprintf(stderr, "Error opening 1st hash index on id\n");
		exit(EXIT_FAILURE);
	}

	if ( (hashSml = LH_OpenIndex(FILENAME_HASH_SML)) == NULL) { /* open hash index based on id */
		fprintf(stderr, "Error opening 2nd hash index on id\n");
		LH_CloseIndex(hashId);
		exit(EXIT_FAILURE);
	}

	if ((hashSur = LH_OpenIndex(FILENAME_HASH_SUR)) == NULL) { /* open hash index based on surname */
		fprintf(stderr, "Error opening hash index on surname\n");
		LH_CloseIndex(hashSml);
		LH_CloseIndex(hashId);
		exit(EXIT_FAILURE);
	}

	if ((hashCt = LH_OpenIndex(FILENAME_HASH_CIT)) == NULL) { /* open hash index based on city */
		fprintf(stderr, "Error opening hash index on city\n");
		LH_CloseIndex(hashId);
		LH_CloseIndex(hashSml);
		LH_CloseIndex(hashSur);
		exit(EXIT_FAILURE);
	}

	if ((hashNm = LH_OpenIndex(FILENAME_HASH_NAM)) == NULL) { /* open hash index based on name */
		fprintf(stderr, "Error opening hash index on name\n");
		LH_CloseIndex(hashId);
		LH_CloseIndex(hashSml);
		LH_CloseIndex(hashSur);
		LH_CloseIndex(hashCt);
		exit(EXIT_FAILURE);
	}

	printf("\n");
	printf("         NOW INSERTING RECORDS         \n");
	printf("=======================================\n");
	while ( fgets(line, LINE_SIZE - 1, stdin) != NULL ) { /* read line, until eof */

		if ( strlen(line) == 0 ) /* skip empty lines */
			continue;

		if ((token = strtok(line, DELIMITERS)) == NULL) { /* parse id */
			fprintf(stderr, "Error parsing id from line %s\n", line);
			LH_CloseIndex(hashId);
			LH_CloseIndex(hashSml);
			LH_CloseIndex(hashSur);
			LH_CloseIndex(hashCt);
			LH_CloseIndex(hashNm);
			exit(EXIT_FAILURE);
		}
		record.id = atoi(token);

		if ((token = strtok(NULL, DELIMITERS)) == NULL) { /* parse name */
			fprintf(stderr, "Error parsing name from line %s\n", line);
			LH_CloseIndex(hashId);
			LH_CloseIndex(hashSml);
			LH_CloseIndex(hashSur);
			LH_CloseIndex(hashCt);
			LH_CloseIndex(hashNm);
			exit(EXIT_FAILURE);
		}
		strncpy(record.name, token, NAME_SIZE);

		if ((token = strtok(NULL, DELIMITERS)) == NULL) { /* parse surname */
			fprintf(stderr, "Error parsing surname from line %s\n", line);
			LH_CloseIndex(hashId);
			LH_CloseIndex(hashSml);
			LH_CloseIndex(hashSur);
			LH_CloseIndex(hashCt);
			LH_CloseIndex(hashNm);
			exit(EXIT_FAILURE);
		}
		strncpy(record.surname, token, SURNAME_SIZE);

		if ((token = strtok(NULL, DELIMITERS)) == NULL) { /* parse city */
			fprintf(stderr, "Error parsing city from line %s\n", line);
			LH_CloseIndex(hashId);
			LH_CloseIndex(hashSml);
			LH_CloseIndex(hashSur);
			LH_CloseIndex(hashCt);
			LH_CloseIndex(hashNm);
			exit(EXIT_FAILURE);
		}
		strncpy(record.city, token, CITY_SIZE);

		for (i = 0; i < strlen(record.city); i++) /* replace trailing new line characters */
			if (record.city[i] == '\n' || record.city[i] == '\r')
				record.city[i] = '\0';

		if (LH_InsertEntry(hashId, record) < 0) { /* insert record in hash index based on id */
			fprintf(stderr, "Error inserting entry in 1st hash index on id\n");
			LH_CloseIndex(hashId);
			LH_CloseIndex(hashSml);
			LH_CloseIndex(hashSur);
			LH_CloseIndex(hashCt);
			LH_CloseIndex(hashNm);
			exit(EXIT_FAILURE);
		}

		if (LH_InsertEntry(hashSur, record) < 0) { /* insert record in hash index based on surname */
			fprintf(stderr, "Error inserting entry in hash index on surname\n");
			LH_CloseIndex(hashId);
			LH_CloseIndex(hashSml);
			LH_CloseIndex(hashSur);
			LH_CloseIndex(hashCt);
			LH_CloseIndex(hashNm);
			exit(EXIT_FAILURE);
		}

		if ( (record.id % 10) == 0 ) /* Insert only a portion of the records in the small index */
			if (LH_InsertEntry(hashSml, record) < 0) { /* insert record in hash index based on id */
				fprintf(stderr, "Error inserting entry in 2nd hash index on id\n");
				LH_CloseIndex(hashId);
				LH_CloseIndex(hashSml);
				LH_CloseIndex(hashSur);
				LH_CloseIndex(hashCt);
				LH_CloseIndex(hashNm);
				exit(EXIT_FAILURE);
			}

		if (LH_InsertEntry(hashCt, record) < 0) { /* insert record in hash index based on city */
			fprintf(stderr, "Error inserting entry in hash index on city \n");
			LH_CloseIndex(hashId);
			LH_CloseIndex(hashSml);
			LH_CloseIndex(hashSur);
			LH_CloseIndex(hashCt);
			LH_CloseIndex(hashNm);
			exit(EXIT_FAILURE);
		}

		if (LH_InsertEntry(hashNm, record) < 0) { /* insert record in hash index based on name */
			fprintf(stderr, "Error inserting entry in hash index on name\n");
			LH_CloseIndex(hashId);
			LH_CloseIndex(hashSml);
			LH_CloseIndex(hashSur);
			LH_CloseIndex(hashCt);
			LH_CloseIndex(hashNm);
			exit(EXIT_FAILURE);
		}

		printf("INSERTED: %d\t%s\t%s\t%s\n", record.id, record.name, record.surname, record.city);
	}

	printf("Closing hash index on id.......");
	if (LH_CloseIndex(hashId) < 0) { /* close id hash index */
		fprintf(stderr, "Error closing id hash index\n");
		LH_CloseIndex(hashSur);
		LH_CloseIndex(hashCt);
		LH_CloseIndex(hashNm);
		LH_CloseIndex(hashSml);
		exit(EXIT_FAILURE);
	}
	printf("...... closed\n");

	printf("Closing hash index on surname...");
	if (LH_CloseIndex(hashSur) < 0) { /* close surname hash index */
		fprintf(stderr, "Error closing surname hash index\n");
		LH_CloseIndex(hashCt);
		LH_CloseIndex(hashNm);
		LH_CloseIndex(hashSml);
		exit(EXIT_FAILURE);
	}
	printf("..... closed\n");

	printf("Closing hash index on name.....");
	if (LH_CloseIndex(hashNm) < 0) { /* close name hash index */
		fprintf(stderr, "Error closing surname hash index\n");
		LH_CloseIndex(hashCt);
		LH_CloseIndex(hashSml);
		exit(EXIT_FAILURE);
	}
	printf("...... closed\n");

	printf("Closing hash index on city.....");
	if (LH_CloseIndex(hashCt) < 0) { /* close city hash index */
		fprintf(stderr, "Error closing surname hash index\n");
		LH_CloseIndex(hashSml);
		exit(EXIT_FAILURE);
	}
	printf("...... closed\n");

	printf("Closing hash index 2 on id.....");
	if ( LH_CloseIndex(hashSml) < 0 ){
		fprintf(stderr, "Error closing surname hash index\n");
		exit(EXIT_FAILURE);
	}
	printf("...... closed\n");

	return 0;
}

