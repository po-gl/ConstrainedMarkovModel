#include <iostream>
#include "constrainedmarkov.h"

using namespace std;

// TODO: Use proper data instead of Gutenburg books


void printHelp() {
  printf("usage: markov [--debug] -c constraint training_text\n");
}


int main(int argc, char *argv[]) {

  // Set default values
  bool debug = false;
  string constraint;
  string trainingFilePath;

  // Parse Arguments
  for (int i = 1; i < argc; i++) {
    if (strncmp(argv[i], "--debug", 7) == 0) {  // Debug flag
      debug = true;
    } else if (strncmp(argv[i], "help", 4) == 0) {
      printHelp();
      return 0;
    } else if (strncmp(argv[i], "-c", 2) == 0) {
      if (i+1 < argc) {
        constraint = argv[++i];
      }
    } else {
      trainingFilePath = argv[i];
    }
  }

  if (trainingFilePath.empty()) {
    printf("Training text is needed.\n");
    printHelp();
    return 0;
  } else if (constraint.empty()) {
    printf("Constraint is needed.\n");
    printHelp();
    return 0;
  }


  ConstrainedMarkovModel model;
  model.train(trainingFilePath, constraint);

  vector<string> sentence = model.generateSentence();

  printf("Generated Sentence:\n    ");
  for (string word : sentence) {
    printf("%s ", word.c_str());
  }
  printf("\n");
  printf("Sentence probability: %f\n\n", model.getSentenceProbability(sentence));
  return 0;
}