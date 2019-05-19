#include <string>
#include <vector>
#include <regex>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <stdio.h>
#include <sys/stat.h>

#include "utils.h"

using namespace std;


vector< vector<string> > Utils::splitAll(string str, string sentenceDelims, string wordDelims) {
  vector< vector<string> > ret;

  regex sentenceExp("[^" + sentenceDelims + "]+");
  auto begin = sregex_iterator(str.begin(), str.end(), sentenceExp);
  auto end = sregex_iterator();

  regex wordExp("[^" + wordDelims + "]+");
  regex_iterator<string::const_iterator> sentenceBegin;
  regex_iterator<string::const_iterator> sentenceEnd = sregex_iterator();

  string sentence;
  vector<string> workingSentence;
  while (begin != end) {
//    auto debugTime = clock();
    sentence = begin->str();

    sentenceBegin = sregex_iterator(sentence.begin(), sentence.end(), wordExp);

    workingSentence = vector<string>();
//    workingSentence.reserve(16);

    while (sentenceBegin != sentenceEnd) {
      workingSentence.push_back(sentenceBegin->str());
      sentenceBegin++;
    }
//    printf("%35s: %f\n", "Elapsed Time on sentence", (float) (clock() - debugTime) / CLOCKS_PER_SEC);
    ret.push_back(workingSentence);
    begin++;
  }
  return ret;
}


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


// Idea: read in sentences into vectors as fast as possible first, then thread remove non-words


string Utils::readInTrainingSentences(string filePath) {
  ifstream file;
  stringstream buffer;
  file.open(filePath, ios::in);

  if (file.is_open()) {
    buffer << file.rdbuf();
    file.close();
  } else {
    printf("ERROR::No file was found at %s\n", filePath.c_str());  // TODO: throw error
  }

  return buffer.str();
}


vector<vector<string> > Utils::processTrainingSentences(string text, int markovOrder) {
  vector<vector<string> > data;

  // Split the line along delimiters for sentences
  vector<string> sentences = Utils::splitAndLower(text, ".?!");

  // Split up words in sentences
  data.reserve(sentences.size());
  for (const string &sentence : sentences) {
    // TODO: attach part of speech to word

    vector<string> words = Utils::split(sentence, "\\s,#@$%&;:\"\\(\\)0-9");
//    vector<string> words = Utils::split(sentence, "\\s0-9");

    // Handle (most) contractions
    for (int i = 0; i < words.size(); i++) {
      if (i == 0) continue;

      if (words[i].find('\'') != string::npos) {
        words[i - 1].append(words[i]);
        words.erase(words.begin() + i);
      }
    }

    // Combine words to increase the markov order
    if (markovOrder > 1) {
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
    } else {
      data.push_back(words);
    }
  }
  return data;
}


string Utils::getBasename(string filePath) {
  string basename;
  regex directoryExp("[^/]+");
  auto begin = sregex_iterator(filePath.begin(), filePath.end(), directoryExp);
  auto end = sregex_iterator();

  while (begin != end) {
    basename = begin->str();
    begin++;
  }
  return basename;
}
