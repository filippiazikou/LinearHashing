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

#define ID_VALUE 3
#define ID_SML 100
#define NAME_VALUE "Stefanos"
#define SURNAME_VALUE "Pefani"
#define CITY_VALUE "Thes/niki"

int main(int argc, char** argv) {

	int id = ID_VALUE;
	LH_info *hashId, *hashSur, *hashNm, *hashCt, *hashSml;
	void* value;
	int recCnt;

	BF_Init();


	/* Opening and checking linear hash index on id */
	if ( (hashId = LH_OpenIndex(FILENAME_HASH_ID)) == NULL ) { /* open hash index on id */
		fprintf(stderr, "Error opening hash index1\n");
		exit(EXIT_FAILURE);
	}

	if ( strcmp(hashId->attrName, ID) ){
		fprintf(stderr, "Invalid type of hash index. Unknown hash field for id index\n");
		LH_CloseIndex(hashId);
		exit(EXIT_FAILURE);
	}


	/* Opening and checking linear hash index on city */
	if ( (hashCt = LH_OpenIndex(FILENAME_HASH_CIT)) == NULL ) { /* open hash index on city */
		fprintf(stderr, "Error opening hash index2\n");
		LH_CloseIndex(hashId);
		exit(EXIT_FAILURE);
	}

	if ( strcmp(hashCt->attrName, CITY) ){
		fprintf(stderr, "Invalid type of hash index. Unknown hash field for city index\n");
		LH_CloseIndex(hashId);
		LH_CloseIndex(hashCt);
		exit(EXIT_FAILURE);
	}

	/* Opening and checking linear hash index on surname */
	if ( (hashSur = LH_OpenIndex(FILENAME_HASH_SUR)) == NULL ) { /* open hash index on surname */
		fprintf(stderr, "Error opening hash index3\n");
		LH_CloseIndex(hashId);
		LH_CloseIndex(hashCt);
		exit(EXIT_FAILURE);
	}

	if ( strcmp(hashSur->attrName, SURNAME) ){
		fprintf(stderr, "Invalid type of hash index. Unknown hash field for surname index\n");
		LH_CloseIndex(hashId);
		LH_CloseIndex(hashCt);
		LH_CloseIndex(hashSur);
		exit(EXIT_FAILURE);
	}

	/* Opening and checking linear hash index on small index */
	if ( (hashSml = LH_OpenIndex(FILENAME_HASH_SML)) == NULL ) { /* open hash index on surname */
		fprintf(stderr, "Error opening hash index4\n");
		LH_CloseIndex(hashId);
		LH_CloseIndex(hashCt);
		LH_CloseIndex(hashSur);
		exit(EXIT_FAILURE);
	}

	if ( strcmp(hashSml->attrName, ID) ){
		fprintf(stderr, "Invalid type of hash index. Unknown hash field for id index 2\n");
		LH_CloseIndex(hashId);
		LH_CloseIndex(hashCt);
		LH_CloseIndex(hashSur);
		LH_CloseIndex(hashSml);
		exit(EXIT_FAILURE);
	}

	/* Opening and checking linear hash index on name */
	if ( (hashNm = LH_OpenIndex(FILENAME_HASH_NAM)) == NULL ) { /* open hash index on name */
		fprintf(stderr, "Error opening hash index5\n");
		BF_PrintError("sdfads");
		LH_CloseIndex(hashId);
		LH_CloseIndex(hashCt);
		LH_CloseIndex(hashSur);
		LH_CloseIndex(hashSml);
		exit(EXIT_FAILURE);
	}

	if ( strcmp(hashNm->attrName, NAME) ){
		fprintf(stderr, "Invalid type of hash index. Unknown hash field for name index\n");
		LH_CloseIndex(hashId);
		LH_CloseIndex(hashCt);
		LH_CloseIndex(hashSur);
		LH_CloseIndex(hashSml);
		LH_CloseIndex(hashNm);
		exit(EXIT_FAILURE);
	}


	printf("\n\n");
	printf("=====================================\n");
	printf("NOW SEARCHING IN ID INDEX FOR ID = %d\n", id);
	printf("=====================================\n");
	if ( ( recCnt = LH_FindAllEntries(*hashId, (void*)&id)) < 0 ){
		fprintf(stderr, "Error in finding entries while searching id index\n");
		LH_CloseIndex(hashId);
		LH_CloseIndex(hashCt);
		LH_CloseIndex(hashSur);
		LH_CloseIndex(hashSml);
		LH_CloseIndex(hashNm);
		exit(EXIT_FAILURE);
	}else{
		printf("Located %d records with ID = %d\n", recCnt, id);
	}

	printf("\n\n");
	printf("===============================================\n");
	printf("NOW SEARCHING IN NAME INDEX FOR NAME = %s\n", NAME_VALUE);
	printf("===============================================\n");
	value = NAME_VALUE;
	if ( ( recCnt = LH_FindAllEntries(*hashNm, value)) < 0 ){
		fprintf(stderr, "Error in finding entries while searching name index\n");
		LH_CloseIndex(hashId);
		LH_CloseIndex(hashCt);
		LH_CloseIndex(hashSur);
		LH_CloseIndex(hashSml);
		LH_CloseIndex(hashNm);
		exit(EXIT_FAILURE);
	}else{
		printf("Located %d records with NAME = %s\n", recCnt, NAME_VALUE);
	}

	printf("\n\n");
	printf("===============================================\n");
	printf("NOW SEARCHING IN CITY INDEX FOR CITY = %s\n", CITY_VALUE);
	printf("===============================================\n");
	value = CITY_VALUE;
	if ( ( recCnt = LH_FindAllEntries(*hashCt, value)) < 0 ){
		fprintf(stderr, "Error in finding entries while searching city index\n");
		LH_CloseIndex(hashId);
		LH_CloseIndex(hashCt);
		LH_CloseIndex(hashSur);
		LH_CloseIndex(hashSml);
		LH_CloseIndex(hashNm);
		exit(EXIT_FAILURE);
	}else{
		printf("Located %d records with CITY = %s\n", recCnt, CITY_VALUE);
	}

	printf("\n\n");
	printf("===============================================\n");
	printf("NOW SEARCHING IN ID INDEX 2 FOR ID = %d\n", ID_SML);
	printf("===============================================\n");
	id = ID_SML;
	if ( ( recCnt = LH_FindAllEntries(*hashSml, (void*)&id)) < 0 ){
		fprintf(stderr, "Error in finding entries while searching small id index\n");
		LH_CloseIndex(hashId);
		LH_CloseIndex(hashCt);
		LH_CloseIndex(hashSur);
		LH_CloseIndex(hashSml);
		LH_CloseIndex(hashNm);
		exit(EXIT_FAILURE);
	}else{
		printf("Located %d records with ID = %d\n", recCnt, id);
	}

	printf("\n\n");
	printf("=====================================================\n");
	printf("NOW SEARCHING IN INDEX FOR SURNAME = %s\n", SURNAME_VALUE);
	printf("=====================================================\n");
	value = SURNAME_VALUE;
	if ( ( recCnt = LH_FindAllEntries(*hashSur, value)) < 0 ){
		fprintf(stderr, "Error in finding entries while searching surname index\n");
		LH_CloseIndex(hashId);
		LH_CloseIndex(hashCt);
		LH_CloseIndex(hashSur);
		LH_CloseIndex(hashSml);
		LH_CloseIndex(hashNm);
		exit(EXIT_FAILURE);
	}else{
		printf("Located %d records with SURNAME = %s\n", recCnt, SURNAME_VALUE);
	}


	if ( LH_CloseIndex(hashId) < 0 ) { /* close hash index */
		fprintf(stderr, "Error closing hash index on id\n");
		LH_CloseIndex(hashCt);
		LH_CloseIndex(hashSur);
		LH_CloseIndex(hashSml);
		LH_CloseIndex(hashNm);
		exit(EXIT_FAILURE);
	}

	if ( LH_CloseIndex(hashCt) < 0 ) { /* close hash index */
		fprintf(stderr, "Error closing hash index on city\n");
		LH_CloseIndex(hashSur);
		LH_CloseIndex(hashSml);
		LH_CloseIndex(hashNm);
		exit(EXIT_FAILURE);
	}

	if ( LH_CloseIndex(hashSur) < 0 ) { /* close hash index */
		fprintf(stderr, "Error closing hash index on surname\n");
		LH_CloseIndex(hashSml);
		LH_CloseIndex(hashNm);
		exit(EXIT_FAILURE);
	}

	if ( LH_CloseIndex(hashSml) < 0 ) { /* close hash index */
		fprintf(stderr, "Error closing hash small index on id\n");
		LH_CloseIndex(hashNm);
		exit(EXIT_FAILURE);
	}

	if ( LH_CloseIndex(hashNm) < 0 ) { /* close hash index */
		fprintf(stderr, "Error closing hash index on name\n");
		exit(EXIT_FAILURE);
	}


	printf("\n");
	return 0;
}

