# Codexion

A dining-philosophers-style concurrency simulation. `N` coders sit around a
table sharing `N` USB dongles. To compile, a coder needs **two** dongles (the
one on each side). After compiling, a coder debugs, then refactors, then tries
to compile again. If a coder goes too long without compiling, they **burn
out** and the simulation stops.

## Build

```sh
cd coders
make
```

This produces the `codexion` binary.

## Usage

```sh
./codexion coders_num burnout_time compiling_time debugging_time \
           refactoring_time required_compile dongle_cooldown scheduler
```

| Argument           | Meaning                                                       |
| ------------------ | ------------------------------------------------------------- |
| `coders_num`       | Number of coders (and dongles)                                |
| `burnout_time`     | Max time (ms) a coder can go without compiling before burnout |
| `compiling_time`   | Time (ms) spent compiling (holding two dongles)               |
| `debugging_time`   | Time (ms) spent debugging                                     |
| `refactoring_time` | Time (ms) spent refactoring                                   |
| `required_compile` | Compilations each coder must finish for the run to end        |
| `dongle_cooldown`  | Minimum idle time (ms) before a released dongle can be reused |
| `scheduler`        | Dongle queue policy: `fifo` or `edf`                          |

Example:

```sh
./codexion 5 800 100 100 50 3 0 fifo
```

Output is one event per line: `timestamp_ms coder_id message`, where the
message is one of `has taken a dongle`, `is compiling`, `is debugging`,
`is refactoring`, or `burned out`.

## Design

- **One thread per coder** plus a **monitor thread**. All threads wait on a
  condition variable and start together once every thread is created.
- **Odd-numbered coders start first**: even-numbered coders sleep briefly at
  startup, which staggers dongle requests and avoids the initial stampede.
- Coders also alternate grab order (odd: left then right, even: right then
  left) to prevent circular-wait deadlock.
- Each dongle is protected by a mutex and holds a **2-slot priority queue** of
  requests (only its two neighbours can ever queue for it):
  - `fifo` — requests are served in arrival order;
  - `edf` — earliest deadline first, where a coder's deadline is
    `last_compilation + burnout_time` (the hungriest coder wins).
- The monitor wakes every millisecond, declares a burnout if a coder's last
  compilation is older than `burnout_time`, and stops the simulation once
  every coder has reached `required_compile` compilations.
