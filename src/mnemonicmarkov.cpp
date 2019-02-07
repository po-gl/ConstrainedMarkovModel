#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <random>

#include "mnemonicmarkov.h"
#include "utils.h"

using namespace std;

MnemonicMarkovModel::MnemonicMarkovModel() {
  // Initialize random
  random_device rd;
  randGenerator = mt19937(rd());
  randDistribution = uniform_real_distribution<double>(0.0, 1.0);
}


void MnemonicMarkovModel::applyConstraints(vector<string> constraintSentence, int markovOrder) {

  // Constraints:
  // + First letter matches constraint string
  // + Words must be certain length
  // + Every other word must be certain length (last must be long)
  // + Last word must proceed an <<END>>
  // - Words must match first part of constraint if possible (relaxed)

  bool firstCharMatches;
  bool wordLengthMet;
  bool proceedsEndSuitable;
  bool proceedsEnd = false;

  int wordLen = 5;

  int debugCount = 0;
  int debugCountTot = 0;

  for (int i = 0; i < constraintSentence.size(); i+=markovOrder) {
    int m = ceil((double)i / markovOrder);
    string constraintWord = constraintSentence[i];
    printf("DEBUG:: constraintWord: %s\n", constraintWord.c_str());



    // auto matrix = transitionMatrices[i];
    for (auto node = transitionMatrices[m].begin(); node != transitionMatrices[m].end();) {
      vector<string> nodeSequence = Utils::split(node->first, "\\s");  // split on spaces
      auto testfirst = node->first;
      auto testsecond = node->second;

      if (nodeSequence.size() < markovOrder) {
        node = transitionMatrices[m].erase(node);
        debugCount++;
        continue;
      }

      firstCharMatches = true;
      for (int j = i, k = 0; j < constraintSentence.size() && k < nodeSequence.size(); j++, k++) {
        if (constraintSentence[j][0] != nodeSequence[k][0]) {
          firstCharMatches = false;
          break;
        }
      }


      wordLengthMet = true;
      for (int j = i, k = 0; j < constraintSentence.size() && k < nodeSequence.size(); j++, k++) {
        if (nodeSequence[k].size() < wordLen) {
          wordLengthMet = false;
          break;
        }
      }

      proceedsEndSuitable = (i == constraintSentence.size() - 1);
      // Set to true if <<END>> is found in the node's proceeding map
      if (proceedsEndSuitable) {  // constraint for end node
        proceedsEnd = (node->second.find(END) != node->second.end());
      }

      // Remove nodes that don't satisfy the constraint
      // (letter in constraint string == first letter of word)
//      if (!firstCharMatches || !wordLengthMet || (!proceedsEnd && proceedsEndSuitable)) {
//      if (!firstCharMatches || !wordLengthMet) {
      if (!firstCharMatches) {
        node = transitionMatrices[m].erase(node);
        debugCount++;
      } else {
        node++;
      }
      debugCountTot++;
    }
  }

  printf("DEBUG:: Removed nodes: %d / %d\n", debugCount, debugCountTot); // 534472/549018
}


// TODO: Move this to Utils and pass sentences in Model constructor
vector< vector<string> > MnemonicMarkovModel::readInTrainingSentences(string filePath, int markovOrder) {
  vector< vector<string> > data;

  ifstream file;
  stringstream buffer;
  file.open(filePath, ios::in);

  if (file.is_open()) {
    buffer << file.rdbuf();
  } else {
    printf("ERROR::No file was found at %s\n", filePath.c_str());  // TODO: throw error
  }

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
        words[i-1].append(words[i]);
        words.erase(words.begin()+i);
      }
    }

    // Combine two words to increase the markov order
    vector<string> combinedWords;
    combinedWords.reserve(words.size()/markovOrder);
    for (int i = 0; i < words.size(); i+=markovOrder) {
      string word;
      for (int j = i; j < words.size() && j < i+markovOrder; j++) {
        word += words[j] + " ";
      }
      word.pop_back(); // remove extra space

      combinedWords.push_back(word);
    }

    data.push_back(combinedWords);
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
  return data;
}