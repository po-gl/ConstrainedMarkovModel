#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <random>

#include "constrainedmarkov.h"
#include "utils.h"

using namespace std;


// TODO: Templates for non-string use cases

ConstrainedMarkovModel::ConstrainedMarkovModel() {
  // Initialize random
  random_device rd;
  randGenerator = mt19937(rd());
  randDistribution = uniform_real_distribution<double>(0.0, 1.0);
}


void ConstrainedMarkovModel::train(string filePath, string constraint) {
  this->sentenceLength = constraint.size();
  vector< vector<string> > trainingSequences = readInTrainingSentences(filePath); 

  // iterate over sentences
  for (vector<string> sentence : trainingSequences) {
    // iterate over sentence words
    for (int i = 0; i < sentence.size(); i++) {
      string prevWord = (i == 0) ? START : sentence[i - 1];
      string currWord = sentence[i];

      increment(transitionProbs, prevWord, currWord);
    }
    if (!sentence.empty()) {
      increment(transitionProbs, sentence[sentence.size() - 1], END);
    }
  }

  // TODO: Should the Mnemonic train on start and end transition tables? do we care if the sequence of words starts/ends a sentence?

  // copy matrices for each word (note that START is added later, see addStartTransition())
  for (int i = 0; i < sentenceLength; i++) {
    transitionMatrices.push_back(transitionProbs);
  }

  // Apply constraint by removing nodes that violate the constraint
  applyConstraints(constraint);

  // Enforce arc-consistency
  removeDeadNodes();

  // Add in start transition matrices
  addStartTransition();

  // Normalize as described in Pachet's paper
  normalize();
}


void ConstrainedMarkovModel::increment(unordered_map< string, unordered_map<string, double> > &transitionProbs, string word, string nextWord) {

  auto transition = transitionProbs.emplace(word, unordered_map<string, double>());
  unordered_map<string, double> *innerMap = &transition.first->second;

  auto foundProb = innerMap->find(nextWord);
  if (foundProb != innerMap->end()) {
    foundProb->second += 1.0;
  } else {
    innerMap->insert(make_pair(nextWord, 1.0));
  }
}


void ConstrainedMarkovModel::applyConstraints(string constraint) {
  // TODO: Specific NHMM class for specific problem; implementing just this method

  // HARDCODED
  // string constraint = "twd";  // The weather door (smalltest.txt)

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


void ConstrainedMarkovModel::removeDeadNodes() {
  // Enforce arc-consistency
  for (int i = transitionMatrices.size() - 1; i > 0; i--) {

    // TODO: make more efficient and/or clear with graph data structure
    // This is a tree structured CSP, so no backtracking is needed
    // Iterate through the previous word's matrix, looking at the tail

    auto *currWordMatrix = &transitionMatrices[i];
    auto *prevWordMatrix = &transitionMatrices[i-1];
    for (auto iter = prevWordMatrix->begin(); iter != prevWordMatrix->end();) {

      auto *prevWordMap = &iter->second;
      for (auto prevInnerIter = prevWordMap->begin(); prevInnerIter != prevWordMap->end();) {

        // Remove edges that do not lead to the next word
        if (currWordMatrix->find(prevInnerIter->first) == currWordMatrix->end()) {
          prevInnerIter = prevWordMap->erase(prevInnerIter);
        } else {
          prevInnerIter++;
        }
      }

      // Remove a node if there are no edges coming out of it
      if (prevWordMap->size() == 0) {
        iter = prevWordMatrix->erase(iter);
      } else {
        iter++;
      }
    }
  }
}

void ConstrainedMarkovModel::normalize() {
  // We first normalize individually the last matrix (Pachet) **CITE

  double prevSum = 1.0;

  for (int i = transitionMatrices.size() - 1; i >= 0; i--) {

    auto *transitionMap = &transitionMatrices[i];

    for (auto &map : *transitionMap) {

      // normalize in a normal fashion
      auto *innerMap = &map.second;
      double sumZ = 0.0;
      for (const auto &pair : *innerMap) {
        sumZ += pair.second;
      }

      double sumA = sumZ * prevSum;

      for (const auto &pair : *innerMap) {
        if (i == transitionMatrices.size() - 1) {
          // normalize individually for the last matrix
          innerMap->at(pair.first) /= sumZ;
        } else { 
          // normalize in a propagating manor for the middle and first matrices
          innerMap->at(pair.first) = innerMap->at(pair.first) * prevSum / sumA;
        }
      }

      prevSum *= sumZ;
    }
  }
}


void ConstrainedMarkovModel::addStartTransition() {
  // create new matrix with start as the only node to all the other transitionMatrices[1] firsts
  // then insert the new start matrix at the front of transitionMatrices
  unordered_map< string, unordered_map<string, double> > startTransition;

  unordered_map<string, double> innerStartMap;
  for (const auto map : transitionMatrices[0]) {
    // Equal probaility for all starting words
    // TODO: Weight starting probabilities by frequency?
    innerStartMap.insert(make_pair(map.first, 1.0));
  }
  startTransition.insert(make_pair(START, innerStartMap));

  transitionMatrices.insert(transitionMatrices.begin(), startTransition);
}


vector<string> ConstrainedMarkovModel::generateSentence() {

  if (transitionMatrices.empty()) {
    printf("ERROR::Model is not trained.\n");  // TODO: throw error
    return vector<string>();
  }

  vector<string> sentence;

  string nextWord = getNextWord(START, 0);
  sentence.push_back(nextWord);

  string prevWord = nextWord;
  for (int i = 1; i < sentenceLength; i++) {
    nextWord = getNextWord(prevWord, i);
    sentence.push_back(nextWord);
    prevWord = nextWord;
  }

  return sentence;
}


string ConstrainedMarkovModel::getNextWord(string prevWord, int wordIndex) {
  unordered_map<string, double> map = transitionMatrices[wordIndex][prevWord];
  double randVal = randDistribution(randGenerator);

  double sum = 0.0;
  for (const auto &pair : map) {
    sum += map[pair.first];

    if (sum > randVal) {
      return pair.first;
    }
  }
  return "";  // TODO: throw error
}


vector< vector<string> > ConstrainedMarkovModel::readInTrainingSentences(string filePath) {
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
  vector<string> sentences = splitAndLower(buffer.str(), ".?!");

  // Split up words in sentences
  for (const string &sentence : sentences) {
    data.push_back(split(sentence, "\\s,;:\"\\(\\)"));
  }

  file.close();
  return data;
}


double ConstrainedMarkovModel::calculateProbability(vector<string> sentence) {
  double prob = 1.0;
  for (int i = 0; i < sentence.size(); i++) {
    string prevWord;
    if (i == 0) {
      prevWord = START;
    } else {
      prevWord = sentence[i-1];
    }

    string currWord = sentence[i];

    prob *= transitionProbs[prevWord][currWord];
  }
  return prob;
}


void ConstrainedMarkovModel::printTransitionProbs() {
  auto probs = transitionProbs;
  for (const auto &firstWord : probs) {
    printf("%20s >>> ", firstWord.first.c_str());
    for (const auto &nextWord : firstWord.second) {
      printf("%s:(%0.3f) ", nextWord.first.c_str(), nextWord.second);
    }
    printf("\n");
  }
}