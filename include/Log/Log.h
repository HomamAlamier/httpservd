#ifndef LOG_H
#define LOG_H

#include <string>
#include <iostream>


#ifdef LOG_DEBUG
    const bool debug_log = true;
#else
    const bool debug_log = false;
#endif // LOG_DEBUG

#include <chrono>
std::string nowToString();

#ifdef LOG_FILE
    void logToFile(const std::string& text);
#endif // DEBUG


enum LogType
{
    Info, Debug, Warning, Error, Fetal
};

template <class T> 
void log_WriteLine(LogType type, const std::string& text)
{
    if (type == Debug && !debug_log)
        return;

    std::string line = "[" + nowToString() + "] (" + typeid(T).name() + ") "; 
    switch (type)
    {
    case Info: line += "Info: "; break;
    case Debug: line += "Debug: "; break;
    case Warning: line += "Warning: "; break;
    case Error: line += "Error: "; break;
    case Fetal: line += "Fetal: "; break;
    }

    line += text;
#ifdef LOG_COUT
    std::cout << line << std::endl;
#endif
#ifdef LOG_FILE
    logToFile(line);
#endif // LOG_COUT

}
#endif // !LOG_H