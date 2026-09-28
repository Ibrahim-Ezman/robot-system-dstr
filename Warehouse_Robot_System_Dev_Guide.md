# Warehouse Robot Navigation System — Full Development Guide
### CT077-3-2-DSTR Lab Evaluation Work #2

---

## A. CONCISE SUMMARY OF THE ASSIGNMENT

You are building **one single integrated C++ prototype** for a warehouse robot system. The system must manage customer orders, assign them to robots, help robots navigate to items, and allow robots to return by retracing their path. There are **3 mandatory modules** (Tasks 1–3) and **2 optional modules** (Tasks 4–5 for teams of 4–5). Every module must be connected into one working program. STL containers (`vector`, `list`, `queue`, `stack`, `map`, etc.) are **not allowed** — all containers must be hand-built.

---

## SECTION 1 — ASSIGNMENT UNDERSTANDING

### 1.1 Purpose of the System (Plain Language)

The system simulates how a warehouse manages orders and robots automatically:
- Customers place orders → orders are queued up fairly.
- A robot is selected by rotation → no single robot is overloaded.
- The robot moves step-by-step to the item location → every step is recorded.
- Once the item is picked, the robot retraces its path in reverse to return to base.
- (Optional) Items are stored in a searchable structure so their shelf location can be found fast.
- (Optional) The warehouse physical layout (zones → aisles → shelves) is modeled as a tree so routes can be planned.

### 1.2 Overall Workflow (Order Arrival → Robot Return)

```
[Step 1]  New order arrives
           → Order is added to the Order Queue (Task 1)

[Step 2]  Order is dequeued for processing
           → Next available robot is selected from Circular Queue (Task 2)

[Step 3]  Item location is determined
           → Item Search Module (Task 4) looks up shelf location from BST

[Step 4]  Route is planned through warehouse layout
           → Warehouse Layout Module (Task 5) provides path: Zone → Aisle → Shelf

[Step 5]  Robot starts moving step-by-step
           → Each move is pushed onto the Path Stack (Task 3)

[Step 6]  Robot reaches item → picks it up → task complete

[Step 7]  Robot returns by reversing its path
           → Steps are popped from the Stack in reverse order

[Step 8]  Robot status is updated → ready for next assignment
```

---

## SECTION 2 — OVERALL SYSTEM ARCHITECTURE

### 2.1 How All Modules Connect

```
┌──────────────────────────────────────────────────────────────────────┐
│                          MAIN PROGRAM (main.cpp)                      │
│                                                                        │
│  [1] Load CSV files into custom data structures                        │
│  [2] Present menu / drive the workflow                                 │
│  [3] Call modules in sequence                                          │
└───────────┬──────────────────────────────────────────────────────────┘
            │
            ▼
┌──────────────────┐       ┌──────────────────────┐
│  Task 1          │──────▶│  Task 2               │
│  Order Queue     │ order │  Robot Circular Queue │
│  (Queue FIFO)    │ data  │  (Circular Queue)     │
└──────────────────┘       └──────────┬───────────┘
                                       │ assigned robot + order
                                       ▼
                           ┌──────────────────────┐
                           │  Task 4 (optional)   │
                           │  Item Search BST     │
                           │  → returns location  │
                           └──────────┬───────────┘
                                       │ shelf location
                                       ▼
                           ┌──────────────────────┐
                           │  Task 5 (optional)   │
                           │  Warehouse Tree      │
                           │  → generates route   │
                           └──────────┬───────────┘
                                       │ step-by-step path
                                       ▼
                           ┌──────────────────────┐
                           │  Task 3              │
                           │  Path Stack          │
                           │  → forward + reverse │
                           └──────────────────────┘
```

### 2.2 What `main.cpp` Should Do (Step by Step)

| Step | Action |
|------|--------|
| 1 | Load `orders.csv` → enqueue all pending orders into custom Queue |
| 2 | Load `robots.csv` → insert all robots into custom Circular Queue |
| 3 | Load `items.csv` → insert all items into custom BST (Task 4) |
| 4 | Load `warehouse_layout.csv` → build the warehouse Tree (Task 5) |
| 5 | Show main menu — allow user to trigger actions (add order, process order, etc.) |
| 6 | When "Process Next Order" is selected: dequeue order → assign robot → search item → get route → push path steps → simulate forward movement → pop steps → simulate return |
| 7 | After completing each task, update CSV files as needed |

