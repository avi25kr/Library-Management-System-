#include <iostream>
#include <string>
#include <unordered_map>
#include <queue>
#include <vector>
#include <fstream>
#include <sstream>
#include <ctime>

using namespace std;

class Book {
public:
    int id;
    string title;
    string author;
    bool isIssued;
    int issuedTo;
    string issueDate;
    string dueDate;

    Book() : id(0), isIssued(false), issuedTo(-1) {}

    Book(int id, const string& title, const string& author)
        : id(id), title(title), author(author), isIssued(false), issuedTo(-1) {}

    void display() const {
        cout << "ID: " << id << ", Title: " << title << ", Author: " << author
             << ", Issued: " << (isIssued ? "Yes" : "No");
        if (isIssued) {
            cout << ", Issued to Member ID: " << issuedTo << ", Issue Date: " << issueDate
                 << ", Due Date: " << dueDate;
        }
        cout << endl;
    }
};

class Member {
public:
    int id;
    string name;

    Member() : id(0) {}

    Member(int id, const string& name) : id(id), name(name) {}

    void display() const {
        cout << "ID: " << id << ", Name: " << name << endl;
    }
};

class Transaction {
public:
    int bookId;
    int memberId;
    string type; // "issue" or "return"
    string date;

    Transaction(int bookId, int memberId, const string& type, const string& date)
        : bookId(bookId), memberId(memberId), type(type), date(date) {}
};

string getCurrentDate() {
    time_t now = time(0);
    tm *ltm = localtime(&now);
    char date[11];
    sprintf(date, "%04d-%02d-%02d", 1900 + ltm->tm_year, 1 + ltm->tm_mon, ltm->tm_mday);
    return string(date);
}

string getDueDate(int daysToAdd) {
    time_t now = time(0) + daysToAdd * 24 * 3600;
    tm *ltm = localtime(&now);
    char date[11];
    sprintf(date, "%04d-%02d-%02d", 1900 + ltm->tm_year, 1 + ltm->tm_mon, ltm->tm_mday);
    return string(date);
}

class Library {
private:
    unordered_map<int, Book> books;
    unordered_map<int, Member> members;
    queue<Transaction> transactions;
    unordered_map<int, vector<int>> reservations; // Book ID -> List of Member IDs

    void saveBooks() const {
        ofstream outFile("books.txt");
        if (!outFile) {
            cerr << "Error opening books.txt for writing." << endl;
            return;
        }
        for (const auto& pair : books) {
            const Book& book = pair.second;
            outFile << book.id << "," << book.title << "," << book.author << ","
                    << book.isIssued << "," << book.issuedTo << ","
                    << book.issueDate << "," << book.dueDate << endl;
        }
        outFile.close();
    }

    void loadBooks() {
        ifstream inFile("books.txt");
        if (!inFile) {
            cerr << "Error opening books.txt for reading." << endl;
            return;
        }
        string line;
        while (getline(inFile, line)) {
            stringstream ss(line);
            string token;
            vector<string> tokens;
            while (getline(ss, token, ',')) {
                tokens.push_back(token);
            }
            int id = stoi(tokens[0]);
            string title = tokens[1];
            string author = tokens[2];
            bool isIssued = stoi(tokens[3]);
            int issuedTo = stoi(tokens[4]);
            string issueDate = tokens[5];
            string dueDate = tokens[6];
            Book book(id, title, author);
            book.isIssued = isIssued;
            book.issuedTo = issuedTo;
            book.issueDate = issueDate;
            book.dueDate = dueDate;
            books[id] = book;
        }
        inFile.close();
    }

    void saveMembers() const {
        ofstream outFile("members.txt");
        if (!outFile) {
            cerr << "Error opening members.txt for writing." << endl;
            return;
        }
        for (const auto& pair : members) {
            const Member& member = pair.second;
            outFile << member.id << "," << member.name << endl;
        }
        outFile.close();
    }

    void loadMembers() {
        ifstream inFile("members.txt");
        if (!inFile) {
            cerr << "Error opening members.txt for reading." << endl;
            return;
        }
        string line;
        while (getline(inFile, line)) {
            stringstream ss(line);
            string token;
            vector<string> tokens;
            while (getline(ss, token, ',')) {
                tokens.push_back(token);
            }
            int id = stoi(tokens[0]);
            string name = tokens[1];
            members[id] = Member(id, name);
        }
        inFile.close();
    }

    void saveTransactions() const {
        ofstream outFile("transactions.txt");
        if (!outFile) {
            cerr << "Error opening transactions.txt for writing." << endl;
            return;
        }
        queue<Transaction> temp = transactions;
        while (!temp.empty()) {
            const Transaction& t = temp.front();
            outFile << t.bookId << "," << t.memberId << "," << t.type << "," << t.date << endl;
            temp.pop();
        }
        outFile.close();
    }

    void loadTransactions() {
        ifstream inFile("transactions.txt");
        if (!inFile) {
            cerr << "Error opening transactions.txt for reading." << endl;
            return;
        }
        string line;
        while (getline(inFile, line)) {
            stringstream ss(line);
            string token;
            vector<string> tokens;
            while (getline(ss, token, ',')) {
                tokens.push_back(token);
            }
            int bookId = stoi(tokens[0]);
            int memberId = stoi(tokens[1]);
            string type = tokens[2];
            string date = tokens[3];
            transactions.push(Transaction(bookId, memberId, type, date));
        }
        inFile.close();
    }

public:
    Library() {
        loadBooks();
        loadMembers();
        loadTransactions();
    }

