
#include <functional>
#include <iostream>
#include <string>
#include "websocketpp/config/asio_no_tls.hpp"
#include "websocketpp/server.hpp"


class ChatServer {
  typedef websocketpp::server<websocketpp::config::asio> Server;

public:

  explicit ChatServer(unsigned int port) {

    server.set_message_handler(std::bind(&ChatServer::on_message, this, std::placeholders::_1, std::placeholders::_2));
    
    server.init_asio();
    server.listen(port);
    server.start_accept();
  };

  void run() {
    server.run();
  };


private:

  void on_message(websocketpp::connection_hdl hdl, Server::message_ptr msg) {
    std::cout << msg->get_payload() << std::endl;
  };

  Server server;
};

int main() {

  ChatServer server(12345);
  server.run();

}




