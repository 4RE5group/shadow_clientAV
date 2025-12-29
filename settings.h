#ifndef SETTINGSFILEHANDLER_H_
#define SETTINGSFILEHANDLER_H_

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <stdexcept>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <cstring>

#include <limits>
#include <string>
#include <fstream>
#include <stdexcept>
#include <ios>

char* getSetting(char* name,  char* path);
void writeSetting(char* name, char* value, char* path);

std::string get_hash_from_line(const std::string& line);
std::string search_hex_hash(const std::string& file_path, const std::string& target_hash);


#endif
