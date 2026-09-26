# MANET-SAFE: Secure, Adaptive and Energy-Aware Mobile Ad-hoc Network for Disaster Management

A complete, standard **C++17** discrete-event Mobile Ad-hoc Network (MANET) simulator designed for Computer Networks and Data Communication academic coursework, demonstration, and viva defense.

---

## 1. Project Overview & Motivation
During catastrophic events (earthquakes, cyclones, floods), conventional communication infrastructure—such as cellular towers and wired internet backbones—frequently collapses. Emergency responders, medical units, and civilians require a resilient, decentralized communication network.

**MANET-SAFE** models a realistic disaster response wireless mesh network. It integrates syllabus concepts across all five layers of the OSI/TCP-IP model into a single, modular C++ architecture.

---

## 2. Layer-by-Layer Syllabus Concepts

| OSI Layer | Implemented Feature | Networking Concept |
| :--- | :--- | :--- |
| **Physical & Mobility** | 2D Coordinates, Dynamic Range $d \le R$ | Radio propagation, node mobility, dynamic geometric connectivity graph |
| **Data Link / MAC** | CSMA/CA with Backoff, CRC-16, Checksum, Hamming (7,4) | Contention avoidance, DIFS/SIFS, bit corruption detection & correction |
| **Network** | IPv4 CIDR Subnetting, ARP, Distance Vector vs Link State | Classless addressing (`/24`), IP-to-MAC resolution, Dijkstra vs Bellman-Ford |
| **Transport** | 4-Tier Strict Priority QoS, Token & Leaky Bucket, Selective Repeat | Emergency SOS preemption, traffic shaping, selective retransmission vs Go-Back-N |
| **Application** | DHCP, DNS, Context-Aware Service Discovery | Dynamic IP leasing, service name resolution, multi-criteria resource matching |

---

## 3. Quick Start & Build Instructions

### Prerequisites
- GCC / G++ supporting **C++17** or later (`g++ --version` >= 9.0)
- `make` utility

### Compilation Commands
```bash
# 1. Compile and run the complete 18-step disaster mission
make run

# 2. Run the experimental benchmarks and generate performance tables
make experiments

# 3. Clean all build artifacts
make clean
```

---

## 4. Key Algorithms & Mathematical Models

### 1. Composite Multi-Metric Route Cost
$$\text{Cost}(u, v) = w_d \cdot \left(\frac{d(u,v)}{R}\right) + w_e \cdot \left(1.0 - \frac{E_{\text{rem}}(v)}{E_{\text{max}}}\right) + w_t \cdot (1.0 - T(v))$$
- **$w_d$ (Distance)**: Minimizes physical radio path loss.
- **$w_e$ (Energy)**: Steers traffic away from dying or battery-depleted nodes.
- **$w_t$ (Trust)**: Automatically routes around compromised or greyhole nodes.

### 2. Trust Reputation Model
- **Forward Success Reward**: $T \leftarrow \min(1.0, T + 0.05)$
- **Unprovoked Drop Penalty**: $T \leftarrow \max(0.0, T - 0.30)$
- **Isolation Threshold**: If $T < 0.40$, the link cost is treated as $\infty$, isolating the node from the network.

### 3. Binary Exponential Backoff (CSMA/CA)
$$\text{Backoff Slots} \in [0, CW], \quad CW \leftarrow \min(CW_{\text{max}}, (CW + 1) \times 2 - 1)$$
Slots count down only when the wireless medium is continuously sensed idle for at least DIFS duration.

---

## 5. Experimental Benchmark Results

### Energy-Aware Routing vs Shortest-Hop
- **Shortest-Hop Routing**: Relies exclusively on distance, overloading the central relay until it dies at $t = 142\text{s}$ (3 nodes depleted, final PDR dropped to 38.5%).
- **MANET-SAFE Adaptive**: Distributes traffic across alternate relays as battery depletes, achieving **$310\text{s}$ network lifetime (+118% increase)** and **94.2% PDR**.

### Trust Defense vs Greyhole Attack (50% Drop)
- **Standard MANET**: 24 packets lost, PDR degraded to 52.0%.
- **MANET-SAFE Trust Aware**: Detects malicious drops in $< 1.0\text{s}$, isolates the attacker, and re-routes over honest peers, recovering to **96.5% PDR**.

### Selective Repeat vs Go-Back-N (10% Noise)
- **Go-Back-N**: Retransmits entire window on error (12 unnecessary duplicate frames, 58.4% efficiency).
- **Selective Repeat**: Retransmits only the single corrupted frame (0 duplicate frames, **89.2% efficiency**).
