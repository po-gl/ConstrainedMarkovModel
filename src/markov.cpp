#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <random>
#include <time.h>

#include "utils.h"
#include "options.h"
#include "console.h"
#include "markov.h"

using namespace std;

// TODO: Templates for non-string use cases

MarkovModel::MarkovModel() {
  // Initialize random
  random_device rd;
  randGenerator = mt19937(rd());
  randDistribution = uniform_real_distribution<double>(0.0, 1.0);

  this->markovOrder = 0;
  this->trainingSequences = vector< vector<string> >();
  this->transitionProbs = unordered_map< string, unordered_map<string, double> >();
}


MarkovModel::MarkovModel(Options options) {
  // Initialize random
  random_device rd;
  randGenerator = mt19937(rd());
  randDistribution = uniform_real_distribution<double>(0.0, 1.0);

  this->markovOrder = 0;
  this->trainingSequences = vector< vector<string> >();
  this->transitionProbs = unordered_map< string, unordered_map<string, double> >();

  time_t startTime; // used for debug timing

    // Read/Pre-process training sequences
  vector< vector<string> > trainingSequences;
  if (options.getUseCache()) {
    // Read in training sentences from cache
    startTime = clock();
    Utils::readFromCache(*this, Utils::getBasename(options.getTrainingFilePath()));
    Console::debugPrint("%-35s: %f\n", "Elapsed Time Reading From Cache", (float) (clock() - startTime) / CLOCKS_PER_SEC);
  }

  // TODO: Rebuild cache reading it fails or if markov order is different
  // Read/Process/Train model
  if (this->getProbabilityMatrix().empty()){
    if (options.getUseCache())
      Console::debugPrint("No cache found for file.\n");

    // Read in training sentences
    startTime = clock();
    string trainingText = Utils::readInTrainingSentences(options.getTrainingFilePath());
    Console::debugPrint("%-35s: %f\n", "Elapsed Time Reading Data", (float) (clock() - startTime) / CLOCKS_PER_SEC);

    // Process training sentences
    startTime = clock();
    trainingSequences = Utils::processTrainingSentences(trainingText, options.getMarkovOrder());
    Console::debugPrint("%-35s: %f\n", "Elapsed Time Processing Data", (float) (clock() - startTime) / CLOCKS_PER_SEC);

    this->train(trainingSequences, options.getMarkovOrder());

    if (options.getUseCache())
      // Write to cache
      Utils::writeToCache(this, Utils::getBasename(options.getTrainingFilePath()));
  }
}


void MarkovModel::train(vector< vector<string> > trainingSequences, int markovOrder) {

  this->markovOrder = markovOrder;  // default parameter = 1
  this->trainingSequences = trainingSequences;

  // Clear model data structures
  transitionProbs.clear();

  // iterate over sentences
  for (vector<string> sentence : trainingSequences) {
    // iterate over sentence words
    for (int i = 0; i < sentence.size(); i++) {
      string prevWord = (i == 0) ? START : sentence[i - 1];
      string currWord = sentence[i];

      this->increment(transitionProbs, prevWord, currWord);
    }
    if (!sentence.empty()) {
      this->increment(transitionProbs, sentence[sentence.size() - 1], END);
    }
  }

  // Normalize all probabilities from 0.0-1.0
  for (auto &pairInnerMap : transitionProbs) {
      this->normalize(pairInnerMap.second);
  }
}


void MarkovModel::increment(unordered_map< string, unordered_map<string, double> > &transitionProbs, string word, const string& nextWord) {

  auto transition = transitionProbs.emplace(word, unordered_map<string, double>());
  unordered_map<string, double> *innerMap = &transition.first->second;

  auto foundProb = innerMap->find(nextWord);
  if (foundProb != innerMap->end()) {
    foundProb->second += 1.0;
  } else {
    innerMap->insert(make_pair(nextWord, 1.0));
  }
}


void MarkovModel::normalize(unordered_map<string, double> &map) {
  double sum = 0.0;
  for (const auto &pair : map) {
    sum += pair.second;
  }

  for (const auto &pair : map) {
    map[pair.first] = map[pair.first] / sum;
  }
}


vector<string> MarkovModel::generateSentence(int length) {

  if (transitionProbs.empty()) {
    printf("ERROR::Model is not trained.\n");  // TODO: throw error
    return vector<string>();
  }

  vector<string> sentence;

  string nextWord = getNextWord(START);
  sentence.push_back(nextWord);

  string prevWord = nextWord;
  for (int i = 1; i < length; i++) {
    nextWord = getNextWord(prevWord);
    sentence.push_back(nextWord);
    prevWord = nextWord;
  }

  return sentence;
}


double MarkovModel::getSentenceProbability(vector<string> sentence) {
  double prob = 1.0;

  string currWord;
  string nextWord;

  for (int i = 0; i < sentence.size(); i++) {
    if (i == 0) {
      currWord = START;
      nextWord = sentence[i];
    } else {
      currWord = sentence[i-1];
      nextWord = sentence[i];
    }

    double p = this->transitionProbs[currWord][nextWord];
    if (p != 0)
        prob *= p;
  }
  return prob;
}


string MarkovModel::getNextWord(const string& prevWord) {
  unordered_map<string, double> map = this->transitionProbs[prevWord];
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


double MarkovModel::calculateProbability(vector<string> sentence) {
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


unordered_map<string, int> MarkovModel::getWordFrequencies(vector< vector<string> > sentences) {
  unordered_map<string, int> frequencies;

  for (const auto &sentence : sentences) {
    for (const auto &word : sentence) {
      frequencies[word]++;
    }
  }
  return frequencies;
}


void MarkovModel::printTransitionProbs() {
  for (const auto &firstWord : this->transitionProbs) {
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
