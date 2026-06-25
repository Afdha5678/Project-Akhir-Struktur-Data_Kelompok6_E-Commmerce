#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cctype>
#include <algorithm>

using namespace std;

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

class TransactionManager {
private:
    vector<TransactionItem> transactions;
    vector<Product> products;

    string transactionFilename;
    string productFilename;

public:
    TransactionManager(const string& transactionFile, const string& productFile)
        : transactionFilename(transactionFile), productFilename(productFile) {}

    vector<string> splitCSVLine(const string& line) {
        vector<string> parts;
        string part;
        stringstream ss(line);

        while (getline(ss, part, ',')) {
            parts.push_back(part);
        }

        return parts;
    }

    bool isNumberString(const string& s) {
        if (s.empty()) return false;
        for (char c : s) {
            if (!isdigit(static_cast<unsigned char>(c))) return false;
        }
        return true;
    }

    string formatId(const string& prefix, int number, int width) {
        stringstream ss;
        ss << prefix << setw(width) << setfill('0') << number;
        return ss.str();
    }

    string inputFormattedId(const string& label, const string& prefix, int width) {
        string raw;
        while (true) {
            cout << label;
            getline(cin, raw);

            if (!isNumberString(raw)) {
                cout << "Input harus berupa angka. Coba lagi.\n";
                continue;
            }

            int number = stoi(raw);
            if (number <= 0) {
                cout << "ID harus lebih besar dari 0. Coba lagi.\n";
                continue;
            }

            return formatId(prefix, number, width);
        }
    }

    bool isValidDateFormat(const string& date) {
        if (date.length() != 10) return false;
        if (date[4] != '-' || date[7] != '-') return false;

        for (int i = 0; i < 10; i++) {
            if (i == 4 || i == 7) continue;
            if (!isdigit(static_cast<unsigned char>(date[i]))) return false;
        }

        int year = stoi(date.substr(0, 4));
        int month = stoi(date.substr(5, 2));
        int day = stoi(date.substr(8, 2));

        if (year <= 0) return false;
        if (month < 1 || month > 12) return false;
        if (day < 1 || day > 31) return false;

        return true;
    }

    string inputValidDate() {
        string date;
        while (true) {
            cout << "Masukkan tanggal transaksi (format: YYYY-MM-DD): ";
            getline(cin, date);

            if (isValidDateFormat(date)) {
                return date;
            }

            cout << "Format tanggal salah. Gunakan format YYYY-MM-DD.\n";
        }
    }

    int inputQuantity() {
        string raw;
        while (true) {
            cout << "Masukkan quantity: ";
            getline(cin, raw);

            if (!isNumberString(raw)) {
                cout << "Quantity harus berupa angka.\n";
                continue;
            }

            int qty = stoi(raw);
            if (qty <= 0) {
                cout << "Quantity harus lebih besar dari 0.\n";
                continue;
            }

            return qty;
        }
    }

    string trim(const string& s) {
        size_t start = 0;
        while (start < s.size() && isspace(static_cast<unsigned char>(s[start]))) {
            start++;
        }

        size_t end = s.size();
        while (end > start && isspace(static_cast<unsigned char>(s[end - 1]))) {
            end--;
        }

        return s.substr(start, end - start);
    }

    string inputNonEmptyString(const string& label) {
        string value;
        while (true) {
            cout << label;
            getline(cin, value);

            if (!trim(value).empty()) {
                return trim(value);
            }

            cout << "Input tidak boleh kosong. Coba lagi.\n";
        }
    }

    int inputMenuChoice() {
        string raw;
        while (true) {
            cout << "Pilih: ";
            getline(cin, raw);

            if (!isNumberString(raw)) {
                cout << "Masukkan input yang benar.\n";
                continue;
            }

            int choice = stoi(raw);
            if (choice < 1 || choice > 13) {
                cout << "Masukkan input yang benar.\n";
                continue;
            }

            return choice;
        }
    }

