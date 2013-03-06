/* counter - overflow - flag */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "BF.h"
#include "LH.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
int LH_CreateIndex(char *fileName, char* attrName, char attrType,  int attrLength, int buckets, float loadThrs)
{
	LH_info info;
	char *overflowFile;
	char *block;
	int error, i;
	
	/* Create the file */
	error = BF_CreateFile( fileName );
	if( error < 0)
	{
		BF_PrintError( "LH_CreateIndex1..." );
		return FALSE;
	}
	
	/* initialize struct HT_info */
	info.fileDesc = BF_OpenFile( fileName );
	if( info.fileDesc < 0)
	{
		BF_PrintError( "LH_CreateIndex2..." );
		return FALSE;
	}

	info.capacity = ( BLOCK_SIZE - (3*sizeof(int)) ) / sizeof(Record);
	strcpy(info.attrName, attrName);
	info.attrType = attrType;
	info.attrLength = attrLength;
	info.buckets = buckets; 
	info.loadThrs = loadThrs;
	info.total_recs = 0;
	info.pointer = 1;
	info.overflow_pointer = -1;
	info.array = NULL;
	info.array_size = 0;
	info.first_empty = -1;
	

	/* Create the header */
	error = BF_AllocateBlock( info.fileDesc );	/* allocating HP header */
	if( error < 0)
	{
		BF_PrintError( "LH_CreateIndex3..." );
		return FALSE;
	}

	error = BF_ReadBlock(info.fileDesc, BF_GetBlockCounter(info.fileDesc) - 1, (void *)&block);
	if(error < 0)
	{
		BF_PrintError( "LH_CreateIndex4..." );
		return FALSE;
	}
	
	memcpy( (void *)block, (void *)&info, sizeof(LH_info));
	
	
	/* writing updated header back to file */
	error = BF_WriteBlock( info.fileDesc, BF_GetBlockCounter(info.fileDesc) - 1);
	if( error < 0)
	{
		BF_PrintError( "LH_CreateIndex5..." );
		return FALSE;
	}
	

	
	/*Allocate space for buckets*/
	for (i = 0 ; i < buckets; i++) {
		error = BF_AllocateBlock( info.fileDesc );	/* allocating HP header */
		if( error < 0)
		{
			BF_PrintError( "LH_CreateIndex6..." );
			return FALSE;
		}
	
	
		error = BF_ReadBlock(info.fileDesc, BF_GetBlockCounter(info.fileDesc) - 1, (void *)&block);
		if(error < 0)
		{
			BF_PrintError( "LH_CreateIndex4..." );
			return FALSE;
		}
		
		*(int *)block = 0;
		*(int *)(block + sizeof(int)) = -1;
		
		/* writing updated header back to file */
		error = BF_WriteBlock( info.fileDesc, BF_GetBlockCounter(info.fileDesc) - 1);
		if( error < 0)
		{
			BF_PrintError( "LH_CreateIndex5..." );
			return FALSE;
		}
	}
	
	error = BF_CloseFile(info.fileDesc);
	if( error < 0 )
	{
		BF_PrintError( "LH_CreateIndex7..." );
		return FALSE;
	}

	
	overflowFile = malloc(strlen(fileName) + 5);
	sprintf(overflowFile, "%s.tmp", fileName);
	
	error = BF_CreateFile( overflowFile );
	if( error < 0)
	{
		BF_PrintError( "LH_CreateIndex8..." );
		return FALSE;
	}
	
	free( overflowFile );
	return OK;
}


