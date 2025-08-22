#ifndef RESERVATION_MANAGER_H
#define RESERVATION_MANAGER_H

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <deque>
#include <functional>
#include "Reservation.h"

class ReservationManager {
public:

    bool reserve(int userId, int bookId, std::string* err);
    bool reserve(const std::string& userId, const std::string& bookId, std::string* err);

    bool cancel(int userId, int bookId);
    bool cancel(const std::string& userId, const std::string& bookId);

    void onBookReturned(int bookId);
    void onBookReturned(const std::string& bookId);

    bool acceptNotified(int userId, int bookId, std::string* err);
    bool acceptNotified(const std::string& userId, const std::string& bookId, std::string* err);

    std::deque<Reservation> getQueue(int bookId) const;
    std::deque<Reservation> getQueue(const std::string& bookId) const;

    void setNotifier(std::function<void(int,int,const std::string&)> notifier);

    bool enqueue(const std::string& userId, const std::string& bookId);
    bool dequeue(const std::string& bookId);
    Reservation* front(const std::string& bookId);
    bool isUserInQueue(const std::string& userId, const std::string& bookId) const;
    size_t queueSize(const std::string& bookId) const;

private:
    static std::string toStr(int x) { return std::to_string(x); }

    std::unordered_map<std::string, std::unordered_set<std::string>> usersInQueue_;
    std::unordered_map<std::string, std::deque<Reservation>> queues_;

    std::function<void(const std::string&, const std::string&, const std::string&)> stringNotifier_;
};

#endif



