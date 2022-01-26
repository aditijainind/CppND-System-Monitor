#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>

#include "process.h"
#include "linux_parser.h"
using std::string;
using std::to_string;
using std::vector;

// TODO(Done): Return this process's ID
int Process::Pid() const { return pid_; }

// TODO(Done): Return this process's CPU utilization
float Process::CpuUtilization() const { 
    long total_time = LinuxParser::ActiveJiffies(Pid());
    long uptime = LinuxParser::UpTime();
    long starttime = LinuxParser::GetProcessStat(Pid(), LinuxParser::ProcessUtilStates::starttime_);
    long seconds = uptime - ((float)starttime / sysconf(_SC_CLK_TCK));
    float cpu_usage = (float)(((float)total_time / sysconf(_SC_CLK_TCK)) / seconds);
    //std::cout<<"cpu_usage"<<cpu_usage<<"\n";
    return cpu_usage;
}

// TODO(Done): Return the command that generated this process
string Process::Command(){ 
    return LinuxParser::Command(Pid()); 
}

// TODO(Done): Return this process's memory utilization
string Process::Ram() { return LinuxParser::Ram(Pid()); }

// TODO(Done): Return the user (name) that generated this process
string Process::User() { 
    return LinuxParser::Uid(Pid());
}

// TODO(Done): Return the age of this process (in seconds)
long int Process::UpTime() { return LinuxParser::UpTime(Pid()); }

// TODO(Done): Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a) const { 
    return CpuUtilization()<a.CpuUtilization(); 
}