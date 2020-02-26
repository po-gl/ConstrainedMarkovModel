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
  auto markovModel = MarkovModel(options);

  for (const auto &constraint : options.getConstraints()) {
    Console::debugPrint("%-35s: %s\n", "Constraint", constraint.c_str());

    auto model = MnemonicMarkovModel(markovModel, Utils::cleanConstraint(constraint), options);
    model.printDebugInfo(options);
    auto generatedSentences = model.generateSentences(options);

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