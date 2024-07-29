#include <iostream>
#include <string>
#include "test.pb.h"

int main(int argc , char* argv[])
{
    fixbug::LonginResponse   res;
    fixbug::ResultCode * pres  = res.mutable_result();
    pres->set_errcode(0);
    pres->set_errmsg("登陆成功");

    fixbug::GetFriendListResponse   fres;
    fixbug::User*  u1 =  fres.add_friendlist();
    u1->set_age(0);



    fixbug::ResultCode  *   pfres = fres.mutable_result();
    pfres->set_errcode(0);
    pfres->set_errmsg("success");


    // fixbug::LoginRequest  req;
    // req.set_name("wxh");
    // req.set_pwd("123456");
    // std::string sendstr;
    // if(!req.SerializeToString(&sendstr))    return -1;

    // std::cout<<sendstr.c_str()<<std::endl;

    // fixbug::LoginRequest   reqnew;
    // reqnew.ParseFromString(sendstr);

    // std::cout<<reqnew.name() << std::endl;
    // std::cout<<reqnew.pwd() << std::endl;

    return 0;
}