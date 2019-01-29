#include <iostream>
#include <time.h>
#include "mnemonicmarkov.h"

using namespace std;

// TODO: Use proper data instead of Gutenburg books


void printHelp() {
  printf("usage: markov [--debug | -d] -c constraint training_text\n");
}


int main(int argc, char *argv[]) {

  // Set default values
  bool debug = false;
  string constraint;
  string trainingFilePath;

  time_t startTime;
  time_t endTime;

  // Parse Arguments
  for (int i = 1; i < argc; i++) {
    // Debug flag
    if (strncmp(argv[i], "--debug", 7) == 0 || strncmp(argv[i], "-d", 2) == 0) {
      debug = true;

    // Help
    } else if (strncmp(argv[i], "help", 4) == 0) {
      printHelp();
      return 0;

    // Constraint flag
    } else if (strncmp(argv[i], "-c", 2) == 0) {
      if (i+1 < argc) {
        constraint = argv[++i];
      }

    // Training file path
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


  MnemonicMarkovModel model;

  startTime = clock();
  model.train(trainingFilePath, constraint);
  endTime = clock();
  time_t trainingTime = endTime - startTime;


  startTime = clock();
  vector<string> sentence = model.generateSentence();
  endTime = clock();
  time_t sentenceGenerationTime = endTime - startTime;

  // Print standard output
  for (string word : sentence) {
      printf("%s ", word.c_str());
  }
  printf("\n");

  if (debug) {  // Print debug information
    printf("\n");
    printf("%45s\n", "=== DEBUG INFORMATION ===");

    printf("%35s: %f\n", "Elapsed Training Time (sec)", (float)trainingTime/CLOCKS_PER_SEC);
    printf("%35s: %f\n", "Elapsed Sentence Gen Time (sec)", (float)sentenceGenerationTime/CLOCKS_PER_SEC);

    printf("%35s: ", "Transition Matrix sizes");
    vector<int> sizes = model.getTransitionMatricesSizes();
    for (auto size : sizes) {
      printf("%d --> ", size);
    }
    printf("\n");
    
    printf("%35s: %f\n", "Sentence probability", model.getSentenceProbability(sentence));

    printf("%35s: ", "Generated Sentence");
    for (string word : sentence) {
      printf("%s ", word.c_str());
    }
    printf("\n");

    printf("\n");
  }

 return 0;
}