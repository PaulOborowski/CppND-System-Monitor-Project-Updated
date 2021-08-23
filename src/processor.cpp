#include "processor.h"

#include <string>

#include "linux_parser.h"

using std::stof;
using std::string;

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() {
  // read from filesystem

  float newIdle, newNonIdle, newTotal, cpuLoad;

  // calculate percentage
  newIdle = LinuxParser::IdleJiffies();
  newNonIdle = LinuxParser::ActiveJiffies();
  newTotal = LinuxParser::Jiffies();
  cpuLoad = (newTotal - this->prevTotal - (newIdle - this->prevIdle)) /
            (newTotal - this->prevTotal);

  // update member variables
  this->prevIdle = newIdle;
  this->prevNonIdle = newNonIdle;
  this->prevTotal = newTotal;

  return cpuLoad;
}