#ifndef MAGAZINE_H
#define MAGAZINE_H

#include "Book.h"

class Magazine : public Book {
private:
    int issueNumber;

public:
    Magazine(int id, const std::string& title, const std::string& author,
             const std::string& publishDate, int pages, int issueNumber);

    int getIssueNumber() const;
    void setIssueNumber(int n);

    std::string typeName() const override { return "Magazine"; }
};

#endif

