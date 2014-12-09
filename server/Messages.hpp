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
    std::string msg = "Users connected: ";
    for(const auto& r : ctx.rooms) {
      for(const auto& u : r.second) {
        msg += u.second.name + " ";
      }
    }

    ctx.server.send(hdl, JSONSystemEncoder::encode(msg), websocketpp::frame::opcode::text);
  };
};

struct RoomsMessageHandler {
  void operator()(Context& ctx, websocketpp::connection_hdl hdl, const picojson::object& obj) {
    std::string rooms_str = "Available rooms: ";
    for( auto room_pair : ctx.rooms) {
      std::cout << room_pair.first << std::endl;
      rooms_str += " " + room_pair.first;
    };

    ctx.server.send(hdl, JSONSystemEncoder::encode(rooms_str), websocketpp::frame::opcode::text);
  };
};

struct RoomMessageHandler {
  void operator()(Context& ctx, websocketpp::connection_hdl hdl, const picojson::object& obj) {
    boost::optional<std::string> room_name = ctx.get_room(hdl);
    if(room_name) {
      ctx.server.send(hdl, JSONSystemEncoder::encode("Current room: " + room_name.get()),
          websocketpp::frame::opcode::text);
    }
  };
};

struct CreateMessageHandler {
  void operator()(Context& ctx, websocketpp::connection_hdl hdl, const picojson::object& obj) {
    const std::string& room_name = obj.at("name").get<std::string>();
    if(priv::has_key(ctx.rooms, room_name)) {
      ctx.server.send(hdl, JSONSystemEncoder::encode("Room already exist."), websocketpp::frame::opcode::text);
    } else {
      ctx.rooms[room_name] = Room();

      std::string msg = "New room \"" + room_name + "\" created.";
      ctx.broadcast_msg<JSONSystemEncoder>(msg);
    }
  };
};


class MessageManager {

public:

  MessageManager() {

    handlers["register"] = RegisterMessageHandler();
    handlers["text"] = TextMessageHandler();
    handlers["users"] = UsersMessageHandler();
    handlers["rooms"] = RoomsMessageHandler();
    handlers["room"] = RoomMessageHandler();
    handlers["create"] = CreateMessageHandler();

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
