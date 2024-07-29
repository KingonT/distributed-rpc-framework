#include "mprpcconfig.h"
#include <cstdio>
#include <string>
#include <iostream>



void substring(std::string& dst_string)
{
        int index = dst_string.find_first_not_of(' ', 0);
        if(index != -1)
        {
            dst_string = dst_string.substr(index, dst_string.size()-index);
        }
        index = dst_string.find_last_not_of(' ', dst_string.size()-1);
        if(index != -1)
        {
            dst_string = dst_string.substr(0, index+1);
        }
}


void MprpcConfig::LoadConfigFile(const char* configfile)
{
    FILE * fp = fopen(configfile,"r");
    
    while(feof(fp) == 0)
    {
        char str[1024] = {0};
        fgets(str,sizeof(str),fp);
        std::string  dst_string(str);
        substring(dst_string);
        if(dst_string[0] == '#' || dst_string[0] == '\n')   continue;

        int index = dst_string.find('=',0);
        if(index == -1)             continue;

        std::string  key  = dst_string.substr(0 , index);
        substring(key);

        std::string  value = dst_string.substr(index+1 , dst_string.size()-index-1);
        size_t pos =  value.find('\n');
        if(pos != -1)  {
            value.erase(pos,1);
        }      
        substring(value);

        configMap_.insert({key,value});
    }

}

std::string  MprpcConfig::FindConfig(const std::string& str_key)
{   
    auto it =  configMap_.find(str_key);
    if(it != configMap_.end())  return it->second;
    else return "";
}