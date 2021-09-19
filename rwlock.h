#pragma once
#include <mutex>
#include <condition_variable>
#include <stdio.h>

namespace ak {
    class RwLock;

    class ReadGuard {
        friend RwLock;
        public:
            ~ReadGuard();
        private:
            ReadGuard(RwLock& lock);
            RwLock& lock;
    };

    inline ReadGuard::ReadGuard(RwLock& lock) : lock{lock} {}

    class WriteGuard {
        friend RwLock;
        public:
            ~WriteGuard();
        private:
            WriteGuard(RwLock& lock);
            RwLock& lock;
    };

    inline WriteGuard::WriteGuard(RwLock& lock) : lock{lock} {}

    class RwLock {
        friend WriteGuard;
        friend ReadGuard;
        private:
            std::mutex mtx;
            std::condition_variable cond_var;
            int reader_count;
            int writer_count;
        public:
            RwLock();
            ReadGuard read();
            WriteGuard write();
    };

    inline RwLock::RwLock() : reader_count{}, writer_count{} {}

    inline ReadGuard RwLock::read() {
        std::unique_lock<std::mutex> lock_guard(mtx);

        while(writer_count > 0) {
            cond_var.wait(lock_guard);
        }

        ++reader_count;
        ReadGuard guard(*this);
        return guard;
    }

    inline WriteGuard RwLock::write() {
        std::unique_lock<std::mutex> lock_guard(mtx);

        while(reader_count > 0 || writer_count > 0) {
            cond_var.wait(lock_guard);
        }

        ++writer_count;
        WriteGuard guard(*this);
        return guard;
    }

    inline ReadGuard::~ReadGuard() {
        std::unique_lock<std::mutex> lock_guard(lock.mtx);
        --lock.reader_count;
        if(lock.reader_count < 1) {
            lock.cond_var.notify_one();
        }
    }

    inline WriteGuard::~WriteGuard() {
        std::unique_lock<std::mutex> lock_guard(lock.mtx);
        --lock.writer_count;
        lock.cond_var.notify_all();
    }
}
