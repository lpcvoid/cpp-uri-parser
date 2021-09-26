#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "doctest/doctest/doctest.h"
#include "URI.hpp"

TEST_CASE("URI : complete with user, pass, port and query"){
    URI uri("https://user:pass@example.com:9000/query/query");
    CHECK_EQ(uri.get_result(), URI::URIParsingResult::success);
    CHECK_EQ(uri.get_protocol(), "https");
    CHECK_NE(uri.get_authority(), std::nullopt);
    CHECK_NE(uri.get_port(), std::nullopt);
    CHECK_EQ(uri.get_port().value(), 9000);
    CHECK_NE(uri.get_username(), std::nullopt);
    CHECK_EQ(uri.get_username().value(), "user");
    CHECK_NE(uri.get_password(), std::nullopt);
    CHECK_EQ(uri.get_password().value(), "pass");
    CHECK_NE(uri.get_host(), std::nullopt);
    CHECK_EQ(uri.get_host().value(), "example.com");
    CHECK_EQ(uri.get_query(), "/query/query");
}

TEST_CASE("URI : host, port, query"){
    URI uri("https://example.com:9000/query/query");
    CHECK_EQ(uri.get_result(), URI::URIParsingResult::success);
    CHECK_EQ(uri.get_protocol(), "https");
    CHECK_NE(uri.get_authority(), std::nullopt);
    CHECK_NE(uri.get_port(), std::nullopt);
    CHECK_EQ(uri.get_port().value(), 9000);
    CHECK_NE(uri.get_host(), std::nullopt);
    CHECK_EQ(uri.get_host().value(), "example.com");
    CHECK_EQ(uri.get_query(), "/query/query");
}

TEST_CASE("URI : protocol and host only"){
    URI uri("tcp://example.com");
    CHECK_EQ(uri.get_result(), URI::URIParsingResult::success);
    CHECK_EQ(uri.get_protocol(), "tcp");
    CHECK_NE(uri.get_authority(), std::nullopt);
    CHECK_EQ(uri.get_port(), std::nullopt);
    CHECK_EQ(uri.get_username(), std::nullopt);
    CHECK_EQ(uri.get_password(), std::nullopt);
    CHECK_NE(uri.get_host(), std::nullopt);
    CHECK_EQ(uri.get_host().value(), "example.com");
    CHECK_EQ(uri.get_query(), std::nullopt);
}

TEST_CASE("URI : empty protocol"){
    URI uri("://example.com");
    CHECK_EQ(uri.get_result(), URI::URIParsingResult::empty_protocol);
}