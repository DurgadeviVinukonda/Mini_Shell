# 🖥️ Mini Shell in C (Linux)

A simple Linux command-line shell implemented in C that mimics basic behavior of Bash.  
This project demonstrates process management, signal handling, job control, and piping using system calls.

---

## 🚀 Features

- ✅ Execute external commands (e.g., `ls`, `pwd`, `sleep`)
- ✅ Built-in commands:
  - `cd`
  - `pwd`
  - `echo`
  - `exit`
  - `clear`
- ✅ Job control:
  - `jobs` → list all jobs
  - `fg` → bring job to foreground
  - `bg` → run job in background
- ✅ Signal handling:
  - `Ctrl + C` → terminate process
  - `Ctrl + Z` → stop process and move to jobs list
  - `SIGCHLD` → handle child process termination
- ✅ Piping support (`|`)
- ✅ Custom shell prompt (`PS1=`)

---

## 🧠 Concepts Used

- Process creation (`fork`)
- Program execution (`execvp`)
- Process control (`waitpid`)
- Signal handling (`SIGINT`, `SIGTSTP`, `SIGCHLD`)
- Linked List (for job management)
- File handling & parsing
- Inter-process communication (pipes)

---

## 🏗️ Project Structure
```
├── main.c # Entry point and signal registration
├── scan_input.c # Input handling and command parsing
├── functions.c # Built-in and external command execution
├── signal.c # Signal handling and job control
├── header.h # Structure definitions and prototypes
├── External_command.txt # List of external commands
```
---

## ⚙️ How It Works

1. Shell reads user input
2. Identifies command type (built-in / external)
3. For external commands:
   - Creates child process using `fork()`
   - Executes command using `execvp()`
4. Parent process:
   - Waits using `waitpid()`
   - Handles signals for job control
5. Jobs are stored in a linked list

---

## 📌 Sample Usage
```
Minishell:$ sleep 10
^Z
[1]+ Stopped sleep 10

Minishell:$ jobs
[1]+ Stopped sleep 10

Minishell:$ bg
[1]+ sleep 10

Minishell:$ fg
sleep 10

```
---

## 🧪 Compilation & Run
```
gcc *.c -o minishell
./minishell
```
## ⚠️ Notes

- Job control is implemented using a singly linked list
- Signal handling ensures proper process management
- Designed for learning system programming concepts

---
