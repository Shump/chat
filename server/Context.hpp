#ifndef CONTEXT_HPP
#define CONTEXT_HPP 

#include <map>
#include <string>

#include <iostream>

#include "websocketpp/config/asio_no_tls.hpp"
#include "websocketpp/server.hpp"

#include "picojson.h"

#include <boost/optional.hpp>

//#include "Room.hpp"

namespace chat {

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
  UserData(const std::string& name): 
    name(name) {};
  std::string name;
};

typedef std::map<websocketpp::connection_hdl, UserData, std::owner_less<websocketpp::connection_hdl>> Room;


struct Context {

  typedef websocketpp::server<websocketpp::config::asio> ServerType;
  ServerType server;

  typedef std::map<std::string, Room> RoomsType;
  RoomsType rooms;

  template<typename Encoder = JSONSystemEncoder>
  void send_msg(websocketpp::connection_hdl hdl, const std::string& msg) {
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
  
  boost::optional<std::string> get_room(websocketpp::connection_hdl hdl) {
    for(auto& r : rooms) {
      if(priv::has_key(r.second, hdl)) {
        return boost::optional<std::string>(r.first);
      }
    }
    return boost::optional<std::string>();
  };

  boost::optional<UserData> get_user_data(websocketpp::connection_hdl hdl) {
    boost::optional<std::string> room_name = get_room(hdl);
    if(room_name) {
      return boost::optional<UserData>(rooms[room_name.get()][hdl]);
    } else {
      return boost::optional<UserData>();
    }
  };

  void change_room(websocketpp::connection_hdl hdl, const std::string& new_room) {
    boost::optional<std::string> room_name = get_room(hdl);
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
  
  void remove_user(websocketpp::connection_hdl hdl) {
    boost::optional<std::string> room_name = get_room(hdl);
    if(room_name) {
      UserData ud = rooms[room_name.get()][hdl];
      rooms[room_name.get()].erase(hdl);

      broadcast_msg(ud.name + " left the chat.");
    }
  };

};





}



#endif /* CONTEXT_HPP */
