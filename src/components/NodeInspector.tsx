import React from 'react';
import { NodeDevice, RouteEntry } from '../types';
import { Battery, Shield, Cpu, Network, MapPin } from 'lucide-react';

interface NodeDetailsProps {
  node: NodeDevice | null;
  routes: RouteEntry[];
  onRecharge: (id: number) => void;
  onToggleMalicious: (id: number) => void;
}

export const NodeInspector: React.FC<NodeDetailsProps> = ({
  node,
  routes,
  onRecharge,
  onToggleMalicious,
}) => {
  if (!node) {
    return (
      <div className="bg-white border border-slate-200/90 rounded-xl p-5 shadow-sm flex flex-col items-center justify-center text-slate-400 h-full text-xs text-center">
        <Cpu className="w-8 h-8 mb-2 opacity-50 text-blue-500" />
        <p className="font-medium text-slate-600">Select any node on the mesh</p>
        <p className="text-[11px] text-slate-400 mt-0.5">Inspect battery, routing table & trust score</p>
      </div>
    );
  }

  const battPercent = Math.max(0, Math.min(100, (node.batteryJoules / node.maxBatteryJoules) * 100));

  return (
    <div className="bg-white border border-slate-200/90 rounded-xl p-5 shadow-sm space-y-4 text-xs">
      <div className="flex items-start justify-between border-b border-slate-100 pb-3">
        <div>
          <div className="flex items-center space-x-2">
            <span className="font-bold text-slate-900 text-sm">{node.name}</span>
            <span className="px-1.5 py-0.5 rounded bg-blue-50 text-[10px] text-blue-700 font-mono font-semibold border border-blue-100">
              ID: {node.id}
            </span>
          </div>
          <p className="text-[11px] text-slate-500 font-mono mt-0.5">
            IP: {node.ip} | MAC: 0x{node.mac.toString(16).toUpperCase()}
          </p>
        </div>

        <span className="px-2 py-0.5 rounded text-[10px] font-bold tracking-wide uppercase bg-slate-100 text-slate-700">
          {node.role.replace('_', ' ')}
        </span>
      </div>

      {/* Gauges: Battery & Reputation */}
      <div className="grid grid-cols-2 gap-2.5">
        <div className="bg-slate-50 p-2.5 rounded-lg border border-slate-200/80">
          <div className="flex items-center justify-between text-slate-600 mb-1">
            <span className="flex items-center space-x-1 font-medium text-[11px]">
              <Battery className="w-3.5 h-3.5 text-emerald-600" />
              <span>Battery</span>
            </span>
            <span className="font-mono text-slate-800 font-bold">{battPercent.toFixed(0)}%</span>
          </div>
          <div className="w-full bg-slate-200 h-2 rounded-full overflow-hidden">
            <div
              className={`h-full transition-all duration-300 ${
                battPercent < 25 ? 'bg-rose-500' : 'bg-emerald-500'
              }`}
              style={{ width: `${battPercent}%` }}
            />
          </div>
          <button
            onClick={() => onRecharge(node.id)}
            className="mt-1.5 text-[10px] text-blue-600 hover:text-blue-800 hover:underline block font-medium"
          >
            Recharge to 100%
          </button>
        </div>

        <div className="bg-slate-50 p-2.5 rounded-lg border border-slate-200/80">
          <div className="flex items-center justify-between text-slate-600 mb-1">
            <span className="flex items-center space-x-1 font-medium text-[11px]">
              <Shield className="w-3.5 h-3.5 text-indigo-600" />
              <span>Trust Rating</span>
            </span>
            <span className="font-mono text-slate-800 font-bold">{node.trustScore.toFixed(2)}</span>
          </div>
          <div className="w-full bg-slate-200 h-2 rounded-full overflow-hidden">
            <div
              className={`h-full transition-all duration-300 ${
                node.trustScore < 0.4 ? 'bg-rose-500' : 'bg-indigo-600'
              }`}
              style={{ width: `${node.trustScore * 100}%` }}
            />
          </div>
          <button
            onClick={() => onToggleMalicious(node.id)}
            className="mt-1.5 text-[10px] text-rose-600 hover:text-rose-800 hover:underline block font-medium"
          >
            {node.isMalicious ? 'Disable Drop Attack' : 'Enable Drop Attack'}
          </button>
        </div>
      </div>

      {/* Layer 3: Routing Table */}
      <div>
        <h3 className="font-bold text-slate-800 mb-1.5 flex items-center space-x-1.5 text-[11.5px]">
          <Network className="w-3.5 h-3.5 text-blue-600" />
          <span>Active Forwarding Table (Unit III)</span>
        </h3>
        <div className="max-h-32 overflow-y-auto rounded-lg border border-slate-200 bg-slate-50/60">
          <table className="w-full text-left font-mono text-[10.5px]">
            <thead className="bg-slate-100 text-slate-600 sticky top-0 border-b border-slate-200">
              <tr>
                <th className="p-1.5">Dest IP</th>
                <th className="p-1.5">Next Hop</th>
                <th className="p-1.5">Hops</th>
                <th className="p-1.5">Cost</th>
              </tr>
            </thead>
            <tbody className="divide-y divide-slate-200/80 text-slate-700">
              {routes.map((r, i) => (
                <tr key={i} className="hover:bg-white">
                  <td className="p-1.5 text-blue-700 font-semibold">{r.destinationIp}</td>
                  <td className="p-1.5">{r.nextHopIp}</td>
                  <td className="p-1.5">{r.hops}</td>
                  <td className="p-1.5 font-bold text-emerald-700">{r.cost.toFixed(2)}</td>
                </tr>
              ))}
              {routes.length === 0 && (
                <tr>
                  <td colSpan={4} className="p-2 text-center text-slate-400">
                    No active paths available
                  </td>
                </tr>
              )}
            </tbody>
          </table>
        </div>
      </div>

      {/* Discovered Neighbors */}
      <div className="pt-2 border-t border-slate-100">
        <div className="flex items-center justify-between text-slate-500 mb-1 text-[11px]">
          <span className="flex items-center space-x-1">
            <MapPin className="w-3.5 h-3.5 text-blue-500" />
            <span className="font-medium text-slate-700">Discovered 1-Hop Neighbors</span>
          </span>
          <span className="font-mono text-slate-600">{node.activeNeighbors.length} nodes</span>
        </div>
        <div className="flex flex-wrap gap-1">
          {node.activeNeighbors.map((nbrId) => (
            <span
              key={nbrId}
              className="px-2 py-0.5 rounded bg-slate-100 border border-slate-200 text-[10px] font-medium text-slate-700"
            >
              Node {nbrId}
            </span>
          ))}
          {node.activeNeighbors.length === 0 && (
            <span className="text-slate-400 italic text-[11px]">Isolated (no nodes in {node.range}m range)</span>
          )}
        </div>
      </div>
    </div>
  );
};