/////////////////////////////////////////////////////////////////////////////
LH_info* LH_OpenIndex(char* fileName)
{
	LH_info *info;
	char *block, *tmp;
	int error, fileDesc, i;
	
	info = malloc(sizeof(LH_info));
	if( info == NULL)
	{
		printf("LH_OpenIndex1... Malloc error..\n");
		return NULL;
	}
	
	
	fileDesc = BF_OpenFile( fileName );
	if( fileDesc < 0)
	{
		BF_PrintError( "LH_OpenIndex2..." );
		return NULL;
	}
	
	error = BF_ReadBlock(fileDesc, 0, (void *)&block);
	if(error < 0)
	{
		BF_PrintError( "LH_OpenIndex3..." );
		return NULL;
	}
	
	
	memcpy((void *) info, (void *) block, sizeof(LH_info));

	
	tmp = malloc( strlen(fileName) + 5);
	sprintf(tmp, "%s.tmp", fileName);
	
	info->overflow_fd = BF_OpenFile( tmp );
	if( info->overflow_fd < 0)
	{
		BF_PrintError( "LH_OpenIndex4..." );
		return NULL;
	}
	
	info->fileDesc = fileDesc;
	if (info->first_empty != -1)
		info->overflow_pointer = 0;
	else
		info->overflow_pointer = -1;
	
	if(info->first_empty != -1)
	{	
		for (i = info->first_empty ; i < BF_GetBlockCounter(info->overflow_fd) -1; i++ ) 
		{
			error = BF_ReadBlock(info->overflow_fd, i, (void *)&block);
			if(error < 0)
			{
				BF_PrintError( "LH_OpenIndex5..." );
				return NULL;
			} 
				
			if( *(int *)(block + 2*sizeof(int)) == 0)
			{
				info->array_size += 1;
				info->array = realloc(info->array, info->array_size*sizeof(int));
				if (info->array == NULL) {
					printf("Error in realloc 1\n");
					return NULL;
				}
				info->array[info->array_size - 1] = i;
			}
	
			error = BF_WriteBlock( info->overflow_fd, i);
			if(error < 0)
			{
				BF_PrintError( "LH_OpenIndex6..." );
				return NULL;
			}
		}
	}
	
	free(tmp);
	return info;
}

///////////////////////////////////////////////////////////////////////////
int LH_CloseIndex(LH_info *header_info)
{
	int error;
	int i;
	int min;
	char *block;
	
	if  (header_info->overflow_pointer != -1) {
		min = header_info->array[header_info->overflow_pointer];
		/*Kratame sto header to mikrotero block tou array*/
		for ( i = header_info->overflow_pointer ; i < header_info->array_size ; i++) {
			if (header_info->array[i] <  min)
				min = header_info->array[i];
		}
		
		header_info->first_empty = min;
	}
	else {
		header_info->first_empty = -1;
	}
	if (header_info->array != NULL) {
		free(header_info->array);
		header_info->array = NULL;
	}
	header_info->overflow_pointer = -1;
	header_info->array_size = 0;
	

	error = BF_ReadBlock(header_info->fileDesc, 0, (void *)&block);
	if(error < 0)
	{
		BF_PrintError( "LH_CloseIndex1..." );
		return FALSE;
	}
		
	memcpy(block, header_info, sizeof(LH_info));
		
	/* writing updated header back to file */
	error = BF_WriteBlock( header_info->fileDesc, 0);
	if( error < 0)
	{
		BF_PrintError( "LH_CloseIndex2..." );
		return FALSE;
	}
	
	
	error = BF_CloseFile( header_info->fileDesc );
	if( error < 0)
	{
		BF_PrintError( "LH_CloseIndex1..." );
		return FALSE;
	}
	
	error = BF_CloseFile( header_info->overflow_fd );
	if( error < 0)
	{
		BF_PrintError( "LH_CloseIndex2..." );
		return FALSE;
	}
	
	free( header_info );
	return OK;
}


