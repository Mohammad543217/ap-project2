#ifndef TEXTBOOK_H
#define TEXTBOOK_H

#include "Book.h"
#include <string>

class TextBook : public Book {
private:
    std::string level;
    std::string major;

public:
    TextBook(int id, const std::string& title, const std::string& author,
             const std::string& publishDate, int pages,
             const std::string& level, const std::string& major);

    const std::string& getLevel() const;
    const std::string& getMajor() const;
    void setLevel(const std::string& l);
    void setMajor(const std::string& m);

    std::string typeName() const override { return "TextBook"; }
};

#endif


