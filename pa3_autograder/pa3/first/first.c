#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

struct line** cache;
long unsigned int cachesize;
long unsigned int blocksize;
char* cachepolicy;
char* associativity;
long unsigned int numSets;
long unsigned int numLines;
long unsigned int association;
int numSetBits;
int numTagBits;
int numOffsetBits;
long unsigned int setMask;
long unsigned int tagMask;
long unsigned int offsetMask;
int reads = 0;
int writes = 0;
int hits = 0;
int misses = 0;
int readcount = 0;
int writecount = 0;
long unsigned int addressSetIndex;
long unsigned int addressTag;
long unsigned int addressOffset;


struct line{
	int valid;
	long unsigned int tag;
};

struct line** initialize(char**);
int search(struct line**, long unsigned int);
void read(struct line**, long unsigned int);
void write(struct line**, long unsigned int);
void insert(struct line***, long unsigned int);
void bringToFront(struct line***, long unsigned int);
long unsigned int twoPower(int);
int powerTwoCheck(long unsigned int);

void bringToFront(struct line*** cache, long unsigned int address){
	addressSetIndex = (address & setMask)>>numOffsetBits;
	addressTag = ((address & tagMask)>>numOffsetBits)>>numSetBits;
	int i, j;
	struct line temp;

	for(i = 0; i < association; i++){
		if((*cache)[addressSetIndex][i].valid == 1){
			if((*cache)[addressSetIndex][i].tag == addressTag){
				temp.valid = 1;
				temp.tag = addressTag;
				for(j = i; j > 0; j--){
					(*cache)[addressSetIndex][j].valid = (*cache)[addressSetIndex][j - 1].valid;
					(*cache)[addressSetIndex][j].tag = (*cache)[addressSetIndex][j - 1].tag;
				}
				(*cache)[addressSetIndex][0].valid = temp.valid;
				(*cache)[addressSetIndex][0].tag = temp.tag;
				break;
			}
		}
	}

return;
}

int powerTwoCheck(long unsigned int n) { 
	if (n == 0){
		return 0;
	} 
		while (n != 1){ 
			if (n%2 != 0){
				return 0;
			}
		n = n/2; 
		} 
	return 1;
} 

long unsigned int twoPower(int size){
	long unsigned int result = 1;
	int i;
	for(i = 0; i < size; i++){
		result *= 2;
	}
	return result;
}

void insert(struct line*** cache, long unsigned int address){
	addressSetIndex = (address & setMask)>>numOffsetBits;
	addressTag = ((address & tagMask)>>numOffsetBits)>>numSetBits;
	int i;
	if(!strcmp(associativity, "direct")){
		(*cache)[addressSetIndex][0].valid = 1;
		(*cache)[addressSetIndex][0].tag = addressTag;
		return;
	}else{
		for(i = (association - 1); i > 0; i--){
			(*cache)[addressSetIndex][i].valid = (*cache)[addressSetIndex][i - 1].valid;
			(*cache)[addressSetIndex][i].tag = (*cache)[addressSetIndex][i - 1].tag;
		}
		(*cache)[addressSetIndex][0].valid = 1;
		(*cache)[addressSetIndex][0].tag = addressTag;
	}


	return;
}

void write(struct line** cache, long unsigned int address){
	if(search(cache, address)){
/***New***/
		if(!strcmp(cachepolicy, "lru")){
			bringToFront(&cache, address);
		}
/********/		
		hits++;
	}else{
		misses++;
		reads++;
		insert(&cache, address);
	}
	writes++;
return;	
}

void read(struct line** cache, long unsigned int address){
	if(search(cache, address)){
/***New***/
		if(!strcmp(cachepolicy, "lru")){
			bringToFront(&cache, address);
		}
/********/		
		hits++;
	}else{
		misses++;
		reads++;	
		insert(&cache, address);
	}
return;
}

