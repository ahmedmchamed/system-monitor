#include <dirent.h>
#include <unistd.h>
#include <vector>
#include <filesystem>

#include "linux_parser.h"

using std::stof;
using std::stoi;
using std::stol;
using std::string;
using std::to_string;
using std::vector;
using std::ifstream;

// vector<long> jiffiesHelper() 
//{

// 	string line, cpu, value;
// 	vector<long> processes;

// 	ifstream input(kProcDirectory + kStatFilename);

// 	std::getline(input, line);
// 	std::istringstream lineStream(line);

// 	lineStream >> cpu;

// 	while(lineStream >> value) {
// 		processes.push_back(stol(value));
// 	}

// 	if (processes.begin() != processes.end()) return processes;

// }

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() 
{
	string line;
	string key;
	string value;
	ifstream filestream(kOSPath);
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

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() 
{
	string os, kernel;
	string line;
	ifstream stream(kProcDirectory + kVersionFilename);
	if (stream.is_open()) {
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
	// DIR* directory = opendir(kProcDirectory.c_str());
	// dirent* file;
	// while ((file = readdir(directory)) != nullptr) {
    // // Is this a directory?
	// 	if (file->d_type == DT_DIR) {
	// 	// Is every character of the name a digit?
	// 		string filename(file->d_name);
	// 		if (std::all_of(filename.begin(), filename.end(), isdigit))
	// 		{
	// 			int pid = stoi(filename);
	// 			pids.push_back(pid);
	// 		}
	// 	}
	// }
	// closedir(directory);
	for (auto &entry : std::filesystem::directory_iterator(kProcDirectory))
	{
		if (entry.is_directory())
		{
			pidDirectory = entry.path().filename().string();
			if (std::all_of(pidDirectory.begin(), pidDirectory.end(), isdigit))
			{
				int pid = stoi(pidDirectory);
				pids.push_back(pid);
			}
		}
	}
	return pids;
}

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() 
{ 
	//line string to read the stream from the file
	string line;
	//key, unit strings to read each item per line
	string key, value, unit;
	//int values to store the actual memory numbers
	float memtotal{0.0f}, memfree{0.0f}, memavailable{0.0f};
	//fstream for the memmory utlisation filei
	ifstream input(kProcDirectory + kMeminfoFilename);
	//vector to store the memory allocation values
	vector<string> sizes{};

	if (input.is_open()) {
			while (getline(input, line)) {
					std::istringstream i_input(line);
					i_input >> key >> value >> unit;
					sizes.push_back(value);
			}
	}
	//Assign values to each respective memory allocation
	memtotal = stof(sizes[0]);
	memfree = stof(sizes[1]);
	//memavailable = std::stof(sizes[2]);

	//Calculate total memory allocation
	float memory;
	memory = (memtotal - memfree) / memtotal;

	return memory;
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() 
{
	string line;
	string uptime_value, idle_value;

	//Read the input from the uptime file in the proc directory
	ifstream input(kProcDirectory + kUptimeFilename);

	if (input.is_open()) {
			while(getline(input, line)) {
					std::istringstream i_input(line);
					i_input >> uptime_value >> idle_value;
			}
	}

	long uptime{0}, idle{0};

	uptime = stol(uptime_value);
//		idle = std::stol(idle_value);

	return uptime;

}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() 
{
	string line, label;
	long jiffies_number{0};
	vector<string> processes;

	ifstream input(kProcDirectory + kStatFilename);

	getline(input, line);
	std::istringstream i_input(line);

	while (i_input >> label)
			processes.push_back(label);
	
	//Declaring labels for each jiffy
	long user{0}, nice{0}, system{0}, idle{0}, iowait{0}, irq{0}, 
	softirq{0}, steal{0}, guest{0}, guest_nice{0};

	//Assigning a label for each jiffy from the processes vector
	user = stol(processes[1]);
	nice = stol(processes[2]);
	system = stol(processes[3]);
	idle = stol(processes[4]);
	iowait = stol(processes[5]);
	irq = stol(processes[6]);
	softirq = stol(processes[7]);
	steal = stol(processes[8]);
	guest = stol(processes[9]);
	guest_nice = stol(processes[10]);

	jiffies_number = user + nice + system + idle + iowait 
			+ irq + softirq + steal + guest + guest_nice;

	return jiffies_number;
}

// TODO: Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) 
{ 
	string line, label;
	vector<string> processes;

	//Defining a path object for a pid directory
	string pid_path = kProcDirectory + "/" + to_string(pid) + kStatFilename;

	ifstream pid_input(pid_path);
	while (getline(pid_input, line)) {
			std::istringstream p_input(line);
			while (p_input >> label) {
					processes.push_back(label);
			}
	}

	//Declaring the required jiffy objects for the PID
	long utime{0}, stime{0}, cutime{0}, cstime{0}, starttime{0};

	utime = stol(processes[13]);
	stime = stol(processes[14]);
	cutime = stol(processes[15]);
	cstime = stol(processes[16]);
	starttime = stol(processes[21]);

	long pid_active_total{0};

	pid_active_total = utime + stime + cutime 
			+ cstime + starttime;

	return pid_active_total;
}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies()
{
	// string line, label;
	// vector<string> processes;

	// ifstream input(kProcDirectory + kStatFilename);

	// //only need the aggregate info, and so discarding the while loop
	// getline(input, line);
	// std::istringstream i_input(line);
	// while (i_input >> label) {
	// 		processes.push_back(label);
	// }
// item{0}, idle{0}, iowait{0}, irq{0}, 
	// softirq{0}, steal{0}, guest{0}, guest_nice{0};

	// long user_time{0}, nice_time{0}, idle_all_time{0}, system_all_time{0}, 
	// 	 virt_all_time{0}, total_time{0};

	// user = stol(processes[1]);
	// nice = stol(processes[2]);
	// system = stol(processes[3]);
	// idle = stol(processes[4]);
	// iowait = stol(processes[5]);
	// irq = stol(processes[6]);
	// softirq = stol(processes[7]);
	// steal = stol(processes[8]);
	// guest = stol(processes[9]);
	// guest_nice = stol(processes[10]);

	// //Calculating the active times, using aggregate values
	// user_time = user - guest;
	// nice_time = nice - guest_nice;
	// idle_all_time = idle + iowait;
	// system_all_time = system + irq + softirq;
	// virt_all_time = guest + guest_nice;

	// //Calculating total active time
	// total_time = user_time + nice_time + idle_all_time + system_all_time + 
	// 		virt_all_time;

	// return total_time;

	vector<string> processes = CpuUtilization();

	long userTime = stol(processes[kUser_]) - stol(processes[kGuest_]); 
	// long niceTime = stol()
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() 
{ 
	string line, label;
	string cpu_label, user, nice, system, idle, iowait, irq, 
			softirq, steal, guest, guest_nice;
	
	//File stream
	ifstream input(kProcDirectory + kStatFilename);

	//Going to avoid using a vector for this one, for a change
	getline(input, line);
	std::istringstream i_input(line);

	i_input >> cpu_label >> user >> nice >> system >> idle
			>> iowait >> irq >> softirq >> steal >> guest >> guest_nice;

	//Defining and calculating the idle time, using aggregate values
	long idle_time{0};

	idle_time = stol(idle) + stol(iowait);

	return idle_time;

}

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() 
{ 
	//Declaring strings for the getline operation
	string cpu, line, value;
	//Vector to hold the aggregate cpu values
	vector<string> processes;
	//Vector to return the cpu percentage calculation
	vector<string> cpu_percentage;

	ifstream input(kProcDirectory + kStatFilename);

	getline(input, line);
	std::istringstream cpuDetails(line);

	cpuDetails >> cpu;

	while (cpuDetails >> value) {
			processes.push_back(value);
	}

	if (processes.begin() != processes.end()) {
		return processes;
	}

	//Declaring long objects to hold the converted cpu jiffies
	// long user{0}, nice{0}, system{0}, idle{0}, iowait{0}, irq{0},
	// 	   softirq{0}, steal{0}, guest{0}, guest_nice{0};

	// user = stol(processes[1]);
	// nice = stol(processes[2]);
	// system = stol(processes[3]);
	// idle = stol(processes[4]);
	// iowait = stol(processes[5]);
	// irq = stol(processes[6]);
	// softirq = stol(processes[7]);
	// steal = stol(processes[8]);
	// guest = stol(processes[9]);
	// guest_nice = stol(processes[10]);

	// //Objects to calculate the cpu utilisation percentage as per Stack example
	// long idle_jiffies{0}, non_idle_jiffies{0}, total{0}, cpu_utilisation{0};

	// //Idle calculation
	// idle_jiffies = idle + iowait;

	// //Non idle calculation
	// non_idle_jiffies = user + nice + system + irq + softirq + steal;

	// //Total calculation
	// total = idle_jiffies + non_idle_jiffies;

	// cpu_utilisation = (total - idle) / total;

	// //Converting the cpu utilisation value above into a string to
	// //push back into the vector to be returned
	// string cpu_utilisation_string(to_string(cpu_utilisation));
	
	// cpu_percentage.push_back(cpu_utilisation_string);

	// return cpu_percentage;
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() 
{ 
	string line;
	vector<string> processNumber;

	ifstream input(kProcDirectory + kStatFilename);

	while(getline(input, line)) {
			if (line.find("processes") != string::npos) {
					processNumber.push_back(line);
			} else {
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

	while (getline(input, line)) {
			if (!line.find("procs_running")) {
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

	while (getline(input, line)) {
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
		while (getline(input, line)) {
				std::istringstream i_input(line);

				if (!line.find("VmSize")) {
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

		while (getline(input, line)) {
				std::istringstream i_input(line);

				if (!line.find("Uid")) {
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

		while (getline(input, line)) {
				//replacing the colon delimiter with a space to
				//make it easier to parse through the line for the 
				//username
				std::replace(line.begin(), line.end(), ':', ' ');

				if (!line.find(uid)) {
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

		while (getline(input, line )) {
				pid_stat_line.push_back(line);
		}

		long uptime_ticks = stol(pid_stat_line[21]);

		long pid_uptime{0};

		pid_uptime = uptime_ticks / sysconf(_SC_CLK_TCK);

		return pid_uptime;
}
