#pragma once

#include "google/protobuf/service.h"
#include "google/protobuf/descriptor.h"
#include "google/protobuf/message.h"
#include "google/protobuf/stubs/callback.h"
#include "mprpcapplication.h"
#include "rpcheader.pb.h"
#include <string>
#include <iostream>   


class MprpcChannel: public google::protobuf::RpcChannel
{
private:
    // 所有通过stub 代理对象调用的rpc方法 ，都走到这里， 统一做rpc方法调用的数据序列化和网络发送
    virtual void CallMethod(const google::protobuf::MethodDescriptor* method,
                                google::protobuf::RpcController* controller, 
                                const google::protobuf::Message* request,
                                google::protobuf::Message* response, 
                                google::protobuf::Closure* done);
public:
    MprpcChannel(){}    
    ~MprpcChannel(){}
};


