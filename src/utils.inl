#include "utils.h"

template <class T>
void Utils::readFromCache(T &ret, string fileName) {
  string cacheFilePath = Utils::cacheDirectory + fileName + Utils::cacheSuffix;
  ifstream file;
  file.open(cacheFilePath);
  string line;

  if (file.is_open()) {
    boost::archive::binary_iarchive iarch(file);
    iarch & ret;  // read from cache
    file.close();
    // TODO: add try/catch for if the the cache is messed up
  } else {
    printf("ERROR::No file was found at %s\n", cacheFilePath.c_str());  // TODO: throw error
  }
}


template <class T>
void Utils::writeToCache(T data, string fileName) {

  // Create cache directory if it doesn't already exist
  if (mkdir(Utils::cacheDirectory.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
    if (errno == EEXIST) {
    } else {
      printf("ERROR::Unable to create directory %s\n", Utils::cacheDirectory.c_str());  // TODO: throw error
    }
  }
  string cacheFilePath = Utils::cacheDirectory + fileName + Utils::cacheSuffix;
  ofstream file;
  file.open(cacheFilePath);

  if (file.is_open()) {
    boost::archive::binary_oarchive oarch(file);
    oarch & data;  // write to cache
    file.close();
  } else {
    printf("ERROR::Unable to open file at %s\n", cacheFilePath.c_str());  // TODO: throw error
  }
}