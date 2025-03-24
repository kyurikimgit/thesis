#ifndef _ENVIRONMENT_H
#define _ENVIRONMENT_H

#include <string>

class Environment {
public:
    std::string loader;
    std::string algorithm;
    bool online;
    int stepWidth;
    char testType;
    double epsilon;
    int machines;
    double machineSize;
    int startJobs;
    std::string file;
    bool randomJobSelection;
    int runs;
    int cpus;
    int ram;
	int maxLocalDisk;
	int maxNetwork;
	float intervalMin;
	float intervalMax;
//For the vbpInstance
    int vbpClass;
    int vbpSize;
    int vbpDimension;
    int vbpInstance;
};

#endif
