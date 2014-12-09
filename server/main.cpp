
#include <functional>
#include <iostream>
#include <string>
#include <sstream>
#include <exception>

#include "Context.hpp"
#include "Messages.hpp"

#include "websocketpp/config/asio_no_tls.hpp"
#include "websocketpp/server.hpp"

#include "picojson.h"

namespace chat {


picojson::object parse_json_message(const std::string& str) {
  std::istringstream buf(str);
  picojson::value v;
  std::string err;
  err = picojson::parse(v, buf);
  if(err.empty() && v.is<picojson::object>()) {
    return v.get<picojson::object>();
  } else {
    throw std::runtime_error("Unable to parse JSON string");
  }
};


class ChatServer {
typedef Context::ServerType Server;
public:

  explicit ChatServer(unsigned int port) {
    using std::placeholders::_1;
    using std::placeholders::_2;

    Context::RoomsType& rooms = context.rooms;
    Context::ServerType& server = context.server;

    rooms["lobby"] = Room();

    server.set_message_handler(std::bind(&ChatServer::on_message, this, _1, _2));
    server.set_close_handler(std::bind(&ChatServer::on_close, this, _1));
    
    server.init_asio();
    server.listen(port);
    server.start_accept();
  };

  void run() {
    context.server.run();
  };

private:

  void on_message(websocketpp::connection_hdl hdl, Server::message_ptr msg) {
    try {

      auto val = parse_json_message(msg->get_payload());
      msg_manager.process_message(context, hdl, val);

    } catch( const std::exception& ex) {
      std::cerr << ex.what() << std::endl;
    };
  };

  void on_close(websocketpp::connection_hdl hdl) {
    context.remove_user(hdl);
  };

  Context context;
  MessageManager msg_manager;
};

}

int main(int argc, char *argv[]) {
  int port = 12345;

  if(argc > 1) {
    try {
      port = std::stoi(argv[1]);
    } catch(const std::exception& e) {
      std::cout << 
        "Unable to convert portnumber: " << argv[1] << ", using defualt port 12345" <<std::endl;
    }
  } else {
    std::cout << "No arguments, defualting listen port to 12345" << std::endl;
  }

  chat::ChatServer server(port);
  server.run();

}



