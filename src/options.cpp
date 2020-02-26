#include <string>
#include <string.h>
#include <algorithm>

#include "options.h"
#include "debug.h"

Options::Options() {
  // Set default options
  this->debug = false;
  this->deepDebug = false;
  this->help = false;
  this->constraints = vector<string>();
  this->markovOrder = 1;
  this->sentenceCount = 1;
  this->useCache = false;
  this->trainingFilePath = "";
  this->port = 7799;  // unassigned port
  this->shouldRunAsServer = false;
}

void Options::parseArguments(int argc, char *argv[]) {
  for (int i = 1; i < argc; i++) {
    // Debug flag
    if (strcasecmp(argv[i], "--debug") == 0 || strcasecmp(argv[i], "-d") == 0) {
      Debug::setDebugEnabled(true);
      this->debug = true;

    // DeepDebug (Slow) flag
    } else if (strcasecmp(argv[i], "--deepdebug") == 0 || strcasecmp(argv[i], "-dd") == 0) {
      Debug::setDebugEnabled(true);
      Debug::setDeepDebugEnabled(true);
      this->debug = true;
      this->deepDebug = true;

    // Help
    } else if (strcasecmp(argv[i], "help") == 0 || strcasecmp(argv[i], "--help") == 0 || strcasecmp(argv[i], "-h") == 0) {
      this->help = true;

    // Constraint flag
    } else if (strcasecmp(argv[i], "--constraint") == 0 || strcasecmp(argv[i], "-c") == 0) {
      if (i+1 < argc) {
        string constraint = argv[++i];
        // Strip double quotes
        constraint.erase(remove( constraint.begin(), constraint.end(), '\"'), constraint.end());

        constraints.push_back(constraint);
      }

    // Markov Order
    } else if (strcasecmp(argv[i], "--markovorder") == 0 || strcasecmp(argv[i], "-m") == 0) {
      if (i+1 < argc) {
        this->markovOrder = atoi(argv[++i]);
      }

    // Generated sentence count
    } else if (strcasecmp(argv[i], "-n") == 0) {
      if (i+1 < argc) {
        this->sentenceCount = atoi(argv[++i]);
      }

    // Use cached files
    } else if (strcasecmp(argv[i], "--cache") == 0) {
      this->useCache = true;

    // Port number
    } else if (strcasecmp(argv[i], "--port") == 0 || strcasecmp(argv[i], "-p") == 0) {
      if (i+1 < argc) {
        this->port = atoi(argv[++i]);
      }

    // Should run as server
    } else if (strcasecmp(argv[i], "--server") == 0 || strcasecmp(argv[i], "-s") == 0) {
      this->shouldRunAsServer = true;

    // Training file path
    } else {
      this->trainingFilePath = argv[i];
    }
  }
}

bool Options::getDebug() {
  return this->debug;
}

bool Options::getDeepDebug() {
  return this->deepDebug;
}

bool Options::getHelp() {
  return this->help;
}

vector<string> Options::getConstraints() {
  return this->constraints;
}

int Options::getMarkovOrder() {
  return this->markovOrder;
}

int Options::getSentenceCount() {
  return this->sentenceCount;
}

bool Options::getUseCache() {
  return this->useCache;
}

string Options::getTrainingFilePath() {
  return this->trainingFilePath;
}

int Options::getPort() {
  return this->port;
}

bool Options::getShouldRunAsServer() {
  return this->shouldRunAsServer;
}