### 2.3 How Data Moves Between Modules

```
orders.csv     ──→  Task 1 Queue          ──→  Task 2 (passes order to robot)
robots.csv     ──→  Task 2 Circular Queue ──→  Task 3 (robot starts moving)
items.csv      ──→  Task 4 BST            ──→  Task 3 (provides destination)
layout.csv     ──→  Task 5 Tree           ──→  Task 3 (provides route steps)
movement_log   ←──  Task 3 Stack          (writes completed path back to CSV)
```

---

## SECTION 3 — MODULE-BY-MODULE BREAKDOWN

---

### TASK 1: Order Management Module

**Purpose:** Receive incoming customer orders and ensure they are processed one at a time in the order they arrived (FIFO = First In, First Out).

**Exact Tasks to Perform:**
- Load existing pending orders from `orders.csv` into a Queue at startup.
- Allow adding new orders (enqueue).
- Process the next order (dequeue) when a robot is ready.
- Display all pending orders in the queue.
- Display completed orders (separate list or updated CSV).
- Handle the case where the queue is empty (attempt to process with no orders).

**Data Structure Principle:** **Custom Queue (Linked-List based)**
- Uses nodes where each node holds an order.
- `enqueue()` adds to the rear.
- `dequeue()` removes from the front.
- `peek()` shows the front without removing.
- `isEmpty()` and `isFull()` for edge case handling.

**Core Operations:**
| Operation | Description |
|-----------|-------------|
| `enqueue(order)` | Add new order to rear |
| `dequeue()` | Remove and return front order |
| `peek()` | View front order without removing |
| `displayAll()` | Show all queued orders |
| `isEmpty()` | Check if queue is empty |

**Edge Cases:**
- Queue is empty when dequeue is called → print "No pending orders"
- Adding a duplicate order ID → check before enqueue
- System restart → reload from CSV so no orders are lost

**Expected Demo Output:**
```
=== ORDER QUEUE ===
[Front] Order#001 - Item: ITEM_A - Zone: B
        Order#002 - Item: ITEM_C - Zone: A
        Order#003 - Item: ITEM_B - Zone: C [Rear]

Processing Order#001...
Order#001 dequeued. Assigned to Robot R2.
```

---

### TASK 2: Robot Assignment Module

**Purpose:** Distribute work fairly among all robots by assigning tasks in a circular rotation. No robot should be skipped permanently; unavailable robots are skipped temporarily.

**Exact Tasks to Perform:**
- Load robot list from `robots.csv` into a Circular Queue.
- When a task arrives, rotate through the queue to find the next available robot.
- Skip robots marked as BUSY or MAINTENANCE.
- Mark assigned robot as BUSY.
- Mark robot as AVAILABLE again after task is complete.
- Display robot status list.

**Data Structure Principle:** **Custom Circular Queue (Array or Linked-List based)**
- The tail wraps back to the head — this is what makes assignment "rotating."
- A pointer tracks the "current" robot position.
- The queue never truly empties (robots cycle continuously).
- Size is fixed (number of robots is known from CSV).

**Core Operations:**
| Operation | Description |
|-----------|-------------|
| `assignNext()` | Find and return next AVAILABLE robot |
| `setStatus(id, status)` | Mark robot as BUSY or AVAILABLE |
| `displayAll()` | Show all robots and their current status |
| `skipBusy()` | Internal: rotate past BUSY/MAINTENANCE robots |

**Edge Cases:**
- All robots are BUSY → print "No robots available, order waiting"
- Only one robot exists → it handles everything sequentially
- Robot enters MAINTENANCE mid-cycle → skip permanently until status changes

**Expected Demo Output:**
```
=== ROBOT CIRCULAR QUEUE ===
[R1: AVAILABLE] → [R2: BUSY] → [R3: AVAILABLE] → [R4: MAINTENANCE] → (back to R1)

Assigning Order#001...
Skipping R1 (no reason given)... ← only for demo rotation
Robot R3 assigned to Order#001. Status → BUSY.
```

