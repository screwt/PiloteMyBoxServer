
#include <ctime>
#include <iostream>
#include <string>
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>

using boost::asio::ip::udp;


class udp_server
{
public:
  udp_server(boost::asio::io_service& io_service)
    : socket_(io_service, udp::endpoint(udp::v4(), 8888))
  {
    start_receive();
  }

private:
  void start_receive(){
    std::cout << "receiving" << std::endl;
    socket_.async_receive_from(
			       boost::asio::buffer(recv_buffer_), remote_endpoint_,
			       boost::bind(&udp_server::handle_receive, this,
					   boost::asio::placeholders::error,
					   boost::asio::placeholders::bytes_transferred));
  }

  void handle_receive(const boost::system::error_code& error,
		      std::size_t /*bytes_transferred*/){
    std::cout << "handle receiving" << error << std::endl;
    std::cout << "UDP mess received" << std::endl;
    boost::shared_ptr<std::string> message(
					   new std::string("Hellox"));

    socket_.async_send_to(boost::asio::buffer(*message), remote_endpoint_,
			  boost::bind(&udp_server::handle_send, this, message,
				      boost::asio::placeholders::error,
				      boost::asio::placeholders::bytes_transferred));

    start_receive();
  }

  void handle_send(boost::shared_ptr<std::string> /*message*/,
		   const boost::system::error_code& /*error*/,
		   std::size_t /*bytes_transferred*/){
    std::cout << "hendle send" << std::endl;
    
  }
  
  udp::socket socket_;
  udp::endpoint remote_endpoint_;
  boost::array<char, 1> recv_buffer_;
};
