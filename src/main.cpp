#include <iostream>
#include <time.h>
#include <algorithm>
#include <string>
#include <string.h>
#include "utils.h"
#include "debug.h"
#include "console.h"
#include "mnemonicmarkov.h"

using namespace std;

// TODO: Implement an interactive mode
// TODO: Make printing consistent
// TODO: Print progress reading in files and processing data

void printHelp() {
  printf("usage: markov [--debug | -d] -c constraint [-m] [-n] [-p] training_text\n");
}


int main(int argc, char *argv[]) {

  // Set default values
  bool debug = false;
  string trainingFilePath;
  vector<string> constraints;
  int markovOrder = 1;
  int sentenceCount = 1;
  bool useCache = false;

  time_t startTime;
  time_t endTime;

  // Parse Arguments
  // TODO: create options class
  for (int i = 1; i < argc; i++) {
    // Debug flag
    if (strcasecmp(argv[i], "--debug") == 0 || strcasecmp(argv[i], "-d") == 0) {
      debug = true;
      Debug::setDebugEnabled(true);

    // Help
    } else if (strcasecmp(argv[i], "help") == 0) {
      printHelp();
      return 0;

    // Constraint flag
    } else if (strcasecmp(argv[i], "-c") == 0) {
      if (i+1 < argc) {
        string constraint = argv[++i];
        // Strip double quotes
        constraint.erase(remove( constraint.begin(), constraint.end(), '\"'), constraint.end());

        constraints.push_back(constraint);
      }

    // Markov Order
    } else if (strcasecmp(argv[i], "-m") == 0) {
      if (i+1 < argc) {
        markovOrder = atoi(argv[++i]);
      }

    // Generated sentence count
    } else if (strcasecmp(argv[i], "-n") == 0) {
      if (i+1 < argc) {
        sentenceCount = atoi(argv[++i]);
      }

    // Use cached files
    } else if (strcasecmp(argv[i], "-p") == 0) {
      useCache = true;

    // Training file path
    } else {
      trainingFilePath = argv[i];
    }
  }

  if (trainingFilePath.empty()) {
    printf("Training text is needed.\n");
    printHelp();
    return 0;
  } else if (constraints.empty()) {
    printf("Constraint is needed.\n");
    printHelp();
    return 0;
  }

  for (const auto &constraint : constraints) {
    Console::debugPrint("%35s: %s\n", "Constraint", constraint.c_str());
  }


  // Read/Pre-process training sequences
  vector< vector<string> > trainingSequences;
  if (useCache) {
    // Read in training sentences from cache
    startTime = clock();
    trainingSequences = Utils::readFromCache(Utils::getBasename(trainingFilePath));
    Console::debugPrint("%35s: %f\n", "Elapsed Time Reading Data From Cache", (float) (clock() - startTime) / CLOCKS_PER_SEC);
  }

  if (trainingSequences.empty()){
    if (useCache)
      Console::debugPrint("No cache found for file.\n");

    // Read in training sentences
    startTime = clock();
    string trainingText = Utils::readInTrainingSentences(trainingFilePath);
    Console::debugPrint("%35s: %f\n", "Elapsed Time Reading Data", (float) (clock() - startTime) / CLOCKS_PER_SEC);

    // Process training sentences
    startTime = clock();
    trainingSequences = Utils::processTrainingSentences(trainingText, markovOrder);
    Console::debugPrint("%35s: %f\n", "Elapsed Time Processing Data", (float) (clock() - startTime) / CLOCKS_PER_SEC);
    Console::debugPrint("%35s: %d\n", "Training Sentence Count", trainingSequences.size());

    if (useCache)
      Utils::writeToCache(Utils::getBasename(trainingFilePath), trainingSequences);
  }


  MnemonicMarkovModel model;

  for (const auto &constraint : constraints) {

    startTime = clock();
    model.train(trainingSequences, Utils::splitAndLower(constraint, "\\s,"), markovOrder);
    endTime = clock();
    time_t trainingTime = endTime - startTime;


    startTime = clock();
    vector<vector<string> > generatedSentences;
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

      printf("%35s: %f\n", "Elapsed Training Time (sec)", (float) trainingTime / CLOCKS_PER_SEC);
      printf("%35s: %f\n", "Elapsed Sentence(s) Gen Time (sec)", (float) sentenceGenerationTime / CLOCKS_PER_SEC);

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
    }


    // Print standard output
    for (const auto &sentence : generatedSentences) {
      for (const string &word : sentence) {
        printf("%s ", word.c_str());
      }
      printf("\n");
    }
  }

 return 0;
}