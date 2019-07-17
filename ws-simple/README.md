# ws-simple

A simple WebSocket echo server and client.

* The C++ server uses [Boost.Beast](https://github.com/boostorg/beast)
* The HTML 5 client doesn't have any dependencies

Note that the server is WebSockets only. You will need an additional simple HTTP server to provide the HTML static content. Any HTTP server will do. For example one of the simplest ones is `python -m SimpleHTTPServer`.
