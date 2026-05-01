# Vaccine Management System

A C-based command-line application for managing vaccine batches, tracking stock, and registering patient vaccinations. Supports both **English** and **Portuguese** via a command-line argument.

---

## Project Structure

```
vaccine_project/
├── hash_table.c
├── main.c
├── vaccine_system.c
├── vaccine_system.h
└── README.md
```

| File | Description |
|------|-------------|
| `main.c` | Entry point of the program. Handles the command loop and user input parsing. |
| `vaccine_system.c` | Core logic for the system — adding batches, validating dates, sorting, and registering vaccinations. |
| `vaccine_system.h` | Header file containing all data structures (`Date`, `Batch`, `Vaccination`, `HashTable`) and function declarations. |
| `hash_table.c` | Custom hash table implementation used to efficiently look up users and their vaccination histories. |

---

## How to Compile

Make sure you have a C compiler installed (e.g. GCC). Open a terminal, navigate to the project folder, and run:

```bash
gcc -Wall -Wextra -o vaccine_manager main.c vaccine_system.c hash_table.c
```

This links all `.c` source files and produces an executable named `vaccine_manager`.

---

## How to Run

The program supports two language modes:

```bash
# Run in English (default)
./vaccine_manager

# Run in Portuguese
./vaccine_manager pt
```

---

## Available Commands

Once the program is running, it accepts the following commands:

| Command | Action |
|---------|--------|
| `c` | Create a new vaccine batch |
| `t` | Set the current system time |
| `l` | List vaccines (all batches or a specific one) |
| `a` | Add a new vaccination record for a user |
| `u` | List users and their vaccination records |
| `r` | Remove / deplete a specific vaccine batch |
| `d` | Delete vaccination records by user, date, or batch |
| `q` | Quit the program and free all memory safely |

---

## Requirements

- GCC or any C99-compatible compiler
- Unix-like environment (Linux, macOS) or Windows with MinGW/WSL
