#ifndef PROCESS_H
#define PROCESS_H

#include <string>
#include <vector>
/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
 public:
  Process(int newpid, long upTimeSystem);
  int Pid();                               // TODO: See src/process.cpp
  std::string User();                      // TODO: See src/process.cpp
  std::string Command();                   // TODO: See src/process.cpp
  float CpuUtilization();                  // TODO: See src/process.cpp
  std::string Ram();                       // TODO: See src/process.cpp
  long int UpTime();                       // TODO: See src/process.cpp
  bool operator<(Process const& a) const;  // TODO: See src/process.cpp
  bool operator>(Process const& a) const;

  // TODO: Declare any necessary private members
 private:
  int pid;
  std::string user;
  std::string uid;
  std::string command;
  float cpuUtilization;
  std::vector<long> processTimes;
  std::string ram;
  long int upTime;

  float CpuUtilization(
      long upTimeSystem);  // should be only called by constructor
};

#endif