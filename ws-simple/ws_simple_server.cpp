// HTML 5 GUI Demo
// Copyright (c) 2019 Borislav Stanimirov
//
// Distributed under the MIT Software License
// See accompanying file LICENSE.txt or copy at
// https://opensource.org/licenses/MIT
//
#include <iostream>

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/ip/tcp.hpp>

#include <cstdlib>
#include <functional>
#include <iostream>
#include <string>
#include <thread>
#include <memory>
#include <set>
#include <deque>
#include <chrono>
#include <cassert>

using namespace std;
using namespace std::chrono_literals;

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

atomic_uint32_t sid = 0;

struct Packet
{
    string textBuffer;
    vector<uint8_t> binaryBuffer;
    bool text = false;
};

using PacketPtr = shared_ptr<Packet>;

class Session : public enable_shared_from_this<Session>
{
public:
    Session(tcp::socket&& socket)
        : m_id(++sid)
        , m_ws(move(socket))
    {
        // set suggested timeout settings for the websocket
        m_ws.set_option(websocket::stream_base::timeout::suggested(beast::role_type::server));

        // set a decorator to change the Server of the handshake
        m_ws.set_option(websocket::stream_base::decorator(
            [](websocket::response_type& res)
            {
                res.set(http::field::server, string(BOOST_BEAST_VERSION_STRING) + "ws-simple-server");
            }));

        cout << "Created session " << m_id << '\n';
    }

    void run()
    {
        m_ws.async_accept(beast::bind_front_handler(&Session::onAccept, shared_from_this()));
    }

    void onAccept(beast::error_code e)
    {
        if (e) return fail(e, "accept");
        doRead();
    }

    void doRead()
    {
        m_ws.async_read(m_readBuf, beast::bind_front_handler(&Session::onRead, shared_from_this()));
    }

    void onRead(beast::error_code e, size_t /*length*/)
    {
        if (e == websocket::error::closed) return close();
        if (e) return fail(e, "read");

        if (!m_ws.got_text())
        {
            cout << "o_O Got non text. Ignoring.\n";
        }
        else
        {
            auto packet = make_shared<Packet>();
            packet->text = true;
            auto data = reinterpret_cast<const char*>(m_readBuf.cdata().data());
            packet->textBuffer.assign(data, data + m_readBuf.size());
            onReceive(packet);
        }

        m_readBuf.clear();
        doRead(); // read loop
    }

    void onReceive(const PacketPtr& packet)
    {
        cout << "Received: " << packet->textBuffer << '\n';
        // echo
        send(packet);
    }

    void send(const PacketPtr& packet)
    {
        m_writeQueue.emplace_back(packet);
        if (m_writeQueue.size() > 1) return; // we're already writing

        doWrite();
    }

    void doWrite()
    {
        assert(!m_writeQueue.empty());

        auto& packet = m_writeQueue.front();
        m_ws.text(packet->text);
        auto handler = beast::bind_front_handler(&Session::onWrite, shared_from_this());
        if (packet->text) m_ws.async_write(net::buffer(packet->textBuffer), std::move(handler));
        else m_ws.async_write(net::buffer(packet->binaryBuffer), std::move(handler));
    }

    void onWrite(beast::error_code e, std::size_t)
    {
        if (e) return fail(e, "write");

        m_writeQueue.pop_front();
        if (m_writeQueue.empty()) return;

        doWrite();
    }

    void fail(beast::error_code e, const char* source)
    {
        cerr << "Session " << m_id << " error: " << e <<  " in " << source << '\n';
    }

    void close()
    {
        cout << "Session " << m_id << " closed \n";
    }

private:
    const uint32_t m_id;
    websocket::stream<tcp::socket> m_ws;

    // io
    beast::flat_buffer m_readBuf, m_writeBuf;

    deque<PacketPtr> m_writeQueue;
};

class Server
{
public:
    Server(tcp::endpoint endpoint)
        : m_context(1)
        , m_acceptor(m_context, endpoint)
    {}

    int run()
    {
        doAccept();
        m_context.run();
        return 0;
    }

    void doAccept()
    {
        m_acceptor.async_accept(beast::bind_front_handler(&Server::onAccept, this));
    }

    void onAccept(beast::error_code error, tcp::socket socket)
    {
        if (error)
        {
            cerr << "Server::onAccept error: " << error << '\n';
            return;
        }

        auto session = make_shared<Session>(move(socket));
        session->run();

        doAccept();
    }

private:
    net::io_context m_context;
    tcp::acceptor m_acceptor;
};

int main()
{
    ///////////////////////////////////////////////////////////////////////////
    // args
    const auto address = boost::asio::ip::tcp::v4(); // net::ip::make_address(argAddr);
    const uint16_t port = 7654;

    ///////////////////////////////////////////////////////////////////////////
    // run
    Server server(tcp::endpoint(address, port));
    return server.run();
}