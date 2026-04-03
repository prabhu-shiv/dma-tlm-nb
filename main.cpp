#include <systemc>
#include "initiator.h"
#include "target.h"
#include "bus.h"
#include "peripheral.h"

using namespace sc_core;

int sc_main(int argc, char* argv[])
{
    Initiator dma("dma");
    Target memory("memory");
    Peripheral periph("peripheral");
    Bus bus("bus");

    // Bind DMA → Bus
    dma.socket.bind(bus.target_socket);

    // Bind Bus → Memory & Peripheral
    bus.initiator_socket_mem.bind(memory.socket);
    bus.initiator_socket_periph.bind(periph.socket);

    sc_start();

    return 0;
}