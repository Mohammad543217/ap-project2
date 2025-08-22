#ifndef BOOKDATAACCESS_H
#define BOOKDATAACCESS_H

#include <string>
#include <vector>
#include <memory>

class Book;

class BookDataAccess {
public:
    explicit BookDataAccess(const std::string& filePath);

    bool addBook(const std::shared_ptr<Book>& b);
    std::vector<std::shared_ptr<Book>> getAll() const;
    std::shared_ptr<Book> getById(int id) const;
    bool updateBook(const Book& updated);
    bool deleteBook(int id);

private:
    std::string filePath_;
    void saveToFile(const std::vector<std::shared_ptr<Book>>& books) const;
    std::vector<std::shared_ptr<Book>> loadFromFile() const;
};

#endif