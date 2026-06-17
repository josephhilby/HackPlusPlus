export const WIDTH = 512;
export const HEIGHT = 256;
export const WORDS_PER_ROW = 32;

export type AssemblyRow =
  | { type: "label"; label: string }
  | { type: "instruction"; text: string; address: number };

export interface AssemblyPanelProps {
  title?: string;
  instructions?: string[];
  currentPc?: number | null;
  onStep: () => void;
  onClose: () => void;
  canStep?: boolean;
}

export interface MachineState {
  pc: number;
  flags: number;
  cycles: number;
  programId: string | null;
}

export interface Program {
  id: string;
  asm: string;
  bin: string;
}

export interface UseMachineProps {
  onFramebuffer?: (buffer: Uint16Array) => void;
}

export interface WasmModule {
  HEAPU8: Uint8Array;
  _init(): void;
  _get_rom_ptr(): number;
  _commit_rom(length: number): void;
  _run(): void;
  _stop(): void;
  _step(): void;
  _reset(): void;
  _set_keyboard(value: number): void;
  _get_state_ptr(): number;
  _get_framebuffer_ptr(): number;
}

export interface MachineControlsProps {
  machineState: string;
  onRun: () => void;
  onStop: () => void;
  onStep: () => void;
  onReset: () => void;
  canStep: boolean;
  canReset: boolean;
  canToggleRun: boolean;
}

export interface MachineStatusProps {
  state: string;
  programName: string;
  pc: number;
  cycles: number;
}

export interface ProgramLoaderProps {
  programs: Program[];
  selectedProgramId: string;
  onProgramChange: (id: string) => void;
  onLoad: () => void;
  onView: () => void;
  canView: boolean;
  isLoading?: boolean;
}
