# HTML 5 GUI Demo
Demos for using HTML 5 as a GUI for C++ apps

# Demos list

* [ws-simple](ws-simple): A simple echo-server using WebSockets. Uses [Boost.Beast](https://github.com/boostorg/beast)
* [cef-async](cef-async): A simple [CEF](https://bitbucket.org/chromiumembedded/cef-project/src/master/) echo application using *asynchronous* communication
* [cef-sync](cef-sync): A simple [CEF](https://bitbucket.org/chromiumembedded/cef-project/src/master/) echo application using synchronous communication
* [fs-browser](fs-browser): A simple filesystem browser demo with two implementations: WebSockets and CEF

# Building

*This repo has submodules*. Clone with `--recursive` or call `git submodule update --init` after cloning.

There's a master CMakeLists.txt in the root directory, which can be used to build all demos. It downloads and prepares all dependencies, except Boost. You need to configure with `-DHTML5_DEMO_BOOST_DIR=path/to/your/boost/installation` or have Boost in your global include and library paths.

Currently the only supported configuration is to build all demos. I may provide configuration options to build only selected ones, but it won't be soon. In the meantime I welcome pull requests which do so.

# License

All demos under the MIT license. See accompanying file LICENSE or copy [here](https://opensource.org/licenses/MIT).

Copyright &copy; 2019 [Borislav Stanimirov](https://ibob.github.io/)
