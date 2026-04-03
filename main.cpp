#include <systemc>
#include "initiator.h"
#include "target.h"
#include "bus.h"
#include "peripheral.h"
#include "monitor.h"

using namespace sc_core;

int sc_main(int argc, char* argv[])
{
    Initiator dma("dma");
    Target memory("memory");
    Peripheral periph("peripheral");
    Bus bus("bus");
    Monitor monitor("monitor");

    bus.monitor = &monitor;

    dma.socket.bind(bus.target_socket);
    bus.initiator_socket_mem.bind(memory.socket);
    bus.initiator_socket_periph.bind(periph.socket);

    sc_start();

    return 0;
}