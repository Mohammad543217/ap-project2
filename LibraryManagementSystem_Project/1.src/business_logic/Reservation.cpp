#include "Reservation.h"

Reservation::Reservation(const std::string& userId,
                         const std::string& bookId,
                         ReservationStatus status)
    : userId_(userId),
      bookId_(bookId),
      status_(status),
      createdAt_(std::time(nullptr)) {}

const std::string& Reservation::getUserId() const noexcept { return userId_; }
const std::string& Reservation::getBookId() const noexcept { return bookId_; }
ReservationStatus Reservation::getStatus() const noexcept { return status_; }
std::time_t Reservation::getCreatedAt() const noexcept { return createdAt_; }
const std::optional<std::time_t>& Reservation::getNotifyUntil() const noexcept { return notifyUntil_; }

void Reservation::setStatus(ReservationStatus s) noexcept { status_ = s; }
void Reservation::setCreatedAt(std::time_t t) noexcept { createdAt_ = t; }
void Reservation::setNotifyUntil(std::time_t t) { notifyUntil_ = t; }
void Reservation::clearNotifyUntil() { notifyUntil_.reset(); }
