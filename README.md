# cpp-uri-parser
This is a small, modern header only URI parsing library for C++. 

### Why?

My main motivation for writing this is that all the other libs I found
where part of large frameworks - which is not a bad thing, but I didn't 
want to include a lot of other things in my project.

### Tests
There are tests which attempt to cover edge cases. You can run them by 
cloning the repo (with submodules!), and then running:

    cmake -B build
    cmake --build build

This will build the tests in a subdirectory called `build`. 
You can then execute `cpp_uri_parser_tests`.
