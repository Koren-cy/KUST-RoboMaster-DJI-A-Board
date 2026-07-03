# Design It Twice

When exploring alternative interfaces for a deepening candidate, use parallel sub-agent designs. Based on "Design It Twice" (Ousterhout) — your first idea is unlikely to be the best.

Uses the vocabulary in [SKILL.md](SKILL.md) — **module**, **interface**, **seam**, **adapter**, **leverage**.

## Process

### 1. Frame the problem space

Write a user-facing explanation:

- The constraints any new interface would need to satisfy
- The dependencies it would rely on (see [DEEPENING.md](DEEPENING.md))
- A rough illustrative code sketch to ground the constraints

### 2. Spawn sub-agents (3+)

Each sub-agent gets a different design constraint:

- Agent 1: "Minimize the interface — aim for 1–3 entry points max."
- Agent 2: "Maximise flexibility — support many use cases."
- Agent 3: "Optimise for the most common caller — make the default case trivial."

Each sub-agent outputs:
1. Interface (types, methods, params — plus invariants, ordering, error modes)
2. Usage example
3. What the implementation hides behind the seam
4. Dependency strategy
5. Trade-offs

### 3. Present and compare

Contrast by **depth** (leverage at the interface), **locality** (where change concentrates), and **seam placement**. Give your recommendation. Be opinionated — the user wants a strong read, not a menu.
