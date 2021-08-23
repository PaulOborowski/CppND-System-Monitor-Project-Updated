#include "process.h"

#include <unistd.h>

#include <cctype>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

// constructor to initialize with PID and get member data using LinuxParser
Process::Process(int newPid, long upTimeSystem) : pid(newPid) {
  this->user = LinuxParser::User(this->pid);
  this->processTimes = LinuxParser::ProcessTimes(this->pid);
  this->cpuUtilization = this->CpuUtilization(upTimeSystem);
  this->ram = LinuxParser::Ram(this->pid);
  this->command = LinuxParser::Command(this->pid);
  this->upTime= LinuxParser::UpTime(this->pid);
  /* debug using command line
  this->command = to_string(this->processTimes[0])  + " "
  +to_string(this->processTimes[1]) + " " + to_string(this->processTimes[4]) + "
  " + to_string(upTimeSystem) + " f: " +  to_string(cpuUtilization);
                  */
}

// TODO: Return this process's ID
int Process::Pid() { return this->pid; }

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() { return this->cpuUtilization; }

float Process::CpuUtilization(long upTimeSystem) {
  float result = upTimeSystem - processTimes[4];  // elapsed time in seconds
  // float result = this->uptime;  // elapsed time in seconds
  if (result > 0) {
    result = 1.0 *
             (processTimes[0] + processTimes[1] + processTimes[2] +
              processTimes[3]) /
             result;
  } else
    result = 0.0;

  return result;
}

// TODO: Return the command that generated this process
string Process::Command() { 
  if (this->command.size()>40) return this->command.substr(0,40).append("...");
  return this->command; }

// TODO: Return this process's memory utilization
string Process::Ram() { return this->ram; }

// TODO: Return the user (name) that generated this process
string Process::User() { return this->user; }

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() {
  return this->upTime;  // in seconds
}

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator>(Process const& a) const {
  // return this->pid > a.pid;
  return this->cpuUtilization > a.cpuUtilization;
  // return std::stol(this->ram) > std::stol(a.ram);
}

bool Process::operator<(Process const& a) const {
  // return this->pid < a.pid;
  return this->cpuUtilization < a.cpuUtilization;
  // return std::stol(this->ram) < std::stol(a.ram);
}