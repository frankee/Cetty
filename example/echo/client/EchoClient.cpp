// echo.cpp : Defines the entry point for the console application.
//
#include "boost/thread.hpp"
#include "boost/date_time.hpp"

#include "EchoClientHandler.h"

#include "cetty/bootstrap/ClientBootstrap.h"
#include "cetty/channel/socket/asio/AsioClientSocketChannelFactory.h"
#include "cetty/channel/IpAddress.h"
#include "cetty/channel/SocketAddress.h"
#include "cetty/channel/Channels.h"
#include "cetty/channel/ChannelFuture.h"


using namespace cetty::channel;
using namespace cetty::channel::socket::asio;

using namespace cetty::bootstrap;
using namespace cetty::buffer;

using namespace cetty::util;

int main(int argc, char* argv[])
{
    // Print usage if no argument is specified.
    if (argc < 3 || argc > 7) {
        printf(
            "Usage: EchoClient \n\ <host> <port> [<first message size> <client count> <send message intervals> <io thread count>]");
        return -1;
    }

    // Parse options.
    std::string host = argv[1];
    int port = atoi(argv[2]);
    int firstMessageSize = 256;
    if (argc >= 4) {
        firstMessageSize = atoi(argv[3]);
    }

    int clientCount = 1;
    if (argc >= 5) {
        clientCount = atoi(argv[4]);
    }

    int sendIntervals = 0;
    if (argc >= 6) {
        sendIntervals = atoi(argv[5]);
    }

    int ioThreadCount = 1;
    if (argc >= 7) {
        ioThreadCount = atoi(argv[6]);
    }

    // Configure the client.
    ChannelFactoryPtr factory = ChannelFactoryPtr(new AsioClientSocketChannelFactory(ioThreadCount));
    ClientBootstrap bootstrap(factory);

    // Set up the pipeline factory.
    bootstrap.setPipelineFactory(Channels::pipelineFactory(
        Channels::pipeline(ChannelHandlerPtr(
            new EchoClientHandler(firstMessageSize, sendIntervals)))));

    // Start the connection attempt.
    std::vector<Channel*> clientChannels;
    for (int i = 0; i < clientCount; ++i) {
        ChannelFuturePtr future = bootstrap.connect(SocketAddress(host, port));
        BOOST_ASSERT(future->awaitUninterruptibly().isSuccess());

        clientChannels.push_back(&(future->getChannel()));
    }

    // Wait until the connection is closed or the connection attempt fails.
    for (size_t i = 0; i < clientChannels.size(); ++i) {
        clientChannels[i]->getCloseFuture()->awaitUninterruptibly();
    }

    // Shut down thread pools to exit.
    bootstrap.releaseExternalResources();

	return 0;
}