---

### TASK 3: Robot Navigation and Path Tracking Module

**Purpose:** Record every step the robot takes as it moves toward the item, and allow the robot to return by reversing those steps. This uses a Stack (LIFO — Last In, First Out).

**Exact Tasks to Perform:**
- Receive the route (list of step directions) from the layout module or as manual input.
- Push each movement step onto a Stack as the robot moves forward.
- Once the destination is reached, pop steps one by one to simulate return.
- Display the full forward path log.
- Display the full reverse path log.
- Handle obstacles: if a step is blocked, pop back to the last safe position (backtrack).

**Data Structure Principle:** **Custom Stack (Array or Linked-List based)**
- `push()` records each forward step.
- `pop()` retrieves steps in reverse order for return journey.
- `peek()` shows the last step taken without removing it.

**Core Operations:**
| Operation | Description |
|-----------|-------------|
| `push(step)` | Record one movement step |
| `pop()` | Remove and return the most recent step |
| `peek()` | View top of stack (last step) |
| `displayPath()` | Print all steps in order |
| `reverseReturn()` | Loop pop() until stack is empty, printing reverse path |
| `isEmpty()` | Check if robot is already at base |

**Edge Cases:**
- Stack is empty when pop is called → robot is already at starting point
- Obstacle encountered mid-path → pop back several steps, reroute
- Path has zero steps → robot was already at destination

**Expected Demo Output:**
```
=== FORWARD PATH (Robot R3 → Order#001) ===
Step 1: FORWARD
Step 2: LEFT
Step 3: FORWARD
Step 4: FORWARD
Step 5: RIGHT
[Destination Reached - Item ITEM_A picked up]

=== RETURN PATH (Reverse) ===
Step 1: LEFT      (was RIGHT)
Step 2: BACKWARD  (was FORWARD)
Step 3: BACKWARD  (was FORWARD)
Step 4: RIGHT     (was LEFT)
Step 5: BACKWARD  (was FORWARD)
[Robot R3 returned to base]
```

---

### TASK 4: Item Search and Management Module *(Optional)*

**Purpose:** Store all warehouse items in a structure that allows fast lookup by ID or name. The robot needs to know the exact shelf location of an item before it can navigate there.

**Exact Tasks to Perform:**
- Load all items from `items.csv` into a Binary Search Tree (BST), keyed by Item ID.
- Search for an item by ID or name.
- Insert new items into the BST.
- Delete items that are removed from the warehouse.
- Display all items in sorted order (in-order traversal).
- Return the shelf location of a found item (to be passed to Task 3/5).

**Data Structure Principle:** **Custom Binary Search Tree (BST)**
- Each node stores: ItemID, Name, ZoneID, AisleID, ShelfID, Quantity.
- Left subtree has smaller IDs; right subtree has larger IDs.
- In-order traversal gives a sorted listing of all items.

**Core Operations:**
| Operation | Description |
|-----------|-------------|
| `insert(item)` | Add a new item node |
| `search(id)` | Find and return item by ID |
| `searchByName(name)` | Traverse to find by name (full traversal) |
| `deleteItem(id)` | Remove item and rebalance BST |
| `inOrderDisplay()` | Print all items in sorted order |
| `getLocation(id)` | Return zone/aisle/shelf for navigation |

**Edge Cases:**
- Item not found → print "Item not found" and return null
- Duplicate item ID on insert → reject or update existing
- Tree is empty when searching → handle gracefully
- Deleting a node with two children → use in-order successor method

**Expected Demo Output:**
```
=== ITEM DATABASE (BST In-Order) ===
ID: ITEM_001 | Name: Laptop   | Zone: A | Aisle: 2 | Shelf: 4 | Qty: 15
ID: ITEM_002 | Name: Mouse    | Zone: A | Aisle: 2 | Shelf: 7 | Qty: 30
ID: ITEM_003 | Name: Keyboard | Zone: B | Aisle: 1 | Shelf: 2 | Qty: 20

Search: ITEM_002
→ Found: Mouse | Location: Zone A, Aisle 2, Shelf 7
```

---

### TASK 5: Warehouse Layout and Navigation Module *(Optional)*

