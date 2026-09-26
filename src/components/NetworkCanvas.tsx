import React from 'react';
import { NodeDevice, PacketInTransit } from '../types';
import { Wifi, Info } from 'lucide-react';

interface NetworkCanvasProps {
  nodes: NodeDevice[];
  selectedNodeId: number | null;
  onSelectNode: (id: number) => void;
  packetsInTransit: PacketInTransit[];
  onMoveNode: (id: number, pos: { x: number; y: number }) => void;
}

export const NetworkCanvas: React.FC<NetworkCanvasProps> = ({
  nodes,
  selectedNodeId,
  onSelectNode,
  packetsInTransit,
  onMoveNode,
}) => {
  const [draggingId, setDraggingId] = React.useState<number | null>(null);

  const handlePointerDown = (id: number, e: React.PointerEvent) => {
    e.stopPropagation();
    onSelectNode(id);
    setDraggingId(id);
    (e.target as HTMLElement).setPointerCapture(e.pointerId);
  };

  const handlePointerMove = (e: React.PointerEvent<SVGSVGElement>) => {
    if (draggingId === null) return;
    const svg = e.currentTarget.getBoundingClientRect();
    const x = Math.max(25, Math.min(675, e.clientX - svg.left));
    const y = Math.max(25, Math.min(460, e.clientY - svg.top));
    onMoveNode(draggingId, { x, y });
  };

  const handlePointerUp = (e: React.PointerEvent) => {
    if (draggingId !== null) {
      setDraggingId(null);
    }
  };

  // Node theme palette for crisp light mode
  const getNodeFill = (node: NodeDevice) => {
    if (node.batteryJoules <= 1) return '#94a3b8'; // depleted slate
    if (node.trustScore < 0.4) return '#ef4444'; // shunned red
    switch (node.role) {
      case 'COMMAND_CENTER': return '#2563eb'; // blue
      case 'RESCUE_TEAM': return '#059669'; // emerald
      case 'AMBULANCE': return '#d97706'; // amber
      case 'VOLUNTEER': return '#7c3aed'; // violet
      case 'SENSOR': return '#0891b2'; // cyan
      default: return '#4b5563';
    }
  };

  return (
    <div className="relative w-full h-[470px] bg-slate-50/70 rounded-xl border border-slate-200/90 overflow-hidden shadow-sm select-none">
      {/* Crisp subtle dot grid */}
      <div 
        className="absolute inset-0 opacity-40 pointer-events-none"
        style={{
          backgroundImage: 'radial-gradient(#94a3b8 1px, transparent 1px)',
          backgroundSize: '24px 24px',
        }}
      />

      {/* Header bar within canvas */}
      <div className="absolute top-3 left-4 z-10 flex items-center space-x-2 text-xs bg-white/90 backdrop-blur-sm px-3 py-1.5 rounded-lg border border-slate-200 text-slate-700 shadow-sm">
        <span className="flex items-center space-x-1.5 text-emerald-600 font-semibold">
          <Wifi className="w-3.5 h-3.5 animate-pulse" />
          <span>2.4 GHz Mesh Topology</span>
        </span>
        <span className="text-slate-300">|</span>
        <span className="text-slate-500">Drag any node to simulate movement</span>
      </div>

      <svg
        className="w-full h-full cursor-crosshair"
        viewBox="0 0 700 480"
        onPointerMove={handlePointerMove}
        onPointerUp={handlePointerUp}
      >
        <defs>
          <radialGradient id="nodeRangeGlow" cx="50%" cy="50%" r="50%">
            <stop offset="0%" stopColor="#3b82f6" stopOpacity="0.08" />
            <stop offset="100%" stopColor="#3b82f6" stopOpacity="0.0" />
          </radialGradient>
        </defs>

        {/* 1. Wireless Radio Range Circles */}
        {nodes.map((node) => {
          const isSelected = selectedNodeId === node.id;
          const rangePx = node.range * 1.55;
          return (
            <g key={`range-${node.id}`} className="pointer-events-none">
              <circle
                cx={node.pos.x}
                cy={node.pos.y}
                r={rangePx}
                fill={isSelected ? 'url(#nodeRangeGlow)' : 'none'}
                stroke={isSelected ? '#3b82f6' : '#cbd5e1'}
                strokeWidth={isSelected ? 1.5 : 0.8}
                strokeDasharray={isSelected ? '4 3' : '2 4'}
                opacity={isSelected ? 0.9 : 0.45}
              />
            </g>
          );
        })}

        {/* 2. Active Neighbor Physical Links */}
        {nodes.map((node) =>
          node.activeNeighbors.map((nbrId) => {
            if (node.id > nbrId) return null;
            const nbr = nodes.find((n) => n.id === nbrId);
            if (!nbr) return null;

            const isDead = node.batteryJoules <= 1 || nbr.batteryJoules <= 1;
            const isUntrusted = node.trustScore < 0.4 || nbr.trustScore < 0.4;

            let strokeColor = '#3b82f6';
            if (isDead) strokeColor = '#94a3b8';
            else if (isUntrusted) strokeColor = '#ef4444';

            return (
              <line
                key={`link-${node.id}-${nbr.id}`}
                x1={node.pos.x}
                y1={node.pos.y}
                x2={nbr.pos.x}
                y2={nbr.pos.y}
                stroke={strokeColor}
                strokeWidth={1.8}
                strokeOpacity={isDead ? 0.25 : 0.65}
                strokeDasharray={isUntrusted ? '4 3' : undefined}
              />
            );
          })
        )}

        {/* 3. Packets In Transit */}
        {packetsInTransit.map((pkt) => {
          const fromNode = nodes.find((n) => n.id === pkt.fromNodeId);
          const toNode = nodes.find((n) => n.id === pkt.toNodeId);
          if (!fromNode || !toNode) return null;

          const curX = fromNode.pos.x + (toNode.pos.x - fromNode.pos.x) * pkt.progress;
          const curY = fromNode.pos.y + (toNode.pos.y - fromNode.pos.y) * pkt.progress;

          const isCritical = pkt.packet.priority === 'CRITICAL';
          const isCorrupt = pkt.status === 'corrupted';

          return (
            <g key={pkt.id} className="pointer-events-none">
              <circle
                cx={curX}
                cy={curY}
                r={isCritical ? 7 : 5}
                fill={isCorrupt ? '#ef4444' : isCritical ? '#e11d48' : '#2563eb'}
                className="animate-pulse"
              />
              <circle
                cx={curX}
                cy={curY}
                r={isCritical ? 13 : 9}
                fill="none"
                stroke={isCorrupt ? '#ef4444' : isCritical ? '#e11d48' : '#2563eb'}
                strokeWidth={1.5}
                opacity={0.4}
              />
            </g>
          );
        })}

        {/* 4. Nodes */}
        {nodes.map((node) => {
          const isSelected = selectedNodeId === node.id;
          const isDead = node.batteryJoules <= 1;
          const isUntrusted = node.trustScore < 0.4;
          const nodeFill = getNodeFill(node);

          return (
            <g
              key={`node-${node.id}`}
              transform={`translate(${node.pos.x}, ${node.pos.y})`}
              className="cursor-pointer"
              onPointerDown={(e) => handlePointerDown(node.id, e)}
            >
              {/* Outer selection ring */}
              {isSelected && (
                <circle
                  r={22}
                  fill="none"
                  stroke="#2563eb"
                  strokeWidth={2}
                  strokeDasharray="4 2"
                />
              )}

              {/* Node bubble */}
              <circle
                r={16}
                fill="#ffffff"
                stroke={nodeFill}
                strokeWidth={3}
                className="transition-all duration-150 shadow-sm"
              />

              {/* Node ID */}
              <text
                textAnchor="middle"
                dy={5}
                fill="#0f172a"
                fontSize="12"
                fontWeight="700"
                className="pointer-events-none font-sans"
              >
                {node.id}
              </text>

              {/* Node Name pill */}
              <g transform="translate(0, 24)" className="pointer-events-none">
                <rect
                  x={-46}
                  y={-1}
                  width={92}
                  height={15}
                  rx={4}
                  fill="#ffffff"
                  stroke="#cbd5e1"
                  strokeWidth={0.8}
                />
                <text
                  textAnchor="middle"
                  dy={10}
                  fill="#334155"
                  fontSize="9.5"
                  fontWeight="600"
                >
                  {node.name}
                </text>
              </g>

              {/* Battery gauge bar */}
              <g transform="translate(-14, -21)" className="pointer-events-none">
                <rect width={28} height={4} rx={2} fill="#e2e8f0" stroke="#cbd5e1" strokeWidth={0.5} />
                <rect
                  width={Math.max(0, 28 * (node.batteryJoules / node.maxBatteryJoules))}
                  height={4}
                  rx={2}
                  fill={node.batteryJoules / node.maxBatteryJoules < 0.25 ? '#ef4444' : '#10b981'}
                />
              </g>

              {/* Untrusted warning badge */}
              {isUntrusted && !isDead && (
                <g transform="translate(10, -16)" className="pointer-events-none">
                  <circle r={6} fill="#ef4444" />
                  <text textAnchor="middle" dy={3} fill="#ffffff" fontSize="8" fontWeight="bold">!</text>
                </g>
              )}
            </g>
          );
        })}
      </svg>
    </div>
  );
};
