#ifndef BOOK_H
#define BOOK_H

#include <string>

enum class BookStatus { Available, Borrowed, Reserved, Lost };

class Book {
protected:
    int id;
    std::string title;
    std::string author;
    std::string publishDate;
    int pages;
    BookStatus status;

public:
    Book(int id, const std::string& title, const std::string& author,
         const std::string& publishDate, int pages);
    virtual ~Book() = default;

    int getId() const;
    const std::string& getTitle() const;
    const std::string& getAuthor() const;
    const std::string& getPublishDate() const;
    int getPages() const;
    BookStatus getStatus() const;

    void setTitle(const std::string& t);
    void setAuthor(const std::string& a);
    void setPublishDate(const std::string& d);
    void setPages(int p);
    void setStatus(BookStatus s);

    virtual std::string typeName() const { return "Book"; }
};

#endif

