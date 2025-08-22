#include "Book.h"

Book::Book(int id, const std::string& title, const std::string& author,
           const std::string& publishDate, int pages)
    : id(id), title(title), author(author), publishDate(publishDate),
      pages(pages), status(BookStatus::Available) {}

int Book::getId() const {
    return id;
}

const std::string& Book::getTitle() const {
    return title;
}

const std::string& Book::getAuthor() const {
    return author;
}

const std::string& Book::getPublishDate() const {
    return publishDate;
}

int Book::getPages() const {
    return pages;
}

BookStatus Book::getStatus() const {
    return status;
}

void Book::setTitle(const std::string& t) {
    title = t;
}

void Book::setAuthor(const std::string& a) {
    author = a;
}

void Book::setPublishDate(const std::string& d) {
    publishDate = d;
}

void Book::setPages(int p) {
    pages = p;
}

void Book::setStatus(BookStatus s) {
    status = s;
}