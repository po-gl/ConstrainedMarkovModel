#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <netinet/in.h>
#include <arpa/inet.h>

#include <string>
#include <thread>

#include "server.h"
#include "options.h"
#include "console.h"
#include "utils.h"
#include "markov.h"
#include "main.h"
#include "threadqueue.h" 


Server::Server(int port, Options options, int threadCount, int bufferSize) {
  this->queue = std::unique_ptr<ThreadQueue<ConnectionData> >(new ThreadQueue<ConnectionData>(&mutex, &cv));

  this->port = port;
  this->options = options;
  // Thread count cannot be less than 1
  this->threadCount = (threadCount < 1) ? 1 : threadCount;
  this->bufferSize = bufferSize;
  this->shouldStop = false;
}


void Server::startServerLoop() {
  Console::debugPrint("Starting Server Loop\n");
  // Train non-constrained Markov model
  // auto markovModel = Main::trainMarkov(this->options);
  auto markovModel = MarkovModel(this->options);
  this->markovModel = markovModel;

  Console::debugPrint("Creating Socket on port %d\n", this->port);
  int server_fd = createSocket(this->port);

  // Start worker threads
  for(int i = 0; i < this->threadCount; i++) {
    std::thread worker(performWork, i, &this->shouldStop,
                       &this->queue, &this->mutex, &this->cv,
                       &this->options, &this->markovModel);
    this->threadPool.push_back(&worker);
    worker.detach();
  }

  // Start connection broker loop on main thread (blocks main thread)
  startConnectionBrokerLoop(server_fd, options);
}


void Server::stop() {
  // std::unique_lock<std::mutex> lock(this->mutex);
  this->shouldStop = true;
}


void Server::performWork(int threadID, bool *shouldStop,
                         std::unique_ptr<ThreadQueue<ConnectionData> > *queue,
                         std::mutex *mutex, std::condition_variable *cv,
                         Options *options, MarkovModel *markovModel) {
  while (!*shouldStop) {
    ConnectionData data;
    // Wait for queue element
    while (!(*queue)->pop(data)) {
      Console::debugPrint("Waiting on thread %d\n", threadID);
      std::unique_lock<std::mutex> lock(*mutex);
      cv->wait(lock);  // Non-busy wait on thread
    }

    Console::debugPrint("Thread %d working on constraint: %s\n", threadID, Utils::cleanConstraint(data.constraint).c_str());

    auto model = MnemonicMarkovModel(*markovModel, Utils::cleanConstraint(data.constraint), *options);
    model.printDebugInfo(*options);
    auto generatedSentences = model.generateSentences(*options);


    // Send sentences + data back to client
    string builder;

    // Mnemonic sentences
    for (const auto &sentence : generatedSentences) {
      for (const auto &word : sentence) {
        builder += word + " ";
      }
      builder += "::";
    }
    builder.pop_back();
    builder.pop_back();

    builder += "$$$";

    // Words removed by constraints
    for (int i = 0; i < generatedSentences.size(); i++) {
      for (int j = 0; j < model.getSentenceLength(); j++) {
        builder += model.sampleRemovedNodeByConstraint(j) + " ";
      }
      builder += "::";
    }
    builder.pop_back();
    builder.pop_back();

    builder += "$$$";

    // Words removed by Arc consistency
    for (int i = 0; i < generatedSentences.size(); i++) {
      for (int j = 0; j < model.getSentenceLength(); j++) {
        builder += model.sampleRemovedNodeByArcConsistency(j) + " ";
      }
      builder += "::";
    }
    builder.pop_back();
    builder.pop_back();

    int sval = write(data.accepted_fd, builder.c_str(), builder.size());
    if (sval < 0) {
      perror("Send back error");
    }
    close(data.accepted_fd);
  }
}


void Server::startConnectionBrokerLoop(int server_fd, Options options) {
  char buffer[this->bufferSize];

  while (!this->shouldStop) {
    memset(buffer, 0, this->bufferSize);  // clear buffer
    int accepted_fd = acceptConnections(server_fd);

    // Send to queue
    int rval = read(accepted_fd, buffer, this->bufferSize);
    if (rval < 0) {
      perror("read error");
    } else if (rval == 0) {
      perror("connection closed");
    } else {
      ConnectionData data;
      data.accepted_fd = accepted_fd;
      data.constraint = buffer;
      this->queue->push(data);  // worker threads are notified on push
    }
  }
}


int Server::createSocket(int port) {
  // Create socket
  int server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd < 0) {
    perror("socket error");
    exit(-1);
  }

  // Forcefully attach socket to port
  int opt = 1;
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt))) 
  { 
    perror("socket options error"); 
    exit(-1);
  } 

  // Set struct
  struct sockaddr_in address;
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = htonl(INADDR_ANY);
  address.sin_port = htons(port);

  // Bind socket to address and port
  if (::bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
    perror("bind error");
    exit(-1);
  }

  // Set socket to listen
  if (listen(server_fd, 1) < 0) {
    perror("listen error");
    exit(-1);
  }

  return server_fd;
}


int Server::acceptConnections(int server_fd) {
  struct sockaddr_in cin;
  socklen_t cin_sz = sizeof(cin);

  int accepted_fd = accept(server_fd, (struct sockaddr *)&cin, &cin_sz);
  if (accepted_fd < 0) {
    perror("accept error");
  }

  if (sizeof(cin) == cin_sz) {
    Console::debugPrint("connection from %s:%d\n", inet_ntoa(cin.sin_addr), (int)ntohs(cin.sin_port));
  }

  return accepted_fd;
}

