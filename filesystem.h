#ifndef _filesys_h_
#define _filesys_h_

#include <stdio.h>
#include <math.h>
#include <time.h>
#include "files.h"


#define PAGE_SIZE 510
#define RECORDS_PER_PAGE (PAGE_SIZE-sizeof(page))/sizeof(meta)

class FILESYS {
	FILE * fp;
	DR * cwd;
	const char* filename;
	bool opened;
	long size;
	long total_pages;
	private:
		void util_drive_stats();
		
		void util_print_page(page *);
		void util_print_meta(meta *);
		void write_directory(DR *);
	public:
		void util_print_pages();
		     FILESYS(const char*);
		void open(const char*);		// opens the drive (file)
		void close();				// closes the drive
		bool reformat();				// fresh install
		bool make_directory(const char *);
		bool remove_directory();
		bool open_directory(int);
		bool make_file(const char * name);
		bool remove_file();
		int  allocate_page(FL*);
		void deallocate_page(int);
		vector<string> list_directory();
		void free_page_list(page *);
		     ~FILESYS();
};

/**
 * free_page_list()
 *
 * Will read the first page or "free list" on disk into memory.
 * 
 * @param 	page *	points to the head of the free list of pages
 */

void FILESYS::free_page_list(page * pg)
{
	fseek(fp, 0, SEEK_SET);
	fread((void*)pg, sizeof(page), 1, fp);
}

/**
 * __constructor()
 *
 * 
 *
 */

// add a "start" function. Have two constructors, one with param and one without
// that way other applications can use the api without having to declare a drive
FILESYS::FILESYS(const char* fname = "")
{	
	cout << "Starting filesystem..." << endl << endl;
	
	if (fname != "")
	{
		try
		{
			open(fname);
		}
		catch (char * e)
		{
			throw e;
		}
	}
}

/**
 * open()
 *
 *
 *
 */

void FILESYS::open(const char* fname)
{
	page temp;
	filename = fname;
	fp = fopen(fname, "rb+");

	if (fp == NULL)
	{
		throw "Drive Not Found";
	}

	opened = true;	// make sure fp is open first

	cout << "Drive Opened." << endl;
	
	free_page_list(&temp);

	// determine if it needs to be reformatted
	// if not, then go as normal
	// move this to the SHELL -> this has no CLI
	// if so, prompt the user if they would like to reformat
		// if so, reformat, go as normal
		// if not, then fuck em. They're on their own.

	if (temp.status != 'n')
	{
		// Make this a CorruptedDriveException
		// so it can actually be handled correctly
		throw "Corrupted Drive";
	}
	else
	{
		util_drive_stats();	// gather information about the drive
							// must be called before util_print_pages()
		reformat();
		cout << "\tSize: " << size << " bytes" << endl << endl;

		open_directory(1);
		make_directory("test");
	}
}

/**
 * close()
 *
 *
 *
 */

void FILESYS::close()
{
	fclose(fp);
	opened = false;
}

/**
 * reformat()
 *
 *
 *
 */

bool FILESYS::reformat()
{
	// zero out file
	// format the file ;)

	int blank         = 0;
	int file_counter  = 0;
	long counter      = 0;
	long location     = 0;

	cout << "Formatting drive..." << endl;

	util_drive_stats();		// gathers information about the drive
	
	fseek(fp, 0, SEEK_SET);
	while(ftell(fp) < size)
	{
		fwrite(&blank, sizeof(int), 1, fp);
	}

	fseek(fp, 0, SEEK_SET);
	// count by pages since the file size might yield excess space
	// write all pages to disk

	//cout << "totalpages = " << total_pages << endl;

	while (counter < total_pages)
	{
		fseek(fp, location, SEEK_SET);
		page free_list; // move this out???????
		free_list.id = counter;
		// last page so it shouldn't point to anything
		if (counter == total_pages-1)
		{
			free_list.next = 0;
		}
		else
		{
			free_list.next = counter+1;
		}
		free_list.status = 'n';
		//if (counter < 10)
		//{
		/*printf("\tCreating... page %ld\tloc %ld\tftell %ld\n", 
				counter, location, ftell(fp)
		);*/
		//}
		fwrite((void*)&free_list, sizeof(page), 1, fp);
		counter++;
		location += PAGE_SIZE;
	}

	util_print_pages();

	// create root directory
	cwd = new DR();
	cwd->name("root");

	cout << "Allocated page " << allocate_page(cwd) 
	     << " for " << cwd->name() << endl;

	write_directory(cwd);

	cout << "REFORMAT:: " << cwd->name() << endl;

	util_print_pages();
}

/**
 * allocate_page()
 *
 *
 *
 */

