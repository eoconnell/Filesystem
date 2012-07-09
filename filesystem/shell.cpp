#include "shell.h"
#include "common.h"

#include <string>
#include <iostream>

using namespace std;

SHELL::~SHELL()
{
	cout << "~SHELL()" << endl;
}

void SHELL::run()
{
	string command;
	vector<string> args;

	try
	{
		fs.open("disks/drive5.vdsk");
	}
	catch (char * e)
	{
		cout << "Exception: " << e << endl;
		return;
	}
	
	while(true)
	{
		// command = cli.prompt("OC-FILESYS: > ");
		command = cli.prompt(">>> ");
		// move this to execute() when ready.
		if (command.empty())
		{
			continue;
		}
		
		if (!command.compare("exit"))
		{
			cli.say("Exiting..").color("red").strike().write();
			break;
		}
		
		args = explode(command, " ");
		command = args.at(0);	// command to execute
		args.erase(args.begin(), args.begin()+1);
		execute(command, args);
	}
}

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
			cli.say("Reformat..").color("red").write();
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

	if (!cmd.compare("pd"))
	{
		fs.util_print_pages();
	}

	if (!cmd.compare("clear"))
	{
		cli.clear();
	}

	if (!cmd.compare("touch"))
	{
		fs.make_file(args[0]);
	}

	// gonna be one ugly mess
	return true;
}