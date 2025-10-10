#ifndef CONCURRENTQUEUE_CONCURRENTQUEUE_H
#define CONCURRENTQUEUE_CONCURRENTQUEUE_H
#include <algorithm>
#include <mutex>
#include <queue>

namespace SelfCQ {

    /**
     * @brief A thread-safe concurrent queue implementation based on std::queue.
     *
     * This class provides a thread-safe way to enqueue and dequeue elements. It uses
     * separate mutexes for enqueue and dequeue operations to improve performance
     * in multi-threaded environments.
     *
     * @tparam T The type of elements stored in the queue.
     */
    template<typename T>
    class concurrentQueue {
        std::queue<T> _queue;
        std::mutex _enqueue_mutex, _dequeue_mutex;
        std::condition_variable _condition;

    public:
        /**
         * @brief Type alias for the value type stored in the queue.
         */
        using value_type = T;

        /**
         * @brief Type alias for a reference to the value type.
         */
        using reference = value_type&;

        /**
         * @brief Type alias for size type used by the queue.
         */
        using size_type = std::size_t;

        /**
         * @brief Adds an element to the end of the queue.
         *
         * @param data The element to be added.
         */
        void enqueue(const value_type& data) {
            std::unique_lock<std::mutex> lock(this->_enqueue_mutex);

            this->_queue.push(data);
        }

        /**
         * @brief Enqueues a range of elements into the queue.
         *
         * @param begin An iterator pointing to the first element to enqueue.
         * @param end An iterator pointing to one past the last element to enqueue.
         */
        template<typename Iterator>
        void enqueue_bulk(Iterator begin, Iterator end) {
            std::for_each(begin, end, [this](const value_type& data) { this->enqueue(data); });
        }

        /**
         * @brief Enqueues a fixed number of elements starting from a given iterator.
         *
         * @param begin An iterator pointing to the first element to enqueue.
         * @param count The number of elements to enqueue.
         */
        template<typename Iterator>
        void enqueue_bulk(Iterator begin, const size_type count) {
            this->enqueue_bulk(begin, begin + count);
        }

        /**
         * @brief Waits until an element is available and then dequeues it.
         *
         * This function blocks until an element becomes available in the queue.
         *
         * @param data Reference to store the dequeued element.
         */
        void dequeue_wait(value_type& data) {
            std::unique_lock<std::mutex> lock(this->_dequeue_mutex);
            this->_condition.wait(lock, [this] { return !this->_queue.empty(); });

            data = this->_queue.front();
            this->_queue.pop();

            this->_condition.notify_one();
        }

        /**
         * @brief Attempts to dequeue an element from the queue.
         *
         * If the queue is empty, returns false without blocking.
         *
         * @param data Reference to store the dequeued element.
         * @return true if an element was successfully dequeued, false otherwise.
         */
        bool dequeue(value_type& data) {
            std::unique_lock<std::mutex> lock(this->_dequeue_mutex);

            if (this->empty(true))
                return false;

            data = this->_queue.front();
            this->_queue.pop();

            this->_condition.notify_one();

            return true;
        }

        // /**
        //  * @brief Retrieves the front element of the queue without removing it.
        //  *
        //  * @param data Reference to store the front element.
        //  * @return true if the queue is not empty and the front element was retrieved, false otherwise.
        //  */
        // bool front(reference data) {
        //     std::unique_lock<std::mutex> lock(this->_dequeue_mutex);
        //
        //     if (this->empty(true))
        //         return false;
        //
        //     data = this->_queue.front();
        //
        //     return true;
        // }

        /**
         * @brief Returns the number of elements currently in the queue.
         *
         * @note This method locks the dequeue mutex to ensure thread safety.
         * @return The current size of the queue.
         */
        size_type size() {
            std::lock_guard lock(this->_dequeue_mutex);

            return this->size(true);
        }

        /**
         * @brief Checks whether the queue is empty.
         *
         * @return true if the queue is empty, false otherwise.
         */
        bool empty()  {
            return this->size() == 0;
        }

    private:
        /**
         * @brief Internal helper method to get the size of the queue.
         *
         * @param flag Unused parameter for overload resolution.
         * @return The current size of the queue.
         */
        size_type size(bool) {
            std::unique_lock ulock(this->_enqueue_mutex);

            const size_type size = this->_queue.size();

            return size;
        }

        /**
         * @brief Internal helper method to check if the queue is empty.
         *
         * @param flag Unused parameter for overload resolution.
         * @return true if the queue is empty, false otherwise.
         */
        bool empty(bool)  {
            return this->size(true) == 0;
        }

        // For Compatibility
    public:
        /**
         * @brief Alias for size() method for compatibility with moodycamel::concurrent_queue.
         *
         * @return The current size of the queue.
         */
        size_type size_approx()  {
            return this->size();
        }

        /**
         * @brief Alias for dequeue() method for compatibility with moodycamel::concurrent_queue.
         *
         * @param data Reference to store the dequeued element.
         * @return true if an element was successfully dequeued, false otherwise.
         */
        bool try_dequeue(value_type& data) {
            return this->dequeue(data);
        }
    };
}

#endif //CONCURRENTQUEUE_CONCURRENTQUEUE_H