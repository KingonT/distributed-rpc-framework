#pragma once
#include "lockqueue.h"
#include <time.h>
#include  <iostream>

#define LOG_INFO(logmsgformat, ...) \
        do{  \
            Logger& log = Logger::GetIntence();  \
            log.Setloglevel(INFO);               \
            char c[1024] = {0};                  \
            snprintf(c , 1024 , logmsgformat,##__VA_ARGS__); \
            log.Writelog(c);                       \
        }while(0)


#define LOG_ERR(logmsgformat, ...) \
        do{  \
            Logger& log = Logger::GetIntence();  \
            log.Setloglevel(ERROR);               \
            char c[1024] = {0};                  \
            snprintf(c , 1024 , logmsgformat,##__VA_ARGS__); \
            log.Writelog(c);                       \
        }while(0)

enum LogLevel
{
    INFO,
    ERROR,
};

class Logger
{
public:
    void Setloglevel(LogLevel level);
    void Writelog(const std::string&);

    static  Logger& GetIntence();
private:
    LogLevel                level_;
    LockQueue<std::string>  lqueue_;    

    Logger(); 
    ~Logger();
    Logger(const Logger&) = delete;
    Logger(Logger&&) = delete;
    Logger& operator = (const Logger&) = delete;
};