    string toLowerCase(const string& s) {
        string result = s;
        for (char& c : result) {
            c = static_cast<char>(tolower(static_cast<unsigned char>(c)));
        }
        return result;
    }

    string normalizeProductName(const string& name) {
        return toLowerCase(trim(name));
    }

    void printTransactionHeader() {
        cout << left
             << setw(10) << "TransID"
             << setw(10) << "CustID"
             << setw(14) << "Date"
             << setw(10) << "ProdID"
             << setw(18) << "Product"
             << setw(15) << "Category"
             << setw(8)  << "Qty" << "\n";

        cout << string(85, '-') << "\n";
    }

    void printTransactionItem(const TransactionItem& item) {
        cout << left
             << setw(10) << item.transactionId
             << setw(10) << item.customerId
             << setw(14) << item.transactionDate
             << setw(10) << item.productId
             << setw(18) << item.productName
             << setw(15) << item.category
             << setw(8)  << item.quantity << "\n";
    }

    void printProductHeader() {
        cout << left
             << setw(10) << "ProdID"
             << setw(20) << "Product"
             << setw(15) << "Category" << "\n";

        cout << string(45, '-') << "\n";
    }

    void printProduct(const Product& p) {
        cout << left
             << setw(10) << p.productId
             << setw(20) << p.productName
             << setw(15) << p.category << "\n";
    }

    bool loadProductsFromFile() {
        ifstream file(productFilename);
        if (!file.is_open()) {
            return false;
        }

        products.clear();
        string line;

        while (getline(file, line)) {
            if (line.empty()) continue;

            vector<string> parts = splitCSVLine(line);
            if (parts.size() != 3) continue;

            Product p;
            p.productId = trim(parts[0]);
            p.productName = trim(parts[1]);
            p.category = trim(parts[2]);

            products.push_back(p);
        }

        file.close();
        return true;
    }

    bool saveProductsToFile() {
        ofstream file(productFilename);
        if (!file.is_open()) {
            return false;
        }

        for (const auto& p : products) {
            file << p.productId << ","
                 << p.productName << ","
                 << p.category << "\n";
        }

        file.close();
        return true;
    }

    Product* findProductById(const string& productId) {
        for (auto& p : products) {
            if (p.productId == productId) {
                return &p;
            }
        }
        return nullptr;
    }

    Product* findProductByNameNormalized(const string& productName) {
        string target = normalizeProductName(productName);

        for (auto& p : products) {
            if (normalizeProductName(p.productName) == target) {
                return &p;
            }
        }

        return nullptr;
    }

    void displayAllProducts() {
        if (products.empty()) {
            cout << "Master produk kosong.\n";
            return;
        }

        printProductHeader();
        for (const auto& p : products) {
            printProduct(p);
        }
    }

    bool loadTransactionsFromFile() {
        ifstream file(transactionFilename);
        if (!file.is_open()) {
            return false;
        }

        transactions.clear();
        string line;

        while (getline(file, line)) {
            if (line.empty()) continue;

            vector<string> parts = splitCSVLine(line);
            if (parts.size() != 7) continue;

            TransactionItem item;
            item.transactionId   = trim(parts[0]);
            item.customerId      = trim(parts[1]);
            item.transactionDate = trim(parts[2]);
            item.productId       = trim(parts[3]);
            item.productName     = trim(parts[4]);
            item.category        = trim(parts[5]);
            item.quantity        = stoi(trim(parts[6]));

            transactions.push_back(item);
        }

        file.close();
        return true;
    }

    bool saveTransactionsToFile() {
        ofstream file(transactionFilename);
        if (!file.is_open()) {
            return false;
        }

        for (const auto& item : transactions) {
            file << item.transactionId << ","
                 << item.customerId << ","
                 << item.transactionDate << ","
                 << item.productId << ","
                 << item.productName << ","
                 << item.category << ","
                 << item.quantity << "\n";
        }

        file.close();
        return true;
    }

