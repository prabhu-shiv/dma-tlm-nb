#include <systemc>
#include "initiator.h"
#include "target.h"

using namespace sc_core;

int sc_main(int argc, char* argv[])
{
    Initiator initiator("initiator");
    Target target("target");

    initiator.socket.bind(target.socket);

    sc_start();

    return 0;
}