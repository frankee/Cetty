#if !defined(CETTY_UTIL_INTERNAL_ASIO_ASIODEADLINETIMEOUT_H)
#define CETTY_UTIL_INTERNAL_ASIO_ASIODEADLINETIMEOUT_H

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

#include <boost/bind.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <boost/asio/placeholders.hpp>

#include "cetty/util/Timeout.h"
#include "cetty/util/TimerTask.h"
#include "cetty/util/TimeUnit.h"

namespace cetty { namespace util { namespace internal { namespace asio {

using namespace cetty::util;

class AsioDeadlineTimer;

class AsioDeadlineTimeout : public cetty::util::Timeout {
public:
    AsioDeadlineTimeout(AsioDeadlineTimer& timer,
                        boost::asio::io_service& ioService,
                        TimerTask* task,
                        boost::int64_t delay);

    AsioDeadlineTimeout(AsioDeadlineTimer& timer,
                        boost::asio::io_service& ioService,
                        const TaskType& task,
                        boost::int64_t delay);

    virtual ~AsioDeadlineTimeout() {}
    
    virtual Timer& getTimer() const;

    virtual bool isExpired() const { return (state == TIMER_EXPIRED); }
    virtual bool isCancelled() const { return (state == TIMER_CANCELLED); }
    virtual bool isActive() const { return (state == TIMER_ACTIVE); }

    virtual void cancel() {
        deadlineTimer.cancel();
    }

    void expiresFromNow(TimerTask& task, boost::int64_t delay, const TimeUnit& unit) {
        /*
        deadlineTimer.expires_from_now(
            boost::posix_time::milliseconds(unit.toMillis(delay)));
        deadlineTimer.async_wait(boost::bind(&AsioDeadlineTimeout::TimerTaskHandler,
                                             this,
                                             boost::asio::placeholders::error,
                                             boost::ref(task)));
        state = TIMER_ACTIVE;*/
    }

    void expiresFromNow(const TaskType& task, boost::int64_t delay, const TimeUnit& unit) {
        deadlineTimer.expires_from_now(
            boost::posix_time::milliseconds(unit.toMillis(delay)));
        deadlineTimer.async_wait(boost::bind(&AsioDeadlineTimeout::TimerFunctionHandler,
                                             this,
                                             boost::asio::placeholders::error,
                                             task));
        state = TIMER_ACTIVE;
    }

    virtual int expiresFromNow() const {
//         deadline_timer::duration_type duration =
//             deadline_timer::traits_type::subtract(deadline_timer.expires_at(),
//                                                   deadline_timer::traits_type::now());
//         return deadline_timer::traits_type::to_posix_duration(duration).total_milliseconds();
//     }

        return 0;
    }

    void TimerFunctionHandler(const boost::system::error_code& error, const TaskType& task) {
        if (boost::asio::error::operation_aborted == error) {
            state = TIMER_CANCELLED;
        }
        else {
            state = TIMER_EXPIRED;
            task(*this);
        }
    }

    void TimerTaskHandler(const boost::system::error_code& error, TimerTask& task) {
        if (boost::asio::error::operation_aborted == error) {
            state = TIMER_CANCELLED;
        }
        else {
            state = TIMER_EXPIRED;
            task.run(*this);
        }
    }

private:
    enum {
        TIMER_UNINITIALIZED  = 0,
        TIMER_CANCELLED      = 1,
        TIMER_EXPIRED        = 2,
        TIMER_ACTIVE         = 4,
    };

private:
    int state;
    AsioDeadlineTimer& timer;
    boost::asio::deadline_timer deadlineTimer;
};

}}}}

#endif //#if !defined(CETTY_UTIL_INTERNAL_ASIO_ASIODEADLINETIMEOUT_H)
