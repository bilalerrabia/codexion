
*This project has been created as part of the 42 curriculum by berrabia.*

# Codexion

## Description

Codexion is a concurrent simulation inspired by the classic **Dining Philosophers** problem, reimagined in a software engineering context. Instead of philosophers eating, **coders** compete for shared **USB dongles** in order to compile their code.

Each coder needs **two dongles** simultaneously to compile. Dongles are shared resources — only one coder can hold a dongle at a time. A coder that goes too long without compiling **burns out** and ends the simulation.

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

---

## Blocking Cases Handled

### Deadlock Prevention

Deadlock requires four conditions simultaneously: mutual exclusion, hold-and-wait, and circular wait. **circular wait** by enforcing an **asymmetric dongle acquisition order**:

- Odd-indexed coders take `left_dongle` first, then `right_dongle`
- Even-indexed coders take `right_dongle` first, then `left_dongle`

This prevents the circular dependency that would cause a classic deadlock.


### Dongle Cooldown Handling

Each dongle tracks `last_time_taken`. Before a coder can acquire a dongle, it checks that enough time has elapsed since the dongle was last released. If not, the coder uses `pthread_cond_timedwait` to sleep until the cooldown expires, then re-evaluates.

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
wakeup = get_time_ms() + 5;
ts.tv_sec  = wakeup / 1000;
ts.tv_nsec = (wakeup % 1000) * 1000000;
pthread_cond_timedwait(&coder->wait_cond, &dongle->mutex, &ts);
```

This avoids the mutex mismatch problem: the monitor would need to know which `dongle->mutex` each coder is currently waiting on, which is impossible to guarantee safely.

### Start Barrier

All coder threads are created before any of them begin executing. A `all_ready` flag and `global->cond` condition variable act as a barrier — coders wait until main broadcasts the start signal, ensuring a synchronized and fair start:

```c
pthread_mutex_lock(&global->stop_mutex);
while (!global->all_ready)
    pthread_cond_wait(&global->cond, &global->stop_mutex);
pthread_mutex_unlock(&global->stop_mutex);
```

---

# Concurrency Concepts — Codexion

## Concurrency vs Parallelism
**Concurrency** runs multiple threads on a single core by rapidly switching between them. **Parallelism** runs threads truly simultaneously across multiple cores. Not every thread needs a dedicated core — when a thread blocks on I/O, its core switches to another thread instead of sitting idle.

## CPU Scheduling
**Non-preemptive:** a thread runs until it voluntarily yields (finishes or blocks on I/O).  
**Preemptive:** a hardware timer interrupts a thread when its time quantum expires, forcing a switch.

## Context Switch & Dispatcher
When the scheduler decides to switch threads, the **dispatcher** saves the current thread's state (program counter, registers, status) into its **TCB (Thread Control Block)** and restores the next thread's saved state. The time lost doing this is called **dispatch latency**.

## Race Conditions
Caused by preemption: a thread is interrupted mid-operation while another thread accesses the same shared data, leaving the program in an inconsistent state.

## Mutex
Protects shared data by ensuring only one thread enters a critical section at a time, relying on **atomic actions** — operations that complete in one indivisible step with no intermediate state visible to other threads.

## Condition Variables
Allow a thread to sleep and wait for a specific condition without busy-waiting. `pthread_cond_wait` atomically releases the mutex and sleeps; when signaled, it reacquires the mutex before returning. Always check the condition in a `while` loop to guard against spurious wakeups.

## Opaque Type
A type whose internal structure is hidden — only a pointer is exposed, not the definition. Enforces encapsulation. `FILE *` is the canonical example.

## System Limits
`ulimit -u` gives the max threads per user. `nproc` gives the number of available CPU cores.

## Resources

### Documentation

### Articles
- [Dining Philosophers Problem — Wikipedia](https://en.wikipedia.org/wiki/Dining_philosophers_problem)
- [Earliest Deadline First Scheduling — Wikipedia](https://en.wikipedia.org/wiki/Earliest_deadline_first_scheduling)
- https://www.cs.unibo.it/~ghini/didattica/sistop/pthreads_tutorial/POSIX_Threads_Programming.htm#Thread

### AI Usage

Claude was used throughout this project as a learning and debugging tool.