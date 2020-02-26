#ifndef MNEMONIC_MARKOV_H
#define MNEMONIC_MARKOV_H

#include "constrainedmarkov.h"

class MnemonicMarkovModel : public ConstrainedMarkovModel {
public:
  MnemonicMarkovModel();

  MnemonicMarkovModel(MarkovModel markovModel, string constraint, Options options);

  ~MnemonicMarkovModel() {};


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
   * @param constraintSequence sequence of constraints
   * @author Porter Glines 1/21/19
   */
  void applyConstraints(vector<string> constraintSequence);
};

#endif