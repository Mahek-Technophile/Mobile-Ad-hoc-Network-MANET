import React from 'react';
import { TopicExplanation } from '../utils/topicExplanations';
import { BookOpen, X, Sparkles, CheckCircle2, ChevronRight } from 'lucide-react';

interface TopicOverviewCardProps {
  topic: TopicExplanation | null;
  onClose: () => void;
}

export const TopicOverviewCard: React.FC<TopicOverviewCardProps> = ({ topic, onClose }) => {
  if (!topic) return null;

  return (
    <div className="bg-white border border-blue-200/90 rounded-xl p-4 shadow-xl shadow-blue-500/5 relative overflow-hidden transition-all duration-200">
      {/* Accent top gradient stripe */}
      <div className="absolute top-0 left-0 right-0 h-1 bg-gradient-to-r from-blue-500 via-sky-400 to-indigo-500" />

      <div className="flex items-start justify-between gap-3 mb-2">
        <div className="flex items-center space-x-2">
          <div className="w-7 h-7 rounded-lg bg-blue-50 flex items-center justify-center text-blue-600 border border-blue-100">
            <BookOpen className="w-4 h-4" />
          </div>
          <div>
            <span className="text-[10px] uppercase font-bold tracking-wider text-blue-700 bg-blue-50 px-2 py-0.5 rounded-full border border-blue-100">
              {topic.unit}
            </span>
            <h3 className="text-sm font-bold text-slate-800 mt-0.5">{topic.title}</h3>
          </div>
        </div>

        <button
          onClick={onClose}
          className="text-slate-400 hover:text-slate-600 p-1 rounded-md hover:bg-slate-100 transition"
          aria-label="Dismiss topic overview"
        >
          <X className="w-4 h-4" />
        </button>
      </div>

      <div className="grid grid-cols-1 md:grid-cols-2 gap-3 mt-3 text-xs bg-slate-50/80 p-3 rounded-lg border border-slate-200/70">
        <div>
          <span className="text-[11px] font-bold text-slate-700 block mb-0.5">
            What Just Happened:
          </span>
          <p className="text-slate-600 leading-relaxed text-[11.5px]">{topic.whatHappened}</p>
        </div>
        <div>
          <span className="text-[11px] font-bold text-blue-700 block mb-0.5">
            Why It Matters in Computer Networks:
          </span>
          <p className="text-slate-600 leading-relaxed text-[11.5px]">{topic.whyItMatters}</p>
        </div>
      </div>
    </div>
  );
};
