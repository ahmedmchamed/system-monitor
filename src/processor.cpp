#include "processor.h"
#include "linux_parser.h"

using std::stof;
//Return the aggregate CPU utilization
float Processor::Utilization() { 
    return stof(LinuxParser::CpuUtilization().at(0));
}