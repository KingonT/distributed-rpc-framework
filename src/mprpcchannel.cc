#include "mprpcchannel.h"
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>              
#include <cerrno>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstdio>
#include "zookeeperutil.h"

void MprpcChannel::CallMethod(const google::protobuf::MethodDescriptor* method,
                                google::protobuf::RpcController* controller, 
                                const google::protobuf::Message* request,
                                google::protobuf::Message* response, 
                                google::protobuf::Closure* done)
{
    const google::protobuf::ServiceDescriptor * pser =  method->service();
    std::string method_name =  method->name();                                 
    std::string service_name = pser->name();

    /*  headersize + header + argssize + argsstr    */
    std::string argsstr;
    uint32_t    argssize = 0;
    if(!request->SerializeToString(&argsstr))   
    {
        std::cout <<"SerializetoString failed! :"<< argsstr<<"\n";
    }
    argssize = argsstr.size();
    std::string sendstring;
    
    mprpc::RpcHeader   rpcheader;
    rpcheader.set_service_name(service_name);
    rpcheader.set_method_name(method_name);
    rpcheader.set_argsize(argssize);

    std::string     headerstr;
    if(!rpcheader.SerializeToString(&headerstr))                                                         
    {
        std::cout <<"serialize header failed!\n";
        std::cout << rpcheader.service_name()<<":"<< rpcheader.method_name()<<":"<< rpcheader.argsize()<< "\n";
        return ;
    }
    uint32_t headerstrsize = headerstr.size();

    sendstring.insert(0, reinterpret_cast<char*>(&headerstrsize) , sizeof(headerstrsize));
    sendstring += headerstr;
    sendstring += argsstr;
printf("%s",sendstring.c_str());
    // tcp coding..
    int fd = socket(AF_INET,SOCK_STREAM,0);
    if(-1 == fd)
    {
        std::cout<<"creator socket failed!:" << strerror(errno) << "\n";
        exit(EXIT_FAILURE);
    }    

    // 读取配置文件  
    // std::string  ip = mprpcApplication::GetConfig().FindConfig("rpcserviceip");
    // uint32_t   port = atoi(mprpcApplication::GetConfig().FindConfig("rpcserviceport").c_str());

    // 改为从配置中心zookeeper 读取服务IP和端口
    ZkClient    zkcli;
    zkcli.Start();
    std::string  path = "/" + service_name + "/" + method_name;
    std::string  ip_port =  zkcli.GetData(path.c_str());
    if(ip_port.size() == 0)
    {
        std::cout << "zkclient get path data failed!. \n";
        return ;
    }
    size_t pos = ip_port.find(":");
    if(pos == std::string::npos)
    {
        std::cout<<"\" : \" not find.\n";
        return ;
    }
    std::string  ip = ip_port.substr(0,pos);
    uint16_t  port = atoi(ip_port.substr(pos+1).c_str());


    struct sockaddr_in  addrin;
    addrin.sin_family = AF_INET;
    addrin.sin_port = htons(port);
    addrin.sin_addr.s_addr = inet_addr(ip.c_str());

    int ret = connect(fd,(struct sockaddr*)&addrin, sizeof(addrin));
    if(ret == -1)
    {
        std::cout <<"connect Failed!"<< strerror(errno) << "\n";
        exit(EXIT_FAILURE);
    }

    ssize_t size =  send(fd, sendstring.c_str(), sendstring.size(), 0);
    if(size == -1)
    {
        std::cout<< "send failed!" << strerror(errno) << "\n";
        close(fd);
        return ;
    }

    char buffer[1024];
    size = recv(fd,buffer,sizeof(buffer),0);
    if(size == -1)
    {
        std::cout<< "send failed!" << strerror(errno) << "\n";
        close(fd);
        return ;
    }

    if(!response->ParseFromArray(buffer, size ))
    {
        std::cout<< "reponse parse failed!" << "\n";
        close(fd);
        return ;       
    }

    close(fd);
} 




