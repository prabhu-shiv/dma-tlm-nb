#ifndef MONITOR_H
#define MONITOR_H

#include <systemc>
#include <tlm>

using namespace sc_core;
using namespace tlm;
using namespace sc_dt;

struct Monitor : sc_module
{
    SC_CTOR(Monitor) {}

    void log_fw(tlm_generic_payload& trans, tlm_phase phase)
    {
        std::cout << "[MONITOR][FW] " << sc_time_stamp() << " | ";

        if (phase == BEGIN_REQ) std::cout << "BEGIN_REQ ";
        else if (phase == END_REQ) std::cout << "END_REQ ";

        print_trans(trans);
    }

    void log_bw(tlm_generic_payload& trans, tlm_phase phase)
    {
        std::cout << "[MONITOR][BW] " << sc_time_stamp() << " | ";

        if (phase == BEGIN_RESP) std::cout << "BEGIN_RESP ";
        else if (phase == END_RESP) std::cout << "END_RESP ";

        print_trans(trans);
    }

    void print_trans(tlm_generic_payload& trans)
    {
        uint64 addr = trans.get_address();
        int* data = reinterpret_cast<int*>(trans.get_data_ptr());

        std::string cmd =
            (trans.get_command() == TLM_READ_COMMAND) ? "READ" : "WRITE";

        std::cout << cmd
                  << " addr=" << addr
                  << " data=" << *data
                  << std::endl;
    }
};

#endif