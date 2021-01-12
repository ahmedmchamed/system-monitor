#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>
#include <fstream>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

// TODO: Return this process's ID
Process::Process(int pid): pid(pid) {}

int Process::Pid() { 
    return pid;
}

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() { 

    string value;
    string pidStatLine;
    vector<string> pidStatValues;
    
    std::ifstream pidStatFile(LinuxParser::kProcDirectory + "/" + 
    to_string(pid) + LinuxParser::kStatFilename);

    std::getline(pidStatFile, pidStatLine);

    std::istringstream lineStream(pidStatLine);

    while (lineStream >> value) {
        pidStatValues.push_back(value);
    }

    long uTime = std::stol(pidStatValues[13]);
    long sTime = std::stol(pidStatValues[14]);
    long cuTime = std::stol(pidStatValues[15]);
    long csTime = std::stol(pidStatValues[16]);
    long startTime = std::stol(pidStatValues[21]);

}

// TODO: Return the command that generated this process
string Process::Command() { return string(); }

// TODO: Return this process's memory utilization
string Process::Ram() { return string(); }

// TODO: Return the user (name) that generated this process
string Process::User() { return string(); }

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() { return 0; }

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a[[maybe_unused]]) const { return true; }