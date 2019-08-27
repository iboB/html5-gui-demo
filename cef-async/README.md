# cef-async

A simple [CEF](https://bitbucket.org/chromiumembedded/cef-project/src/master/)-based echo application

* The render process communicates with JS bindings based on the [jsbind](https://github.com/Chobolabs/jsbind/) library
* It sends a process message to the browser process with the input from clients
* The browser process send process messages back to the render process for a response
