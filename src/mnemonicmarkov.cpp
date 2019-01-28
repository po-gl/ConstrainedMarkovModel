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

  for (int i = 0; i < constraint.size(); i++) {

    // auto matrix = transitionMatrices[i];
    for (auto iter = transitionMatrices[i].begin(); iter != transitionMatrices[i].end();) {
      // Remove nodes that don't satisfy the constraint 
      // (letter in constraint string == first letter of word)
      if (constraint[i] != iter->first[0]) {
        iter = transitionMatrices[i].erase(iter);
      } else {
        iter++;
      }
    }
  }
}


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
  for (const string &sentence : sentences) {
    data.push_back(Utils::split(sentence, "\\s,;:\"\\(\\)"));
  }

  file.close();
  return data;
}