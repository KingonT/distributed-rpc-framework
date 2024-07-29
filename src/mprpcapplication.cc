#include  "mprpcapplication.h"
#include  <stdlib.h>
#include  <unistd.h>
#include  <iostream>

MprpcConfig  mprpcApplication::fileconfig_;


void ShowArgsHelp()
{
        std::cout<<"Uage :\n ./bin  [-i] [config_file] \n";
}


void mprpcApplication::Init(int argc, char** argv)
{
        if(argc < 2)
        {
                ShowArgsHelp();
                exit(EXIT_FAILURE);
        }

        std::string  confile;
        int c = 0;
        while ( (c=getopt(argc,argv,"i:"))!= -1)
        {
                switch (c)
                {
                case 'i': confile= optarg; break;
                
                case '?': ShowArgsHelp();
                          exit(EXIT_FAILURE);      
                
                case ':': ShowArgsHelp();
                          exit(EXIT_FAILURE);
                default:  ShowArgsHelp();
                          exit(EXIT_FAILURE);
                }
        }
        
        // 加载配置文件
        fileconfig_.LoadConfigFile(confile.c_str());
}


mprpcApplication&  mprpcApplication::GetInstance()
{
        static  mprpcApplication app;
        return app;
}

MprpcConfig& mprpcApplication::GetConfig()
{
        return fileconfig_;
}