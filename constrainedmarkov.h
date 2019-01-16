#include <string>
#include <vector>
#include <unordered_map>
#include <random>

using namespace std;


class ConstrainedMarkovModel {
public:
  
  ConstrainedMarkovModel();
  ~ConstrainedMarkovModel() {};

  void train(string filePath);

  vector<string> generateSentence();

  vector< vector<string> > readInTrainingSentences(string filePath);

private:
  const string START = "<<START>>";
  const string END = "<<END>>";

  unordered_map< string, unordered_map<string, double> > transitionProbs;

  mt19937 randGenerator;
  uniform_real_distribution<double> randDistribution;

  string getNextWord(string prevWord);

  double calculateProbability(vector<string> sentence);

  void printTransitionProbs(unordered_map< string, unordered_map<string, double> > probs);

  void normalize(unordered_map<string, double> &map);

  void increment(unordered_map< string, unordered_map<string, double> > &transitionProbs, string word, string nextWord);
};