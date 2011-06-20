#ifndef _cli_h_
#define _cli_h_

#include <iostream>
#include <string>
#include <vector>
#include <map>

class CLI {

	string wait_msg;
	map<string, string> text_style;
	map<string, string> text_color;
	map<string, string> background_color;
	public:
		       CLI();
		string color(string, string, string);
		string bold(string);
		string emph(string);
		string inverse(string);
		string strike(string);
		string read();
		string readline();
		void   write(string, string, string);
		string prompt(string, bool);
		string prompt(string, vector<string>); 
		void   error(string);
		void   beep(int);
		void   wait(double, bool);
		void   new_line(int);
		void   clear_screen();
		       ~CLI();
};

/**
 * Constructor
 *
 *
 *
 */
 
CLI::CLI()
{
	wait_msg = "Press any key to continue...";
	
	text_style["reset"]         = "\x1b[0m";
	text_style["bold_on"]       = "\x1b[1m";
	text_style["italics_on"]    = "\x1b[3m";
	text_style["underline_on"]  = "\x1b[4m";
	text_style["inverse_on"]    = "\x1b[27m";
	text_style["strike_on"]     = "\x1b[9m";
	text_style["bold_off"]      = "\x1b[22m";
	text_style["italics_off"]   = "\x1b[23m";
	text_style["underline_off"] = "\x1b[24m";
	text_style["inverse_off"]   = "\x1b[27m";
	text_style["strike_off"]    = "\x1b[29m";

	text_color["black"]         = "\x1b[30m";
	text_color["red"]           = "\x1b[31m";
	text_color["green"]         = "\x1b[32m";
	text_color["yellow"]        = "\x1b[33m";
	text_color["blue"]          = "\x1b[34m";
	text_color["magenta"]       = "\x1b[35m";
	text_color["cyan"]          = "\x1b[36m";
	text_color["white"]         = "\x1b[37m";
	text_color["black"]         = "\x1b[38m";
	text_color["default"]       = "\x1b[39m";
	
	background_color["black"]   = "\x1b[40m";
	background_color["red"]     = "\x1b[41m";
	background_color["green"]   = "\x1b[42m";
	background_color["yellow"]  = "\x1b[43m";
	background_color["blue"]    = "\x1b[44m";
	background_color["magenta"] = "\x1b[45m";
	background_color["cyan"]    = "\x1b[46m";
	background_color["white"]   = "\x1b[47m";
	background_color["default"] = "\x1b[49m";
}

/**
 * color()
 *
 *
 *
 */

string CLI::color(string text, string color, string bg = "")
{
	text.insert(0, text_color[color]);
	if (!bg.empty())
	{
		text.insert(0, background_color[bg]);
	}
	text.append(text_style["reset"]);
	
	return text;
}

/**
 * bold()
 *
 *
 *
 */

string CLI::bold(string text)
{
	text.insert(0, text_style["bold_on"]);
	text.append(text_style["bold_off"]);
	
	return text;
}

/**
 * emph()
 *
 *
 *
 */

string CLI::emph(string text)
{
	text.insert(0, text_style["italics_on"]);
	text.append(text_style["italics_off"]);
	
	return text;
}

/**
 * inverse()
 *
 *
 *
 */

string CLI::inverse(string text)
{
	text.insert(0, text_style["inverse_on"]);
	text.append(text_style["inverse_off"]);
	
	return text;
}

/**
 * strike()
 *
 *
 *
 */

string CLI::strike(string text)
{
	text.insert(0, text_style["strike_on"]);
	text.append(text_style["strike_off"]);
	
	return text;
}

/**
 * read()
 *
 *
 *
 */

string CLI::read()
{
	string input;
	cin >> input;
	
	return input;
}

/**
 * readline()
 *
 *
 *
 */

string CLI::readline()
{
	string input;
	getline(cin, input);
	
	return input;
}

/**
 * write()
 *
 *
 *
 */

void CLI::write(string text, string color = "", string bg = "")
{
	cout << CLI::color(text, color, bg) << endl;
}

/**
 * prompt()
 *
 *
 *
 */

string CLI::prompt(string text, bool newline = false)
{
	cout << text;
	if (newline)
	{
		cout << endl;
	}
	return readline();	// trim it here?
}

/**
 * prompt()
 *
 *
 *
 */

string CLI::prompt(string text, vector<string> options)
{
	cout << text << endl;
	cout << "[ ";
	for (int i=0; i < options.size(); i++)
	{
		cout << options.at(i) << " ";
	}
	cout << "]" << endl;

	string input = readline();
	// print options
	// check options
	// return value
}

/**
 * error()
 *
 *
 *
 */

void CLI::error(string text)
{
	cout << CLI::color(text, "red") << endl;
}

/**
 * beep()
 *
 *
 *
 */

void CLI::beep(int num)
{
	for (int i=0; i< num; i++)
	{
		cout << "\x07";
	}
}

/**
 * wait()
 *
 *
 *
 */

void CLI::wait(double seconds = 0, bool countdown = false)
{
	// count down sleeping 1 second at each iteration
	if (countdown)
	{
		double time = seconds;
		
		while (time > 0)
		{
			cout << time << "... " << endl;
			sleep(1);
			time--;
		}
		write("");
	}
	// just sleep or prompt
	else
	{
		if (seconds > 0)
		{
			sleep(seconds);
		}
		else
		{
			prompt(wait_msg);
		}
	}
}

/**
 * new_line()
 *
 *
 *
 */

void CLI::new_line(int num)
{
	for (int i=0; i<num; i++)
	{
		write("");
	}
}

/**
 * clear_screen()
 *
 *
 *
 */

void CLI::clear_screen()
{
	// clear the screen
}

/**
 * Deconstructor
 *
 *
 *
 */

CLI::~CLI()
{
	cout << "~CLI()" << endl;
}

#endif