#ifndef _filesys_h_
#define _filesys_h_

#include <stdio.h>
#include <math.h>
#include <time.h>
#include "files.h"
#include "cli.h"


#include <sstream>


#define PAGE_SIZE 510
#define RECORDS_PER_PAGE (PAGE_SIZE-sizeof(page))/sizeof(meta)

class FILESYS {

	FILE * fp;
	DR * cwd;
	const char* filename;
	bool opened;
	long int size;
	long total_pages;

	CLI cli;

	private:

		/**
		 * Private: Wrapper for fseek.
		 *
		 * TODO: keep offset the bounds of the vdsk.
		 *
		 * offset - Number of bytes to offset from origin.
		 * origin - Position from where offset is added.
		 *          
		 *          SEEK_SET - begining of the file
		 *          SEEK_CUR - current position of the file pointer.
		 *          SEEK_END - end of the file.
		 *
		 * Returns nothing.
		 */
		void seek(long int offset, int origin = SEEK_SET)
		{
			// cli.say("seek to ").color("blue").write(false);
			// cout << offset << endl;
			fseek(fp, offset, origin);
		}

		/**
		 * Private:
		 */
		void util_drive_stats()
		{
			seek(0, SEEK_END);
			size = ftell(fp);		// total file size in bytes

			printf("%ld\n", size);

			// number of pages this drive can store
			total_pages = floor(size / PAGE_SIZE);
		}
		
		/**
		 * Private:
		 */
		void util_print_page(page * pg)
		{
			printf("\tid %d\tnext %d\tstatus %c\n", pg->id, pg->next, pg->status);
		}

		/**
		 * Private:
		 */
		void util_print_meta(meta * metr)
		{
			printf("\n\tfid %d\tpid %d\ttyp %c\tname %s\tpg %d\t\n\n",
					metr->file_id, metr->parent_id, metr->type, metr->name, metr->page_id);
		}

		void write_meta_data(meta * metr, int p, int n)
		{
			// printf("P.id=%d\tcount=%d\n", page_id, count);
			// cout << ((p*PAGE_SIZE)+sizeof(page))+(sizeof(meta)*(n));
			// cout << "\t";
			// cout << (510 * p) + 6 + (32 * n);
			// cli.say("=fp").color("yellow").write();

			int location = (p*PAGE_SIZE) + sizeof(page) + (n*sizeof(meta));
			// cout << location << "-" << ftell(fp) << endl;

			cli.say("wrote ").say(metr->name).say(" to ").color("blue").write(false);
			cout << location << endl;

			seek(location);
			fwrite((void*)&metr, sizeof(meta), 1, fp);
		}

		/**
		 * Private:
		 */
		void write_directory(DR * dir)
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
			int count        = 0;

			int page_id = dir->get_data().page_id;

			cout << "WRITE DIRECTORY @ page " << page_id << endl;

			seek(page_id*PAGE_SIZE);
			fread((void*)&pg, sizeof(page), 1, fp);

			util_print_page(&pg);

			overflow_id = pg.next;

			// Write out this directories data
			meta data = dir->get_data();
			data.type = 's';

			util_print_meta(&data);

			// seek((page_id*PAGE_SIZE)+sizeof(page));
			// fwrite((void*)&data, sizeof(meta), 1, fp);
			// cout << ftell(fp) << " " << count << endl;
			write_meta_data(&data, page_id, count++);
			// cout << ftell(fp) << " " << count << endl;
			// count++;

			DR * par = dir->parent();
			if (par != NULL)
			{
				cout << "\tWrite out parent" << endl;
				data = par->get_data();
				data.type = 'p';
				util_print_meta(&data);
				// fwrite((void*)&data, sizeof(meta), 1, fp);
				// count++;
				// cout << ftell(fp) << " " << count << endl;
				write_meta_data(&data, page_id, count++);
				// cout << ftell(fp) << " " << count << endl;
			}

			for (int i=0; i<tree.size(); i++)
			{
				temp = tree.at(i);

				if (count >= RECORDS_PER_PAGE)
				{
					cout << "\tGo to overflow page; count=" << count << endl;
					if (!overflow_id)
					{
						cout << pg.next << endl;
						cout << "OVERFLOW ID " << overflow_id << " " << i << endl;
						// need to allocate a page
						// point the current page to it
						// set overflow_id to this new page_id
						overflow_id = allocate_page(dir);
						pg.next     = overflow_id;
						cout << pg.next << endl;
						// fseek(fp, pg.id*PAGE_SIZE, SEEK_SET);
						util_print_page(&pg);
						seek(pg.id*PAGE_SIZE);
						fwrite((void*)&pg, sizeof(page), 1, fp);
						seek(pg.id*PAGE_SIZE);
						page checkpage;
						fread((void*)&checkpage, sizeof(page), 1, fp);
						util_print_page(&checkpage);
					}

					// fseek(fp, overflow_id*PAGE_SIZE, SEEK_SET);
					seek(overflow_id*PAGE_SIZE);
					fread((void*)&pg, sizeof(page), 1, fp);

					// cout << "Overflow Page" << endl;
					cli.say("Overflow Page").color("red").write();
					cout << ftell(fp) << endl;
					util_print_page(&pg);

					overflow_id = pg.next;
					count = 0; // figure out whether its 0 or 1
				}

				// cout << "\tWriting meta data; count=" << count << " i=" << i << endl;
				data = temp->get_data();
				util_print_meta(&data);
				// cout << "Before " << ftell(fp) << endl;
				// fwrite((void*)&data, sizeof(meta), 1, fp);
				// write_meta_data(&data, pg.id, count);
				// cout << ftell(fp) << endl;
				// count++;
				// cout << ftell(fp) << " " << count << endl;
				write_meta_data(&data, pg.id, count++);
				// cout << ftell(fp) << " " << count << endl;
			}

