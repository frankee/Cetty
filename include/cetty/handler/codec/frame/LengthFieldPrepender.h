#if !defined(CETTY_HANDLER_CODEC_FRAME_LENGTHFIELDPREPENDER_H)
#define CETTY_HANDLER_CODEC_FRAME_LENGTHFIELDPREPENDER_H

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

#include "cetty/handler/codec/oneone/OneToOneEncoder.h"
#include "cetty/util/Exception.h"
#include "cetty/util/Integer.h"

namespace cetty { namespace handler { namespace codec { namespace frame { 

using namespace ::cetty::util;
using namespace ::cetty::handler::codec::oneone;

/**
 * An encoder that prepends the length of the message.  The length value is
 * prepended as a binary form.  It is encoded in either big endian or little
 * endian depending on the default {@link ByteOrder} of the current
 * {@link ChannelBufferFactory}.
 * <p>
 * For example, <tt>{@link LengthFieldPrepender}(2)</tt> will encode the
 * following 12-bytes string:
 * <pre>
 * +----------------+
 * | "HELLO, WORLD" |
 * +----------------+
 * </pre>
 * into the following:
 * <pre>
 * +--------+----------------+
 * + 0x000C | "HELLO, WORLD" |
 * +--------+----------------+
 * </pre>
 * If you turned on the <tt>lengthIncludesLengthFieldLength</tt> flag in the
 * constructor, the encoded data would look like the following
 * (12 (original data) + 2 (prepended data) = 14 (0xE)):
 * <pre>
 * +--------+----------------+
 * + 0x000E | "HELLO, WORLD" |
 * +--------+----------------+
 * </pre>
 *
 * 
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 * @version $Rev: 2121 $, $Date: 2010-02-02 09:38:07 +0900 (Tue, 02 Feb 2010) $
 */

class LengthFieldPrepender : public ::cetty::handler::codec::oneone::OneToOneEncoder {
public:
    /**
     * Creates a new instance.
     *
     * @param lengthFieldLength the length of the prepended length field.
     *                          Only 1, 2, 3, 4, and 8 are allowed.
     *
     * @throws InvalidArgumentException
     *         if <tt>lengthFieldLength</tt> is not 1, 2, 3, 4, or 8
     */
    LengthFieldPrepender(int lengthFieldLength)
        : lengthFieldLength(lengthFieldLength),
          lengthIncludesLengthFieldLength(false) {
        validateLengthFieldLength();
    }

    /**
     * Creates a new instance.
     *
     * @param lengthFieldLength the length of the prepended length field.
     *                          Only 1, 2, 3, 4, and 8 are allowed.
     * @param lengthIncludesLengthFieldLength
     *                          if <tt>true</tt>, the length of the prepended
     *                          length field is added to the value of the
     *                          prepended length field.
     *
     * @throws InvalidArgumentException
     *         if <tt>lengthFieldLength</tt> is not 1, 2, 3, 4, or 8
     */
    LengthFieldPrepender(
            int lengthFieldLength, bool lengthIncludesLengthFieldLength)
            : lengthFieldLength(lengthFieldLength),
              lengthIncludesLengthFieldLength(lengthIncludesLengthFieldLength) {
        validateLengthFieldLength();
    }

    virtual ChannelMessage encode(
            ChannelHandlerContext& ctx, Channel& channel, const ChannelMessage& msg) {
        
        ChannelBufferPtr body = msg.smartPointer<ChannelBuffer>();
        if (!body) {
            return msg;
        }

        ChannelBufferPtr header = channel.getConfig().getBufferFactory().getBuffer(body->order(), lengthFieldLength);
        
        int length = lengthIncludesLengthFieldLength ?
                        body->readableBytes() + lengthFieldLength : body->readableBytes();
        switch (lengthFieldLength) {
        case 1:
            if (length >= 256) {
                throw InvalidArgumentException(
                    std::string("length does not fit into a byte: ") + Integer::toString(length));
            }
            header->writeByte(length);
            break;
        case 2:
            if (length >= 65536) {
                throw InvalidArgumentException(
                    std::string("length does not fit into a short integer: ") + Integer::toString(length));
            }
            header->writeShort(length);
            break;
        case 3:
            if (length >= 16777216) {
                throw InvalidArgumentException(
                    std::string("length does not fit into a medium integer: ") + Integer::toString(length));
            }
            header->writeMedium(length);
            break;
        case 4:
            header->writeInt(length);
            break;
        case 8:
            header->writeLong(length);
            break;
        default:
            throw RuntimeException("should not reach here");
        }
        return ChannelMessage(ChannelBuffers::wrappedBuffer(header, body));
    }

private:
    void validateLengthFieldLength() {
        if (lengthFieldLength != 1 && lengthFieldLength != 2 &&
            lengthFieldLength != 3 && lengthFieldLength != 4 &&
            lengthFieldLength != 8) {
                throw InvalidArgumentException(
                    std::string("lengthFieldLength must be either 1, 2, 3, 4, or 8: ") +
                    Integer(lengthFieldLength));
        }
    }

private:
    bool lengthIncludesLengthFieldLength;
    int  lengthFieldLength;
};

}}}}

#endif //#if !defined(CETTY_HANDLER_CODEC_FRAME_LENGTHFIELDPREPENDER_H)
