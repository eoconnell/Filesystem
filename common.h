
#ifndef _common_h_
#define _common_h_

#include <string>
#include <vector>

/**
 * Trims a string of any characters provided in the character list.
 *
 * str      - String to trim.
 * charlist - String sequence of characters to be trimmed. (optional)
 *
 * Examples
 *
 *     trim("    Hello World.	  ")
 *     # => "Hello World."
 *
 * Returns the String after the trimmings.
 */
string trim(string str, string charlist = " \t\f\v\n\r")
{
	int first, last;
	last = str.find_last_not_of(charlist);

	// only contains chars that are being trimmed, return ""
	if (last  == string::npos)
	{
		return "";
	}

	first = str.find_first_not_of(charlist);

	if (first == string::npos)
	{
		first = 0;
	}

	return str.substr(first, (last-first)+1);
}

/**
 * Explodes a delimited String into a Vector of substrings.
 *
 * str   - String sequence separated by the delimiter.
 * delim - String delimiter.
 *
 * Examples
 *
 *     trim(Red,White,Blue", ",")
 *     # => [0] => "Red"
 *          [1] => "White"
 *          [2] => "Blue"
 *
 * Returns a Vector of substrings found between delimiters.
 */
vector<string> explode(string str, string delim)
{
	size_t left  = 0;	// left bound
	size_t right = 0;	// right bound
	size_t delim_len = delim.length();
	size_t str_len   = str.length();
	vector<string> result;

	// that was easy...
	if ((str = trim(str)).empty())
	{
		return result;
	}
	// add each substring between delimiters to the vector 
	do 
	{
		right = str.find(delim, left);
		result.push_back(str.substr(left, right-left));
		left = right + delim_len;
	} 
	while (right < str_len);
	
	return result;
}

#endif
