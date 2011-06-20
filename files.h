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
 *
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
 *
 *
 *
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
 *
 *
 *
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
 *
 *
 *
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
 *
 *
 *
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

 // add a ".." directory (parent pointer) and a "." directory (self pointer)

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

// have FILESYS var that points to the active directory

// Using pointers to parent tree and self dir will not work for ".." & "."

/**
 *
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
/*
DR::DR(DR* dir)
{
	data.type = 'd';
	file_type = 'd';

	parent(dir);
	add(this);
}*/

/**
 * tree()
 *
 *
 */
 
vector<FL*> DR::children()
{
	// files.insert(files.begin(), this);	// add itself
	// if (par != NULL)
	// {
	// 	files.insert(files.begin(), par);	// add its parent if one exists
	// }
	// I do not think it should do this ^^^^^
	return files;
}

/**
 *
 *
 *
 */
 
// todo: figure out why the fuck i cant do this
DR * DR::parent(DR* dir = NULL)
{
	if (dir == NULL)
	{
		return par;
	}
	else
	{
		par = dir;
	}
}

/**
 *
 *
 *
 */
 
void DR::add(FL* file)
{
	files.push_back(file);
}

/**
 *
 *
 *
 */
 
void DR::add(DR* dir)
{
	dir->parent(this);
	files.push_back(dir);
}

#endif