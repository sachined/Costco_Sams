## 4. Costco_Sams
# 🛒 Costco & Sam's Club — Cash Register Simulation

A C++ simulation of a warehouse store cash register system, complete with barcode scanning, product lookups, and a customer queue — using a range of STL data structures.

## System Overview
```
bc3of9.txt       ← barcode-to-letter key (Code 39 encoding)
Products (XML)   ← product list with price details
Cart (XML)       ← individual cart with product barcodes
Carts (XML)      ← queue of customer carts
    ↓
Cash register logic: scan barcode → look up product → total cart → process queue
```
## Data Structures Used
| Structure | Purpose |
|-----------|---------|
| `std::unordered_map` | Barcode → product lookup (O(1) average) |
| `std::queue` | Customer cart queue (FIFO checkout line) |
| `std::stack` | Item processing stack |
| `std::vector` | Product list storage |
| `std::tuple` | Product detail records (name, price, quantity) |

## Key Concepts
- Barcode decoding using Code 39 key file
- XML data parsing for products and carts
- Multiple STL container types in a single system
- Queue-based customer simulation

## Build
```
bash
g++ -std=c++11 -o register *.cpp
./register
```