**Purpose:** Model the physical warehouse as a hierarchical tree (Warehouse → Zones → Aisles → Shelves) and generate a step-by-step navigation path from start to a target shelf.

**Exact Tasks to Perform:**
- Load warehouse structure from `warehouse_layout.csv` into a tree.
- Allow traversal of the tree (BFS or DFS) to visit all nodes.
- Given a target shelf location (from Task 4), generate a route: Zone → Aisle → Shelf.
- Output the route as a list of movement steps for Task 3 to consume.
- Display the full warehouse tree structure visually (indented text representation).

**Data Structure Principle:** **Custom General Tree (N-ary Tree)**
- Root node = Warehouse.
- Level 1 = Zones (A, B, C...).
- Level 2 = Aisles within each zone.
- Level 3 = Shelves within each aisle.
- Each node has: `name`, `type` (zone/aisle/shelf), `children[]`.

**Core Operations:**
| Operation | Description |
|-----------|-------------|
| `insertNode(parent, child)` | Build tree from CSV data |
| `findNode(name)` | Search tree for a specific location |
| `generateRoute(start, target)` | Return path as list of steps |
| `displayTree()` | Print tree with indentation |
| `BFSTraversal()` | Breadth-first traversal of all nodes |
| `DFSTraversal()` | Depth-first traversal of all nodes |

**Edge Cases:**
- Target shelf does not exist in tree → print "Location not found"
- Warehouse has disconnected zones → handle as separate subtrees
- Path from start to target goes through multiple zones → trace correctly

**Expected Demo Output:**
```
=== WAREHOUSE LAYOUT ===
Warehouse
├── Zone A
│   ├── Aisle 1
│   │   ├── Shelf 1
│   │   └── Shelf 2
│   └── Aisle 2
│       ├── Shelf 3
│       └── Shelf 4
└── Zone B
    └── Aisle 1
        └── Shelf 1

Route: BASE → Zone A → Aisle 2 → Shelf 4
Steps generated: [ENTER_ZONE_A, MOVE_TO_AISLE_2, MOVE_TO_SHELF_4]
→ Passed to Task 3 path stack.
```

---

## SECTION 4 — DATA STRUCTURE RECOMMENDATIONS

| Module | Recommended Structure | Why It Fits | Key Algorithms |
|--------|----------------------|-------------|----------------|
| Task 1: Order Management | Custom Linked-List Queue | Orders must be processed in arrival order (FIFO). Linked list allows unlimited orders without pre-declaring size. | `enqueue`, `dequeue`, `peek`, size tracking |
| Task 2: Robot Assignment | Custom Circular Queue (Array) | Robots rotate indefinitely. A circular array wraps the rear pointer back to index 0, simulating endless rotation. | Modulo-based pointer wrap `(rear+1) % capacity`, skip-if-busy logic |
| Task 3: Path Tracking | Custom Linked-List Stack | The last step taken is the first step to reverse (LIFO). A stack perfectly models "undo last move." | `push`, `pop`, `peek`, loop-pop for reverse path |
| Task 4: Item Search | Custom Binary Search Tree | Items can be searched by ID quickly (O(log n) on average). BST naturally sorts items and supports fast insert/delete. | Recursive insert, recursive search, in-order traversal, BST deletion (in-order successor) |
| Task 5: Warehouse Layout | Custom N-ary Tree | Warehouse has a clear parent-child hierarchy (Warehouse → Zone → Aisle → Shelf). A tree is the natural fit. | DFS traversal for path finding, BFS for level display, recursive insert |

---

## SECTION 5 — SHARED CSV FILE DESIGN

### 5.1 Complete File List

| File Name | Used By | Purpose |
|-----------|---------|---------|
| `orders.csv` | Task 1, Task 2 | Stores all customer orders |
| `robots.csv` | Task 2 | Stores all robots and their status |
| `items.csv` | Task 4, Task 3 | Stores item details and shelf locations |
| `warehouse_layout.csv` | Task 5, Task 3 | Stores warehouse structure |
| `movement_log.csv` | Task 3 | Records robot movement paths |

---

### 5.2 File Structures

