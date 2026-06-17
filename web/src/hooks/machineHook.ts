import { useEffect, useRef, useState } from "react";
import { MachineState, UseMachineProps } from "../types";
import MachineClient from "../runtime/machineClient.js";

export default function useMachine({ onFramebuffer }: UseMachineProps) {
  const machineRef = useRef<MachineClient | null>(null);

  const [runtimeStatus, setRuntimeStatus] = useState<string>(
    "Initializing Hack++ runtime...",
  );
  const [result, setResult] = useState<string>("");

  const [machineState, setMachineState] = useState<MachineState>({
    pc: 0,
    flags: 0,
    cycles: 0,
    programId: null,
  });

  useEffect(() => {
    let cancelled = false;

    async function init() {
      try {
        const machine = new MachineClient();
        await machine.init();

        if (cancelled) return;

        machineRef.current = machine;
        setRuntimeStatus("Hack++ runtime ready.");

        const initialState = await machine.getState();
        if (!cancelled) {
          setMachineState((prev) => ({
            ...prev,
            ...initialState,
          }));
        }
      } catch (error) {
        if (cancelled) return;
        setRuntimeStatus("Runtime initialization failed.");
        setResult(error instanceof Error ? error.message : "Unknown error.");
      }
    }

    init();

    return () => {
      cancelled = true;
    };
  }, []);

  function getReadyMachine(): MachineClient | null {
    const machine = machineRef.current;

    if (!machine || !machine.isReady()) {
      setResult("Machine not ready.");
      return null;
    }

    return machine;
  }

  async function refreshScreen() {
    try {
      const machine = getReadyMachine();
      if (!machine) return;

      const buffer = await machine.getFramebuffer();
      onFramebuffer?.(buffer);
      setResult("Screen updated.");
    } catch (error) {
      setResult(error instanceof Error ? error.message : "Unknown error.");
    }
  }

  async function applyMachineUpdate(
    action: (machine: MachineClient) => Promise<any>,
    options: { programId?: string; refreshScreen?: boolean } = {},
  ) {
    try {
      const machine = getReadyMachine();
      if (!machine) return;

      const nextState = await action(machine);

      setMachineState((prev) => ({
        ...prev,
        ...nextState,
        ...(options.programId !== undefined
          ? { programId: options.programId }
          : {}),
      }));

      if (options.refreshScreen) {
        await refreshScreen();
      }
    } catch (error) {
      setResult(error instanceof Error ? error.message : "Unknown error.");
    }
  }

  async function load(program) {
    await applyMachineUpdate((machine) => machine.load(program), {
      programId: program.id,
      refreshScreen: true,
    });
  }

  async function run() {
    await applyMachineUpdate((machine) => machine.run());
  }

  async function stop() {
    await applyMachineUpdate((machine) => machine.stop());
  }

  async function step() {
    await applyMachineUpdate((machine) => machine.step(), {
      refreshScreen: true,
    });
  }

  async function reset() {
    await applyMachineUpdate((machine) => machine.reset(), {
      refreshScreen: true,
    });
  }

  async function setKeyboard(value) {
    await applyMachineUpdate((machine) => machine.setKeyboard(value), {
      refreshScreen: true,
    });
  }

  return {
    runtimeStatus,
    result,
    machineState,
    load,
    run,
    stop,
    step,
    reset,
    setKeyboard,
    refreshScreen,
  };
}
