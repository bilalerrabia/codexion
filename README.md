
*This project has been created as part of the 42 curriculum by [berrabia].*

# Codexion

## Description

Codexion is a concurrent simulation inspired by the classic **Dining Philosophers** problem, reimagined in a software engineering context. Instead of philosophers eating, **coders** compete for shared **USB dongles** in order to compile their code.

Each coder needs **two dongles** simultaneously to compile. Dongles are shared resources — only one coder can hold a dongle at a time. A coder that goes too long without compiling **burns out** and ends the simulation.

The goal of the project is to implement a correct, deadlock-free, starvation-free concurrent simulation using POSIX threads, with support for two scheduling strategies: **FIFO** and **EDF** (Earliest Deadline First).

---

## Instructions

### Compilation

```bash
make
```

### Execution

```bash
./codexion [coders] [burnout_time] [compile_time] [debug_time] [refactor_time] [required_compilations] [dongle_cooldown] [scheduler]
```

### Parameters

| Parameter | Description |
|---|---|
| `coders` | Number of coder threads |
| `burnout_time` | Max time (ms) a coder can go without compiling |
| `compile_time` | Time (ms) to compile |
| `debug_time` | Time (ms) to debug |
| `refactor_time` | Time (ms) to refactor |
| `required_compilations` | Number of compilations each coder must complete |
| `dongle_cooldown` | Cooldown (ms) between dongle uses |
| `scheduler` | Scheduling strategy: `fifo` or `edf` |

### Example

```bash
./codexion 5 800 200 100 100 3 0 fifo
./codexion 5 800 200 100 100 3 50 edf
```

### Output format

```
[timestamp_ms] [coder_id] [state]
```

States: `has taken a dongle`, `is compiling`, `is debugging`, `is refactoring`, `burned out`.

---

## Blocking Cases Handled

### Deadlock Prevention

Deadlock requires four conditions simultaneously: mutual exclusion, hold-and-wait, and circular wait. **circular wait** by enforcing an **asymmetric dongle acquisition order**:

- Odd-indexed coders take `left_dongle` first, then `right_dongle`
- Even-indexed coders take `right_dongle` first, then `left_dongle`

This prevents the circular dependency that would cause a classic deadlock.

### Starvation Prevention

The **EDF scheduler** assigns each waiting coder a priority key based on their deadline (`last_compilation + burnout_time`). The coder closest to burning out is always served first. A **priority queue** (min-heap) inside each dongle ensures fair, deadline-aware ordering of waiting coders.

The **FIFO scheduler** uses arrival timestamp as the key, ensuring first-come-first-served ordering with no indefinite waiting.

### Dongle Cooldown Handling

Each dongle tracks `last_time_taken`. Before a coder can acquire a dongle, it checks that enough time has elapsed since the dongle was last released. If not, the coder uses `pthread_cond_timedwait` to sleep until the cooldown expires, then re-evaluates.

### Precise Burnout Detection

A dedicated **monitor thread** checks all coders every 1ms. For each coder it reads `last_compilation` and `compilation_number` . If `last_compilation + burnout_time <= current_time` and the coder has not met the required count, `sim_stop` is set to `1` and the simulation ends.

### Log Serialization

All output is serialized through a single `print_mutex`. The `log_state` function locks this mutex before printing.

---

## Thread Synchronization Mechanisms

### Primitives Used

| Primitive | Purpose |
|---|---|
| `pthread_mutex_t stop_mutex` | Protects `sim_stop` and the while-loop condition in `coder_func` |
| `pthread_mutex_t coder->mutex` | Protects `last_compilation` and `compilation_number` per coder |
| `pthread_mutex_t dongle->mutex` | Protects dongle state (`taken`, `last_time_taken`, priority queue) |
| `pthread_mutex_t print_mutex` | Serializes all printf output |
| `pthread_cond_t coder->wait_cond` | Coders wait on this for dongle availability |
| `pthread_cond_t global->cond` | Used as a start barrier — coders wait until all are ready |


### Monitor → Coder Communication

The monitor does **not** broadcast to coders. Instead it only sets `sim_stop = 1` under `stop_mutex` and returns. Coders use `pthread_cond_timedwait` with a 5ms timeout everywhere they wait, so they wake up periodically, check `sim_stop`, and exit cleanly:

```c
// request_dongle — self-waking wait
wakeup = get_time_ms() + 5;
ts.tv_sec  = wakeup / 1000;
ts.tv_nsec = (wakeup % 1000) * 1000000;
pthread_cond_timedwait(&coder->wait_cond, &dongle->mutex, &ts);
```

This avoids the mutex mismatch problem: the monitor would need to know which `dongle->mutex` each coder is currently waiting on, which is impossible to guarantee safely.

### Start Barrier

All coder threads are created before any of them begin executing. A `all_ready` flag and `global->cond` condition variable act as a barrier — coders wait until main broadcasts the start signal, ensuring a synchronized and fair start:

```c
// coder_func start
pthread_mutex_lock(&global->stop_mutex);
while (!global->all_ready)
    pthread_cond_wait(&global->cond, &global->stop_mutex);
pthread_mutex_unlock(&global->stop_mutex);
```

---

## Resources

### Documentation

### Articles
- [Dining Philosophers Problem — Wikipedia](https://en.wikipedia.org/wiki/Dining_philosophers_problem)
- [Coffman Conditions — Wikipedia](https://en.wikipedia.org/wiki/Deadlock#Necessary_conditions)
- [Earliest Deadline First Scheduling — Wikipedia](https://en.wikipedia.org/wiki/Earliest_deadline_first_scheduling)

### AI Usage

Claude was used throughout this project as a learning and debugging tool:

- **Learning**: Step-by-step exercises to build up from basic thread creation to mutexes, condition variables, and producer-consumer patterns before tackling the project itself.
- **Debugging**: Interpreting Helgrind/DRD output, identifying data races, inconsistent lock orders, and mutex mismatches across `coder_func`, `monitor_func`, `request_dongle`, and `release_dongle`.
- **Architecture decisions**: Discussing the tradeoffs between broadcasting from the monitor vs. using `timedwait` for self-waking coders, and the asymmetric dongle acquisition order for deadlock prevention.
