#ifndef INITIATOR_H
#define INITIATOR_H

#include <systemc>
#include <tlm>
#include <tlm_utils/simple_initiator_socket.h>

using namespace sc_core;
using namespace tlm;

struct Initiator : sc_module, tlm_bw_transport_if<>
{
    tlm_utils::simple_initiator_socket<Initiator> socket;

    SC_CTOR(Initiator) : socket("socket")
    {
        socket.register_nb_transport_bw(this, &Initiator::nb_transport_bw);
        SC_THREAD(process);
    }

    void process()
    {
        tlm_generic_payload* trans = new tlm_generic_payload;

        int data = 100;
        sc_time delay = SC_ZERO_TIME;

        trans->set_command(TLM_WRITE_COMMAND);
        trans->set_address(0x100);
        trans->set_data_ptr(reinterpret_cast<unsigned char*>(&data));
        trans->set_data_length(4);

        tlm_phase phase = BEGIN_REQ;

        std::cout << "[INITIATOR] BEGIN_REQ sent\n";

        tlm_sync_enum status = socket->nb_transport_fw(*trans, phase, delay);

        if (status == TLM_UPDATED && phase == END_REQ)
        {
            std::cout << "[INITIATOR] END_REQ received\n";
        }
    }

    virtual tlm_sync_enum nb_transport_bw(
        tlm_generic_payload& trans,
        tlm_phase& phase,
        sc_time& delay)
    {
        if (phase == BEGIN_RESP)
        {
            std::cout << "[INITIATOR] BEGIN_RESP received\n";

            phase = END_RESP;

            std::cout << "[INITIATOR] Sending END_RESP\n";

            return TLM_UPDATED;
        }
        return TLM_ACCEPTED;
    }

    virtual void invalidate_direct_mem_ptr(sc_dt::uint64, sc_dt::uint64) {}
};

#endif