#if !defined(CETTY_BUFFER_ABSTRACTCHANNELBUFFERFACTORY_H)
#define CETTY_BUFFER_ABSTRACTCHANNELBUFFERFACTORY_H

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

#include "cetty/buffer/ChannelBufferFactory.h"

namespace cetty { namespace buffer {

/**
 * A skeletal implementation of {@link ChannelBufferFactory}.
 *
 * 
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 *
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 */
class AbstractChannelBufferFactory : public ChannelBufferFactory {
public:
    virtual ~AbstractChannelBufferFactory() {}

	virtual ChannelBufferPtr getBuffer(int capacity) {
        return getBuffer(getDefaultOrder(), capacity);
    }

    virtual ChannelBufferPtr getBuffer(ByteOrder endianness, int capacity) = 0;

    virtual ChannelBufferPtr getBuffer(const Array& buf, int offset, int length) {
        return getBuffer(getDefaultOrder(), buf, offset, length);
    }

    virtual ChannelBufferPtr getBuffer(ByteOrder endianness,
                                       const Array& array,
                                       int offset,
                                       int length) = 0;

    virtual ByteOrder getDefaultOrder() const {
        return this->defaultOrder;
    }

protected:
    /**
     * Creates a new factory whose default {@link ByteOrder} is
     * {@link ByteOrder#BIG_ENDIAN}.
     */
    AbstractChannelBufferFactory() : defaultOrder(ByteOrder::BYTE_ORDER_BIG) {
    }

    /**
     * Creates a new factory with the specified default {@link ByteOrder}.
     *
     * @param defaultOrder the default {@link ByteOrder} of this factory
     */
    AbstractChannelBufferFactory(ByteOrder defaultOrder)
        : defaultOrder(defaultOrder) {
    }

private:
	ByteOrder defaultOrder;
};

}}


#endif //#if !defined(CETTY_BUFFER_ABSTRACTCHANNELBUFFERFACTORY_H)
