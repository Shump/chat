#ifndef CONTEXT_HPP
#define CONTEXT_HPP 

#include <map>
#include <string>

#include <iostream>

#include "websocketpp/config/asio_no_tls.hpp"
#include "websocketpp/server.hpp"

#include "picojson.h"

#include <boost/optional.hpp>

namespace chat {

using boost::optional;
using conn_hdl = websocketpp::connection_hdl;

  namespace priv {
    template<typename K, typename V, typename C>
    bool has_key(const std::map<K, V, C>& m, K k) {
      auto search = m.find(k);
      return search != m.end() ? true : false;
    };
  }

struct JSONSystemEncoder {
  static std::string encode(const std::string& text) {
    picojson::object obj;
    obj["type"] = picojson::value("system");
    obj["text"] = picojson::value(text);
    return picojson::value(obj).serialize();
  };
};

struct UserData {
  UserData() {};
  UserData(const std::string& name) : name(name) {};
  std::string name;
};

typedef std::map<conn_hdl, UserData, std::owner_less<conn_hdl>> Room;


struct Context {

  typedef websocketpp::server<websocketpp::config::asio> ServerType;
  ServerType server;

  typedef std::map<std::string, Room> RoomsType;
  RoomsType rooms;

  template<typename Encoder = JSONSystemEncoder>
  void send_msg(conn_hdl hdl, const std::string& msg) {
    server.send(hdl, Encoder::encode(msg), websocketpp::frame::opcode::text);
  };

  template<typename Encoder = JSONSystemEncoder>
  void broadcast_msg(const std::string& msg) {
    for(auto r : rooms) {
      for(auto u : r.second) {
        send_msg<Encoder>(u.first, msg);
      }
    }
  };

  template<typename Encoder = JSONSystemEncoder>
  void roomcast_msg(const std::string& room, const std::string& msg) {
    if(priv::has_key(rooms, room)) {
      auto r = rooms[room];
      for(auto u : r) {
        send_msg<Encoder>(u.first, msg);
      }
    } else {
      std::cerr << "roomcast error: " + room + " does not exist!" << std::endl;
    }
  };
  
  optional<std::string> get_room(conn_hdl hdl) {
    for(auto& r : rooms) {
      if(priv::has_key(r.second, hdl)) {
        return optional<std::string>(r.first);
      }
    }
    return optional<std::string>();
  };

  optional<UserData> get_user_data(conn_hdl hdl) {
    optional<std::string> room_name = get_room(hdl);
    if(room_name) {
      return optional<UserData>(rooms[room_name.get()][hdl]);
    } else {
      return optional<UserData>();
    }
  };

  void change_room(conn_hdl hdl, const std::string& new_room) {
    optional<std::string> room_name = get_room(hdl);
    if(room_name) {
      if(priv::has_key(rooms, new_room)) {
        UserData ud = rooms[room_name.get()][hdl];
        rooms[room_name.get()].erase(hdl);
        rooms[new_room][hdl] = ud;

        broadcast_msg("User " + ud.name + " joined room " + new_room);
      } else {
        send_msg(hdl, "Room \"" + new_room + "\" does not exist.");
      }
    } else {
      std::cerr << "Room does not exist for handler" << std::endl;
    }
  };
  
  void remove_user(conn_hdl hdl) {
    optional<std::string> room_name = get_room(hdl);
    if(room_name) {
      UserData ud = rooms[room_name.get()][hdl];
      rooms[room_name.get()].erase(hdl);

      broadcast_msg(ud.name + " left the chat.");
    }
  };

};





}



#endif /* CONTEXT_HPP */