int FILESYS::allocate_page(FL * file)
{
	page list, temp;
	
	fseek(fp, 0, SEEK_SET);
	//cout << ftell(fp) << endl;
	fread((void*)&list, sizeof(page), 1, fp);	// read free list from the drive

	fseek(fp, list.next*PAGE_SIZE, SEEK_SET);	// go to the next free page
	//cout << ftell(fp) << endl;
	fread((void*)&temp, sizeof(page), 1, fp);

	//util_print_page(&list);
	//util_print_page(&temp);

	list.next = temp.next;
	temp.next = 0;
	temp.status = file->type();

	//util_print_page(&temp);
	//util_print_page(&list);

	fseek(fp, temp.id*PAGE_SIZE, SEEK_SET);		// rewind to the start again
	fwrite((void*)&temp, sizeof(page), 1, fp);	// write the new page out

	fseek(fp, 0, SEEK_SET);
	fwrite((void*)&list, sizeof(page), 1, fp);

	file->page(temp.id);

	return temp.id;
}

void FILESYS::deallocate_page(int page_id)
{
	
}

/**
 * make_directory()
 *
 * This method will create a new directory and add it to the current working
 * directory. It writes both directories meta data to their page.
 *
 * @param 	char *	Name of the new directory
 */

bool FILESYS::make_directory(const char * name)
{
	// create new DR
	// allocate a page for it
	// call cwd->add(DR)

	DR * make = new DR();
	make->name(name);

	int page_id = allocate_page(make);

	make->page(page_id);
	// init meta data
	// call cwd->add()
	// write_directory(cwd)

	cwd->add(make);
	write_directory(cwd);
	write_directory(make);
}

/**
 * open_directory()
 * 
 * Seeks to the desired page and reads the directory's meta data into
 * memory. Of course doing checks to make sure it is infact a directory.
 *
 * @param	integer		ID of the page to open
 */

bool FILESYS::open_directory(int page_id)
{
	page pg;
	meta metr;
	DR * dir = new DR();

	int overflow_id = 0;
	int count       = 0;

	bool valid = false;
	
	fseek(fp, page_id*PAGE_SIZE, SEEK_SET);
	//cout << ftell(fp) << endl;
	fread((void*)&pg, sizeof(page), 1, fp);
	//cout << ftell(fp) << endl;
	// At this point the file pointer should be 6 bytes past the page header

	cout << "OPEN_DIRECTORY @ page " << page_id << endl;

	while (pg.status == 'd')
	{		
		overflow_id = pg.next;

		fread((void*)&metr, sizeof(meta), 1, fp);
		// printf("metr: fid %d\tpid %d\ttyp %c\tname %s\tpg %d\t\n",
		// 	metr.file_id, metr.parent_id, metr.type, metr.name, metr.page_id);
		util_print_meta(&metr);

		if (metr.type == 's')
		{
			// self data
			dir->set_data(metr);
			cwd = dir;		// we know for sure it is a directory and has
							// a self record, good enough for me.
			valid = true;
		}
		else if (metr.type == 'p')
		{
			cout << "\tPARENT DATA" << endl;
			DR * par = new DR();
			par->name(metr.name);
			par->set_data(metr);
			dir->parent(par);	// make sure this actually works
								// thinking run-time error
		}
		else if (metr.type == 'f' or metr.type == 'd')
		{
			// do the normal shit. dir->add()
			cout << "\tNORMAL FILE/DIR" << endl;
		}
		else
		{
			// our journey is over.
			cout << "\tNOT a recognized file type." << endl;
			break;
		}

		if (count >= RECORDS_PER_PAGE)
		{
			cout << "\tGo to overflow page" << endl;
			if (overflow_id)
			{
				// seek there
				// read page
				// set new overflow_id
				// set num back to 0
				fseek(fp, overflow_id*PAGE_SIZE, SEEK_SET);
				fread((void*)&pg, sizeof(page), 1, fp);
				count = 0; // should it be -1
			}
			else
			{
				// no more records
				break;
			}
		}

		count++;
	}

	if (valid)
	{
		cout << "Opened Directory '" << dir->name() << "'" << endl;
	}
	else
	{
		cout << "error....." << endl;
	}

	//cout << "PAGE " << page_id << " is not a directory." << endl;
	return false;

	// read first meta record, one should exist
	// use that to create the DR object
		// foreach of the rest, do the same but call DR.add()


}

/**
 * write_directory()
 *
 *
 *
 */

