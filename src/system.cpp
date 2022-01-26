#include <unistd.h>
#include <cstddef>
#include <set>
#include <string>
#include <vector>

#include "process.h"
#include "processor.h"
#include "system.h"
#include "linux_parser.h"
#include "format.h"

using namespace std;
/*You need to complete the mentioned TODOs in order to satisfy the rubric criteria "The student will be able to extract and display basic data about the system."

You need to properly format the uptime. Refer to the comments mentioned in format. cpp for formatting the uptime.*/

// TODO: Return the system's CPU
Processor& System::Cpu() { return cpu_; }

// TODO: Return a container composed of the system's processes
vector<Process>& System::Processes() { 
    vector<int> pids = LinuxParser::Pids();
    processes_.clear();
    for(int p: pids){
        processes_.emplace_back(p);
    }
    
    sort(processes_.rbegin(), processes_.rend());

    return processes_; 
}

// TODO(Done): Return the system's kernel identifier (string)
string System::Kernel() { 
    return LinuxParser::Kernel(); 
}

// TODO: Return the system's memory utilization
float System::MemoryUtilization() { 
    return LinuxParser::MemoryUtilization();
}

// TODO(Done): Return the operating system name
string System::OperatingSystem() { 
    return LinuxParser::OperatingSystem(); 
}

// TODO(Done): Return the number of processes actively running on the system
int System::RunningProcesses() { 
    return LinuxParser::RunningProcesses();
}

// TODO(Done): Return the total number of processes on the system
int System::TotalProcesses() { 
    return LinuxParser::TotalProcesses(); 
}

// TODO(Done): Return the number of seconds since the system started running
long int System::UpTime() { 
    return LinuxParser::UpTime();
}
