# 🍔 Restaurant Management System (Discrete-Event Simulation)

A high-performance C++ discrete-event simulation of a restaurant's end-to-end order management system. The system dynamically orchestrates multiple order types, physical resource constraints (chefs, tables, and delivery scooters), and operational priorities using advanced data structures and deterministic scheduling algorithms.

---

## 📌 Project Overview & Timeline
The simulation models the complete lifecycle of an order across two major constraint satisfaction stages: **Chef Assignment** (Preparation) and **Service Assignment** (Dining/Delivery). The architecture operates on incremental discrete time steps (1 unit time each) to monitor resource utilization and system efficiency.

### 🕒 Key Performance Metrics Tracked
* **Idle Time 1:** (TA - TQ) -> Time spent waiting for an available chef.
* **Order Cook Period:** (TC = TR - TA) -> Active cooking time.
* **Idle Time 2:** (TS - TR) -> Time spent waiting for a table or scooter.
* **Service Duration:** (Tserv = TF - TS) -> Active delivery or dine-in consumption time.

---

## 🛠️ System Entities & Constraints

### 1. Orders
* **Dine-in (OD):** Divided into **Grilled (ODG)** (requires Special Chefs) and **Normal (ODN)**. Requires a matching table capacity and specific dining duration. Supports table-sharing opt-ins.
* **Takeaway (OT):** Collected on-site by customers after a fixed 1-timestep packing delay.
* **Delivery (OV):** Categorized into **Cold (OVC)** (supports absolute cancellation), **Grilled (OVG)** (priority-driven), and **Normal (OVN)**.

### 2. Resources
* **Chefs:** **Special Chefs (CS)** handle grilled orders; **Normal Chefs (CN)** handle standard preparations. Process efficiency is regulated by programmatic speed metrics (dishes per timestep).
* **Scooters:** Speed-rated delivery units. Undergoes cyclic maintenance (Main Dur) after completing a configured threshold of deliveries (Main Ords).
* **Tables:** Variable-capacity physical units inside the restaurant.

---

## 🏗️ Data Structures & Architecture

The core engineering challenge relies on leveraging the optimal Abstract Data Type (ADT) for each subsystem queue:

* **Actions Queue:** Chronological order requests and cancellations -> **Queue (FIFO)**
* **Pending Normal Orders (ODN, OT, OVC, OVN):** First-Come, First-Served basis -> **Queue (FIFO)**
* **Pending Grilled Orders (OVG):** Weighted priority equation (Price, Size, Distance) -> **Priority Queue (Max-Heap)**
* **Ready Dine-In Orders:** Best-fit matching based on required seats -> **Balanced BST / Sorted List**
* **Ready Delivery (OVC first):** Strict OVC priority, then standard FCFS -> **Priority Queue / Multi-level Queue**
* **Available Tables:** Capacity and minimum available seats filtering -> **Min-Heap or Sorted Array**
* **Available Scooters:** Shorter cumulative tracking distance priority -> **Priority Queue (Min-Heap)**
* **Active Resources (Cooking/Service):** Deterministic remaining time-to-finish tracking -> **Singly / Doubly Linked List**

### ⚡ Operational Logic & Edge Cases
* **Order Cancellation:** OVC cancellation requests immediately stop cooking progression, flush the order from the hierarchy, and liberate the assigned chef instantly.
* **Table Sharing:** Implements a Best-Fit memory block assignment heuristic to optimize table utilization before allocating isolated tables.

---

## 🚀 Advanced Features Implemented (Bonus Subsystems)

* **OVG Overwait Escalation:** Handles OVG items exceeding the overwait threshold (TH) by pushing them into a high-priority eviction queue to minimize cold-food metrics.
* **Rescue Scooter Protocol:** Models mechanical hardware degradation. If a delivery scooter fails mid-transit, a rescue unit intercepts the breakdown location to finish delivery while routing the failed asset to maintenance.
* **COMBO Parallel Orders:** Multi-chef tasks (up to 4 chefs simultaneously) that require bulk delivery logistics (2+ scooters), prioritized across all system queues.

---

## 💻 Interface & Execution Modes

The engine supports dual executing modes depending on the runtime flags:

1. **Interactive Mode:** Frame-by-frame console animation displaying exact system snapshots (Pending counts, Chef assignments, Cooking matrix, Available scooters, and live In-Service tracking) per timestep.
2. **Silent Mode:** Highly optimized execution bypassing IO rendering bottleneck to strictly dump telemetry and statistical analytical reporting into an external text file.

### 📊 Telemetry Metrics Exported
* Total execution count broken down per order, chef, and scooter classifications.
* Percentage metrics tracking Successful completions vs. Cancellations and Overwait violations.
* System Efficiency Ratios: **Chefs Utilization %** and **Scooters Utilization %** over total simulation runtime.

---
