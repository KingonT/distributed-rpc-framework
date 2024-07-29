#include "mprpcprovider.h"
#include "rpcheader.pb.h"
#include "zookeeperutil.h"
#include <cstring>

void RpcProvider::NotifyService(google::protobuf::Service* service)
{
    ServiceInfo    serinfo;

    // 获取服务对象描述信息
    const google::protobuf::ServiceDescriptor* pserviceDesc = service->GetDescriptor();

    // 获取服务的名字
    std::string sername = pserviceDesc->name();
    // 获取服务对象service的方法数量
    int methodCnt = pserviceDesc->method_count();
    
    // std::cout<<"service_name:"<<sername<< "\n";
    // std::cout<<"methcount:"<<methodCnt<< "\n";

    for(int i=0; i< methodCnt ; i++)
    {
        // 获取服务对象指定下标的服务方法的描述(抽象描述)
        const google::protobuf::MethodDescriptor* pmethodDesc = pserviceDesc->method(i);
        std::string method_name = pmethodDesc->name();
        serinfo.m_methodmap.insert({method_name, pmethodDesc});
        std::cout<<"method_name:"<<method_name<< "\n";
    }
    serinfo.m_service = service;
    m_servicemap.insert({sername,serinfo});
}

void RpcProvider::Run()
{
    std::string ip = mprpcApplication::GetConfig().FindConfig("rpcserviceip");
    uint16_t port = atoi(mprpcApplication::GetConfig().FindConfig("rpcserviceport").c_str());
    muduo::net::InetAddress  address(ip,port);
    // 创建Tcpserver 对象
    muduo::net::TcpServer  server(&m_eventloop, address , "RpcServiceip");
    //绑定连接回调和消息读写回调方法   分离网络代码和业务代码
    server.setConnectionCallback(std::bind(&RpcProvider::Onconnect,this, std::placeholders::_1));
    server.setMessageCallback(std::bind(&RpcProvider::OnMessage,this,std::placeholders::_1,
                                    std::placeholders::_2,std::placeholders::_3));

    // 设置muduo库的线程
    server.setThreadNum(4);

    ZkClient  zkcli;
    zkcli.Start();
    for(auto & sp:m_servicemap)
    {
        std::string server_path = "/" + sp.first;
        zkcli.Create(server_path.c_str(), nullptr, 0);
        for(auto & mp : sp.second.m_methodmap)
        {
            std::string method_path = server_path + "/"+ mp.first;
            char method_path_data[128] = {0};
            snprintf(method_path_data,sizeof(method_path_data) , "%s:%d",ip.c_str(),port);
            zkcli.Create(method_path.c_str(),method_path_data,strlen(method_path_data),ZOO_EPHEMERAL);                    
        }
    
    }


    server.start();
    m_eventloop.loop();
}

void  RpcProvider::Onconnect(const muduo::net::TcpConnectionPtr& pcon)
{
    if(!pcon->connected())
    {
        pcon->shutdown();
    }
}
/*

header_size(4字节) + header_str + args_str

*/
void  RpcProvider::OnMessage(const muduo::net::TcpConnectionPtr& pconnect,
                            muduo::net::Buffer* buffer,
                            muduo::Timestamp timesp)
{ 
    std::string revbuffer = buffer->retrieveAllAsString();

    // 读取字符流前四个字节的内容
    uint32_t   headersize = 0;
    revbuffer.copy((char*)&headersize, 4 , 0);

    // 根据header_size读取数据头的原始字符流
    std::string rpc_header_str = revbuffer.substr(4 , headersize);
    mprpc::RpcHeader    rpcheader;
    std::string ser_name , method_name , args;
    uint32_t     args_size;
    if(rpcheader.ParseFromString(rpc_header_str))
    {
        ser_name = rpcheader.service_name();
        method_name = rpcheader.method_name();
        args_size = rpcheader.argsize(); 
    }else
    {
        // 反序列化失败
        std::cout  <<"rpc_header_str:" << rpc_header_str << "\n";
        return ;
    }
     // 获取rpc方法参数的字符流数据
    args = revbuffer.substr(4 + rpc_header_str.size(), args_size);

    // 打印调试信息
    std::cout << "-------------------\n";
    std::cout << "sername:"<<ser_name<< "methodname:"<<method_name<< "args:" << args << "\n";
    std::cout << "-------------------\n";

    std::unordered_map<std::string,ServiceInfo>::iterator it1 = m_servicemap.find(ser_name);
    if(it1 == m_servicemap.end())
    {
        // no find
        std::cout << "ser_name:"<< ser_name<< "not fund!\n";
        return ;
    }
    auto it2 =  it1->second.m_methodmap.find(method_name);
    if(it2 == it1->second.m_methodmap.end())
    {
        // no find
        std::cout << "method_name:"<< method_name << "not fund!\n";
        return ;
    }
    google::protobuf::Service * pser = it1->second.m_service;
    const google::protobuf::MethodDescriptor* pmethod =  it2->second;

    //  
    google::protobuf::Message * request = pser->GetRequestPrototype(pmethod).New();
    if(!request->ParseFromString(args))
    {
        std::cout << "args parse failed!"<< args<<"\n";
        return ;
    }
    google::protobuf::Message * response = pser->GetResponsePrototype(pmethod).New();
    if(!response)   
    {
        std::cout << "message new response failed!\n";
    }

    // 给下面的method方法调用 ， 绑定一个Closure 回调函数 
    google::protobuf::Closure* clobackfun = google::protobuf::NewCallback<RpcProvider,const muduo::net::TcpConnectionPtr& ,
                                                                         google::protobuf::Message* >(this , &RpcProvider::SendMessage , pconnect, response);
    // 在框架上根据远端rpc请求 ， 调用当前rpc节点上的方法
    // new UserServic().login(controller, requset, response, done)
    pser->CallMethod(pmethod,nullptr,request,response, clobackfun);
}                         


void RpcProvider::SendMessage(const muduo::net::TcpConnectionPtr&  tcp_ptr,google::protobuf::Message* reponse)
{
    std::string    reponse_str;
    if(reponse->SerializePartialToString(&reponse_str))
    {
        // 序列化成功后 通过网路把rpc方法执行结果发送回rpc调用方
        tcp_ptr->send(reponse_str);
        tcp_ptr->shutdown();
    }
    else{
        std::cout << "serialize failed!\n";
    }
    //模拟tcp 短链接 ， 主动断开
    tcp_ptr->shutdown();
}