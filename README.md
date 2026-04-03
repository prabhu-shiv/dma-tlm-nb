# TLM-2.0 Based DMA Controller Using Non-Blocking Transport

## Overview

This project implements a Direct Memory Access (DMA) controller using SystemC TLM-2.0 with a non-blocking transport interface. The design models asynchronous data transfer between memory and a peripheral through a bus interconnect using phase-based communication.

The implementation follows the TLM-2.0 4-phase protocol and demonstrates modular system design with clearly separated components.

---

## Features

* TLM-2.0 non-blocking transport (`nb_transport_fw`, `nb_transport_bw`)
* Full 4-phase protocol:

  * BEGIN_REQ
  * END_REQ
  * BEGIN_RESP
  * END_RESP
* DMA read and write operation
* Address-based bus routing
* Separate memory and peripheral targets
* Event-driven timing model
* Non-intrusive transaction monitor with timestamp logging

---

## System Architecture

The system consists of the following modules:

* DMA (Initiator)
* Bus (Interconnect)
* Memory (Target)
* Peripheral (Target)
* Monitor (Observer)

### Data Flow

1. DMA issues a READ request to memory
2. Memory responds with data
3. DMA stores data in a local buffer
4. DMA issues a WRITE request to peripheral
5. Peripheral receives and processes the data

---

## Address Mapping

| Address Range | Target     |
| ------------- | ---------- |
| 0x00 - 0x7F   | Memory     |
| 0x80 - 0xFF   | Peripheral |

---

## File Structure

```
.
├── main.cpp
├── initiator.h
├── target.h
├── peripheral.h
├── bus.h
├── monitor.h
└── README.md
```

---

## Build Instructions

### Prerequisites

* SystemC 3.0.2 (or compatible)
* g++ compiler

### Compilation

```
g++ -I. -I$SYSTEMC_HOME/include -L. -L$SYSTEMC_HOME/lib-linux64 -lsystemc -o sim main.cpp
```

### Run

```
./sim
```

---

## Sample Output

```
[DMA] Sending READ
[MONITOR][FW] 0 s | BEGIN_REQ READ addr=10 data=0
[BUS] Routing to MEMORY
[MEM] READ addr=10 data=100
[MEM] Sending BEGIN_RESP
[MONITOR][BW] 10 ns | BEGIN_RESP READ addr=10 data=100
[BUS] Backward path forwarding
[DMA] Read complete, data=100

[DMA] Sending WRITE
[MONITOR][FW] 10 ns | BEGIN_REQ WRITE addr=144 data=100
[BUS] Routing to PERIPHERAL
[PERIPHERAL] Received data = 100
[PERIPHERAL] Sending BEGIN_RESP
[MONITOR][BW] 15 ns | BEGIN_RESP WRITE addr=144 data=100
[BUS] Backward path forwarding
[DMA] Write complete
```

---

## Key Concepts

### Non-Blocking Transport

Transactions are split into phases instead of blocking calls, allowing concurrency and realistic timing behavior.

### 4-Phase Protocol

* BEGIN_REQ: Initiator sends request
* END_REQ: Target accepts request
* BEGIN_RESP: Target sends response
* END_RESP: Initiator completes transaction

### Event-Driven Simulation

System timing is modeled using `sc_event` and delayed notifications to emulate hardware latency.

### Modular Design

Each component is implemented independently, improving scalability, readability, and maintainability.

---

## Limitations

* Single outstanding transaction (no pipelining)
* No burst transfers
* No arbitration (single initiator system)
* Simplified memory and peripheral models

---

## Future Improvements

* Support multiple DMA channels
* Add burst transfer capability
* Implement arbitration for multiple initiators
* Introduce transaction queues and pipelining
* Extend monitor for detailed protocol tracing

---
