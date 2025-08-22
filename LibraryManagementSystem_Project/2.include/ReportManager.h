#ifndef REPORT_MANAGER_H
#define REPORT_MANAGER_H

#include <vector>
#include <string>
#include <chrono>
#include <cstddef>


struct LoanRecord {
    int bookId{};
    std::chrono::system_clock::time_point borrowAt{};
    std::chrono::system_clock::time_point dueAt{};
    std::chrono::system_clock::time_point returnAt{};
    bool returned{false};
};

struct ViewEvent {
    int bookId{};
};

struct WaitRecord {
    int bookId{};
    std::chrono::seconds wait{0};
};

struct QueueSnapshot {
    std::size_t totalQueueLength{0};
};


struct PopularBook {
    int bookId{};
    std::size_t borrows{0};
    std::size_t views{0};
    double avgWaitHours{0.0};
    double score{0.0};
};

class User;
struct UserFineEntry {
    int userId{};
    std::string userName;
    double totalFines{0.0};
};


struct Analytics {
    double avgLoanDays{0.0};    
    double onTimeReturnRate{0.0};
    double avgQueueLength{0.0};
};


class ReportManager {
public:

    std::vector<PopularBook>
    computePopularBooks(const std::vector<LoanRecord>& loans,
                        const std::vector<ViewEvent>& views,
                        const std::vector<WaitRecord>& waits,
                        std::size_t topN = 10) const;


    std::vector<UserFineEntry>
    topFinedUsers(const std::vector<User>& users, std::size_t topN = 10) const;


    Analytics
    computeAnalytics(const std::vector<LoanRecord>& loans,
                     const std::vector<QueueSnapshot>& queueSnapshots) const;
};

#endif