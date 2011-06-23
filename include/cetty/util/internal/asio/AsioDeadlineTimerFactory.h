#if !defined(CETTY_UTIL_INTERNAL_ASIO_ASIODEADLINETIMERFACTORY_H)
#define CETTY_UTIL_INTERNAL_ASIO_ASIODEADLINETIMERFACTORY_H

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
/*
 * Copyright (c) 2010-2011 frankee zhou (frankee.zhou at gmail dot com)
 * Distributed under under the Apache License, version 2.0 (the "License").
 */

#include <vector>
#include "cetty/util/TimerFactory.h"
#include "cetty/util/internal/asio/AsioDeadlineTimer.h"
#include "cetty/channel/socket/asio/AsioServicePool.h"

namespace cetty { namespace channel { namespace socket { namespace asio {
class AsioSocketChannel;
class AsioDatagramChannel;
}}}}

namespace cetty { namespace util { namespace internal { namespace asio {

using namespace cetty::util;
using namespace cetty::channel::socket::asio;

template<class AsioChannelType>
class AsioDeadlineTimerFactory : public cetty::util::TimerFactory {
public:
    AsioDeadlineTimerFactory(AsioServicePool& pool) : pool(pool) {
        int j = pool.size();
        for (int i = 0; i < j; ++i) {
            timers.push_back(TimerPtr(new AsioDeadlineTimer(pool.getIOService(i))));
        }
    }

    virtual ~AsioDeadlineTimerFactory() {}

    virtual const TimerPtr& getTimer(cetty::channel::Channel& channel) {
        AsioChannelType* socketChannel =
            dynamic_cast<AsioChannelType*>(&channel);

        BOOST_ASSERT(socketChannel && "Can't parse the correct channel instance.");
        if (socketChannel) {
            return timers[socketChannel->getIOService().index()];
        }

        throw InvalidArgumentException("Can't parse the correct channel instance.");
    }

    virtual void stopTimers() {
        std::size_t j = timers.size();
        for (std::size_t i = 0; i < j; ++i) {
            timers[i]->stop();
        }
    }

private:
    AsioServicePool& pool;
    std::vector<TimerPtr> timers;
};

typedef AsioDeadlineTimerFactory<AsioSocketChannel> AsioSocketDeadlineTimerFactory;
typedef AsioDeadlineTimerFactory<AsioDatagramChannel> AsioDatagramDeadlineTimerFactory;

}}}}

#endif //#if !defined(CETTY_UTIL_INTERNAL_ASIO_ASIODEADLINETIMERFACTORY_H)
