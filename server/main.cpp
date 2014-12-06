
#include <functional>
#include <iostream>
#include <string>
#include <sstream>
#include <exception>
#include <memory>

#include <map>
#include <list>

#include "Room.hpp"
#include "Message.hpp"

#include "websocketpp/config/asio_no_tls.hpp"
#include "websocketpp/server.hpp"

#include "picojson.h"


class ChatServer {
typedef websocketpp::server<websocketpp::config::asio> Server;
public:

  explicit ChatServer(unsigned int port) {

    rooms["lobby"] = Room();

    server.set_message_handler(std::bind(&ChatServer::on_message, this, 
          std::placeholders::_1, std::placeholders::_2));
    
    server.init_asio();
    server.listen(port);
    server.start_accept();
  };

  void run() {
    server.run();
  };


private:

  picojson::value parse_message(const std::string& str) {
    std::istringstream buf(str);
    picojson::value v;
    std::string err;
    err = picojson::parse(v, buf);
    if(err.empty() && v.is<picojson::object>()) {
      return v;
    } else {
      throw std::runtime_error("Unable to parse JSON string");
    }
  };

  void on_message(websocketpp::connection_hdl hdl, Server::message_ptr msg) {
    std::cout << msg->get_payload() << std::endl;

    try {

      auto val = parse_message(msg->get_payload());
      auto msg = create_message<ChatServer>(val);

    } catch( const std::exception& ex) {
      std::cerr << ex.what() << std::endl;
    };
  };

  Server server;

  std::map<std::string, Room> rooms;
};

int main() {

  ChatServer server(12345);
  server.run();

}




