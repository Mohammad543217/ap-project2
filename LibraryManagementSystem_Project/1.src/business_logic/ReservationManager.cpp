#include "ReservationManager.h"
#include <algorithm>

// ---- نسخه‌های مبتنی بر string ----
bool ReservationManager::reserve(const std::string& userId, const std::string& bookId, std::string* err) {
    auto& users = usersInQueue_[bookId];
    if (users.find(userId) != users.end()) {
        if (err) *err = "کاربر از قبل در صف همین کتاب است.";
        return false;
    }
    users.insert(userId);
    // اگر سازنده‌ی Reservation شما (userId, bookId, ReservationStatus) است:
    // برای مستقل بودن از نام مقادیر enum (مثل Pending)، از مقدار پیش‌فرض استفاده می‌کنیم.
    queues_[bookId].emplace_back(userId, bookId, ReservationStatus{});
    return true;
}

bool ReservationManager::cancel(const std::string& userId, const std::string& bookId) {
    auto itQ = queues_.find(bookId);
    if (itQ == queues_.end()) return false;

    auto& dq = itQ->second;
    bool removed = false;
    for (auto it = dq.begin(); it != dq.end(); ++it) {
        if (it->getUserId() == userId) {
            dq.erase(it);
            removed = true;
            break;
        }
    }
    if (removed) {
        auto itSet = usersInQueue_.find(bookId);
        if (itSet != usersInQueue_.end()) {
            itSet->second.erase(userId);
        }
    }
    return removed;
}

void ReservationManager::onBookReturned(const std::string& bookId) {
    auto it = queues_.find(bookId);
    if (it == queues_.end() || it->second.empty()) return;

    auto& res = it->second.front();
    // اینجا فقط notify می‌کنیم (بدون وابستگی به enum خاص)
    if (stringNotifier_) {
        stringNotifier_(res.getUserId(), res.getBookId(), "کتاب آمادهٔ امانت است. لطفاً تایید کنید.");
    }
}

bool ReservationManager::acceptNotified(const std::string& userId, const std::string& bookId, std::string* err) {
    auto it = queues_.find(bookId);
    if (it == queues_.end() || it->second.empty()) {
        if (err) *err = "صف خالی است.";
        return false;
    }
    auto& frontRes = it->second.front();
    if (frontRes.getUserId() != userId) {
        if (err) *err = "این کاربر در ابتدای صف نیست.";
        return false;
    }

    // انجام رزرو (برداشتن از صف)
    it->second.pop_front();
    usersInQueue_[bookId].erase(userId);
    return true;
}

std::deque<Reservation> ReservationManager::getQueue(const std::string& bookId) const {
    auto it = queues_.find(bookId);
    if (it == queues_.end()) return {};
    return it->second; // کپی
}

void ReservationManager::setNotifier(std::function<void(int,int,const std::string&)> notifier) {
    if (!notifier) { stringNotifier_ = nullptr; return; }
    // تبدیل: string -> int (اگر ممکن باشد)
    stringNotifier_ = [notifier](const std::string& suid,
                                 const std::string& sbid,
                                 const std::string& msg) {
        int uid = 0, bid = 0;
        try { uid = std::stoi(suid); } catch (...) {}
        try { bid = std::stoi(sbid); } catch (...) {}
        notifier(uid, bid, msg);
    };
}

// ---- Overloadهای int که main.cpp صدا می‌زند ----
bool ReservationManager::reserve(int userId, int bookId, std::string* err) {
    return reserve(toStr(userId), toStr(bookId), err);
}
bool ReservationManager::cancel(int userId, int bookId) {
    return cancel(toStr(userId), toStr(bookId));
}
void ReservationManager::onBookReturned(int bookId) {
    onBookReturned(toStr(bookId));
}
bool ReservationManager::acceptNotified(int userId, int bookId, std::string* err) {
    return acceptNotified(toStr(userId), toStr(bookId), err);
}
std::deque<Reservation> ReservationManager::getQueue(int bookId) const {
    return getQueue(toStr(bookId));
}

// ---- API ساده‌تر (در صورت نیاز در جاهای دیگر) ----
bool ReservationManager::enqueue(const std::string& userId, const std::string& bookId) {
    std::string dummy;
    return reserve(userId, bookId, &dummy);
}

bool ReservationManager::dequeue(const std::string& bookId) {
    auto it = queues_.find(bookId);
    if (it == queues_.end() || it->second.empty()) return false;
    auto userId = it->second.front().getUserId();
    it->second.pop_front();usersInQueue_[bookId].erase(userId);
    return true;
}

Reservation* ReservationManager::front(const std::string& bookId) {
    auto it = queues_.find(bookId);
    if (it == queues_.end() || it->second.empty()) return nullptr;
    return &it->second.front();
}

bool ReservationManager::isUserInQueue(const std::string& userId, const std::string& bookId) const {
    auto it = usersInQueue_.find(bookId);
    if (it == usersInQueue_.end()) return false;
    return it->second.find(userId) != it->second.end();
}

size_t ReservationManager::queueSize(const std::string& bookId) const {
    auto it = queues_.find(bookId);
    if (it == queues_.end()) return 0;
    return it->second.size();
}