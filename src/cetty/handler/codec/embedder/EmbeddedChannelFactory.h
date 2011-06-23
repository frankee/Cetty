#if !defined(CETTY_HANDLER_CODEC_EMBEDDER_EMBEDDEDCHANNELFACTORY_H)
#define CETTY_HANDLER_CODEC_EMBEDDER_EMBEDDEDCHANNELFACTORY_H

/*
 * Copyright 2009 Red Hat, Inc.
 *
 * Red Hat licenses this file to you under the Apache License, version 2.0
 * (the "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at:
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

#include "cetty/channel/ChannelFactory.h"

namespace cetty { namespace handler { namespace codec { namespace embedder { 

using namespace cetty::channel;

/**
 * 
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 * @version $Rev: 2080 $, $Date: 2010-01-26 18:04:19 +0900 (Tue, 26 Jan 2010) $
 */

class EmbeddedChannelFactory : public cetty::channel::ChannelFactory {
public:
    static ChannelFactory* INSTANCE;

    virtual ~EmbeddedChannelFactory() {}

    virtual Channel* newChannel(ChannelPipeline* pipeline);

    virtual void releaseExternalResources() {
        // No external resources
    }

private:
    EmbeddedChannelFactory() {}
};

}}}}

#endif //#if !defined(CETTY_HANDLER_CODEC_EMBEDDER_EMBEDDEDCHANNELFACTORY_H)