#### `orders.csv`
```
OrderID, ItemID, CustomerName, Priority, Status, Timestamp
ORD001,  ITEM_002, Alice,      NORMAL,   PENDING, 2026-05-01 09:00
ORD002,  ITEM_005, Bob,        HIGH,     PENDING, 2026-05-01 09:05
ORD003,  ITEM_001, Carol,      NORMAL,   COMPLETED, 2026-05-01 08:30
```
- **Status values:** `PENDING`, `PROCESSING`, `COMPLETED`
- **Task 1** loads all PENDING orders into its Queue on startup.
- **Task 2** reads OrderID + ItemID to assign to a robot.

---

#### `robots.csv`
```
RobotID, Name,   Status,      CurrentOrderID, TotalTasksDone
R001,    RoboA,  AVAILABLE,   ,               12
R002,    RoboB,  BUSY,        ORD001,         8
R003,    RoboC,  MAINTENANCE, ,               5
R004,    RoboD,  AVAILABLE,   ,               15
```
- **Status values:** `AVAILABLE`, `BUSY`, `MAINTENANCE`
- **Task 2** loads this file to build its Circular Queue.
- `CurrentOrderID` is updated when a robot is assigned.

---

#### `items.csv`
```
ItemID,   Name,     ZoneID, AisleID, ShelfID, Quantity, Weight_kg
ITEM_001, Laptop,   A,      2,       4,       15,       1.5
ITEM_002, Mouse,    A,      2,       7,       30,       0.2
ITEM_003, Keyboard, B,      1,       2,       20,       0.5
ITEM_004, Monitor,  C,      3,       1,       8,        3.0
```
- **Task 4** loads this into a BST keyed on `ItemID`.
- `ZoneID`, `AisleID`, `ShelfID` are used by Task 5 to locate the shelf.
- Task 3 uses the location output from Task 4 to know the destination.

---

#### `warehouse_layout.csv`
```
NodeID, NodeName,  NodeType,  ParentID
ROOT,   Warehouse, ROOT,
Z_A,    Zone_A,    ZONE,      ROOT
Z_B,    Zone_B,    ZONE,      ROOT
Z_C,    Zone_C,    ZONE,      ROOT
A_A1,   Aisle_1,   AISLE,     Z_A
A_A2,   Aisle_2,   AISLE,     Z_A
A_B1,   Aisle_1,   AISLE,     Z_B
S_A2_4, Shelf_4,   SHELF,     A_A2
S_A2_7, Shelf_7,   SHELF,     A_A2
S_B1_2, Shelf_2,   SHELF,     A_B1
```
- **Task 5** reads this and builds the N-ary Tree by inserting each node under its `ParentID`.
- `NodeType` tells the tree builder which level a node belongs to.
- Task 5 outputs a route (e.g., `Z_A → A_A2 → S_A2_4`) as a string sequence to Task 3.

---

#### `movement_log.csv`
```
LogID, RobotID, OrderID, StepNo, Direction, Timestamp, Phase
1,     R002,    ORD001,  1,      FORWARD,   2026-05-01 09:10, FORWARD
2,     R002,    ORD001,  2,      LEFT,      2026-05-01 09:10, FORWARD
3,     R002,    ORD001,  3,      FORWARD,   2026-05-01 09:10, FORWARD
4,     R002,    ORD001,  3,      BACKWARD,  2026-05-01 09:12, RETURN
5,     R002,    ORD001,  2,      RIGHT,     2026-05-01 09:12, RETURN
6,     R002,    ORD001,  1,      BACKWARD,  2026-05-01 09:12, RETURN
```
- **Task 3** writes to this file after completing each path.
- `Phase` = `FORWARD` or `RETURN`.
- Can be used to audit robot journeys.

---

### 5.3 Relationships Between Files

```
orders.csv ─── ItemID ─────────────→ items.csv (Task 4 finds location)
orders.csv ─── OrderID ────────────→ robots.csv (Task 2 assigns robot)
items.csv  ─── ZoneID/AisleID/ShelfID → warehouse_layout.csv (Task 5 generates route)
robots.csv ─── RobotID ────────────→ movement_log.csv (Task 3 logs movement)
orders.csv ─── OrderID ────────────→ movement_log.csv (links log to order)
```

