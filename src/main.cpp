#include <iostream>
#include <time.h>
#include <algorithm>
#include <string>
#include <string.h>
#include "utils.h"
#include "debug.h"
#include "console.h"

#include "mnemonicmarkov.h"

#include "markov.h"

using namespace std;

// TODO: Implement an interactive mode
// TODO: Make printing consistent
// TODO: Print progress reading in files and processing data

// TODO: Use log probs to prevent underflow for really large sentences

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
  }
  else if (constraints.empty()) {
    printf("Constraint is needed.\n");
    printHelp();
    return 0;
  }


  // Non-constrained Markov model
  MarkovModel markovModel;

  // Read/Pre-process training sequences
  vector< vector<string> > trainingSequences;
  if (useCache) {
    // Read in training sentences from cache
    startTime = clock();
    Utils::readFromCache(markovModel, Utils::getBasename(trainingFilePath));
    Console::debugPrint("%-35s: %f\n", "Elapsed Time Reading From Cache", (float) (clock() - startTime) / CLOCKS_PER_SEC);
  }

  // TODO: Rebuild cache reading it fails or if markov order is different
  if (markovModel.getProbabilityMatrix().empty()){
    if (useCache)
      Console::debugPrint("No cache found for file.\n");

    // Read in training sentences
    startTime = clock();
    string trainingText = Utils::readInTrainingSentences(trainingFilePath);
    Console::debugPrint("%-35s: %f\n", "Elapsed Time Reading Data", (float) (clock() - startTime) / CLOCKS_PER_SEC);

    // Process training sentences
    startTime = clock();
    trainingSequences = Utils::processTrainingSentences(trainingText, markovOrder);
    Console::debugPrint("%-35s: %f\n", "Elapsed Time Processing Data", (float) (clock() - startTime) / CLOCKS_PER_SEC);
    Console::debugPrint("%-35s: %d\n", "Training Sentence Count", trainingSequences.size());

    markovModel.train(trainingSequences, markovOrder);

    if (useCache)
      Utils::writeToCache(markovModel, Utils::getBasename(trainingFilePath));
  }


  // Constrained Markov model
  MnemonicMarkovModel model;

  // Apply constraints to model
  for (const auto &constraint : constraints) {
    Console::debugPrint("\n%-35s: %d\n", "Markov Order", markovOrder);
    Console::debugPrint("%-35s: %s\n", "Constraint", constraint.c_str());

    // Train model
    startTime = clock();
    model.train(markovModel, Utils::splitAndLower(constraint, "\\s,"));
    Console::debugPrint("%-35s: %f\n", "Elapsed Training Time", (float)(clock() - startTime) / CLOCKS_PER_SEC);

    // Generate sentences
    startTime = clock();
    vector<vector<string> > generatedSentences;
    generatedSentences.reserve(sentenceCount);
    for (int i = 0; i < sentenceCount; i++) {
      generatedSentences.push_back(model.generateSentence());
    }
    Console::debugPrint("%-35s: %f\n", "Elapsed Sentence(s) Gen Time", (float)(clock() - startTime) / CLOCKS_PER_SEC);

    // Print matrix sizes (debug)
    Console::debugPrint("%-35s: ", "Transition Matrix sizes");
    vector<int> sizes = model.getTransitionMatricesSizes();
    for (auto size : sizes) {
      Console::debugPrint("%d --> ", size);
    }
    Console::debugPrint("\n");

    // Print generated sentences with probabilities (debug)
    Console::debugPrint("\n%s  (%d)\n", "Generated Sentences", sentenceCount);
    Console::debugPrint("%-10s: %s\n", "(prob)", "(sentence)");
    for (const auto &sentence : generatedSentences) {
      Console::debugPrint("%-10f: ", model.getSentenceProbability(sentence));

      for (const string &word : sentence) {
        Console::debugPrint("%s ", word.c_str());
      }
      Console::debugPrint("\n");
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