//////////////////////////////////////////////////////////////////////////
int LH_InsertEntry(LH_info *header_info, Record record)
{
	char *value, *block;
	int hashValue, write_bucket, write_fd, overflow_bucket;
	int counter = 0, block_no = -1, i, overflow_block, error;
	
	if( strcmp(header_info->attrName, "id") == 0  ){
		value = (char *)malloc( sizeof(int) );
		memcpy(value, &record.id, sizeof(int) );
	}
	else if ( strcmp(header_info->attrName, "name") == 0  )
	{
		value = (char *)malloc( strlen(record.name) +  1);
		memcpy(value, &record.name, strlen(record.name) + 1);
	}
	else if ( strcmp(header_info->attrName, "surname") == 0  )
	{
		value = (char *)malloc( strlen(record.surname) +  1);
		memcpy(value, &record.surname, strlen(record.surname) + 1);
	}
	else if ( strcmp(header_info->attrName, "city") == 0  )
	{
		value = (char *)malloc( strlen(record.city) +  1);
		memcpy(value, &record.city, strlen(record.city) + 1);
	}
	
	hashValue = hash_function( (void *)value, header_info->attrType, header_info->buckets );
	if (hashValue < header_info->pointer) {
		hashValue = hash_function( (void *)value, header_info->attrType, 2*header_info->buckets );
	}
	
	
	/*Read the bucket*/
	error = BF_ReadBlock(header_info->fileDesc, hashValue, (void *)&block);
	if(error < 0)
	{
		BF_PrintError( "LH_InsertEntry1..." );
		return FALSE;
	}
	
	
	/*Vriskoume to block sto arxiko i arxeio i sto arxeio twn overflow pou prepei na prostethei i eggrafi*/
	write_bucket = hashValue ;	/* grafoume sto LH arxeio ektos an exei overflow */
	write_fd = header_info->fileDesc;
	
	for(;;)
	{
		overflow_bucket = *(int *)(block+sizeof(int));
		if( overflow_bucket >= 0 )
		{		
			error = BF_WriteBlock( write_fd, write_bucket);
			if( error < 0)
			{
				BF_PrintError( "LH_InsertEntry2..." );
				return FALSE;
			}
			
			write_bucket = overflow_bucket;
			write_fd = header_info->overflow_fd;
			error = BF_ReadBlock(write_fd, write_bucket, (void *)&block);
			if(error < 0)
			{
				BF_PrintError( "LH_InsertEntry3..." );
				return FALSE;
			}
			
		}else
			break;
	}
	
	counter = *(int *) block;
	
	/*An xwraei sto block*/
	if(counter < header_info->capacity)
	{
		memcpy( block+3*sizeof(int)+counter*sizeof(record), &record, sizeof(record));
		header_info->total_recs++;
		counter++;
		memcpy(block, &counter, sizeof(int));
		
		error = BF_WriteBlock( write_fd, write_bucket);
		if( error < 0)
		{
			BF_PrintError( "LH_InsertEntry4..." );
			return FALSE;
		}
	}else	/* find unused overflow bucket */
	{
		if(header_info->overflow_pointer >= 0)
		{
			block_no = header_info->array[header_info->overflow_pointer];

			if(header_info->overflow_pointer != header_info->array_size - 1 )
				header_info->overflow_pointer += 1;
			else
				header_info->overflow_pointer = -1;
		}else
		{
			error = BF_AllocateBlock( header_info->overflow_fd );
			if( error < 0)
			{
				BF_PrintError( "LH_InsertEntry5..." );
				return FALSE;
			}

			block_no = BF_GetBlockCounter( header_info->overflow_fd) - 1;
		}
		
		*(int *)(block + sizeof(int)) = block_no;
		
		error = BF_WriteBlock( write_fd, write_bucket );
		if( error < 0)
		{
			BF_PrintError( "LH_InsertEntry6..." );
			return FALSE;
		}
		
		error = BF_ReadBlock(header_info->overflow_fd, block_no, (void *)&block);
		if(error < 0)
		{
			BF_PrintError( "LH_InsertEntry7..." );
			return ;
		}
		
		
		*(int *)block = 1;
		*(int *)(block+sizeof(int)) = -1;
		*(int *)(block+2*sizeof(int)) = 1;

		memcpy( block+3*sizeof(int), &record, sizeof(record));
		header_info->total_recs++;

		error = BF_WriteBlock( header_info->overflow_fd, block_no);
		if( error < 0)
		{
			BF_PrintError( "LH_InsertEntry8..." );
			return FALSE;
		}
	
	}
	
	/* Elegxos gia diaspasi */
	if (header_info->total_recs > header_info->loadThrs * ( (BF_GetBlockCounter(header_info->fileDesc) - 1) * header_info->capacity) ){
		// printf("oops spliting\n");
		split(header_info);
	}
		
	return OK;
}


