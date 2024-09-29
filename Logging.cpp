#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

enum LogLevel {
    Info,
    Error
};

class Logger {
public:
    Logger(const string& filename)
    {
        logFile.open(filename, ios::app);
        if (!logFile.is_open()) {
            cerr << "Error opening log file." << endl;
        }
    }

    ~Logger() { logFile.close(); }

    void log(LogLevel level, const string& message)
    {
        time_t now = time(0);
        tm* timeinfo = localtime(&now);
        char timestamp[20];
        strftime(timestamp, sizeof(timestamp),
                 "%Y-%m-%d %H:%M:%S", timeinfo);

        ostringstream logEntry;
        logEntry << "[" << timestamp << "] "
                 << levelToString(level) << ": " << message
                 << endl;

        cout << logEntry.str();

        if (logFile.is_open()) {
            logFile << logEntry.str();
            logFile.flush();
        }
    }

private:
    ofstream logFile;
    string levelToString(LogLevel level)
    {
        switch (level) {
        case Info:
            return "INFO";
        case Error:
            return "ERROR";
        default:
            return "UNKNOWN";
        }
    }
};