---

### 5.4 How Modules 4 and 5 Affect the CSV Design

> **Note for teams with 4–5 members:** Without Tasks 4 and 5, the system can still work using simplified hardcoded or manually entered locations. But when Tasks 4 and 5 are included, the CSV design must be enriched as follows:

| Change | Reason |
|--------|--------|
| `items.csv` must include `ZoneID`, `AisleID`, `ShelfID` | Task 4's BST returns these to Task 5 for route generation |
| `warehouse_layout.csv` must exist as a separate file | Task 5 cannot build its tree without this structured data |
| `NodeID` and `ParentID` columns must be consistent with `ZoneID` in `items.csv` | Task 5 must be able to locate the shelf node from `items.csv` location data |
| `movement_log.csv` may record full route labels (e.g., `Zone_A`, `Aisle_2`) instead of just directions | When Task 5 generates named routes, those names enrich the log |

---

## SECTION 6 — MAIN WORKFLOW / EXECUTION FLOW

### Step-by-Step Main Program Flow

```
STARTUP:
  1. Open orders.csv → read all PENDING rows → enqueue each into Order Queue (Task 1)
  2. Open robots.csv → read all robots → insert into Circular Queue (Task 2)
  3. Open items.csv → read all items → insert into BST (Task 4, if included)
  4. Open warehouse_layout.csv → build N-ary Tree (Task 5, if included)
  5. Display main menu

MENU OPTIONS:
  [1] Add New Order      → prompt user → enqueue → append to orders.csv
  [2] Process Next Order → full pipeline (see below)
  [3] View Pending Orders
  [4] View Robot Status
  [5] Search Item (Task 4)
  [6] Display Warehouse Tree (Task 5)
  [7] View Movement Log
  [0] Exit

PROCESS NEXT ORDER (Option 2) — Full Pipeline:
  Step 1: Check if Order Queue is empty → if yes, print "No orders" and return
  Step 2: Dequeue front order (Task 1) → update its status to PROCESSING in orders.csv
  Step 3: Call Task 2 → assignNext() → get next AVAILABLE robot
          → update robot status to BUSY in robots.csv
  Step 4: (If Task 4 present) Search BST for ItemID from order
          → retrieve ZoneID, AisleID, ShelfID
  Step 5: (If Task 5 present) Call generateRoute(start, targetShelf) on tree
          → receive list of steps/directions
  Step 6: (Task 3) Push each step onto Path Stack → print forward journey
  Step 7: Task 3 pops all steps → print return journey
  Step 8: Append full path to movement_log.csv
  Step 9: Mark order as COMPLETED in orders.csv
          Mark robot as AVAILABLE in robots.csv

EXIT:
  - Flush any in-memory status updates back to CSV files
  - Print goodbye message
```

### When to Update CSV Files

| Event | File to Update | Field to Change |
|-------|---------------|-----------------|
| New order added | `orders.csv` | New row appended |
| Order dequeued for processing | `orders.csv` | `Status` → `PROCESSING` |
| Order completed | `orders.csv` | `Status` → `COMPLETED` |
| Robot assigned | `robots.csv` | `Status` → `BUSY`, `CurrentOrderID` set |
| Robot returns | `robots.csv` | `Status` → `AVAILABLE`, `TotalTasksDone` +1 |
| Robot finishes path | `movement_log.csv` | All steps appended |
| Item added/removed | `items.csv` | New row / row deleted |

---

## SECTION 7 — PRESENTATION / DEMO GUIDANCE

### What Each Member Should Demonstrate

| Member | Module | What to Show |
|--------|--------|-------------|
| Member 1 (Task 1) | Order Management | Add 3 new orders live. Show queue filling up. Process one order (dequeue). Show empty queue edge case. |
| Member 2 (Task 2) | Robot Assignment | Show 4 robots in circular queue. Assign 3 orders — show rotation. Mark one robot BUSY, show it is skipped. Mark one MAINTENANCE, show permanent skip. |
| Member 3 (Task 3) | Path Tracking | Show a robot moving forward (5+ steps pushed to stack). Show destination reached. Show reverse return (steps popped). Show complete log. |
| Member 4 (Task 4) | Item Search | Insert 5 items into BST. Do in-order traversal. Search for an item by ID. Delete an item. Show updated tree. |
| Member 5 (Task 5) | Warehouse Layout | Build tree from CSV. Display full tree. Generate route from base to a specific shelf. Show BFS or DFS traversal. |

