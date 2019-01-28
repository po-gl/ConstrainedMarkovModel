#ifndef CONSTRAINED_MARKOV_H
#define CONSTRAINED_MARKOV_H

#include <string>
#include <vector>
#include <unordered_map>
#include <random>

using namespace std;


/**
 * @brief Constrained Markov Model
 */
class ConstrainedMarkovModel {
public:
  
  ConstrainedMarkovModel() {};
  ~ConstrainedMarkovModel() {};

  /**
   * @brief Train the constrained markov model using training sentences
   * 
   * Reads in the training text at the given filePath and increments
   * the transition probabilities while iterating over words.
   * 
   * Removes probabilities that don't satisfy the constraint, then
   * backpropagates through the matrices to remove probabilties that
   * end before the word length is satisfied
   * 
   * Finally normalizes the probabilities
   * 
   * @param filePath path to training text
   * @author Porter Glines 1/13/19
   */
  void train(string filePath, string constraint);


  /**
   * @brief Generates a sentence
   * 
   * @return vector<string> array of words making up a sentence
   * @author Porter Glines 1/13/19
   */
  vector<string> generateSentence();


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
   * @brief Get the length the model has trained on
   * 
   * @return int 
   */
  int getSentenceLength() { return sentenceLength; }

  /**
   * @brief Read in training text into an array of sentences made up of arrays of words
   * 
   * This is a pure virtual function
   * 
   * @param filePath path to training text
   * @return vector< vector<string> > array of sentences made up of arrays of words
   */
  virtual vector< vector<string> > readInTrainingSentences(string filePath) = 0;

  /**
   * @brief Print the transition probabilities for debugging
   */
  void printTransitionProbs();

protected:
  /// Marker representing the start of a sentence
  const string START = "<<START>>";
  /// Marker representing the end of a sentence
  const string END = "<<END>>";

  int sentenceLength;

  /// Random generator
  mt19937 randGenerator;
  /// Random distribution used by the generator
  uniform_real_distribution<double> randDistribution;

  /// Transition probability matrices between words
  vector< unordered_map< string, unordered_map<string, double> > > transitionMatrices;

private:
  /// Stores training sentences used to train the model
  vector< vector<string> > trainingSequences;

  /// Original transition probability matrices mapping words -> (word, prob), (word, prob)...
  unordered_map< string, unordered_map<string, double> > transitionProbs;

  /**
   * @brief Apply constraints to the transition matrices
   * 
   * Modify (delete) nodes int transitionMatrices[] that violate 
   * the constraint rules.
   * 
   * This is a pure virtual function
   */
  virtual void applyConstraints(string constraint) = 0;

  /**
   * @brief Remove nodes that violate arc consistency
   * Should be called after applying constraints and
   * before normalizing
   * 
   * enforces arc-consistency
   * 
   * @author Porter Glines 1/21/19
   */
  void removeDeadNodes();

  /**
   * @brief Adds a transition layer from START to the next layer
   * should be called after all other layers are settled but not
   * before the transition matrices are normalized
   * 
   * @author Porter Glines 1/21/19
   */
  void addStartTransition();

  /**
   * @brief Get the next word in a sentence given the previous word
   * 
   * Adheres to the markov property
   * 
   * @param prevWord previous word
   * @param wordIndex 
   * @return string next word generated
   */
  string getNextWord(string prevWord, int wordIndex);

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
   * @brief Normalize the transitionMatrices according to the method
   * described by Pachet **CITE
   * 
   * The normalized transitionMatrices retains the same probability
   * distribution as the original transitionMatrices but will then
   * be stochastic (each row adding up to 1.0)
   * 
   * @author Porter Glines 1/22/19
   */
  void normalize();

  /**
   * @brief Increment the probability in the probability matrix for a word given its next word
   * 
   * @param transitionProbs transition probability matrix mapping words -> (word, prob), (word, prob)...
   * @param word current word
   * @param nextWord next word
   */
  void increment(unordered_map< string, unordered_map<string, double> > &transitionProbs, string word, string nextWord);
};

#endif