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


void MnemonicMarkovModel::applyConstraints(string constraint) {
  // TODO: Specific NHMM class for specific problem; implementing just this method

  // Constraints:
  // - First letter matches constraint string
  // - Words must be longer than 4 characters
  // - Last word must proceed an <<END>>

  bool proceedsEnd = false;

  for (int i = 0; i < constraint.size(); i++) {

    // auto matrix = transitionMatrices[i];
    for (auto node = transitionMatrices[i].begin(); node != transitionMatrices[i].end();) {
      auto test = node->second;

      if (i == constraint.size() - 1) {  // constraint for end node
        // Set to true if <<END>> is found in the node's proceeding map
        proceedsEnd = (node->second.find(END) != node->second.end());
      }

      // Remove nodes that don't satisfy the constraint 
      // (letter in constraint string == first letter of word)
//      if (constraint[i] != node->first[0] || node->first.size() < 3 || (!proceedsEnd && i == constraint.size())) {
      if (constraint[i] != node->first[0]) {
        node = transitionMatrices[i].erase(node);
      } else {
        node++;
      }
    }
  }
}


// TODO: Move this to Utils and pass sentences in Model constructor
vector< vector<string> > MnemonicMarkovModel::readInTrainingSentences(string filePath) {
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
    data.push_back(Utils::split(sentence, "\\s,#@$%&;:\"\\(\\)1234567890"));
  }

  // Handle (most) contractions
  for (vector<string> &sentence : data) {
    for (int i = 0; i < sentence.size(); i++) {
      if (i == 0) continue;

      if (sentence[i].find('\'') != string::npos) {
        sentence[i-1].append(sentence[i]);
        sentence.erase(sentence.begin()+i);
      }
    }
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