////////////////////////////////////////////////////////////////////////////////////////////
int split(LH_info * header_info) 
{
	char *block, *split_block, *init_block, temp_block[BLOCK_SIZE], *new_block;
	int new_bucket, temp_overflow_bucket, temp_counter, init_bucket, split_bucket;
	int error, j;
	Record record;
	
	
	error = BF_AllocateBlock( header_info->fileDesc );	/* allocating new block */
	if( error < 0)
	{
		BF_PrintError( "split1..." );
		return FALSE;
	}
	
	new_bucket = BF_GetBlockCounter(header_info->fileDesc) - 1;

	error = BF_ReadBlock(header_info->fileDesc, new_bucket, (void *)&new_block);
	if(error < 0)
	{
		BF_PrintError( "split2..." );
		return FALSE;
	} 
	
	*(int *) new_block= 0;
	*(int *) (new_block + sizeof(int)) = -1;
	
	
	error = BF_WriteBlock( header_info->fileDesc, new_bucket);
	if ( error < 0)
	{
		BF_PrintError( "split3..." );
		return FALSE;
	}
	
	split_bucket = header_info->pointer;
	
	/*Allagi tou pointer*/
	if( header_info->buckets == header_info->pointer) {
		header_info->pointer = 1;
		header_info->buckets *= 2;
	}
	else {
		header_info->pointer += 1;
	}
	
	error = BF_ReadBlock(header_info->fileDesc, split_bucket, (void *)&split_block);
	if(error < 0)
	{
		BF_PrintError( "split6..." );
		return FALSE;
	}
	
	
	temp_overflow_bucket = *(int *)(split_block + sizeof(int));
	temp_counter = *(int *)split_block;
	memcpy(temp_block, split_block, BLOCK_SIZE);
	*(int *) (split_block + sizeof(int)) = -1;
	*(int *) split_block = 0;

	
	error = BF_WriteBlock( header_info->fileDesc, split_bucket);
	if ( error < 0)
	{
		BF_PrintError( "split7..." );
		return FALSE;
	}
	
	for(j=0; j<temp_counter; j++)
	{
		memcpy(&record, temp_block+ 3*sizeof(int) + j*sizeof(record), sizeof(record));
		header_info->total_recs -= 1;
		LH_InsertEntry(header_info, record);
	}

	for(;;) 
	{
		if (temp_overflow_bucket >= 0) 
		{
			error = BF_ReadBlock(header_info->overflow_fd, temp_overflow_bucket, (void *)&split_block);
			if(error < 0)
			{
				BF_PrintError( "split8..." );
				return FALSE;
			}
			
			/* Insert oles tis eggrafes pou exoun ta overflow buckets */
			memcpy(temp_block, split_block, BLOCK_SIZE);
			*(int *) (split_block + sizeof(int)) = -1;
			*(int *) split_block = 0;

			/* Enimerwsi oti to overflow block den xrisimopoieitai */
			*(int *)(split_block + 2*sizeof(int)) = 0;
			header_info->array = realloc(header_info->array, (header_info->array_size+1)*sizeof(int) );
			if (header_info->array == NULL) {
				printf("Error in realloc 2\n");
				return FALSE;
			}
			header_info->array[header_info->array_size] = temp_overflow_bucket;
			header_info->array_size += 1;
			
			if( header_info->overflow_pointer == -1)
				/* An den ipirxei allo eleythero block to vazoume na deixnei sto telos */
				/* afou kapoia pou mporei na itan endiamesa simainei oti xrisimopoiithikan */
				header_info->overflow_pointer = header_info->array_size - 1;	
			
			error = BF_WriteBlock( header_info->overflow_fd, temp_overflow_bucket);
			if ( error < 0)
			{
				BF_PrintError( "split9..." );
				return FALSE;
			}
			
			temp_overflow_bucket = *(int *) (temp_block + sizeof(int));
			temp_counter =  *(int *) temp_block;
			
			for (j = 0 ; j < temp_counter ; j++ ) {
				memcpy(&record, temp_block+ 3*sizeof(int) + j*sizeof(record), sizeof(record));
				header_info->total_recs -= 1;
				LH_InsertEntry(header_info, record);
			}
		}
		else {
			break;
		}
	
	}
	
	return OK;
}
	
