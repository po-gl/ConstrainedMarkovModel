#include <string>
#include <vector>
#include <regex>
#include "util.h"

using namespace std;

/**
 * Splits a string along delimiters
 * uses Regular expressions for splitting
 * Porter Glines 1/7/19
 **/
vector<string> split(string str, string delims) {
  vector<string> ret;

  regex wordsExp("[^" + delims + "]+");
  auto begin = sregex_iterator(str.begin(), str.end(), wordsExp);
  auto end = sregex_iterator();

  while (begin != end) {
    ret.push_back(begin->str());
    begin++;
  }

  return ret;
}
