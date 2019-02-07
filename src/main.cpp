#include <iostream>
#include <time.h>
#include <algorithm>
#include "utils.h"
#include "mnemonicmarkov.h"

using namespace std;

// TODO: Use proper data instead of Gutenburg books


void printHelp() {
  printf("usage: markov [--debug | -d] -c constraint training_text\n");
}


int main(int argc, char *argv[]) {

  // Set default values
  bool debug = false;
  string trainingFilePath;
  string constraint;
  int markovOrder = 1;
  int sentenceCount = 1;

  time_t startTime;
  time_t endTime;

  // Parse Arguments
  // TODO: create options class
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
        // Strip double quotes
        constraint.erase(remove( constraint.begin(), constraint.end(), '\"'), constraint.end());
      }

    } else if (strncmp(argv[i], "-m", 2) == 0) {
      if (i+1 < argc) {
        markovOrder = atoi(argv[++i]);
      }

    // Generated sentence count
    } else if (strncmp(argv[i], "-n", 2) == 0) {
      if (i+1 < argc) {
        sentenceCount = atoi(argv[++i]);
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
  model.train(trainingFilePath, Utils::splitAndLower(constraint, "\\s,"), markovOrder);
  endTime = clock();
  time_t trainingTime = endTime - startTime;


  startTime = clock();
  vector< vector<string> > generatedSentences;
  generatedSentences.reserve(sentenceCount);
  for (int i = 0; i < sentenceCount; i++) {
    generatedSentences.push_back(model.generateSentence());
  }
  endTime = clock();
  time_t sentenceGenerationTime = endTime - startTime;


  if (debug) {  // Print debug information
    printf("\n");
    printf("%45s\n", "=== DEBUG INFORMATION ===");
    printf("%35s: %s\n", "Constraint", constraint.c_str());
    printf("%35s: %d\n", "Markov Order", markovOrder);

    printf("%35s: %f\n", "Elapsed Training Time (sec)", (float)trainingTime/CLOCKS_PER_SEC);
    printf("%35s: %f\n", "Elapsed Sentence(s) Gen Time (sec)", (float)sentenceGenerationTime/CLOCKS_PER_SEC);

    printf("%35s: ", "Transition Matrix sizes");
    vector<int> sizes = model.getTransitionMatricesSizes();
    for (auto size : sizes) {
      printf("%d --> ", size);
    }
    printf("\n");
    printf("\n");

    printf("%35s (%d) ===\n", "=== Generated Sentences", sentenceCount);
    printf("%35s: %s\n", "(prob)", "(sentence)");
    for (const auto &sentence : generatedSentences) {
      printf("%35f: ", model.getSentenceProbability(sentence));

      for (const string &word : sentence) {
        printf("%s ", word.c_str());
      }
      printf("\n");
    }
    printf("%35s\n", "==========================================================");
  }


  // Print standard output
  for (const auto &sentence : generatedSentences) {
    for (const string &word : sentence) {
      printf("%s ", word.c_str());
    }
    printf("\n");
  }

 return 0;
}