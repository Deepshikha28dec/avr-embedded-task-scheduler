# AVR Embedded Task Scheduler

Lightweight cooperative task scheduler implemented in C for an AVR microcontroller.

The scheduler uses a periodic timer interrupt as a system time base and executes one-shot and recurring tasks from the main application loop.

## Features

- Cooperative task scheduling
- Periodic and one-shot tasks
- Function-pointer based task execution
- Timer-driven time base
- Static task allocation
- Linked-list task management
- Embedded C implementation

## Architecture

```text
Hardware Timer
     ↓
System Tick
     ↓
Task Scheduler
     ↓
Ready Tasks
     ↓
Main Superloop
