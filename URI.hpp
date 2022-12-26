/**
 * This is a small library that aims to implement
 * RFC3986 (https://datatracker.ietf.org/doc/html/rfc3986).
 *
 * This will be std::string_view'ed as soon as MacOS supports
 * c++20 and the constructor which can deal with two iterators:
 * https://en.cppreference.com/w/cpp/string/basic_string_view/basic_string_view
 */

#pragma once
#include <algorithm>
#include <optional>
#include <string>
#include <utility>

class URI {
public:
  enum class URIParsingResult {
    success,        // no error
    empty_protocol, // protocol suffix was implied but empty
    empty_uri       // uri is empty
  };
  struct URIAuthority {
    std::string host{};
    std::string username{};
    std::string password{}; // deprecated in rfc3986
    std::optional<uint16_t> port{};
  };

private:
  std::optional<URIAuthority> _authority;
  std::string _protocol{};
  std::string _query{};
  std::string _uri{};
  URIParsingResult _result = URIParsingResult::empty_uri;

public:
  explicit URI(std::string uri) : _uri(std::move(uri)) { _result = parse(); }

  [[nodiscard]] std::optional<std::string> get_protocol() const {
    if (!_protocol.empty()) {
      return _protocol;
    }
    return std::nullopt;
  }
  [[nodiscard]] std::optional<URIAuthority> get_authority() const {
    return _authority;
  }
  [[nodiscard]] std::optional<uint16_t> get_port() const {
    if (_authority) {
      return _authority->port;
    }
    return std::nullopt;
  }
  [[nodiscard]] std::optional<std::string> get_host() const {
    if (_authority) {
      if (!_authority->host.empty()){
        return _authority->host;
      }
    }
    return std::nullopt;
  }
  [[nodiscard]] std::optional<std::string> get_username() const {
    if (_authority) {
      if (!_authority->username.empty()){
        return _authority->username;
      }
    }
    return std::nullopt;
  }
  [[nodiscard]] std::optional<std::string> get_password() const {
    if (_authority) {
      if (!_authority->password.empty()){
        return _authority->password;
      }
    }
    return std::nullopt;
  }
  [[nodiscard]] std::optional<std::string> get_query() const {
    if (!_query.empty()){
      return _query;
    }
    return std::nullopt;
  }
  [[nodiscard]] URIParsingResult get_result() const { return _result; }

private:
  URIParsingResult parse() {

    if (_uri.empty()) {
      return URIParsingResult::empty_uri;
    }

    const std::string prot_seg_delim("://");
    const std::string userinfo_delim("@");
    const std::string pass_port_delim(":");
    const std::string slash_delim("/");
    // find protocol segment terminator
    auto proto_term_it = std::search(
        _uri.begin(), _uri.end(), prot_seg_delim.begin(), prot_seg_delim.end());
    if (proto_term_it != _uri.end()) {
      if (proto_term_it == _uri.begin()) {
        return URIParsingResult::empty_protocol;
      }
      _protocol = std::string(_uri.begin(), proto_term_it);
    }
    URIAuthority authority;
    auto userinfo_term_it = std::search(
        _uri.begin(), _uri.end(), userinfo_delim.begin(), userinfo_delim.end());
    if (userinfo_term_it != _uri.end()) {
      std::string userinfo =
          std::string(proto_term_it + prot_seg_delim.size(), userinfo_term_it);
      auto userinfo_term_pass =
          std::search(userinfo.begin(), userinfo.end(), pass_port_delim.begin(),
                      pass_port_delim.end());
      if (userinfo_term_pass != _uri.end()) {
        // we have a password element, deprecated but oh well
        authority.username = std::string(userinfo.begin(), userinfo_term_pass);
        authority.password = std::string(
            userinfo_term_pass + pass_port_delim.size(), userinfo.end());
      } else {
        // only username
        authority.username = userinfo;
      }
    }
    std::string::iterator it_host_begin{};
    std::string::iterator it_host_end{};
    if (userinfo_term_it != _uri.end()) {
      // we had a userinfo part, we start from there
      it_host_begin = userinfo_term_it + userinfo_delim.size();
    } else {
      // no userinfo, start from either protocol start or start of string
      if (proto_term_it != _uri.end()){
        it_host_begin = proto_term_it + prot_seg_delim.size();
      } else {
        it_host_begin = _uri.begin();
      }
    }
    // now we figure out the end
    auto slash_end_it = std::search(it_host_begin, _uri.end(),
                                    slash_delim.begin(), slash_delim.end());
    if (slash_end_it != _uri.end()) {
      // we have an end slash
      it_host_end = slash_end_it;
    } else {
      // we don't have an end slash - we can assume the uri ends here, since
      // that's the only legal character that can follow
      it_host_end = _uri.end();
    }
    // we have complete host part - check if we need to extract port too
    auto port_end_it =
        std::search(it_host_begin, it_host_end, pass_port_delim.begin(),
                    pass_port_delim.end());
    if (port_end_it != _uri.end()) {
      // we have a port
      authority.host = std::string(it_host_begin, port_end_it);
      // now, either the string ends, or we have a slash
      std::string port =
          std::string(port_end_it + pass_port_delim.size(), it_host_end);
      if (!port.empty()) {
        // check if port is numeric
        if (std::find_if(port.begin(), port.end(), [](uint8_t c) {
              return !std::isdigit(c);
            }) == port.end()) {
          authority.port = std::stol(std::string(port));
        }
      }
    } else {
      // only host
      authority.host = std::string(it_host_begin, it_host_end);
    }
    _authority = authority;
    // was that all or is there still a query?
    if (it_host_end != _uri.end()) {
      // query until the end
      _query = std::string(it_host_end, _uri.end());
    }
    return URIParsingResult::success;
  }
};