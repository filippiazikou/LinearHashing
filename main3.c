#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "BF.h"
#include "LH.h"

#define FILENAME_HASH_ID "hash_id1"
#define FILENAME_HASH_SML "hash_id2"
#define FILENAME_HASH_SUR "hash_sur"
#define FILENAME_HASH_NAM "hash_nam"
#define FILENAME_HASH_CIT "hash_cit"

#define ID_SML 6780

int main(){

	int id;
	int recCnt;

	/*  */
	BF_Init();

	LH_info* smlInf;

	if ( (smlInf = LH_OpenIndex(FILENAME_HASH_SML)) == NULL ){
		fprintf(stderr, "Error opening hash index on name\n");
		exit(EXIT_FAILURE);
	}

	id = ID_SML;
	printf("\n\n");
	printf("===============================================\n");
	printf("NOW SEARCHING IN ID INDEX 2 FOR ID = %d\n", id);
	printf("===============================================\n");
	if ( ( recCnt = LH_FindAllEntries(*smlInf, (void*)&id)) < 0 ){
		fprintf(stderr, "Error in finding entries while searching small id index\n");
		LH_CloseIndex(smlInf);
		exit(EXIT_FAILURE);
	}else{
		printf("Located %d records with ID = %d\n", recCnt, id);
	}

	if ( LH_CloseIndex(smlInf) < 0 ){
		fprintf(stderr, "Error in finding entries while searching small id index\n");
		exit(EXIT_FAILURE);
	}

	printf("Requesting statistics for %s\n", FILENAME_HASH_ID);
	printf("------------------------------------\n", FILENAME_HASH_ID);
	if ( HashStatistics(FILENAME_HASH_ID) < 0 ){
		fprintf(stderr, "Error in getting hash statistics\n");
		exit(EXIT_FAILURE);
	}

	printf("\nRequesting statistics for %s\n", FILENAME_HASH_NAM);
	printf("------------------------------------\n", FILENAME_HASH_NAM);
	if ( HashStatistics(FILENAME_HASH_NAM) < 0 ){
		fprintf(stderr, "Error in getting hash statistics\n");
		exit(EXIT_FAILURE);
	}

	printf("\nRequesting statistics for %s\n", FILENAME_HASH_SML);
	printf("------------------------------------\n", FILENAME_HASH_SML);
	if ( HashStatistics(FILENAME_HASH_SML) < 0 ){
		fprintf(stderr, "Error in getting hash statistics\n");
		exit(EXIT_FAILURE);
	}

	printf("\nRequesting statistics for %s\n", FILENAME_HASH_SUR);
	printf("------------------------------------\n", FILENAME_HASH_SUR);
	if ( HashStatistics(FILENAME_HASH_SUR) < 0 ){
		fprintf(stderr, "Error in getting hash statistics\n");
		exit(EXIT_FAILURE);
	}

	printf("\nRequesting statistics for %s\n", FILENAME_HASH_CIT);
	printf("------------------------------------\n", FILENAME_HASH_CIT);
	if ( HashStatistics(FILENAME_HASH_CIT) < 0 ){
		fprintf(stderr, "Error in getting hash statistics\n");
		exit(EXIT_FAILURE);
	}

	return 0;
}
