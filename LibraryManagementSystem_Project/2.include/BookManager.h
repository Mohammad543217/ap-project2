#ifndef BOOK_MANAGER_H
#define BOOK_MANAGER_H

#include <memory>
#include <string>
#include <vector>
#include "Book.h"     

class BookManager {
public:
    
    BookManager();
    explicit BookManager(const std::vector<std::shared_ptr<Book>>& initial);

    const std::vector<std::shared_ptr<Book>>& listAll() const;

    std::shared_ptr<Book> getById(int id) const;

    bool exists(int id) const;

    bool addBook(const std::shared_ptr<Book>& b, std::string* err = nullptr);

    bool removeBook(int id);

    bool updateBook(int id, const Book& patch, std::string* err = nullptr);

    std::vector<std::shared_ptr<Book>> searchByTitle (const std::string& q) const;
    std::vector<std::shared_ptr<Book>> searchByAuthor(const std::string& q) const;
    std::vector<std::shared_ptr<Book>> searchByCategory(const std::string& q) const; 

    std::vector<std::shared_ptr<Book>> sortByTitle      (bool ascending = true) const;
    std::vector<std::shared_ptr<Book>> sortByAuthor     (bool ascending = true) const;
    std::vector<std::shared_ptr<Book>> sortByPublishDate(bool ascending = true) const;

private:

    bool validateNewBook(const std::shared_ptr<Book>& b, std::string& err) const;


    std::vector<std::shared_ptr<Book>> books_;
};

#endif 
