// components/gates/gateGeometry.ts

/* ------------------------------------------------ */
/* shared SVG view frame                            */
/* ------------------------------------------------ */

// Standard top-level SVG frame used by gate demo components.
export const VIEW_W = 450
export const VIEW_H = 220

// Default placement of the canonical gate frame inside the SVG view.
// Primitive gates can usually use this directly.
// Composite gates may override it locally if needed.
export const VIEW_GATE_OFFSET_X = 80
export const VIEW_GATE_OFFSET_Y = 46

/* ------------------------------------------------ */
/* canonical gate frame                             */
/* ------------------------------------------------ */

// Standard local coordinate system used by primitive gate bodies.
export const GATE_W = 260
export const GATE_H = 128

export const MID_X = GATE_W / 2
export const MID_Y = GATE_H / 2

/* ------------------------------------------------ */
/* shared wire geometry                             */
/* ------------------------------------------------ */

// Length of the external input/output wire stubs.
export const WIRE_LEN = 75

// Input wire span.
export const IN_X0 = 0
export const IN_X1 = WIRE_LEN

// Output wire span.
export const OUT_X1 = GATE_W
export const OUT_X0 = GATE_W - WIRE_LEN

// Binary input lanes.
export const IN_TOP_Y = 26
export const IN_BOTTOM_Y = GATE_H - IN_TOP_Y

// Unary input lane.
export const IN_SINGLE_Y = MID_Y

/* ------------------------------------------------ */
/* shared body anchors                              */
/* ------------------------------------------------ */

// Small inset between wire endpoints and the visible start of the body.
// This keeps the body edge from sitting exactly on the input wire endpoint.
export const BODY_INSET_X = 1
export const BODY_X = IN_X1 + BODY_INSET_X

/* ------------------------------------------------ */
/* AND / NAND family geometry                       */
/* ------------------------------------------------ */

// Right-side vertical anchor for the AND-family body before the semicircle.
export const AND_BODY_RIGHT_X = 148

// Right-side wire offset for vertical arch
export const AND_OUT_X0 = OUT_X0 + 30

// Semicircle radius used by AND / NAND.
export const AND_BODY_RADIUS = GATE_H / 2

/* ------------------------------------------------ */
/* NAND bubble geometry                             */
/* ------------------------------------------------ */

// Output inversion bubble for NAND.
export const NAND_BUBBLE_R = 10

// Bubble sits at the effective output of the AND body.
export const NAND_BUBBLE_CX = AND_BODY_RIGHT_X + AND_BODY_RADIUS
export const NAND_BUBBLE_CY = MID_Y

/* ------------------------------------------------ */
/* NOT gate geometry                                */
/* ------------------------------------------------ */

// NOT triangle vertical span.
export const NOT_TOP_Y = 14
export const NOT_BOTTOM_Y = GATE_H - NOT_TOP_Y

// Triangle tip before the inversion bubble.
export const NOT_TIP_X = 166

// NOT inversion bubble.
export const NOT_BUBBLE_R = 9
export const NOT_BUBBLE_CX = NOT_TIP_X + NOT_BUBBLE_R - 2
export const NOT_BUBBLE_CY = MID_Y

// output wire start for NOT gate (just after bubble)
export const NOT_OUT_X0 = NOT_BUBBLE_CX + NOT_BUBBLE_R

/* ------------------------------------------------ */
/* OR family geometry (OR / XOR)                    */
/* ------------------------------------------------ */

// OR-family input wires terminate slightly deeper because of the concave back.
export const OR_INPUT_BIAS_X = 10
export const OR_IN_X1 = IN_X1 + OR_INPUT_BIAS_X

// Concave back-curve control point.
export const OR_BACK_CTRL_X = 116

// Forward lobe control point.
export const OR_LOBE_CTRL_X = 212

// Forward tip of the OR silhouette.
export const OR_TIP_X = 248

// OR-family output wire begins farther right because of the pointed nose.
export const OR_OUT_X0 = OUT_X0 + 63

// Top / bottom lobe spread.
export const OR_TOP_CTRL_Y = - 10
export const OR_BOTTOM_CTRL_Y = GATE_H + 10

/* ------------------------------------------------ */
/* XOR-specific geometry                            */
/* ------------------------------------------------ */

// Offset for the additional leading XOR curve.
export const XOR_BACK_OFFSET_X = 14

/* ------------------------------------------------ */
/* MUX / DEMUX family geometry                      */
/* ------------------------------------------------ */

// shared trapezoid body
export const MUX_BODY_TOP_INSET_X = 34
export const MUX_BODY_BOTTOM_INSET_X = 34
export const MUX_BODY_RIGHT_X = 200

// select line enters from bottom center
export const MUX_SELECT_X = 143.5
export const MUX_SELECT_Y1 = (-WIRE_LEN / 2) + 5
export const MUX_SELECT_Y2 = 0

// left/right wire anchors
export const MUX_LEFT_X0 = IN_X0
export const MUX_LEFT_X1 = BODY_X

export const MUX_RIGHT_X0 = MUX_BODY_RIGHT_X
export const MUX_RIGHT_X1 = OUT_X1

// center input/output lane
export const MUX_MID_IO_Y = MID_Y

/* ------------------------------------------------ */
/* top-level label positions (view space)           */
/* ------------------------------------------------ */

// Input / output label anchors for top-level gate demo SVGs.
export const LABEL_IN_X = 10
export const LABEL_OUT_X = VIEW_W - 20

export const LABEL_A_Y = VIEW_GATE_OFFSET_Y + IN_TOP_Y
export const LABEL_B_Y = VIEW_GATE_OFFSET_Y + IN_BOTTOM_Y
export const LABEL_OUT_Y = VIEW_GATE_OFFSET_Y + MID_Y