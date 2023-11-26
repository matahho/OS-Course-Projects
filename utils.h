#ifndef UTILS_h
#define UTILS_h

#include <string>
#include <vector>
#include <iostream>
#include <filesystem>

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

std::vector<std::string> getAllDirectories(std::string path);


std::string fifoFileName (std::string path , std::string builingName, std::string com);


void makingFifos(std::vector<std::string>allBuilingName , std::vector<std::string>allComodityName , std::vector<std::string> &fifos);


#endif 
