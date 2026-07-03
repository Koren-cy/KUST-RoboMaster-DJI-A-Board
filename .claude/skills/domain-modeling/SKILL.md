---
name: domain-modeling
description: Build and sharpen a project's domain model. Use when the user wants to pin down domain terminology or a ubiquitous language, record an architectural decision, or when another skill needs to maintain the domain model.
---

# Domain Modeling

Actively build and sharpen the project's domain model as you design. This is the *active* discipline — challenging terms, inventing edge-case scenarios, and writing the glossary and decisions down the moment they crystallise. (Merely *reading* `CONTEXT.md` for vocabulary is not this skill — that's a one-line habit any skill can do. This skill is for when you're changing the model, not just consuming it.)

## File structure

Most repos have a single context:

```
/
├── CONTEXT.md
├── docs/
│   └── adr/
│       ├── 0001-event-sourced-orders.md
│       └── 0002-postgres-for-write-model.md
└── src/
```

If a `CONTEXT-MAP.md` exists at the root, the repo has multiple contexts.

Create files lazily — only when you have something to write. If no `CONTEXT.md` exists, create one when the first term is resolved. If no `docs/adr/` exists, create it when the first ADR is needed.

## During the session

### Challenge against the glossary

When the user uses a term that conflicts with the existing language in `CONTEXT.md`, call it out immediately. "Your glossary defines 'chassis' as X, but you seem to mean Y — which is it?"

### Sharpen fuzzy language

When the user uses vague or overloaded terms, propose a precise canonical term. "You're saying 'motor' — do you mean the DJI 6020 gimbal motor or the GM3508 drive motor? Those are different types."

### Discuss concrete scenarios

When domain relationships are being discussed, stress-test them with specific scenarios. Invent scenarios that probe edge cases and force the user to be precise about the boundaries between concepts.

### Cross-reference with code

When the user states how something works, check whether the code agrees. If you find a contradiction, surface it: "Your code treats CAN1 as motor-only, but you just said gimbal commands also go through CAN1 — which is right?"

### Update CONTEXT.md inline

When a term is resolved, update `CONTEXT.md` right there. Don't batch these up — capture them as they happen.

`CONTEXT.md` should be totally devoid of implementation details. Do not treat `CONTEXT.md` as a spec, a scratch pad, or a repository for implementation decisions. It is a glossary and nothing else.

Example for this project:

```markdown
# CONTEXT.md — KUST RoboMaster A-Board

## Chassis
The four-wheel swerve drive base. Each wheel has independent steering (direction) and drive (speed) motors.

## Swerve Module
One corner of the chassis: a drive motor (GM3508) + steering motor (6020) + encoder.

## CAN Bus
- CAN1: chassis drive motors (GM3508 × 4)
- CAN2: gimbal motors (6020 × 2, 6623 × 1)
- Baud: 1 Mbps, standard frame ID

## DBUS
Remote control protocol over USART1. DJI DR16 receiver. 18 channels at 100k baud.

## PID Cascade
Outer loop (position) → Inner loop (velocity). Position PID feeds velocity PID as setpoint.

## GCODE (if applicable)
Serial command protocol for debugging. USART6 at 115200 baud.
```

### Offer ADRs sparingly

Only offer to create an ADR when all three are true:

1. **Hard to reverse** — the cost of changing your mind later is meaningful
2. **Surprising without context** — a future reader will wonder "why did they do it this way?"
3. **The result of a real trade-off** — there were genuine alternatives and you picked one for specific reasons

If any of the three is missing, skip the ADR.
