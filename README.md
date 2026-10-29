*This project has been created as part of the 42 curriculum by astehl, edarmaev.*


# minishell 🐚

A lightweight Unix shell written in C, developed as part of the **42 School curriculum**.

`minishell` recreates the core behavior of a Unix shell, using Bash as a reference. The project explores command parsing, environment variable expansion, process execution, pipes, redirections, and signal handling.

The goal is to understand how a shell interprets user input and coordinates processes, files, and the operating system to execute commands.

## Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Built-in Commands](#built-in-commands)
- [Getting Started](#getting-started)
- [Usage](#usage)
- [Project Structure](#project-structure)
- [Technical Highlights](#technical-highlights)
- [42 Project Requirements](#42-project-requirements)
- [Acknowledgments](#acknowledgments)

## Overview

A shell is a command-line interface that allows users to interact with an operating system by executing commands, managing files, and combining programs.

This project implements a simplified shell in C, following the mandatory requirements of the 42 Minishell subject.

It provides an interactive prompt, interprets user input, expands environment variables, executes commands, and manages input/output redirections and pipelines.

The repository is organized into the main shell implementation, its supporting custom C library (`libft`), and optional debugging tools. The library was further extended and improved during Minishell development to support the project's requirements.

## Features

### Command Processing

- Interactive command prompt using GNU Readline.
- Command history.
- Command execution using `PATH`, relative paths, and absolute paths.
- Single-quote and double-quote handling.
- Environment variable expansion, including `$?` for the last foreground pipeline's exit status.

### Redirections and Pipelines

- Input redirection (`<`).
- Output redirection (`>`).
- Append-mode output redirection (`>>`).
- Here-documents (`<<`).
- Pipelines (`|`) connecting the output of one command to the input of the next.

### Environment and Signals

- Environment variable management.
- Handling of `Ctrl-C`, `Ctrl-D`, and `Ctrl-\`.
- Signal-aware interactive prompt behavior.

### Built-in Commands

The shell implements the following mandatory built-ins:

- `echo` — print text, with support for `-n`.
- `cd` — change the current working directory.
- `pwd` — display the current working directory.
- `export` — manage exported environment variables.
- `unset` — remove environment variables.
- `env` — display environment variables.
- `exit` — terminate the shell.

## Getting Started

### Prerequisites

Before building the project, make sure you have:

- A Unix-like development environment.
- A C compiler, such as `cc` or `clang`.
- GNU Readline development headers and library.
- `make`.
- The libraries and build tools required by the project.

The project uses **[libft](./libft)**, a custom C library originally developed during the first 42 Common Core project and extended with additional functions and improvements throughout Minishell development.

### Installation

Clone the repository:

```bash
git clone https://github.com/MoriaProfa/minishell.git
```

Navigate to the project directory:

```bash
cd minishell
```

Compile the project:

```bash
make
```

This builds the `minishell` executable and compiles the required `libft` library.

### Run

Start the shell:

```bash
./minishell
```

An interactive prompt will appear, allowing you to enter commands.

### Makefile Commands

| Command | Description |
|---|---|
| `make` | Compile the project. |
| `make all` | Build the executable. |
| `make clean` | Remove project object files and clean `libft`. |
| `make fclean` | Remove object files, the executable, and compiled library artifacts. |
| `make re` | Perform a clean rebuild. |

## Usage

Once the shell is running, you can enter commands interactively.

### Basic Commands

```bash
minishell$ echo "Hello, world!"
Hello, world!

minishell$ pwd
/path/to/current/directory

minishell$ ls -la
```

### Environment Variables

```bash
minishell$ export USER_NAME=42
minishell$ echo $USER_NAME
42
```

Display the exit status of the last foreground pipeline:

```bash
minishell$ echo $?
0
```

### Pipes

Pass the output of one command to another:

```bash
minishell$ ls -la | grep minishell
```

### Redirections

Read input from a file:

```bash
minishell$ cat < input.txt
```

Write output to a file:

```bash
minishell$ echo "Hello, file!" > output.txt
```

Append output to a file:

```bash
minishell$ echo "Another line" >> output.txt
```

### Here-Documents

Read input until the specified delimiter is entered:

```bash
minishell$ cat << EOF
Hello from a here-document!
EOF
```

### Directory Navigation

```bash
minishell$ cd ..
minishell$ pwd
```

These examples illustrate the intended interface and required shell behavior. Individual cases should be tested against the implementation.

## Project Structure

The repository is organized into the main shell implementation, its supporting utility library, and optional debugging tools.

```text
minishell/
├── Makefile
├── minishell.c
├── minishell.h
├── libft/
├── debug/
│   └── minishell.supp
└── src/
    ├── builtins/
    ├── command_table/
    ├── environment/
    ├── execution/
    ├── heredoc/
    ├── msh_utils/
    ├── redirection/
    └── tokenizer/
```

### Main Components

| Component | Responsibility |
|---|---|
| `minishell.c` | Main program entry point. |
| `src/builtins/` | Built-in command implementations and related utilities. |
| `src/command_table/` | Command parsing and command data management. |
| `src/environment/` | Environment variable management. |
| `src/execution/` | Command execution and execution utilities. |
| `src/heredoc/` | Here-document input collection and redirection. |
| `src/msh_utils/` | General shell utilities and signal handling. |
| `src/redirection/` | File redirection handling. |
| `src/tokenizer/` | Input tokenization, expansion, and token processing. |
| `libft/` | Custom C library originally developed during the first 42 Common Core project and extended with additional functions and improvements throughout Minishell development. |
| `debug/` | Optional utilities used during development and memory analysis. |

## Technical Highlights

### Process Management

External commands are executed using Unix process-management functions, including `fork()`, `execve()`, and `waitpid()`.

### File Descriptors

Redirections and pipelines rely on file descriptor operations such as `open()`, `close()`, `dup()`, and `dup2()`.

### Parsing and Expansion

The shell processes user input, recognizes quoting and shell operators, and expands environment variables according to the required quoting rules.

### Signal Handling

Interactive signal handling must preserve the shell prompt's expected behavior without allowing the signal handler to access the shell's main data structures.

### Debugging Tools

The `debug/` directory contains optional tools used to inspect the shell's internal behavior and analyze memory issues during development.

- `print_helpers.c` — helper functions for debugging internal data structures and inspecting parsing or execution results.
- `minishell.supp` — Valgrind suppression rules used to filter selected reports during memory leak analysis.

These files are intended for development and debugging purposes and are not part of the standard build.

### Memory Debugging with Valgrind

To check for memory leaks and file descriptor issues, run:

```bash
valgrind --leak-check=full \
         --show-leak-kinds=all \
         --track-origins=yes \
         --track-fds=all \
         --trace-children=yes \
         --suppressions=./debug/minishell.supp \
         ./minishell
```
Run it from the repository root, where the `Makefile` and `minishell` executable are located.

## 42 Project Requirements

This project follows the mandatory scope of the Minishell assignment.

The implementation is restricted to the required shell functionality and the authorized external functions specified by the subject. `libft` is authorized.

The Makefile provides the required targets:

- `NAME`
- `all`
- `clean`
- `fclean`
- `re`

The project focuses on implementing the fundamental behavior of a shell rather than reproducing every feature of Bash.

## Acknowledgments

- **42 School** — for the Minishell project and its learning objectives.
- **libft** — our own C library developed during the first 42 Common Core project and improved through this project.
- **GNU Readline** — for interactive input and command history.
- **Bash** — as a behavioral reference for shell functionality.
