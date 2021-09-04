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

//Return this process's ID
Process::Process(int processId): pid(processId) {}

int Process::Pid()
{ 
    return pid;
}

//Return this process's CPU utilization
long Process::CpuUtilization() 
{
    return LinuxParser::ActiveJiffies(pid);
}

//Return the command that generated this process
string Process::Command()
{ 
    return LinuxParser::Command(pid); 
}

//Return this process's memory utilization
string Process::Ram()
{ 
    return LinuxParser::Ram(pid); 
}

//Return the user (name) that generated this process
string Process::User()
{ 
    return LinuxParser::User(pid);
}

//Return the age of this process (in seconds)
long int Process::UpTime()
{ 
    return LinuxParser::UpTime(pid);
}

//Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& rhs) const
{
    //Returning what seems to be the sensible things to compare against - should double check though
    return cpuUtilization < rhs.cpuUtilization &&
        ramUsage < rhs.ramUsage &&
        uptime < rhs.uptime;
}