# AVR Embedded Task Scheduler

A lightweight cooperative task scheduler implemented in C for an AVR ATmega32U4 microcontroller.

The project was originally developed as part of the **Software for Embedded Systems** course at Hamburg University of Technology (TUHH) and has been cleaned and documented as a standalone portfolio project.

## Overview

The scheduler uses a single hardware timer with a **1 ms time base** to manage multiple software tasks.

Tasks are executed from the main application loop rather than directly inside the interrupt service routine, keeping interrupt execution short and predictable.

The scheduler supports:

- Periodic tasks
- One-shot tasks
- Function-pointer-based task execution
- Task parameters
- Static task allocation
- Singly linked task management
- Interrupt-safe scheduler operations
- Millisecond-resolution timing

## Architecture

The system follows a simple cooperative scheduling model:

```text
             Timer0
            1 ms tick
                |
                v
       +------------------+
       | Scheduler Update |
       |      ISR         |
       +------------------+
                |
                | marks expired tasks
                v
       +------------------+
       |   Task List      |
       | linked list      |
       +------------------+
                |
                v
       +------------------+
       | scheduler_run()  |
       +------------------+
                |
                v
          Execute Task
        outside the ISR

