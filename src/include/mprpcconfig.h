#pragma once
#include <unordered_map>
#include <string>


// 框架读取配置文件类
class MprpcConfig
{
private:
    std::unordered_map<std::string,std::string>   configMap_;


public:
    MprpcConfig(/* args */){}
    ~MprpcConfig(){}

    void LoadConfigFile(const char* config_file);
    std::string  FindConfig(const std::string& str_key);
};


