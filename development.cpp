// ==========================================================
//  Warehouse Inventory & Order Processing
//  Algorithms and Object-Oriented Programming & Lab - Week 7
//  Task: compile it, run it, and find every bug.
//  Be ready to explain each bug, how you proved it, and your fix.
// ==========================================================
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
using namespace std;

struct Item {
    string sku;
    string name;
    int categoryId;
    int priceCents;
    int stock;
    int reorderLevel;
};

struct Category {
    int id;
    string name;
    int parentId;      // 0 means top level
};

struct OrderLine {
    string sku;
    int qty;
};

// ----------------------------------------------------------
//  Helpers
// ----------------------------------------------------------

string formatMoney(int cents) {
    ostringstream out;
    out << '$' << (cents / 100) << '.'
        << setw(2) << setfill('0') << (cents % 100);
    return out.str();
}

// Reduces a price by a whole-number percentage, e.g. 10 means 10% off.
int applyDiscount(int cents, int percent) {
    return cents - cents * (percent / 100);
}

bool needsReorder(const Item& item) {
    return item.stock < item.reorderLevel;
}

void printLine(char symbol, int length) {
    for (int i = 0; i < length; i++) {
        cout << symbol;
    }
    cout << endl;
}

void printHeader(const string& title) {
    cout << endl;
    printLine('=', 56);
    cout << "  " << title << endl;
    printLine('=', 56);
}

// ----------------------------------------------------------
//  Inventory class
// ----------------------------------------------------------

class Inventory {
private:
    vector<Item> items;
    vector<Category> categories;
    vector<string> activityLog;
    int orderCounter;

public:
    Inventory() : orderCounter(0) {}

    void addItem(const Item& item) {
        items.push_back(item);
    }

    void addCategory(const Category& category) {
        categories.push_back(category);
    }

    const Item* findItem(const string& sku) const {
        for (size_t i = 0; i < items.size(); i++) {
            if (items[i].sku == sku) {
                return &items[i];
            }
        }
        return nullptr;
    }

    const Category* findCategory(int id) const {
        for (size_t i = 0; i < categories.size(); i++) {
            if (categories[i].id == id) {
                return &categories[i];
            }
        }
        return nullptr;
    }

    string categoryName(int id) const {
        const Category* c = findCategory(id);
        return (c == nullptr) ? "(unknown)" : c->name;
    }

    // How many levels below the top the category sits (top level = 0).
    int categoryDepth(int id) const {
        const Category* c = findCategory(id);
        if (c == nullptr || c->parentId == 0) {
            return 0;
        }
        return 1 + categoryDepth(c->parentId);
    }

    // Removes qty units from stock. Returns false if there is not enough.
    bool sell(const string& sku, int qty) {
        for (auto item : items) {
            if (item.sku == sku) {
                if (item.stock < qty) {
                    return false;
                }
                item.stock -= qty;
                return true;
            }
        }
        return false;
    }

    int orderTotal(const vector<OrderLine>& order, int discountPercent) const {
        int subtotal = 0;
        for (size_t i = 0; i < order.size(); i++) {
            const Item* it = findItem(order[i].sku);
            if (it != nullptr) {
                subtotal += it->priceCents * order[i].qty;
            }
        }
        return applyDiscount(subtotal, discountPercent);
    }

    // All-or-nothing: either every line can be filled, or nothing is sold.
    bool processOrder(const vector<OrderLine>& order, int discountPercent) {
        orderCounter++;
        string tag = "Order #" + to_string(orderCounter);

        for (size_t i = 0; i < order.size(); i++) {
            const Item* it = findItem(order[i].sku);
            if (it == nullptr) {
                activityLog.push_back(tag + " FAILED: unknown SKU " + order[i].sku);
                cout << tag << " failed: unknown SKU " << order[i].sku << endl;
                return false;
            }
            if (it->stock < order[i].qty {
                activityLog.push_back(tag + " FAILED: not enough stock for " + it->sku);
                cout << tag << " failed: not enough stock for " << it->sku << endl;
                return false;
            }
        }

        int total = orderTotal(order, discountPercent);
        for (size_t i = 0; i < order.size(); i++) {
            sell(order[i].sku, order[i].qty);
        }
        activityLog.push_back(tag + " OK: " + formatMoney(total));
        cout << tag << " accepted, total " << formatMoney(total);
        if (discountPercent > 0) {
            cout << " (" << discountPercent << "% discount)";
        }
        cout << endl;
        return true;
    }

    void printStockTable() const {
        cout << left << setw(8) << "SKU" << setw(15) << "Name"
             << setw(17) << "Category" << right << setw(9) << "Price"
             << setw(7) << "Stock" << endl;
        printLine('-', 56);
        for (size_t i = 0; i < items.size(); i++) {
            const Item& it = items[i];
            cout << left << setw(8) << it.sku << setw(15) << it.name
                 << setw(17) << categoryName(it.categoryId)
                 << right << setw(9) << formatMoney(it.priceCents)
                 << setw(7) << it.stock << endl;
        }
    }

    void printLowStock() const {
        int found = 0;
        for (size_t i = 0; i < items.size(); i++) {
            if (needsReorder(items[i])) {
                cout << "  " << items[i].sku << "  " << items[i].name
                     << "  (stock " << items[i].stock
                     << ", reorder level " << items[i].reorderLevel << ")" << endl;
                found++;
            }
        }
        if (found == 0) {
            cout << "  Nothing needs reordering." << endl;
        }
    }

    void printCategoryDepths() const {
        for (size_t i = 0; i < categories.size(); i++) {
            int depth = categoryDepth(categories[i].id);
            cout << "  " << string(depth * 2, ' ') << categories[i].name
                 << "  (level " << depth << ")" << endl;
        }
    }

    // Prints the activity log, newest entry first.
    void printActivityLog() const {
        for (size_t i = activityLog.size() - 1; i >= 0; i--) {
            cout << "  " << activityLog[i] << endl;
        }
    }
};

// ----------------------------------------------------------
//  Main program
// ----------------------------------------------------------

int main() {
    Inventory inv;

    inv.addCategory({1, "Electronics", 0});
    inv.addCategory({2, "Input Devices", 1});
    inv.addCategory({3, "Displays", 1});
    inv.addCategory({6, "Cables", 7});
    inv.addCategory({7, "Adapters", 6});

    inv.addItem({"KB-100", "Keyboard",     2, 2499, 15, 5});
    inv.addItem({"MS-200", "Mouse",        2, 1299,  8, 6});
    inv.addItem({"MN-300", "Monitor",      3, 15999, 4, 3});
    inv.addItem({"CB-400", "USB Cable",    6,  599, 50, 20});
    inv.addItem({"LS-500", "Laptop Stand", 3, 3499,  6, 2});
    inv.addItem({"WC-600", "Webcam",       2, 4599,  3, 4});

    printHeader("1. STOCK BEFORE ORDERS");
    inv.printStockTable();

    printHeader("2. PROCESSING ORDERS");
    inv.processOrder({{"KB-100", 3}, {"MS-200", 2}}, 10);
    inv.processOrder({{"WC-600", 1}, {"CB-400", 10}}, 0);
    inv.processOrder({{"LS-500", 10}}, 0);
    inv.processOrder({{"XX-999", 1}}, 0);

    printHeader("3. STOCK AFTER ORDERS");
    inv.printStockTable();

    printHeader("4. LOW STOCK REPORT");
    inv.printLowStock();

    printHeader("5. CATEGORY LEVELS");
    inv.printCategoryDepths();

    printHeader("6. ACTIVITY LOG (NEWEST FIRST)");
    inv.printActivityLog();

    return 0;
}