    bool saveAllData() {
        bool productSaved = saveProductsToFile();
        bool transactionSaved = saveTransactionsToFile();
        return productSaved && transactionSaved;
    }

    bool loadAllData() {
        bool productLoaded = loadProductsFromFile();
        bool transactionLoaded = loadTransactionsFromFile();
        return productLoaded && transactionLoaded;
    }

    void displayAllTransactions() {
        if (transactions.empty()) {
            cout << "Data transaksi kosong.\n";
            return;
        }

        printTransactionHeader();
        for (const auto& item : transactions) {
            printTransactionItem(item);
        }
    }

    void displaySearchResult(const vector<TransactionItem>& result) {
        if (result.empty()) {
            cout << "Data tidak ditemukan.\n";
            return;
        }

        printTransactionHeader();
        for (const auto& item : result) {
            printTransactionItem(item);
        }
    }

    vector<TransactionItem> searchByTransaction(const string& transactionId) {
        vector<TransactionItem> result;

        for (const auto& item : transactions) {
            if (item.transactionId == transactionId) {
                result.push_back(item);
            }
        }

        return result;
    }

    vector<TransactionItem> searchByCustomer(const string& customerId) {
        vector<TransactionItem> result;

        for (const auto& item : transactions) {
            if (item.customerId == customerId) {
                result.push_back(item);
            }
        }

        return result;
    }

    vector<TransactionItem> searchByProduct(const string& productId) {
        vector<TransactionItem> result;

        for (const auto& item : transactions) {
            if (item.productId == productId) {
                result.push_back(item);
            }
        }

        return result;
    }

    bool transactionExists(const string& transactionId) {
        for (const auto& item : transactions) {
            if (item.transactionId == transactionId) {
                return true;
            }
        }
        return false;
    }

    TransactionItem* getFirstTransactionItem(const string& transactionId) {
        for (auto& item : transactions) {
            if (item.transactionId == transactionId) {
                return &item;
            }
        }
        return nullptr;
    }

    TransactionItem* findExactTransactionProduct(const string& transactionId, const string& productId) {
        for (auto& item : transactions) {
            if (item.transactionId == transactionId && item.productId == productId) {
                return &item;
            }
        }
        return nullptr;
    }


    bool inputYesNo(const string& label) {
        string answer;
        while (true) {
            cout << label;
            getline(cin, answer);
            answer = toLowerCase(trim(answer));

            if (answer == "y" || answer == "ya") return true;
            if (answer == "n" || answer == "no" || answer == "tidak" || answer == "t") return false;

            cout << "Masukkan y/ya atau n/tidak.\n";
        }
    }

    bool containsString(const vector<string>& data, const string& value) {
        for (const string& item : data) {
            if (item == value) return true;
        }
        return false;
    }

    int inputPositiveNumber(const string& label) {
        string raw;
        while (true) {
            cout << label;
            getline(cin, raw);

            if (!isNumberString(raw)) {
                cout << "Input harus berupa angka.\n";
                continue;
            }

            int number = stoi(raw);
            if (number <= 0) {
                cout << "Input harus lebih besar dari 0.\n";
                continue;
            }

            return number;
        }
    }

    void updateTransactionInteractive() {
        cout << "\n=== UPDATE ITEM TRANSAKSI ===\n";

        string transactionId = inputFormattedId("Masukkan ID transaksi yang ingin diupdate (angka saja): ", "T", 4);
        string productId = inputFormattedId("Masukkan ID produk pada transaksi tersebut (angka saja): ", "P", 3);

        TransactionItem* item = findExactTransactionProduct(transactionId, productId);

        if (item == nullptr) {
            cout << "Item transaksi tidak ditemukan.\n";
            return;
        }

        cout << "\nData saat ini:\n";
        printTransactionHeader();
        printTransactionItem(*item);

        int newQuantity = inputQuantity();
        item->quantity = newQuantity;

        if (inputYesNo("Apakah tanggal transaksi juga ingin diubah? (y/n): ")) {
            item->transactionDate = inputValidDate();
        }

        cout << "Item transaksi berhasil diupdate.\n";
    }

