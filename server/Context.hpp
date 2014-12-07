#ifndef CONTEXT_HPP
#define CONTEXT_HPP 

#include <map>
#include <string>

#include <iostream>

#include "websocketpp/config/asio_no_tls.hpp"
#include "websocketpp/server.hpp"

#include "picojson.h"

#include "Room.hpp"

namespace chat {

  namespace priv {
    template<typename K, typename V>
    bool has_key(std::map<K, V> m, K k) {
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

struct Context {

  typedef websocketpp::server<websocketpp::config::asio> ServerType;
  ServerType server;

  typedef std::map<std::string, Room> RoomsType;
  RoomsType rooms;

  void broadcast_system_msg(const std::string& msg) {
    for(auto r : rooms) {
      for(auto u : r.second.users) {
        server.send(u.connection, priv::create_system_msg(msg), websocketpp::frame::opcode::text);
      }
    }
  };

  void roomcast_system_msg(const std::string& room, const std::string& msg) {
    if(priv::has_key(rooms, room)) {

    } else {
      std::cerr << "roomcast error: " + room + " does not exist!" << std::endl;
    }
  };
  

};





}



#endif /* CONTEXT_HPP */
