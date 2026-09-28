# Warehouse Robot Navigation System — DSTR Project

**Module:** CT077-3-2-DSTR Lab Evaluation Work #2
**University:** Asia Pacific University (APU)
**Language:** C++ (no STL containers — all data structures hand-built)

---

## Project Overview

A C++ prototype simulating a warehouse robot navigation system for an e-commerce warehouse. Customer orders are queued, assigned to robots in round-robin fashion, navigated to via step-by-step path tracking, and robots return by retracing their path.

## System Architecture

```
orders.csv → Task 1 Queue → Task 2 Circular Queue → Task 4 BST (item location)
                                                          ↓
warehouse_layout.csv → Task 5 N-ary Tree → generateRoute() → Task 3 Path Stack → Robot movement
```

## Full System Workflow

1. New order received → enqueued in Order Queue (Task 1)
2. Order dequeued → assigned to robot via Circular Queue rotation (Task 2)
3. Item location searched in BST (Task 4)
4. Route generated from warehouse tree (Task 5)
5. Steps pushed onto Path Stack → robot moves forward (Task 3)
6. Destination reached → steps popped in reverse → robot returns (Task 3)
7. Status updated in CSV files

## Data Structures Used

| Module | Structure | Purpose |
|--------|-----------|---------|
| Task 1 — Order Management | Custom Linked-List Queue | FIFO order processing |
| Task 2 — Robot Assignment | Custom Circular Queue | Round-robin robot rotation |
| Task 3 — Path Tracking | Custom Linked-List Stack | LIFO forward/reverse path |
| Task 4 — Item Search | Custom Binary Search Tree | O(log n) item lookup |
| **Task 5 — Warehouse Layout** | **Custom N-ary Tree** | **Hierarchical zone→aisle→shelf navigation** |

---

## 🎯 MY CONTRIBUTION: Task 5 — Warehouse Layout and Navigation Module

**I was responsible for Task 5** (optional module for teams of 4–5 members). This module models the physical warehouse as a hierarchical tree and generates step-by-step navigation routes for robots.

### What Task 5 Was About

Task 5 required modeling the warehouse physical layout (zones → aisles → shelves) as an N-ary tree so routes can be planned. The robot needs to know the path from base to a target shelf before it can navigate.

### What I Implemented

- **WarehouseNode.hpp** — N-ary tree node struct with nodeID, nodeName, nodeType (ROOT/ZONE/AISLE/SHELF), and dynamic children array
- **WarehouseTree.hpp / WarehouseTree.cpp** — Full N-ary tree class with:
  - buildFromCSV() — Two-pass construction from warehouse_layout.csv
  - generateRoute() — Generates step-by-step navigation: ENTER_ZONE → MOVE_TO_AISLE → MOVE_TO_SHELF
  - bfsTraversal() — Breadth-first search using array-based queue
  - dfsTraversal() — Depth-first search using recursion
  - findNode() — DFS node lookup by ID
  - displayTree() — Indented visual tree display
  - isEmpty() — O(1) empty check
- **Integration in main.cpp** — Task 5 routes are passed to Task 3 (Path Stack) for robot navigation; menu options 14–17 for tree operations

### Why N-ary Tree

"I chose an N-ary tree because the warehouse has a clear parent-child hierarchy: warehouse → zones → aisles → shelves. A tree naturally represents this structure and makes path traversal straightforward."

### Task 5 in the Assignment Brief

The DSTR assignment brief specifies Task 5 as:
- **Functional Requirements:** Model the warehouse layout into structured sections (zones, aisles, shelves), Define connections between different locations, Provide navigation routes from one point to another, Support traversal through all warehouse sections, Integrate with the robot navigation module for path planning
- **Key Features:** Structured representation of warehouse layout, Efficient route generation, Scalable design for large warehouse environments
- **Expected Output:** Visual or textual representation of warehouse layout, Path between selected locations, Traversal results

### File Structure

```
robotSystem/
├── main.cpp                    # Main program — full pipeline integration
├── WarehouseNode.hpp           # N-ary tree node (MY TASK 5)
├── WarehouseTree.hpp           # N-ary tree class header (MY TASK 5)
├── WarehouseTree.cpp           # N-ary tree implementation (MY TASK 5)
├── OrderQueue.hpp/cpp          # Task 1 — Order Queue
├── RobotCircularQueue.hpp/cpp  # Task 2 — Robot Circular Queue
├── PathStack.hpp/cpp           # Task 3 — Path Stack with map visualization
├── ItemBST.hpp/cpp             # Task 4 — Item Search BST
├── CSVLoader.hpp/cpp           # CSV loading utilities
├── Item.hpp / Robot.hpp / Order.hpp  # Data structs
├── orders.csv / robots.csv / items.csv / warehouse_layout.csv  # Data
└── Warehouse_Robot_System_Dev_Guide.md  # Full assignment spec
```

## Group Work Note

This is a **group project** for CT077-3-2-DSTR. Each member implemented one module:
- Task 1: Order Queue (Member 1)
- Task 2: Robot Circular Queue (Member 2)
- Task 3: Path Stack (Member 3)
- Task 4: Item BST (Member 4)
- **Task 5: Warehouse Layout N-ary Tree (Me — Ibrahim Bin Mohd Ezman, TP081387)**

---

*Repository created for academic portfolio purposes.*
