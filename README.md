# Industrial Pastry Management System

A sophisticated simulation system for managing an industrial pastry shop, implementing complex data structures and algorithms for order management, inventory control, and delivery scheduling.

## Project Overview

This project simulates the complete workflow of an industrial pastry shop, from recipe management to ingredient inventory and order fulfillment. The system operates in discrete time steps and handles multiple concurrent operations including recipe creation, inventory management with expiration tracking, order processing, and delivery scheduling.

## Grade Achievement

**Final Grade: 30L (30 cum laude)** - Maximum grade with honors

### Grade Distribution Table

| Problem | Test Name | Time Limit | Memory Limit | Type | File | Token | Grade |
|---------|-----------|------------|--------------|------|------|-------|-------|
| Open | Open | 45,000 seconds | 150 MiB | Batch | Open[.c] | Si | 30L |
| 18 | 18 | 14,000 seconds | 35.0 MiB | Batch | 18[.c] | Si | 30L |
| 21 | 21 | 11,500 seconds | 30.0 MiB | Batch | 21[.c] | Si | 30L |
| 24 | 24 | 9,000 seconds | 25.0 MiB | Batch | 24[.c] | Si | 30L |
| 27 | 27 | 6,500 seconds | 20.0 MiB | Batch | 27[.c] | Si | 30L |
| 30 | 30 | 4,000 seconds | 15.0 MiB | Batch | 30[.c] | Si | 30L |
| Lode | Lode | 1,500 seconds | 14.0 MiB | Batch | Lode[.c] | Si | 30L |

## Features

### Core Functionality
- **Recipe Management**: Add and remove recipes with multiple ingredients
- **Inventory Control**: Track ingredient quantities with expiration dates using min-heaps
- **Order Processing**: Handle customer orders with availability checking
- **Delivery Scheduling**: Optimize truck loading based on weight and chronological order
- **Real-time Processing**: All operations occur in discrete time steps

### Data Structures Implemented
- **Hash Tables**: For efficient recipe, ingredient, and inventory lookups
- **Min-Heaps**: For expiration date management and delivery scheduling
- **Max-Heaps**: For printing orders by weight priority
- **Doubly Linked Lists**: For recipe and order queue management
- **Dynamic Arrays**: For flexible ingredient storage

### Key Algorithms
- **Hash Functions**: Custom implementation for string-based lookups
- **Heap Operations**: Insert, extract-min, and heapify for priority queues
- **Expiration Management**: Automatic removal of expired ingredients
- **Order Optimization**: FIFO processing with weight-based loading

## System Requirements

- C Compiler (GCC recommended)
- Make utility
- Unix/Linux environment (for testing scripts)

## How to Build and Run

### Building the Project

```bash
# Compile the project
make

# Or compile manually
gcc -o app main.c -std=c99 -O2
```

### Running the Application

The application reads commands from standard input and processes them sequentially:

```bash
# Run with manual input
./app

# Run with test files (recommended)
./app < open_tests/open1.txt
./app < open_tests/open2.txt
# ... continue with open3.txt through open11.txt
```

### Testing with Open Test Cases

The project includes comprehensive test cases in the `open_tests/` directory:

```bash
# Run a specific test
./app < open_tests/open5.txt

# Compare output with expected results
./app < open_tests/open5.txt > output.txt
diff output.txt expected_results/open5_expected.txt

# Automated testing script (if available)
for i in {1..11}; do
    echo "Testing open$i.txt"
    ./app < open_tests/open$i.txt > temp_output.txt
    diff temp_output.txt expected_results/open$i.txt
done
```

## Input Format

The system expects the following input format:

1. **Configuration Line**: `<courier_frequency> <truck_capacity>`
2. **Commands** (one per line):
   - `aggiungi_ricetta <recipe_name> <ingredient1> <quantity1> <ingredient2> <quantity2> ...`
   - `rimuovi_ricetta <recipe_name>`
   - `rifornimento <ingredient1> <quantity1> <expiration1> <ingredient2> <quantity2> <expiration2> ...`
   - `ordine <recipe_name> <quantity>`

## Output Format

The system provides real-time feedback:
- `aggiunta` / `ignorato` (for recipe additions)
- `rimossa` / `ordini in sospeso` / `non presente` (for recipe removal)
- `rifornito` (for inventory restocking)
- `accettato` / `rifiutato` (for orders)
- Delivery manifests: `<order_time> <recipe_name> <quantity>`
- `camioncino vuoto` (when delivery truck is empty)

## Architecture Details

### Memory Management
- Dynamic allocation for all data structures
- Comprehensive deallocation in cleanup phase
- Realloc-based array expansion for scalability

### Hash Table Implementation
- Separate chaining for collision resolution
- Custom hash functions optimized for string keys
- Multiple tables for different entity types

### Time Complexity
- Recipe lookup: O(1) average, O(n) worst case
- Ingredient management: O(log n) for heap operations
- Order processing: O(m log n) where m is orders and n is ingredients

### Space Complexity
- O(R + I + O) where R=recipes, I=ingredients, O=orders
- Efficient memory usage with dynamic allocation

## Project Structure

```
.
├── main.c              # Main implementation file
├── Makefile           # Build configuration
├── README.md          # This file
├── open_tests/        # Test input files
│   ├── open1.txt
│   ├── open2.txt
│   └── ...
└── expected_results/  # Expected output files
    ├── open1.txt
    ├── open2.txt
    └── ...
```

## Performance Characteristics

The solution is optimized for:
- **Time Efficiency**: Meets all time constraints (1.5-45k seconds)
- **Memory Efficiency**: Stays within memory limits (14-150 MiB)
- **Scalability**: Handles large datasets with thousands of operations

## Academic Context

This project was developed as the final examination for the "Algorithms and Data Structures" course (2023-2024 academic year). It demonstrates advanced understanding of:
- Complex data structure design and implementation
- Algorithm optimization and analysis
- Memory management in C
- Real-time system simulation
- Performance optimization under constraints

## Contributing

This is an academic project and represents completed coursework. The implementation achieved maximum grades across all test cases, including the challenging "Lode" (honors) test with strict time and memory constraints.

## License

Academic project - Educational use only.