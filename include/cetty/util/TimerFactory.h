#if !defined(CETTY_UTIL_TIMERFACTORY_H)
#define CETTY_UTIL_TIMERFACTORY_H

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

#include <boost/scoped_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/assert.hpp>

#include "cetty/util/Timer.h"

namespace cetty { namespace channel {
class Channel;
}}

namespace cetty { namespace util {

using namespace cetty::channel;

class TimerFactory;
typedef boost::shared_ptr<TimerFactory> TimerFactoryPtr;

class TimerFactory {
public:
    virtual ~TimerFactory() {}

    /**
     * may make sure the timer task, which will be performed in the same
     * thread as the channel, if under multi-io-thread mode.
     * 
     */
    virtual const TimerPtr& getTimer(Channel& channel) = 0;

    /**
     *
     */
    virtual void stopTimers() = 0;

public:
    /**
     * injection of the implement of the TimerFactory.
     */
    static void setFactory(const TimerFactoryPtr& timerFactory);

    static void resetFactory(const TimerFactoryPtr& timerFactory);

    static TimerFactory& getFactory();

private:
    static TimerFactoryPtr factory;
};

}}

#endif //#if !defined(CETTY_UTIL_TIMERFACTORY_H)
