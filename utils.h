#include <string>
#include <vector>

using namespace std;

#ifndef Utils_H
#define Utils_H

/**
 * Splits a string along delimiters
 * heavily utilizes regular expressions
 * 
 * delims: string of delimiters 
 *    example: \\s would split on whitespace
 *             \\s.,?! would split on whitespace and some punctuation
 * 
 * Returns vector string representing the words that were split up
 * 
 * Porter Glines 1/7/19
 **/
vector<string> split(string st, string delims);

/**
 * Splits a string along delimiters and lower characters
 * heavily utilizes regular expressions
 * 
 * delims: string of delimiters 
 *    example: \\s would split on whitespace
 *             \\s.,?! would split on whitespace and some punctuation
 * 
 * Returns vector string representing the words that were split up
 * 
 * Porter Glines 1/22/19
 **/
vector<string> splitAndLower(string st, string delims);

#endif