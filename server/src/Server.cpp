#include "Server.hpp"
#include <memory>

uint16_t PORT = 9999;

using namespace boost::asio;

Server::Server()
    : ioContextM(io_context()),
      acceptorM(ip::tcp::acceptor(ioContextM, ip::tcp::endpoint(ip::tcp::v4(), PORT))),
      messagesM(MessageQueue<Message>()),
      connectionsM(std::vector<std::unique_ptr<Connection>>()),
      workGuardM(boost::asio::make_work_guard(ioContextM))
{
    ioThreadM = std::thread([this]()
                            { ioContextM.run(); });
    acceptThreadM = std::thread([this]()
                                { acceptConnection(); });
};

void Server::acceptConnection()
{
    ip::tcp::socket socket(ioContextM);
    acceptorM.accept(socket);
    std::cout << "Accepted connection from " << socket.remote_endpoint().address().to_string()
              << ":" << socket.remote_endpoint().port() << std::endl;

    std::unique_ptr<Connection> connection = std::make_unique<Connection>(
        Connection::owner::server, ioContextM, std::move(socket), messagesM);
    connection->connectToClient();
    connectionsM.push_back(std::move(connection));
    acceptConnection();
}

void Server::processMessages()
{
    while (true)
    {
        if (!messagesM.empty())
        {
            Message msg = messagesM.pop_front();
            std::cout << msg.getBody() << std::endl;
        }
        else
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
    }
}