    ~Library() {
        saveBooks();
        saveMembers();
        saveTransactions();
    }

    void addBook(int id, const string& title, const string& author) {
        books[id] = Book(id, title, author);
    }

    void addMember(int id, const string& name) {
        members[id] = Member(id, name);
    }

    void issueBook(int bookId, int memberId) {
        if (books.find(bookId) == books.end()) {
            cout << "Book not found." << endl;
            return;
        }
        if (members.find(memberId) == members.end()) {
            cout << "Member not found." << endl;
            return;
        }
        if (books[bookId].isIssued) {
            cout << "Book is already issued. Adding to reservation list." << endl;
            reservations[bookId].push_back(memberId);
        } else {
            books[bookId].isIssued = true;
            books[bookId].issuedTo = memberId;
            books[bookId].issueDate = getCurrentDate();
            books[bookId].dueDate = getDueDate(14); // 2 weeks from now
            transactions.push(Transaction(bookId, memberId, "issue", getCurrentDate()));
            cout << "Book issued to member ID " << memberId << endl;
        }
    }

    void returnBook(int bookId, int memberId) {
        if (books.find(bookId) == books.end()) {
            cout << "Book not found." << endl;
            return;
        }
        if (members.find(memberId) == members.end()) {
            cout << "Member not found." << endl;
            return;
        }
        if (books[bookId].isIssued && books[bookId].issuedTo == memberId) {
            books[bookId].isIssued = false;
            books[bookId].issuedTo = -1;
            books[bookId].issueDate = "";
            books[bookId].dueDate = "";
            transactions.push(Transaction(bookId, memberId, "return", getCurrentDate()));
            cout << "Book returned by member ID " << memberId << endl;

            // Check if there are reservations for this book
            if (!reservations[bookId].empty()) {
                int nextMemberId = reservations[bookId].front();
                reservations[bookId].erase(reservations[bookId].begin());
                issueBook(bookId, nextMemberId);
            }
        } else {
            cout << "Book was not issued to this member." << endl;
        }
    }

    void displayBooks() const {
        for (const auto& pair : books) {
            pair.second.display();
        }
    }

    void displayMembers() const {
        for (const auto& pair : members) {
            pair.second.display();
        }
    }

    void displayTransactions() const {
        queue<Transaction> temp = transactions;
        while (!temp.empty()) {
            const Transaction& t = temp.front();
            cout << "Book ID: " << t.bookId << ", Member ID: " << t.memberId
                 << ", Type: " << t.type << ", Date: " << t.date << endl;
            temp.pop();
        }
    }
};

// int main() {
//     Library lib;

//     lib.addBook(1, "The Great Gatsby", "F. Scott Fitzgerald");
//     lib.addBook(2, "1984", "George Orwell");

//     lib.addMember(1, "Alice");
//     lib.addMember(2, "Bob");

//     lib.displayBooks();
//     lib.displayMembers();

//     lib.issueBook(1, 1);
//     lib.issueBook(2, 2);

//     lib.displayBooks();
//     lib.displayTransactions();

//     lib.returnBook(1, 1);
//     lib.returnBook(2, 2);

//     lib.displayBooks();
//     lib.displayTransactions();

//     return 0;
// }

int main() {
    Library library;

    int choice;
    do {
        cout << "\nLibrary Management System\n";
        cout << "1. Add Book\n";
        cout << "2. Add Member\n";
        cout << "3. Issue Book\n";
        cout << "4. Return Book\n";
        cout << "5. Display Books\n";
        cout << "6. Display Members\n";
        cout << "7. Display Transactions\n";
        cout << "8. Search Book by Title\n";
        cout << "9. Search Member by Name\n";
        cout << "0. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        int id, memberId, bookId;
        string title, author, name;

        switch (choice) {
            case 1:
                cout << "Enter Book ID: ";
                cin >> id;
                cin.ignore();
                cout << "Enter Book Title: ";
                getline(cin, title);
                cout << "Enter Book Author: ";
                getline(cin, author);
                library.addBook(id, title, author);
                break;
            case 2:
                cout << "Enter Member ID: ";
                cin >> id;
                cin.ignore();
                cout << "Enter Member Name: ";
                getline(cin, name);
                library.addMember(id, name);
                break;
            case 3:
                cout << "Enter Book ID to issue: ";
                cin >> bookId;
                cout << "Enter Member ID: ";
                cin >> memberId;
                library.issueBook(bookId, memberId);
                break;
            case 4:
                cout << "Enter Book ID to return: ";
                cin >> bookId;
                cout << "Enter Member ID: ";
                cin >> memberId;
                library.returnBook(bookId, memberId);
                break;
            case 5:
                library.displayBooks();
                break;
            case 6:
                library.displayMembers();
                break;
            case 7:
                library.displayTransactions();
                break;
            case 8:
                cin.ignore();
                cout << "Enter Book Title to search: ";
                getline(cin, title);
                library.searchBookByTitle(title);
                break;
            case 9:
                cin.ignore();
                cout << "Enter Member Name to search: ";
                getline(cin, name);
                library.searchMemberByName(name);
                break;
            case 0:
                cout << "Exiting...\n";
                break;
            default:
                cout << "Invalid choice! Please try again.\n";
        }
    } while (choice != 0);

    return 0;
}
