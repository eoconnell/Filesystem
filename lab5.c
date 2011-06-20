#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct {
	unsigned short int file_id;
	unsigned short int parent_id;
	char type; // unsigned?
	char name[12]; // restrict to number of bytes
	unsigned short int block;
	unsigned int size;
	time_t timestamp;
} record;

#include "linkedlist.h"

typedef struct {
	unsigned short int block_id;
	unsigned short int next_id;
	char status;
} block;

#define BLOCK_SIZE 512
#define TOTAL_BLOCK_RECORDS (BLOCK_SIZE-sizeof(block))/sizeof(record)

void resetDisk(FILE* fp);
void formatdisk(FILE* fp);

block getFreeBlock(FILE* fp);
int allocateFreeBlock(FILE* fp, char status);

void printFree(FILE* fp);
void printBlocks(FILE* fp);


int createDirectory(char *name, FILE* fp);
int createFile(char *name, FILE* fp);
int createRecord(record* rec, unsigned short int fid, unsigned short int pid, char type, char* name, unsigned short int block);
void readDirectory(FILE* fp, int block_id);
void printRecords();
void writeRecords(FILE* fp, int block_id);
int removeFile(char *name, FILE* fp);

void deallocateBlock(FILE* fp, int block_id);

typedef struct {
	int blocks[33];
	int size;		// total size of file
	int count;	// how many bytes in the file we are writting/reading
	record *rec;
} OCFILE;



// user program
void readCmdLn(char *a);
int parseCmdLn(char *cmd, char **args, char *delim);

int file_counter = 0;	// NOTE: should probably keep this counter on disk so we do not loose track ----------------------
int cur_dir_block = 1;	// block_id of the current directory
char sys_path[80];

void formatdisk(FILE* fp){
	// go through disk and allocate every
	int location = 0;
	int counter = 0;
	file_counter = 0;
	printf("Formatting disk....\n");
	resetDisk(fp);
	while(counter < 25){
		block free_list;
		free_list.block_id = counter;
		free_list.next_id = counter + 1;
		free_list.status = 'u';
		fseek(fp, location, SEEK_SET);
		printf("\tCreating... block %d\tlocation %d\tftell %d\n", counter, location, ftell(fp));
		fwrite((void*)&free_list, sizeof(block), 1, fp);
		counter++;
		location+=(BLOCK_SIZE);
	}
	// make the last block point to nothing
	block last;
	last.block_id = counter-1;
	last.next_id = 0;
	last.status = 'u';
	fseek(fp, (location-BLOCK_SIZE), SEEK_SET);
	fwrite((void*)&last, sizeof(block), 1, fp);

	// set up root directory
	printf("\tCreating Root directory...\n");
	record root;
	int block_id = allocateFreeBlock(fp, 'd');
	cur_dir_block = block_id;
	strcpy (sys_path,"~/");
	createRecord(&root, file_counter++, 0, 'd', "root", block_id);
	list_head = list_create(root);
	writeRecords(fp, block_id);
	
	printf("Done formatting.\n");
}

// returns the free list block
block getFreeBlock(FILE* fp){
	block temp;
	fseek(fp, 0, SEEK_SET);					// go to begining of free list
	fread((void*)&temp, sizeof(block), 1, fp);	// read first block
	printf("getFreeBlock:\tblock_id %u\tnext_id %u\tstatus %c\t\n", temp.block_id, temp.next_id, temp.status);
	return temp;
}

