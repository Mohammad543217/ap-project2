#include <iostream>
#include <memory>
#include <limits>
#include <string>
#include <vector>
#include <map>
#include <chrono>

#include "Book.h"
#include "TextBook.h"
#include "Magazine.h"
#include "ReferenceBook.h"
#include "BookManager.h"

#include "User.h"
#include "UserManager.h"

#include "Reservation.h"
#include "ReservationManager.h"

#include "ReportManager.h"
#include "ConfigManager.h"

static void clearIn() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}
static int readInt(const std::string& p) {
    for (;;) {
        std::cout << p;
        int v; if (std::cin >> v) { clearIn(); return v; }
        clearIn(); std::cout << "ورودی نامعتبر.\n";
    }
}
static double readDouble(const std::string& p) {
    for (;;) {
        std::cout << p;
        double v; if (std::cin >> v) { clearIn(); return v; }
        clearIn(); std::cout << "ورودی نامعتبر.\n";
    }
}
static std::string readLine(const std::string& p) {
    std::cout << p;
    std::string s; std::getline(std::cin, s);
    return s;
}

static void uiBooks(BookManager& bm) {
    for (;;) {
        std::cout << "\n=== مدیریت کتاب‌ها ===\n"
                  << "1) افزودن کتاب\n"
                  << "2) حذف کتاب\n"
                  << "3) ویرایش کتاب\n"
                  << "4) جستجو: عنوان\n"
                  << "5) جستجو: نویسنده\n"
                  << "6) جستجو: دسته‌بندی\n"
                  << "7) مرتب‌سازی عنوان\n"
                  << "8) مرتب‌سازی نویسنده\n"
                  << "9) مرتب‌سازی تاریخ انتشار\n"
                  << "10) نمایش همه\n"
                  << "0) بازگشت\n";
        int c = readInt("انتخاب: ");
        if (c==0) return;

        if (c==1) {
            std::cout << "نوع کتاب: 1) Book  2) TextBook  3) Magazine  4) ReferenceBook\n";
            int t = readInt("انتخاب: ");
            int id = readInt("ID: ");
            std::string title = readLine("عنوان: ");
            std::string author = readLine("نویسنده: ");
            std::string pdate = readLine("تاریخ انتشار (YYYY-MM-DD): ");
            int pages = readInt("تعداد صفحات: ");

            std::shared_ptr<Book> b;
            if (t == 2) {
               std::string level = readLine("سطح تحصیلی: ");
               std::string major = readLine("رشته: ");
               b = std::make_shared<TextBook>(id, title, author, pdate, pages, level, major);
            } else if (t == 3) {
                int issue = readInt("شماره انتشار: ");
                b = std::make_shared<Magazine>(id, title, author, pdate, pages, issue);
            } else if (t == 4) {
                std::string category = readLine("دسته‌بندی (مثلاً Reference/Science/...): ");
                b = std::make_shared<ReferenceBook>(id, title, author, category, pdate, pages);
            } else {
                b = std::make_shared<Book>(id, title, author, pdate, pages);
            }
            std::string err;
            if (bm.addBook(b,&err)) std::cout << "ثبت شد.\n";
            else std::cout << "خطا: " << err << "\n";
        }
        else if (c==2) {
            int id = readInt("ID برای حذف: ");
            std::cout << (bm.removeBook(id) ? "حذف شد.\n" : "پیدا نشد.\n");
        }
        else if (c==3) {
            int id = readInt("ID برای ویرایش: ");
            Book patch(id,"","","",0);
            std::string t = readLine("عنوان جدید (خالی=بدون تغییر): "); if(!t.empty()) patch.setTitle(t);
            t = readLine("نویسنده جدید (خالی=بدون تغییر): "); if(!t.empty()) patch.setAuthor(t);
            t = readLine("تاریخ انتشار جدید (خالی=بدون تغییر): "); if(!t.empty()) patch.setPublishDate(t);
            int p = readInt("صفحات جدید (<=0 بدون تغییر): "); if (p>0) patch.setPages(p);
            std::string err;
            std::cout << (bm.updateBook(id,patch,&err) ? "به‌روزرسانی شد.\n" : ("خطا: "+err+"\n"));
        }
        else if (c==4) {
            auto v = bm.searchByTitle(readLine("عبارت: "));
            for (auto& b: v) std::cout << b->getId() << " | " << b->getTitle() << " | " << b->getAuthor() << " | " << b->typeName() << "\n";
        }
        else if (c==5) {
            auto v = bm.searchByAuthor(readLine("نویسنده: "));for (auto& b: v) std::cout << b->getId() << " | " << b->getTitle() << " | " << b->getAuthor() << " | " << b->typeName() << "\n";
        }
        else if (c==6) {
            auto v = bm.searchByCategory(readLine("دسته (Book/TextBook/Magazine/ReferenceBook): "));
            for (auto& b: v) std::cout << b->getId() << " | " << b->getTitle() << " | " << b->getAuthor() << " | " << b->typeName() << "\n";
        }
        else if (c==7) {
            auto v = bm.sortByTitle(true);
            for (auto& b: v) std::cout << b->getId() << " | " << b->getTitle() << "\n";
        }
        else if (c==8) {
            auto v = bm.sortByAuthor(true);
            for (auto& b: v) std::cout << b->getId() << " | " << b->getAuthor() << " | " << b->getTitle() << "\n";
        }
        else if (c==9) {
            auto v = bm.sortByPublishDate(true);
            for (auto& b: v) std::cout << b->getId() << " | " << b->getPublishDate() << " | " << b->getTitle() << "\n";
        }
        else if (c==10) {
            for (auto& b: bm.listAll()) std::cout << b->getId() << " | " << b->getTitle() << " | " << b->getAuthor() << " | " << b->typeName() << "\n";
        }
    }
}

