#include <zmq.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <boost/json.hpp>
#include <iostream>
#include "MsgClass.h"

using namespace boost::json;

int main (void)
{
    printf ("Connecting to hello world server…\n");
    void *context = zmq_ctx_new ();
    void *requester = zmq_socket (context, ZMQ_REQ);
    zmq_connect (requester, "tcp://localhost:5555");

    int request_nbr;
    for (request_nbr = 0; request_nbr != 10; request_nbr++) {
        char buffer [10];
        printf ("Sending Hello %d…\n", request_nbr);

        NSJsonClass::MsgSetReg tsmsg(12345,67890);
        auto jv = boost::json::value_from(tsmsg);
        std::cout << "jv type:[" << typeid(jv).name() << "],value:" << jv << std::endl;

        zmq_send (requester, jv.get_string().c_str(), 5, 0);

        zmq_recv (requester, buffer, 10, 0);
        printf ("Received World %d\n", request_nbr);
    }
    zmq_close (requester);
    zmq_ctx_destroy (context);
    return 0;
}