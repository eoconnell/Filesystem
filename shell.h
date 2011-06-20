#ifndef _shell_h_
#define _shell_h_

#include <string>
#include "cli.h"

class SHELL {

	CLI     cli;
	FILESYS fs;
	string  cwd;
	string  path;
	public:
		void run();
		bool execute(string, vector<string>);
		     ~SHELL();
};

/**
 * run()
 *
 *
 *
 */

void SHELL::run()
{
	string command;
	vector<string> args;

	try
	{
		fs.open("drive5");
	}
	catch (char * e)
	{
		cout << "Exception: " << e << endl;
		return;
	}
	
	while(true)
	{
		command = cli.prompt("OC-FILESYS: > ");
		//command = cli.prompt(">>> ");
		// move this to execute() when ready.
		if (command.empty())
		{
			continue;
		}
		
		if (!command.compare("exit"))
		{
			cout << cli.color("Exiting.", "red") << endl;
			break;
		}
		
		args = explode(" ", command);
		command = args.at(0);	// command to execute
		args.erase(args.begin(), args.begin()+1);
		execute(command, args);
	}
}

/**
 * execute()
 *
 *
 *
 */

bool SHELL::execute(string cmd, vector<string> args)
{
	cout << "Execute: " << cmd << endl;
	for (int i=0; i<args.size(); i++)
	{
		cout << " " << args[i] << endl;
	}

	if (!cmd.compare("reformat"))
	{
		string ans = cli.prompt("Are you sure you want to reformat? Everything will be lost. [Y,N]", true);
		if (!trim(ans).compare("Y"))
		{
			cli.write("Reformat..", "red");
		}
	}

	if (!cmd.compare("ls"))
	{
		vector<string> list = fs.list_directory();
		for (int i=0; i < list.size(); i++)
		{
			cout << list.at(i) << "\t";
		}

		cout << endl;
	}

	// gonna be one ugly mess
	return true;
}

/**
 * Deconstructor
 *
 *
 *
 */

SHELL::~SHELL()
{
	cout << "~SHELL()" << endl;
}

#endif