#include "Server.hpp"


int main()
{
    try
    {
        boost::asio::io_service io_service;
        Server server(io_service);
        io_service.run(); // Start the IO service 
    }
    catch (const std::exception& e)
    {
        std::cout << "Exeption: " << e.what();
    }
    return 0;
}