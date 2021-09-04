#include <unistd.h>
#include <vector>
#include <filesystem>

#include "system.h"
#include "linux_parser.h"

using std::stof;
using std::stoi;
using std::stol;
using std::string;
using std::to_string;
using std::vector;
using std::ifstream;

string LinuxParser::OperatingSystem() 
{
	string line;
	string key;
	string value;
	ifstream filestream {kOSPath};

	if (filestream.is_open()) 
	{
		while (std::getline(filestream, line))
		{
			std::replace(line.begin(), line.end(), ' ', '_');
			std::replace(line.begin(), line.end(), '=', ' ');
			std::replace(line.begin(), line.end(), '"', ' ');
			std::istringstream linestream(line);
			while (linestream >> key >> value) 
			{
				if (key == "PRETTY_NAME") {
					std::replace(value.begin(), value.end(), '_', ' ');
					return value;
				}
			}
		}
	}

	return value;
}

string LinuxParser::Kernel() 
{
	string os, kernel;
	string line;
	ifstream stream {kProcDirectory + kVersionFilename};

	if (stream.is_open())
	{
		std::getline(stream, line);
		std::istringstream linestream(line);
		linestream >> os >> kernel;
	}

	return kernel;
}

vector<int> LinuxParser::Pids() 
{
	vector<int> pids;
	string pidDirectory;

	for (auto &entry : std::filesystem::directory_iterator(kProcDirectory))
	{
		if (entry.is_directory())
		{
			pidDirectory = entry.path().filename().string();
			if (std::all_of(pidDirectory.begin(), pidDirectory.end(), isdigit))
			{
				int pid {stoi(pidDirectory)};
				pids.push_back(pid);
			}
		}
	}
	return pids;
}

float LinuxParser::MemoryUtilization() 
{ 
	//Flag to stop parsing the file
	bool dataFound {false};
	//line string to read the stream from the file
	string line;
	//key, unit strings to read each item per line
	string key, value, unit;
	//int values to store the actual memory numbers
	float memtotal {0.0f}, memfree {0.0f};
	//fstream for the memmory utlisation file
	ifstream memoryUtilisation {kProcDirectory + kMeminfoFilename};
	//vector to store the memory allocation values
	vector<string> sizes{};

	if (memoryUtilisation.is_open())
	{
		while (getline(memoryUtilisation, line) && dataFound)
		{
			std::istringstream inputStream {line};
			inputStream >> key >> value >> unit;

			sizes.push_back(value);

			if (key == "Buffers:")
			{
				dataFound = true;
			}
		}
	}

	//Assign values to each respective memory allocation
	if (!sizes.empty())
	{
		memtotal = stof(sizes.at(0));
		memfree = stof(sizes.at(1));
		//memavailable = std::stof(sizes[2]);
	}

	//Calculate total memory allocation
	float memory {(memtotal - memfree) / memtotal};

	return memory;
}

