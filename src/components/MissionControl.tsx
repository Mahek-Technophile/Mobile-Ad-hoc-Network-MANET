import React, { useState } from 'react';
import { NodeDevice, PacketPriority } from '../types';
import { Send, Zap, Bug, RefreshCw, Radio, Flame, ShieldAlert, Sparkles } from 'lucide-react';

interface ControlsProps {
  nodes: NodeDevice[];
  onSendEmergencySos: (srcId: number, destId: number, priority: PacketPriority, message: string) => void;
  onDrainBattery: (nodeId: number) => void;
  onRechargeBattery: (nodeId: number) => void;
  onToggleMalicious: (nodeId: number) => void;
  onInjectNoise: () => void;
  onResetTopology: () => void;
  routingMode: 'LINK_STATE' | 'DISTANCE_VECTOR';
  onToggleRoutingMode: () => void;
}

export const MissionControl: React.FC<ControlsProps> = ({
  nodes,
  onSendEmergencySos,
  onDrainBattery,
  onToggleMalicious,
  onInjectNoise,
  onResetTopology,
  routingMode,
  onToggleRoutingMode,
}) => {
  const [srcNodeId, setSrcNodeId] = useState<number>(1);
  const [destNodeId, setDestNodeId] = useState<number>(5);
  const [priority, setPriority] = useState<PacketPriority>('CRITICAL');
  const [message, setMessage] = useState<string>('SOS: Evacuation required at flood sector 5');

  const handleSend = (e: React.FormEvent) => {
    e.preventDefault();
    if (srcNodeId === destNodeId) return;
    onSendEmergencySos(srcNodeId, destNodeId, priority, message);
  };

  return (
    <div className="bg-white border border-slate-200/90 rounded-xl p-5 shadow-sm space-y-4">
      {/* Title & Algorithm Toggle */}
      <div className="flex items-center justify-between border-b border-slate-100 pb-3">
        <div className="flex items-center space-x-2">
          <Radio className="w-4 h-4 text-blue-600" />
          <h2 className="text-xs font-bold text-slate-800 uppercase tracking-wider">
            Network Actions & Fault Injection
          </h2>
        </div>
        <button
          onClick={onToggleRoutingMode}
          className="px-2.5 py-1 text-xs font-medium rounded-lg bg-blue-50 hover:bg-blue-100/80 text-blue-700 border border-blue-200 transition"
        >
          Routing: {routingMode === 'LINK_STATE' ? 'Link State (Dijkstra)' : 'Distance Vector (B-F)'}
        </button>
      </div>

      {/* Primary Dispatch Form */}
      <form onSubmit={handleSend} className="space-y-3">
        <div className="grid grid-cols-1 md:grid-cols-4 gap-2.5 text-xs">
          <div>
            <label className="text-slate-500 font-medium block mb-1">Source Node</label>
            <select
              value={srcNodeId}
              onChange={(e) => setSrcNodeId(Number(e.target.value))}
              className="w-full bg-slate-50 border border-slate-200 rounded-lg px-2.5 py-1.5 text-slate-700 focus:outline-none focus:border-blue-500 font-medium"
            >
              {nodes.map((n) => (
                <option key={n.id} value={n.id}>
                  Node {n.id} ({n.name})
                </option>
              ))}
            </select>
          </div>

          <div>
            <label className="text-slate-500 font-medium block mb-1">Destination Node</label>
            <select
              value={destNodeId}
              onChange={(e) => setDestNodeId(Number(e.target.value))}
              className="w-full bg-slate-50 border border-slate-200 rounded-lg px-2.5 py-1.5 text-slate-700 focus:outline-none focus:border-blue-500 font-medium"
            >
              {nodes.map((n) => (
                <option key={n.id} value={n.id}>
                  Node {n.id} ({n.name})
                </option>
              ))}
            </select>
          </div>

          <div>
            <label className="text-slate-500 font-medium block mb-1">QoS Priority (Unit IV)</label>
            <select
              value={priority}
              onChange={(e) => setPriority(e.target.value as PacketPriority)}
              className="w-full bg-slate-50 border border-slate-200 rounded-lg px-2.5 py-1.5 text-slate-700 focus:outline-none focus:border-blue-500 font-medium"
            >
              <option value="CRITICAL">CRITICAL (Emergency SOS)</option>
              <option value="HIGH">HIGH (Tactical Command)</option>
              <option value="NORMAL">NORMAL (Sensor Telemetry)</option>
              <option value="LOW">LOW (Diagnostic Ping)</option>
            </select>
          </div>

          <div className="flex items-end">
            <button
              type="submit"
              className="w-full bg-blue-600 hover:bg-blue-700 text-white font-semibold py-1.5 px-3 rounded-lg flex items-center justify-center space-x-1.5 transition shadow-sm text-xs"
            >
              <Send className="w-3.5 h-3.5" />
              <span>Route Packet</span>
            </button>
          </div>
        </div>

        <div>
          <label className="text-slate-500 font-medium text-xs block mb-1">Payload Content</label>
          <input
            type="text"
            value={message}
            onChange={(e) => setMessage(e.target.value)}
            className="w-full bg-slate-50 border border-slate-200 rounded-lg px-3 py-1.5 text-xs text-slate-700 focus:outline-none focus:border-blue-500"
            placeholder="Type payload string..."
          />
        </div>
      </form>

      {/* Guided Demonstration Buttons */}
      <div className="pt-2 border-t border-slate-100 flex flex-wrap items-center gap-2 text-xs">
        <span className="text-slate-400 font-medium text-[11px] mr-1">Interactive Scenarios:</span>

        <button
          onClick={() => onDrainBattery(2)}
          className="bg-amber-50 hover:bg-amber-100/80 text-amber-800 border border-amber-200 px-2.5 py-1 rounded-md flex items-center space-x-1.5 transition font-medium text-[11.5px]"
        >
          <Flame className="w-3.5 h-3.5 text-amber-600" />
          <span>Drain Node 2 (Energy Failover)</span>
        </button>

        <button
          onClick={() => onToggleMalicious(3)}
          className="bg-rose-50 hover:bg-rose-100/80 text-rose-800 border border-rose-200 px-2.5 py-1 rounded-md flex items-center space-x-1.5 transition font-medium text-[11.5px]"
        >
          <ShieldAlert className="w-3.5 h-3.5 text-rose-600" />
          <span>Toggle Node 3 Greyhole</span>
        </button>

        <button
          onClick={onInjectNoise}
          className="bg-purple-50 hover:bg-purple-100/80 text-purple-800 border border-purple-200 px-2.5 py-1 rounded-md flex items-center space-x-1.5 transition font-medium text-[11.5px]"
        >
          <Bug className="w-3.5 h-3.5 text-purple-600" />
          <span>Inject CRC-16 Noise (ARQ)</span>
        </button>

        <button
          onClick={onResetTopology}
          className="bg-slate-100 hover:bg-slate-200/80 text-slate-700 border border-slate-200 px-2.5 py-1 rounded-md flex items-center space-x-1.5 transition ml-auto font-medium text-[11.5px]"
        >
          <RefreshCw className="w-3.5 h-3.5 text-slate-500" />
          <span>Reset Network</span>
        </button>
      </div>
    </div>
  );
};
