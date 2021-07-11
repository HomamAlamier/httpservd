#include <Log/Log.h>

#ifdef LOG_FILE
    #include <fstream>
    std::fstream _logFile("log.txt", std::ios::out);
#endif // LOG_FILE

std::string nowToString()
{
    time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::tm * ptm = std::localtime(&now);
    char buffer[32];
    // Format: Mo, 15.06.2009 20:20:00
    std::strftime(buffer, 32, "%Y-%m-%d %H:%M:%S", ptm);  
    return std::string(buffer);
}
#ifdef LOG_FILE
    void logToFile(const std::string& text)
    {
        _logFile.write(line.c_str(), line.size());
        _logFile.flush();
    }
#endif // LOG_FILE