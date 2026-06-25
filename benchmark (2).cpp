#include <algorithm>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace std;
using namespace std::chrono;

struct Product {
    string productId;
    string productName;
    string category;
};

struct TransactionItem {
    string transactionId;
    string customerId;
    string transactionDate;
    string productId;
    string productName;
    string category;
    int quantity;
};

struct ProductFrequency {
    string productId;
    string productName;
    string category;
    int totalQuantity;
};

struct BenchmarkRow {
    int dataSize;
    string structureName;
    string operation;
    double timeMilliseconds;
    size_t memoryBytes;
};

string formatId(const string& prefix, int number, int width) {
    stringstream ss;
    ss << prefix << setw(width) << setfill('0') << number;
    return ss.str();
}

vector<Product> generateProducts(int productCount) {
    vector<Product> products;
    products.reserve(productCount);

    vector<string> categories = {
        "Elektronik", "Aksesoris", "Komputer", "Gaming", "Storage"
    };

    for (int i = 1; i <= productCount; i++) {
        Product p;
        p.productId = formatId("P", i, 3);
        p.productName = "Produk_" + to_string(i);
        p.category = categories[(i - 1) % categories.size()];
        products.push_back(p);
    }

    return products;
}

vector<TransactionItem> generateTransactions(int dataSize, const vector<Product>& products) {
    vector<TransactionItem> transactions;
    transactions.reserve(dataSize);

    const int customerCount = 100;
    const int itemsPerTransaction = 3;

    for (int i = 0; i < dataSize; i++) {
        int transactionNumber = (i / itemsPerTransaction) + 1;
        int customerNumber = (i % customerCount) + 1;
        const Product& product = products[i % products.size()];

        TransactionItem item;
        item.transactionId = formatId("T", transactionNumber, 5);
        item.customerId = formatId("C", customerNumber, 3);
        item.transactionDate = "2026-05-" + formatId("", (i % 28) + 1, 2);
        item.productId = product.productId;
        item.productName = product.productName;
        item.category = product.category;
        item.quantity = (i % 5) + 1;

        transactions.push_back(item);
    }

    return transactions;
}

class VectorManager {
private:
    vector<TransactionItem> transactions;
    vector<Product> products;

public:
    VectorManager(const vector<TransactionItem>& tx, const vector<Product>& prod)
        : transactions(tx), products(prod) {}

    vector<TransactionItem> searchByTransaction(const string& transactionId) const {
        vector<TransactionItem> result;
        for (const auto& item : transactions) {
            if (item.transactionId == transactionId) result.push_back(item);
        }
        return result;
    }

    vector<TransactionItem> searchByCustomer(const string& customerId) const {
        vector<TransactionItem> result;
        for (const auto& item : transactions) {
            if (item.customerId == customerId) result.push_back(item);
        }
        return result;
    }

    vector<TransactionItem> searchByProduct(const string& productId) const {
        vector<TransactionItem> result;
        for (const auto& item : transactions) {
            if (item.productId == productId) result.push_back(item);
        }
        return result;
    }

    void insertTransaction(const TransactionItem& item) {
        transactions.push_back(item);
    }

    bool updateTransactionQuantity(const string& transactionId, const string& productId, int newQuantity) {
        for (auto& item : transactions) {
            if (item.transactionId == transactionId && item.productId == productId) {
                item.quantity = newQuantity;
                return true;
            }
        }
        return false;
    }

    bool deleteTransactionItem(const string& transactionId, const string& productId) {
        for (auto it = transactions.begin(); it != transactions.end(); ++it) {
            if (it->transactionId == transactionId && it->productId == productId) {
                transactions.erase(it);
                return true;
            }
        }
        return false;
    }

