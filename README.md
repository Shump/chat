

Simple chat client and server using websockets.

The client is written using html5 websockets using angular.js as it's framework. I'm relatively inexperienced in JavaScript so it is not very pretty code.

The server is written in c++11 using the websocketspp and picojson libraries.

Requirements:
  - boost library:
    - Anything required by the websocketpp library (mainly boost-asio and boost-system).
    - boost-optional
  - A c++11-capable compiler.

The makefile assumes the boost library is installed in the compiler's default paths. All other libraries are included in the repository for simplicity.


