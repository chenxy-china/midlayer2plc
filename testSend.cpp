#include <zmq.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <boost/json.hpp>
#include <iostream>
#include <vector>
#include "MsgClass.h"

using namespace boost::json;

int main (int argc ,char* argv[])
{
    void *context = zmq_ctx_new ();
    void *requester = zmq_socket (context, ZMQ_REQ);
    zmq_connect (requester, "tcp://localhost:5555");

    boost::json::value jv;

    if(argc > 1){
        if (strcmp(argv[1], "1") == 0) {
            /*读取bits*/
            NSJsonClass::MsgGetBits tsmsg = NSJsonClass::MsgGetBits(0,50);
            tsmsg.type = "MsgGetBits";
            //将类实例序列化成字符串
            jv = boost::json::value_from(tsmsg);
        } else if (strcmp(argv[1], "2") == 0) {
            /*读取多个寄存器*/
            NSJsonClass::MsgGetRegs tsmsg = NSJsonClass::MsgGetRegs(2,50);
            tsmsg.type = "MsgGetRegs";
            //将类实例序列化成字符串
            jv = boost::json::value_from(tsmsg);
        } else if (strcmp(argv[1], "3") == 0) {
            //设置多寄存器
            NSJsonClass::MsgSetRegs tsmsg = NSJsonClass::MsgSetRegs(2,50,15);
            tsmsg.type = "MsgSetRegs";
            //将类实例序列化成字符串
            jv = boost::json::value_from(tsmsg);
        }

    }else{
        //设置单寄存器
        NSJsonClass::MsgSetReg tsmsg = NSJsonClass::MsgSetReg(1,16);
        tsmsg.type = "MsgSetReg";
        //将类实例序列化成字符串
        jv = boost::json::value_from(tsmsg);
    }
    
    std::cout << "jv type:[" << typeid(jv).name() << "],value:" << jv << std::endl;
    std::cout << jv.kind() << std::endl;

    char buffer[1024]={0,};
    strcpy(buffer,serialize(jv).c_str());
    int z = strlen(buffer);

    std::cout <<"Sending :"<< buffer << std::endl;
    zmq_send (requester,buffer,z,0);

    memset(buffer,0,1024);
    zmq_recv (requester, buffer, 1024, 0);
    std::cout <<"Received :"<< buffer << std::endl;

    zmq_close (requester);
    zmq_ctx_destroy (context);
    return 0;
}