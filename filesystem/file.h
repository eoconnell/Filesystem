#ifndef _files_h_
#define _files_h_

#include <vector>
#include <string>
#include <time.h>

typedef struct {
	unsigned short int file_id;
	unsigned short int parent_id;	// this may turn out to be unused
	char type;
	char name[12]; // restrict to number of bytes
	unsigned short int page_id;
	unsigned int size;
	time_t timestamp;
} meta;

typedef struct {
	unsigned short int id;
	unsigned short int next;
	char status;
} page;

/**
 * Class FL (File)
 *
 */
class FL {

	size_t size;

	std::string filename;

	protected:

		char file_type;
		
		meta data;

	public:

		/**
		 * Public: Constructs a bare file.
		 */
		FL();

		FL(meta);

		/**
		 * Public: Set or get the file's name.
		 *
		 * newname - The String to name the file (optional).
		 *
		 * Examples
		 *
		 *     FL.name()
		 *     # => "foo"
		 *
		 *     FL.name("bar")
		 *     # => "bar"
		 *
		 * Returns the String name of the file.
		 */
		std::string name(std::string newname = "");

		/**
		 * Public: Set or get the file's page id on disk.
		 *
		 * id - The Unsigned Short Integer page id (optional).
		 *
		 * Examples
		 *
		 *     FL.page()
		 *     # => 3
		 *
		 *     FL.page(5)
		 *     # => 5
		 *
		 * Returns the file's Unsigned Short Integer page id.
		 */
		unsigned short int page(unsigned short int id = 0);

		/**
		 * Public: Set or get the file's timestamp.
		 *
		 * tm - The file's time_t timestamp (optional).
		 *
		 * Examples
		 *
		 *     FL.timestamp()
		 *     # => "foo"
		 *
		 *     FL.timestamp("bar")
		 *     # => "bar"
		 *
		 * Returns the file's time_t timestamp.
		 */
		time_t timestamp(time_t tm = NULL);

		/**
		 * Public: Gets the file's type.
		 *
		 * Example
		 *
		 *     FL.type()
		 *     # => 'f'
		 *
		 * Returns a Char denoting the file's type.
		 */
		char type();

		/**
		 * ######################################
		 *              NOT TESTED
		 * ######################################
		 * Public: Set or get a file's meta data.
		 *
		 * datr - The file's meta data (optional).
		 *
		 * Examples
		 *
		 *     FL.metr()
		 *     # => <meta struct>
		 *
		 *     FL.metr("bar")
		 *     # => "bar"
		 *
		 * Returns the file's meta data.
		 */
		// meta metr(meta datr = NULL)
		// {
		// 	if (datr != NULL)
		// 	{
		// 		data = datr;

		// 	}
		// 	return data;
		// }

		/**
		 * Public: Gets the file's meta data.
		 *
		 * Example
		 *
		 *     FL.get_data()
		 *     # => <meta struct>
		 *
		 * Returns the file's meta data.
		 */
		meta get_data();

		/**
		 * Public: Sets the file's meta data.
		 *
		 * datr - The meta data to set.
		 *
		 * Example
		 *
		 *     FL.set_data(<meta struct>)
		 *
		 * Returns nothing.
		 */
		void set_data(meta);
};

/**
 * Class DR (Directory) is a FL (File)
 *
 */
class DR: public FL {

	DR* par;
	std::vector<FL*> files;

	public:

		/**
		 * Public: Constructs a new directory.
		 */
		DR();

		DR(meta);

		//DR(DR*); // haven't quite figured out what I was thinking on this yet

		/**
		 * Public: List a directory's files.
		 *
		 * Returns a Vector containing pointers to files in memory.
		 */
		std::vector<FL*> children();

		/**
		 * Public: Set or get the parent directory.
		 *
		 * dir - A pointer to the directory to be set as the parent (optional).
		 *
		 * Examples
		 *
		 *     DR.parent()
		 *     # => DR*
		 *
		 *     DR.parent(dir)
		 *     # => NULL
		 *
		 * Returns a pointer to the parent directory in memory.
		 */
		DR * parent(DR* dir = NULL);

		/**
		 * Public: Add a file to a directory in memory.
		 *
		 * The file is appended to the directory's list.
		 *
		 * Returns nothing.
		 */
		void add(FL*);

		/**
		 * Public: Add a directory to a directory in memory.
		 *
		 * The directory is appended to the directory's list and
		 *  sets itself as the parent of the new directory.
		 *
		 * Returns nothing.
		 */
		void add(DR*);
};

#endif