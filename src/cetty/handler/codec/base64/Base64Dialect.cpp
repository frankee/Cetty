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

#include "cetty/handler/codec/base64/Base64Dialect.h"

namespace cetty { namespace handler { namespace codec { namespace base64 { 

static const char STANDARD_ALPHABET[] = {
    'A', 'B', 'C', 'D', 'E',
    'F', 'G', 'H', 'I', 'J',
    'K', 'L', 'M', 'N', 'O',
    'P', 'Q', 'R', 'S', 'T',
    'U', 'V', 'W', 'X', 'Y',
    'Z', 'a', 'b', 'c', 'd',
    'e', 'f', 'g', 'h', 'i',
    'j', 'k', 'l', 'm', 'n',
    'o', 'p', 'q', 'r', 's',
    't', 'u', 'v', 'w', 'x',
    'y', 'z', '0', '1', '2',
    '3', '4', '5', '6', '7',
    '8', '9', '+', '/' 
};

static const char STANDARD_DECODABET[] = {
    -9, -9, -9, -9, -9, -9,
    -9, -9, -9, // Decimal  0 -  8
    -5, -5, // Whitespace: Tab and Linefeed
    -9, -9, // Decimal 11 - 12
    -5, // Whitespace: Carriage Return
    -9, -9, -9, -9, -9, -9, -9, -9, -9, -9, -9, -9, -9, // Decimal 14 - 26
    -9, -9, -9, -9, -9, // Decimal 27 - 31
    -5, // Whitespace: Space
    -9, -9, -9, -9, -9, -9, -9, -9, -9, -9, // Decimal 33 - 42
    62, // Plus sign at decimal 43
    -9, -9, -9, // Decimal 44 - 46
    63, // Slash at decimal 47
    52, 53, 54, 55, 56, 57, 58, 59, 60, 61, // Numbers zero through nine
    -9, -9, -9, // Decimal 58 - 60
    -1, // Equals sign at decimal 61
    -9, -9, -9, // Decimal 62 - 64
     0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, // Letters 'A' through 'N'
    14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, // Letters 'O' through 'Z'
    -9, -9, -9, -9, -9, -9, // Decimal 91 - 96
    26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, // Letters 'a' through 'm'
    39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, // Letters 'n' through 'z'
    -9, -9, -9, -9, // Decimal 123 - 126
 /* -9,-9,-9,-9,-9,-9,-9,-9,-9,-9,-9,-9,-9,     // Decimal 127 - 139
    -9,-9,-9,-9,-9,-9,-9,-9,-9,-9,-9,-9,-9,     // Decimal 140 - 152
    -9,-9,-9,-9,-9,-9,-9,-9,-9,-9,-9,-9,-9,     // Decimal 153 - 165
    -9,-9,-9,-9,-9,-9,-9,-9,-9,-9,-9,-9,-9,     // Decimal 166 - 178
    -9,-9,-9,-9,-9,-9,-9,-9,-9,-9,-9,-9,-9,     // Decimal 179 - 191
    -9,-9,-9,-9,-9,-9,-9,-9,-9,-9,-9,-9,-9,     // Decimal 192 - 204
    -9,-9,-9,-9,-9,-9,-9,-9,-9,-9,-9,-9,-9,     // Decimal 205 - 217
    -9,-9,-9,-9,-9,-9,-9,-9,-9,-9,-9,-9,-9,     // Decimal 218 - 230
    -9,-9,-9,-9,-9,-9,-9,-9,-9,-9,-9,-9,-9,     // Decimal 231 - 243
    -9,-9,-9,-9,-9,-9,-9,-9,-9,-9,-9,-9         // Decimal 244 - 255 */
};

static const char URL_SAFE_ALPHABET[] = {
    'A', 'B', 'C', 'D', 'E',
    'F', 'G', 'H', 'I', 'J',
    'K', 'L', 'M', 'N', 'O',
    'P', 'Q', 'R', 'S', 'T',
    'U', 'V', 'W', 'X', 'Y',
    'Z', 'a', 'b', 'c', 'd',
    'e', 'f', 'g', 'h', 'i',
    'j', 'k', 'l', 'm', 'n',
    'o', 'p', 'q', 'r', 's',
    't', 'u', 'v', 'w', 'x',
    'y', 'z', '0', '1', '2',
    '3', '4', '5', '6', '7',
    '8', '9', '-', '_'
};

static const char URL_SAFE_DECODABET[] = {
    -9, -9, -9, -9, -9, -9,
    -9, -9, -9, // Decimal  0 -  8
    -5, -5, // Whitespace: Tab and Linefeed
    -9, -9, // Decimal 11 - 12
    -5, // Whitespace: Carriage Return
    -9, -9, -9, -9, -9, -9, -9, -9, -9, -9, -9, -9, -9, // Decimal 14 - 26
    -9, -9, -9, -9, -9, // Decimal 27 - 31
    -5, // Whitespace: Space
    -9, -9, -9, -9, -9, -9, -9, -9, -9, -9, // Decimal 33 - 42
    -9, // Plus sign at decimal 43
    -9, // Decimal 44
    62, // Minus sign at decimal 45
    -9, // Decimal 46
    -9, // Slash at decimal 47
    52, 53, 54, 55, 56, 57, 58, 59, 60, 61, // Numbers zero through nine
    -9, -9, -9, // Decimal 58 - 60
    -1, // Equals sign at decimal 61
    -9, -9, -9, // Decimal 62 - 64
     0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, // Letters 'A' through 'N'
    14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, // Letters 'O' through 'Z'
    -9, -9, -9, -9, // Decimal 91 - 94
    63, // Underscore at decimal 95
    -9, // Decimal 96
    26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, // Letters 'a' through 'm'
    39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, // Letters 'n' through 'z'
    -9, -9, -9, -9, // Decimal 123 - 126
  /*-9,-9,-9,-9,-9,-9,-9,-9,-9,-9,-9,-9,-9,     // Decimal 127 - 139
    -9,-9,-9,-9,-9,-9,-9,-9,-9,-9,-9,-9,-9,     // Decimal 140 - 152
    -9,-9,-9,-9,-9,-9,-9,-9,-9,-9,-9,-9,-9,     // Decimal 153 - 165
    -9,-9,-9,-9,-9,-9,-9,-9,-9,-9,-9,-9,-9,     // Decimal 166 - 178
    -9,-9,-9,-9,-9,-9,-9,-9,-9,-9,-9,-9,-9,     // Decimal 179 - 191
    -9,-9,-9,-9,-9,-9,-9,-9,-9,-9,-9,-9,-9,     // Decimal 192 - 204
    -9,-9,-9,-9,-9,-9,-9,-9,-9,-9,-9,-9,-9,     // Decimal 205 - 217
    -9,-9,-9,-9,-9,-9,-9,-9,-9,-9,-9,-9,-9,     // Decimal 218 - 230
    -9,-9,-9,-9,-9,-9,-9,-9,-9,-9,-9,-9,-9,     // Decimal 231 - 243
    -9,-9,-9,-9,-9,-9,-9,-9,-9,-9,-9,-9         // Decimal 244 - 255 */
};

static const char ORDERED_ALPHABET[] = {
    '-', '0', '1', '2', '3',
    '4', '5', '6', '7', '8',
    '9', 'A', 'B', 'C', 'D',
    'E', 'F', 'G', 'H', 'I',
    'J', 'K', 'L', 'M', 'N',
    'O', 'P', 'Q', 'R', 'S',
    'T', 'U', 'V', 'W', 'X',
    'Y', 'Z', '_', 'a', 'b',
    'c', 'd', 'e', 'f', 'g',
    'h', 'i', 'j', 'k', 'l',
    'm', 'n', 'o', 'p', 'q',
    'r', 's', 't', 'u', 'v',
    'w', 'x', 'y', 'z' 
};

static const char ORDERED_DECODABET[] = {
    -9, -9, -9, -9, -9, -9,
    -9, -9, -9, // Decimal  0 -  8
    -5, -5, // Whitespace: Tab and Linefeed
    -9, -9, // Decimal 11 - 12
    -5, // Whitespace: Carriage Return
    -9, -9, -9, -9, -9, -9, -9, -9, -9, -9, -9, -9, -9, // Decimal 14 - 26
    -9, -9, -9, -9, -9, // Decimal 27 - 31
    -5, // Whitespace: Space
    -9, -9, -9, -9, -9, -9, -9, -9, -9, -9, // Decimal 33 - 42
    -9, // Plus sign at decimal 43
    -9, // Decimal 44
     0, // Minus sign at decimal 45
    -9, // Decimal 46
    -9, // Slash at decimal 47
     1, 2, 3, 4, 5, 6, 7, 8, 9, 10, // Numbers zero through nine
    -9, -9, -9, // Decimal 58 - 60
    -1, // Equals sign at decimal 61
    -9, -9, -9, // Decimal 62 - 64
    11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, // Letters 'A' through 'M'
    24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, // Letters 'N' through 'Z'
    -9, -9, -9, -9, // Decimal 91 - 94
    37, // Underscore at decimal 95
    -9, // Decimal 96
    38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, // Letters 'a' through 'm'
    51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, // Letters 'n' through 'z'
    -9, -9, -9, -9, // Decimal 123 - 126
 /* -9,-9,-9,-9,-9,-9,-9,-9,-9,-9,-9,-9,-9,     // Decimal 127 - 139
    -9,-9,-9,-9,-9,-9,-9,-9,-9,-9,-9,-9,-9,     // Decimal 140 - 152
    -9,-9,-9,-9,-9,-9,-9,-9,-9,-9,-9,-9,-9,     // Decimal 153 - 165
    -9,-9,-9,-9,-9,-9,-9,-9,-9,-9,-9,-9,-9,     // Decimal 166 - 178
    -9,-9,-9,-9,-9,-9,-9,-9,-9,-9,-9,-9,-9,     // Decimal 179 - 191
    -9,-9,-9,-9,-9,-9,-9,-9,-9,-9,-9,-9,-9,     // Decimal 192 - 204
    -9,-9,-9,-9,-9,-9,-9,-9,-9,-9,-9,-9,-9,     // Decimal 205 - 217
    -9,-9,-9,-9,-9,-9,-9,-9,-9,-9,-9,-9,-9,     // Decimal 218 - 230
    -9,-9,-9,-9,-9,-9,-9,-9,-9,-9,-9,-9,-9,     // Decimal 231 - 243
    -9,-9,-9,-9,-9,-9,-9,-9,-9,-9,-9,-9         // Decimal 244 - 255 */
};

const Base64Dialect Base64Dialect::STANDARD(STANDARD_ALPHABET, STANDARD_DECODABET, true);
const Base64Dialect Base64Dialect::URL_SAFE(URL_SAFE_ALPHABET, URL_SAFE_DECODABET, false);
const Base64Dialect Base64Dialect::ORDERED(ORDERED_ALPHABET, ORDERED_DECODABET, true);

}}}}