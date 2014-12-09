#ifndef MESSAGES_HPP
#define MESSAGES_HPP 

#include <map>
#include <functional>
#include <string>

#include <iostream>

#include "Context.hpp"

#include "picojson.h"

namespace chat {


struct RegisterMessageHandler {
  void operator()(Context& ctx, websocketpp::connection_hdl hdl, const picojson::object& obj) {
    std::cout << "Register called!" << std::endl;

    std::cout << "User " << obj.at("user") << " entered lobby" << std::endl;

    const std::string& username = obj.at("user").get<std::string>();
    ctx.rooms["lobby"][hdl] = UserData(username);

    ctx.broadcast_msg<JSONSystemEncoder>(username + " joined the chat.");

  };
};

struct TextMessageHandler {
  void operator()(Context& ctx, websocketpp::connection_hdl hdl, const picojson::object& obj) {
    std::cout << "Text message recieved!" << std::endl;
  };
};

struct UsersMessageHandler {
  void operator()(Context& ctx, websocketpp::connection_hdl hdl, const picojson::object& obj) {
    std::cout << "Users command recieved!" << std::endl;
  };
};

struct RoomsMessageHandler {
  void operator()(Context& ctx, websocketpp::connection_hdl hdl, const picojson::object& obj) {
    std::cout << "Rooms command recieved!" << std::endl;

    std::string rooms_str = "Available rooms: ";
    for( auto room_pair : ctx.rooms) {
      std::cout << room_pair.first << std::endl;
      rooms_str += " " + room_pair.first;
    };

    ctx.server.send(hdl, priv::create_system_msg(rooms_str), websocketpp::frame::opcode::text);
  };
};


class MessageManager {

public:

  MessageManager() {

    handlers["register"] = RegisterMessageHandler();
    handlers["text"] = TextMessageHandler();
    handlers["users"] = UsersMessageHandler();
    handlers["rooms"] = RoomsMessageHandler();

  };

  void process_message(Context& ctx, websocketpp::connection_hdl con_hdl, const picojson::object& obj) {

    if(priv::has_key(obj, std::string("type")) &&
        obj.at("type").is<std::string>()) {

      std::string type = obj.at("type").get<std::string>();

      if(priv::has_key(handlers, type)) {
        auto hdl = handlers.at(type);
        hdl(ctx, con_hdl, obj);
      } else {
        std::cerr << "Error: no handler for type: " + type << std::endl;
      };

    } else {

      std::cout << "No type member!" << std::endl;

    };

  };

private:

  typedef std::map<std::string, std::function<void(Context&, websocketpp::connection_hdl, const picojson::object&)>> HandlerMap;
  HandlerMap handlers;


};


}


#endif /* MESSAGES_HPP */
