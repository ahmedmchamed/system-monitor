#ifndef PROCESS_H
#define PROCESS_H

#include <string>
/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
    public:
        Process() = default;
        Process(int);
        int Pid();
        std::string User();
        std::string Command();
        float CpuUtilization();
        std::string Ram();
        long int UpTime();
        bool operator<(Process const& rhs) const;

  //Declare any necessary private members
    private:
        int pid {0};
        std::string username;
        std::string commandName;
        float cpuUtilization {0};
        std::string ramUsage;
        long uptime {0};
};

#endif