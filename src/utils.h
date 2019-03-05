#include <string>
#include <vector>

using namespace std;

#ifndef Utils_H
#define Utils_H

namespace Utils {
  /**
   * @brief Splits a string along delimiters
   * heavily utilizes regular expressions
   *
   * @param st string to split
   * @param delims string of delimiters
   *    example: \\s would split on whitespace
   *             \\s.,?! would split on whitespace and some punctuation
   * 
   * @return vector string representing the words that were split up
   * 
   * @author Porter Glines 1/7/19
   **/
  vector<string> split(string st, string delims);

  /**
   * @brief Splits a string along delimiters and lower characters
   * heavily utilizes regular expressions
   *
   * @param st string to split
   * @param delims: string of delimiters
   *    example: \\s would split on whitespace
   *             \\s.,?! would split on whitespace and some punctuation
   * 
   * @return vector string representing the words that were split up
   * 
   * @author Porter Glines 1/22/19
   **/
  vector<string> splitAndLower(string st, string delims);

  /**
   * @brief Split a string along delimiters into sentences consisting of words
   * @param str string to split
   * @param sentenceDelims delimiters for sentences
   * @param wordDelims delimiters for words
   * @return 2D vector of words in sentences
   * @author Proter Glines 3/5/19
   */
  vector< vector<string> > splitAll(string str, string sentenceDelims, string wordDelims);

  /**
   * @brief Read in training text
   *
   * @param filePath path to training text
   * @return text string
   * @author Proter Glines 3/5/19
   */
  string readInTrainingSentences(string filePath);

  /**
   * @brief Process training sentences
   *
   * Condense expanded contractions in COCA dataset
   * Combine words for higher than 1 markov order
   *
   * @param text entire input text
   * @param markovOrder lookahead for markov model
   * @return 2D vector of words in sentences
   * @author Proter Glines 3/5/19
   */
  vector<vector<string> > processTrainingSentences(string text, int markovOrder);
}

#endif