    void deleteTransactionInteractive() {
        cout << "\n=== DELETE ITEM TRANSAKSI ===\n";

        string transactionId = inputFormattedId("Masukkan ID transaksi yang ingin dihapus (angka saja): ", "T", 4);
        string productId = inputFormattedId("Masukkan ID produk pada transaksi tersebut (angka saja): ", "P", 3);

        for (auto it = transactions.begin(); it != transactions.end(); ++it) {
            if (it->transactionId == transactionId && it->productId == productId) {
                cout << "\nData yang akan dihapus:\n";
                printTransactionHeader();
                printTransactionItem(*it);

                if (!inputYesNo("Yakin ingin menghapus item ini? (y/n): ")) {
                    cout << "Penghapusan dibatalkan.\n";
                    return;
                }

                transactions.erase(it);
                cout << "Item transaksi berhasil dihapus.\n";
                return;
            }
        }

        cout << "Item transaksi tidak ditemukan.\n";
    }

    void displayTopNProducts() {
        cout << "\n=== TOP-N PRODUK PALING SERING DIBELI ===\n";

        if (transactions.empty()) {
            cout << "Data transaksi kosong.\n";
            return;
        }

        int n = inputPositiveNumber("Masukkan jumlah produk teratas yang ingin ditampilkan: ");
        vector<TransactionItem> productTotals;

        for (const auto& item : transactions) {
            bool found = false;

            for (auto& total : productTotals) {
                if (total.productId == item.productId) {
                    total.quantity += item.quantity;
                    found = true;
                    break;
                }
            }

            if (!found) {
                TransactionItem totalItem = item;
                productTotals.push_back(totalItem);
            }
        }

        sort(productTotals.begin(), productTotals.end(), [](const TransactionItem& a, const TransactionItem& b) {
            if (a.quantity == b.quantity) return a.productId < b.productId;
            return a.quantity > b.quantity;
        });

        cout << left
             << setw(6) << "Rank"
             << setw(10) << "ProdID"
             << setw(20) << "Product"
             << setw(15) << "Category"
             << setw(14) << "Total Qty" << "\n";
        cout << string(65, '-') << "\n";

        int limit = min(n, static_cast<int>(productTotals.size()));
        for (int i = 0; i < limit; i++) {
            cout << left
                 << setw(6) << (i + 1)
                 << setw(10) << productTotals[i].productId
                 << setw(20) << productTotals[i].productName
                 << setw(15) << productTotals[i].category
                 << setw(14) << productTotals[i].quantity << "\n";
        }
    }

