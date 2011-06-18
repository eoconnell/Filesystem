
#ifndef _common_h_
#define _common_h_

#include <string>
#include <vector>

/**
 * trim()
 *
 * Trims a string of any characters provided in the charlist.
 *
 * @param	string	string
 * @param	string	charlist
 * @return	string
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
 * explode()
 * 
 * Returns a vector of strings, each of which is a substring formed
 * by splitting it on boundaries formed by the string delimiter.
 *
 * @param	string	delimiter
 * @param	string	string
 * @return  vector<string>
 */

vector<string> explode(string delim, string str)
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
