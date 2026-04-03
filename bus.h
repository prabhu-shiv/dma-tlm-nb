#ifndef BUS_H
#define BUS_H

#include <systemc>
#include <tlm>
#include <tlm_utils/simple_target_socket.h>
#include <tlm_utils/simple_initiator_socket.h>
#include "monitor.h"

using namespace sc_core;
using namespace tlm;
using namespace sc_dt;

struct Bus : sc_module, tlm_bw_transport_if<>
{
    tlm_utils::simple_target_socket<Bus> target_socket;
    tlm_utils::simple_initiator_socket<Bus> initiator_socket_mem;
    tlm_utils::simple_initiator_socket<Bus> initiator_socket_periph;

    Monitor* monitor;

    SC_CTOR(Bus)
        : target_socket("target_socket"),
          initiator_socket_mem("initiator_socket_mem"),
          initiator_socket_periph("initiator_socket_periph"),
          monitor(nullptr)
    {
        target_socket.register_nb_transport_fw(this, &Bus::nb_transport_fw);

        initiator_socket_mem.register_nb_transport_bw(this, &Bus::nb_transport_bw);
        initiator_socket_periph.register_nb_transport_bw(this, &Bus::nb_transport_bw);
    }

    virtual tlm_sync_enum nb_transport_fw(
        tlm_generic_payload& trans,
        tlm_phase& phase,
        sc_time& delay)
    {
        if (monitor) monitor->log_fw(trans, phase);

        uint64 addr = trans.get_address();

        if (addr < 0x80)
        {
            std::cout << "[BUS] Routing to MEMORY\n";
            return initiator_socket_mem->nb_transport_fw(trans, phase, delay);
        }
        else
        {
            std::cout << "[BUS] Routing to PERIPHERAL\n";
            return initiator_socket_periph->nb_transport_fw(trans, phase, delay);
        }
    }

    virtual tlm_sync_enum nb_transport_bw(
        tlm_generic_payload& trans,
        tlm_phase& phase,
        sc_time& delay)
    {
        if (monitor) monitor->log_bw(trans, phase);

        std::cout << "[BUS] Backward path forwarding\n";

        return target_socket->nb_transport_bw(trans, phase, delay);
    }

    virtual void invalidate_direct_mem_ptr(sc_dt::uint64, sc_dt::uint64) {}
};

#endif