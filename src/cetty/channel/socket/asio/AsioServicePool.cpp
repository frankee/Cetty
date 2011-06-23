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

#include "cetty/channel/socket/asio/AsioServicePool.h"

#include <boost/bind.hpp>

#include "cetty/util/Exception.h"

namespace cetty { namespace channel { namespace socket { namespace asio {

AsioServicePool::AsioServicePool(int poolSize)
  : usingthread(true),
    running(false),
    nextIOserviceIndex(0),
    mainThreadId(boost::this_thread::get_id()) {
    if (poolSize < 0) {
        poolSize = boost::thread::hardware_concurrency();
    }
    else if (poolSize == 0) {
        usingthread = false;
        poolSize = 1;
    }

    // Give all the io_services work to do so that their run() functions will not
    // exit until they are explicitly stopped.
    for (int i = 0; i < poolSize; ++i) {
        IOservicePtr ioService(new IOService(i));
        WorkPtr work(new boost::asio::io_service::work(ioService->service()));
        ioServices.push_back(ioService);
        works.push_back(work);
    }

    // automatic start
    if (usingthread) {
        // Create a pool of threads to run all of the io_services.
        for (std::size_t i = 0; i < ioServices.size(); ++i) {
            ThreadPtr thread(new boost::thread(
                boost::bind(&AsioServicePool::runIOservice,
                            this,
                            boost::ref(ioServices[i]->service()))));
            threads.push_back(thread);
        }
        running = true;
    }
}

void AsioServicePool::run() {
    if (running) return;

    if (!usingthread) {
        runIOservice(ioServices[0]->service());
    }

    running = true;
}

void cetty::channel::socket::asio::AsioServicePool::waitForExit() {
    // Wait for all threads in the pool to exit.
    for (std::size_t i = 0; i < threads.size(); ++i) {
        threads[i]->join();
    }
}

void AsioServicePool::stop() {
    if (!running) return;

    // Explicitly stop all io_services.
    for (std::size_t i = 0; i < ioServices.size(); ++i) {
        ioServices[i]->service().stop();
    }
}

AsioServicePool::IOService& AsioServicePool::getIOService() {
    // Only one service.
    if (ioServices.size() == 1) {
        return *ioServices[0];
    }

    // Use a round-robin scheme to choose the next io_service to use.
    IOService& ioService = *ioServices[nextIOserviceIndex];
    ++nextIOserviceIndex;
    if (nextIOserviceIndex == (int)ioServices.size()) {
        nextIOserviceIndex = 0;
    }

    return ioService;
}

std::size_t AsioServicePool::runIOservice(boost::asio::io_service& ioservice) {
    boost::system::error_code err;
    std::size_t opCount = ioservice.run(err);

    // if error happened, try to recover.
    if (err) {
        printf("io service has error = %d\n", err.value());
    }
    return opCount;
}

boost::thread::id AsioServicePool::getThreadId(int index) {
    if (!usingthread) return mainThreadId;

    BOOST_ASSERT(index >= 0 && index < (int)threads.size() && "Out of range");
    return threads[index]->get_id();
}

}}}}
