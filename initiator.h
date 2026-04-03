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

    sc_event response_done;

    int buffer;

    SC_CTOR(Initiator) : socket("socket")
    {
        socket.register_nb_transport_bw(this, &Initiator::nb_transport_bw);
        SC_THREAD(process);
    }

    void process()
    {
        // -------- READ --------
        tlm_generic_payload trans_read;

        trans_read.set_command(TLM_READ_COMMAND);
        trans_read.set_address(10);
        trans_read.set_data_ptr(reinterpret_cast<unsigned char*>(&buffer));
        trans_read.set_data_length(4);

        tlm_phase phase = BEGIN_REQ;
        sc_time delay = SC_ZERO_TIME;

        std::cout << "[DMA] Sending READ\n";

        socket->nb_transport_fw(trans_read, phase, delay);

        wait(response_done);

        std::cout << "[DMA] Read complete, data=" << buffer << "\n";

        // -------- WRITE --------
        tlm_generic_payload trans_write;

        trans_write.set_command(TLM_WRITE_COMMAND);
        trans_write.set_address(0x90);
        trans_write.set_data_ptr(reinterpret_cast<unsigned char*>(&buffer));
        trans_write.set_data_length(4);

        phase = BEGIN_REQ;

        std::cout << "[DMA] Sending WRITE\n";

        socket->nb_transport_fw(trans_write, phase, delay);

        wait(response_done);

        std::cout << "[DMA] Write complete\n";
    }

    virtual tlm_sync_enum nb_transport_bw(
        tlm_generic_payload& trans,
        tlm_phase& phase,
        sc_time& delay)
    {
        if (phase == BEGIN_RESP)
        {
            phase = END_RESP;

            response_done.notify();

            return TLM_UPDATED;
        }

        return TLM_ACCEPTED;
    }

    virtual void invalidate_direct_mem_ptr(sc_dt::uint64, sc_dt::uint64) {}
};

#endif