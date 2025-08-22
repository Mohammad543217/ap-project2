#include "ReservationDataAccess.h"

#include <fstream>
#include <sstream>
#include <algorithm>
#include <string>

namespace {

inline std::string trim(std::string s) {
    auto notSpace = [](int ch){ return !std::isspace(ch); };
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), notSpace));
    s.erase(std::find_if(s.rbegin(), s.rend(), notSpace).base(), s.end());
    return s;
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

inline std::string statusToString(ReservationStatus st) {
    return std::to_string(static_cast<int>(st));
}

inline ReservationStatus stringToStatus(const std::string& s) {
    int v = 0;
    if (!toIntSafe(trim(s), v)) v = 0; 
    return static_cast<ReservationStatus>(v);
}

} 

ReservationDataAccess::ReservationDataAccess(const std::string& filePath)
: filePath_(filePath) {}


bool ReservationDataAccess::add(const Reservation& r) {
    auto all = loadFromFile();


    auto exists = std::any_of(all.begin(), all.end(), [&](const Reservation& x){
        return x.getUserId() == r.getUserId() && x.getBookId() == r.getBookId();
    });
    if (exists) return false;

    all.push_back(r);
    saveToFile(all);
    return true;
}

std::vector<Reservation> ReservationDataAccess::getAll() const {
    return loadFromFile();
}

std::vector<Reservation> ReservationDataAccess::getByBook(int bookId) const {
    auto all = loadFromFile();
    std::vector<Reservation> out;
    for (const auto& r : all) {
        int bid = 0;
        if (toIntSafe(r.getBookId(), bid) && bid == bookId) {
            out.push_back(r);
        }
    }
    return out;
}

std::vector<Reservation> ReservationDataAccess::getByUser(int userId) const {
    auto all = loadFromFile();
    std::vector<Reservation> out;
    for (const auto& r : all) {
        int uid = 0;
        if (toIntSafe(r.getUserId(), uid) && uid == userId) {
            out.push_back(r);
        }
    }
    return out;
}

bool ReservationDataAccess::update(const Reservation& r) {
    auto all = loadFromFile();
    bool found = false;
    for (auto& x : all) {
        if (x.getUserId() == r.getUserId() && x.getBookId() == r.getBookId()) {
            // تنها چیزی که منطقی‌ست در به‌روزرسانی عوض شود، وضعیت است
            x.setStatus(r.getStatus());
            found = true;
            break;
        }
    }
    if (!found) return false;
    saveToFile(all);
    return true;
}

bool ReservationDataAccess::remove(int userId, int bookId) {
    auto all = loadFromFile();
    auto before = all.size();

    all.erase(std::remove_if(all.begin(), all.end(), [&](const Reservation& r){
        int uid = 0, bid = 0;
        return toIntSafe(r.getUserId(), uid) && toIntSafe(r.getBookId(), bid)
               && uid == userId && bid == bookId;
    }), all.end());

    if (all.size() == before) return false;
    saveToFile(all);
    return true;
}


void ReservationDataAccess::saveToFile(const std::vector<Reservation>& items) const {
    std::ofstream out(filePath_, std::ios::trunc);
    if (!out) {

        return;
    }


    out << "userId,bookId,status\n";
    for (const auto& r : items) {

        out << r.getUserId() << ","
            << r.getBookId() << ","
            << statusToString(r.getStatus()) << "\n";
    }
}

std::vector<Reservation> ReservationDataAccess::loadFromFile() const {
    std::vector<Reservation> out;
    std::ifstream in(filePath_);if (!in) {

        return out;
    }

    std::string line;
    bool first = true;
    while (std::getline(in, line)) {
        if (line.empty()) continue;

        if (first) {
            first = false;

            std::istringstream hs(line);
            std::string c1, c2, c3;
            if (std::getline(hs, c1, ',') && std::getline(hs, c2, ',') && std::getline(hs, c3, ',')) {
                if (trim(c1) == "userId" && trim(c2) == "bookId") {

                    continue;
                }
            }
        
            
        }

        std::istringstream ss(line);
        std::string suid, sbid, sstat;
        if (!std::getline(ss, suid, ',')) continue;
        if (!std::getline(ss, sbid, ',')) continue;
        if (!std::getline(ss, sstat, ',')) sstat = "0";

        suid = trim(suid);
        sbid = trim(sbid);
        sstat = trim(sstat);

        ReservationStatus st = stringToStatus(sstat);

        
        Reservation r(suid, sbid, st);
        out.push_back(r);
    }

    return out;
}