int allocateFreeBlock(FILE* fp, char status){
	//block free, temp;
	block freeHead, temp;
	int location;
	freeHead = getFreeBlock(fp);	// returns the free list block
	fseek(fp, freeHead.next_id*BLOCK_SIZE, SEEK_SET);	// go to the next free block
	fread((void*)&temp, sizeof(block), 1, fp);

	freeHead.next_id = temp.next_id;	// make free list point to new next free block
	temp.next_id = 0;
	temp.status = status;
	
	fseek(fp, temp.block_id*BLOCK_SIZE, SEEK_SET);	// go to the next free block
	
	fwrite((void*)&temp, sizeof(block), 1, fp);
	location = freeHead.block_id*BLOCK_SIZE;
	fseek(fp, location, SEEK_SET);
	fwrite((void*)&freeHead, sizeof(block), 1, fp);
	//printf("Allocating block %u as %c\n", temp.block_id, status);
	return temp.block_id;
}
/*
void readDirectory(FILE* fp, int block_id){

	NODE *list;
	record rec;
	// block 
	fseek(fp, ((block_id*BLOCK_SIZE)+sizeof(block)), SEEK_SET); // go to the point right after the block on the disk
	int num = 0;
	//printf("\nreading records from directory....%d\n", ftell(fp));
	while(1){
		//printf("%d ", ftell(fp));
		fread((void*)&rec, sizeof(record), 1, fp);	   // read first record
		//printf("'%c'\n", rec.type);
		if(rec.type != 'f' && rec.type != 'd'){		   // check to see if it is a valid record
			// no more records in directory
			//printf("done reading.\n\n");
			break;
		}
		if(num == 0){
			// on first record
			list_head = list_create(rec);			// add record to linked list
			list = list_head;
			
		}else {
			list = list_insert_after(list, rec);	// 
		}
	num++;
		// if num > TOTAL_BLOCK_RECORDS then seek to next block
	}
}*/
// with overflow
void readDirectory(FILE* fp, int block_id){

	NODE *list;
	record rec;
	// block 
	fseek(fp, (block_id*BLOCK_SIZE), SEEK_SET); // go to the point right after the block on the disk
	block blk;
	fread((void*)&blk, sizeof(block), 1, fp);
	int overflow_id = blk.next_id;
	int num = 0;
	//printf("\nreading records from directory....%d\n", ftell(fp));
	while(1){
		//printf("%d ", ftell(fp));
		fread((void*)&rec, sizeof(record), 1, fp);	   // read first record
		//printf("'%c'\n", rec.type);
		if(rec.type != 'f' && rec.type != 'd'){		   // check to see if it is a valid record
			// no more records in directory
			//printf("done reading.\n\n");
			break;
		}
		if(num == 0){
			// on first record
			list_head = list_create(rec);			// add record to linked list
			list = list_head;
			
		}else {
			list = list_insert_after(list, rec);	// 
		}

		if(num >= TOTAL_BLOCK_RECORDS){
			printf("Go to overflow block\n");
			if(overflow_id){
				fseek(fp, (overflow_id*BLOCK_SIZE), SEEK_SET);
				fread((void*)&blk, sizeof(block), 1, fp);
				overflow_id = blk.next_id;
				num = 0;
			}else {
				break;
			}	
		}
	num++;
		// if num > TOTAL_BLOCK_RECORDS then seek to next block
	}
}
/*
void writeRecords(FILE* fp, int block_id){
	NODE *list = list_head;
	record rec;
	int retval = fseek(fp, ((block_id*BLOCK_SIZE)+sizeof(block)), SEEK_SET);	// seek to beginning of records in block
	int count = 1;
	do{
		rec = list->rec;
		fwrite((void*)&list->rec, sizeof(record), 1, fp);
	
		list = list->next;
		count ++;
	}while(list != NULL);
} */

// with overflow
void writeRecords(FILE* fp, int block_id){
	NODE *list = list_head;
	fseek(fp, (block_id*BLOCK_SIZE), SEEK_SET);	// seek to beginning of records in block
	block blk;
	fread((void*)&blk, sizeof(block), 1, fp);
	int overflow_id = blk.next_id;
	int count = 1;
	do{
		if(count > TOTAL_BLOCK_RECORDS){
			printf("Go to overflow block count=%d\n", count);
			if(!overflow_id){
				overflow_id = allocateFreeBlock(fp, 'd');
				blk.next_id = overflow_id;
				fseek(fp, (blk.block_id*BLOCK_SIZE), SEEK_SET);
				fwrite((void*)&blk, sizeof(block), 1, fp);
			}
			fseek(fp, (overflow_id*BLOCK_SIZE), SEEK_SET);
			fread((void*)&blk, sizeof(block), 1, fp);
			overflow_id = blk.next_id;
			count = 1;
		}
		printf("Writing record with count %d\n", count);
		fwrite((void*)&list->rec, sizeof(record), 1, fp);
		list = list->next;
		count ++;
	}while(list != NULL);

	if(count <= TOTAL_BLOCK_RECORDS && overflow_id){
		printf("deallocate extra block\n");
		// there was an extra block allocated to this directory
		// but it no longer needs it so deallocate it
		deallocateBlock(fp, overflow_id);
	}
}

