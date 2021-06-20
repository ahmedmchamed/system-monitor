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
Process::Process(int pid): pid(pid) {}

int Process::Pid() { 
    return pid;
}

//Return this process's CPU utilization
float Process::CpuUtilization() 
{
    return cpuUtilization;
}

//Return the command that generated this process
string Process::Command()
{ 
    return commandName; 
}

//Return this process's memory utilization
string Process::Ram()
{ 
    return ramUsage; 
}

//Return the user (name) that generated this process
string Process::User()
{ 
    return username;
}

//Return the age of this process (in seconds)
long int Process::UpTime()
{ 
    return uptime;
}

//Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& rhs) const
{
    //Returning what seems to be the sensible things to compare against - should double check though
    return cpuUtilization < rhs.cpuUtilization &&
        ramUsage < rhs.ramUsage &&
        uptime < rhs.uptime;
}