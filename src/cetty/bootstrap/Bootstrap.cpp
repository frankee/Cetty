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

#include "cetty/channel/Channel.h"
#include "cetty/channel/ChannelHandler.h"
#include "cetty/channel/Channels.h"

#include "cetty/util/Exception.h"

#include "cetty/bootstrap/Bootstrap.h"

namespace cetty { namespace bootstrap {

using namespace ::cetty::channel;
using namespace ::cetty::util;

const ChannelFactoryPtr& Bootstrap::getFactory() {
    if (!this->factory) {
        throw IllegalStateException("factory is not set yet.");
    }
    return this->factory;
}

void Bootstrap::setFactory(const ChannelFactoryPtr& factory) {
    if (factory) {
        if (this->factory) {
            throw IllegalStateException("factory can't change once set.");
        }
        this->factory = factory;
    }
    else {
        throw NullPointerException("factory");
    }
}

ChannelPipeline* Bootstrap::getPipeline() {
    if (this->pipeline == NULL) {
        if (externalSetPipelineFactory) {
            throw IllegalStateException("pipelineFactory has already set");
        }
        setPipeline(Channels::pipeline());
    }
    return this->pipeline;
}

void Bootstrap::setPipeline(ChannelPipeline* pipeline) {
    if (pipeline == NULL) {
        throw NullPointerException("pipeline");
    }
    if (externalSetPipelineFactory) {
        throw IllegalStateException("pipelineFactory has already set");
    }

    this->pipeline = pipeline;
    pipelineFactory = Channels::pipelineFactory(pipeline);
    externalSetPipelineFactory = false;
}

ChannelPipeline::ChannelHandlers Bootstrap::getPipelineAsMap() const {
    if (this->pipeline == NULL) {
        throw IllegalStateException("pipeline is not set yet");
    }
    return this->pipeline->toMap();
}

void Bootstrap::setPipelineAsMap(const ChannelPipeline::ChannelHandlers& pipelineMap) {
    if (externalSetPipelineFactory) {
        throw IllegalStateException("pipelineFactory has already set");
    }

    ChannelPipeline* p = Channels::pipeline();
    ChannelPipeline::ChannelHandlers::const_iterator itr = pipelineMap.begin();

    for(; itr != pipelineMap.end(); ++itr) {
        p->addLast(itr->first, itr->second);
    }

    setPipeline(p);
}

void Bootstrap::setPipelineFactory(const ChannelPipelineFactoryPtr& pipelineFactory) {
    if (pipelineFactory) {
        this->pipelineFactory = pipelineFactory;
    }
    else {
        throw NullPointerException("pipelineFactory");
    }

    externalSetPipelineFactory = true;
}

boost::any Bootstrap::getOption(const std::string& key) const {
    std::map<std::string, boost::any>::const_iterator itr = options.find(key);
    if (itr == options.end()) return boost::any();

    return itr->second;
}

void Bootstrap::setOption(const std::string& key, const boost::any& value) {
    if (value.empty()) {
        options.erase(key);
    }
    else {
        options.insert(std::make_pair(key, value));
    }
}

void Bootstrap::releaseExternalResources() {
    if (factory) {
        factory->releaseExternalResources();
    }
}

Bootstrap::Bootstrap()
    : externalSetPipelineFactory(false), pipeline(NULL) {
}

Bootstrap::Bootstrap(const ChannelFactoryPtr& channelFactory)
    : externalSetPipelineFactory(false), pipeline(NULL) {
    setFactory(channelFactory);
}

Bootstrap::~Bootstrap() {
    if (pipeline) {
        delete pipeline;
        pipeline = NULL;
    }
}

}}
