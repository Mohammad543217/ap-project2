#include "BookDataAccess.h"

#include "Book.h"
#include "TextBook.h"
#include "Magazine.h"
#include "ReferenceBook.h"

#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <stdexcept>
#include <memory>

namespace {

inline std::string trim(std::string s) {
    auto notSpace = [](int ch){ return !std::isspace(ch); };
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), notSpace));
    s.erase(std::find_if(s.rbegin(), s.rend(), notSpace).base(), s.end());
    return s;
}

static std::vector<std::string> splitCSV(const std::string& line) {
    std::vector<std::string> cells;
    std::string cur;
    bool inQuotes = false;

    for (size_t i = 0; i < line.size(); ++i) {
        char c = line[i];
        if (inQuotes) {
            if (c == '"') {
                if (i + 1 < line.size() && line[i+1] == '"') {
                    cur.push_back('"'); ++i;
                } else {
                    inQuotes = false;
                }
            } else cur.push_back(c);
        } else {
            if (c == ',') { cells.push_back(trim(cur)); cur.clear(); }
            else if (c == '"') { inQuotes = true; }
            else cur.push_back(c);
        }
    }
    cells.push_back(trim(cur));
    return cells;
}

static std::string escapeCSV(const std::string& s) {
    bool needQuotes = s.find_first_of(",\"\n\r") != std::string::npos;
    if (!needQuotes) return s;
    std::string out = "\"";
    for (char c : s) {
        if (c == '"') out += "\"\"";
        else out.push_back(c);
    }
    out += "\"";
    return out;
}

static std::string joinCSV(const std::vector<std::string>& cells) {
    std::ostringstream oss;
    for (size_t i = 0; i < cells.size(); ++i) {
        if (i) oss << ',';
        oss << escapeCSV(cells[i]);
    }
    return oss.str();
}

inline bool toIntSafe(const std::string& s, int& out) {
    try {
        size_t idx = 0;
        int v = std::stoi(s, &idx);
        if (idx != s.size()) return false;
        out = v;
        return true;
    } catch (...) { return false; }
}

} // namespace

// ====== پیاده‌سازی BookDataAccess ======

BookDataAccess::BookDataAccess(const std::string& filePath)
: filePath_(filePath) {}

bool BookDataAccess::addBook(const std::shared_ptr<Book>& b) {
    auto books = loadFromFile();
    auto it = std::find_if(books.begin(), books.end(),
        [&](const std::shared_ptr<Book>& x){ return x->getId() == b->getId(); });
    if (it != books.end()) return false;
    books.push_back(b);
    saveToFile(books);
    return true;
}

std::vector<std::shared_ptr<Book>> BookDataAccess::getAll() const {
    return loadFromFile();
}

std::shared_ptr<Book> BookDataAccess::getById(int id) const {
    auto books = loadFromFile();
    auto it = std::find_if(books.begin(), books.end(),
        [&](const std::shared_ptr<Book>& x){ return x->getId() == id; });
    return it == books.end() ? nullptr : *it;
}

bool BookDataAccess::updateBook(const Book& updated) {
    auto books = loadFromFile();
    bool found = false;
    for (auto& ptr : books) {
        if (ptr->getId() == updated.getId()) {
            // فیلدهای مشترک
            ptr->setTitle(updated.getTitle());
            ptr->setAuthor(updated.getAuthor());
            ptr->setPublishDate(updated.getPublishDate());
            ptr->setPages(updated.getPages());
            ptr->setStatus(updated.getStatus());
            found = true;
            break;
        }
    }
    if (!found) return false;
    saveToFile(books);
    return true;
}

bool BookDataAccess::deleteBook(int id) {
    auto books = loadFromFile();
    auto before = books.size();
    books.erase(std::remove_if(books.begin(), books.end(),
        [&](const std::shared_ptr<Book>& x){ return x->getId() == id; }),
        books.end());
    if (books.size() == before) return false;
    saveToFile(books);
    return true;
}

