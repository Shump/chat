#ifndef MESSAGE_HPP
#define MESSAGE_HPP 

#include <string>
#include <memory>

#include "websocketpp/server.hpp"
#include "picojson.h"


template<typename server_type>
struct Message {
  enum class Type { TEXT, REG, CREATE, JOIN };

  Message(Type type, std::string user_name) : type(type), user_name(user_name) {};

  virtual ~Message() {};

  const Type type;
  const std::string user_name;

  virtual void process(server_type& s, websocketpp::connection_hdl hdl) = 0;
};

template<typename server_type>
struct TextMessage : public Message<server_type> {
  TextMessage(std::string user_name, std::string text) : 
    Message<server_type>(Message<server_type>::Type::TEXT, user_name), text(text) {};
  virtual ~TextMessage() {};

  virtual void process(server_type& s, websocketpp::connection_hdl hdl) {
  };

  const std::string text;

  static constexpr const char* type = "text";
};

template<typename server_type>
struct RegisterMessage : public Message<server_type> {
  RegisterMessage(std::string user_name) : Message<server_type>(Message<server_type>::Type::REG, user_name) {};
  virtual ~RegisterMessage() {};

  virtual void process(server_type& s, websocketpp::connection_hdl hdl) {

  };

  static constexpr const char* type = "register";

  static std::shared_ptr<Message<server_type>> create(const picojson::object& obj) {
    const std::string& name = obj.at("name").get<std::string>();

    return std::make_shared<RegisterMessage<server_type>>(name);
  };

};

template<typename server_type>
std::shared_ptr<Message<server_type>> create_message(const picojson::value& val) {
  const picojson::object& obj = val.get<picojson::object>();
  const std::string& type = obj.at("type").get<std::string>();

  if(type == RegisterMessage<server_type>::type) {
    return RegisterMessage<server_type>::create(obj);
  } else {
    throw std::runtime_error("Unsupported client message-type: " + type);
  };

};




#endif /* MESSAGE_HPP */
