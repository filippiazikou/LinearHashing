#ifndef RECORD

#define RECORD
#define ID "id"
#define NAME "name"
#define SURNAME "surname"
#define CITY "city"
#define NAME_SIZE 15
#define SURNAME_SIZE 20
#define CITY_SIZE 10

typedef struct {
	int id;
	char name[NAME_SIZE];
	char surname[SURNAME_SIZE];
	char city[CITY_SIZE];
} Record;

#endif

