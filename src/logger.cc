#include "logger.h"

Logger& Logger:: GetIntence()
{
    static Logger log;
    return log;
}


void Logger::Setloglevel(LogLevel level)
{
    level_ = level;
}

void Logger::Writelog(const std::string& msg)
{
    lqueue_.push(msg);

}


Logger::Logger()
{
    std::thread  writeLogTask([&](){
        for(;;)
        {
            time_t now = time(nullptr);
            tm* nowtm = localtime(&now);

            char file_name[128];
            sprintf(file_name,"%d-%d-%d-log.txt",nowtm->tm_year,nowtm->tm_mon ,nowtm->tm_mday);
            FILE * fp = fopen(file_name, "a+");
            if(fp == nullptr)
            {
                std::cout << "logger file:" << file_name << "open failed \n";
                exit(EXIT_FAILURE);
            }
            
            std::string msg = lqueue_.pop();

            char buffer[1024];
            sprintf(buffer, "%d-%d-%d =>[%s]", 
                    nowtm->tm_hour, nowtm->tm_min , nowtm->tm_sec , (level_ == INFO ? "info": " error"));
            msg.insert(0,buffer);
            msg.append("\n");

            fputs(msg.c_str(), fp);
            fclose(fp);
        }
    });
    writeLogTask.detach();
}

Logger::~Logger(){}