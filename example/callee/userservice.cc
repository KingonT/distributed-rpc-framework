#include  <iostream>
#include  <string>
#include  "user.pb.h"  
#include  "mprpcapplication.h"
#include  "mprpcprovider.h"
#include  "logger.h"

class  UserServiceDerived :public fixbug::UserServices
{
public:
    bool  Login(std::string name , std::string pwd)
    {
        std::cout<<"name:"<< name << std::endl;
        std::cout<<"pwd:"<< pwd  << std::endl;
        return true;
    }

    bool  Register(uint32_t id , std::string name, std::string pwd)
    {
        return true;
    }

    virtual void Login(google::protobuf::RpcController* controller,
                       const ::fixbug::LoginRequest* request,
                       ::fixbug::LoginResponse* response,
                       ::google::protobuf::Closure* done)
    {
        std::string name = request->name();
        std::string pwd = request->pwd();

        bool login_result =  Login(name,pwd);

        fixbug::ResaultCode* code = response->mutable_result();
        code->set_errcode(0);
        code->set_errmsg("");
        response->set_success(login_result); 
        done->Run();
    }


    virtual void Register(google::protobuf::RpcController* controller,
                       const ::fixbug::RegisterRequest* request,
                       ::fixbug::RegisterResponse* response,
                       ::google::protobuf::Closure* done)
    {
        bool ret_bool = Register(request->id(),request->name(),request->pwd());
        response->mutable_result()->set_errcode(0);
        response->mutable_result()->set_errmsg("");
        response->set_success(ret_bool);
        done->Run();
    }

};


int main(int argc ,char* argv[])
{
    LOG_INFO("start now");
    mprpcApplication::Init(argc,argv);

    // provdier 是一个rpc网络服务对象。把UserServiceDerived对象发布到rpc节点上
    RpcProvider     provider;
    provider.NotifyService(new UserServiceDerived());
    
    // 启动一个rpc服务发布节点   Run以后进入阻塞状态，远程等待rpc请求
    provider.Run();

    return 0;
}