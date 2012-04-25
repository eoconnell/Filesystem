#ifndef _cli_h_
#define _cli_h_

#include <iostream>
#include <string>
#include <vector>
#include <map>

class CLI {

	string output; // output buffer

	map<string, string> text_style;
	map<string, string> text_color;
	map<string, string> background_color;
	
	public:

		/**
		 * Public: Initializes Command Line Interface styles.
		 */
		CLI()
		{
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
		 * Deconstructor - Do some memory management here.
		 */
		~CLI()
		{
			cout << "~CLI()" << endl;
		}

		/**
		 * Public: Reads a string from standard in.
		 *
		 * Returns the String.
		 */
		string read()
		{
			string input;
			cin >> input;
			
			return input;
		}

		/**
		 * Public: Reads a line from standard in.
		 *
		 * Returns the String.
		 */
		string readline()
		{
			string input;
			getline(cin, input);
			
			return input;
		}

		/**
		 * Public: Prompts the user for input.
		 *
		 * text    - String prompt.
		 * newline - Boolean to add new line after prompt. (optional)
		 *
		 * Examples
		 *
		 *     prompt("What is your name?")
		 *     # => "Evan"
		 *
		 * Returns the String read in from standard in.
		 */
		string prompt(string text, bool newline = false)
		{
			cout << text;
			if (newline)
			{
				cout << endl;
			}
			return readline();	// trim it here?
		}

		/**
		 * Public: Prompts the user for input with valid options.
		 *
		 * text    - String prompt.
		 * options - Vector of Strings containing valid options.
		 *
		 * Examples
		 *
		 *     prompt("Pick a color.", ["Red","Black"])
		 *
		 * Returns the String read in from standard in.
		 */
		string prompt(string text, vector<string> options)
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
		 * Public: Appends text to the output buffer.
		 *
		 * text - String to output.
		 *
		 * Returns a pointer to the object for method chaining.
		 */
		CLI& say(string text)
		{
			output = output.append(text);
			return *this;
		}

		/**
		 * Public: Writes the output buffer to standard out.
		 *
		 * newline - Boolean to add new line after prompt. (optional)
		 *
		 * Examples
		 *
		 *    say("Hello World!").write()
		 *
		 * Returns nothing.
		 */
		void write(bool newline = true)
		{
			cout << output;

			if (newline)
			{
				cout << endl;
			}

			flush();
		}

		/**
		 * Public: Clears the output buffer.
		 *
		 * Returns nothing.
		 */
		void flush()
		{
			output = "";
		}

		/**
		 * Public: Changes the color of text in the output buffer.
		 *
		 * color - String name for the color.
		 *
		 * Examples
		 *
		 *     say("This worked!").color("green")
		 *
		 * Returns a pointer to the object for method chaining.
		 */
		CLI& color(string color)
		{
			string temp = text_color[color];
			temp.append(output);
			temp.append(text_style["reset"]);

			output = temp;
			
			return *this;
		}

		/**
		 * Public: Makes the text in the output buffer bold.
		 *
		 * Examples
		 *
		 *     say("This worked!").bold()
		 *
		 * Returns a pointer to the object for method chaining.
		 */
		CLI& bold()
		{
			string temp = text_style["bold_on"];
			temp.append(output);
			temp.append(text_style["bold_off"]);

			output = temp;
			
			return *this;
		}

		/**
		 * Public: Makes the text in the output buffer bold.
		 *
		 * Examples
		 *
		 *     say("This doesn't work yet :(").emph()
		 *
		 * Returns a pointer to the object for method chaining.
		 */
		CLI& emph()
		{
			string temp = text_style["italics_on"];
			temp.append(output);
			temp.append(text_style["italics_off"]);

			output = temp;
			
			return *this;
		}

		/**
		 * Public: Swaps the background color and text color of the text 
		 *         in the output buffer.
		 *
		 * Examples
		 *
		 *     say("Whoa color swap!").inverse()
		 *
		 * Returns a pointer to the object for method chaining.
		 */
		CLI& inverse()
		{
			string temp = text_style["inverse_on"];
			temp.append(output);
			temp.append(text_style["inverse_off"]);

			output = temp;
			
			return *this;
		}

		/**
		 * Public: Puts a strike through the text in the output buffer.
		 *
		 * Examples
		 *
		 *     say("I didn't mean to say this.").strike()
		 *
		 * Returns a pointer to the object for method chaining.
		 */
		CLI& strike()
		{
			string temp = text_style["strike_on"];
			temp.append(output);
			temp.append(text_style["strike_off"]);

			output = temp;
			
			return *this;
		}

		/**
		 * Public: Writes the error to standard out.
		 *
		 * text - String to output.
		 *
		 * Examples
		 *
		 *     error("Something bad hapened.")
		 *
		 * Returns nothing.
		 */
		void error(string text)
		{
			flush();
			say(text).color("red").write();
		}

		/**
		 * Public: Beeps at you.
		 *
		 * num - Integer how many times. (optional)
		 *
		 * Examples
		 *
		 *     beep()
		 *     >> *BEEP*
		 *
		 *     beep(3)
		 *     >> *BEEP* *BEEP* *BEEP*
		 *
		 * Returns nothing.
		 */
		void beep(int num = 1)
		{
			for (int i=0; i< num; i++)
			{
				cout << "\x07";
			}
		}

		/**
		 * Public: Make the user wait.
		 *
		 * seconds   - Double miliseconds to sleep. (optional)
		 * countdown - Boolean counts down each second. (optional)
		 *
		 * Returns nothing.
		 */
		void wait(double seconds = 0, bool countdown = false)
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
				say("").write();
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
					prompt("Press any key to continue...");
				}
			}
		}

		/**
		 * Public: Writes a bunch of blank lines to standard out.
		 *
		 * Super duper useful...
		 *
		 * Returns nothing.
		 */
		void new_line(int num)
		{
			for (int i=0; i<num; i++)
			{
				say("").write();
			}
		}

		/**
		 * Public: Clears the screen.
		 *
		 * Maybe just call new_line(80).
		 *
		 * Returns nothing.
		 */
		void clear()
		{
			flush();
			new_line(80);
		}
};

#endif