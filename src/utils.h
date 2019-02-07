#include <string>
#include <vector>

using namespace std;

#ifndef Utils_H
#define Utils_H

namespace Utils {
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

  /**
   * @brief Read in training text into an array of sentences made up of arrays of words
   *
   * @param filePath path to training text
   * @return vector< vector<string> > array of sentences made up of arrays of words
   */
  vector< vector<string> > readInTrainingSentences(string filePath, int markovOrder = 1);
}

#endif