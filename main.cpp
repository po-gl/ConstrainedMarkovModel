#include <iostream>
#include "constrainedmarkov.h"

using namespace std;


void printHelp() {
  printf("usage: markov [--debug] training_text\n");
}


int main(int argc, char *argv[]) {

  // Set default values
  bool debug = false;
  string trainingFilePath;

  // Parse Arguments
  for (int i = 1; i < argc; i++) {
    if (strncmp(argv[i], "--debug", 7) == 0) {  // Debug flag
      debug = true;
    } else if (strncmp(argv[i], "help", 4) == 0) {
      printHelp();
    } else {
      trainingFilePath = argv[i];
    }
  }

  if (trainingFilePath.empty()) {
    printf("Training text is needed.\n");
    printHelp();
    return 0;
  }


  ConstrainedMarkovModel model;
  model.train(trainingFilePath);

   printf("Generated Sentence:\n    ");
   for (string word : model.generateSentence()) {
     printf("%s ", word.c_str());
   }
   printf("\n\n");
  return 0;
}