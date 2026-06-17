import { MachineStatusProps } from "../../types";

import "./MachineStatus.css";

export default function MachineStatus({
  state,
  programName,
  pc,
  cycles,
}: MachineStatusProps) {
  return (
    <section className="control-card">
      <h2 className="control-card-title">Machine Status</h2>

      <div className="machine-status-grid">
        <div className="machine-status-row">
          <span className="machine-status-label">State</span>
          <span className="machine-status-value">{state}</span>
        </div>

        <div className="machine-status-row">
          <span className="machine-status-label">Program</span>
          <span className="machine-status-value">{programName}</span>
        </div>

        <div className="machine-status-row">
          <span className="machine-status-label">PC</span>
          <span className="machine-status-value">{`0x${pc.toString(16).toUpperCase().padStart(4, "0")}`}</span>
        </div>

        <div className="machine-status-row">
          <span className="machine-status-label">Cycles</span>
          <span className="machine-status-value">
            {cycles.toLocaleString()}
          </span>
        </div>
      </div>
    </section>
  );
}