int search(struct line** cache, long unsigned int address){
	addressSetIndex = (address & setMask)>>numOffsetBits;
	addressTag = ((address & tagMask)>>numOffsetBits)>>numSetBits;	
	int i;
	for(i = 0; i < association; i++){
		if(cache[addressSetIndex][i].valid == 1){
			if(cache[addressSetIndex][i].tag == addressTag){
					return 1;
			}
		}
	}
return 0;
}

struct line** initialize(char** argv){
	if(!strcmp(associativity, "direct")){
		numSets = cachesize/blocksize;
		numLines = numSets;
		numOffsetBits = log(blocksize)/log(2);
		numSetBits = log(numSets)/log(2);
		numTagBits = 48 - numOffsetBits - numSetBits;
		association = 1;
		offsetMask = ((long unsigned int)1<<numOffsetBits) - 1;
		setMask = ((long unsigned int)1<<(numSetBits)) - 1;
		setMask = setMask<<numOffsetBits;
		tagMask = ((long unsigned int)1<<numTagBits) - 1;
		tagMask = (tagMask<<(numOffsetBits))<<numSetBits;
	}else if(!strcmp(associativity, "assoc")){
		numSets = 1;
		numLines = cachesize/blocksize;
		numOffsetBits = log(blocksize)/log(2);
		numSetBits = 0;
		numTagBits = 48 - numOffsetBits - numSetBits;
		association = numLines;
		offsetMask = ((long unsigned int)1<<numOffsetBits) - 1;
		setMask = 0;
		tagMask = ((long unsigned int)1<<numTagBits) - 1;
		tagMask = (tagMask<<(numOffsetBits))<<numSetBits;
	}else{
		sscanf(associativity, " assoc: %lu ", &association);
		numLines = cachesize/blocksize;
		numSets = numLines/association;
		numOffsetBits = log(blocksize)/log(2);
		numSetBits = log(numSets)/log(2);
		numTagBits = 48 - numOffsetBits - numSetBits;
		offsetMask = ((long unsigned int)1<<numOffsetBits) - 1;
		setMask = ((long unsigned int)1<<(numSetBits)) - 1;
		setMask = setMask<<numOffsetBits;
		tagMask = ((long unsigned int)1<<numTagBits) - 1;
		tagMask = (tagMask<<(numOffsetBits))<<numSetBits;
	}
	struct line** cache = (struct line**) malloc(sizeof(struct line*) * numSets);
	int i, j;
	for(i = 0; i < numSets; i++){
		cache[i] = (struct line*) malloc(sizeof(struct line) * association);
	}
	for(i = 0; i < numSets; i++){
		for(j = 0; j < association; j++){
			cache[i][j].valid = 0;
		}
	}
return cache;
}

int main(int argc, char** argv){
	cachesize = atoi(argv[1]);
	blocksize = atoi(argv[2]);
	cachepolicy = argv[3];
	associativity = argv[4];
	if(!powerTwoCheck(cachesize)|| !powerTwoCheck(blocksize)){
		printf("error");		
		return 0;
	}
	cache = initialize(argv);
	if(!powerTwoCheck(association)){
		printf("error");
		return 0;
	}
	if(strcmp(cachepolicy, "fifo") !=0 && strcmp(cachepolicy, "lru") != 0){
		printf("error");		
		return 0;
	}
	FILE* fp = fopen(argv[5], "r");
	if(fp == NULL){
		printf("error");
		return 0;
	}
	long unsigned int address;
	char instruct;
	while(fscanf(fp, " %c %lx ", &instruct, &address) == 2){
		if(instruct == '#'){
			break;
		}else if(instruct == 'R'){
			read(cache, address);
		}else{
			write(cache, address);
		}
	}
	printf("Memory reads: %d\n", reads);
	printf("Memory writes: %d\n", writes);
	printf("Cache hits: %d\n", hits);
	printf("Cache misses: %d\n", misses);

return 0;
}
