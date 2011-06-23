#if !defined(CETTY_CHANNEL_SOCKET_ASIO_ASIOSERVICEPOOL_H)
#define CETTY_CHANNEL_SOCKET_ASIO_ASIOSERVICEPOOL_H

//
// Copyright (c) 2003-2011 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
/*
 * Copyright (c) 2010-2011 frankee zhou (frankee.zhou at gmail dot com)
 *
 * Distributed under under the Apache License, version 2.0 (the "License").
 * you may not use this file except in compliance with the License.  
 * You may obtain a copy of the License at:
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */

#include <vector>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/mpl/size_t.hpp>

namespace cetty { namespace channel { namespace socket { namespace asio {

/**
 * A pool of io_service objects.
 */
class AsioServicePool : private boost::noncopyable {
public:
    class IOService {
    public:
        IOService(int index) : poolIndex(index) {}

        int index() const { return poolIndex; }
        boost::asio::io_service& service() { return ioService; }
        operator boost::asio::io_service&() { return ioService; }

    private:
        boost::asio::io_service ioService;
        int                     poolIndex;
    };

public:
    /**
     * Construction of AsioServicePool
     *
     * if multi-thread, will automatically run.
     */
    AsioServicePool(int poolSize);

    ~AsioServicePool() {}
    
    /**
     * Run all io_service objects in the pool.
     * Usually, you only need to run manually under single-thread mode.
     */
    void run();

    /**
     *
     */
    void waitForExit();

    /**
     * Stop all io_service objects in the pool.
     */
    void stop();

    /**
     * Get an io_service to use.
     */
    IOService& getIOService();

    boost::asio::io_service& getIOService(int index) {
        return ioServices.at(index)->service();
    }

    /**
     *
     */
    boost::thread::id getThreadId(int index);

    /**
     *
     */
    bool isSingleThread() const { return !usingthread; }

    int size() const { return static_cast<int>(ioServices.size()); }

private:
    typedef boost::shared_ptr<boost::thread> ThreadPtr;
    typedef boost::shared_ptr<IOService> IOservicePtr;
    typedef boost::shared_ptr<boost::asio::io_service::work> WorkPtr;

private:
    std::size_t runIOservice(boost::asio::io_service& ioservice);

private:
    // indicated this pool using thread.
    bool usingthread;

    // io service pool already running.
    bool running;

    // The next io_service to use for a connection.
    int nextIOserviceIndex;

    // 
    boost::thread::id mainThreadId;

    // The pool of io_services.
    std::vector<IOservicePtr> ioServices;

    // The work that keeps the io_services running.
    std::vector<WorkPtr> works;

    std::vector<ThreadPtr> threads;
};

}}}}

#endif //#if !defined(CETTY_CHANNEL_SOCKET_ASIO_ASIOSERVICEPOOL_H)
