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

#ifndef oatpp_network_virtual__Pipe_hpp
#define oatpp_network_virtual__Pipe_hpp

#include "oatpp/core/data/stream/Stream.hpp"

#include "oatpp/core/data/buffer/FIFOBuffer.hpp"
#include "oatpp/core/data/buffer/IOBuffer.hpp"

#include "oatpp/core/concurrency/SpinLock.hpp"

#include <mutex>
#include <condition_variable>

namespace oatpp { namespace network { namespace virtual_ {

class Pipe : public oatpp::base::Countable {
public:
  
  class Reader : public oatpp::data::stream::InputStream {
  private:
    Pipe* m_pipe;
    bool m_nonBlocking;
    
    /**
     * this one used for testing purposes only
     */
    data::v_io_size m_maxAvailableToRead;
  public:
    
    Reader(Pipe* pipe, bool nonBlocking = false)
      : m_pipe(pipe)
      , m_nonBlocking(nonBlocking)
      , m_maxAvailableToRead(-1)
    {}
    
    void setNonBlocking(bool nonBlocking) {
      m_nonBlocking = nonBlocking;
    }
    
    /**
     * this one used for testing purposes only
     * set to -1 in order to ignore this value
     */
    void setMaxAvailableToRead(data::v_io_size maxAvailableToRead);
    
    data::v_io_size read(void *data, data::v_io_size count) override;
    
  };
  
  class Writer : public oatpp::data::stream::OutputStream {
  private:
    Pipe* m_pipe;
    bool m_nonBlocking;
    
    /**
     * this one used for testing purposes only
     */
    data::v_io_size m_maxAvailableToWrtie;
  public:
    
    Writer(Pipe* pipe, bool nonBlocking = false)
      : m_pipe(pipe)
      , m_nonBlocking(nonBlocking)
      , m_maxAvailableToWrtie(-1)
    {}
    
    void setNonBlocking(bool nonBlocking) {
      m_nonBlocking = nonBlocking;
    }
    
    /**
     * this one used for testing purposes only
     * set to -1 in order to ignore this value
     */
    void setMaxAvailableToWrite(data::v_io_size maxAvailableToWrite);
    
    data::v_io_size write(const void *data, data::v_io_size count) override;
    
  };
  
private:
  bool m_open;
  Writer m_writer;
  Reader m_reader;

  oatpp::data::buffer::IOBuffer m_buffer;
  oatpp::data::buffer::SynchronizedFIFOBuffer m_fifo;

  std::mutex m_mutex;
  std::condition_variable m_conditionRead;
  std::condition_variable m_conditionWrite;
public:
  
  Pipe()
    : m_open(true)
    , m_writer(this)
    , m_reader(this)
    , m_buffer()
    , m_fifo(m_buffer.getData(), m_buffer.getSize())
  {}
  
  static std::shared_ptr<Pipe> createShared(){
    return std::make_shared<Pipe>();
  }
  
  Writer* getWriter() {
    return &m_writer;
  }
  
  Reader* getReader() {
    return &m_reader;
  }
  
  void close() {
    {
      std::lock_guard<std::mutex> lock(m_mutex);
      m_open = false;
    }
    m_conditionRead.notify_one();
    m_conditionWrite.notify_one();
  }
  
};
  
}}}

#endif /* oatpp_network_virtual__Pipe_hpp */
