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
 * FL - File
 *
 */

class FL {
	size_t size;
	string filename;
	protected:
		char file_type;
		meta data;
	public:
		FL();
		string name(string);							// one 'n doner
		unsigned short int page(unsigned short int);	// one 'n doner
		time_t timestamp(time_t);
		char type();
		//meta metr(meta);
		meta get_data();
		void set_data(meta);
};

/**
 * Constructor()
 *
 *
 */

FL::FL()
{
	// do some timestamp shit
	data.type = 'f';
	data.parent_id = 0;
	data.file_id = 0;
	file_type = 'f';
}

/**
 * name()
 * 
 * @param	string	new name of the file	(setter)
 * @returns	string	the name of the file	(getter)
 */
 
string FL::name(string newname = "")
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
 * page()
 *
 * @param	unsigned short int	page id	(setter)
 * @returns	unsigned short int	page id	(getter)
 */
 
unsigned short int FL::page(unsigned short int id = 0)
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
 * timestamp()
 *
 * @param	time_t	timestamp	(setter)
 * @returns	time_t	timestamp	(getter)
 */
 
time_t FL::timestamp(time_t tm = NULL)
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
 * type()
 *
 * @returns	char	file's type
 */
 
char FL::type()
{
	return file_type;
}

/**
 *
 *
 *
 */
 
// meta FL::metr(meta datr = NULL)
// {
// 	if (datr == NULL)
// 	{
// 		return data;
// 	}
// 	else
// 	{
// 		data = datr;
// 	}
// }

meta FL::get_data()
{
	return this->data;
}

void FL::set_data(meta datr)
{
	data = datr;
	data.type = file_type;	// quick hack to make sure 's' or 'p' don't make it
	filename = data.name;
}

/**
 * DR - Directory
 *
 */

class DR: public FL {
	DR* par;
	vector<FL*> files;
	public:
		DR();
		//DR(DR*);
		vector<FL*> children();		// returns the list of files & directories
		DR * parent(DR *);
		void add(FL*);	// it takes a file but could also be a directory
		void add(DR *);	// it adds the directory to its tree while adding itself
		                // as a parent to the new dir tree.
		
};

/**
 * Constructor()
 *
 *
 */
 
DR::DR()
{
	data.type = 'd';
	file_type = 'd';
	data.parent_id = 0;
	data.file_id = 0;
	par = NULL;
}

/**
 * children()
 *
 * @returns	vector<FL*>	a collection of file objects in the directory
 */
 
vector<FL*> DR::children()
{
	return files;
}

/**
 * parent()
 *
 * @param 	DR*	pointer to the parent in memory	(setter)
 * @returns	DR*	pointer to the parent in memory	(getter)
 */
 
DR * DR::parent(DR* dir = NULL)
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
 * add()
 *
 * @param 	FL*	pointer to the file in memory
 */
 
void DR::add(FL* file)
{
	files.push_back(file);
}

/**
 * add()
 *
 * @param 	DR*	pointer to the directory in memory
 */
 
void DR::add(DR* dir)
{
	dir->parent(this);
	files.push_back(dir);
}

#endif