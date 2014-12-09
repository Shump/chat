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

    std::string create_system_msg(const std::string& text) {
      typedef picojson::object Object;
      typedef picojson::value Value;
      Object obj;
      obj["type"] = Value("system");
      obj["text"] = Value(text);
      
      return Value(obj).serialize();
    };
  }

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

  void broadcast_system_msg(const std::string& msg) {
    for(auto r : rooms) {
      for(auto u : r.second) {
        server.send(u.first, priv::create_system_msg(msg), websocketpp::frame::opcode::text);
      }
    }
  };

  void roomcast_system_msg(const std::string& room, const std::string& msg) {
    if(priv::has_key(rooms, room)) {
      auto r = rooms[room];
      for(auto u : r) {
        server.send(u.first, priv::create_system_msg(msg), websocketpp::frame::opcode::text);
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
  
  void remove_user(websocketpp::connection_hdl hdl) {
    boost::optional<std::string> room_name = get_room(hdl);
    if(room_name) {
      rooms[room_name.get()].erase(hdl);
    }
  };

};





}



#endif /* CONTEXT_HPP */
