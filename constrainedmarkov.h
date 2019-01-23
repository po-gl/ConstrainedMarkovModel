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
  void train(string filePath, string constraint);


  /**
   * @brief Generates a sentence
   * 
   * @return vector<string> array of words making up a sentence
   * @author Porter Glines 1/13/19
   */
  vector<string> generateSentence();

  /**
   * @brief Get the length the model has trained on
   * 
   * @return int 
   */
  int getSentenceLength() { return sentenceLength; }

  // TODO: Move to utils.cpp and make more generic
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

  int sentenceLength;

  /// Original transition probability matrices mapping words -> (word, prob), (word, prob)...
  unordered_map< string, unordered_map<string, double> > transitionProbs;

  /// Transition probability matrices between words
  vector< unordered_map< string, unordered_map<string, double> > > transitionMatrices;

  /// Random generator
  mt19937 randGenerator;
  /// Random distribution used by the generator
  uniform_real_distribution<double> randDistribution;

  // TODO: Move to inherited class and make a abstract in this class
  /**
   * @brief Apply a constraint to the transition matrices by
   * deleting nodes that violate the constraint.
   * 
   * The constraint is given as a string that represents
   * the required first characters in order of chosen
   * words
   * 
   * e.g. constraint="twd" constrains the model to only
   * generate words starting with "T", "W", and "D" such as
   * "The weather door"
   * 
   * @param constraint required first letters
   * @author Porter Glines 1/21/19
   */
  void applyConstraints(string constraint);

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
   */
  double calculateProbability(vector<string> sentence);

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