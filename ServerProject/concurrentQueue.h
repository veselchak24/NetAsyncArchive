#ifndef CONCURRENTQUEUE_CONCURRENTQUEUE_H
#define CONCURRENTQUEUE_CONCURRENTQUEUE_H
#include <algorithm>
#include <mutex>
#include <queue>

namespace SelfCQ {
    template<typename T>
    class concurrentQueue {
        std::queue<T> _queue;
        std::mutex _enqueue_mutex, _dequeue_mutex;
        std::condition_variable _condition;

    public:
        using value_type = T;
        using reference = value_type&;
        using size_type = std::size_t;

        void enqueue(const value_type& data) {
            std::unique_lock<std::mutex> lock(this->_enqueue_mutex);

            this->_queue.push(data);
        }

        template<typename Iterator>
        void enqueue_bulk(Iterator begin, Iterator end) {
            std::for_each(begin, end, [this](const value_type& data) { this->enqueue(data); });
        }

        template<typename Iterator>
        void enqueue_bulk(Iterator begin, const size_type count) {
            this->enqueue_bulk(begin, begin + count);
        }

        void dequeue_wait(value_type& data) {
            std::unique_lock<std::mutex> lock(this->_dequeue_mutex);
            this->_condition.wait(lock, [this] { return !this->_queue.empty(); });

            data = this->_queue.front();
            this->_queue.pop();

            this->_condition.notify_one();
        }

        bool dequeue(value_type& data) {
            std::unique_lock<std::mutex> lock(this->_dequeue_mutex);

            if (this->empty(true))
                return false;

            data = this->_queue.front();
            this->_queue.pop();

            this->_condition.notify_one();

            return true;
        }

        bool front(reference data) {
            std::unique_lock<std::mutex> lock(this->_dequeue_mutex);

            if (this->empty(true))
                return false;

            data = this->_queue.front();

            return true;
        }

        size_type size() {
            // std::unique_lock ulock(this->_enqueue_mutex);
            std::lock_guard lock(this->_dequeue_mutex);
            //
            // const size_type size = this->_queue.size();
            //
            // return size;

            return this->size(true);
        }

        bool empty()  {
            return this->size() == 0;
        }

    private:
        size_type size(bool) {
            std::unique_lock ulock(this->_enqueue_mutex);

            const size_type size = this->_queue.size();

            return size;
        }

        bool empty(bool)  {
            return this->size(true) == 0;
        }

        // For Compatibility
    public:
        size_type size_approx()  {
            return this->size();
        }

        bool try_dequeue(value_type& data) {
            return this->dequeue(data);
        }
    };
}

#endif //CONCURRENTQUEUE_CONCURRENTQUEUE_H
