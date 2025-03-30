#include <iostream>
#include <vector>
#include <string>

class Book {
public:
    std::string title;
    std::string author;
    
    Book(std::string t, std::string a) : title(t), author(a) {}
    void display() const {
        std::cout << "Knyga: " << title << " | Autorius: " << author << std::endl;
    }
};

class Library {
private:
    std::vector<Book> books;
public:
    void addBook(const Book& book) {
        books.push_back(book);
    }
    void showBooks() const {
        std::cout << "\n-- Knygų sąrašas --" << std::endl;
        for (const auto& book : books) {
            book.display();
        }
    }
};

int main() {
    Library myLibrary;
    myLibrary.addBook(Book("1984", "George Orwell"));
    myLibrary.addBook(Book("Altorių šešėly", "Vincas Mykolaitis-Putinas"));
    
    std::cout << "Sveiki atvykę į Biblioteką!" << std::endl;
    myLibrary.showBooks();
    
    return 0;
}
