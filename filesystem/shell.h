#ifndef _shell_h_
#define _shell_h_

#include "cli.h"
#include "filesystem.h"

#include <string>

class SHELL {

	CLI cli;
	
	FILESYS fs;

	std::string  cwd;

	std::string  path;

	public:

		~SHELL();

		void run();

		bool execute(std::string, std::vector<std::string>);
};

#endif