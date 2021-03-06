#if !defined(CETTY_CHANNEL_SOCKET_ASIO_ASIOIPADDRESS_H)
#define CETTY_CHANNEL_SOCKET_ASIO_ASIOIPADDRESS_H

// Copyright (c) 2005-2006, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// Permission is hereby granted, free of charge, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, reproduce, display, distribute,
// execute, and transmit the Software, and to prepare derivative works of the
// Software, and to permit third-parties to whom the Software is furnished to
// do so, all subject to the following:
// 
// The copyright notices in the Software and this entire statement, including
// the above license grant, this restriction and the following disclaimer,
// must be included in all copies of the Software, in whole or in part, and
// all derivative works of the Software, unless such copies or derivative
// works are solely in the form of machine-executable object code generated by
// a source language processor.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//

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

#include <boost/asio.hpp>

#include <boost/asio/ip/address.hpp>
#include "cetty/channel/IpAddressImpl.h"
#include "cetty/channel/IpAddress.h"

namespace cetty { namespace channel { namespace socket { namespace asio {

using namespace cetty::channel;

class AsioIpAddressImpl : public cetty::channel::IpAddressImpl {
public:
    typedef boost::asio::ip::address    address_type;
    typedef boost::asio::ip::address_v4 address_v4_type;
    typedef boost::asio::ip::address_v6 address_v6_type;

public:
    AsioIpAddressImpl(int family) {
        if (family == IpAddress::IPv4) {
            address = address_type(address_v4_type());
        }
        address = address_type(address_v6_type());
    }

    AsioIpAddressImpl(const std::string& addr)
        : address(address_type::from_string(addr)) {}

    AsioIpAddressImpl(const std::string& addr, int family) {
        if (family == IpAddress::IPv4) {
            address = address_type(address_v4_type::from_string(addr));
        }
        address = address_type(address_v6_type::from_string(addr));
    }

    AsioIpAddressImpl(const address_type& address) : address(address) {}

    virtual ~AsioIpAddressImpl() {}

    virtual std::string toString() const {
        return address.to_string();
    }

    virtual int family() const {
        return address.is_v4() ? IpAddress::IPv4 : IpAddress::IPv6;
    }

    virtual bool isWildcard() const	{
        return address.is_v4() ? address.to_v4().to_ulong() == 0
                               : address.to_v6().is_unspecified();
    }

    virtual bool isBroadcast() const {
        //return address.is_v4() ? address.to_v4().is_multicast()
        return true;
    }
    virtual bool isLoopback() const {
        //return address.is_v4() ? address.to_v4()
        return true;
    }
    virtual bool isMulticast() const {
        return true;
    }
    virtual bool isLinkLocal() const {
        return true;
    }
    virtual bool isSiteLocal() const {
        return true;
    }
    virtual bool isIPv4Mapped() const {
        return address.is_v4() ? true : address.to_v6().is_v4_mapped();
    }
    virtual bool isIPv4Compatible() const {
        return address.is_v4() ? true : address.to_v6().is_v4_compatible();
    }
    virtual bool isWellKnownMC() const {
        return true;
    }
    virtual bool isNodeLocalMC() const {
        return true;
    }
    virtual bool isLinkLocalMC() const {
        return true;
    }
    virtual bool isSiteLocalMC() const {
        return true;
    }
    virtual bool isOrgLocalMC() const {
        return true;
    }
    virtual bool isGlobalMC() const {
        return true;
    }

    virtual bool equals(const IpAddressImpl& ipAddress) const {
        return true;
    }
    virtual int compareTo(const IpAddressImpl& ipAddress) const {
        return 0;
    }

    virtual void mask(const IpAddressImpl& pMask, const IpAddressImpl& pSet) {

    }
    virtual IpAddressImpl* clone() const {
        return NULL;
    }

private:
    address_type address;
};

}}}}

#endif //#if !defined(CETTY_CHANNEL_SOCKET_ASIO_ASIOIPADDRESS_H)