static void uiUsers(UserManager& um) {
    for (;;) {
        std::cout << "\n=== مدیریت کاربران ===\n"
                  << "1) افزودن کاربر\n"
                  << "2) حذف کاربر\n"
                  << "3) ویرایش کاربر\n"
                  << "4) جستجو نام\n"
                  << "5) جستجو ایمیل\n"
                  << "6) مرتب‌سازی نام\n"
                  << "7) نمایش همه\n"
                  << "8) افزودن جریمه\n"
                  << "9) پرداخت جریمه\n"
                  << "0) بازگشت\n";
        int c = readInt("انتخاب: ");
        if (c==0) return;

        if (c==1) {
            int id = readInt("ID: ");
            std::string name = readLine("نام: ");
            std::string email = readLine("ایمیل: ");
            std::cout << "نقش: 1) Regular  2) Librarian\n";
            int r = readInt("انتخاب: ");
            UserRole role = (r==2? UserRole::Librarian : UserRole::Regular);
            User u(id,name,email,role);
            std::string err;
            std::cout << (um.addUser(u,&err) ? "ثبت شد.\n" : ("خطا: "+err+"\n"));
        }
        else if (c==2) {
            int id = readInt("ID: ");
            std::cout << (um.removeUser(id) ? "حذف شد.\n" : "پیدا نشد.\n");
        }
        else if (c==3) {
            int id = readInt("ID: ");
            std::string name = readLine("نام جدید (خالی=بدون تغییر): ");
            std::string email = readLine("ایمیل جدید (خالی=بدون تغییر): ");
            std::cout << "نقش جدید: 1) بدون تغییر  2) Regular  3) Librarian\n";
            int rc = readInt("انتخاب: ");
            User patch(id, name, email, (rc==3?UserRole::Librarian:UserRole::Regular));
            if (rc==1) patch.setRole(um.getByIdOrInvalid(id).getRole());
            std::string err;
            std::cout << (um.updateUser(id,patch,&err) ? "به‌روزرسانی شد.\n" : ("خطا: "+err+"\n"));
        }
        else if (c==4) {
            auto v = um.searchByName(readLine("نام: "));
            for (auto& u: v) std::cout << u.getId() << " | " << u.getName() << " | " << u.getEmail() << "\n";
        }
        else if (c==5) {
            auto v = um.searchByEmail(readLine("ایمیل: "));
            for (auto& u: v) std::cout << u.getId() << " | " << u.getName() << " | " << u.getEmail() << "\n";
        }
        else if (c==6) {
            auto v = um.sortByName(true);
            for (auto& u: v) std::cout << u.getId() << " | " << u.getName() << " | " << u.getEmail() << "\n";
        }
        else if (c==7) {
            for (auto& u: um.listAll()) std::cout << u.getId() << " | " << u.getName() << " | " << u.getEmail() << " | Fines=" << u.getFines() << "\n";
        }
        else if (c==8) {
            int id = readInt("ID: ");
            double amt = readDouble("مبلغ: ");
            std::string err;
            std::cout << (um.addFine(id,amt,&err) ? "انجام شد.\n" : ("خطا: "+err+"\n"));
        }
        else if (c==9) {int id = readInt("ID: ");
            double amt = readDouble("مبلغ: ");
            std::string err;
            std::cout << (um.payFine(id,amt,&err) ? "پرداخت شد.\n" : ("خطا: "+err+"\n"));
        }
    }
}

