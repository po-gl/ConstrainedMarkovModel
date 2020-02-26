#include "markov.h"

template<class Archive>
void MarkovModel::serialize(Archive &ar, const unsigned int /*version*/) {
  ar & this->markovOrder;
  ar & this->trainingSequences;
  ar & this->transitionProbs;
}