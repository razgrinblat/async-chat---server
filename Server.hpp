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


	void acceptConnections();
	void startReading(std::shared_ptr<boost::asio::ip::tcp::socket> socket);
	void broadcast(const std::string& message, std::shared_ptr<boost::asio::ip::tcp::socket> socketptr);
	void handleReadCallBack(const boost::system::error_code& error, std::size_t bytes_transferred,
		std::shared_ptr<boost::asio::ip::tcp::socket> socketptr, std::shared_ptr<std::vector<char>> bufferptr);
	void handleWriteCallBack(const boost::system::error_code& error, std::size_t bytes_transferred);
	

public:
	Server(boost::asio::io_service& io_service);
	~Server();


};


