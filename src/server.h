#ifndef MARKOVSERVER_H
#define MARKOVSERVER_H

#include <thread>
#include <mutex>
#include <condition_variable>

#include "threadqueue.h"
#include "options.h"
#include "models/markov.h"

struct ConnectionData {
  int accepted_fd;
  string constraint;
};

/**
 * @brief Server to connect to client(s) via sockets
 * 
 */
class Server {
public:
  Server(int port, Options options, int threadCount = 2, int bufferSize = 4096);
  ~Server() {};

  void startServerLoop();
  void stop();
  
  static void performWork(int threadID, bool *shouldStop,
                          std::unique_ptr<ThreadQueue<ConnectionData> > *queue,
                          std::mutex *mutex, std::condition_variable *cv,
                          Options *options, MarkovModel *markovModel);

private:
  std::mutex mutex;
  std::condition_variable cv;

  std::unique_ptr<ThreadQueue<ConnectionData> > queue;

  std::thread brokerThread;
  std::vector<std::thread*> threadPool;
  int threadCount;  // Thread count
  int port;
  int bufferSize;
  bool shouldStop;

  Options options;
  MarkovModel markovModel;

  int createSocket(int port);

  int acceptConnections(int server_fd);

  void startConnectionBrokerLoop(int server_fd, Options options);

};

#endif
