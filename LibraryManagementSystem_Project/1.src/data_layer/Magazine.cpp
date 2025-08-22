#include "Magazine.h"

Magazine::Magazine(int id, const std::string& title, const std::string& author,
                   const std::string& publishDate, int pages, int issueNumber)
    : Book(id, title, author, publishDate, pages), issueNumber(issueNumber) {}

int Magazine::getIssueNumber() const { return issueNumber; }
void Magazine::setIssueNumber(int n) { issueNumber = n; }

