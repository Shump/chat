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
    const std::string& text = obj.at("text").get<std::string>();

    boost::optional<std::string> room_name = ctx.get_room(hdl);
    boost::optional<UserData> ud = ctx.get_user_data(hdl);
    if(ud || room_name) {
      std::string format_msg = ud.get().name + ": " + text;
      ctx.roomcast_msg(room_name.get(), format_msg);
    } else {
      std::cerr << "No user data for handle" << std::endl;
    }
    
  };
};

struct UsersMessageHandler {
  void operator()(Context& ctx, websocketpp::connection_hdl hdl, const picojson::object& obj) {
    std::string msg = "Users connected: ";
    for(const auto& r : ctx.rooms) {
      std::string room_name = r.first;
      for(const auto& u : r.second) {
        msg += u.second.name + "(" + room_name + ") ";
      }
    }

    ctx.send_msg(hdl, msg);
  };
};

struct RoomsMessageHandler {
  void operator()(Context& ctx, websocketpp::connection_hdl hdl, const picojson::object& obj) {
    std::string rooms_str = "Available rooms: ";
    for( auto room_pair : ctx.rooms) {
      std::cout << room_pair.first << std::endl;
      rooms_str += " " + room_pair.first;
    };

    ctx.send_msg(hdl, rooms_str);
  };
};

struct RoomMessageHandler {
  void operator()(Context& ctx, websocketpp::connection_hdl hdl, const picojson::object& obj) {
    boost::optional<std::string> room_name = ctx.get_room(hdl);
    if(room_name) {
      ctx.send_msg(hdl, "Current room: " + room_name.get());
    }
  };
};

struct CreateMessageHandler {
  void operator()(Context& ctx, websocketpp::connection_hdl hdl, const picojson::object& obj) {
    const std::string& room_name = obj.at("name").get<std::string>();
    if(priv::has_key(ctx.rooms, room_name)) {
      ctx.send_msg(hdl, "Room already exist.");
    } else {
      ctx.rooms[room_name] = Room();

      std::string msg = "New room \"" + room_name + "\" created.";
      ctx.broadcast_msg<JSONSystemEncoder>(msg);
    }
  };
};

struct JoinMessageHander {
  void operator()(Context& ctx, websocketpp::connection_hdl hdl, const picojson::object& obj) {
    const std::string& room_name = obj.at("name").get<std::string>();
    ctx.change_room(hdl, room_name);
    ctx.send_msg(hdl, "Changed room to: " + room_name);
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
    handlers["join"] = JoinMessageHander();

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
