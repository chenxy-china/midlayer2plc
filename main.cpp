#include <boost/asio/io_service.hpp>
#include <boost/thread/thread.hpp>
#include <boost/bind/bind.hpp> 
#include <zmq.h>
#include <modbus.h>
#include <iostream>
#include "MsgClass.h"

boost::mutex global_stream_lock;

int _closeflag = 1;
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

void serverMQ(boost::shared_ptr< boost::asio::io_service > io_service, void *context, void * responder)
{
    char buffer [1024];
    printf ("Receive wait tid %d \n",gettid());
    int ret = zmq_recv (responder, buffer, 256, 0);
    //接收消息
    if(ret == -1)
    {
        int error = zmq_errno();
        std::cout << "ERROR: messageReceiver socket: wrong message --" << zmq_strerror(error) << std::endl;
        if(_closeflag == 0)
        {
            zmq_close (responder);
            zmq_ctx_destroy (context);
            std::cout << "messageReceiver thread closed......2" << std::endl;
            return;
        }
    } else {
        printf ("Received\n");
        
        //将字符串转换成boost::json::value
        boost::json::value jsonObj = boost::json::value(buffer);
        //对收到的json字符串数据做解析
        auto msgObj = boost::json::value_to<NSJsonClass::MsgBase>(jsonObj);

        std::cout<< "msg type" << msgObj.type << std::endl;

        if(msgObj.type == "MsgSetReg"){
            auto msgSetRegObj = boost::json::value_to<NSJsonClass::MsgSetReg>(jsonObj);

            std::cout<< "set reg ["<< msgSetRegObj.address << "] = "
                << msgSetRegObj.value << "(0x"<< std::hex <<  msgSetRegObj.value << ")" << std::endl;
            zmq_send (responder, "ok", 2, 0);
        }
    }

	io_service->post( boost::bind( &serverMQ, io_service,context, responder ) );
}

int initMQ(boost::shared_ptr< boost::asio::io_service > io_service)
{
    //  Socket to talk to clients
    void *context = zmq_ctx_new ();
    void *responder = zmq_socket (context, ZMQ_REP);

    int recvTime = 500;
    zmq_setsockopt(responder, ZMQ_RCVTIMEO, &recvTime, sizeof(recvTime));

    int rc = zmq_bind (responder, "tcp://*:5555");
    assert (rc == 0);

	io_service->post( boost::bind( &serverMQ, io_service ,context, responder) );

    return 0;
}

int initModbus()
{

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

    boost::shared_ptr< boost::asio::io_service > io_service(new boost::asio::io_service);
	boost::shared_ptr< boost::asio::io_service::work > work(new boost::asio::io_service::work( *io_service ));

	boost::thread_group worker_threads;
	for( int x = 0; x < 4; ++x )
	{
		worker_threads.create_thread( boost::bind( &WorkerThread, io_service ) );
	}

    initMQ(io_service);

    while(_closeflag){
        sleep(100);
    };

    work.reset();
	io_service->stop();
	worker_threads.join_all();

    printf("success quit~\n");
}