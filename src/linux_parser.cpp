#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

#include <filesystem>
#include <string>
#include <vector>

using std::stof;
using std::stol;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  return LineParser(kOSPath, "PRETTY_NAME", {' ', '_', '=', ' ', '"', ' '},
                    {'_', ' '});
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

/*
// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
} */
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  for (const auto entry : std::filesystem::directory_iterator(kProcDirectory)) {
    if (entry.is_directory()) {
      string filename = entry.path().filename();      
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  return pids;
}

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  // output must be between 0 and 1
  float memTotal = std::stof(
      LineParser(kProcDirectory + kMeminfoFilename, "MemTotal:", {}, {}));
  float memFree = std::stof(
      LineParser(kProcDirectory + kMeminfoFilename, "MemFree:", {}, {}));

  return (memTotal - memFree) / memTotal;
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() {
  string upTime, idleTime;
  string line;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> upTime >> idleTime;
  }
  return std::stol(upTime);
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { return ActiveJiffies() + IdleJiffies(); }

/* not needed - used ProcessTimes instead
// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) {
  vector<string> cpuTimes = {};
  vector<int> entries = {13, 14, 15, 16, 21};  // must be sorted
  int numMax = *std::max_element(entries.begin(), entries.end());
  int idEntry = 0;
  string line, buffer;

  std::ifstream stream(kProcDirectory + std::to_string(pid) + "/" +
                       kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    for (int num = 0; num <= numMax; num++) {
      linestream >> buffer;
      if (num == entries[idEntry]) {
        cpuTimes.push_back(buffer);
        idEntry++;
      }
    }
  }

  return stol(cpuTimes[0]) + stol(cpuTimes[1]) + stol(cpuTimes[2]) +
         stol(cpuTimes[0]);
}
*/

// collects all time data required for calculation of cpu ultiziation and
// converts in seconds
vector<long> LinuxParser::ProcessTimes(int pid) {
  vector<long> processTimes = {0, 0, 0, 0, -1};
  vector<int> entries = {13, 14, 15, 16, 21};  // must be sorted
  int numMax = *std::max_element(entries.begin(), entries.end());
  int idEntry = 0;
  string line, buffer;

  std::ifstream stream(kProcDirectory + std::to_string(pid) + "/" +
                       kStatFilename);
  if (stream.is_open()) {
    processTimes[4] = numMax;
    std::getline(stream, line);
    std::istringstream linestream(line);
    for (int num = 0; num <= numMax; num++) {
      linestream >> buffer;
      if (num == entries[idEntry]) {
        processTimes[idEntry] = stol(buffer) / sysconf(_SC_CLK_TCK);
        idEntry++;
      }
    }
  }
  return processTimes;
}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  // read from filesystem
  string cpu, user, nice, system, idle, iowait, irq, softirq, steal;
  string line;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> cpu >> user >> nice >> system >> idle >> iowait >> irq >>
        softirq >> steal;
  }

  return stol(user) + stol(nice) + stol(system) + stol(irq) + stol(softirq) +
         stol(steal);
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  // read from filesystem
  string cpu, user, nice, system, idle, iowait;
  string line;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> cpu >> user >> nice >> system >> idle >> iowait;
  }
  // calculate percentage
  return stol(idle) + stol(iowait);
}

/* not needed, shifted calculation into Processor::Utilization
// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() { return {}; }
*/

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  string result =
      LineParser(kProcDirectory + kStatFilename, "processes", {}, {});
  if (result.empty()) return -1;
  return std::stoi(result);
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  string result =
      LineParser(kProcDirectory + kStatFilename, "procs_running", {}, {});
  if (result.empty()) return -1;
  return std::stoi(result);
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) {
  string line;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + "/" +
                           kCmdlineFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    return line;
  }
  return " ";
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) {
  // VmSize converted to MB
  return std::to_string(
      std::stol(LineParser(
          kProcDirectory + std::to_string(pid) + "/" + kStatusFilename,
          "VmSize:", {}, {})) /
      1024);
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) {
  return LineParser(
      kProcDirectory + std::to_string(pid) + "/" + kStatusFilename, "Uid:", {},
      {});
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) {
  string uid = Uid(pid);
  string line;
  string value, buffer, key;
  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> value >> buffer >> key) {
        if (key == uid) return value;
      }
    }
  }

  return "notFound";
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long int LinuxParser::UpTime(int pid) {
  vector<long> processTimes = ProcessTimes(pid);
  return UpTime() - processTimes[4];
}


// Helper Function for line parsing
string LinuxParser::LineParser(string path, string qkey, vector<char> repBef,
                               vector<char> repAft) {
  string line;
  string key;
  string value;
  std::ifstream filestream(path);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      if (!repBef.empty()) {
        for (unsigned long int id = 0; id < (repBef.size() - 1); id += 2) {
          std::replace(line.begin(), line.end(), repBef[id], repBef[id + 1]);
        }
      }
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == qkey) {
          if (!repAft.empty()) {
            for (unsigned long int id = 0; id < (repAft.size() - 1); id += 2) {
              std::replace(value.begin(), value.end(), repAft[id],
                           repAft[id + 1]);
            }
          }
          return value;
        }
      }
    }
  }
  return "0";
}