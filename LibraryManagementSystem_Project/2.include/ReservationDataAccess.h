#ifndef RESERVATIONDATAACCESS_H
#define RESERVATIONDATAACCESS_H

#include <string>
#include <vector>
#include "Reservation.h"

class ReservationDataAccess {
public:
    explicit ReservationDataAccess(const std::string& filePath);

    bool add(const Reservation& r);
    std::vector<Reservation> getAll() const;
    std::vector<Reservation> getByBook(int bookId) const;
    std::vector<Reservation> getByUser(int userId) const;
    bool update(const Reservation& r);
    bool remove(int userId, int bookId);

private:
    std::string filePath_;

    void saveToFile(const std::vector<Reservation>& items) const;
    std::vector<Reservation> loadFromFile() const;
};

#endif