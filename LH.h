#ifndef LH_H_
#define LH_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "record.h"
#include "BF.h" 

/* bitsNum(mono sta arxika buckets), counter, overflow sta regular*/
/* flag, counter, overflow sta overflow */

#define OK 0
#define FALSE -1

typedef struct {
	int fileDesc;
	char attrName[8];
	char attrType;
	int attrLength;
	int buckets;
	float loadThrs;
	int capacity;
	int pointer;
	int overflow_fd;
	int total_recs;			//poses eggrafes einai sta buckets, mazi me ta overflows
	int overflow_pointer;	//Gia to overflow
	int first_empty;	//Gia to overflow arxeio
	int array_size;		//Gia to overflow arxeio
	int *array;
}LH_info;



int LH_CreateIndex(char *fileName, char* attrName, char attrType,  int attrLength, int buckets, float loadThrs);
LH_info* LH_OpenIndex(char* fileName);
int LH_CloseIndex(LH_info *header_info);
int LH_InsertEntry(LH_info *header_info, Record record);
int LH_FindAllEntries(LH_info header_info, void* value);
int HashStatistics(char* filename);


int hash_function(void* value, char attrtype, int buckets);

int split(LH_info *header_info);
int is_bit_set(char*, int);
void print(char*, int);
int f(char*, int, int);


#endif

