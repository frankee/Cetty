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

#include "cetty/util/internal/asio/AsioDeadlineTimer.h"
#include "cetty/util/internal/asio/AsioDeadlineTimeout.h"
#include "cetty/util/TimeUnit.h"

namespace cetty { namespace util { namespace internal { namespace asio {

using namespace cetty::util;

const TimeoutPtr& AsioDeadlineTimer::newTimeout(TimerTask& task, boost::int64_t delay) {
    timeouts.push_back(TimeoutPtr(
        new AsioDeadlineTimeout(*this, ioService, &task, delay)));
    return timeouts.back();
}

const TimeoutPtr& AsioDeadlineTimer::newTimeout(TimerTask& task, boost::int64_t delay, const TimeUnit& unit) {
    timeouts.push_back(TimeoutPtr(
        new AsioDeadlineTimeout(*this, ioService, &task, unit.toMillis(delay))));
    return timeouts.back();
}

const TimeoutPtr& AsioDeadlineTimer::newTimeout(const TaskType& task, boost::int64_t delay) {
    timeouts.push_back(TimeoutPtr(
        new AsioDeadlineTimeout(*this, ioService, task, delay)));
    return timeouts.back();
}

const TimeoutPtr& AsioDeadlineTimer::newTimeout(const TaskType& task, boost::int64_t delay, const TimeUnit& unit) {
    timeouts.push_back(TimeoutPtr(
        new AsioDeadlineTimeout(*this, ioService, task, unit.toMillis(delay))));
    return timeouts.back();
}

void AsioDeadlineTimer::stop() {
    std::size_t j = timeouts.size();
    for (std::size_t i = 0; i < j; ++i) {
        Timeout& timer = *(timeouts[i]);
        if (timer.isActive()) {
            timer.cancel();
        }
    }
}

}}}}