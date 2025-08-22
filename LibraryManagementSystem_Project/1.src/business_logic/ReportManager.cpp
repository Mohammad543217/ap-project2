#include "ReportManager.h"
#include "User.h"

#include <algorithm>
#include <numeric>
#include <unordered_map>
#include <cmath>
#include <limits>
#include <chrono>

namespace {

// نسخه درست شده: فقط duration ÷ integer
template<typename Duration>
Duration safeDiv(Duration num, long long den, Duration fallback = Duration{}) {
    return (den == 0) ? fallback : (num / den);
}

template<typename Duration>
double toHours(Duration d) {
    return std::chrono::duration_cast<std::chrono::duration<double, std::ratio<3600>>>(d).count();
}

template<typename Duration>
double toDays(Duration d) {
    return std::chrono::duration_cast<std::chrono::duration<double, std::ratio<86400>>>(d).count();
}

static inline double popularityScore(std::size_t borrows, std::size_t views, double avgWaitHours) {
    const double wB = 0.6, wV = 0.3, wW = 0.1;
    return (wB * static_cast<double>(borrows)) +
           (wV * static_cast<double>(views)) +
           (wW * avgWaitHours);
}

} // ← آکولاد اضافی اینجا بسته می‌شود، نه زودتر

std::vector<PopularBook>
ReportManager::computePopularBooks(const std::vector<LoanRecord>& loans,
                                   const std::vector<ViewEvent>& views,
                                   const std::vector<WaitRecord>& waits,
                                   std::size_t topN) const
{

    std::unordered_map<int, std::size_t> borrowCount;
    for (const auto& lr : loans) {
        ++borrowCount[lr.bookId];
    }

    std::unordered_map<int, std::size_t> viewCount;
    for (const auto& ve : views) {
        ++viewCount[ve.bookId];
    }

    std::unordered_map<int, std::pair<std::chrono::seconds, std::size_t>> waitAgg;
    for (const auto& w : waits) {
        auto& agg = waitAgg[w.bookId];
        agg.first  += w.wait;
        agg.second += 1;
    }

    std::unordered_map<int, bool> allIds;
    for (auto& kv : borrowCount) allIds[kv.first] = true;
    for (auto& kv : viewCount)   allIds[kv.first] = true;
    for (auto& kv : waitAgg)     allIds[kv.first] = true;

    std::vector<PopularBook> out;
    out.reserve(allIds.size());

    for (auto& kv : allIds) {
        int bid = kv.first;
        std::size_t b = borrowCount.count(bid) ? borrowCount[bid] : 0;
        std::size_t v = viewCount.count(bid) ? viewCount[bid] : 0;
        double avgWaitH = 0.0;
        if (waitAgg.count(bid) && waitAgg[bid].second > 0) {
            auto totalWait = waitAgg[bid].first; 
            auto cnt       = static_cast<long long>(waitAgg[bid].second);
            auto avgWait   = safeDiv(totalWait, cnt, totalWait); // درست شد
            avgWaitH       = toHours(avgWait);
        }
        PopularBook pb{bid, b, v, avgWaitH, 0.0};
        pb.score = popularityScore(pb.borrows, pb.views, pb.avgWaitHours);
        out.push_back(pb);
    }

    std::stable_sort(out.begin(), out.end(), [](const PopularBook& a, const PopularBook& b) {
        if (a.score != b.score)     return a.score > b.score;
        if (a.borrows != b.borrows) return a.borrows > b.borrows;
        return a.views > b.views;
    });

    if (topN > 0 && out.size() > topN) out.resize(topN);
    return out;
}

std::vector<UserFineEntry>
ReportManager::topFinedUsers(const std::vector<User>& users, std::size_t topN) const
{
    std::vector<UserFineEntry> arr;
    arr.reserve(users.size());
    for (const auto& u : users) {
        arr.push_back(UserFineEntry{u.getId(), u.getName(), u.getFines()});
    }

    std::stable_sort(arr.begin(), arr.end(), [](const UserFineEntry& a, const UserFineEntry& b){
        return a.totalFines > b.totalFines;
    });

    if (topN > 0 && arr.size() > topN) arr.resize(topN);
    return arr;
}

Analytics ReportManager::computeAnalytics(const std::vector<LoanRecord>& loans,
                                          const std::vector<QueueSnapshot>& queueSnapshots) const
{
    Analytics a{};
    using seconds = std::chrono::seconds;

    seconds totalLoan = seconds::zero();
    std::size_t loanCnt = 0;

    std::size_t onTime = 0, totalReturned = 0;

    for (const auto& lr : loans) {
        if (lr.returned) {auto dur = std::chrono::duration_cast<seconds>(lr.returnAt - lr.borrowAt);
            if (dur.count() > 0) {
                totalLoan += dur;
                ++loanCnt;
            }
            ++totalReturned;
            if (lr.returnAt <= lr.dueAt) ++onTime;
        }
    }

    if (loanCnt > 0) {
        auto avgLoan = safeDiv(totalLoan, static_cast<long long>(loanCnt), totalLoan);
        a.avgLoanDays = toDays(avgLoan);
    } else {
        a.avgLoanDays = 0.0;
    }

    a.onTimeReturnRate = (totalReturned == 0)
                         ? 0.0
                         : (static_cast<double>(onTime) / static_cast<double>(totalReturned));

    double sumLen = 0.0;
    std::size_t cnt = 0;
    for (const auto& snap : queueSnapshots) {
        sumLen += static_cast<double>(snap.totalQueueLength);
        ++cnt;
    }
    a.avgQueueLength = (cnt == 0) ? 0.0 : (sumLen / static_cast<double>(cnt));

    return a;
}