long LinuxParser::UpTime() 
{
	string line, uptime_value;

	//Read the input from the uptime file in the proc directory
	ifstream input(kProcDirectory + kUptimeFilename);

	if (input.is_open()) 
	{
		while(getline(input, line))
		{
			std::istringstream i_input(line);
			i_input >> uptime_value;
		}
	}

	long uptime {stol(uptime_value)};

	return uptime;
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() 
{
	string line, label;
	long jiffies_number{0};
	vector<string> processes;

	ifstream input(kProcDirectory + kStatFilename);

	if (input.is_open() && getline(input, line))
	{
		std::istringstream i_input(line);

		while (i_input >> label)
		{
			processes.push_back(label);
		}
	};

	//Declaring labels for each jiffy
	//Assigning a label for each jiffy from the processes vector
	long user {stol(processes.at(1))};
	long nice {stol(processes.at(2))};
	long system {stol(processes.at(3))};
	long idle {stol(processes.at(4))};
	long iowait {stol(processes.at(5))};
	long irq {stol(processes.at(6))};
	long softirq {stol(processes.at(7))};
	long steal {stol(processes.at(8))};
	long guest {stol(processes.at(9))};
	long guest_nice {stol(processes.at(10))};

	jiffies_number = user + nice + system + idle + iowait 
			+ irq + softirq + steal + guest + guest_nice;

	return jiffies_number;
}

long LinuxParser::ActiveJiffies(int pid) 
{ 
	string line, label;
	vector<string> processes;

	//Defining a path object for a pid directory
	string pid_path = kProcDirectory + "/" + to_string(pid) + kStatFilename;

	ifstream pid_input(pid_path);
	while (getline(pid_input, line))
	{
		std::istringstream p_input(line);
		while (p_input >> label)
		{
			processes.push_back(label);
		}
	}

	//Declaring the required jiffy objects for the PID
	long utime {stol(processes.at(13))};
	long stime {stol(processes.at(14))};
	long cutime {stol(processes.at(15))};
	long cstime {stol(processes.at(16))};
	long starttime {stol(processes.at(21))};

	long pid_active_total {utime + stime + cutime + cstime + starttime};

	return pid_active_total;
}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies()
{
	string line, label;
	vector<string> processes;

	ifstream input(kProcDirectory + kStatFilename);

	//only need the aggregate info, and so discarding the while loop
	if (input.is_open() && getline(input, line))
	{
		std::istringstream i_input(line);
		while (i_input >> label)
		{
			processes.push_back(label);
		}
	};

	long user {0}, nice {0}, system {0}, idle{0}, iowait{0}, irq{0}, 
	softirq{0}, steal{0}, guest{0}, guest_nice{0};

	long user_time{0}, nice_time{0}, idle_all_time{0}, system_all_time{0}, 
		virt_all_time{0}, total_time{0};

	if (!processes.empty())
	{
		user = stol(processes.at(1));
		nice = stol(processes.at(2));
		system = stol(processes.at(3));
		idle = stol(processes.at(4));
		iowait = stol(processes.at(5));
		irq = stol(processes.at(6));
		softirq = stol(processes.at(7));
		steal = stol(processes.at(8));
		guest = stol(processes.at(9));
		guest_nice = stol(processes.at(10));
	}

	//Calculating the active times, using aggregate values
	user_time = user - guest;
	nice_time = nice - guest_nice;
	idle_all_time = idle + iowait;
	system_all_time = system + irq + softirq;
	virt_all_time = guest + guest_nice;

	//Calculating total active time
	total_time = user_time + nice_time + idle_all_time + system_all_time + 
			virt_all_time;

	return total_time;
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() 
{ 
	string line, label, cpu_label, user, nice, system, idle, iowait, irq, 
			softirq, steal, guest, guest_nice;
	
	//File stream
	ifstream input(kProcDirectory + kStatFilename);

	//Going to avoid using a vector for this one, for a change
	if (input.is_open() && getline(input, line))
	{
		std::istringstream i_input(line);

		i_input >> cpu_label >> user >> nice >> system >> idle
				>> iowait >> irq >> softirq >> steal >> guest >> guest_nice;
	};

	//Defining and calculating the idle time, using aggregate values
	long idle_time {stol(idle) + stol(iowait)};

	return idle_time;

}

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() 
{ 
	//Declaring strings for the getline operation
	string cpu, line, value;
	//Vector to hold the aggregate cpu values
	vector<string> processes;

	ifstream input(kProcDirectory + kStatFilename);

	if (input.is_open() && getline(input, line))
	{
		std::istringstream cpuDetails(line);

		cpuDetails >> cpu;

		while (cpuDetails >> value)
		{
			processes.push_back(value);
		}
	}

	if (!processes.empty())
	{
		return processes;
	}
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() 
{ 
	string line;
	vector<string> processNumber;

	ifstream input(kProcDirectory + kStatFilename);

	while(getline(input, line))
	{
		if (line.find("processes") != string::npos)
		{
			processNumber.push_back(line);
		} 
		else 
		{
			return 0;
		}
	}
	
	int processes{0};

	processes = stoi(processNumber[1]);

	return processes;
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses()
{ 
	string line;

	vector<string> processes;

	ifstream input(kProcDirectory + kStatFilename);

	while (getline(input, line))
	{
		if (!line.find("procs_running"))
		{
			processes.push_back(line);
		}
	}

	int procs_number{0};

	procs_number = stoi(processes[1]);

	return procs_number;
}

// TODO: Read and return the command associated with a process
string LinuxParser::Command(int pid)
{
	string line, command;

	string pid_path(kProcDirectory + to_string(pid) + kCmdlineFilename);

	ifstream input(pid_path);

	while (getline(input, line))
	{
		std::istringstream i_input(line);
		i_input >> command;
	}

	return command;
}

// TODO: Read and return the memory used by a process
string LinuxParser::Ram(int pid) 
{
	string line, vm_size_label, vm_size_value;

	string pid_path(kProcDirectory + to_string(pid) + kStatusFilename);

	ifstream input(pid_path);

	//Reading the memory used by reading in VmSize
	while (getline(input, line))
	{
		std::istringstream i_input(line);

		if (!line.find("VmSize"))
		{
			i_input >> vm_size_label >> vm_size_value;
		}
	}

	//Need to convert the memory to megabytes
	float vm_size_convert = stof(vm_size_value);

	float vm_size_MB = vm_size_convert / 1000;

	string vm_size_final = to_string(vm_size_MB);

	return vm_size_final;
}

// TODO: Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) 
{
	string line, uid_label, uid;

	string pid_path(kProcDirectory + to_string(pid) + kStatusFilename);

	ifstream input(pid_path);

	while (getline(input, line))
	{
		std::istringstream i_input(line);

		if (!line.find("Uid"))
		{
			i_input >> uid_label >> uid;
		}
	}

	return uid;
}

// TODO: Read and return the user associated with a process
string LinuxParser::User(int pid) 
{
	string line, uid;
	vector<string> username_line;

	uid = LinuxParser::Uid(pid);

	ifstream input(kPasswordPath);

	while (getline(input, line))
	{
		//replacing the colon delimiter with a space to
		//make it easier to parse through the line for the 
		//username
		std::replace(line.begin(), line.end(), ':', ' ');

		if (!line.find(uid))
		{
			username_line.push_back(line);
		}
	}

	string username(username_line[0]);

	return username;
}

// TODO: Read and return the uptime of a process
long int LinuxParser::UpTime(int pid) 
{
	string line, value;
	//Will use vector to extract uptime (21st) element
	vector<string> pid_stat_line;

	string pid_path(kProcDirectory + to_string(pid) + kStatFilename);

	ifstream input(pid_path);

	while (getline(input, line ))
	{
			pid_stat_line.push_back(line);
	}

	long uptime_ticks {stol(pid_stat_line[21])};

	long pid_uptime{uptime_ticks / sysconf(_SC_CLK_TCK)};

	return pid_uptime;
}
