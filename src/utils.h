#include <string>
#include <vector>
#include <unordered_map>
#include <regex>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <stdio.h>
#include <sys/stat.h>

#include <boost/serialization/vector.hpp>
#include <boost/serialization/unordered_map.hpp>
// #include <boost/archive/text_iarchive.hpp>
// #include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

using namespace std;

#ifndef Utils_H
#define Utils_H

namespace Utils {
  const string cacheSuffix = ".CACHE";
  const string cacheDirectory = "./.cache/";

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
   * @author Porter Glines 3/5/19
   */
  vector< vector<string> > splitAll(string str, string sentenceDelims, string wordDelims);

  /**
   * @brief Read in training text
   *
   * @param filePath path to training text
   * @return text string
   * @author Porter Glines 3/5/19
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
   * @author Porter Glines 3/5/19
   */
  vector< vector<string> > processTrainingSentences(string text, int markovOrder=1);

  /**
   * Read from cache
   * @param ret reference to object to populate from cache
   * @param fileName name of original data source file
   * @author Porter Glines 5/13/19
   */
  template <class T>
  void readFromCache(T &ret, string fileName);

  /**
   * Write to cache
   * @param data object to write to cache
   * @param fileName name of original data source file
   * @author Porter Glines 5/13/19
   */
  template <class T>
  void writeToCache(T data, string fileName);

  /**
   * @brief returns the basename for a Unix filepath
   * "/foo/bar/file" would return "file"
   * @param filePath string path
   * @return basename as string
   * @author Porter Glines 3/7/19
   */
  string getBasename(string filePath);
}

#include "utils.inl"

#endif