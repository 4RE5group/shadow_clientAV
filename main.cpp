#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <openssl/md5.h>
#include <settings.h>
#include <string>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <filesystem>

using namespace std;

#define COLOR_RED "\e[31m"
#define COLOR_GREEN "\e[32m"
#define COLOR_RESET "\e[0m"

#define ROOT (string)"."

string calculate_md5(const string& file_path) 
{
    unsigned char hash[MD5_DIGEST_LENGTH];
    MD5_CTX md5_context;
    char buffer[4096];
    ifstream file(file_path, ios::binary);

    if (!file) {
        cerr << "Error: Unable to open file " << file_path << endl;
        return "";
    }

    MD5_Init(&md5_context);

    while (file.read(buffer, sizeof(buffer))) {
        MD5_Update(&md5_context, buffer, file.gcount());
    }
    // Add the remaining bytes if any
    if (file.gcount() > 0) {
        MD5_Update(&md5_context, buffer, file.gcount());
    }

    MD5_Final(hash, &md5_context);

    ostringstream hash_string;
    for (int i = 0; i < MD5_DIGEST_LENGTH; ++i) {
        hash_string << hex << setw(2) << setfill('0') << static_cast<int>(hash[i]);
    }

    file.close();

    return hash_string.str();
}

void start_daemon() {
    pid_t pid = fork();

    if (pid < 0) {
        cerr << "Error: Fork failed." << endl;
        exit(1);
    }
    if (pid > 0) {
        cout << "Daemon started with PID: " << pid << endl;
        exit(0);  // Parent process exits
    }

    // Child process becomes the daemon
    if (setsid() < 0) {
        cerr << "Error: Failed to create new session." << endl;
        exit(1);
    }

    // Redirect standard input/output/error to /dev/null
    freopen("/dev/null", "r", stdin);
    freopen("/dev/null", "w", stdout);
    freopen("/dev/null", "w", stderr);

    ofstream pid_file("/tmp/shadow_client");
    if (pid_file.is_open()) {
        pid_file << pid << endl;
        pid_file.close();
    } else {
        cerr << "Error: Unable to write PID file to /tmp/shadow_client" << endl;
        kill(pid, SIGTERM);  // Terminate the daemon
        exit(1);
    }


    // Simulate daemon running (example logic here)
    while (true) {
        sleep(10);  // Keep the daemon running
    }
}

void addException(string md5) {
    ofstream exceptionList(ROOT+"/exceptions.lst");
    if (!exceptionList.is_open()) {
        cerr << "Error: Could not open exception file" << endl;
        return;
    }
    exceptionList << md5;
    
    exceptionList.close();
}

bool isAnException(string md5, int* foundLine=nullptr) {
    if(filesystem::exists(ROOT+"/exceptions.lst")) {
        ifstream exceptionList(ROOT+"/exceptions.lst");
        if (!exceptionList.is_open()) {
            cerr << "Error: Could not open exception file" << endl;
            return false;
        }
        string line;
        foundLine=0;
        while(true) {
            if (getline(exceptionList, line)) {
                if(line == md5) {
                    return true;
                }
            } else {
                break; // End of file reached
            }
            foundLine++;
        }
        exceptionList.close();
        return false;
    } else {
        // create
        ofstream exceptionList(ROOT+"/exceptions.lst");
        if (!exceptionList.is_open()) {
            cerr << "Error: Could not open exception file" << endl;
            return false;
        }
        exceptionList << "";
        
        exceptionList.close();
        
        return false;
    }
}

void removeException(string md5) {
    int line_to_remove;
    if(isAnException(md5, &line_to_remove)) {
        // that can be removed
        ifstream infile(ROOT+"/exceptions.lst");
        if (!infile.is_open()) {
            cerr << "Error: Unable to open file for reading." << endl;
            return;
        }

        ofstream temp_file(ROOT+"/.tmpOP");
        if (!temp_file.is_open()) {
            cerr << "Error: Unable to open temporary file for writing." << endl;
            return;
        }

        string line;
        int current_line = 1;

        while (getline(infile, line)) {
            if (current_line != line_to_remove) {
                temp_file << line << endl;
            }
            current_line++;
        }

        infile.close();
        temp_file.close();

        // Replace the original file with the temporary file
        if (remove((ROOT+"/exceptions.lst").c_str()) != 0) {
            cerr << "Error: Unable to remove the original file." << endl;
            return;
        }
        if (rename((ROOT+"/.tmpOP").c_str(), (ROOT+"/exceptions.lst").c_str()) != 0) {
            cerr << "Error: Unable to rename the temporary file." << endl;
            return;
        }
    }

    cout << COLOR_GREEN << "[o] Successfully removed exception" << COLOR_RESET << endl;
}