    vector<ProductFrequency> topNProducts(int n) const {
        vector<ProductFrequency> frequencies;

        for (const auto& item : transactions) {
            bool found = false;
            for (auto& freq : frequencies) {
                if (freq.productId == item.productId) {
                    freq.totalQuantity += item.quantity;
                    found = true;
                    break;
                }
            }

            if (!found) {
                ProductFrequency freq;
                freq.productId = item.productId;
                freq.productName = item.productName;
                freq.category = item.category;
                freq.totalQuantity = item.quantity;
                frequencies.push_back(freq);
            }
        }

        sort(frequencies.begin(), frequencies.end(), [](const ProductFrequency& a, const ProductFrequency& b) {
            return a.totalQuantity > b.totalQuantity;
        });

        if (static_cast<int>(frequencies.size()) > n) frequencies.resize(n);
        return frequencies;
    }

    vector<pair<string, int>> frequentlyBoughtTogether(const string& baseProductId, int n) const {
        unordered_set<string> matchingTransactions;
        for (const auto& item : transactions) {
            if (item.productId == baseProductId) matchingTransactions.insert(item.transactionId);
        }

        vector<pair<string, int>> result;
        for (const auto& item : transactions) {
            if (item.productId == baseProductId) continue;
            if (matchingTransactions.count(item.transactionId) == 0) continue;

            bool found = false;
            for (auto& entry : result) {
                if (entry.first == item.productId) {
                    entry.second++;
                    found = true;
                    break;
                }
            }

            if (!found) result.push_back({item.productId, 1});
        }

        sort(result.begin(), result.end(), [](const auto& a, const auto& b) {
            return a.second > b.second;
        });

        if (static_cast<int>(result.size()) > n) result.resize(n);
        return result;
    }

    size_t estimateMemoryBytes() const {
        return transactions.size() * sizeof(TransactionItem) + products.size() * sizeof(Product);
    }
};

class HashMapManager {
private:
    vector<TransactionItem> transactions;
    vector<Product> products;
    unordered_map<string, vector<int>> indexByTransaction;
    unordered_map<string, vector<int>> indexByCustomer;
    unordered_map<string, vector<int>> indexByProduct;
    unordered_map<string, int> indexProductById;
    unordered_map<string, int> productFrequency;
    unordered_map<string, vector<string>> transactionProducts;

    void rebuildIndexes() {
        indexByTransaction.clear();
        indexByCustomer.clear();
        indexByProduct.clear();
        indexProductById.clear();
        productFrequency.clear();
        transactionProducts.clear();

        for (int i = 0; i < static_cast<int>(products.size()); i++) {
            indexProductById[products[i].productId] = i;
        }

        for (int i = 0; i < static_cast<int>(transactions.size()); i++) {
            const auto& item = transactions[i];
            indexByTransaction[item.transactionId].push_back(i);
            indexByCustomer[item.customerId].push_back(i);
            indexByProduct[item.productId].push_back(i);
            productFrequency[item.productId] += item.quantity;
            transactionProducts[item.transactionId].push_back(item.productId);
        }
    }

public:
    HashMapManager(const vector<TransactionItem>& tx, const vector<Product>& prod)
        : transactions(tx), products(prod) {
        rebuildIndexes();
    }

    vector<TransactionItem> searchByTransaction(const string& transactionId) const {
        vector<TransactionItem> result;
        auto it = indexByTransaction.find(transactionId);
        if (it == indexByTransaction.end()) return result;

        for (int index : it->second) result.push_back(transactions[index]);
        return result;
    }

    vector<TransactionItem> searchByCustomer(const string& customerId) const {
        vector<TransactionItem> result;
        auto it = indexByCustomer.find(customerId);
        if (it == indexByCustomer.end()) return result;

        for (int index : it->second) result.push_back(transactions[index]);
        return result;
    }

    vector<TransactionItem> searchByProduct(const string& productId) const {
        vector<TransactionItem> result;
        auto it = indexByProduct.find(productId);
        if (it == indexByProduct.end()) return result;

        for (int index : it->second) result.push_back(transactions[index]);
        return result;
    }

