#include <boost/asio/io_service.hpp>
#include <boost/thread/thread.hpp>
#include <boost/bind/bind.hpp> 
#include <zmq.h>
#include <modbus.h>
#include <iostream>
#include "MsgClass.h"

boost::mutex global_stream_lock;
int _closeflag = 1;

//zeroMQ global data
void *context;
void *responder;
//modbus global data
modbus_t *mb;

static void sig_handler(int signo)
{
    printf("sig_handler\n");
    if (signo == SIGINT) {
        _closeflag = 0;
    }
    return;
}

void WorkerThread( boost::shared_ptr< boost::asio::io_service > io_service )
{
	global_stream_lock.lock();
	std::cout << "[" << boost::this_thread::get_id() << "] Thread Start" << std::endl;
	global_stream_lock.unlock();

	io_service->run();

	global_stream_lock.lock();
	std::cout << "[" << boost::this_thread::get_id() <<	"] Thread Finish" << std::endl;
	global_stream_lock.unlock();
}

void serverMQ(boost::shared_ptr< boost::asio::io_service > io_service)
{
    char buffer [1024];
    int ret = zmq_recv (responder, buffer, 256, 0);
    //接收消息
    if(ret == -1)
    {
        int error = zmq_errno();
        std::cout << zmq_strerror(error) << std::endl;
        if(_closeflag == 0)
        {
            std::cout << "messageReceiver thread closed......2" << std::endl;
            return;
        }
    } else {
        std::cout <<"Received :"<< buffer << std::endl;
        
        //将字符串转换成boost::json::value
        auto jv = boost::json::parse(buffer);
        std::cout << "jv type:[" << typeid(jv).name() << "],value:" << jv << std::endl;
        std::cout << jv.kind() << std::endl;

        int rc = 0;

        //对收到的json字符串数据做解析
        try{
            auto msgObj = boost::json::value_to<NSJsonClass::MsgBase>(jv);
            std::cout<< "msg type" << msgObj.type << std::endl;

            if(msgObj.type == "MsgGetBits"){
                auto msgObj = boost::json::value_to<NSJsonClass::MsgGetBits>(jv);
                std::cout << "Address ="<< msgObj.address<<", size = "<< msgObj.size << std::endl;

                string responmsg = "ok";
                if(msgObj.size > MODBUS_MAX_READ_BITS){
                    responmsg = "size over max red bits";
                }else{
                    //通过modbus读取线圈
                    std::unique_ptr<uint8_t[]> tab_bit = std::make_unique<uint8_t[]>(msgObj.size);
                    memset(tab_bit.get(), 0, msgObj.size * sizeof(uint8_t));
                    rc = modbus_read_bits(mb, msgObj.address, msgObj.size, tab_bit.get());
                    if (rc == -1)
                    {
                        std::cout << modbus_strerror(errno) << std::endl;
                    }
                    boost::json::object obresp;
                    boost::json::array arrbitv;
                    obresp["address"] = msgObj.address;
                    obresp["size"] = msgObj.size;
                    for(int i = 0;i < msgObj.size; i++){
                        arrbitv.push_back(tab_bit[i]);
                    }
                    obresp["resp"] = arrbitv;
                    
                    responmsg = boost::json::serialize(obresp);
                    std::cout << "response msg:" << responmsg << std::endl;
                    std::cout << "size =" << responmsg.size() << std::endl;
                }
                zmq_send (responder, responmsg.c_str(), responmsg.size(), 0);
            }else if(msgObj.type == "MsgSetReg"){
                auto msgObj = boost::json::value_to<NSJsonClass::MsgSetReg>(jv);
                std::cout << "Address ="<< msgObj.address<<", value = 0x"<< std::hex << msgObj.value << std::endl;
                //通过modbus设置寄存器的值
                rc = modbus_write_register(mb, msgObj.address, msgObj.value);
                if (rc == -1)
                {
                    std::cout << modbus_strerror(errno) << std::endl;
                }
                zmq_send (responder, "ok", 2, 0);
            }
            
        }catch(std::exception e)
        {
            std::cout << "value_to failed: " << e.what() << "\n";
            zmq_send (responder, "ng", 2, 0);
        }
    }

    io_service->post( boost::bind( &serverMQ, io_service) );
}

int initMQ()
{
    //  Socket to talk to clients
    context = zmq_ctx_new ();
    responder = zmq_socket (context, ZMQ_REP);

    int recvTime = 5000;
    zmq_setsockopt(responder, ZMQ_RCVTIMEO, &recvTime, sizeof(recvTime));

    int rc = zmq_bind (responder, "tcp://*:5555");
    assert (rc == 0);

    return 0;
}

int initModbus()
{
    uint16_t tab_reg[32];
    int i;
    int rc;

    //打开端口
    mb = modbus_new_tcp("192.168.181.1", 502);
    modbus_set_slave(mb, 1);

    //建立链接
    if(modbus_connect(mb)==-1) {
        std::cout << "Modbus Connection failed:" << modbus_strerror(errno) << std::endl;
        modbus_free(mb);
        return -1;
    }
    return 0;
}

int main(int argc , char** argv)
{
    fflush(stdout);  
    setvbuf(stdout,NULL,_IONBF,0); 
    setvbuf(stderr,NULL,_IONBF,0);

    //CTRL+C 停止进程进入sig_handler函数运行
    //必须放在类Agv构造之后，否则不起作用
    if (signal(SIGINT, sig_handler) == SIG_ERR) {
        printf("signal sigint error: %s\n", strerror (errno));
        return -1;
    }

    if(initModbus() != 0)
    {
        return -1;
    }
    if(initMQ() != 0)
    {
        modbus_close(mb);
        modbus_free(mb);
        return -1;
    }

    boost::shared_ptr< boost::asio::io_service > io_service(new boost::asio::io_service);
	boost::shared_ptr< boost::asio::io_service::work > work(new boost::asio::io_service::work( *io_service ));

	boost::thread_group worker_threads;
	for( int x = 0; x < 4; ++x )
	{
		worker_threads.create_thread( boost::bind( &WorkerThread, io_service ) );
	}

	io_service->post( boost::bind( &serverMQ, io_service) );

    while(_closeflag){
        sleep(100);
    };

    work.reset();
	io_service->stop();
	worker_threads.join_all();

    zmq_close (responder);
    zmq_ctx_destroy (context);

    modbus_close(mb);
    modbus_free(mb);

    printf("success quit~\n");
    return 0;
}