### How to Justify Data Structure Choices in Q&A

**Task 1 — Queue:**
> "I chose a queue because order processing must be fair — first order received should be first processed. This is FIFO behavior. A queue guarantees no order is skipped."

**Task 2 — Circular Queue:**
> "I chose a circular queue because robot assignment must rotate indefinitely without restarting. The tail wraps back to the head using modulo arithmetic, ensuring balanced workload."

**Task 3 — Stack:**
> "I chose a stack because the return journey is the exact reverse of the forward journey. The last step pushed is the first step needed for return — this is LIFO behavior, which is exactly what a stack provides."

**Task 4 — BST:**
> "I chose a BST because items need to be searched quickly by ID. A BST gives O(log n) average search time. In-order traversal also gives a sorted display of all items automatically."

**Task 5 — N-ary Tree:**
> "I chose a tree because the warehouse has a clear parent-child hierarchy: warehouse contains zones, zones contain aisles, aisles contain shelves. A tree naturally represents this structure and makes path traversal straightforward."

---

## SECTION 8 — SUMMARY TABLES

### Quick Reference: Module Summary

| Task | Module Name | Data Structure | Key Constraint | CSV File |
|------|-------------|----------------|----------------|----------|
| 1 | Order Management | Custom Linked-List Queue | FIFO order processing | `orders.csv` |
| 2 | Robot Assignment | Custom Circular Queue | Rotating, never-ending assignment | `robots.csv` |
| 3 | Path Tracking | Custom Stack | LIFO for reverse path | `movement_log.csv` |
| 4 | Item Search | Custom BST | O(log n) search by ID | `items.csv` |
| 5 | Warehouse Layout | Custom N-ary Tree | Hierarchical zone-aisle-shelf structure | `warehouse_layout.csv` |

### File Dependency Map

| CSV File | Created/Loaded By | Read By |
|----------|------------------|---------|
| `orders.csv` | External / Task 1 | Task 1, Task 2 |
| `robots.csv` | External / Task 2 | Task 2, Task 3 |
| `items.csv` | External / Task 4 | Task 4, passed to Task 3/5 |
| `warehouse_layout.csv` | External / Task 5 | Task 5, passed to Task 3 |
| `movement_log.csv` | Task 3 (writes) | Task 3 (reads for display) |

---

## E. HOW MODULES 4 AND 5 CHANGE THE CSV DESIGN

If your team only has 3 members (Tasks 1–3 only), you can skip `items.csv` and `warehouse_layout.csv`, and instead manually input the item location and movement steps in the demo. The system still works, just without automated lookup.

When Tasks 4 and 5 are added:

1. **`items.csv` must be more detailed** — it needs `ZoneID`, `AisleID`, `ShelfID` so Task 4's BST can return a physical location, not just a name.

2. **`warehouse_layout.csv` becomes mandatory** — Task 5 needs structured parent-child data to build its tree. Without it, the tree cannot be built from CSV, and routes cannot be auto-generated.

3. **The link between files becomes critical** — The `ZoneID` in `items.csv` must exactly match the `NodeID` format used in `warehouse_layout.csv`. For example, if `items.csv` says `ZoneID = A`, then the warehouse layout must have a node with `NodeID = Z_A` (or whatever format you choose — just be consistent).

4. **`movement_log.csv` may gain richer data** — If Task 5 generates named route steps (e.g., `"Move to Zone_A"`) instead of just directions (e.g., `"FORWARD"`), the log becomes more meaningful for debugging and demo purposes.

5. **Integration checkpoint in `main.cpp`** — The main program must chain Tasks 4 → 5 → 3 cleanly: search item (Task 4) → get location → find route in tree (Task 5) → pass steps to stack (Task 3). This chain must work as one seamless function call sequence.

---

*End of Development Guide — CT077-3-2-DSTR Lab Evaluation Work #2*
