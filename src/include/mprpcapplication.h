#pragma once
#include "mprpcconfig.h"
// 
class mprpcApplication
{
public:
    static void Init(int argc , char** argv);
    static  mprpcApplication& GetInstance();
    static MprpcConfig&  GetConfig();

private:
    static MprpcConfig      fileconfig_;

private:
    mprpcApplication(){};
    mprpcApplication(const mprpcApplication& ) = delete;
    mprpcApplication(mprpcApplication&&) = delete;
};