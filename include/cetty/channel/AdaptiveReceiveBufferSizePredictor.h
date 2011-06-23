#if !defined(CETTY_CHANNEL_ADAPTIVERECEIVEBUFFERSIZEPREDICTOR_H)
#define CETTY_CHANNEL_ADAPTIVERECEIVEBUFFERSIZEPREDICTOR_H

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

#include <vector>
#include "cetty/channel/ReceiveBufferSizePredictor.h"

namespace cetty { namespace channel {

/**
 * The {@link ReceiveBufferSizePredictor} that automatically increases and
 * decreases the predicted buffer size on feed back.
 * <p>
 * It gradually increases the expected number of readable bytes if the previous
 * read fully filled the allocated buffer.  It gradually decreases the expected
 * number of readable bytes if the read operation was not able to fill a certain
 * amount of the allocated buffer two times consecutively.  Otherwise, it keeps
 * returning the same prediction.
 *
 * 
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 *
 * @version $Rev: 2080 $, $Date: 2010-01-26 18:04:19 +0900 (Tue, 26 Jan 2010) $
 *
 */

class AdaptiveReceiveBufferSizePredictor : public ReceiveBufferSizePredictor {
public:
    static const int DEFAULT_MINIMUM = 64;
    static const int DEFAULT_INITIAL = 1024;
    static const int DEFAULT_MAXIMUM = 65536;

    /**
     * Creates a new predictor with the default parameters.  With the default
     * parameters, the expected buffer size starts from <tt>1024</tt>, does not
     * go down below <tt>64</tt>, and does not go up above <tt>65536</tt>.
     */
    AdaptiveReceiveBufferSizePredictor();

    /**
     * Creates a new predictor with the specified parameters.
     *
     * @param minimum  the inclusive lower bound of the expected buffer size
     * @param initial  the initial buffer size when no feed back was received
     * @param maximum  the inclusive upper bound of the expected buffer size
     */
    AdaptiveReceiveBufferSizePredictor(int minimum, int initial, int maximum);

    int nextReceiveBufferSize() const {
        return nextReceiveBuffSz;
    }

    void previousReceiveBufferSize(int previousReceiveBufferSize);

private:
    void init(int minimum, int initial, int maximum);


private:
    static const int INDEX_INCREMENT = 4;
    static const int INDEX_DECREMENT = 1;

    static const int SIZE_TABLE_CNT = 8 + (32 - 4) * 8;
    static int SIZE_TABLE[SIZE_TABLE_CNT];

    static void initSizeTable();
    static int getSizeTableIndex(int size);

    bool decreaseNow;
    int  minIndex;
    int  maxIndex;
    int  index;
    int  nextReceiveBuffSz;
};
}}

#endif //#if !defined(CETTY_CHANNEL_ADAPTIVERECEIVEBUFFERSIZEPREDICTOR_H)