string analyseFile(string file_path) {
    if(filesystem::exists(file_path)) {
        // string file_path = "malwaredb/mal1";
        string file_hash = calculate_md5(file_path);
        cout << "MD5 Hash: '" << file_hash << "'" << endl;

        string db_path = "";
        string output = "";
        for(const auto file: filesystem::directory_iterator(ROOT+"/database")){
            cout << "Searching in " << file.path() << "...";
            db_path = file.path();
            output = getSetting(file_hash.data(), db_path.data());

            if(output != "not found") {
                if(!isAnException(file_hash)) { // if not an exception
                    cout << COLOR_GREEN << " FOUND" << COLOR_RESET;
                    return output;
                } else {
                    cout << COLOR_GREEN << " EXCLUDED" << COLOR_RESET;
                }
                
            }
            cout << COLOR_RED << " NOT FOUND" << COLOR_RESET << endl;
        }
        
    } else {
        cout << COLOR_RED << "File does not exist" << COLOR_RESET << endl;
        exit(1);
    }
    return "";
}

void shutdown_daemon() {
    // Find and kill the daemon by its PID (example only)
    // This assumes the daemon writes its PID to a file.
    ifstream pid_file("/tmp/shadow_client");
    if (!pid_file.is_open()) {
        cerr << "Error: PID file not found. Is the daemon running?" << endl;
        return;
    }

    pid_t pid;
    pid_file >> pid;
    pid_file.close();

    if (kill(pid, SIGTERM) == 0) {
        cout << "Daemon with PID " << pid << " shut down successfully." << endl;
    } else {
        perror("Error shutting down daemon");
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << COLOR_RED << "Invalid arguments. Use argument -h or --help to get help" << COLOR_RESET << endl;
        return 1;
    }

    string option = argv[1];
    if(option == "-h" || option == "--help"){
        cout << "SHADOW CLIENT - 4re5 group 2024" << endl;
        cout << "commands: " << endl;
        cout << "   -h or --help       : get help" << endl;
        cout << "   -a <filename>      : analyse file" << endl;
        cout << "   -e <filename>      : exclude file from antivirus" << endl;
        cout << "   -r <filename>      : remove file from exclusionList" << endl;
        cout << "   --shutdown         : stop background AV daemon" << endl;
        cout << "   --startup          : start background AV daemon" << endl;
        cout << "   -s                 : print antivirus md5 hash databases" << endl;
        cout << "   -el                : print saved exclusions" << endl;
        
    } 
    else if(option == "-s") {
        cout << "shadow client database list:" << endl;
        for(const auto file: filesystem::directory_iterator(ROOT+"/database")) {
            cout << "   => " << file.path() << endl;
        }
    } else if(option == "-el") {
        cout << "shadow client exclusion list:" << endl;
        ifstream infile(ROOT+"/exceptions.lst");
        if (!infile.is_open()) {
            cerr << "Error: Unable to open exception file for reading." << endl;
        }
        string line;
        while(getline(infile, line)) {
            cout << "   => " << line << endl;
        }
        infile.close();
    }
    else if (option == "-a" && argc == 3) {
        string filename = argv[2];
        cout << "Analysing file: " << filename << endl;
        string output = analyseFile(filename);
        if(output != "") {
            cout << endl << COLOR_RED << "=> " << filename << " is a virus type: '" << output << "'" << COLOR_RESET << endl;
        } else {
            cout << endl << COLOR_GREEN << "=> '" << filename << "' is safe" << COLOR_RESET << endl;
        }
    } else if (option == "-e" && argc == 3) {
        string filename = argv[2];
        addException(calculate_md5(filename));
        cout << COLOR_GREEN << "[+] added exception " << filename << " successfully" << COLOR_RESET << endl;
    } else if (option == "-r" && argc == 3) {
        string filename = argv[2];
        removeException(calculate_md5(filename));
        cout << COLOR_GREEN << "[-] removed exception " << filename << " successfully" << COLOR_RESET << endl;
    } else if (option == "--startup") {
        cout << "Starting as a background daemon..." << endl;
        start_daemon();
    } else if (option == "--shutdown") {
        cout << "Shutting down the background daemon..." << endl;
        shutdown_daemon();
    } else {
        cerr << COLOR_RED << "Invalid arguments. Use argument -h or --help to get help" << COLOR_RESET << endl;
        return 1;
    }

    return 0;
}
