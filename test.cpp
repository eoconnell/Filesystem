#include <iostream>
#include <stdio.h>
#include <string>

using namespace std;

class Cli
{
	string output;
	public:
		Cli()
		{

		}

		Cli& write(string text)
		{
			output = output.append(text);
			return *this;
		}

		Cli& bold()
		{
			string temp = "\x1b[1m";
			temp.append(output);
			temp.append("\x1b[22m");

			output = temp;
			return *this;
		}

		void go()
		{
			cout << output << endl;
		}
};

int main() 
{
	Cli test = Cli();
	test.write("test").bold().go();
}