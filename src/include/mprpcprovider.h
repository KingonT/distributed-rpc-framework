#pragma once
#include "google/protobuf/service.h"
#include  <muduo/net/TcpServer.h>
#include  <muduo/net/EventLoop.h>
#include  <muduo/net/InetAddress.h> 
#include  <muduo/net/TcpConnection.h>
#include  "mprpcapplication.h"
#include  <string>
#include  <functional>
#include  <google/protobuf/descriptor.h>
#include  <unordered_map>

// mprpc
class RpcProvider
{
public:  
    // 框架提供给外部使用的，可以发布rpc方法的函数接口
    void NotifyService(google::protobuf::Service* service);

    // 启动rpc服务节点， 开始提供rpc远程调用服务
    void Run();
private:
    muduo::net::EventLoop      m_eventloop;
    void  Onconnect(const muduo::net::TcpConnectionPtr&);
    void  OnMessage(const muduo::net::TcpConnectionPtr&,
                            muduo::net::Buffer*,
                            muduo::Timestamp);

    struct ServiceInfo
    {
        google::protobuf::Service * m_service;
        std::unordered_map<std::string , const google::protobuf::MethodDescriptor*>  m_methodmap;
    };

    std::unordered_map<std::string , ServiceInfo>   m_servicemap;

private:
    void SendMessage(const muduo::net::TcpConnectionPtr& ,google::protobuf::Message* );

};