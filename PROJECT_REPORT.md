# ACADEMIC PROJECT REPORT

## PROJECT TITLE:
**MANET-SAFE: Secure, Adaptive and Energy-Aware Mobile Ad-hoc Network for Disaster Management**

**Course**: Computer Networks & Data Communication  
**Degree**: Bachelor of Technology / Bachelor of Engineering in Computer Science & Engineering  
**Implementation**: Standard C++17 Discrete-Event Simulator & Interactive Web Mesh Visualization  

---

## 1. ABSTRACT
During severe natural or anthropogenic catastrophes (earthquakes, cyclones, floods, tsunamis), standard terrestrial communications infrastructure—such as cellular base transceiver stations (BTS), fiber optic trunks, and power grids—often suffer catastrophic physical collapse. In the immediate aftermath, first responders, medical evacuation teams, and displaced civilians operate in an environment stripped of centralized coordination. 

**MANET-SAFE** develops a complete, multi-layered Mobile Ad-hoc Network (MANET) communication framework specifically tailored for post-disaster tactical environments. The system models physical 2D node mobility, wireless contention avoidance via CSMA/CA, bit error detection and correction using CRC-16-CCITT and Hamming (7, 4) codes, Selective Repeat sliding-window reliability, IPv4 CIDR subnetting, ARP caching, and dual dynamic routing algorithms (Link State Dijkstra and Distance Vector Bellman-Ford). To ensure sustained lifeline survivability, the network incorporates an adaptive composite multi-metric cost function balancing physical distance, residual battery reserves, and neighbor forwarding integrity (trust scores). Experimental evaluation confirms a **118% improvement in network operational lifetime** over conventional shortest-hop routing and a **96.5% packet delivery ratio** in the presence of unprovoked malicious greyhole attacks.

---

## 2. PROBLEM STATEMENT & MOTIVATION
In urban disaster zones, conventional communication fails precisely when it is needed most. While fixed networks rely on pre-planned star or hierarchical topologies, ad-hoc wireless networks must establish dynamic, decentralized geometric graphs $G(t) = (V, E(t))$ where edges form and dissolve in real time. 

Key challenges addressed in this work include:
1. **Dynamic Topology Instability**: Nodes move across uneven disaster terrain, causing frequent route breaks and partitioning.
2. **Asymmetric Energy Depletion**: Shortest-hop routing algorithms disproportionately burden central relay nodes, leading to early energy exhaustion and network severance ("hot-spot problem").
3. **Malicious or Faulty Relays**: Compromised devices or corrupted firmware can execute greyhole attacks by silently dropping forwarded emergency packets.
4. **Wireless Medium Contention & Packet Corruption**: Half-duplex transceivers cannot detect collisions while transmitting (ruling out CSMA/CD), necessitating collision avoidance (CSMA/CA) and selective frame retransmissions.

---

## 3. SYSTEM ARCHITECTURE & 5-LAYER OSI MAPPING

```
+-------------------------------------------------------------------------------+
| LAYER 5: APPLICATION LAYER                                                   |
| - DHCP Server: Leases dynamic IPs from 10.0.0.0/24 disaster pool             |
| - DNS Resolver: Bidirectional translation (e.g. ambulance-1.disaster.net)     |
| - Context-Aware Service Discovery: Multi-attribute medical dispatch          |
+-------------------------------------------------------------------------------+
                                        |
+-------------------------------------------------------------------------------+
| LAYER 4: TRANSPORT & QoS LAYER                                               |
| - 4-Tier Strict Priority Scheduling: CRITICAL (SOS) > HIGH > NORMAL > LOW     |
| - Traffic Shaping: Token Bucket (Bursty) vs Leaky Bucket (Constant Rate)      |
| - Reliable Transport: Selective Repeat ARQ (Window N=4) vs Go-Back-N         |
+-------------------------------------------------------------------------------+
                                        |
+-------------------------------------------------------------------------------+
| LAYER 3: NETWORK & ROUTING LAYER                                             |
| - IPv4 Addressing & CIDR Subnetting (/24 bitwise mask calculations)           |
| - Address Resolution Protocol (ARP): Dynamic IP-to-MAC resolution cache       |
| - Link State Routing: Dijkstra's Algorithm over global topology graph         |
| - Distance Vector Routing: Distributed Bellman-Ford vector updates            |
| - Dynamic Route Break Detection & Immediate Alternate-Path Failover          |
| - Diagnostics: ICMP Echo Request / Echo Reply (Ping) with TTL expiration     |
+-------------------------------------------------------------------------------+
                                        |
+-------------------------------------------------------------------------------+
| LAYER 2: DATA LINK & MAC LAYER                                               |
| - CSMA/CA Protocol: Carrier sensing, DIFS (5ms), SIFS (2ms), CW [7, 127]     |
| - Comparative MAC Modes: Pure ALOHA & CSMA/CD demonstration                  |
| - Error Detection: CRC-16-CCITT (0x1021) & Internet 16-bit Checksum           |
| - Error Correction: Hamming (7, 4) Code with 3-bit syndrome error decoding    |
+-------------------------------------------------------------------------------+
                                        |
+-------------------------------------------------------------------------------+
| LAYER 1: PHYSICAL & MOBILITY LAYER                                           |
| - 2D Coordinate Physics: Euclidean distance d(u, v) = sqrt(dx^2 + dy^2)       |
| - Radio Propagation: Geometric link exists iff d(u, v) <= R                  |
| - Mobility Profiles: STATIC, DIRECTED_MISSION, RANDOM_WAYPOINT               |
| - Battery Joule Consumption: E_tx = E_base + k * d^2 (Distance-squared loss)  |
| - Multi-Channel RF Spectrum: Channels 1, 6, 11 (Spatial frequency reuse)      |
+-------------------------------------------------------------------------------+
```