    void displayFrequentlyBoughtTogether() {
        cout << "\n=== FREQUENTLY BOUGHT TOGETHER ===\n";

        if (transactions.empty()) {
            cout << "Data transaksi kosong.\n";
            return;
        }

        string targetProductId = inputFormattedId("Masukkan ID produk acuan (angka saja): ", "P", 3);
        Product* targetProduct = findProductById(targetProductId);

        if (targetProduct == nullptr) {
            cout << "Produk acuan tidak ditemukan pada master produk.\n";
            return;
        }

        vector<string> relatedTransactionIds;
        for (const auto& item : transactions) {
            if (item.productId == targetProductId && !containsString(relatedTransactionIds, item.transactionId)) {
                relatedTransactionIds.push_back(item.transactionId);
            }
        }

        if (relatedTransactionIds.empty()) {
            cout << "Produk " << targetProductId << " belum pernah muncul dalam transaksi.\n";
            return;
        }

        vector<TransactionItem> togetherCounts;

        for (const auto& item : transactions) {
            if (item.productId == targetProductId) continue;
            if (!containsString(relatedTransactionIds, item.transactionId)) continue;

            bool found = false;
            for (auto& countItem : togetherCounts) {
                if (countItem.productId == item.productId) {
                    countItem.quantity += 1;
                    found = true;
                    break;
                }
            }

            if (!found) {
                TransactionItem newCount = item;
                newCount.quantity = 1;
                togetherCounts.push_back(newCount);
            }
        }

        if (togetherCounts.empty()) {
            cout << "Belum ada produk lain yang pernah dibeli bersama "
                 << targetProduct->productName << ".\n";
            return;
        }

        sort(togetherCounts.begin(), togetherCounts.end(), [](const TransactionItem& a, const TransactionItem& b) {
            if (a.quantity == b.quantity) return a.productId < b.productId;
            return a.quantity > b.quantity;
        });

        cout << "Produk acuan: " << targetProduct->productId << " - "
             << targetProduct->productName << " (" << targetProduct->category << ")\n\n";

        cout << left
             << setw(6) << "Rank"
             << setw(10) << "ProdID"
             << setw(20) << "Product"
             << setw(15) << "Category"
             << setw(16) << "Frekuensi" << "\n";
        cout << string(70, '-') << "\n";

        for (size_t i = 0; i < togetherCounts.size(); i++) {
            cout << left
                 << setw(6) << (i + 1)
                 << setw(10) << togetherCounts[i].productId
                 << setw(20) << togetherCounts[i].productName
                 << setw(15) << togetherCounts[i].category
                 << setw(16) << togetherCounts[i].quantity << "\n";
        }
    }

    void insertTransactionInteractive() {
        cout << "\n=== INSERT ITEM TRANSAKSI ===\n";

        string transactionId = inputFormattedId("Masukkan ID transaksi (angka saja): ", "T", 4);

        string customerId;
        string transactionDate;

        if (transactionExists(transactionId)) {
            TransactionItem* existingTransaction = getFirstTransactionItem(transactionId);

            cout << "Transaksi " << transactionId << " sudah ada.\n";
            cout << "Customer ID : " << existingTransaction->customerId << "\n";
            cout << "Tanggal     : " << existingTransaction->transactionDate << "\n";
            cout << "Silakan tambahkan item produk baru atau update quantity.\n";

            customerId = existingTransaction->customerId;
            transactionDate = existingTransaction->transactionDate;
        } else {
            customerId = inputFormattedId("Masukkan ID customer (angka saja): ", "C", 3);
            transactionDate = inputValidDate();
        }

        string inputProductId = inputFormattedId("Masukkan ID produk (angka saja): ", "P", 3);

        Product* selectedProduct = findProductById(inputProductId);

        if (selectedProduct == nullptr) {
            cout << "Product ID " << inputProductId << " belum ada pada master produk.\n";
            cout << "Silakan masukkan data produk baru.\n";

            string inputProductName = inputNonEmptyString("Masukkan nama produk: ");

            Product* existingByName = findProductByNameNormalized(inputProductName);

            if (existingByName != nullptr) {
                cout << "Produk tersebut ternyata sudah ada sebagai "
                     << existingByName->productId << ".\n";
                cout << "Sistem akan menggunakan product ID yang sudah ada.\n";

                selectedProduct = existingByName;
            } else {
                string inputCategory = inputNonEmptyString("Masukkan kategori produk: ");

                Product newProduct;
                newProduct.productId = inputProductId;
                newProduct.productName = trim(inputProductName);
                newProduct.category = trim(inputCategory);

                products.push_back(newProduct);
                selectedProduct = &products.back();

                cout << "Produk baru berhasil ditambahkan ke master produk.\n";
            }
        } else {
            cout << "Produk ditemukan: " << selectedProduct->productName
                 << " (" << selectedProduct->category << ")\n";
        }

        int quantity = inputQuantity();

        TransactionItem* existingItem = findExactTransactionProduct(
            transactionId,
            selectedProduct->productId
        );

        if (existingItem != nullptr) {
            if (existingItem->quantity == quantity) {
                cout << "Item transaksi sudah pernah ada dengan data yang sama. Data diabaikan.\n";
                return;
            } else {
                existingItem->quantity = quantity;
                existingItem->productName = selectedProduct->productName;
                existingItem->category = selectedProduct->category;

                cout << "Produk pada transaksi yang sama sudah ada sebagai "
                     << selectedProduct->productId
                     << ". Quantity diperbarui menjadi "
                     << quantity << ".\n";
                return;
            }
        }

        TransactionItem newItem;
        newItem.transactionId = transactionId;
        newItem.customerId = customerId;
        newItem.transactionDate = transactionDate;
        newItem.productId = selectedProduct->productId;
        newItem.productName = selectedProduct->productName;
        newItem.category = selectedProduct->category;
        newItem.quantity = quantity;

        transactions.push_back(newItem);
        cout << "Item transaksi berhasil ditambahkan.\n";
    }