    void insertTransaction(const TransactionItem& item) {
        int index = static_cast<int>(transactions.size());
        transactions.push_back(item);
        indexByTransaction[item.transactionId].push_back(index);
        indexByCustomer[item.customerId].push_back(index);
        indexByProduct[item.productId].push_back(index);
        productFrequency[item.productId] += item.quantity;
        transactionProducts[item.transactionId].push_back(item.productId);
    }

    bool updateTransactionQuantity(const string& transactionId, const string& productId, int newQuantity) {
        auto it = indexByTransaction.find(transactionId);
        if (it == indexByTransaction.end()) return false;

        for (int index : it->second) {
            TransactionItem& item = transactions[index];
            if (item.productId == productId) {
                productFrequency[item.productId] -= item.quantity;
                item.quantity = newQuantity;
                productFrequency[item.productId] += item.quantity;
                return true;
            }
        }
        return false;
    }

    bool deleteTransactionItem(const string& transactionId, const string& productId) {
        auto it = indexByTransaction.find(transactionId);
        if (it == indexByTransaction.end()) return false;

        for (int index : it->second) {
            if (transactions[index].productId == productId) {
                transactions.erase(transactions.begin() + index);
                rebuildIndexes();
                return true;
            }
        }
        return false;
    }

    vector<ProductFrequency> topNProducts(int n) const {
        vector<ProductFrequency> frequencies;
        frequencies.reserve(productFrequency.size());

        for (const auto& entry : productFrequency) {
            ProductFrequency freq;
            freq.productId = entry.first;
            freq.totalQuantity = entry.second;

            auto productIt = indexProductById.find(entry.first);
            if (productIt != indexProductById.end()) {
                const Product& p = products[productIt->second];
                freq.productName = p.productName;
                freq.category = p.category;
            } else {
                freq.productName = "-";
                freq.category = "-";
            }

            frequencies.push_back(freq);
        }

        sort(frequencies.begin(), frequencies.end(), [](const ProductFrequency& a, const ProductFrequency& b) {
            return a.totalQuantity > b.totalQuantity;
        });

        if (static_cast<int>(frequencies.size()) > n) frequencies.resize(n);
        return frequencies;
    }

    vector<pair<string, int>> frequentlyBoughtTogether(const string& baseProductId, int n) const {
        vector<pair<string, int>> result;
        unordered_map<string, int> togetherFrequency;

        auto productIt = indexByProduct.find(baseProductId);
        if (productIt == indexByProduct.end()) return result;

        unordered_set<string> transactionSeen;
        for (int index : productIt->second) {
            transactionSeen.insert(transactions[index].transactionId);
        }

        for (const string& transactionId : transactionSeen) {
            auto txIt = transactionProducts.find(transactionId);
            if (txIt == transactionProducts.end()) continue;

            unordered_set<string> uniqueProductsInTransaction;
            for (const string& otherProductId : txIt->second) {
                if (otherProductId != baseProductId) {
                    uniqueProductsInTransaction.insert(otherProductId);
                }
            }

            for (const string& otherProductId : uniqueProductsInTransaction) {
                togetherFrequency[otherProductId]++;
            }
        }

        for (const auto& entry : togetherFrequency) {
            result.push_back({entry.first, entry.second});
        }

        sort(result.begin(), result.end(), [](const auto& a, const auto& b) {
            return a.second > b.second;
        });

        if (static_cast<int>(result.size()) > n) result.resize(n);
        return result;
    }

    size_t estimateMemoryBytes() const {
        size_t memory = transactions.size() * sizeof(TransactionItem) + products.size() * sizeof(Product);
        memory += indexByTransaction.size() * sizeof(pair<const string, vector<int>>);
        memory += indexByCustomer.size() * sizeof(pair<const string, vector<int>>);
        memory += indexByProduct.size() * sizeof(pair<const string, vector<int>>);
        memory += indexProductById.size() * sizeof(pair<const string, int>);
        memory += productFrequency.size() * sizeof(pair<const string, int>);
        memory += transactionProducts.size() * sizeof(pair<const string, vector<string>>);
        return memory;
    }
};