/////////////////////////////////////////////////////////////////////////
int LH_FindAllEntries(LH_info header_info, void* value)
{
	char *block;
	int hashValue, blocks_read = 0, error, read_fd, read_bucket, overflow_bucket;
	int x=0, counter, i;
	Record rec;
	
	hashValue = hash_function( (void *)value, header_info.attrType, header_info.buckets );
	
	if (hashValue < header_info.pointer) {
		hashValue = hash_function( (void *)value, header_info.attrType, 2*header_info.buckets );
	}
	
	
	read_fd = header_info.fileDesc;
	read_bucket = hashValue;
	
	
	for(;;)
	{
		error = BF_ReadBlock(read_fd, read_bucket, (void* )&block);
		if(error < 0)
		{
			BF_PrintError( "HT_GetAllEntries2..." );
			exit(1);
		}

		blocks_read += 1;

		overflow_bucket = *(int *)(block+sizeof(int));
		counter = *(int *)(block);
		
		for (i = 0 ; i < counter ; i++) 
		{
			memcpy((void*) &rec, (void *) (block + 3*sizeof(int) + i*sizeof(Record)) , sizeof(Record));

			if (strcmp(header_info.attrName, "id") == 0) {
				if (rec.id == *(int *)value ) {
					x++;
					printf("%d) %s  %s  %s\n", rec.id, rec.name,  rec.surname,  rec.city);
				}
			}
			else if (strcmp(header_info.attrName, "name") == 0) {
				if (memcmp(rec.name, (char*) value, strlen(rec.name)) == 0) {
					x++;
					printf("%d) %s  %s  %s\n", rec.id, rec.name,  rec.surname,  rec.city);				

				}
			}
			else if (strcmp(header_info.attrName, "surname") == 0) {
				if (memcmp(rec.surname, (char*) value, strlen(rec.surname)) == 0) {
					x++;
					printf("%d) %s  %s  %s\n", rec.id, rec.name,  rec.surname,  rec.city);
				}
			}
			else if (strcmp(header_info.attrName, "city") == 0) {
				if (memcmp(rec.city, (char*) value, strlen(rec.city)) == 0) {
					x++;
					printf("%d) %s  %s  %s\n", rec.id, rec.name,  rec.surname,  rec.city);

				}
			}
		}
		
		
		if(overflow_bucket >= 0)
		{
			read_fd = header_info.overflow_fd;
			read_bucket = overflow_bucket;
		}else
			break;
	}
	
	printf("Diavastikan %d block kai vrethikan %d apotelesmata\n", blocks_read, x);
	return x;
}

