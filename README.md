# Finding Algorithms Simulation

## Overview

This project implements a mathematical transformation problem solver using multiple search algorithms. The core challenge is to find the minimum-cost sequence of operations to transform a starting number into a target number using six different mathematical operations.

## Problem Definition

**Goal**: Transform a starting integer `S` into a target integer `T` using the minimum total cost.

**Available Operations**:

1. **Increase**: `x → x + 1` (Cost: 2)
2. **Decrease**: `x → x - 1` (Cost: 2)
3. **Double**: `x → 2x` (Cost: `x/2 + 1`)
4. **Half**: `x → x/2` (Cost: `x/4 + 1`)
5. **Square**: `x → x²` (Cost: `(x² - x)/4 + 1`)
6. **Root**: `x → √x` (Cost: `(x - √x)/4 + 1`, only if x is perfect square)

## Architecture Overview

### Core Data Structures

#### State Representation (`topic1.c:132-137`)

```c
typedef struct {
    unsigned long value;      // Current number
    unsigned long cost;       // Total cost to reach this state
    unsigned long depth;      // Search depth
    char path[MAX_PATH_LENGTH]; // Operation sequence
} State;
```

#### Hash Table for Visited States (`topic1.c:9-43`)

- **Size**: 1,000,000 buckets
- **Type**: Separate chaining hash table
- **Purpose**: Prevents revisiting states and cycles
- **Hash Function**: Simple modulo operation

### Search Algorithms Implementation

#### 1. Breadth-First Search (BFS) (`topic1.c:555-570`)

- **Guarantee**: Finds optimal solution in terms of number of operations
- **Time Complexity**: O(b^d) where b is branching factor, d is depth
- **Space Complexity**: O(b^d)
- **Use Case**: When operation count matters more than cost

#### 2. Depth-First Search (DFS) (`topic1.c:573-589`)

- **Depth Limit**: 50 levels to prevent infinite recursion
- **Time Complexity**: O(b^d)
- **Space Complexity**: O(d)
- **Use Case**: Memory-constrained environments, exploratory search

#### 3. Best-First Search (`topic1.c:591-606`)

- **Heuristic**: Distance-based estimation to target
- **Priority**: Pure heuristic value (greedy approach)
- **Use Case**: When good heuristic available and speed matters

#### 4. A\* Search (`topic1.c:608-627`)

- **Priority Function**: `f(n) = g(n) + h(n)`
- **g(n)**: Actual cost from start
- **h(n)**: Heuristic estimate to goal
- **Guarantee**: Optimal solution if heuristic is admissible
- **Use Case**: Best balance of optimality and efficiency

## Critical Algorithm Components

### Heuristic Function (`topic1.c:218-249`)

The heuristic estimates minimum cost to reach target:

**Strategy**:

- If `current < target`: Use doubling when safe, otherwise increment
- If `current > target`: Use halving when safe, otherwise decrement
- **Admissibility**: Guaranteed to never overestimate actual cost

**Key Optimizations**:

- Overflow protection: Doubling limited to values ≤ 500,000,000
- Underflow protection: Halving only when result ≥ target

### Cycle Detection (`topic1.c:195-216`)

Prevents infinite loops by checking if current value already exists in the path history.

### Operation Application (`topic1.c:328-446`)

**Safety Constraints**:

- **Double**: Only if `value ≤ 500,000,000` (prevents overflow)
- **Square**: Only if `value ≤ 31,622` (√10^9, prevents overflow)
- **Root**: Only if value is perfect square
- **Bounds**: All operations respect 0 ≤ value ≤ 10^9

## Performance Characteristics

### Memory Usage

- **Hash Table**: ~8MB (1M pointers + collision chains)
- **Search Queue/Stack**: Variable, depends on search space
- **Path Storage**: Up to 10KB per state

### Time Complexity Analysis

| Algorithm  | Best Case | Average Case | Worst Case |
| ---------- | --------- | ------------ | ---------- |
| BFS        | O(1)      | O(b^(d/2))   | O(b^d)     |
| DFS        | O(1)      | O(b^d)       | O(b^50)    |
| Best-First | O(1)      | O(b^h)       | O(b^∞)     |
| A\*        | O(1)      | O(b^d)       | O(b^d)     |

Where: b = branching factor (~6), d = solution depth, h = heuristic accuracy

## Usage

### Compilation

```bash
gcc -o topic1 topic1.c -lm
```

### Execution

```bash
./topic1 [algorithm] [start] [target] [output_file]
```

**Algorithms**: `breadth`, `depth`, `best`, `astar`

**Example**:

```bash
./topic1 astar 2 4323 solution.txt
```

### Output Format

```
N, C
operation1 value1 cost1
operation2 value2 cost2
...
```

Where:

- `N`: Number of operations
- `C`: Total cost
- Each line: operation name, resulting value, operation cost
