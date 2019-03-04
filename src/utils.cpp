#include <string>
#include <vector>
#include <regex>
#include <fstream>
#include <sstream>
#include <algorithm>
#include "utils.h"

using namespace std;

/**
 * Splits a string along delimiters
 * uses Regular expressions for splitting
 * Porter Glines 1/7/19
 **/
vector<string> Utils::split(string str, string delims) {
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


/**
 * Splits a string along delimiters
 * uses Regular expressions for splitting
 * Porter Glines 1/7/19
 **/
vector<string> Utils::splitAndLower(string str, string delims) {
  vector<string> ret;

  regex wordsExp("[^" + delims + "]+");
  auto begin = sregex_iterator(str.begin(), str.end(), wordsExp);
  auto end = sregex_iterator();

  while (begin != end) {
    string str = begin->str();
    transform(str.begin(), str.end(), str.begin(), ::tolower);
    ret.push_back(str);
    begin++;
  }

  return ret;
}


vector< vector<string> > Utils::readInTrainingSentences(string filePath, int markovOrder) {
  vector<vector<string> > data;

  ifstream file;
  stringstream buffer;
  file.open(filePath, ios::in);

  if (file.is_open()) {
    buffer << file.rdbuf();
  } else {
    printf("ERROR::No file was found at %s\n", filePath.c_str());  // TODO: throw error
  }
  file.close();

  // Split the line along delimiters for sentences
  vector<string> sentences = Utils::splitAndLower(buffer.str(), ".?!");

  // Split up words in sentences
  data.reserve(sentences.size());
  for (const string &sentence : sentences) {
    vector<string> words = Utils::split(sentence, "\\s,#@$%&;:\"\\(\\)1234567890");

    // Handle (most) contractions
    for (int i = 0; i < words.size(); i++) {
      if (i == 0) continue;

      if (words[i].find('\'') != string::npos) {
        words[i - 1].append(words[i]);
        words.erase(words.begin() + i);
      }
    }

    // Combine two words to increase the markov order
    vector<string> combinedWords;
    combinedWords.reserve(words.size() / markovOrder);
    for (int i = 0; i < words.size(); i += markovOrder) {
      string word;
      for (int j = i; j < words.size() && j < i + markovOrder; j++) {
        word += words[j] + " ";
      }
      word.pop_back(); // remove extra space

      combinedWords.push_back(word);
    }

    data.push_back(combinedWords);
  }

  return data;
}


//  // Simple implementation (Gutenburg samples)
//  ifstream file;
//  stringstream buffer;
//  file.open(filePath, ios::in);
//
//  if (file.is_open()) {
//    buffer << file.rdbuf();
//  } else {
//    printf("ERROR::No file was found at %s\n", filePath.c_str());  // TODO: throw error
//  }
//  // Split the line along delimiters for sentences
//  vector<string> sentences = Utils::splitAndLower(buffer.str(), ".?!");
//
//  // Split up words in sentences
//  for (const string &sentence : sentences) {
//    data.push_back(Utils::split(sentence, "\\s,;:\"\\(\\)"));
//  }

//  file.close();