void FILESYS::write_directory(DR * dir)
{
	// could be cwd or a new one
	// access it's meta data and find which page it belongs to
	// seek to that page
	// read that page into memory
	// *do some overflow magic*
	page pg;
	FL * temp;
	vector<FL*> tree = dir->children();
	int overflow_id  = 0;
	int count        = 1;

	int page_id = dir->get_data().page_id;

	cout << "WRITE DIRECTORY @ page " << page_id << endl;

	fseek(fp, page_id*PAGE_SIZE, SEEK_SET);
	fread((void*)&pg, sizeof(page), 1, fp);

	overflow_id = pg.next;

	// Write out this directories data
	meta data = dir->get_data();
	data.type = 's';

	util_print_meta(&data);

	fseek(fp, page_id*PAGE_SIZE+sizeof(page), SEEK_SET);
	fwrite((void*)&data, sizeof(meta), 1, fp);

	DR * par = dir->parent();
	if (par != NULL)
	{
		cout << "\tWrite out parent" << endl;
		data = par->get_data();
		data.type = 'p';
		util_print_meta(&data);
		fwrite((void*)&data, sizeof(meta), 1, fp);
	}


	for (int i=0; i<tree.size(); i++)
	{
		temp = tree.at(i);

		if (count > RECORDS_PER_PAGE)
		{
			cout << "\tGo to overflow page; count=" << count << endl;
			if (!overflow_id)
			{
				// need to allocate a page
				// point the current page to it
				// set overflow_id to this new page_id
				overflow_id = allocate_page(dir);
				pg.next     = overflow_id;
				fseek(fp, pg.id*PAGE_SIZE, SEEK_SET);
				fwrite((void*)&pg, sizeof(page), 1, fp);
			}

			fseek(fp, overflow_id*PAGE_SIZE, SEEK_SET);
			fread((void*)&pg, sizeof(page), 1, fp);
			overflow_id = pg.next;
			count = 0; // figure out whether its 0 or 1
		}

		cout << "\tWriting meta data; count=" << count << endl;
		data = temp->get_data();
		util_print_meta(&data);
		fwrite((void*)&data, sizeof(meta), 1, fp);
		count++;
	}

	if (count <= RECORDS_PER_PAGE && overflow_id)
	{
		cout << "\tDeallocate extra page " << overflow_id << endl;
		deallocate_page(overflow_id);
	}
	
	cout << "\tDone." << endl;
	cout << dir->name() << endl;
}

/**
 * make_file()
 *
 *
 *
 */

bool FILESYS::make_file(const char * name)
{
	FL * make = new FL();
	make->name(name);

	int page_id = allocate_page(make);

	make->page(page_id);

	cwd->add(make);
	write_directory(cwd);
}

/**
 * list_directory()
 *
 *
 *
 */

vector<string> FILESYS::list_directory()
{
	vector<string> list;
	vector<FL*> test;

	open_directory(2);

	make_file("file1");	// bugger when writing
	make_file("file2");
	make_file("file3");
	make_file("file4");
	make_file("file5");
	make_file("file6");
	make_file("file7");
	make_file("file8");
	make_file("file9");
	make_file("file10");
	make_file("file11");
	make_file("file12");
	make_file("file13");
	make_file("file14");
	make_file("file15");
	make_file("file16");


	if (cwd->parent() != NULL)
	{
		list.push_back("..");
	}

	list.push_back(".");

	test = cwd->children();

	for(int i=0; i<test.size(); i++)
	{
		list.push_back(test.at(i)->name());
	}
	
	// list.push_back("dir1");
	// list.push_back("file1");
	// list.push_back("file2");
	// list.push_back("file3");
	// list.push_back("dir2");
	return list;
}

/**
 * make_file()
 *
 *
 *
 */

void FILESYS::util_drive_stats()
{
	fseek(fp, 0, SEEK_END);
	size = ftell(fp);		// total file size in bytes

	// number of pages this drive can store
	total_pages = floor(size / PAGE_SIZE);
}

void FILESYS::util_print_meta(meta * metr)
{
	printf("\n\tfid %d\tpid %d\ttyp %c\tname %s\tpg %d\t\n\n",
			metr->file_id, metr->parent_id, metr->type, metr->name, metr->page_id);
}

/**
 * util_print_page()
 *
 * Prints the data of a page to the console.
 *
 * @param 	page *	pointer to the page
 */

void FILESYS::util_print_page(page * pg)
{
	printf("\tid %d\tnext %d\tstatus %c\n", pg->id, pg->next, pg->status);
}

/**
 * util_print_pages()
 *
 *
 *
 */

void FILESYS::util_print_pages()
{
	page list;
	long counter  = 0;
	long location = 0;

	cout << "\nPrinting all pages..." << endl;

	while (counter < total_pages)
	{
		fseek(fp, location, SEEK_SET);
		// last page so it shouldn't point to anything
		fread((void*)&list, sizeof(page), 1, fp);
		if (counter < 20)
		{
		printf("\tpage %d\tlocbyte %ld\tftell %ld\tstatus %c\n", 
				list.id, location, ftell(fp)-sizeof(page), list.status
		);
		}
		counter++;
		location += PAGE_SIZE;
	}

	cout << "\tDone.\n" << endl;
}

/**
 * __deconstructor()
 *
 *
 *
 */

FILESYS::~FILESYS()
{
	cout << "~FILESYS()" << endl;
	// if fp is still open, close it
	if (opened)
	{
		close();
	}
}

#endif