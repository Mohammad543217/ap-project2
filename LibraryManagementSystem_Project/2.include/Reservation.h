#pragma once
#include <string>
#include <optional>
#include <ctime>

enum class ReservationStatus {
    Waiting,
    Notified,
    Fulfilled,
    Cancelled,
    Expired
};

class Reservation {
public:
    Reservation() = default;
    Reservation(const std::string& userId,
                const std::string& bookId,
                ReservationStatus status = ReservationStatus::Waiting);

    const std::string& getUserId() const noexcept;
    const std::string& getBookId() const noexcept;
    ReservationStatus getStatus() const noexcept;
    std::time_t getCreatedAt() const noexcept;
    const std::optional<std::time_t>& getNotifyUntil() const noexcept;

    void setStatus(ReservationStatus s) noexcept;
    void setCreatedAt(std::time_t t) noexcept;
    void setNotifyUntil(std::time_t t);
    void clearNotifyUntil();

private:
    std::string userId_;
    std::string bookId_;
    ReservationStatus status_ { ReservationStatus::Waiting };
    std::time_t createdAt_ { 0 };
    std::optional<std::time_t> notifyUntil_;
};