# MANET-SAFE: ACADEMIC PRESENTATION SLIDE OUTLINE

**Course**: Computer Networks & Data Communication  
**Project**: MANET-SAFE — Secure, Adaptive and Energy-Aware Mobile Ad-hoc Network for Disaster Management  
**Format**: 12-Slide Defense Deck  

---

### SLIDE 1: TITLE SLIDE
- **Title**: MANET-SAFE: Secure, Adaptive and Energy-Aware Mobile Ad-hoc Network for Disaster Management
- **Domain**: Computer Networks, Wireless Ad-hoc Systems, Disaster Emergency Communications
- **Technology Stack**: Standard C++17 Discrete Simulator, React Interactive Visualizer, Tailwind CSS
- **Team Details**: Candidate Name & Roll Number

---

### SLIDE 2: MOTIVATION & PROBLEM STATEMENT
- **The Disaster Scenario**: Terrestrial towers, optical fiber, and electrical grids collapse during earthquakes or floods.
- **The Problem**: Conventional client-server networks cannot function without base stations.
- **Key Challenges**:
  - *Dynamic Topology*: Rapid route breaks due to continuous node mobility.
  - *Energy Depletion*: Shortest-hop algorithms exhaust intermediate relays, creating network deadzones.
  - *Vulnerability to Attack*: Malicious or faulty nodes act as greyholes by dropping forwarded emergency alerts.
  - *Radio Interference*: Half-duplex transceivers cannot detect collisions while transmitting.

---

### SLIDE 3: SYSTEM ARCHITECTURE & 5-LAYER OSI MAPPING
- **Layer 5 (Application)**: DHCP IP pool leasing, DNS name resolution, context-aware multi-criteria service discovery.
- **Layer 4 (Transport)**: 4-tier strict priority QoS (`CRITICAL` SOS preemption), Token Bucket vs Leaky Bucket traffic shaping, Selective Repeat ARQ.
- **Layer 3 (Network)**: 32-bit IPv4 CIDR subnetting (`/24`), dynamic ARP cache, Link State (Dijkstra) vs Distance Vector (Bellman-Ford), multi-metric adaptive cost function.
- **Layer 2 (Data Link / MAC)**: CSMA/CA with DIFS/SIFS and binary exponential backoff ($CW \in [7, 127]$), CRC-16-CCITT, 16-bit Checksum, Hamming (7, 4) code.
- **Layer 1 (Physical / Mobility)**: 2D Euclidean distance ($d \le R$), path loss ($E_{\text{tx}} \propto d^2$), orthogonal channels (1, 6, 11).

---

### SLIDE 4: PHYSICAL LAYER & WIRELESS MOBILITY
- Geometric Connectivity Graph: $G(t) = (V, E(t))$ where edge $(u, v) \in E(t) \iff d(u, v) \le R$.
- Mobility Models:
  - `STATIC`: Base stations (Command HQ).
  - `DIRECTED_MISSION`: Waypoint-directed rescue teams moving toward target sectors.
  - `RANDOM_WAYPOINT`: Displaced civilians wandering within bounded zones.
- Real-time link break detection upon moving beyond radio range threshold.

---

### SLIDE 5: MAC LAYER: CSMA/CA & ERROR CONTROL
- **Why CSMA/CD Fails in Wireless**: High signal attenuation ($10^5 - 10^8$) means a node's local transmission overwhelms incoming signals; hardware cannot listen while transmitting.
- **CSMA/CA Mechanics**: Senses channel; if busy, backs off using random slots $CW \leftarrow \min(CW_{\text{max}}, 2(CW+1)-1)$. Transmits after DIFS idle interval.
- **Error Control**:
  - **CRC-16-CCITT** polynomial $X^{16} + X^{12} + X^5 + 1$ (`0x1021`) for burst error detection.
  - **Hamming (7, 4) Code**: 3 parity bits detect and correct single-bit transmission errors via syndrome decoding.

