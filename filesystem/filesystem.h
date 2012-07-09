#ifndef _filesys_h_
#define _filesys_h_

#include "file.h"
#include "cli.h"

#define PAGE_SIZE 510
#define RECORDS_PER_PAGE (PAGE_SIZE-sizeof(page))/sizeof(meta)

class FILESYS {

	FILE * fp;
	
	DR * cwd;
	
	const char* filename;
	
	bool opened;
	
	long int size;

	long total_pages;

	CLI cli;	// temporary while testing without shell

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
		void seek(long int offset, int origin = SEEK_SET);

		/**
		 * Private:
		 */
		void util_drive_stats();
		
		/**
		 * Private:
		 */
		void util_print_page(page *);

		/**
		 * Private:
		 */
		void util_print_meta(meta *);

		void write_meta_data(meta *, int, int);

		void read_meta_data(meta *, int, int);

		/**
		 * Private:
		 */
		void write_directory(DR *);

	public:

		void util_print_pages();

		// add a "start" function. Have two constructors, one with param and one without
		// that way other applications can use the api without having to declare a drive

		/**
		 * Public: Constructs the virtual filesystem.
		 */
		FILESYS(std::string fname = "");

		/**
		 * Deconstructor - Do some memory management here.
		 */
		~FILESYS();

		/**
		 * Public: Opens the virtual disk.
		 *
		 * vdsk - 
		 *
		 * Returns nothing.
		 */
		void open(std::string);

		/**
		 * Public: Closes the virtual disk.
		 */
		void close();

		/**
		 * Public: Reformats the virtual disk.
		 *
		 * Completely zeroes out the virtual disk and creates the
		 * root directory.
		 *
		 * Returns Boolean if it was successful
		 */
		bool reformat();

		/**
		 * Public: Creates a new directory to the active directory.
		 *         Both directorie's meta data get written to vdsk.
		 *
		 * name - String name of the directory.
		 *
		 * Returns true if the directory was successfully created.
		 */
		bool make_directory(std::string);

		bool remove_directory();

		/**
		 * open_directory()
		 * 
		 * Seeks to the desired page and reads the directory's meta data into
		 * memory. Of course doing checks to make sure it is infact a directory.
		 *
		 * @param	integer		ID of the page to open
		 */
		bool open_directory(int);

		bool make_file(std::string);

		bool remove_file();

		// TODO: Throw exceptions
		int allocate_page(FL *);

		void deallocate_page(int);

		// temporary i think
		std::vector<std::string> list_directory();

		/**
		 * free_page_list()
		 *
		 * Will read the first page or "free list" on disk into memory.
		 * 
		 * @param 	page *	points to the head of the free list of pages
		 */
		void free_page_list(page *);
};

#endif