    void menu() {
        int choice;
        do {
            cout << "\n========== MENU ==========\n";
            cout << "1. Tampilkan semua transaksi\n";
            cout << "2. Tampilkan master produk\n";
            cout << "3. Insert item transaksi\n";
            cout << "4. Search by transaction\n";
            cout << "5. Search by customer\n";
            cout << "6. Search by product\n";
            cout << "7. Update item transaksi\n";
            cout << "8. Delete item transaksi\n";
            cout << "9. Top-N produk paling sering dibeli\n";
            cout << "10. Frequently bought together\n";
            cout << "11. Simpan\n";
            cout << "12. Load\n";
            cout << "13. Keluar\n";

            choice = inputMenuChoice();

            if (choice == 1) {
                displayAllTransactions();
            }
            else if (choice == 2) {
                displayAllProducts();
            }
            else if (choice == 3) {
                insertTransactionInteractive();
            }
            else if (choice == 4) {
                string transactionId = inputFormattedId(
                    "Masukkan ID transaksi untuk pencarian (angka saja): ",
                    "T",
                    4
                );
                vector<TransactionItem> result = searchByTransaction(transactionId);
                displaySearchResult(result);
            }
            else if (choice == 5) {
                string customerId = inputFormattedId(
                    "Masukkan ID customer untuk pencarian (angka saja): ",
                    "C",
                    3
                );
                vector<TransactionItem> result = searchByCustomer(customerId);
                displaySearchResult(result);
            }
            else if (choice == 6) {
                string productId = inputFormattedId(
                    "Masukkan ID produk untuk pencarian (angka saja): ",
                    "P",
                    3
                );
                vector<TransactionItem> result = searchByProduct(productId);
                displaySearchResult(result);
            }
            else if (choice == 7) {
                updateTransactionInteractive();
            }
            else if (choice == 8) {
                deleteTransactionInteractive();
            }
            else if (choice == 9) {
                displayTopNProducts();
            }
            else if (choice == 10) {
                displayFrequentlyBoughtTogether();
            }
            else if (choice == 11) {
                if (saveAllData()) {
                    cout << "Data transaksi dan master produk berhasil disimpan.\n";
                } else {
                    cout << "Gagal menyimpan data.\n";
                }
            }
            else if (choice == 12) {
                if (loadAllData()) {
                    cout << "Data transaksi dan master produk berhasil dimuat ulang.\n";
                } else {
                    cout << "Gagal memuat ulang data. Pastikan file tersedia.\n";
                }
            }
            else if (choice == 13) {
                cout << "Program selesai.\n";
            }   
        } while (choice != 13);
    }
};

int main() {
    TransactionManager manager("transactions.csv", "products.csv");

    if (!manager.loadProductsFromFile()) {
        cout << "Gagal membaca products.csv. Pastikan file tersedia.\n";
        return 1;
    }

    if (manager.loadTransactionsFromFile()) {
        cout << "Data transaksi berhasil dimuat dari file.\n";
    } else {
        cout << "transactions.csv tidak ditemukan atau kosong. Program tetap berjalan dengan data transaksi kosong.\n";
    }

    manager.menu();
    return 0;
}