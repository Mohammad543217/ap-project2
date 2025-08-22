#include "BookManager.h"
#include "Book.h"
#include "TextBook.h"
#include "Magazine.h"
#include "ReferenceBook.h"

#include <algorithm>
#include <cctype>
#include <stdexcept>
#include <unordered_set>

namespace {

inline std::string toLower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c){ return std::tolower(c); });
    return s;
}

inline std::string trim(std::string s) {
    auto notSpace = [](int ch){ return !std::isspace(ch); };
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), notSpace));
    s.erase(std::find_if(s.rbegin(), s.rend(), notSpace).base(), s.end());
    return s;
}

inline bool containsIC(const std::string& hay, const std::string& needle) {
    if (needle.empty()) return true;
    auto H = toLower(hay);
    auto N = toLower(needle);
    return H.find(N) != std::string::npos;
}

inline std::string detectCategory(const std::shared_ptr<Book>& b) {
    if (std::dynamic_pointer_cast<TextBook>(b))      return "TextBook";
    if (std::dynamic_pointer_cast<Magazine>(b))      return "Magazine";
    if (std::dynamic_pointer_cast<ReferenceBook>(b)) return "ReferenceBook";
    return "Book";
}


inline std::string dateKey(const std::string& d) {
    
    if (d.size() >= 10 && std::isdigit(d[0]) && std::isdigit(d[1]) &&
        std::isdigit(d[2]) && std::isdigit(d[3]) && d[4]=='-') {
        return d;
    }
    
    return d;
}

} 



BookManager::BookManager() = default;

BookManager::BookManager(const std::vector<std::shared_ptr<Book>>& initial) {
    books_ = initial;
}

const std::vector<std::shared_ptr<Book>>& BookManager::listAll() const {
    return books_;
}

std::shared_ptr<Book> BookManager::getById(int id) const {
    auto it = std::find_if(books_.begin(), books_.end(),
                           [&](const std::shared_ptr<Book>& b){ return b->getId() == id; });
    return it == books_.end() ? nullptr : *it;
}

bool BookManager::exists(int id) const {
    return static_cast<bool>(getById(id));
}

bool BookManager::validateNewBook(const std::shared_ptr<Book>& b, std::string& err) const {
    if (!b) { err = "Book pointer is null."; return false; }
    if (b->getId() <= 0) { err = "ID must be positive."; return false; }
    if (exists(b->getId())) { err = "Duplicate ID."; return false; }

    if (trim(b->getTitle()).empty())   { err = "Title is required."; return false; }
    if (trim(b->getAuthor()).empty())  { err = "Author is required."; return false; }
    if (b->getPages() <= 0)            { err = "Pages must be > 0."; return false; }
    if (trim(b->getPublishDate()).empty()) { err = "Publish date is required."; return false; }

    if (std::dynamic_pointer_cast<ReferenceBook>(b)) {
        
    }
    return true;
}

bool BookManager::addBook(const std::shared_ptr<Book>& b, std::string* errorMessage) {
    std::string err;
    if (!validateNewBook(b, err)) {
        if (errorMessage) *errorMessage = err;
        return false;
    }
    books_.push_back(b);
    return true;
}

bool BookManager::removeBook(int id) {
    auto before = books_.size();
    books_.erase(std::remove_if(books_.begin(), books_.end(),
                                [&](const std::shared_ptr<Book>& b){ return b->getId() == id; }),
                 books_.end());
    return books_.size() != before;
}

bool BookManager::updateBook(int id, const Book& patch, std::string* errorMessage) {
    auto b = getById(id);
    if (!b) {
        if (errorMessage) *errorMessage = "Book not found.";
return false;
    }

    
    if (!trim(patch.getTitle()).empty())   b->setTitle(trim(patch.getTitle()));
    if (!trim(patch.getAuthor()).empty())  b->setAuthor(trim(patch.getAuthor()));
    if (!trim(patch.getPublishDate()).empty()) b->setPublishDate(trim(patch.getPublishDate()));
    if (patch.getPages() > 0)              b->setPages(patch.getPages());
    
    b->setStatus(patch.getStatus()); 

    if (std::dynamic_pointer_cast<ReferenceBook>(b)) {
        
    }
    return true;
}


std::vector<std::shared_ptr<Book>> BookManager::searchByTitle(const std::string& q) const {
    std::vector<std::shared_ptr<Book>> out;
    for (auto& b : books_) {
        if (containsIC(b->getTitle(), q)) out.push_back(b);
    }
    return out;
}

std::vector<std::shared_ptr<Book>> BookManager::searchByAuthor(const std::string& q) const {
    std::vector<std::shared_ptr<Book>> out;
    for (auto& b : books_) {
        if (containsIC(b->getAuthor(), q)) out.push_back(b);
    }
    return out;
}

std::vector<std::shared_ptr<Book>> BookManager::searchByCategory(const std::string& q) const {
    std::vector<std::shared_ptr<Book>> out;
    auto qn = toLower(q);
    for (auto& b : books_) {
        auto cat = toLower(detectCategory(b)); 
        if (containsIC(cat, qn)) out.push_back(b);
    }
    return out;
}


std::vector<std::shared_ptr<Book>> BookManager::sortByTitle(bool ascending) const {
    auto v = books_;
    std::stable_sort(v.begin(), v.end(), [&](const auto& a, const auto& b){
        auto A = toLower(a->getTitle());
        auto B = toLower(b->getTitle());
        return ascending ? (A < B) : (A > B);
    });
    return v;
}

std::vector<std::shared_ptr<Book>> BookManager::sortByAuthor(bool ascending) const {
    auto v = books_;
    std::stable_sort(v.begin(), v.end(), [&](const auto& a, const auto& b){
        auto A = toLower(a->getAuthor());
        auto B = toLower(b->getAuthor());
        return ascending ? (A < B) : (A > B);
    });
    return v;
}

std::vector<std::shared_ptr<Book>> BookManager::sortByPublishDate(bool ascending) const {
    auto v = books_;
    std::stable_sort(v.begin(), v.end(), [&](const auto& a, const auto& b){
        auto A = dateKey(a->getPublishDate());
        auto B = dateKey(b->getPublishDate());
        return ascending ? (A < B) : (A > B);
    });
    return v;
}