#include "ReferenceBook.h"

ReferenceBook::ReferenceBook(int id, const std::string& title, const std::string& author,
                             const std::string& category, const std::string& publishDate, int pages)
    : Book(id, title, author, publishDate, pages), category(category) {}

    