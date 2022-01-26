#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

#include <experimental/filesystem>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using std::stof;
using std::string;
using std::to_string;
using std::vector;
namespace fs = std::experimental::filesystem;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;

  for (auto &entry : fs::directory_iterator(kProcDirectory)) {
    string filename = entry.path().filename();
    if (std::all_of(filename.begin(), filename.end(), isdigit)) {
      int pid = stoi(filename);
      pids.push_back(pid);
    }
  }

  return pids;
}

// TODO(Done): Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  string path = kProcDirectory + kMeminfoFilename;
  int memFree = GetDataFromPath(path, "MemFree:");

  int memTotal = GetDataFromPath(path, "MemTotal:");
  int memUtil = memTotal - memFree;

  float percentUtil = 0.0;
  if (memTotal > 0) percentUtil = (float)(memUtil) / memTotal;
  return percentUtil;
}

// TODO(Done): Read and return the system uptime
long LinuxParser::UpTime() {
  long totalUpTime = 0;

  string token;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    while (std::getline(stream, token, ' ')) {
      totalUpTime += std::stol(token);
    }
  }
  return totalUpTime;
}

// TODO(Done): Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  vector<string> cpuUtilStats = CpuUtilization();
  int num_jiffies = 0;
  for (auto stat : cpuUtilStats) {
    num_jiffies += std::stoi(stat);
  }
  return num_jiffies;
}

// TODO(Done): Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) {
  return GetProcessStat(pid, ProcessUtilStates::utime_) +
         GetProcessStat(pid, ProcessUtilStates::stime_) +
         GetProcessStat(pid, ProcessUtilStates::cutime_) +
         GetProcessStat(pid, ProcessUtilStates::cstime_);
}

// TODO(Done): Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { return Jiffies() - IdleJiffies(); }

// TODO(Done): Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  vector<string> cpuUtilStats = CpuUtilization();
  return std::stoi(cpuUtilStats[CPUStates::kIdle_]) +
         std::stoi(cpuUtilStats[CPUStates::kIOwait_]);
}

// TODO(done): Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  vector<string> cpuUtilStats(10, "0");
  string line, token;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      if (line.rfind("cpu", 0) == 0) {
        std::istringstream linestream(line);
        int i = 0;
        while (std::getline(linestream, token, ' ')) {
          if (token.rfind("cpu", 0) != 0 && token != "") {
            int totalVal = std::stoi(cpuUtilStats[i]) + std::stoi(token);
            cpuUtilStats[i] = std::to_string(totalVal);
            i++;
          }
        }
      }
    }
  }
  return cpuUtilStats;
}

// TODO(Done): Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  return GetDataFromPath(kProcDirectory + kStatFilename, "processes");
}

// TODO(Done): Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  return GetDataFromPath(kProcDirectory + kStatFilename, "procs_running");
}

int LinuxParser::GetDataFromPath(string path, string inputKey) {
  string line;
  string key;
  string value;
  int processData = 0;
  std::ifstream filestream(path);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == inputKey) {
          processData = std::stoi(value);
          return processData;
        }
      }
    }
  }
  return processData;
}

// TODO(Done): Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) {
  string cmd;
  string token;
  std::ifstream filestreamUid(kProcDirectory + to_string(pid) +
                              kCmdlineFilename);
  if (filestreamUid.is_open())
    while (std::getline(filestreamUid, cmd))
      ;
  return cmd;
}

// TODO(Done): Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) {
  string line, key, vmsize, unit;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      if (line.rfind("VmSize:", 0) == 0) {
        std::istringstream linestreamUid(line);
        linestreamUid >> key >> vmsize >> unit;
        break;
      }
    }
  }

  std::stringstream stream;
  stream << std::fixed << std::setprecision(2) << ((float)stoi(vmsize)) / 1000;

  return stream.str();
}

// TODO(Done): Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) { return User(pid); }

// TODO(Done): Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) {
  string line, uid;
  std::ifstream filestreamUid(kProcDirectory + to_string(pid) +
                              kStatusFilename);
  if (filestreamUid.is_open()) {
    while (std::getline(filestreamUid, line)) {
      if (line.rfind("Uid:", 0) == 0) {
        std::istringstream linestreamUid(line);
        while (linestreamUid >> uid) {
          if (uid.rfind("Uid:", 0) != 0 && uid != "") break;
        }
      }
    }
  }

  string username = "";
  std::ifstream filestreamUsername(kPasswordPath);
  if (filestreamUsername.is_open()) {
    bool isFound = false;
    while (std::getline(filestreamUsername, line)) {
      std::istringstream linestreamUsername(line);
      int i = 0;
      string token;
      while (std::getline(linestreamUsername, token, ':')) {
        if (i == 0) username = token;

        if (i == 2 && token == uid) {
          isFound = true;
          break;
        }
        i++;
      }
      if (isFound) break;
    }
  }
  return username;
}

// TODO(Done): Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) {
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  int i = 0;
  string token;
  long startTime = 0;
  while (std::getline(filestream, token, ' ')) {
    if (i == 21) {
      startTime = stoi(token) / sysconf(_SC_CLK_TCK);
      break;
    }
    i++;
  }
  return startTime;
}

int LinuxParser::GetProcessStat(int pid, int statnumber) {
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  int i = 1;
  string token;
  int numvalue;
  while (std::getline(filestream, token, ' ')) {
    if (i == statnumber) {
      numvalue = stoi(token);
      // std::cout<<pid<<":"<<statnumber<<":"<<token<<":"<<numvalue<<"\n";

      break;
    }
    i++;
  }
  return numvalue;
}
