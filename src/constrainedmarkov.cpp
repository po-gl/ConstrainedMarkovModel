#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <random>
#include <time.h>

#include "constrainedmarkov.h"
#include "utils.h"

using namespace std;

// TODO: add better constraints (POS)

// TODO: NHMM should work with words, not sentences >>> or, constraints could be added for a "beginning" and "end" to a mnemonic

// TODO: Templates for non-string use cases

void ConstrainedMarkovModel::train(string filePath, vector<string> constraint, int markovOrder) {

  bool debug = true; // TODO: Move timing prints to debug class
  time_t startTime;

  this->markovOrder = markovOrder;  // default parameter = 1
  this->sentenceLength = (int)constraint.size();

  if (debug)
      startTime = clock();

  this->trainingSequences = readInTrainingSentences(filePath, markovOrder);

  if (debug)
      printf("%35s: %f\n", "Elapsed Time Reading Data", (float)(clock() - startTime)/CLOCKS_PER_SEC);


  // iterate over sentences
  for (vector<string> sentence : this->trainingSequences) {
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

  // copy matrices for each word (note that START is added later, see addStartTransition())
  for (int i = 0; i < ceil(((double)sentenceLength) / markovOrder); i++) {
    transitionMatrices.push_back(transitionProbs);
  }


  if (debug)
      startTime = clock();

  // Apply constraint by removing nodes that violate the constraint
  applyConstraints(constraint, markovOrder);


  if (debug)
      printf("%35s: %f\n", "Elapsed Time Applying Constraints", (float)(clock() - startTime)/CLOCKS_PER_SEC);

  if (debug)
      startTime = clock();

  // Enforce arc-consistency
  removeDeadNodes();

  if (debug)
      printf("%35s: %f\n", "Elapsed Time Removing Nodes", (float)(clock() - startTime)/CLOCKS_PER_SEC);


  if (debug)
    startTime = clock();

  // Add in start transition matrices
  addStartTransition();

  if (debug)
    printf("%35s: %f\n", "Elapsed Time Adding Start Matrix", (float)(clock() - startTime)/CLOCKS_PER_SEC);


  if (debug)
    startTime = clock();

  // Normalize as described in Pachet's paper
  normalize();

  if (debug)
    printf("%35s: %f\n", "Elapsed Time Normalizing", (float)(clock() - startTime)/CLOCKS_PER_SEC);
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


// TODO FIX: The last transition table doesn't have any nodes removed; they are ignored when generating, but shouldn't be there?
void ConstrainedMarkovModel::removeDeadNodes() {
  // Enforce arc-consistency
  for (int i = (int)transitionMatrices.size() - 1; i > 0; i--) {

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
      if (prevWordMap->empty()) {
        iter = prevWordMatrix->erase(iter);
      } else {
        iter++;
      }
    }
  }
}

void ConstrainedMarkovModel::normalize() {
  // We first normalize individually the last matrix (Pachet) **CITE
  double prevSum;

  vector< unordered_map<string, double> > aSums;
  aSums.reserve(transitionMatrices.size());
  for (int i = (int)transitionMatrices.size() - 1; i >= 0; i--) {
    aSums.emplace_back(unordered_map<string, double>());
  }

  for (int i = (int)transitionMatrices.size() - 1; i >= 0; i--) {

    auto *transitionMap = &transitionMatrices[i];


    for (auto &map : *transitionMap) {
      auto *innerMap = &map.second;
      double sumA = 0.0;

      // Normalize for the last transition matrix
      if (i == (int)transitionMatrices.size() - 1) {
        // normalize in a normal fashion
        for (const auto &pair : *innerMap) {
          sumA += pair.second;  // update sumA
        }

      // Normalize for 0 to L-1 transition matrices
      } else {
        for (const auto &pair : *innerMap) {
          prevSum = aSums[i+1][pair.first];
          sumA += prevSum * pair.second;  // update sumA
        }
      }

      aSums[i][map.first] = sumA;  // save sums for later use

      for (const auto &pair : *innerMap) {
        if (i == transitionMatrices.size() - 1) {
          // normalize individually for the last matrix
          innerMap->at(pair.first) /= sumA;
        } else {
          prevSum = aSums[i+1][pair.first];
          // normalize in a propagating manor for the middle and first matrices
          innerMap->at(pair.first) = innerMap->at(pair.first) * prevSum / sumA;
        }
      }
    }
  }
}


void ConstrainedMarkovModel::addStartTransition() {
  // Word frequencies are used as the prior probabilities
  unordered_map<string, int> wordFrequencies = getWordFrequencies(this->trainingSequences);

  // create new matrix with start as the only node to all the other transitionMatrices[1] firsts
  // then insert the new start matrix at the front of transitionMatrices
  unordered_map< string, unordered_map<string, double> > startTransition;

  unordered_map<string, double> innerStartMap;
  // transitionMatrices[0] represents the possible starting words (not START yet)
  for (const auto &map : transitionMatrices[0]) {
    // starting probabilities determined frequency
    innerStartMap.insert(make_pair(map.first, wordFrequencies[map.first]));
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
  for (int i = 1; i < transitionMatrices.size() - 1; i++) {
    nextWord = getNextWord(prevWord, i);
    sentence.push_back(nextWord);
    prevWord = nextWord;
  }

  return sentence;
}


double ConstrainedMarkovModel::getSentenceProbability(vector<string> sentence) {
  double prob = 1.0;

  string currWord;
  string nextWord;

  for (int i = 0; i < transitionMatrices.size(); i++) {
    auto currentMatrix = transitionMatrices[i];
    if (i == 0) {
      currWord = START;
      nextWord = sentence[i];
    } else {
      currWord = sentence[i-1];
      nextWord = sentence[i];
    }

    prob *= currentMatrix[currWord][nextWord];
  }
  return prob;
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


unordered_map<string, int> ConstrainedMarkovModel::getWordFrequencies(vector< vector<string> > sentences) {
  unordered_map<string, int> frequencies;

  for (const auto &sentence : sentences) {
    for (const auto &word : sentence) {
      frequencies[word]++;
    }
  }
  return frequencies;
}


vector<int> ConstrainedMarkovModel::getTransitionMatricesSizes() {
  vector<int> sizes;

  sizes.reserve(transitionMatrices.size());
  for (int i = 0; i < transitionMatrices.size(); i++) {
    sizes.push_back((int)transitionMatrices[i].size());
  }
  return sizes;
}


void ConstrainedMarkovModel::printTransitionProbs() {
  for (const auto &matrix : transitionMatrices) {
    printf("|------------------------------------------------------------------|\n");
    printf("|==================================================================|\n");
    printf("|------------------------------------------------------------------|\n");
    for (const auto &firstWord : matrix) {
      printf("%20s >>> ", firstWord.first.c_str());
      double sum = 0.0;
      for (const auto &nextWord : firstWord.second) {
        printf("%s:(%0.3f) ", nextWord.first.c_str(), nextWord.second);
        sum += nextWord.second;
      }
      printf(" sum: >%f<", sum);
      printf("\n");
    }
  }
}
