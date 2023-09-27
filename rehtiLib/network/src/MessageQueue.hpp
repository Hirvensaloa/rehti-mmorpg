#pragma once

#include "Message.hpp"
#include <deque>

// Thread safe queue of incoming messages
class MessageQueue
{
public:
  MessageQueue() = default;

  MessageQueue(const MessageQueue &) = delete;

  virtual ~MessageQueue() { clear(); }

  const Message &front()
  {
    std::scoped_lock lock(muxQueueM);
    return deqQueueM.front();
  }

  const Message &back()
  {
    std::scoped_lock lock(muxQueueM);
    return deqQueueM.back();
  }

  Message pop_front()
  {
    std::scoped_lock lock(muxQueueM);
    auto t = std::move(deqQueueM.front());
    deqQueueM.pop_front();
    return t;
  }

  Message pop_back()
  {
    std::scoped_lock lock(muxQueueM);
    auto t = std::move(deqQueueM.back());
    deqQueueM.pop_back();
    return t;
  }

  void push_back(const Message &item)
  {
    std::scoped_lock lock(muxQueueM);
    deqQueueM.emplace_back(std::move(item));

    std::unique_lock<std::mutex> ul(muxBlockingM);
    cvBlockingM.notify_one();
  }

  void push_front(const Message &item)
  {
    std::scoped_lock lock(muxQueueM);
    deqQueueM.emplace_front(std::move(item));

    std::unique_lock<std::mutex> ul(muxBlockingM);
    cvBlockingM.notify_one();
  }

  bool empty()
  {
    std::scoped_lock lock(muxQueueM);
    return deqQueueM.empty();
  }

  size_t count()
  {
    std::scoped_lock lock(muxQueueM);
    return deqQueueM.size();
  }

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
  std::deque<Message> deqQueueM;
  std::condition_variable cvBlockingM;
  std::mutex muxBlockingM;
};