template <typename Func>
double measureMilliseconds(Func func) {
    auto start = high_resolution_clock::now();
    func();
    auto end = high_resolution_clock::now();
    return duration<double, milli>(end - start).count();
}

void addRow(vector<BenchmarkRow>& rows, int dataSize, const string& structureName,
            const string& operation, double timeMs, size_t memoryBytes) {
    rows.push_back({dataSize, structureName, operation, timeMs, memoryBytes});
}

void writeResultsCSV(const vector<BenchmarkRow>& rows, const string& filename) {
    ofstream file(filename);
    file << fixed << setprecision(6);
    file << "data_size,structure,operation,time_ms,memory_bytes\n";
    for (const auto& row : rows) {
        file << row.dataSize << ","
             << row.structureName << ","
             << row.operation << ","
             << row.timeMilliseconds << ","
             << row.memoryBytes << "\n";
    }
    file.close();
}

void printSummary(const vector<BenchmarkRow>& rows) {
    cout << left << setw(10) << "Data"
         << setw(16) << "Struktur"
         << setw(30) << "Operasi"
         << setw(16) << "Waktu(ms)"
         << setw(16) << "Memori(byte)" << "\n";
    cout << string(88, '-') << "\n";

    for (const auto& row : rows) {
        cout << left << setw(10) << row.dataSize
             << setw(16) << row.structureName
             << setw(30) << row.operation
             << setw(16) << row.timeMilliseconds
             << setw(16) << row.memoryBytes << "\n";
    }
}


const BenchmarkRow* findBenchmarkRow(const vector<BenchmarkRow>& rows, int dataSize,
                                    const string& structureName, const string& operation) {
    for (const auto& row : rows) {
        if (row.dataSize == dataSize && row.structureName == structureName && row.operation == operation) {
            return &row;
        }
    }
    return nullptr;
}

void writeSideBySideCSV(const vector<BenchmarkRow>& rows,
                        const vector<int>& dataSizes,
                        const vector<string>& operations,
                        const string& filename) {
    ofstream file(filename);
    file << fixed << setprecision(6);
    file << "data_size,operation,vector_time_ms,unordered_map_time_ms,";
    file << "faster_structure,time_difference_ms,vector_memory_bytes,unordered_map_memory_bytes,";
    file << "lower_memory_structure,memory_difference_bytes\n";

    for (int dataSize : dataSizes) {
        for (const string& operation : operations) {
            const BenchmarkRow* vectorRow = findBenchmarkRow(rows, dataSize, "vector", operation);
            const BenchmarkRow* mapRow = findBenchmarkRow(rows, dataSize, "unordered_map", operation);
            if (vectorRow == nullptr || mapRow == nullptr) continue;

            string fasterStructure = "seri";
            if (vectorRow->timeMilliseconds < mapRow->timeMilliseconds) fasterStructure = "vector";
            else if (mapRow->timeMilliseconds < vectorRow->timeMilliseconds) fasterStructure = "unordered_map";

            string lowerMemoryStructure = "seri";
            if (vectorRow->memoryBytes < mapRow->memoryBytes) lowerMemoryStructure = "vector";
            else if (mapRow->memoryBytes < vectorRow->memoryBytes) lowerMemoryStructure = "unordered_map";

            double timeDifference = vectorRow->timeMilliseconds > mapRow->timeMilliseconds
                ? vectorRow->timeMilliseconds - mapRow->timeMilliseconds
                : mapRow->timeMilliseconds - vectorRow->timeMilliseconds;
            long long memoryDifference = static_cast<long long>(vectorRow->memoryBytes > mapRow->memoryBytes
                ? vectorRow->memoryBytes - mapRow->memoryBytes
                : mapRow->memoryBytes - vectorRow->memoryBytes);

            file << dataSize << ","
                 << operation << ","
                 << vectorRow->timeMilliseconds << ","
                 << mapRow->timeMilliseconds << ","
                 << fasterStructure << ","
                 << timeDifference << ","
                 << vectorRow->memoryBytes << ","
                 << mapRow->memoryBytes << ","
                 << lowerMemoryStructure << ","
                 << memoryDifference << "\n";
        }
    }
    file.close();
}

