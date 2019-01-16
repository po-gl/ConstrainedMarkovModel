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


ConstrainedMarkovModel::ConstrainedMarkovModel() {
  // Initialize random
  random_device rd;
  randGenerator = mt19937(rd());
  randDistribution = uniform_real_distribution<double>(0.0, 1.0);
}

void ConstrainedMarkovModel::train(string filePath) {
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

  // Normalize all probabilities from 0.0-1.0
  for (auto &pairInnerMap : transitionProbs) {
    normalize(pairInnerMap.second);
  }
}


vector<string> ConstrainedMarkovModel::generateSentence() {
  vector<string> sentence;

  string nextWord = getNextWord(START);
  sentence.push_back(nextWord);

  string prevWord = nextWord;
  while (nextWord != END) {
    nextWord = getNextWord(prevWord);
    sentence.push_back(nextWord);
    prevWord = nextWord;
  }

  return sentence;
}


vector< vector<string> > ConstrainedMarkovModel::readInTrainingSentences(string filePath) {
  vector< vector<string> > data;

  ifstream file;
  stringstream buffer;
  file.open(filePath, ios::in);
  
  if (file.is_open()) {
    buffer << file.rdbuf();
  } else {
    printf("No file was found at %s\n", filePath.c_str());
  }
  // Split the line along delimiters for sentences
  vector<string> sentences = split(buffer.str(), ".?!");

  for (const string &sentence : sentences) {
    data.push_back(split(sentence, "\\s,;:\"\\(\\)"));
  }

  file.close();
  return data;
}


string ConstrainedMarkovModel::getNextWord(string prevWord) {
  unordered_map<string, double> map = transitionProbs[prevWord];
  double randVal = randDistribution(randGenerator);

  double sum = 0.0;
  for (const auto &pair : map) {
    sum += map[pair.first];

    if (sum > randVal) {
      return pair.first;
    }
  }
  return "";
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


void ConstrainedMarkovModel::printTransitionProbs(unordered_map< string, unordered_map<string, double> > probs) {
  for (const auto &firstWord : probs) {
    printf("%20s >>> ", firstWord.first.c_str());
    for (const auto &nextWord : firstWord.second) {
      printf("%s:(%0.3f) ", nextWord.first.c_str(), nextWord.second);
    }
    printf("\n");
  }
}


void ConstrainedMarkovModel::normalize(unordered_map<string, double> &map) {
  double sum = 0.0;
  for (const auto &pair : map) {
    sum += pair.second;
  }

  for (const auto &pair : map) {
    map[pair.first] = map[pair.first] / sum;
  }
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


