#include "TextBook.h"

TextBook::TextBook(int id, const std::string& title, const std::string& author,
                   const std::string& publishDate, int pages,
                   const std::string& level, const std::string& major)
    : Book(id, title, author, publishDate, pages), level(level), major(major) {}

const std::string& TextBook::getLevel() const { return level; }
const std::string& TextBook::getMajor() const { return major; }
void TextBook::setLevel(const std::string& l) { level = l; }
void TextBook::setMajor(const std::string& m) { major = m; }



