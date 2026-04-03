#ifndef PERIPHERAL_H
#define PERIPHERAL_H

#include <systemc>
#include <tlm>
#include <tlm_utils/simple_target_socket.h>

using namespace sc_core;
using namespace tlm;

struct Peripheral : sc_module
{
    tlm_utils::simple_target_socket<Peripheral> socket;

    sc_event response_event;
    tlm_generic_payload* pending_trans;

    SC_CTOR(Peripheral) : socket("socket"), pending_trans(nullptr)
    {
        socket.register_nb_transport_fw(this, &Peripheral::nb_transport_fw);
        SC_THREAD(send_response);
    }

    virtual tlm_sync_enum nb_transport_fw(
        tlm_generic_payload& trans,
        tlm_phase& phase,
        sc_time& delay)
    {
        if (phase == BEGIN_REQ)
        {
            int* data = reinterpret_cast<int*>(trans.get_data_ptr());

            if (trans.get_command() == TLM_WRITE_COMMAND)
            {
                std::cout << "[PERIPHERAL] Received data = " << *data << "\n";
            }

            pending_trans = &trans;

            response_event.notify(5, SC_NS);

            phase = END_REQ;
            return TLM_UPDATED;
        }

        return TLM_ACCEPTED;
    }

    void send_response()
    {
        while (true)
        {
            wait(response_event);

            tlm_phase phase = BEGIN_RESP;
            sc_time delay = SC_ZERO_TIME;

            std::cout << "[PERIPHERAL] Sending BEGIN_RESP\n";

            socket->nb_transport_bw(*pending_trans, phase, delay);
        }
    }
};

#endif