void printSideBySideSummary(const vector<BenchmarkRow>& rows,
                            const vector<int>& dataSizes,
                            const vector<string>& operations) {
    cout << fixed << setprecision(6);
    cout << "\n=== TABEL PERBANDINGAN SEBELAHAN ===\n";
    cout << left << setw(10) << "Data"
         << setw(30) << "Operasi"
         << setw(16) << "Vector(ms)"
         << setw(20) << "Unordered(ms)"
         << setw(18) << "Lebih cepat"
         << setw(18) << "Vector(byte)"
         << setw(18) << "Unordered(byte)" << "\n";
    cout << string(130, '-') << "\n";

    for (int dataSize : dataSizes) {
        for (const string& operation : operations) {
            const BenchmarkRow* vectorRow = findBenchmarkRow(rows, dataSize, "vector", operation);
            const BenchmarkRow* mapRow = findBenchmarkRow(rows, dataSize, "unordered_map", operation);
            if (vectorRow == nullptr || mapRow == nullptr) continue;

            string fasterStructure = "seri";
            if (vectorRow->timeMilliseconds < mapRow->timeMilliseconds) fasterStructure = "vector";
            else if (mapRow->timeMilliseconds < vectorRow->timeMilliseconds) fasterStructure = "unordered_map";

            cout << left << setw(10) << dataSize
                 << setw(30) << operation
                 << setw(16) << vectorRow->timeMilliseconds
                 << setw(20) << mapRow->timeMilliseconds
                 << setw(18) << fasterStructure
                 << setw(18) << vectorRow->memoryBytes
                 << setw(18) << mapRow->memoryBytes << "\n";
        }
    }
}

