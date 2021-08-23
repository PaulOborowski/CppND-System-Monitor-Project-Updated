#include "system.h"

#include <unistd.h>

#include <cstddef>
#include <iostream>
#include <set>
#include <string>
#include <vector>

#include "linux_parser.h"
#include "process.h"
#include "processor.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;

// TODO: Return the system's CPU
Processor& System::Cpu() { return cpu_; }

// TODO: Return a container composed of the system's processes
vector<Process>& System::Processes() {
  // get Pids and parse into Process objects.
  this->processes_.clear();
  vector<int> pids = LinuxParser::Pids();
  this->UpTime();  // refresh system uptime once
  if (!pids.empty()) {
    for (int pid : pids) {
      // emplace_back calls the constructor, no need for Process() call
      this->processes_.emplace_back(pid, this->upTime);
    }
    // sort processes
    std::sort(this->processes_.begin(), this->processes_.end(),std::greater<Process>());
  }
  return this->processes_;
}

// TODO: Return the system's kernel identifier (string)
std::string System::Kernel() {
  string result = LinuxParser::Kernel();
  if (!result.empty()) this->kernel = result;
  return this->kernel;
}

// TODO: Return the system's memory utilization
float System::MemoryUtilization() { return LinuxParser ::MemoryUtilization(); }

// TODO: Return the operating system name
std::string System::OperatingSystem() {
  string result = LinuxParser::OperatingSystem();
  if (!result.empty()) this->operatingsystem = result;
  return this->operatingsystem;
}

// TODO: Return the number of processes actively running on the system
int System::RunningProcesses() {
  int result = LinuxParser::RunningProcesses();
  if (result != -1) this->runningProcesses = result;
  return this->runningProcesses;
}

// TODO: Return the total number of processes on the system
int System::TotalProcesses() {
  int result = LinuxParser::TotalProcesses();
  if (result != -1) this->totalProcesses = result;
  return this->totalProcesses;
}

// TODO: Return the number of seconds since the system started running
long int System::UpTime() {
  this->upTime = LinuxParser::UpTime();  // save for processes
  return this->upTime;
}