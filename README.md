# Warehouse Robot Navigation System — Data Structures and Algorithms (DSTR)

**CT077-3-2-DSTR | C++ | Group project**

A warehouse robot simulation: orders queue up, robots rotate via circular queue, BST finds item locations, and an N-ary tree models the warehouse layout for route planning.

## My part: Task 5 — Warehouse Layout N-ary Tree

- `robot-system/WarehouseNode.hpp` / `WarehouseTree.hpp` / `WarehouseTree.cpp` — N-ary tree with `buildFromCSV()`, `generateRoute()` (ENTER_ZONE → MOVE_TO_AISLE → MOVE_TO_SHELF), BFS, DFS, `findNode()`, `displayTree()`
- Integrated into `robot-system/main.cpp` — Task 5 routes feed into Task 3 (Path Stack); menu options 14–17

Why N-ary tree? Warehouse → zones → aisles → shelves is a natural parent-child hierarchy.

## Group split
Task 1: Order Queue | Task 2: Robot Circular Queue | Task 3: Path Stack | Task 4: Item BST | **Task 5: Warehouse Layout (Me — Ibrahim Bin Mohd Ezman, TP081387)**
