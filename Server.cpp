#include "Server.hpp"


Server::Server(boost::asio::io_service& io_service)
    : _acceptor(io_service), _socket(io_service) {
    boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), PORT);
    _acceptor.open(endpoint.protocol());
    _acceptor.bind(endpoint);
    _acceptor.listen();
    acceptConnections();
}

Server::~Server()
{
    _acceptor.close();
    for (auto& client : _clients)
    {
        if (client.second->is_open())
        {
            client.second->close();
        }
    }
    _clients.clear();
}


void Server::acceptConnections()
{
    auto new_socketptr = std::make_shared<boost::asio::ip::tcp::socket>(_acceptor.get_executor()); // Create a shared pointer to a new socket
    _acceptor.async_accept(*new_socketptr, [this, new_socketptr](const boost::system::error_code& error) {
        if (!error) {
            std::cout << "New connection accepted." << std::endl;
            int socket_fd = new_socketptr->native_handle();
            _clients[socket_fd] = new_socketptr;
            startReading(new_socketptr);
            acceptConnections(); // Prepare for next connection
        }
        else {
            throw std::runtime_error("Error accepting connection: " + error.message());
        }
        });
}

void Server::startReading(std::shared_ptr<boost::asio::ip::tcp::socket> socket)
{
    auto message_size_ptr = std::make_shared<uint32_t>();
    socket->async_receive(boost::asio::buffer(message_size_ptr.get(), sizeof(*message_size_ptr)), [this, socket, message_size_ptr](const boost::system::error_code& error, size_t bytes)
    {
            if (!error)
            {
                _bufferptr->clear();
                _bufferptr->resize(*message_size_ptr);
                //auto bufferptr = std::make_shared<std::vector<char>>(*message_size_ptr);
                socket->async_receive(boost::asio::buffer(*_bufferptr), [this, socket](const boost::system::error_code& error, size_t bytes)
                    {
                        handleReadCallBack(error, bytes, socket, _bufferptr);
                    });
                
            }
            else
            {
                int socket_fd = socket->native_handle();
                _clients.erase(socket_fd);
                socket->close();
                std::cout << socket_fd << " is Disconnected from the chat\n";
            }
    });

}

void Server::handleReadCallBack(const boost::system::error_code& error, std::size_t bytes, std::shared_ptr<boost::asio::ip::tcp::socket> socket, std::shared_ptr<std::vector<char>> buffer)
{
    if (!error)
    {
        if (bytes > 0)
        {
            std::string message(buffer->data(), bytes);
            broadcast(message, socket);
        }
        startReading(socket);
    }
}


void Server::broadcast(const std::string& message, std::shared_ptr<boost::asio::ip::tcp::socket> sender_socket)
{
    auto message_ptr = std::make_shared<std::string>(message);
    for (auto& client : _clients)
    {
        if (client.second != sender_socket)
        {
            uint32_t message_size = message.size(); //send the message size 
            boost::asio::async_write(*client.second, boost::asio::buffer(&message_size, sizeof(message_size)), [this, client, message_ptr](const boost::system::error_code& error, std::size_t bytes)
            {
                    if (!error)
                    {
                        boost::asio::async_write(*client.second, boost::asio::buffer(*message_ptr), [this, message_ptr](const boost::system::error_code& error, size_t bytes)
                        {
                                handleWriteCallBack(error, bytes);
                        });
                    }
            });
        }
    }
}


void Server::handleWriteCallBack(const boost::system::error_code& error, std::size_t bytes_transferred)
{
    if (error)
    {
        throw std::runtime_error("Error writing data: " + error.message());
    }
}


