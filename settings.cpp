#include "settings.h"

using namespace std;

string remove_space(string input) {
    input.erase(remove(input.begin(), input.end(), '	'), input.end());
    input.erase(remove(input.begin(), input.end(), ' '), input.end());
    return input;
}
string splitintwo(const string& str, char delimiter, bool beforeDelimiter = true) {
    size_t pos = str.find(delimiter);
    if (pos != string::npos) {
        if (beforeDelimiter) {
            return str.substr(0, pos);
        } else {
            return str.substr(pos + 1);
        }
    }
    return str; // Return an empty string if delimiter is not found
}

char* delete_crlf(const char* input) {
    size_t input_length = strlen(input);
    char* result = (char*)malloc(input_length + 1); // Allocate memory for the result
    if (result == NULL) {
        return NULL; // Memory allocation failed
    }

    size_t j = 0; // Index for the result buffer
    for (size_t i = 0; i < input_length; ++i) {
        // Look for "\r\n" and skip them
        if (input[i] == '\r' && i + 1 < input_length && input[i + 1] == '\n') {
            ++i; // Skip the '\n' too
            continue;
        }

        // Copy character to the result buffer
        result[j++] = input[i];
    }
    result[j] = '\0'; // Null-terminate the result string

    return result;
}
string trim(const string& str) {
    // Find the first non-space character
    size_t start = str.find_first_not_of(" \t\n\r");

    // If the string is all spaces, return an empty string
    if (start == string::npos) return "";

    // Find the last non-space character
    size_t end = str.find_last_not_of(" \t\n\r");

    // Return the trimmed substring
    return str.substr(start, end - start + 1);
}

char* getSetting(char* name, char* path)
{
    char* output="not found";

    ifstream file(path); // Open the file
    if (!file.is_open()) {
        printf("Can't open  settings file\n");
        return "not found";
    }

    string line;
    string name2;
    while (getline(file, line)) { 
        name2=delete_crlf(splitintwo(line, ':', true).c_str()); // Get setting name
        if(remove_space(name2) == (string)name)
        {
            name2=splitintwo(line, ':', false);
            name2=trim(name2);
            output=delete_crlf(name2.c_str()); // Get setting value
        }
    }

    file.close(); // Close the file
    return output;
}

void writeSetting(char* name, char* value, char* path)
{
	FILE* fptr;

	fptr = fopen(path,"w");

	if(fptr == NULL)
	{
		printf("[x] Error writting SettingsFile: doesn't exist\n");   
		exit(1);             
	}

	fprintf(fptr,"name2: custom_value");
	fclose(fptr);
}

string get_hash_from_line(const string& line) {
    auto pos = line.find(":");
    if (pos != string::npos) {
        return line.substr(0, pos);  // Return only the hash part
    }
    return "";  // Return empty string if format is invalid
}

string search_hex_hash(const string& file_path, const string& target_hash) {
    ifstream file(file_path);
    if (!file.is_open()) {
        throw runtime_error("Failed to open file: " + file_path);
    }

    file.seekg(0, ios::end);
    streampos file_size = file.tellg();
    streampos start = 0;
    streampos end = file_size;

    string line;
    while (start <= end) {
        streampos mid = (start + end) / 2;
        file.seekg(mid);

        // Move to the beginning of the current line
        if (mid > 0) {
            file.ignore(numeric_limits<streamsize>::max(), '\n');
        }

        if (!getline(file, line)) {
            break;  // End of file reached
        }

        string hash = get_hash_from_line(line);

        if (hash == target_hash) {
            return line.substr(hash.length() + 2);  // Return the text part after ": "
        } else if (hash < target_hash) {
            start = file.tellg();  // Move to the right
        } else {
            end = mid - 1;  // Move to the left
        }

        // Handle special case where the file has only one line
        if (start == 0 && end == 0) {
            file.seekg(0);
            if (getline(file, line)) {
                hash = get_hash_from_line(line);
                if (hash == target_hash) {
                    return line.substr(hash.length() + 2);
                }
            }
            break;
        }
    }

    return "";  // Hash not found
}
