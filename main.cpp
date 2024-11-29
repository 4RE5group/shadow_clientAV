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

string calculate_md5(const string& file_path) {
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

bool analyseFile(string file_path) {
    if(std::filesystem::exists(file_path)) {
        // string file_path = "malwaredb/mal1";
        string file_hash = calculate_md5(file_path);
        cout << "MD5 Hash: '" << file_hash << "'" << endl;

        string db_path = "";
        string output = "";
        for(const auto file: filesystem::directory_iterator("./database")){
            cout << "Searching in " << file.path() << "...";
            db_path = file.path(); //"virus.lst";  // Database file
            output = search_hex_hash(db_path, file_hash);

            if(output != "") {
                return true;
            }
            cout << COLOR_RED << " NOT FOUND" << COLOR_RESET << endl;
        }
        return false;
        
    } else {
        cout << COLOR_RED << "File does not exist" << COLOR_RESET << endl;
        return false;
    }
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
    cout << search_hex_hash("./database/4re5.lst", "d988060862d01ba2e6a5a618ebb644d1");



    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " [-a filename | -e filename | --startup | --shutdown]" << endl;
        return 1;
    }

    string option = argv[1];

    if (option == "-a" && argc == 3) {
        string filename = argv[2];
        cout << "Analysing file: " << filename << endl;
        if(analyseFile(filename)) {
            cout << COLOR_RED << "=> " << filename << " is a virus /!\\" << COLOR_RESET << endl;
        } else {
            cout << COLOR_GREEN << "=> " << filename << " is safe" << COLOR_RESET << endl;
        }
    } else if (option == "-e" && argc == 3) {
        string filename = argv[2];
        cout << "e: " << filename << endl;
    } else if (option == "--startup") {
        cout << "Starting as a background daemon..." << endl;
        start_daemon();
    } else if (option == "--shutdown") {
        cout << "Shutting down the background daemon..." << endl;
        shutdown_daemon();
    } else {
        cerr << COLOR_RED << "Invalid arguments. Usage: " << argv[0] << " [-a filename | -e filename | --startup | --shutdown]" << COLOR_RESET << endl;
        return 1;
    }

    return 0;
}
