// components/gates/gateGeometry.ts

/* ------------------------------------------------ */
/* canonical gate frame */
/* ------------------------------------------------ */

export const GATE_W = 260
export const GATE_H = 128

export const MID_X = GATE_W / 2
export const MID_Y = GATE_H / 2

/* ------------------------------------------------ */
/* shared wire geometry */
/* ------------------------------------------------ */

// amount each external wire extends beyond the gate body
export const WIRE_LEN = 75

// input wire span
export const IN_X0 = 0
export const IN_X1 = WIRE_LEN

// output wire span
export const OUT_X1 = GATE_W
export const OUT_X0 = GATE_W - WIRE_LEN

// binary gate input lanes
export const IN_TOP_Y = 26
export const IN_BOTTOM_Y = GATE_H - IN_TOP_Y

// unary gate input lane
export const IN_SINGLE_Y = MID_Y

/* ------------------------------------------------ */
/* shared body geometry */
/* ------------------------------------------------ */

// x-position where the visible gate body begins
export const BODY_X = IN_X1 + 1

// right-side anchor used by AND / NAND family bodies
export const BODY_RIGHT_X = 148

// semicircle radius for AND / NAND right arc
export const BODY_RADIUS = GATE_H / 2

/* ------------------------------------------------ */
/* NAND bubble geometry */
/* ------------------------------------------------ */

// inversion bubble placed slightly into the output neck
export const NAND_BUBBLE_R = 10
export const NAND_BUBBLE_CX = BODY_RIGHT_X + BODY_RADIUS
export const NAND_BUBBLE_CY = MID_Y

/* ------------------------------------------------ */
/* NOT gate geometry */
/* ------------------------------------------------ */

// triangle vertical span inside the canonical frame
export const NOT_TOP_Y = 14
export const NOT_BOTTOM_Y = GATE_H - NOT_TOP_Y

// triangle tip before inversion bubble
export const NOT_TIP_X = 166

// NOT inversion bubble
export const NOT_BUBBLE_R = 9
export const NOT_BUBBLE_CX = NOT_TIP_X + NOT_BUBBLE_R - 2
export const NOT_BUBBLE_CY = MID_Y

/* ------------------------------------------------ */
/* OR gate geometry */
/* ------------------------------------------------ */

// concave back-curve control point
export const OR_BACK_CTRL_X = 116

// concave back-curve wire bias
export const OR_IN_X = 10

// forward lobe control point
export const OR_LOBE_CTRL_X = 212

// forward tip of the OR silhouette
export const OR_TIP_X = 248

// top / bottom lobe spread
export const OR_TOP_CTRL_Y = 0
export const OR_BOTTOM_CTRL_Y = GATE_H