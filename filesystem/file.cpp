#include "file.h"

#include <vector>
#include <string>
#include <time.h>

using namespace std;

/**
 * Public: Constructs a bare file.
 */
FL::FL()
{
	// do some timestamp shit
	data.type = 'f';
	data.parent_id = 0;
	data.file_id = 0;
	file_type = 'f';
}

FL::FL(meta datr)
{
	file_type = 'f';
	set_data(datr);
}

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
string FL::name(string newname /*= ""*/)
{
	if (newname.empty())
	{
		return filename;
	}
	else
	{
		strcpy(data.name, newname.substr(0, 12).c_str());
		filename = newname;
		return newname;
	}
}

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
unsigned short int FL::page(unsigned short int id /*= 0*/)
{
	if (id)
	{
		data.page_id = id;
	}
	else
	{
		return data.page_id;
	}
}

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
time_t FL::timestamp(time_t tm /*= NULL*/)
{
	if (time == NULL)
	{
		return data.timestamp;
	}
	else
	{
		data.timestamp = tm;
	}
}

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
char FL::type()
{
	return file_type;
}

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
meta FL::get_data()
{
	return this->data;
}

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
void FL::set_data(meta datr)
{
	data = datr;
	data.type = file_type;	// quick hack to make sure 's' or 'p' don't make it
	filename = data.name;
}


/**
 * Public: Constructs a new directory.
 */
DR::DR()
{
	data.type = 'd';
	file_type = 'd';
	data.parent_id = 0;
	data.file_id = 0;
	par = NULL;
}

DR::DR(meta datr)
{
	file_type = 'd';
	set_data(datr);
}

//DR(DR*); // haven't quite figured out what I was thinking on this yet

/**
 * Public: List a directory's files.
 *
 * Returns a Vector containing pointers to files in memory.
 */
vector<FL*> DR::children()
{
	return files;
}

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
DR * DR::parent(DR* dir /*= NULL*/)
{
	if (dir == NULL)
	{
		return par;
	}
	else
	{
		par = dir;
		return NULL;
	}
}

/**
 * Public: Add a file to a directory in memory.
 *
 * The file is appended to the directory's list.
 *
 * Returns nothing.
 */
void DR::add(FL* file)
{
	files.push_back(file);
}

/**
 * Public: Add a directory to a directory in memory.
 *
 * The directory is appended to the directory's list and
 *  sets itself as the parent of the new directory.
 *
 * Returns nothing.
 */
void DR::add(DR* dir)
{
	dir->parent(this);
	files.push_back(dir);
}