			if (count < RECORDS_PER_PAGE && overflow_id)
			{
				cout << "\tDeallocate extra page " << overflow_id << endl;
				deallocate_page(overflow_id);
			}
			
			cout << "\tDone. " << dir->name() << endl;

			// cout << "Before Check " << ftell(fp) << endl;
			// meta checkData;
			// seek(12798-32);
			// seek(8676);
			// seek(12798-32-32-32-32-32);
			// cout << "At Check " << ftell(fp) << endl;
			// fread((void*)&checkData, sizeof(meta), 1, fp);
			// cout << "After Check " << ftell(fp) << endl;
			// util_print_meta(&checkData);

		}

	public:

		void util_print_pages()
		{
			page list;
			long counter  = 0;
			long location = 0;

			cout << "\nPrinting all pages..." << endl;

			while (counter < total_pages)
			{
				seek(location);
				// last page so it shouldn't point to anything
				fread((void*)&list, sizeof(page), 1, fp);
				if (counter < 30)
				{
				printf("\tpage %d\tlocbyte %ld\tftell %ld  \tstatus %c\n", 
						list.id, location, ftell(fp)-sizeof(page), list.status
				);
				}
				counter++;
				location += PAGE_SIZE;
			}

			cout << "\tDone.\n" << endl;
		}

		// add a "start" function. Have two constructors, one with param and one without
		// that way other applications can use the api without having to declare a drive

		/**
		 * Public: Constructs the virtual filesystem.
		 */
		FILESYS(string fname = "")
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
		 * Deconstructor - Do some memory management here.
		 */
		~FILESYS()
		{
			cout << "~FILESYS()" << endl;
			if (opened)
			{
				close();
			}
		}

		/**
		 * Public: Opens the virtual disk.
		 *
		 * vdsk - 
		 *
		 * Returns nothing.
		 */
		void open(string vdsk)
		{
			page temp;
			filename = vdsk.c_str();
			fp = fopen(vdsk.c_str(), "rb+");

			if (fp == NULL)
			{
				throw "Drive Not Found";
			}

			opened = true;	// make sure fp is open first

			// cout << "Drive Opened." << endl;
			
			free_page_list(&temp); // read the free list into temp

			// determine if it needs to be reformatted
			// if not, then go as normal
			// move this to the SHELL -> this has no CLI
			// if so, prompt the user if they would like to reformat
				// if so, reformat, go as normal
				// if not, then fuck em. They're on their own.

			if (temp.status != 'n')
			{
				throw "Corrupted Drive";
			}
			else
			{
				util_drive_stats();	// gather information about the drive
									// must be called before util_print_pages()
				reformat();
				cout << "\tSize: " << size << " bytes" << endl << endl;

				open_directory(1);
				int test = make_directory("foo");

				// make_file("Foo.txt");

				ostringstream oss;
				int z=0;
				while(z < (RECORDS_PER_PAGE))
				{
					oss << "File-" << z;
					make_file(oss.str());
					oss.str("");
					
					z = z + 1;
				}

util_print_pages();
open_directory(test);
				open_directory(1);

				// write_meta_data(NULL, 1, 0);
				// write_meta_data(NULL, 1, 1);
				// write_meta_data(NULL, 1, 2);

				// write_meta_data(NULL, 17, 0);

				vector<FL*> children = cwd->children();
				vector<FL*>::iterator iter;

				printf("LS '%s' '%d'\n", cwd->name().c_str(), (int)children.size());
				for (iter = children.begin(); iter != children.end(); ++iter) {
					string name = (*iter)->name();
					printf("'%s'\n", name.c_str());
				}
				//make_directory("test");

				util_print_pages();
			}
		}

		/**
		 * Public: Closes the virtual disk.
		 */
		void close()
		{
			fclose(fp);
			opened = false;
		}

		/**
		 * Public: Reformats the virtual disk.
		 *
		 * Completely zeroes out the virtual disk and creates the
		 * root directory.
		 *
		 * Returns Boolean if it was successful
		 */
		bool reformat()
		{
			int blank         = 0;
			int file_counter  = 0;
			long counter      = 0;
			long location     = 0;

			cout << "Formatting drive..." << endl;

			util_drive_stats();		// gathers information about the drive
			
			seek(0);
			while(ftell(fp) < size)
			{
				fwrite(&blank, sizeof(int), 1, fp);
			}

			seek(0);
			// count by pages since the file size might yield excess space
			// write all pages to disk

			cout << "totalpages = " << total_pages << endl;

			while (counter < total_pages)
			{
				seek(location);
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

			allocate_page(cwd);
			// cout << "Allocated page " << allocate_page(cwd) 
			// 	 << " for " << cwd->name() << endl;

			write_directory(cwd);

			cout << "REFORMAT:: " << cwd->name() << endl;

			util_print_pages();
		}

		/**
		 * Public: Creates a new directory to the active directory.
		 *         Both directorie's meta data get written to vdsk.
		 *
		 * name - String name of the directory.
		 *
		 * Returns true if the directory was successfully created.
		 */
		bool make_directory(string name)
		{
			// create new DR
			// allocate a page for it
			// call cwd->add(DR)

			DR * newDir = new DR();
			newDir->name(name);

			int page_id = allocate_page(newDir);

			cwd->add(newDir);
			write_directory(cwd);
			write_directory(newDir);

			return page_id;
		}

		bool remove_directory();

		/**
		 * open_directory()
		 * 
		 * Seeks to the desired page and reads the directory's meta data into
		 * memory. Of course doing checks to make sure it is infact a directory.
		 *
		 * @param	integer		ID of the page to open
		 */
		bool open_directory(int page_id)
		{
			page pg;
			meta metr;
			DR * dir = new DR();

			int overflow_id = 0;
			int count       = 0;

			bool valid = false;

			seek(page_id*PAGE_SIZE);
			cout << ftell(fp) << endl;

			fread((void*)&pg, sizeof(page), 1, fp);
			// At this point the file pointer should be 6 bytes past the page header

			util_print_page(&pg);

			cout << "OPEN_DIRECTORY @ page " << page_id << endl;

			while (pg.status == 'd')
			{		
				overflow_id = pg.next;

				// cout << "Overflow: " << overflow_id << endl;
				cout << ftell(fp) << endl;
				fread((void*)&metr, sizeof(meta), 1, fp);
				// printf("metr: fid %d\tpid %d\ttyp %c\tname %s\tpg %d\t\n",
				// 	metr.file_id, metr.parent_id, metr.type, metr.name, metr.page_id);
				cli.say("-----").color("green").write();
				util_print_meta(&metr);
				cli.say("-----").color("red").write();

				if (metr.type == 's')
				{
					// self data
					dir->set_data(metr);
					cwd = dir;		// we know for sure it is a directory and has
									// a self record, good enough for me.
					valid = true;
					cli.say("self data").color("blue").write();
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
				else if (metr.type == 'f')
				{
					// do the normal shit. dir->add()

					FL * file = new FL(metr);

					dir->add(file);

					cout << "\tNORMAL FILE " << file->name() << endl;
				}
				else if (metr.type == 'd')
				{
					DR * subDir = new DR(metr);

					dir->add(subDir);

					cout << "\tNORMAL DIR " << subDir->name() << endl;
				}
				else
				{
					// our journey is over.
					// cout << "\tNOT a recognized file type '" <<  << "." << endl;
					// printf("\tNot a recognized file type '%c'.\n", metr.type);
					cli.say("ERROR NO FILE TYPE").color("red").write();
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
						// seek(overflow_id*PAGE_SIZE);
						fread((void*)&pg, sizeof(page), 1, fp);
						util_print_page(&pg);
						count = 0; // should it be -1
					}
					else
					{
						// no more records
						break;
					}
				}

				cout << count++ << endl;
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

		bool make_file(string name)
		{
			FL * newFile = new FL();
			newFile->name(name);

			// try
			int page_id = allocate_page(newFile);
			// catch

			cwd->add(newFile);
			write_directory(cwd);
		}

		bool remove_file();

		// TODO: Throw exceptions
		int  allocate_page(FL * file)
		{
			page list, temp;
			
			seek(0);
			fread((void*)&list, sizeof(page), 1, fp);	// read free list from the drive

			seek(list.next*PAGE_SIZE);	// go to next free page
			fread((void*)&temp, sizeof(page), 1, fp);

			//util_print_page(&list);
			//util_print_page(&temp);

			list.next = temp.next;
			temp.next = 0;
			temp.status = file->type();

			//util_print_page(&temp);
			//util_print_page(&list);

			seek(temp.id*PAGE_SIZE);
			fwrite((void*)&temp, sizeof(page), 1, fp);	// write the new page out

			seek(0);
			fwrite((void*)&list, sizeof(page), 1, fp);

			file->page(temp.id);

			cout << temp.id;
			cli.say(" allocated to ").say(file->name()).bold().color("green").write();

			return temp.id;
		}

		void deallocate_page(int page_id)
		{
			
		}

		// temporary i think
		vector<string> list_directory()
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
		 * free_page_list()
		 *
		 * Will read the first page or "free list" on disk into memory.
		 * 
		 * @param 	page *	points to the head of the free list of pages
		 */
		void free_page_list(page * pg)
		{
			seek(0);
			fread((void*)pg, sizeof(page), 1, fp);
		}
};

#endif