void BookDataAccess::saveToFile(const std::vector<std::shared_ptr<Book>>& books) const {
    std::ofstream out(filePath_, std::ios::trunc);if (!out) throw std::runtime_error("Cannot open books CSV for writing: " + filePath_);


    out << "type,id,title,author,publishDate,pages,status,extra1,extra2\n";

    for (const auto& b : books) {
        std::string type = "book";
        std::string extra1, extra2;

        if (auto tb = std::dynamic_pointer_cast<TextBook>(b)) {
            type = "textbook";
            extra1 = tb->getLevel();
            extra2 = tb->getMajor();
        } else if (auto mg = std::dynamic_pointer_cast<Magazine>(b)) {
            type = "magazine";
            extra1 = std::to_string(mg->getIssueNumber());
        } else if (std::dynamic_pointer_cast<ReferenceBook>(b)) {
            type = "referencebook";

        } else {
            type = "book";
        }

        std::vector<std::string> row = {
            type,
            std::to_string(b->getId()),
            b->getTitle(),
            b->getAuthor(),
            b->getPublishDate(),
            std::to_string(b->getPages()),

            (b->getStatus()==BookStatus::Available ? "available" :
             b->getStatus()==BookStatus::Borrowed  ? "borrowed"  :
             b->getStatus()==BookStatus::Reserved  ? "reserved"  :
                                                     "lost"),
            extra1,
            extra2
        };
        out << joinCSV(row) << "\n";
    }
}

std::vector<std::shared_ptr<Book>> BookDataAccess::loadFromFile() const {
    std::vector<std::shared_ptr<Book>> out;
    std::ifstream in(filePath_);
    if (!in) {
        return out;
    }

    std::string line;

    
    if (std::getline(in, line)) {
        auto head = splitCSV(line);
        if (head.empty() || head[0] != "type") {
    
            in.clear();
            in.seekg(0);
        }
    }

    while (std::getline(in, line)) {
        if (line.empty()) continue;
        auto cells = splitCSV(line);
        if (cells.size() < 7) continue;

        // ترتیب ستون‌ها:
        // 0:type  1:id  2:title  3:author  4:publishDate  5:pages  6:status  7:extra1  8:extra2
        const std::string type = trim(cells[0]);
        int id = 0;              toIntSafe(cells[1], id);
        const std::string title  = (cells.size() > 2 ? cells[2] : "");
        const std::string author = (cells.size() > 3 ? cells[3] : "");
        const std::string pdate  = (cells.size() > 4 ? cells[4] : "");
        int pages = 0;           toIntSafe(cells[5], pages);
        const std::string status = (cells.size() > 6 ? cells[6] : "available");
        const std::string extra1 = (cells.size() > 7 ? cells[7] : "");
        const std::string extra2 = (cells.size() > 8 ? cells[8] : "");

        BookStatus st = BookStatus::Available;
        std::string stLower = status;
        std::transform(stLower.begin(), stLower.end(), stLower.begin(),
                       [](unsigned char c){ return std::tolower(c); });
        if (stLower == "borrowed")  st = BookStatus::Borrowed;
        else if (stLower == "reserved") st = BookStatus::Reserved;
        else if (stLower == "lost")     st = BookStatus::Lost;

        std::shared_ptr<Book> b;

        std::string tLower = type;
        std::transform(tLower.begin(), tLower.end(), tLower.begin(),
                       [](unsigned char c){ return std::tolower(c); });

        if (tLower == "textbook") {
            auto tb = std::make_shared<TextBook>(id, title, author,
                                                 pdate, pages,
                                                 /*level*/ extra1,
                                                 /*major*/ extra2);
            tb->setStatus(st);
            b = tb;

        } else if (tLower == "magazine") {
            int issue = 0;
            toIntSafe(extra1, issue);
            auto mg = std::make_shared<Magazine>(id, title, author,pdate, pages,
                                                 issue);
            mg->setStatus(st);
            b = mg;

        } else if (tLower == "referencebook") {
            auto rb = std::make_shared<ReferenceBook>(id, title, author,
                                                      /*category*/ "Reference",
                                                      pdate, pages);
            rb->setStatus(st);
            b = rb;

        } else {
            b = std::make_shared<Book>(id, title, author, pdate, pages);
            b->setStatus(st);
        }

        out.push_back(b);
    }

    return out;
}