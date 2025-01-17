/***************************************************************************
 *
 * Project         _____    __   ____   _      _
 *                (  _  )  /__\ (_  _)_| |_  _| |_
 *                 )(_)(  /(__)\  )( (_   _)(_   _)
 *                (_____)(__)(__)(__)  |_|    |_|
 *
 *
 * Copyright 2018-present, Leonid Stryzhevskyi <lganzzzo@gmail.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ***************************************************************************/

#ifndef oatpp_async_Executor_hpp
#define oatpp_async_Executor_hpp

#include "./Processor.hpp"

#include "oatpp/core/concurrency/SpinLock.hpp"
#include "oatpp/core/concurrency/Thread.hpp"

#include "oatpp/core/collection/LinkedList.hpp"

#include <tuple>
#include <mutex>
#include <condition_variable>

namespace oatpp { namespace async {
  
class Executor {
private:
  
  class TaskSubmission {
  public:
    virtual ~TaskSubmission() {};
    virtual AbstractCoroutine* createCoroutine() = 0;
  };
  
  /**
   * Sequence generating templates
   * used to convert tuple to parameters pack
   * Example: expand SequenceGenerator<3>:
   * // 2, 2, {} // 1, 1, {2} // 0, 0, {1, 2} // 0, {0, 1, 2}
   * where {...} is int...S
   */
  template<int ...> struct IndexSequence {};
  template<int N, int ...S> struct SequenceGenerator : SequenceGenerator <N - 1, N - 1, S...> {};
  template<int ...S>
  struct SequenceGenerator<0, S...> {
    typedef IndexSequence<S...> type;
  };
  
  template<typename CoroutineType, typename ... Args>
  class SubmissionTemplate : public TaskSubmission {
  private:
    std::tuple<Args...> m_params;
  public:
    
    SubmissionTemplate(Args... params)
      : m_params(std::make_tuple(params...))
    {}
    
    virtual AbstractCoroutine* createCoroutine() {
      return creator(typename SequenceGenerator<sizeof...(Args)>::type());
    }
    
    template<int ...S>
    AbstractCoroutine* creator(IndexSequence<S...>) {
      return CoroutineType::getBench().obtain(std::get<S>(m_params) ...);
    }
    
  };
  
  class SubmissionProcessor : public oatpp::concurrency::Runnable {
  private:
    typedef oatpp::collection::LinkedList<std::shared_ptr<TaskSubmission>> Tasks;
  private:
    void consumeTasks();
  private:
    oatpp::async::Processor m_processor;
    oatpp::concurrency::SpinLock::Atom m_atom;
    Tasks m_pendingTasks;
  private:
    bool m_isRunning;
    std::mutex m_taskMutex;
    std::condition_variable m_taskCondition;
  public:
    SubmissionProcessor();
  public:
    
    void run() override;
    void stop();
    void addTaskSubmission(const std::shared_ptr<TaskSubmission>& task);
    
  };

public:
  static const v_int32 THREAD_NUM_DEFAULT;
private:
  v_int32 m_threadsCount;
  std::shared_ptr<oatpp::concurrency::Thread>* m_threads;
  std::shared_ptr<SubmissionProcessor>* m_processors;
  std::atomic<v_word32> m_balancer;
public:
  
  Executor(v_int32 threadsCount = THREAD_NUM_DEFAULT);
  
  ~Executor();
  
  void join();
  
  void detach();
  
  void stop();
  
  template<typename CoroutineType, typename ... Args>
  void execute(Args... params) {
    auto processor = m_processors[m_balancer % m_threadsCount];
    
    auto submission = std::make_shared<SubmissionTemplate<CoroutineType, Args...>>(params...);
    processor->addTaskSubmission(submission);
    
    m_balancer ++;
  }
  
};
  
}}

#endif /* oatpp_async_Executor_hpp */