void printRecords(){
	NODE *list = list_head;
	printf("ID\tName\t\tSize\tType\tBlock ID\tParent ID\tDate Modified\n");
	do{
		printf("%d\t%s\t\t%d\t%c\t%d\t\t%d\t\t%s", list->rec.file_id, list->rec.name, list->rec.size, list->rec.type, list->rec.block, list->rec.parent_id, ctime(&list->rec.timestamp));
		list = list->next;
	}while(list != NULL);
}

void listDirectory(){
	
}

int createDirectory(char *name, FILE* fp){
	NODE* list = list_head;
	int block_id = allocateFreeBlock(fp, 'd'); // get free block for file
	record rec;
	createRecord(&rec, file_counter++, cur_dir_block, 'd', name, block_id);
	while(list->next != NULL) list = list->next;
	list_insert_after(list, rec);
	writeRecords(fp, cur_dir_block); //send in parent_id
	strcpy(rec.name, "");
	strcat(rec.name, name);
	// write record to dir block
	fseek(fp, ((block_id*BLOCK_SIZE)+sizeof(block)), SEEK_SET);
	fwrite((void*)&rec, sizeof(record), 1, fp);
}
int removeDirectory(char *name, FILE* fp);
int removeDirectory(char *name, FILE* fp){
	int file_count = 0;
	int i = 0;

	char *file_names[50];
	int parent_dir = cur_dir_block;
	int block_id;
	record dirRec;
	dirRec = list_find_name(list_head, name, 'd');
	block_id = dirRec.block;
	cur_dir_block = dirRec.block;
	readDirectory(fp, cur_dir_block);
	NODE* list = list_head;
	
	printRecords();

	// goes through the directories records, recursively calls removeDirectory 
	// while adding all the files to be removed at the end.
	while(list){
		if(list->rec.type == 'f'){
			file_names[file_count] = list->rec.name;
			file_count++;
		}else{
			if(strcmp(list->rec.name, name) != 0){
				removeDirectory(list->rec.name, fp);
			}
		}
		list = list->next;
	}
	file_names[file_count] = list->rec.name;
	for( i = 0; i < file_count; i++){
		removeFile(file_names[i], fp);
	}

	
	block free, new_free;
	
	printf("remove: %s\n", name);
	printRecords();
	list_remove_small(list_head, name);	// removes its own record
	dirRec.type = '@';
	list_insert_end(list_head, dirRec);
	writeRecords(fp, cur_dir_block);
	
	block freeHead = getFreeBlock(fp);
	int oldFirstFree = freeHead.next_id;
	freeHead.next_id = block_id;
	printf("old free %d\nblockid = %d\n", oldFirstFree, block_id);
	
	printf("FreeHead:\tblock_id %u\tnext_id %u\tstatus %c\t\n", freeHead.block_id, freeHead.next_id, freeHead.status);
	
	// go to freelist & write its info back
	fseek(fp, 0, SEEK_SET);
	fwrite(&freeHead, sizeof(block), 1, fp);

	block temp;
	// go to every block that the dir has allocated to it
	// and change them to free
	int last_block_id; //required to seek after reading... very annoying
	do {
		fseek(fp, ((block_id*BLOCK_SIZE)), SEEK_SET);	// seek to the file block
		fread((void*)&temp, sizeof(block), 1, fp);
		
		temp.status = 'u';
		last_block_id = block_id;
		block_id = temp.next_id;
		if(block_id == 0){
			temp.next_id = oldFirstFree;
			printf("end of dir\n");
		}
		
		fseek(fp, ((last_block_id*BLOCK_SIZE)), SEEK_SET);	// seek to the file block
		fwrite((void*)&temp, sizeof(block), 1, fp);
	}while(block_id != 0);


	// go up to parent dir and delete this dir record <!------------------------
	writeRecords( fp, cur_dir_block);
	cur_dir_block = parent_dir;

	readDirectory(fp, cur_dir_block);
	list_remove_small(list_head, name);
	dirRec.type = '@';
	list_insert_end(list_head, dirRec);

	writeRecords(fp, cur_dir_block);
	readDirectory(fp, cur_dir_block);
	return 1;
}


int createFile(char *name, FILE* fp){
	NODE* list = list_head;
	int block_id = allocateFreeBlock(fp, 'f'); // get free block for file
	record rec;
	createRecord(&rec, file_counter++, cur_dir_block, 'f', name, block_id);
	while(list->next != NULL) list = list->next;
	list_insert_after(list, rec);
	writeRecords(fp, cur_dir_block); //send in parent_id
}

