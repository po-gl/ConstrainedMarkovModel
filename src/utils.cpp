#include <string>
#include <vector>
// #include <regex>
#include <boost/regex.hpp>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <stdio.h>
#include <sys/stat.h>

#include "utils.h"

using namespace std;

const vector<string> STOP_WORDS = { "ourselves", "hers", "between", "yourself", "but", "again", "there", "about", "once", "during", "out", "very", "having", "with", "they", "own", "an", "be", "some", "for", "do", "its", "yours", "such", "into", "of", "most", "itself", "other", "off", "is", "s", "am", "or", "who", "as", "from", "him", "each", "the", "themselves", "until", "below", "are", "we", "these", "your", "his", "through", "don", "nor", "me", "were", "her", "more", "himself", "this", "down", "should", "our", "their", "while", "above", "both", "up", "to", "ours", "had", "she", "all", "no", "when", "at", "any", "before", "them", "same", "and", "been", "have", "in", "will", "on", "does", "yourselves", "then", "that", "because", "what", "over", "why", "so", "can", "did", "not", "now", "under", "he", "you", "herself", "has", "just", "where", "too", "only", "myself", "which", "those", "i", "after", "few", "whom", "t", "being", "if", "theirs", "my", "against", "a", "by", "doing", "it", "how", "further", "was", "here", "than" };


vector< vector<string> > Utils::splitAll(string str, string sentenceDelims, string wordDelims) {
  vector< vector<string> > ret;

  boost::regex sentenceExp("[^" + sentenceDelims + "]+");
  auto begin = boost::sregex_iterator(str.begin(), str.end(), sentenceExp);
  auto end = boost::sregex_iterator();

  boost::regex wordExp("[^" + wordDelims + "]+");
  boost::regex_iterator<string::const_iterator> sentenceBegin;
  boost::regex_iterator<string::const_iterator> sentenceEnd = boost::sregex_iterator();

  string sentence;
  vector<string> workingSentence;
  while (begin != end) {
//    auto debugTime = clock();
    sentence = begin->str();

    sentenceBegin = boost::sregex_iterator(sentence.begin(), sentence.end(), wordExp);

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

  boost::regex wordsExp("[^" + delims + "]+");
  auto begin = boost::sregex_iterator(str.begin(), str.end(), wordsExp);
  auto end = boost::sregex_iterator();

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

  boost::regex wordsExp("[^" + delims + "]+");
  auto begin = boost::sregex_iterator(str.begin(), str.end(), wordsExp);
  auto end = boost::sregex_iterator();

  while (begin != end) {
    string str = begin->str();
    transform(str.begin(), str.end(), str.begin(), ::tolower);
    ret.push_back(str);
    begin++;
  }

  return ret;
}

string Utils::cleanConstraint(string constraint) {
  // Return the same constraint if a space is found
  for (int i = 0; i < constraint.size(); i++) {
    if (constraint[i] == ' ') {
      return constraint;
    }
  }

  string newConstraint;
  // Else split constraint into individual letters
  for (const auto &letter : constraint) {
    newConstraint += letter;
    newConstraint += " ";
  }
  newConstraint.erase(newConstraint.size()-1);
  return newConstraint;
}

string Utils::readInTrainingSentences(string filePath) {
  ifstream file;
  stringstream buffer;
  file.open(filePath, ios::in);

  if (file.is_open()) {
    buffer << file.rdbuf();
    file.close();
  } else {
    printf("ERROR::No file was found %s\n", filePath.c_str());  // TODO: throw error
    exit(-1);
  }

  return buffer.str();
}


vector<vector<string> > Utils::processTrainingSentences(string text, int trainingSentenceLimit, int markovOrder) {
  vector<vector<string> > data;

  // Split the line along delimiters for sentences
  vector<string> sentences = Utils::splitAndLower(text, ".?!");

  // Split up words in sentences
  data.reserve(sentences.size());
  for (const string &sentence : sentences) {
    // TODO: attach part of speech to word

    vector<string> words = Utils::split(sentence, "\\s,#@$%&;:\"\\(\\)0-9");
//    vector<string> words = Utils::split(sentence, "\\s0-9");

    for (int i = 0; i < words.size(); i++) {
      if (i == 0) continue;

      // Remove specific elements
      if (words[i] == "<p>") {
        words.erase(words.begin() + i);
        continue;
      }

      // Handle (most) contractions
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

  if (trainingSentenceLimit != 0) { // if there is a sentence limit
    vector<vector<string> > newData(trainingSentenceLimit);
    std::copy(data.begin(), data.begin() + trainingSentenceLimit, newData.begin());
    data = newData;
  }

  return data;
}


string Utils::getBasename(string filePath) {
  string basename;
  boost::regex directoryExp("[^/]+");
  auto begin = boost::sregex_iterator(filePath.begin(), filePath.end(), directoryExp);
  auto end = boost::sregex_iterator();

  while (begin != end) {
    basename = begin->str();
    begin++;
  }
  return basename;
}


bool Utils::isStopWord(string word) {
  return std::find(STOP_WORDS.begin(), STOP_WORDS.end(), word) != STOP_WORDS.end();
}