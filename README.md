## 🍰 Industrial Pastry Management System

> A simulation system for managing an industrial pastry shop, implementing efficient data structures and algorithms for order management, inventory control, and delivery scheduling.

![Language](https://img.shields.io/badge/language-C-blue)
![Build](https://img.shields.io/badge/build-Makefile-success)
![Grade](https://img.shields.io/badge/grade-30L-brightgreen)

📄 See the [project specification](Project_specifications/Specifica_API_23-24.pdf)  
🧪 Explore the [test cases and expected results](open_tests/) 

---

## Table of Contents
- [Project Overview](#project-overview)
- [Grade Achievement](#grade-achievement)
- [Features](#features)
- [Repository Structure](#repository-structure)
- [System Requirements](#system-requirements)
- [How to Build and Run](#how-to-build-and-run)
- [Testing](#testing)
- [Example Run](#example-run)
- [Input Format](#input-format)
- [Output Format](#output-format)
- [License](#license)

---

## Project Overview

This project simulates the complete workflow of an industrial pastry shop, from recipe management to ingredient inventory and order fulfillment.  
The system operates in **discrete time steps** and handles multiple concurrent operations:

- Recipe creation and removal  
- Ingredient inventory management with expiration tracking  
- Order processing with acceptance/queueing  
- Delivery scheduling with truck capacity optimization  

---

## Grade Achievement

**Final Grade: 30L (30 cum laude)** – Maximum grade with honors 🎉

### Grade Distribution Table

| Problem | Test Name | Time Limit | Memory Limit | 
|---------|-----------|------------|--------------|
| Open    | Open      | 45,000 s   | 150 MiB      | 
| 18      | 18        | 14,000 s   | 35.0 MiB     |  
| 21      | 21        | 11,500 s   | 30.0 MiB     | 
| 24      | 24        | 9,000 s    | 25.0 MiB     | 
| 27      | 27        | 6,500 s    | 20.0 MiB     | 
| 30      | 30        | 4,000 s    | 15.0 MiB     | 
| Lode    | Lode      | 1,500 s    | 14.0 MiB     | 

---

## Features

### Core Functionality
- **Recipe Management**: add and remove recipes with multiple ingredients
- **Inventory Control**: track ingredient quantities with expiration dates using min-heaps
- **Order Processing**: handle customer orders with availability checking and queuing
- **Delivery Scheduling**: optimize truck loading based on weight and order chronology
- **Real-time Processing**: all operations occur in discrete time steps

### Data Structures Implemented
- **Hash Tables** – efficient recipe, ingredient, and inventory lookups  
- **Min-Heaps** – expiration date management and delivery scheduling  
- **Max-Heaps** – order prioritization by weight  
- **Doubly Linked Lists** – recipe catalog and order queues  
- **Dynamic Arrays** – flexible ingredient storage  

### Key Algorithms
- **Custom Hash Functions** for string-based lookups  
- **Heap Operations**: insert, extract-min, heapify  
- **Expiration Management**: automatic removal of expired ingredients  
- **Order Optimization**: FIFO processing with weight-based truck loading  

---

## Repository Structure

```text
.
├── API.c                       # Main implementation
├── Makefile                    # Build system
├── Project_specifications/     # Official specification (PDF)
│   └── Presentazione PFAPI 2023-2024.pdf
│   └── Presentazione_strumenti.pdf
│   └── Specifica_API_23-24.pdf
├── open_tests/                 # Provided test cases and expected results
└── README.md                   # Documentation
```

## System Requirements

- C compiler (GCC recommended)  
- Make utility  
- Unix/Linux environment (for testing scripts)  

---

## How to Build and Run

### Building

```bash
# Compile the project using the Makefile
make

# Or compile manually
gcc -o app API.c -std=c99 -O2
```

### Running

```bash
# Run with manual input
./app

# Run with test files (recommended)
./app < open_tests/open1.txt
./app < open_tests/open2.txt
```

---

## Testing

Comprehensive **open test cases** are available in the `open_tests/` directory.  

```bash
# Run a specific test
./app < open_tests/open5.txt

# Compare output with expected results
./app < open_tests/open5.txt > output.txt
diff output.txt open_tests/open5.output.txt
```

---

## Example Run

**Input**
```
5 325
aggiungi_ricetta torta farina 50 uova 10 zucchero 20
rifornimento farina 100 10 uova 100 10 zucchero 100 10
ordine torta 2
```

**Output**
```
aggiunta
rifornito
accettato
camioncino vuoto
```

---

## Input Format

1. **Configuration Line**  
   `<courier_frequency> <truck_capacity>`  

2. **Commands** (one per line):  
   - `aggiungi_ricetta <recipe_name> <ingredient1> <quantity1> ...`  
   - `rimuovi_ricetta <recipe_name>`  
   - `rifornimento <ingredient1> <quantity1> <expiration1> ...`  
   - `ordine <recipe_name> <quantity>`  

---

## Output Format

- `aggiunta` / `ignorato` – recipe management  
- `rimossa` / `ordini in sospeso` / `non presente` – recipe removal  
- `rifornito` – inventory restock  
- `accettato` / `rifiutato` – order handling  
- Delivery manifests: `<order_time> <recipe_name> <quantity>`  
- `camioncino vuoto` – when delivery truck is empty  

---

## License

This project was developed as part of the **Final Exam of Algorithms and Data Structures (2023/24)**.  
Feel free to use it for study and reference purposes. For any other use, please check academic integrity rules.
