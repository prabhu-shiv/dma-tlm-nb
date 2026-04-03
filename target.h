#ifndef TARGET_H
#define TARGET_H

#include <systemc>
#include <tlm>
#include <tlm_utils/simple_target_socket.h>

using namespace sc_core;
using namespace tlm;
using namespace sc_dt;

struct Target : sc_module
{
    tlm_utils::simple_target_socket<Target> socket;

    int mem[256];  // simple memory

    sc_event response_event;
    tlm_generic_payload* pending_trans;

    SC_CTOR(Target) : socket("socket"), pending_trans(nullptr)
    {
        socket.register_nb_transport_fw(this, &Target::nb_transport_fw);
        SC_THREAD(send_response);

        // Initialize memory
        for (int i = 0; i < 256; i++)
            mem[i] = i * 10;
    }

    virtual tlm_sync_enum nb_transport_fw(
        tlm_generic_payload& trans,
        tlm_phase& phase,
        sc_time& delay)
    {
        if (phase == BEGIN_REQ)
        {
            uint64 addr = trans.get_address();
            int* data = reinterpret_cast<int*>(trans.get_data_ptr());

            if (trans.get_command() == TLM_READ_COMMAND)
            {
                *data = mem[addr];
                std::cout << "[MEM] READ addr=" << addr << " data=" << *data << "\n";
            }
            else if (trans.get_command() == TLM_WRITE_COMMAND)
            {
                mem[addr] = *data;
                std::cout << "[MEM] WRITE addr=" << addr << " data=" << *data << "\n";
            }

            pending_trans = &trans;

            response_event.notify(10, SC_NS);

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

            std::cout << "[MEM] Sending BEGIN_RESP\n";

            socket->nb_transport_bw(*pending_trans, phase, delay);
        }
    }
};

#endif