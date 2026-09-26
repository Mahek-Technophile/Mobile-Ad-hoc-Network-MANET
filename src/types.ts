export type NodeType = 'COMMAND_CENTER' | 'RESCUE_TEAM' | 'AMBULANCE' | 'SENSOR' | 'VOLUNTEER';
export type PacketPriority = 'CRITICAL' | 'HIGH' | 'NORMAL' | 'LOW';
export type PacketType = 'DATA' | 'BEACON' | 'ACK' | 'NACK' | 'PING' | 'SERVICE_REQ' | 'SERVICE_REP';

export interface Coordinate {
  x: number;
  y: number;
}

export interface NetworkPacket {
  packetId: number;
  srcIp: string;
  destIp: string;
  type: PacketType;
  priority: PacketPriority;
  ttl: number;
  sequenceNumber: number;
  payload: string;
  timestamp: number;
}

export interface MacFrame {
  srcMac: number;
  nextHopMac: number;
  seqNumber: number;
  crc16: number;
  packet: NetworkPacket;
}

export interface NodeDevice {
  id: number;
  ip: string;
  mac: number;
  role: NodeType;
  name: string;
  pos: Coordinate;
  range: number; // meters
  batteryJoules: number;
  maxBatteryJoules: number;
  trustScore: number; // 0.0 - 1.0
  isMalicious: boolean;
  dropProbability: number;
  channel: number; // 1, 6, 11
  speed: number;
  targetWaypoint?: Coordinate;
  activeNeighbors: number[];
  packetsSent: number;
  packetsReceived: number;
  packetsDropped: number;
}

export interface RouteEntry {
  destinationIp: string;
  nextHopIp: string;
  nextHopMac: number;
  hops: number;
  cost: number;
}

export interface SimulationEventLog {
  id: string;
  timestamp: number;
  category: 'TOPOLOGY' | 'ROUTING' | 'MAC' | 'ERROR' | 'QOS' | 'SECURITY' | 'SERVICE';
  level: 'info' | 'success' | 'warning' | 'error';
  message: string;
  nodeId?: number;
}

export interface PacketInTransit {
  id: string;
  fromNodeId: number;
  toNodeId: number;
  progress: number; // 0 to 1
  packet: NetworkPacket;
  status: 'transmitting' | 'delivered' | 'corrupted' | 'dropped';
}
