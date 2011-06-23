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

#include "cetty/channel/CompleteChannelFuture.h"
#include "cetty/util/Exception.h"
#include "cetty/logging/InternalLogger.h"
#include "cetty/logging/InternalLoggerFactory.h"

namespace cetty { namespace channel {

using namespace ::cetty::util;
using namespace ::cetty::logging;

InternalLogger* CompleteChannelFuture::logger =
                    InternalLoggerFactory::getInstance("CompleteChannelFuture");

void CompleteChannelFuture::setListener(const ListenerFunction& listener) {
    if (listener.empty()) return;

    try {
        listener(shared_from_this());
    }
    catch (const Exception& e) {
        logger->warn("An exception was thrown by ChannelFutureListener .", e);
    }
}

void CompleteChannelFuture::addListener(ChannelFutureListener* listener) {
    if (NULL == listener) return;

    try {
        listener->operationComplete(shared_from_this());
    }
    catch (const Exception& e) {
        logger->warn("An exception was thrown by ChannelFutureListener .", e);
    }
}

}}