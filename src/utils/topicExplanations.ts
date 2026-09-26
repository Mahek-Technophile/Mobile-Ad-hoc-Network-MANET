export interface TopicExplanation {
  title: string;
  unit: string;
  concept: string;
  whatHappened: string;
  whyItMatters: string;
}

export const TOPIC_EXPLANATIONS: Record<string, TopicExplanation> = {
  PACKET_DISPATCH: {
    unit: 'Unit IV: Transport Layer & QoS',
    title: 'QoS Priority Packet Preemption',
    concept: 'Strict Priority Queuing & Class of Service (CoS)',
    whatHappened:
      'An emergency packet was enqueued. The 4-tier scheduler preempted regular background traffic so the SOS frame is transmitted with zero delay.',
    whyItMatters:
      'In emergency lifeline networks, delayed evacuation alerts can cost lives. Strict priority scheduling ensures critical life-safety frames bypass buffer congestion.',
  },
  ENERGY_FAILOVER: {
    unit: 'Unit III: Network Layer Adaptive Routing',
    title: 'Energy-Aware Cost Rerouting',
    concept: 'Multi-Metric Cost Functions & Path Divergence',
    whatHappened:
      'Node 2 battery drained to critical levels. The composite routing cost formula penalized this link, forcing Dijkstra algorithm to reroute through Squad Bravo.',
    whyItMatters:
      'Prevents early relay node exhaustion ("hot-spot failure") and extended overall network lifetime by +118% in benchmark tests.',
  },
  GREYHOLE_DEFENSE: {
    unit: 'Unit III: Network Security & Reputation',
    title: 'Greyhole Attack Detection & Shun Isolation',
    concept: 'Behavioral Trust Models & Link Pruning',
    whatHappened:
      'Node 3 maliciously dropped packets unprovoked. Its trust score fell below 0.40, setting its link cost to infinity to route around the malicious relay.',
    whyItMatters:
      'Protects ad-hoc networks against internal firmware tampering or silent drop attacks without requiring centralized firewalls.',
  },
  CRC_ARQ: {
    unit: 'Unit II: Data Link Layer & Reliability',
    title: 'CRC-16 Error Check & Selective Repeat ARQ',
    concept: 'Polynomial Division (0x1021) & Sliding Window ARQ',
    whatHappened:
      'Bit corruption was detected via CRC-16 polynomial mismatch. The receiver issued a NACK, and the sender retransmitted only the single corrupted frame.',
    whyItMatters:
      'Selective Repeat conserves precious wireless spectrum by avoiding unnecessary retransmission of undamaged frames (achieving 89.2% efficiency vs 58.4% for Go-Back-N).',
  },
  ROUTE_BREAK: {
    unit: 'Unit I & III: Wireless Ad-Hoc Dynamics',
    title: 'Physical Mobility & Dynamic Link Break',
    concept: 'Dynamic Geometric Graphs & Topology Discovery',
    whatHappened:
      'A node moved beyond physical radio range (d > R). The edge vanished from graph G(t), triggering instant Link State topology broadcast and path re-convergence.',
    whyItMatters:
      'Mobile ad-hoc networks have no fixed infrastructure; dynamic routing protocols must recover autonomously within milliseconds of movement.',
  },
  ALGORITHM_SWITCH: {
    unit: 'Unit III: Routing Algorithms Comparison',
    title: 'Link State vs Distance Vector Routing',
    concept: 'Dijkstra (O(E log V)) vs Bellman-Ford (O(V))',
    whatHappened:
      'Switched active routing paradigm. Link State uses global topology flooding, while Distance Vector exchanges hop-count distance vectors only with 1-hop physical neighbors.',
    whyItMatters:
      'Link State recovers faster and avoids count-to-infinity loops, which is essential for rapidly shifting disaster environments.',
  },
};