///////////////////////////////////////////////////////////////////////
int HashStatistics(char* filename)
{
	int i, j, error, fileDesc, overflow_fd, temp_block_counter = 0, temp_rec_counter = 0, overflow_id, read_fd, prev;
	int sum_recs = 0, sum_blocks = 0;
	char *block, *overflow_block, *header, *overflowFile;
	int total_blocks;
	int min, max;
	float mesos_oros_eggrafwn, mesos_oros_block;
	LH_info info;

	
	/* opening hash file */
	fileDesc = BF_OpenFile( filename );
	if( fileDesc < 0)
	{
		BF_PrintError( "HashStatistics1..." );
		return FALSE;
	}
	
	total_blocks = BF_GetBlockCounter( fileDesc ) - 1;
	
	/* opening overflow file */
	overflowFile = malloc(strlen(filename) + 5);
	sprintf(overflowFile, "%s.tmp", filename);
	overflow_fd = BF_OpenFile( overflowFile );
	if( overflow_fd < 0)
	{
		BF_PrintError( "HashStatistics2..." );
		return FALSE;
	}


	max = 0;	/* initialize counter for max blocks*/
	min = BF_GetBlockCounter( overflow_fd ) - 1;	/* same for min blocks */

/*
	error = BF_ReadBlock(fileDesc, 0, (void *)&head);
	if(error < 0)
	{
		BF_PrintError( "HashStatistics11..." );
		return FALSE;
	}

	memcpy(&info, head, sizeof(LH_info));

	BF_WriteBlock( fileDesc, 0);
	if( error < 0)
	{
		BF_PrintError( "HashStatistics10..." );
		return FALSE;
	}
*/

	/* for every block of hash file */
	for(i=1; i<=total_blocks; i++)
	{
		temp_block_counter = 0;
		temp_rec_counter = 0;

		error = BF_ReadBlock(fileDesc, i, (void *)&block);
		if(error < 0)
		{
			BF_PrintError( "HashStatistics2..." );
			return FALSE;
		}

		sum_blocks += 1;	/* synolika block gia to meso arithmo block kathe bucket */
		temp_block_counter += 1;	/* block gia to current bucket */
		temp_rec_counter += *(int *)block;	/* recs gia to current bucket */

		BF_WriteBlock( fileDesc, i);
		if( error < 0)
		{
			BF_PrintError( "HashStatistics3..." );
			return FALSE;
		}

		overflow_id = *(int *)(block + sizeof(int));
		for(;;)
		{
			if( overflow_id >=0 )
			{
				error = BF_ReadBlock(overflow_fd, overflow_id, (void *)&overflow_block);
				if( error < 0)
				{
					BF_PrintError( "HashStatistics4..." );
					return FALSE;
				}

				temp_block_counter += 1;
				temp_rec_counter += *(int *)overflow_block;
				prev = *(int *)(overflow_block + sizeof(int));

				error = BF_WriteBlock( overflow_fd, overflow_id);
				if( error < 0)
				{
					BF_PrintError( "HashStatistics5..." );
					return FALSE;
				}
	
				overflow_id = prev;
			}else
				break;
		}

		sum_blocks += temp_block_counter;		
		sum_recs += temp_rec_counter;

		if( temp_rec_counter < min )
			min = temp_rec_counter;

		if( temp_rec_counter > max )
			max = temp_rec_counter;
	}

	printf("Synoliko plithos block = %d\n", total_blocks);

	mesos_oros_eggrafwn = (float)sum_recs / total_blocks;
	printf("Min #eggrafwn = %d \nMax #eggrafwn = %d \nMesos oros eggrafwn = %f\n", min, max, mesos_oros_eggrafwn);


	mesos_oros_block = (float)sum_blocks / total_blocks;
	printf("Mesos oros block gia kathe bucket = %f\n", mesos_oros_block);


	error = BF_CloseFile(fileDesc);
	if( error < 0 )
	{
		BF_PrintError( "HashStatistics6..." );
		return FALSE;
	}

	error = BF_CloseFile(overflow_fd);
	if( error < 0 )
	{
		BF_PrintError( "HashStatistics7..." );
		return FALSE;
	}


   return OK;
}

///////////////////////////////////////////////////////////////////////
int hash_function(void* value, char attrtype, int buckets)
{
	int length, new_int;
	char *new_string;
	float new_float;
	int i, k=0;
	int result = 0;
	if( attrtype == 'c' )
	{	
		length = strlen( value );
		new_string = malloc( length + 1 );
		strcpy(new_string, value);

		for(i=0; i<length; i++) {
			k += new_string[i];	
		}

		result = k % buckets;
	}else
	if( attrtype == 'i' )
	{
		new_int = *(int *)value;
		new_int += ( new_int * 123) / 7;

		result = new_int % buckets;
	}else
	if( attrtype == 'f' )
	{
		memcpy(&new_float, value, sizeof(float));
		new_int += (int)(new_float*30) + (int)fmod( new_float * 100 , 13);		

		result = (int)new_float % buckets;
	}

  return result+1;
}
