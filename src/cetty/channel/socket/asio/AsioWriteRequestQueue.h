#if !defined(CETTY_CHANNEL_SOCKET_ASIO_ASIOWRITEREQUESTQUEUE_H)
#define CETTY_CHANNEL_SOCKET_ASIO_ASIOWRITEREQUESTQUEUE_H

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

#include <deque>
#include <boost/array.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/static_assert.hpp>

#include "cetty/buffer/ChannelBuffer.h"
#include "cetty/buffer/GatheringBuffer.h"
#include "cetty/channel/ChannelFuture.h"
#include "cetty/channel/Channels.h"

namespace cetty { namespace channel {
class MessageEvent;
}}

namespace cetty { namespace channel { namespace socket { namespace asio {

using namespace cetty::buffer;
using namespace cetty::channel;

class AsioSocketChannel;

template<class T, std::size_t N>
class TruncatableArray : public boost::array<T, N> {
public:
    // type definitions
    typedef T*             iterator;
    typedef const T*       const_iterator;
    typedef T&             reference;
    typedef const T&       const_reference;

public:
    TruncatableArray() : truncatedIndex(1) {
        BOOST_STATIC_ASSERT(N > 0);
    }

public:
    int truncatedIndex;

public:

    iterator        end()       { return boost::array<T, N>::elems+truncatedIndex; }
    const_iterator  end() const { return boost::array<T, N>::elems+truncatedIndex; }

    reference back()  {
        BOOST_ASSERT(truncatedIndex);
        return boost::array<T, N>::elems[truncatedIndex-1];
    }
    const_reference back() const {
        BOOST_ASSERT(truncatedIndex);
        return boost::array<T, N>::elems[truncatedIndex-1];
    }
};

class AsioGatheringBuffer : public cetty::buffer::GatheringBuffer {
public:
    typedef boost::asio::mutable_buffer asio_buffer;

public:
    AsioGatheringBuffer() : byteSize(0) {}
    virtual ~AsioGatheringBuffer() {}


    virtual bool empty() const {
        return buffers.truncatedIndex == 0;
    }

    /**
     * underline memory block count.
     */
    virtual int  blockCount() const {
        return buffers.truncatedIndex;
    }

    virtual int  bytesCount() const {
        if (byteSize == 0) {
            for (int i = 0; i < buffers.truncatedIndex; ++i) {
                byteSize += (int)boost::asio::buffer_size(buffers[i]);
            }
        }

        return byteSize;
    }
    
    virtual void clear() {
        buffers.truncatedIndex = 0;
        byteSize = 0;
    }

    virtual void append(char* data, int size) {
        BOOST_ASSERT(buffers.truncatedIndex < MAX_BUFFER_COUNT);
        buffers[buffers.truncatedIndex++] = asio_buffer(data, size);
        byteSize += size;
    }

    virtual std::pair<char*, int> at(int index) {
        BOOST_ASSERT(index >= 0 && index < MAX_BUFFER_COUNT);
        asio_buffer& buffer = buffers[index];
        return std::make_pair<char*,int>(
                        boost::asio::buffer_cast<char*>(buffer),
                        (int)boost::asio::buffer_size(buffer));
    }

public:
    const static int  MAX_BUFFER_COUNT = 8;

    mutable int byteSize;
    TruncatableArray<asio_buffer, MAX_BUFFER_COUNT> buffers;
};

class AsioWriteRequest {
public:
    typedef boost::asio::mutable_buffer asio_buffer;

public:
    asio_buffer         buffer;
    AsioGatheringBuffer gathring;

    int                 writeBufferSize;

public:
    AsioWriteRequest(const MessageEvent& evt);
    ~AsioWriteRequest() {}

    bool hasBuffers() const { return gathring.buffers.truncatedIndex > 0; }
};

class AsioWriteOperation {
public:
    int writeBufferSize;
    ChannelFuturePtr future;

    AsioWriteOperation() : writeBufferSize(0) {}

    AsioWriteOperation(int size, const ChannelFuturePtr& f)
        : writeBufferSize(size), future(f) {
    }

    AsioWriteOperation(const AsioWriteOperation& op)
        : writeBufferSize(op.writeBufferSize),
          future(op.future) {
    }

    bool setSuccess() {
        return future ? future->setSuccess() : false;
    }
    bool setFailure(const Exception& cause) {
        return future ? future->setFailure(cause) : false;
    }
};

// no need ensure thread safe.
class AsioWriteOperationQueue {
public:
    AsioWriteOperationQueue() : channel(NULL), writeBufferSize(0) {}
    ~AsioWriteOperationQueue() {}

    void setChannel(AsioSocketChannel& channel) {
        this->channel = &channel;
    }

    bool  empty() const { return ops.empty(); }
    size_t size() const { return ops.size(); }

    AsioWriteOperation& peek() {
        return ops.front();
    }

    AsioWriteOperation& poll() {
        polledOp = ops.front();
        ops.pop_front();
        minusWriteBufferSize(polledOp.writeBufferSize);
        return polledOp;
    }

    void offer(const AsioWriteRequest& request, const ChannelFuturePtr& f) {
        ops.push_back(AsioWriteOperation(request.writeBufferSize, f));
        plusWriteBufferSize(request.writeBufferSize);
    }

    int  getWriteBufferSize() const { return writeBufferSize; }

private:
    void plusWriteBufferSize(int messageSize);
    void minusWriteBufferSize(int messageSize);

private:
    AsioSocketChannel* channel;

    int writeBufferSize;

    AsioWriteOperation polledOp;
    std::deque<AsioWriteOperation> ops;
};

}}}}

#endif //#if !defined(CETTY_CHANNEL_SOCKET_ASIO_ASIOWRITEREQUESTQUEUE_H)
