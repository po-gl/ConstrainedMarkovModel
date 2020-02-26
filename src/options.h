#ifndef MARKOV_OPTIONS_H
#define MARKOV_OPTIONS_H

#include <string>
#include <vector>

using namespace std;

/**
 * @brief Class to manage command line options and arguments
 * 
 * List of options:
 * --debug | -d
 * help | --help | -h
 * --constraint | -c
 * --markovorder | -m
 * -n
 * --cache
 * trainingFilePath
 * 
 * @author Porter Glines 5/19/19
 */
class Options {
public:
  Options();

  /**
   * @brief Parse command line arguments into options
   * 
   * @param argc count of arguments
   * @param argv cstring array of arguments
   * @author Porter Glines 5/19/19
   */
  void parseArguments(int argc, char *argv[]);

  /**
   * @brief Get the Debug object
   * 
   * @return bool debug
   * @author Porter Glines 5/19/19 
   */
  bool getDebug();

  /**
   * @brief Get the Deep Debug object (Slow)
   * 
   * @return bool deep debug
   * @author Porter Glines 5/19/19 
   */
  bool getDeepDebug();

  /**
   * @brief Get the Help object
   * 
   * @return bool help
   * @author Porter Glines 5/19/19 
   */
  bool getHelp();

  /**
   * @brief Get the Constraints object
   * 
   * @return vector<string> constraints
   * @author Porter Glines 5/19/19 
   */
  vector<string> getConstraints();

  /**
   * @brief Get the Markov Order object
   * 
   * @return int 
   * @author Porter Glines 5/19/19 
   */
  int getMarkovOrder();

  /**
   * @brief Get the Sentence Count object
   * 
   * @return int 
   * @author Porter Glines 5/19/19 
   */
  int getSentenceCount();

  /**
   * @brief Get the Use Cache object
   * 
   * @return bool use cache 
   * @author Porter Glines 5/19/19 
   */
  bool getUseCache();

  /**
   * @brief Get the Training File Path object
   * 
   * @return string file path
   * @author Porter Glines 5/19/19 
   */
  string getTrainingFilePath();

  /**
   * @brief Get the Training Sentence Limit object
   * 
   * @return int sentence limit
   * @author Porter Glines 2/26/20
   */
  int getTrainingSentenceLimit();

  /**
   * @brief Get the port object
   * 
   * @return int port number
   * @author Porter Glines 2/23/19
   */
  int getPort();

  /**
   * @brief Get the shouldRunAsServer object
   * 
   * @return true if program should run as socket server
   * @author Porter Glines 2/23/19
   */
  bool getShouldRunAsServer();


private:
  bool debug;
  bool deepDebug;
  bool help;
  vector<string> constraints;
  int markovOrder;
  int sentenceCount;
  bool useCache;
  string trainingFilePath;
  int trainingSentenceLimit;
  int port;
  bool shouldRunAsServer;
};

#endif