---

### SLIDE 6: TRANSPORT & QoS LIFELINE PRIORITIZATION
- **4-Tier Strict Priority Scheduling**:
  - Tier 0: `CRITICAL` (SOS Evacuation alert) — Dispatched with zero queuing delay.
  - Tier 1: `HIGH` (Tactical rescue squad orders).
  - Tier 2: `NORMAL` (Gas/seismic sensor telemetry).
  - Tier 3: `LOW` (Background diagnostic pings).
- **Traffic Shaping**:
  - **Token Bucket**: Allows bursty transmissions up to capacity $B$, refilling at rate $R$.
  - **Leaky Bucket**: Enforces a constant output rate to eliminate jitter.

---

### SLIDE 7: SLIDING WINDOW: SELECTIVE REPEAT vs GO-BACK-N
- **Go-Back-N ($N=4$)**: Retransmits all frames from the lost packet forward, wasting scarce wireless bandwidth (12 unnecessary duplicate frames in tests).
- **Selective Repeat ($N=4$)**: Receiver maintains out-of-order buffer and returns individual ACKs/NACKs; sender retransmits **only** the corrupted frame (0 duplicate frames, 89.2% channel efficiency).

---

### SLIDE 8: ADAPTIVE MULTI-METRIC ROUTING
- **Composite Cost Equation**:
  $$\text{Cost}(u, v) = 0.30 \cdot \left(\frac{d}{R}\right) + 0.35 \cdot \left(1 - \frac{E_{\text{rem}}}{E_{\text{max}}}\right) + 0.35 \cdot (1 - \text{Trust})$$
- **Dynamic Energy Balancing**: As relay battery drops below 25%, route cost escalates, automatically shifting traffic to alternate healthy relays.
- Prevents early node death and extends network lifetime by **118%**.

---

### SLIDE 9: TRUST MANAGEMENT & GREYHOLE ATTACK DEFENSE
- **Behavioral Reputation Tracking**:
  - Successful downstream forward confirmation: $T \leftarrow \min(1.0, T + 0.05)$.
  - Malicious packet drop: $T \leftarrow \max(0.0, T - 0.30)$.
- **Isolation Threshold ($T < 0.40$)**: The node's link cost becomes $\infty$, pruning it from Dijkstra shortest-path calculations.
- Detects and mitigates greyhole attacks in $< 1.0\text{s}$, achieving **96.5% PDR**.

---

### SLIDE 10: APPLICATION LAYER & CONTEXT-AWARE DISCOVERY
- **DHCP**: Dynamic allocation from `10.0.0.0/24` subnet.
- **DNS**: Hostname-to-IP resolution (`ambulance-1.disaster.net` $\to$ `10.0.0.4`).
- **Context-Aware Service Query**: Trapped civilians query: *"Find nearest viable Ambulance"*.
  - Engine evaluates: Role (`AMBULANCE`), Distance, Battery residual ($>25\%$), and Trust rating ($>0.60$) to dispatch the optimal medical vehicle.

---

### SLIDE 11: EXPERIMENTAL RESULTS & BENCHMARKS
- **Lifetime**: Adaptive routing achieved **310s** vs **142s** for standard shortest-hop (+118%).
- **Security**: Recovered from 52.0% PDR under attack to **96.5% PDR** with trust isolation.
- **ARQ**: Selective Repeat achieved **89.2% channel efficiency** vs 58.4% for Go-Back-N.
- **Routing**: Link State converged in $\mathcal{O}(E \log V)$ with zero count-to-infinity risk.

---

### SLIDE 12: CONCLUSION & FUTURE SCOPE
- **Conclusion**: MANET-SAFE delivers a fully functional, mathematically grounded, 5-layer emergency mesh architecture verified under realistic disaster conditions.
- **Future Enhancements**: Drone relay placement optimization (UAV deployment) and post-quantum cryptographic authentication for tactical military/UN rescue missions.
