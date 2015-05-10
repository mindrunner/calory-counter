# calory-counter by Lukas Elsner

The task was to develop a client/server application for professional dietitians
using C and BSD sockets. The server should load a csv-file with food and their
nutrition values. Furthermore, it must handle multiple client sessions at once.
Every client can query food by search terms and add new food to the list. This
document covers the implementation report, as well as occurred problems while
implementing the application.

Compile and run:

1) "cmake . && make"        - to compile calory-counter with CMake
2) cd bin/                  - to change into output directory
3) ./diet-server -h         - for help
4) ./diet-client -h         - for help
4) ./diet-server            - for starting server with default values
5) ./diet-client            - for starting client with default values


Run 'doxygen doxy.gen' to regenerate source code documentation.
