#include <iostream>
#include <boost/asio.hpp>

using namespace boost::asio;

uint16_t PORT = 9999;

int main() {
    io_service ioService;
    ip::tcp::acceptor acceptor(ioService, ip::tcp::endpoint(ip::tcp::v4(), PORT)); 

    std::cout << "Server started on port " << PORT << std::endl;

    while (true) {
        try {
            ip::tcp::socket socket(ioService);
            acceptor.accept(socket);

            std::cout << "Accepted connection from " << socket.remote_endpoint().address().to_string()
                      << ":" << socket.remote_endpoint().port() << std::endl;

            char data[1024];
            size_t length = socket.read_some(buffer(data));

            std::cout << "Received " << length << " bytes from client.\nClient said: " << data << std::endl;            
            socket.write_some(buffer(data, length)); // Echo the received data back to the client
        } catch (std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }

    return 0;
}
