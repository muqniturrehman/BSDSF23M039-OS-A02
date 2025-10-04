# 🧠 BSDSF23M039 – Operating Systems Assignment 01

This repository contains the source code and documentation for **Operating Systems Assignment 01**, completed by **Muqnit ur Rehman (BSDSF23M039)** as part of the **BS Data Science** program at **Punjab University College of Information Technology (PUCIT)**.

---

## 📘 Project Overview
This assignment focuses on **core Operating System concepts** — such as process management, inter-process communication (IPC), and file handling — implemented using the **C programming language** in a **Linux (Debian/Ubuntu/WSL)** environment.

---

## 🧩 Contents
| File | Description |
|------|--------------|
| `main.c` | Main program file — contains process creation and control logic. |
| `myfilefunctions.h` | Custom header defining reusable file operation functions. |
| `Makefile` | Automates compilation, linking, and cleanup tasks. |
| `README.md` | Documentation file (this one). |

---

## ⚙️ Features
- Demonstrates **process creation** using `fork()`
- Implements **file I/O operations** (read/write/append)
- Uses **system calls** like `open`, `close`, `read`, `write`
- Includes **error handling** and **memory management**
- Built and tested on **Linux (Debian 12 / WSL2)**

---

## 🧮 Compilation & Execution

### 🔧 Using `gcc` manually
```
gcc main.c -o os_assignment
./os_assignment
```

### 🧰 Using Makefile
```
make          # compile
./main        # run the compiled program
make clean    # remove compiled files
```

---

## 🧠 Learning Outcomes
- Understanding how processes are created and managed in Linux  
- Familiarity with low-level file handling in C  
- Experience writing modular code using headers and Makefiles  
- Practical exposure to Linux command-line compilation  

---

## 🖥️ Environment Used
- **OS:** Debian 12 (Bookworm) / WSL2 on Windows 10  
- **Compiler:** `gcc (Debian 12.2.0)`  
- **Editor:** Visual Studio Code / nano  

---

## 📂 Folder Structure
```
BSDSF23M039-OS-A01/
│
├── main.c
├── myfilefunctions.h
├── Makefile
└── README.md
```

---

## ✍️ Author
**Muqnit ur Rehman**  
BSDSF23M039 – BS Data Science  
Punjab University College of Information Technology (PUCIT)  
📧 [muqniturrehman@gmail.com](mailto:muqniturrehman@gmail.com)

---

## 🪶 License
This project is submitted as part of academic coursework.  
You are free to view or reference the code for learning purposes.
