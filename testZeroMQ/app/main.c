#include <stdio.h>
#include <zmq.h>

int
main(int argc, char** argv)
{
    void* context = zmq_init(1);
    return 0;
}