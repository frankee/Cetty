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

#include "cetty/handler/codec/http/DefaultHttpHeader.h"

#include <string.h>

#include "cetty/handler/codec/http/HttpCodecUtil.h"
#include "cetty/util/Integer.h"

namespace cetty { namespace handler { namespace codec { namespace http {

using namespace cetty::util;

const std::string DefaultHttpHeader::EMPTY_VALUE;

DefaultHttpHeader::DefaultHttpHeader()
    : head(new Entry), freelist(NULL) {
    head->before = head->after = head;
    memset(entries, 0, sizeof(entries));
}

DefaultHttpHeader::~DefaultHttpHeader() {
    clear();
    clearFreeList();
    if (head) {
        delete head;
    }
}

void DefaultHttpHeader::validateHeaderName(const std::string& name) {
    HttpCodecUtil::validateHeaderName(name);
}

void DefaultHttpHeader::add(const std::string& name, const std::string& value) {
    validateHeaderName(name);
    HttpCodecUtil::validateHeaderValue(value);

    int h = hash(name);
    int i = index(h);
    addHeader(h, i, name, value);
}

void DefaultHttpHeader::add(const std::string& name, int value) {
    add(name, Integer::toString(value));
}

void DefaultHttpHeader::remove(const std::string& name) {
    if (name.empty()) return;

    int h = hash(name);
    int i = index(h);
    removeHeader(h, i, name);
}

void DefaultHttpHeader::remove(const std::string& name, const std::string& value) {
    if (name.empty()) return;

    int h = hash(name);
    int i = index(h);
    removeHeader(h, i, name, value);
}

void DefaultHttpHeader::set(const std::string& name, const std::string& value) {
    if (value.empty()) return;

    validateHeaderName(name);
    HttpCodecUtil::validateHeaderValue(value);

    int h = hash(name);
    int i = index(h);
    removeHeader(h, i, name);
    addHeader(h, i, name, value);
}

void DefaultHttpHeader::set(const std::string& name, const StringList& values) {
    if (values.empty()) {
        return;
    }

    validateHeaderName(name);

    int h = hash(name);
    int i = index(h);

    removeHeader(h, i, name);

    for (size_t i = 0; i < values.size(); ++i) {
        if (values[i].empty()) {
            break;
        }

        HttpCodecUtil::validateHeaderValue(values[i]);
        addHeader(h, i, name, values[i]);
    }
}

void DefaultHttpHeader::set(const std::string& name, int value) {
    set(name, Integer::toString(value));
}

void DefaultHttpHeader::set(const std::string& name, const IntList& values) {
    StringList list;
    for (size_t i = 0; i < values.size(); ++i) {
        list.push_back(Integer::toString(values[i]));
    }
    set(name, list);
}

void DefaultHttpHeader::clear() {
    for (int i = 0; i < BUCKET_SIZE; ++i) {
        Entry* e = entries[i];
        while (e != NULL) {
            Entry* next = e->next;
            delete e;
            e = next;
        }
        entries[i] = NULL;
    }
    head->before = head->after = head;
}

const std::string& DefaultHttpHeader::get(const std::string& name) const {
    if (name.empty()) {
        return EMPTY_VALUE;
    }

    int h = hash(name);
    int i = index(h);
    Entry* e = entries[i];
    while (e != NULL) {
        if (e->hash == h && eq(name, e->key)) {
            return e->value;
        }

        e = e->next;
    }
    return EMPTY_VALUE;
}

HttpHeader::StringList DefaultHttpHeader::gets(const std::string& name) const {
    StringList values;
    if (name.empty()) {
        return values;
    }
    this->gets(name, values);
    return values;
}

void DefaultHttpHeader::gets(const std::string& name, StringList& headers) const {
    if (name.empty()) {
        return;
    }
    headers.clear();

    int h = hash(name);
    int i = index(h);
    Entry* e = entries[i];
    while (e != NULL) {
        if (e->hash == h && eq(name, e->key)) {
            headers.push_back(e->value);
        }
        e = e->next;
    }
}

HttpHeader::NameValueList DefaultHttpHeader::gets() const {
    NameValueList all;

    Entry* e = head->after;
    while (e != head) {
        all.push_back(std::make_pair(e->key, e->value));
        e = e->after;
    }
    return all;
}

void DefaultHttpHeader::gets(NameValueList& nameValues) const {
    nameValues.clear();

    Entry* e = head->after;
    while (e != head) {
        nameValues.push_back(std::make_pair(e->key, e->value));
        e = e->after;
    }
}

bool DefaultHttpHeader::contains(const std::string& name) const {
    return !get(name).empty();
}

HttpHeader::StringList DefaultHttpHeader::getNames() const {
    StringList names;

    Entry* e = head->after;
    while (e != head) {
        names.push_back(e->key);
        e = e->after;
    }
    return names;
}

void DefaultHttpHeader::getNames(StringList& names) const {
    names.clear();

    Entry* e = head->after;
    while (e != head) {
        names.push_back(e->key);
        e = e->after;
    }
}

DefaultHttpHeader::Entry* DefaultHttpHeader::newEntry(int h, const std::string& name, const std::string& value) {
    if (freelist) {
        Entry* entry = freelist;
        freelist = freelist->next;
        return entry;
    }

    return new Entry(h, name, value);
}

void DefaultHttpHeader::deleteEntry(Entry* entry) {
    entry->clear();

    if (freelist) {
        entry->next = freelist;
        freelist = entry;
    }

    freelist = entry;
}

void DefaultHttpHeader::clearFreeList() {
    Entry* entry;
    while(freelist) {
        entry = freelist->next;
        delete freelist;
        freelist = entry;
    }
}

void DefaultHttpHeader::addHeader(int h, int i, const std::string& name, const std::string& value) {
    // Update the hash table.
    Entry* e = entries[i];
    Entry* entry;
    entries[i] = entry = newEntry(h, name, value);
    entry->next = e;

    // Update the linked list.
    entry->addBefore(head);
}

void DefaultHttpHeader::removeHeader(int h, int i, const std::string& name) {
    Entry* e = entries[i];
    if (e == NULL) {
        return;
    }

    for (;;) {
        if (e->hash == h && eq(name, e->key)) {
            e->remove();
            Entry* next = e->next;
            if (next != NULL) {
                entries[i] = next;
                e = next;
            }
            else {
                entries[i] = NULL;
                return;
            }
        }
        else {
            break;
        }
    }

    for (;;) {
        Entry* next = e->next;
        if (next == NULL) {
            break;
        }
        if (next->hash == h && eq(name, next->key)) {
            e->next = next->next;
            next->remove();
        }
        else {
            e = next;
        }
    }
}

void DefaultHttpHeader::removeHeader(int h, int i, const std::string& name, const std::string& value) {
    Entry* e = entries[i];
    if (e == NULL) {
        return;
    }

    for (;;) {
        if (e->hash == h && eq(name, e->key) && eq(value, e->value)) {
            e->remove();
            Entry* next = e->next;
            if (next != NULL) {
                entries[i] = next;
                e = next;
            }
            else {
                entries[i] = NULL;
                return;
            }
        }
        else {
            break;
        }
    }

    for (;;) {
        Entry* next = e->next;
        if (next == NULL) {
            break;
        }
        if (next->hash == h && eq(name, next->key) && eq(value, next->value)) {
            e->next = next->next;
            next->remove();
        }
        else {
            e = next;
        }
    }
}

int DefaultHttpHeader::hash(const std::string& name) {
    int h = 0;
    for (int i = (int)name.size() - 1; i >= 0; --i) {
        char c = name[i];
        if (c >= 'A' && c <= 'Z') {
            c += 32;
        }
        h = 31 * h + c;
    }

    if (h > 0) {
        return h;
    }
    else if (h == Integer::MIN_VALUE) {
        return Integer::MAX_VALUE;
    }
    else {
        return -h;
    }
}

bool DefaultHttpHeader::eq(const std::string& name1, const std::string& name2) {
    size_t nameLen = name1.size();
    if (nameLen != name2.size()) {
        return false;
    }

    for (int i = (int)nameLen - 1; i >= 0; --i) {
        char c1 = name1.at(i);
        char c2 = name2.at(i);
        if (c1 != c2) {
            if (c1 >= 'A' && c1 <= 'Z') {
                c1 += 32;
            }
            if (c2 >= 'A' && c2 <= 'Z') {
                c2 += 32;
            }
            if (c1 != c2) {
                return false;
            }
        }
    }
    return true;
}

int DefaultHttpHeader::index(int hash) {
    return hash % BUCKET_SIZE;
}

}}}}