int removeFile(char *name, FILE* fp){
	NODE* list = list_head;
	int block_id;

	block_id = list_remove(list, name, 'f');
	writeRecords(fp, cur_dir_block);

	block freeHead = getFreeBlock(fp);
	int oldFirstFree = freeHead.next_id;
	freeHead.next_id = block_id;

	block temp;
	// go to every block that the file has allocated to it
	// and change them to free
	int last_block_id; //required to seek after reading... very annoying
	do {
		fseek(fp, ((block_id*BLOCK_SIZE)), SEEK_SET);	// seek to the file block
		fread((void*)&temp, sizeof(block), 1, fp);
		
		temp.status = 'u';
		last_block_id = block_id;
		block_id = temp.next_id;
		if(block_id == 0){
			temp.next_id = oldFirstFree;
			printf("end of file\n");
		}
		
		fseek(fp, ((last_block_id*BLOCK_SIZE)), SEEK_SET);	// seek to the file block
		fwrite((void*)&temp, sizeof(block), 1, fp);
	}while(block_id != 0);

	// change free list pointer to point to newly removed file
	fseek(fp, ((freeHead.block_id*BLOCK_SIZE)), SEEK_SET);	// seek to the free list block
	fwrite((void*)&freeHead, sizeof(block), 1, fp);

	record dumb;
	dumb.type = '@';
	list_insert_end(list_head, dumb);
	writeRecords(fp, cur_dir_block);
	readDirectory(fp, cur_dir_block);
	
}

void deallocateBlock(FILE* fp, int block_id){

	// get next free block id
	block freeHead = getFreeBlock(fp);
	int oldFirstFree = freeHead.next_id;
	freeHead.next_id = block_id;

	// set free list to point to new free block
	fseek(fp, 0, SEEK_SET);
	fwrite((void*)&freeHead, sizeof(block), 1, fp);

	// go to new free block
	fseek(fp, (block_id*BLOCK_SIZE), SEEK_SET);

	//deallocate it & set its next id to next free block id
	block free;
	free.block_id = block_id;
	free.next_id = oldFirstFree;
	free.status = 'u';

	fwrite((void*)&free, sizeof(block), 1, fp);	
	
}

int createRecord(record* rec, unsigned short int fid, unsigned short int pid, char type, char* name, unsigned short int block){
	rec->file_id = fid;
	rec->parent_id = pid;
	rec->type = type;
	strcpy(rec->name, name);
	rec->block = block;
	rec->size = 0;
	time(&rec->timestamp);
	//rec->timestamp = 0;
}

void printFree(FILE* fp){
	block temp;
	int loc = 0;
	printf("\nFree Blocks:\n");
	do {
		fseek(fp, loc, SEEK_SET);
		fread((void*)&temp, sizeof(block), 1, fp);
		printf("\tblock_id %u\tnext_id %u\tstatus %c\t\n", temp.block_id, temp.next_id, temp.status);
		loc = temp.next_id*BLOCK_SIZE;
	}while(temp.next_id!=0);
}

void printBlocks(FILE* fp){
	block temp;
	int loc = 0;
	int num = 0;
	printf("\nAll Blocks:\n");
	do {
		fseek(fp, loc, SEEK_SET);
		fread((void*)&temp, sizeof(block), 1, fp);
		printf("\tblock_id %u\tnext_id %u\tstatus %c\t\n", temp.block_id, temp.next_id, temp.status);
		loc = loc+BLOCK_SIZE;
		num++;
	}while(num < 25);
}