int main() {
    vector<int> dataSizes = {1000, 5000, 10000, 50000};
    vector<string> operations = {
        "build_or_load",
        "search_transaction",
        "search_customer",
        "search_product",
        "insert_transaction",
        "update_transaction",
        "delete_transaction",
        "top_n_products",
        "frequently_bought_together"
    };
    const int productCount = 100;
    const int repeatSearch = 200;
    vector<BenchmarkRow> rows;

    for (int dataSize : dataSizes) {
        vector<Product> products = generateProducts(productCount);
        vector<TransactionItem> transactions = generateTransactions(dataSize, products);

        string searchTransactionId = formatId("T", (dataSize / 6), 5);
        string searchCustomerId = "C050";
        string searchProductId = "P050";

        TransactionItem newItem;
        newItem.transactionId = formatId("T", dataSize + 1, 5);
        newItem.customerId = "C001";
        newItem.transactionDate = "2026-05-28";
        newItem.productId = "P001";
        newItem.productName = "Produk_1";
        newItem.category = "Elektronik";
        newItem.quantity = 3;

        {
            double buildTime = measureMilliseconds([&]() {
                VectorManager manager(transactions, products);
                (void)manager.estimateMemoryBytes();
            });
            VectorManager manager(transactions, products);
            size_t memory = manager.estimateMemoryBytes();

            addRow(rows, dataSize, "vector", "build_or_load", buildTime, memory);
            addRow(rows, dataSize, "vector", "search_transaction", measureMilliseconds([&]() {
                for (int i = 0; i < repeatSearch; i++) manager.searchByTransaction(searchTransactionId);
            }), memory);
            addRow(rows, dataSize, "vector", "search_customer", measureMilliseconds([&]() {
                for (int i = 0; i < repeatSearch; i++) manager.searchByCustomer(searchCustomerId);
            }), memory);
            addRow(rows, dataSize, "vector", "search_product", measureMilliseconds([&]() {
                for (int i = 0; i < repeatSearch; i++) manager.searchByProduct(searchProductId);
            }), memory);
            addRow(rows, dataSize, "vector", "insert_transaction", measureMilliseconds([&]() {
                manager.insertTransaction(newItem);
            }), manager.estimateMemoryBytes());
            addRow(rows, dataSize, "vector", "update_transaction", measureMilliseconds([&]() {
                manager.updateTransactionQuantity(searchTransactionId, searchProductId, 9);
            }), manager.estimateMemoryBytes());
            addRow(rows, dataSize, "vector", "delete_transaction", measureMilliseconds([&]() {
                manager.deleteTransactionItem(searchTransactionId, searchProductId);
            }), manager.estimateMemoryBytes());
            addRow(rows, dataSize, "vector", "top_n_products", measureMilliseconds([&]() {
                manager.topNProducts(10);
            }), manager.estimateMemoryBytes());
            addRow(rows, dataSize, "vector", "frequently_bought_together", measureMilliseconds([&]() {
                manager.frequentlyBoughtTogether(searchProductId, 10);
            }), manager.estimateMemoryBytes());
        }

        {
            double buildTime = measureMilliseconds([&]() {
                HashMapManager manager(transactions, products);
                (void)manager.estimateMemoryBytes();
            });
            HashMapManager manager(transactions, products);
            size_t memory = manager.estimateMemoryBytes();

            addRow(rows, dataSize, "unordered_map", "build_or_load", buildTime, memory);
            addRow(rows, dataSize, "unordered_map", "search_transaction", measureMilliseconds([&]() {
                for (int i = 0; i < repeatSearch; i++) manager.searchByTransaction(searchTransactionId);
            }), memory);
            addRow(rows, dataSize, "unordered_map", "search_customer", measureMilliseconds([&]() {
                for (int i = 0; i < repeatSearch; i++) manager.searchByCustomer(searchCustomerId);
            }), memory);
            addRow(rows, dataSize, "unordered_map", "search_product", measureMilliseconds([&]() {
                for (int i = 0; i < repeatSearch; i++) manager.searchByProduct(searchProductId);
            }), memory);
            addRow(rows, dataSize, "unordered_map", "insert_transaction", measureMilliseconds([&]() {
                manager.insertTransaction(newItem);
            }), manager.estimateMemoryBytes());
            addRow(rows, dataSize, "unordered_map", "update_transaction", measureMilliseconds([&]() {
                manager.updateTransactionQuantity(searchTransactionId, searchProductId, 9);
            }), manager.estimateMemoryBytes());
            addRow(rows, dataSize, "unordered_map", "delete_transaction", measureMilliseconds([&]() {
                manager.deleteTransactionItem(searchTransactionId, searchProductId);
            }), manager.estimateMemoryBytes());
            addRow(rows, dataSize, "unordered_map", "top_n_products", measureMilliseconds([&]() {
                manager.topNProducts(10);
            }), manager.estimateMemoryBytes());
            addRow(rows, dataSize, "unordered_map", "frequently_bought_together", measureMilliseconds([&]() {
                manager.frequentlyBoughtTogether(searchProductId, 10);
            }), manager.estimateMemoryBytes());
        }
    }

    writeResultsCSV(rows, "benchmark_vector_vs_unordered_map_results.csv");
    writeSideBySideCSV(rows, dataSizes, operations, "benchmark_side_by_side_results.csv");

    printSideBySideSummary(rows, dataSizes, operations);
    cout << "\nHasil format panjang disimpan ke benchmark_vector_vs_unordered_map_results.csv\n";
    cout << "Hasil tabel sebelahan disimpan ke benchmark_side_by_side_results.csv\n";

    return 0;
}
