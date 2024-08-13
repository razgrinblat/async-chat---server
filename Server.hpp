#pragma once
#include <boost/asio.hpp>
#include <iostream>
#include <memory>
#include <array>
#include <vector>
#include <unordered_map>


class Server
{

private:
	static constexpr auto PORT = 8080;
	boost::asio::io_service _io_service;
	boost::asio::ip::tcp::acceptor _acceptor;
	boost::asio::ip::tcp::socket _socket;
	std::unordered_map<int, std::shared_ptr<boost::asio::ip::tcp::socket>> _clients;
	std::shared_ptr<std::vector<char>> _bufferptr = std::make_shared<std::vector<char>>();

	/**
	* @brief Asynchronously accepts incoming client connections.
	* @throws std::runtime_error if there is an error accepting a connection.
	*/
	void acceptConnections();
	/**
	* @brief Begins asynchronously reading messages from a connected client.
	* @param socket A shared pointer to the client's socket.
	*/
	void startReading(std::shared_ptr<boost::asio::ip::tcp::socket> socket);
	/**
	* @brief Handles the received message from a client.
	* @param error The error code from the asynchronous read operation.
	* @param bytes The number of bytes transferred.
	* @param socket A shared pointer to the client's socket.
	* @param buffer A shared pointer to the buffer containing the received message.
	*/
	void handleReadCallBack(const boost::system::error_code& error, std::size_t bytes_transferred,
		std::shared_ptr<boost::asio::ip::tcp::socket> socketptr, std::shared_ptr<std::vector<char>> bufferptr);
	/**
	* @brief Broadcasts a message to all connected clients except the sender.
	* @param message The message string to broadcast.
	* @param sender_socket A shared pointer to the sender's socket.
	*/
	void broadcast(const std::string& message, std::shared_ptr<boost::asio::ip::tcp::socket> socketptr);
	/**
	* @brief Handles the callback after a message is written to a client socket.
	* @param error The error code from the asynchronous write operation.
	* @throws std::runtime_error if there is an error writing data.
	*/
	void handleWriteCallBack(const boost::system::error_code& error);
	

public:
	Server(boost::asio::io_service& io_service);
	~Server();


};


