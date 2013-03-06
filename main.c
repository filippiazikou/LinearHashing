#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "LH.h"
#include "BF.h"

int main(int argc,  char** argv)
{
        char* keyfield;
        char* value;
        Record r;
        int fd, i;
        LH_info *info;
	int k = 5;

        if( argc == 4 )
        {
                if( !strcmp(argv[1], "city") || !strcmp(argv[1], "name") || !strcmp(argv[1], "surname") || !strcmp(argv[1], "id") ) {
                        keyfield = malloc( strlen(argv[1]) + 1 );
                        strcpy(keyfield, argv[1]);
                        value = malloc( strlen(argv[2]) + 1 );
                        strcpy(value, argv[2]);
			k = atoi(argv[3]);
                }else
                        goto Default;
        }else
        {
                goto Default;
        }

        goto L;

Default:
        keyfield = malloc(5);
        strcpy(keyfield, "city");
        value = malloc(8);
        strcpy(value, "Avlida");


L:
	BF_Init();
        LH_CreateIndex( "index","surname", 'c', 20, 4, 0.50 );
        info = LH_OpenIndex("index");

        for(i=0; i<k; i++)
        {
                scanf("%d", &r.id);
                scanf("%s", r.name);
                scanf("%s", r.surname);
                scanf("%s", r.city);
//              printf("To insert: %d, %s, %s, %s\n", r.id, r.name, r.surname, r.city);
                LH_InsertEntry(info, r);
        }

	printf("\n\n\n~~~~~~~~~~LINEAR:~~~~~~~~~~~~~\n");
        LH_FindAllEntries( *info, (void *) value);

        LH_CloseIndex( info );

        free(value);
        free(keyfield);
        return 0;
}


