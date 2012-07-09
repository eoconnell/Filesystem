#include <iostream>
#include <fstream>
#include <stdio.h>

#include <string>
#include <vector>
using namespace std;


#include "files.h"
#include "filesystem.h"
#include "common.h"
#include "cli.h"
#include "shell.h"

// shell functions
	// use cli functions to interact with file-system functions
// file-system functions // more of an api
	// interacts with the disk
// cli functions
	// basic input and output

int main()
{
	printf("\n\n-------FILE-SYSTEM-------\n");
	printf("PAGE_SIZE %d\n", PAGE_SIZE);
	printf("RECORDS_PER_PAGE %lu\n", RECORDS_PER_PAGE);
	printf("size of meta: %lu\n", sizeof(meta));
	printf("size of page: %lu\n", sizeof(page));
	printf("size of time_t: %lu\n", sizeof(time_t));
	printf("-------------------------\n\n");


	DR test;
	cout << test.name("Hello") << endl;

	FILESYS filesys;
	// try 
	// {
	// 	filesys.open("disks/drive5.vdsk");
	// }
	// catch (char * e)
	// {
	// 	cout << "Exception: " << e << endl;
	// 	filesys.reformat();
	// }

	try
	{
		filesys.open("disks/drive5.vdsk");
	}
	catch (char * e)
	{
		cout << "Exception: " << e << endl;
	}

	// SHELL sh;
	// sh.run();

	return 0;
}