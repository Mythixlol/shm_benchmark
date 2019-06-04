#include <iostream>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sstream>
#include <thread>
#include <chrono>

#define runs 1000

// define queue-buffer length and number of elements for insertion
#define fd_size 32 * 1024
const int elements = 5000;

int dataSet[elements];