static void uiReservations(ReservationManager& rm) {
    for (;;) {
        std::cout << "\n=== سیستم رزرو ===\n"
                  << "1) رزرو کتاب\n"
                  << "2) لغو رزرو\n"
                  << "3) اعلام بازگشت کتاب\n"
                  << "4) پذیرش رزرو (نفر اول)\n"
                  << "5) نمایش صف کتاب\n"
                  << "0) بازگشت\n";
        int c = readInt("انتخاب: ");
        if (c==0) return;

        if (c==1) {
            int uid = readInt("ID کاربر: ");
            int bid = readInt("ID کتاب: ");
            std::string err;
            std::cout << (rm.reserve(uid,bid,&err) ? "در صف رزرو قرار گرفت.\n" : ("خطا: "+err+"\n"));
        }
        else if (c==2) {
            int uid = readInt("ID کاربر: ");
            int bid = readInt("ID کتاب: ");
            std::cout << (rm.cancel(uid,bid) ? "لغو شد.\n" : "رزروی یافت نشد.\n");
        }
        else if (c==3) {
            int bid = readInt("ID کتاب: ");
            rm.onBookReturned(bid);
            std::cout << "اعلان به نفر اول (در صورت وجود) ارسال شد.\n";
        }
        else if (c==4) {
            int uid = readInt("ID کاربر: ");
            int bid = readInt("ID کتاب: ");
            std::string err;
            std::cout << (rm.acceptNotified(uid,bid,&err) ? "رزرو تایید و انجام شد.\n" : ("خطا: "+err+"\n"));
        }
        else if (c==5) {
            int bid = readInt("ID کتاب: ");
            auto q = rm.getQueue(bid);
            std::cout << "طول صف: " << q.size() << "\n";
            for (auto& r : q) std::cout << "user=" << r.getUserId() << " status=" << (int)r.getStatus() << "\n";
        }
    }
}

int main() {
    ConfigManager cfg("4.config/application.ini");
    cfg.load();

    BookManager bookManager;
    UserManager userManager;
    ReservationManager reservationManager;
    reservationManager.setNotifier([](int uid,int bid,const std::string& msg){
        std::cout << "[اعلان] کاربر " << uid << " | کتاب " << bid << " -> " << msg << "\n";
    });

    for (;;) {
        std::cout << "\n==== سامانه مدیریت کتابخانه ====\n"
                  << "1) مدیریت کتاب‌ها\n"
                  << "2) مدیریت کاربران\n"
                  << "3) سیستم رزرو\n"
                  << "0) خروج\n";
        int c = readInt("انتخاب: ");
        if (c==0) break;
        if (c==1) uiBooks(bookManager);
        else if (c==2) uiUsers(userManager);
        else if (c==3) uiReservations(reservationManager);
    }
    return 0;
}


