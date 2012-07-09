#include "filesystem/file.h"
#include "filesystem/cli.h"
#include "filesystem/shell.h"
// #include "filesystem/filesystem.h"

#include <iostream>
#include <string>

using namespace std;

int main() {

	/*
		Compiler Tests
	 */

	FL file;
	file.name("foo");
	cout << file.name();

	CLI cli;
	cli.say("\"Hello World\" - CLI").color("green").write();

	SHELL shell;
	shell.run();

	return 0;
}