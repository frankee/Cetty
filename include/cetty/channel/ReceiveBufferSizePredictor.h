#if !defined(CETTY_CHANNEL_RECEIVEBUFFERSIZEPREDICTOR_H)
#define CETTY_CHANNEL_RECEIVEBUFFERSIZEPREDICTOR_H

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

namespace cetty { namespace channel {

/**
 * Predicts the number of readable bytes in the receive buffer of a
 * {@link Channel}.
 * <p>
 * It calculates the close-to-optimal capacity of the {@link ChannelBuffer}
 * for the next read operation depending on the actual number of read bytes
 * in the previous read operation.  More accurate the prediction is, more
 * effective the memory utilization will be.
 * <p>
 * Once a read operation is performed and the actual number of read bytes is
 * known, an I/O thread will call {@link #previousReceiveBufferSize(int)} to
 * update the predictor so it can predict more accurately next time.
 *
 * 
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 *
 */

class ReceiveBufferSizePredictor {
public:
    virtual ~ReceiveBufferSizePredictor() {}

    /**
     * Predicts the capacity of the {@link ChannelBuffer} for the next
     * read operation depending on the actual number of read bytes in the
     * previous read operation.
     *
     * @return the expected number of readable bytes this time
     */
    virtual int nextReceiveBufferSize() const = 0;

    /**
     * Updates this predictor by specifying the actual number of read bytes
     * in the previous read operation.
     *
     * @param preReceiveBufSz
     *        the actual number of read bytes in the previous read operation
     */
    virtual void previousReceiveBufferSize(int preReceiveBufSz) = 0;
};

}}


#endif //#if !defined(CETTY_CHANNEL_RECEIVEBUFFERSIZEPREDICTOR_H)