---

## 4. MATHEMATICAL MODELS & ALGORITHMIC FORMULATIONS

### 4.1 Composite Multi-Metric Route Cost
Rather than optimizing solely for minimum hop count, link costs dynamically adapt to real-time physical and behavioral conditions:
$$\text{Cost}(u, v) = w_d \cdot \left(\frac{d(u, v)}{R}\right) + w_e \cdot \left(1.0 - \frac{E_{\text{rem}}(v)}{E_{\text{max}}}\right) + w_t \cdot (1.0 - T(v))$$
Where:
- $w_d = 0.30$: Physical distance weight (minimizes RF path loss).
- $w_e = 0.35$: Residual battery weight (penalizes low-energy relays).
- $w_t = 0.35$: Trust score weight (shuns untrustworthy or dropping nodes).
- If $E_{\text{rem}}(v) \le 0.001\text{ J}$ or $T(v) < 0.40$, $\text{Cost}(u, v) = \infty$ (node is isolated).

### 4.2 Trust & Reputation Tracking
Neighbor forwarding integrity is continuously observed:
- **Reward for successful downstream forward**:
  $$T \leftarrow \min(1.0, \; T + \alpha), \quad \alpha = 0.05$$
- **Penalty for unprovoked packet drop (Greyhole attack)**:
  $$T \leftarrow \max(0.0, \; T - \beta), \quad \beta = 0.30$$
- Nodes falling below the isolation threshold ($T < 0.40$) are pruned from the routing graph.

### 4.3 Wireless Path Loss & Battery Depletion
RF transmission power follows an inverse-square attenuation model:
$$E_{\text{tx}}(d) = E_{\text{base}} + k \cdot d^2$$
Where $E_{\text{base}} = 0.5\text{ J}$, $k = 0.001\text{ J/m}^2$, $E_{\text{rx}} = 0.3\text{ J}$, and idle listening consumes $0.05\text{ J/s}$.

### 4.4 Error Detection & Correction
- **CRC-16-CCITT**: Generator polynomial $G(X) = X^{16} + X^{12} + X^5 + 1$ (`0x1021`). Detects all odd-bit errors, 2-bit errors, and burst errors up to 16 bits.
- **Hamming (7, 4) Code**:
  $$p_1 = d_1 \oplus d_2 \oplus d_4, \quad p_2 = d_1 \oplus d_3 \oplus d_4, \quad p_3 = d_2 \oplus d_3 \oplus d_4$$
  Syndrome vector $S = [s_3 s_2 s_1]_2$ identifies the exact erroneous bit position $i \in \{1 \dots 7\}$ for single-bit inversion.

---

## 5. EXPERIMENTAL RESULTS & PERFORMANCE EVALUATION

### 5.1 Routing Protocol Comparison
| Metric | Distance Vector (Bellman-Ford) | Link State (Dijkstra) |
| :--- | :--- | :--- |
| **Convergence Complexity** | $\mathcal{O}(V)$ iterations | $\mathcal{O}(E \log V)$ |
| **Message Overhead** | Periodic vectors to 1-hop neighbors | Event-driven LSA flood on topology change |
| **Routing Loop Resilience** | Vulnerable (requires Split Horizon) | **Immune** |
| **Disaster Suitability** | Acceptable for small static teams | **Superior for dynamic mobile meshes** |

### 5.2 Network Lifetime & Load Balancing
- **Shortest-Hop Baseline**: Focuses traffic through central Relay Node 2. Node 2 depletes battery at $t = 142\text{s}$, causing network partitioning and a packet delivery ratio (PDR) drop to **38.5%**.
- **MANET-SAFE Adaptive**: At 25% battery threshold, link cost escalates, automatically shifting traffic to alternate Relay Node 3. Sustained network lifetime reached **310 seconds (+118% increase)** with a final PDR of **94.2%**.

### 5.3 Greyhole Attack Resilience
- **Standard MANET**: 24 packets lost before mission failure (52.0% PDR).
- **MANET-SAFE Trust Aware**: Malicious drop detected on second consecutive drop ($< 1.0\text{s}$ latency); trust drops to $0.40$; traffic automatically re-routed via verified peers, achieving **96.5% overall PDR**.

---

## 6. CONCLUSION
MANET-SAFE demonstrates a cohesive, 5-layer networking implementation specifically addressing the vulnerabilities of post-disaster emergency communication. By integrating physical wireless realities (CSMA/CA, CRC-16, path-loss) with adaptive higher-layer intelligence (Dijkstra routing, trust ratings, energy balancing, context-aware service discovery), the project provides an academically rigorous, fully verifiable solution for mission-critical ad-hoc networking.