void resetDisk(FILE* fp){
	int blank = 0;
	fseek(fp, 0, SEEK_END);
	long int end = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	while(ftell(fp) < end){
		fwrite(&blank, sizeof(int), 1, fp);
	}
}
OCFILE *open_file(char *filename, FILE* fp);
OCFILE *open_file(char *filename, FILE* fp){
	OCFILE *file;
	NODE *temp;
	file = malloc(sizeof(OCFILE));
	temp = list_find(list_head, filename, 'f');
	file->size = temp->rec.size;
	file->count = 0;
	int i = 0;
	
	int block_id = temp->rec.block;
	block blk;
	// adding all allocated blocks to the blocks array
	do {
		fseek(fp, ((block_id*BLOCK_SIZE)), SEEK_SET);	// seek to the file block
		fread((void*)&blk, sizeof(block), 1, fp);
		
		file->blocks[i] = block_id;
		block_id = blk.next_id;
		i++;
	}while(block_id != 0);
	for(;i<33; i++){
		file->blocks[i] = 0;
	}
	file->rec = &temp->rec;
	time(&file->rec->timestamp);
	
	printf("TEMP %d\t%s\t\t%d\t%c\t%d\t\t%d\t\t%s", temp->rec.file_id, temp->rec.name, temp->rec.size, temp->rec.type, temp->rec.block, temp->rec.parent_id, ctime(&temp->rec.timestamp));
	printf("FILE %d\t%s\t\t%d\t%c\t%d\t\t%d\t\t%s", file->rec->file_id, file->rec->name, file->rec->size, file->rec->type, file->rec->block, file->rec->parent_id, ctime(&file->rec->timestamp));
	printf("bytecount = %d\tfilesize = %d\tblocks[0]=%d\n", file->count, file->size, file->blocks[0]);
	return file;
}

void close_file(OCFILE *file);
void close_file(OCFILE *file){
	free(file);
	// call write records
}
void write_file(void* data, int size, int num, OCFILE* file, FILE* fp);
void write_file(void* data, int size, int num, OCFILE* file, FILE* fp){
	printf("WRITE FILE\n");
	char *write = data;
	int i, j;
	int index = 0;
	fseek(fp, (file->blocks[index]*BLOCK_SIZE)+sizeof(block), SEEK_SET);
	for(j = 0; j<num; j++){

		for(i = 0; i<size; i++){
			if(i/(BLOCK_SIZE - sizeof(block)) > index){
				// we are writing to the next block
				index++;
				if(file->blocks[index] == 0){
					// need to allocate a block to it
					int block_id;
					block temp;
					
					fseek(fp, file->blocks[index-1]*BLOCK_SIZE, SEEK_SET);
					fread((void*)&temp, sizeof(block), 1, fp);
					
					temp.next_id = allocateFreeBlock(fp, 'f');
					file->blocks[index] = temp.next_id;
					
					fseek(fp, file->blocks[index-1]*BLOCK_SIZE, SEEK_SET);
					fwrite((void*)&temp, sizeof(block), 1, fp);	
				}
				fseek(fp, (file->blocks[index]*BLOCK_SIZE)+sizeof(block), SEEK_SET);
			}
			printf("writing...%c\n", write[i]);
			fwrite(write, 1, 1, fp);
			write++;
		}
	}

	// update the timestamp
	time(&file->rec->timestamp);
	file->rec->size = num*size;
	writeRecords(fp, cur_dir_block);
	printf("DONE\n");
}
void read_file(void* data, int size, int num, OCFILE* file, FILE* fp);
void read_file(void* data, int size, int num, OCFILE* file, FILE* fp){
	char *read = data;
	int i, j;
	int index = 0;
	fseek(fp, (file->blocks[index]*BLOCK_SIZE)+sizeof(block), SEEK_SET);
	for(j = 0; j<num; j++){

		for(i = 0; i<size; i++){
			if(i/(BLOCK_SIZE - sizeof(block)) > index){
				index++;
				if(file->blocks[index]==0){
					// stop reading.
					return;
				}
				fseek(fp, (file->blocks[index]*BLOCK_SIZE)+sizeof(block), SEEK_SET);
			}
			printf("reading...\n");
			fread(read, 1, 1, fp);
			read++;
		}
	}
	printf("DONE\n");
	
}



