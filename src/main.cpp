#include <iostream>
#include <time.h>
#include <algorithm>
#include <string>
#include <string.h>
#include "utils.h"

#include "main.h"
#include "debug.h"
#include "options.h"
#include "console.h"
#include "server.h"
#include "markov.h"
#include "mnemonicmarkov.h"


using namespace std;

// TODO: Implement an interactive mode
// TODO: Print progress reading in files and processing data

// TODO: Use log probs to prevent underflow for really large sentences

MarkovModel Main::trainMarkov(Options options) {
  time_t startTime; // used for debug timing

  // Non-constrained Markov model
  MarkovModel markovModel;

  // Read/Pre-process training sequences
  vector< vector<string> > trainingSequences;
  if (options.getUseCache()) {
    // Read in training sentences from cache
    startTime = clock();
    Utils::readFromCache(markovModel, Utils::getBasename(options.getTrainingFilePath()));
    Console::debugPrint("%-35s: %f\n", "Elapsed Time Reading From Cache", (float) (clock() - startTime) / CLOCKS_PER_SEC);
  }

  // TODO: Rebuild cache reading it fails or if markov order is different
  if (markovModel.getProbabilityMatrix().empty()){
    if (options.getUseCache())
      Console::debugPrint("No cache found for file.\n");

    // Read in training sentences
    startTime = clock();
    string trainingText = Utils::readInTrainingSentences(options.getTrainingFilePath());
    Console::debugPrint("%-35s: %f\n", "Elapsed Time Reading Data", (float) (clock() - startTime) / CLOCKS_PER_SEC);

    // Process training sentences
    startTime = clock();
    trainingSequences = Utils::processTrainingSentences(trainingText, options.getMarkovOrder());
    Console::debugPrint("%-35s: %f\n", "Elapsed Time Processing Data", (float) (clock() - startTime) / CLOCKS_PER_SEC);

    markovModel.train(trainingSequences, options.getMarkovOrder());

    if (options.getUseCache())
      // Write to cache
      Utils::writeToCache(markovModel, Utils::getBasename(options.getTrainingFilePath()));
  }

  return markovModel;
}


MnemonicMarkovModel Main::trainConstrainedMarkov(Options options, MarkovModel markovModel, string constraint) {
  time_t startTime; // used for debug timing

  // Constrained Markov model
  MnemonicMarkovModel constrainedModel;

  // Train model (Apply constraints)
  startTime = clock();
  constrainedModel.train(markovModel, Utils::splitAndLower(constraint, "\\s,"));
  Console::debugPrint("%-35s: %f\n", "Elapsed Training Time", (float)(clock() - startTime) / CLOCKS_PER_SEC);

  return constrainedModel;
}


vector<vector<string> > Main::generateSentences(Options options, MnemonicMarkovModel model) {
  time_t startTime; // used for debug timing

  // Generate sentences
  startTime = clock();
  vector<vector<string> > generatedSentences;
  generatedSentences.reserve(options.getSentenceCount());
  for (int i = 0; i < options.getSentenceCount(); i++) {
    generatedSentences.push_back(model.generateSentence());
  }
  Console::debugPrint("\n%-35s: %f\n", "Elapsed Sentence(s) Gen Time", (float)(clock() - startTime) / CLOCKS_PER_SEC);

  // Print generated sentences with probabilities (debug)
  Console::debugPrint("%s  (%d)\n", "Generated Sentences", options.getSentenceCount());
  Console::debugPrint("%-10s: %s\n", "(prob)", "(sentence)");
  for (const auto &sentence : generatedSentences) {
    Console::debugPrint("%-10f: ", model.getSentenceProbability(sentence));

    for (const string &word : sentence) {
      Console::debugPrint("%s ", word.c_str());
    }
    Console::debugPrint("\n");
  }

  return generatedSentences;
}


void Main::printMarkovDebugInfo(Options options, MnemonicMarkovModel model) {
  // Print markov order (debug)
  Console::debugPrint("\n%-35s: %d\n", "Markov Order", options.getMarkovOrder());

  // Print training sequence count
  Console::debugPrint("%-35s: %d\n", "Training Sentence Count", model.getTrainingSequences().size());

  // Print matrix sizes (debug)
  Console::debugPrint("%-35s: ", "Transition Matrix sizes");
  vector<int> sizes = model.getTransitionMatricesSizes();
  for (auto size : sizes) {
    Console::debugPrint("%d --> ", size);
  }
  Console::debugPrint("\n");
}


int runAsCommandLineTool(Options options) {
  // Check options
  if (options.getTrainingFilePath().empty()) {
    printf("Training text is needed.\n");
    Console::printHelp();
    return 0;
  }
  else if (options.getConstraints().empty()) {
    printf("Constraint is needed.\n");
    Console::printHelp();
    return 0;
  }

  // Train non-constrained Markov model
  auto markovModel = Main::trainMarkov(options);

  for (const auto &constraint : options.getConstraints()) {
    Console::debugPrint("%-35s: %s\n", "Constraint", constraint.c_str());

    auto model = Main::trainConstrainedMarkov(options, markovModel, constraint);
    Main::printMarkovDebugInfo(options, model);
    auto generatedSentences = Main::generateSentences(options, model);

    // Print to standard output
    for (const auto &sentence : generatedSentences) {
      for (const string &word : sentence) {
        printf("%s ", word.c_str());
      }
      printf("\n");
    }
  }
  return 0;
}


int main(int argc, char *argv[]) {

  // Parse Arguments
  Options options;
  options.parseArguments(argc, argv);

  if (options.getHelp()) {
    Console::printHelp();
    return 0;
  }

  if (!options.getShouldRunAsServer()) {
    return runAsCommandLineTool(options);
  }
  else {
    Server server(options.getPort(), options);
    server.startServerLoop();
  }

  return 0;
}