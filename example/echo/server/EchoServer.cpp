// echo.cpp : Defines the entry point for the console application.
//

#include <boost/thread.hpp>
#include <boost/date_time.hpp>

#include "EchoServerHandler.h"

#include "cetty/bootstrap/ServerBootstrap.h"

#include "cetty/channel/Channels.h"
#include "cetty/channel/IpAddress.h"
#include "cetty/channel/SocketAddress.h"
#include "cetty/channel/socket/asio/AsioServerSocketChannelFactory.h"

using namespace cetty::channel;
using namespace cetty::channel::socket::asio;

using namespace cetty::bootstrap;
using namespace cetty::buffer;

using namespace cetty::util;

int main(int argc, char* argv[]) {
    int threadCount = -1;
    if (argc == 2) {
        threadCount = atoi(argv[1]);
    }

    ServerBootstrap bootstrap(ChannelFactoryPtr(new AsioServerSocketChannelFactory(threadCount)));
    bootstrap.setPipeline(Channels::pipeline(ChannelHandlerPtr(new EchoServerHandler)));

	bootstrap.setOption("child.tcpNoDelay", boost::any(true));
	bootstrap.setOption("reuseAddress", boost::any(true));
    bootstrap.setOption("backlog", boost::any(4096));

    // Bind and start to accept incoming connections.
    Channel* c = bootstrap.bind(SocketAddress(IpAddress::IPv4, 1980));
    if (c->isBound()) {
        printf("Server is running...\n");
        printf("To quit server, press 'q'.\n");
        
        char input;
        do {
            input = getchar();
            if (input == 'q') {
                c->close()->awaitUninterruptibly();
                bootstrap.releaseExternalResources();
                return 0;
            }
        } while (true);
    }

    bootstrap.releaseExternalResources();
	return -1;
}
