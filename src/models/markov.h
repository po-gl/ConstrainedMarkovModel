#ifndef MARKOV_H
#define MARKOV_H

#include <string>
#include <vector>
#include <unordered_map>
#include <random>
#include <boost/serialization/access.hpp>

#include "../options.h"

using namespace std;


/**
 * @brief Markov Model
 */
class MarkovModel {
public:
  
  MarkovModel();

  MarkovModel(Options options);

  ~MarkovModel() {};

  /**
   * @brief Train the markov model using training sentences
   * 
   * Reads in the training text at the given filePath and increments
   * the transition probabilities while iterating over words.
   * 
   * @param trainingSequences vector of sentences to train on
   * @param markovOrder specifies the markov order of the model (the lookahead distance)
   * @author Porter Glines 1/13/19
   */
  void train(vector< vector<string> > trainingSequences, int markovOrder = 1);

  /**
   * @brief Generates a sentence
   * 
   * @return vector<string> array of words making up a sentence
   * @author Porter Glines 1/13/19
   */
  vector<string> generateSentence(int length);

  /**
   * @brief Get the probability of a specific sentence being generated
   * 
   * multiplies the probabilities between each word to get the total
   * probability of a sentence
   * 
   * @param sentence generated sentence
   * @return double probability of the given sentence
   */
  double getSentenceProbability(vector<string> sentence);

  /**
   * @brief Print the transition probabilities for debugging
   */
  void printTransitionProbs();

  /**
   * @brief Get the markov order
   * @return int markov order
   * @author Porter Glines 5/5/19
   */
  int getMarkovOrder() { return this->markovOrder; }

  /**
   * @brief Get the training sequences
   * @return training sequences
   * @author Porter Glines 5/5/19
   */
  vector< vector<string> > getTrainingSequences() { return this->trainingSequences; }

  /**
   * @brief Get the probability matrix
   * @return unordered_map< string, unordered_map<string, double> > transition probabilities
   * @author Porter Glines 5/5/19
   */
  unordered_map< string, unordered_map<string, double> > getProbabilityMatrix() { return this->transitionProbs; }

protected:
  /// Marker representing the start of a sentence
  string START = "<<START>>";
  /// Marker representing the end of a sentence
  string END = "<<END>>";

  /// Specifies the markov order (lookahead distance) for the model
  int markovOrder;

  int sentenceLength;

  /// Random generator
  mt19937 randGenerator;
  /// Random distribution used by the generator
  uniform_real_distribution<double> randDistribution;

private:
  /// Transition probability matrices mapping words -> (word, prob), (word, prob)...
  unordered_map< string, unordered_map<string, double> > transitionProbs;

  vector< vector<string> > trainingSequences;

  friend class boost::serialization::access;

  template<class Archive>
  void serialize(Archive &ar, const unsigned int /*version*/);
  // template<class Archive>
  // void serialize(Archive &ar, const unsigned int /*version*/) {
  //   ar & this->markovOrder;
  //   ar & this->trainingSequences;
  //   ar & this->transitionProbs;
  // }

  /**
   * @brief Get the next word in a sentence given the previous word
   * 
   * Adheres to the markov property
   * 
   * @param prevWord previous word
   * @return string next word generated
   */
  string getNextWord(const string& prevWord);

  /**
   * @brief Calculate the probability of a sentence
   * 
   * @param sentence sentence consisting of words
   * @return double probability of sentence
   * @author Porter Glines 1/26/19
   */
  double calculateProbability(vector<string> sentence);


  /**
   * @brief Calculate the frequencies of words
   * 
   * The frequencies can be used as the prior probabilities
   * 
   * @param sentences training sentences
   * @return unordered_map<string, int> Frequences map (word, frequency)
   * @author Porter Glines 1/26/19
   */
  unordered_map<string, int> getWordFrequencies(vector< vector<string> > sentences);

  /**
   * @brief Normalize a map to have values from 0 to 1
   *
   * @param map to normalize
   * @author Porter Glines 1/16/19
   */
  void normalize(unordered_map<string, double> &map);

  /**
   * @brief Increment the probability in the probability matrix for a word given its next word
   * 
   * @param transitionProbs transition probability matrix mapping words -> (word, prob), (word, prob)...
   * @param word current word
   * @param nextWord next word
   */
  void increment(unordered_map< string, unordered_map<string, double> > &transitionProbs, string word, const string& nextWord);
};

#include "markov.inl"

#endif