import React from 'react';
import { SimulationEventLog } from '../types';
import { Terminal, CheckCircle2, AlertCircle, Radio } from 'lucide-react';

interface EventLogsProps {
  logs: SimulationEventLog[];
  onClearLogs: () => void;
}

export const EventLogConsole: React.FC<EventLogsProps> = ({ logs, onClearLogs }) => {
  const getIcon = (level: SimulationEventLog['level']) => {
    switch (level) {
      case 'success': return <CheckCircle2 className="w-3.5 h-3.5 text-emerald-600 shrink-0" />;
      case 'warning': return <AlertCircle className="w-3.5 h-3.5 text-amber-600 shrink-0" />;
      case 'error': return <AlertCircle className="w-3.5 h-3.5 text-rose-600 shrink-0" />;
      default: return <Radio className="w-3.5 h-3.5 text-blue-600 shrink-0" />;
    }
  };

  return (
    <div className="bg-white border border-slate-200/90 rounded-xl p-4 shadow-sm flex flex-col h-60 text-xs font-mono">
      <div className="flex items-center justify-between border-b border-slate-100 pb-2 mb-2 font-sans">
        <div className="flex items-center space-x-2 text-slate-700">
          <Terminal className="w-4 h-4 text-blue-600" />
          <span className="font-bold text-xs uppercase tracking-wider">
            Protocol Journal & Log
          </span>
        </div>
        <button
          onClick={onClearLogs}
          className="text-[11px] text-slate-400 hover:text-slate-600 transition"
        >
          Clear
        </button>
      </div>

      <div className="flex-1 overflow-y-auto space-y-1 pr-1 text-[11px]">
        {logs.map((log) => (
          <div
            key={log.id}
            className="flex items-start space-x-2 p-1 rounded hover:bg-slate-50 transition"
          >
            {getIcon(log.level)}
            <span className="text-slate-400 shrink-0">
              [{new Date(log.timestamp).toLocaleTimeString()}]
            </span>
            <span className="px-1.5 py-0.2 rounded bg-slate-100 text-[9.5px] font-bold text-slate-600 shrink-0">
              {log.category}
            </span>
            <span className="text-slate-700 break-all leading-relaxed">{log.message}</span>
          </div>
        ))}
        {logs.length === 0 && (
          <div className="h-full flex items-center justify-center text-slate-400 italic font-sans text-xs">
            Ready for packet transmission...
          </div>
        )}
      </div>
    </div>
  );
};
