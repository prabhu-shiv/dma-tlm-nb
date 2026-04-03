#ifndef TARGET_H
#define TARGET_H

#include <systemc>
#include <tlm>
#include <tlm_utils/simple_target_socket.h>

using namespace sc_core;
using namespace tlm;

struct Target : sc_module
{
    tlm_utils::simple_target_socket<Target> socket;

    SC_CTOR(Target) : socket("socket")
    {
        socket.register_nb_transport_fw(this, &Target::nb_transport_fw);
    }

    virtual tlm_sync_enum nb_transport_fw(
        tlm_generic_payload& trans,
        tlm_phase& phase,
        sc_time& delay)
    {
        if (phase == BEGIN_REQ)
        {
            std::cout << "[TARGET] Received BEGIN_REQ\n";

            int* data = reinterpret_cast<int*>(trans.get_data_ptr());
            std::cout << "[TARGET] Data = " << *data << "\n";

            phase = BEGIN_RESP;

            return socket->nb_transport_bw(trans, phase, delay);
        }
        return TLM_ACCEPTED;
    }
};

#endif