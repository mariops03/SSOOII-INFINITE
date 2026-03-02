# SSOOII-INFINITE

Concurrent programming project in C that demonstrates inter-process communication using POSIX signals. Built for the Operating Systems II course.

## What it does

The program creates a hierarchy of 6 processes (1 parent, 4 children, 2 grandchildren) that pass a SIGUSR1 signal in a ring pattern, simulating a relay race between processes. After 25 seconds, a SIGALRM fires and the parent prints an ASCII art infinity symbol displaying the total number of completed signal passes, then gracefully terminates all child processes.

## How it works

```
        Parent
       /  |   \   \
     H1  H2   H3  H4
         |     |
         N2    N3
```

1. **Parent** forks 4 children (H1, H2, H3, H4). H2 and H3 each fork a grandchild (N2, N3).
2. Parent sends `SIGUSR1` to H2, starting the relay.
3. Each process, upon receiving `SIGUSR1`, forwards it to the next process in the ring.
4. The signal alternates paths: Parent → H2 → N2 → H1 → Parent → H3 → N3 → H4 → Parent → ...
5. Parent increments a counter on each full pass.
6. After 25 seconds, `SIGALRM` triggers the finale: an infinity symbol (∞) rendered in ASCII art showing the pass count.
7. Parent sends `SIGTERM` to all children and waits for clean termination.

## Key concepts

- **`fork()`** — Process creation and hierarchy
- **`kill()` / `SIGUSR1`** — Inter-process signal passing
- **`sigaction()`** — Signal handler registration
- **`sigsuspend()`** — Atomic signal wait (avoids race conditions)
- **`sigprocmask()`** — Signal masking for controlled delivery
- **`alarm()` / `SIGALRM`** — Timed termination
- **`SIGTERM`** — Graceful child process shutdown
- **`waitpid()`** — Zombie process prevention

## Running it

```bash
gcc -o infinito infinito.c
./infinito
```

The program runs for 25 seconds, then prints the result and exits.

## Context

Built for the **Sistemas Operativos II** course (Computer Engineering degree, Universidad de Salamanca).