#pragma once

#include <deque>

// Thread safe queue for storing messages which prevents accessing same queue
// simultaneously.
template <typename T>
class MessageQueue
{
public:
  MessageQueue() = default;

  MessageQueue(const MessageQueue<T> &) = delete;

  virtual ~MessageQueue() { clear(); }

  // Returns and maintains item at front of Queue
  const T &front()
  {
    std::scoped_lock lock(muxQueueM);
    return deqQueueM.front();
  }

  // Returns and maintains item at back of Queue
  const T &back()
  {
    std::scoped_lock lock(muxQueueM);
    return deqQueueM.back();
  }

  // Removes and returns item from front of Queue
  T pop_front()
  {
    std::scoped_lock lock(muxQueueM);
    auto t = std::move(deqQueueM.front());
    deqQueueM.pop_front();
    return t;
  }

  // Removes and returns item from back of Queue
  T pop_back()
  {
    std::scoped_lock lock(muxQueueM);
    auto t = std::move(deqQueueM.back());
    deqQueueM.pop_back();
    return t;
  }

  // Adds an item to back of Queue
  void push_back(const T &item)
  {
    std::scoped_lock lock(muxQueueM);
    deqQueueM.emplace_back(std::move(item));

    std::unique_lock<std::mutex> ul(muxBlockingM);
    cvBlockingM.notify_one();
  }

  // Adds an item to front of Queue
  void push_front(const T &item)
  {
    std::scoped_lock lock(muxQueueM);
    deqQueueM.emplace_front(std::move(item));

    std::unique_lock<std::mutex> ul(muxBlockingM);
    cvBlockingM.notify_one();
  }

  // Returns true if Queue has no items
  bool empty()
  {
    std::scoped_lock lock(muxQueueM);
    return deqQueueM.empty();
  }

  // Returns number of items in Queue
  size_t count()
  {
    std::scoped_lock lock(muxQueueM);
    return deqQueueM.size();
  }

  // Clears Queue
  void clear()
  {
    std::scoped_lock lock(muxQueueM);
    deqQueueM.clear();
  }

  void wait()
  {
    while (empty())
    {
      std::unique_lock<std::mutex> ul(muxBlockingM);
      cvBlockingM.wait(ul);
    }
  }

protected:
  std::mutex muxQueueM;
  std::deque<T> deqQueueM;
  std::condition_variable cvBlockingM;
  std::mutex muxBlockingM;
};