/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
int main(){
	printf("\n\n-------FILE-SYSTEM-------\n");
	printf("BLOCK_SIZE %d\n", BLOCK_SIZE);
	printf("TOTAL_BLOCK_RECORDS %d\n", TOTAL_BLOCK_RECORDS);
	printf("size of record: %d\n", sizeof(record));
	printf("size of block: %d\n", sizeof(block));
	printf("size of time_t: %d\n", sizeof(time_t));
	printf("-------------------------\n\n");
	
	FILE *fp;
	fp = fopen("Drive2MB", "rb+");
	block temp;

	fseek(fp, sizeof(block), SEEK_SET);
	fread((void*)&file_counter, sizeof(int), 1, fp);
	fseek(fp, 0, SEEK_SET);

	printf("Starting file system...\n");
	
	fread((void*)&temp, sizeof(block), 1, fp);
	if(temp.status != 'u'){
		printf("The disk needs formatting...\n");
		formatdisk(fp);
	}else {
		printf("Reading root directory...\n");
		readDirectory(fp, cur_dir_block);
	}

	strcpy(sys_path,"~/");

	char command[80];
	char *argv[100]; //array of commands 
	record findRec;
	while(1){
		printf("OC-FILESYS: %s> ", sys_path);
		readCmdLn(command);
		if(command[0] == '\0'){
			continue;
		}
		int argc = parseCmdLn(command, argv, " ");
		if(argv[0] == '\0'){
			continue;
		} 
		if(strcmp(argv[0],"exit") == 0){
			// write out file counter
			fseek(fp, sizeof(block), SEEK_SET);
			fwrite((void*)&file_counter, sizeof(int), 1, fp);
			fclose(fp);
			exit(0);
		}
		else if(strcmp(argv[0],"mkfile") == 0){
			// check if it is greater than 12
			if(strlen(argv[1]) > 12)
				printf("file name is too long.\n");
			else
				createFile(argv[1], fp);
		}
		else if(strcmp(argv[0],"mkdir") == 0){
			if(strlen(argv[1]) > 12)
				printf("dir name is too long.\n");
			else
				createDirectory(argv[1], fp);
		}
		else if(!strcmp(argv[0],"rmfile")){
			removeFile(argv[1], fp);
		}
		else if(!strcmp(argv[0],"rmdir")){
			removeDirectory(argv[1], fp);
		}
		else if(!strcmp(argv[0],"ls")){
			// list the files in directory
			printRecords();
		}
		else if(!strcmp(argv[0],"freeblocks")){
			printFree(fp);
		}
		else if(!strcmp(argv[0],"printblocks")){
			printBlocks(fp);
		}
		else if(!strcmp(argv[0],"reformat")){
			formatdisk(fp);
		}
		else if(!strcmp(argv[0],"printlist")){
			list_print(list_head);
		}
		else if(!strcmp(argv[0],"open")){
			OCFILE *ocf = open_file(argv[1], fp);
			printf("bytecount = %d\tfilesize = %d\tblocks[0]=%d\n", ocf->count, ocf->size, ocf->blocks[0]);
			char *test = "hello";
			write_file(test, 5, 1, ocf, fp);

			char test2[6];
			
			read_file(&test2, 5, 1, ocf, fp);
			printf("read: %s\n", test2);
		}
		else if(!strcmp(argv[0],"cd")){
			// ex dir1/dir2/
			printf("%s\n", argv[1]);
			char *path[50];
			char *spath[50];
			int i = 0;
			int block_id, depth;
			char sys_path2[80];
			parseCmdLn(argv[1], path, "/");
			while(path[i] != '\0'){
				//printf("read %s\n", path[i]);
				// find dirname in the linked list
				if(strcmp(path[i], "~") == 0){
					cur_dir_block = 1;
					readDirectory(fp, 1);
					strcpy(sys_path, "~/");
				}
				else if(!strcmp(path[i], "..")){
					if(cur_dir_block == 1){
						printf("Restricted access.\n");
						break;
					}
					printf("sys_path %s\n", sys_path);
					
					strcpy(sys_path2, sys_path);
					depth = parseCmdLn(sys_path2, spath, "/");
					printf("%s\t%d\n", spath, depth);
				}else {
					findRec = list_find_name(list_head, path[i], 'd');
					if(findRec.block > 0){
						strcat(sys_path, path[i]);
						strcat(sys_path, "/");
						printf("readDirectory %d\n", findRec.block);
						cur_dir_block = findRec.block;
						readDirectory(fp, findRec.block);
					}else {
						printf("Could not find directory '%s'\n", path[i]);
						break;
					}
				}
				i++;
			}
			
		}
	}
	
	fclose(fp);
}

void readCmdLn(char *a){
	char c = getchar();
	int i = 0;
	while(c != '\n'){
		a[i] = c;
		c = getchar();
		i++;
	}
	a[i] = '\0';
}

int parseCmdLn(char *cmd, char **args, char *delim){
	int c=0;
	/*printf("%s\n", cmd);*/
	char *tok = strtok(cmd, delim);
	while(tok != NULL){
		args[c] = tok;
		tok = strtok(NULL, delim);
		c++;
 	}
	args[c] = NULL; //as requested
	return c;
}

