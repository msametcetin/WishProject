#  The WISH Shell Project

> **WISH** (WIsh SHe**ll**) is a simple command-line interpreter (shell) implementation developed as a project for an Operating Systems course. It handles basic command execution, built-in commands, and I/O redirection.

##  Overview

The WISH Shell is designed to emulate the core functionalities of a standard Unix shell. It reads user commands from standard input or a batch file, parses them, and executes them by creating child processes. The main goal of this project is to demonstrate an understanding of process management, system calls (`fork()`, `execvp()`, `waitpid()`), and I/O handling in C.

##  Features

The current implementation of the WISH Shell includes the following features:

* **Command Execution:** Supports running external programs found within the defined `path`.
* **Batch Mode:** Reads and executes commands from a specified input file (script) line by line.
* **Interactive Mode:** Runs commands interactively, reading input from the user.
* **Input/Output Redirection:** Supports redirecting standard output (`>`) to a specified file.
* **Built-in Commands:**
    * `exit`: Terminates the shell process.
    * `cd [path]`: Changes the current working directory. If no path is provided, behavior is system-dependent (or defined by project specifications).
    * `path [dir1 dir2 ...]`: Sets the execution path for external commands. Only commands found in the specified directories are executable.

##  Technologies Used

| Technology | Purpose |
| :--- | :--- | :--- |
| **C Language** | The core logic and system call implementation (using `wish.c`). |
| **Unix/Linux System Calls** | Process management (`fork`, `execvp`, `waitpid`) and file operations. |
| **Shell Scripting** | Used primarily for testing and automation (`tester` directory). |

##  Getting Started

These instructions will guide you through compiling and running the WISH shell on a Unix-like system (Linux, macOS, or WSL).

### Prerequisites

You need a C compiler, such as **GCC**, to compile the source code.

```bash
# Check if GCC is installed
gcc --version

Installation and Execution
Clone the repository (if not already done):

Bash

git clone [https://github.com/msametcetin/WishProject.git](https://github.com/msametcetin/WishProject.git)
cd WishProject
Compile the source code: Use gcc to compile the wish.c file and create an executable named wish.

Bash

gcc -o wish wish.c
Run in Interactive Mode: Simply run the generated executable:

Bash

./wish
(The shell will prompt for commands.)

Run in Batch Mode (Script Execution): Provide a text file containing a list of commands as an argument:

Bash

./wish <your_script_file>.txt
 Testing
The project includes a tester folder, likely containing scripts or test cases to verify the shell's functionality against project requirements.

 License
This project is open-source. Please specify the license type here.
