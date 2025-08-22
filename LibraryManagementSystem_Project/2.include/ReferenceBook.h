#ifndef REFERENCEBOOK_H
#define REFERENCEBOOK_H

#include "Book.h"
#include <string>

class ReferenceBook : public Book {
private:
    std::string category;

public:
    ReferenceBook(int id, const std::string& title, const std::string& author,
                  const std::string& category, const std::string& publishDate, int pages);

    const std::string& getCategory() const { return category; }
    std::string typeName() const override { return "ReferenceBook"; }
};

#endif


