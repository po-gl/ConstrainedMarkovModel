#include <string>
#include <vector>
#include <unordered_map>
#include <random>

using namespace std;


class ConstrainedMarkovModel {
public:
  
  ConstrainedMarkovModel();
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
  void train(string filePath);

  /**
   * @brief Generates a sentence
   * 
   * @return vector<string> array of words making up a sentence
   * @author Porter Glines 1/13/19
   */
  vector<string> generateSentence();

  /**
   * @brief Read in training text into an array of sentences made up of arrays of words
   * 
   * @param filePath path to training text
   * @return vector< vector<string> > array of sentences made up of arrays of words
   */
  vector< vector<string> > readInTrainingSentences(string filePath);

  /**
   * @brief Print the transition probabilities for debugging
   */
  void printTransitionProbs();

private:
  /// Marker representing the start of a sentence
  const string START = "<<START>>";
  /// Marker representing the end of a sentence
  const string END = "<<END>>";

  /// Original transition probability matrices mapping words -> (word, prob), (word, prob)...
  unordered_map< string, unordered_map<string, double> > transitionProbs;

  /// Random generator
  mt19937 randGenerator;
  /// Random distribution used by the generator
  uniform_real_distribution<double> randDistribution;

  /**
   * @brief Get the next word in a sentence given the previous word
   * 
   * Adheres to the markov property
   * 
   * @param prevWord previous word
   * @return string next word generated
   */
  string getNextWord(string prevWord);

  /**
   * @brief Calculate the probability of a sentence
   * 
   * @param sentence sentence consisting of words
   * @return double probability of sentence
   */
  double calculateProbability(vector<string> sentence);

  /**
   * @brief Normalize a map of words and probabilities
   * 
   * @param map unordered_map (word, probability)
   */
  void normalize(unordered_map<string, double> &map);

  /**
   * @brief Increment the probability in the probability matrix for a word given its next word
   * 
   * @param transitionProbs transition probability matrix mapping words -> (word, prob), (word, prob)...
   * @param word current word
   * @param nextWord next word
   */
  void increment(unordered_map< string, unordered_map<string, double> > &transitionProbs, string word, string nextWord);
};