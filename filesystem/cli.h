#ifndef _cli_h_
#define _cli_h_

#include <string>
#include <vector>
#include <map>

class CLI {

	std::string output; // output buffer

	std::map<std::string, std::string> text_style;
	
	std::map<std::string, std::string> text_color;

	std::map<std::string, std::string> background_color;

	public:

		/**
		 * Public: Initializes Command Line Interface styles.
		 */
		CLI();
		
		/**
		 * Deconstructor - Do some memory management here.
		 */
		~CLI();

		/**
		 * Public: Reads a string from standard in.
		 *
		 * Returns the String.
		 */
		std::string read();

		/**
		 * Public: Reads a line from standard in.
		 *
		 * Returns the String.
		 */
		std::string readline();

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
		std::string prompt(std::string text, bool newline = false);

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
		std::string prompt(std::string, std::vector<std::string>);

		/**
		 * Public: Appends text to the output buffer.
		 *
		 * text - String to output.
		 *
		 * Returns a pointer to the object for method chaining.
		 */
		CLI& say(std::string);

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
		void write(bool newline = true);

		/**
		 * Public: Clears the output buffer.
		 *
		 * Returns nothing.
		 */
		void flush();

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
		CLI& color(std::string);

		/**
		 * Public: Makes the text in the output buffer bold.
		 *
		 * Examples
		 *
		 *     say("This worked!").bold()
		 *
		 * Returns a pointer to the object for method chaining.
		 */
		CLI& bold();

		/**
		 * Public: Makes the text in the output buffer bold.
		 *
		 * Examples
		 *
		 *     say("This doesn't work yet :(").emph()
		 *
		 * Returns a pointer to the object for method chaining.
		 */
		CLI& emph();

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
		CLI& inverse();

		/**
		 * Public: Puts a strike through the text in the output buffer.
		 *
		 * Examples
		 *
		 *     say("I didn't mean to say this.").strike()
		 *
		 * Returns a pointer to the object for method chaining.
		 */
		CLI& strike();

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
		void error(std::string);

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
		void beep(int num = 1);

		/**
		 * Public: Make the user wait.
		 *
		 * seconds   - Double miliseconds to sleep. (optional)
		 * countdown - Boolean counts down each second. (optional)
		 *
		 * Returns nothing.
		 */
		void wait(double seconds = 0, bool countdown = false);

		/**
		 * Public: Writes a bunch of blank lines to standard out.
		 *
		 * Super duper useful...
		 *
		 * Returns nothing.
		 */
		void new_line(int);

		/**
		 * Public: Clears the screen.
		 *
		 * Maybe just call new_line(80).
		 *
		 * Returns nothing.
		 */
		void clear();
};

#endif