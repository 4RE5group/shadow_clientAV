#ifndef SETTINGSFILEHANDLER_H_
#define SETTINGSFILEHANDLER_H_

#include <limits>

char* getSetting(char* name,  char* path);
void writeSetting(char* name, char* value, char* path);



std::string get_hash_from_line(const std::string& line) {
    auto pos = line.find(":");
    if (pos != std::string::npos) {
        return line.substr(0, pos);  // Return only the hash part
    }
    return "";  // Return empty string if format is invalid
}

// Function to search for a hash in the file using binary search
std::string search_hex_hash(const std::string& file_path, const std::string& target_hash) {
    std::ifstream file(file_path, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        std::cerr << "Error: Unable to open file " << file_path << std::endl;
        return "";
    }

    // Get the size of the file
    std::streampos file_size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::streampos start = 0;
    std::streampos end = file_size;
    std::streampos mid;
    std::string line;

    while (start < end) {
        // Calculate the midpoint
        mid = start + (end - start) / 2;

        // Move to the midpoint and adjust to the start of a line
        file.seekg(mid);
        if (mid > 0) {
            file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');  // Skip to the start of the next line
        }

        // Read the current line
        if (!std::getline(file, line)) {
            break;  // If unable to read, break the loop
        }

        // Extract the hash from the line
        std::string current_hash = get_hash_from_line(line);

        // Compare the extracted hash with the target hash
        if (current_hash == target_hash) {
            auto pos = line.find(": ");
            if (pos != std::string::npos) {
                return line.substr(pos+2, line.length()); // return malware name
            }
            return line;  // Target hash found
        } else if (current_hash < target_hash) {
            start = file.tellg();  // Move to the right half
        } else {
            end = mid;  // Move to the left half
        }
    }

    return "";  // Target hash not found
}


#endif