#define _WIN32_WINNT 0x0501 // Windows XP
#define WINVER 0x0501
//
// main.cpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2012 Benoit Juin
//
// 
// 
//

#include <cstdlib>
#include <iostream>
#include <deque>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>
#include "../include/message.hpp"
#include "../include/messageprocessor.hpp"
#include "../include/udp_server.hpp"


using boost::asio::ip::tcp;

//-- definition du type message_queue
typedef std::deque<message> message_queue;


class session{ 
private:
  boost::asio::streambuf strb_;
  tcp::socket socket_;
  enum { max_length = 1024 };
  char data_[max_length];
  message _mess;

public:
  session(boost::asio::io_service& io_service)
    : socket_(io_service)
  {
  }

  tcp::socket& socket(){
    return socket_;
  }

  void start(){
    boost::asio::async_read_until(socket_,
				  strb_,
				  "\n",
				  boost::bind(&session::handle_read, this,
					      boost::asio::placeholders::error,
					      boost::asio::placeholders::bytes_transferred));
  }

private:
  //-- call back appeler lorsque'une ligne est prete à etre lue
  void handle_read(const boost::system::error_code& error,
		   size_t bytes_transferred){
    if (!error){
      std::ostream os(&strb_);
      std::istream in(&strb_);
      std::string tmpstr;
      std::getline(in,tmpstr);
	
      //-- tentative de creation du message
      try{
	//-- initialisation du message
	_mess.init(tmpstr); 
	//-- recup du message
	os << _mess.giveoutput();
      }catch(int e){
	std::cout << "An exception occurred : message can't be created, incorrect data" << e << std::endl;
	//-- renvoi du message
	os << "An exception occurred : message can't be created, incorrect data" << e << "\n\r";
      } 

      boost::asio::async_write(socket_,
			       strb_,
			       boost::bind(&session::handle_write, this,
					   boost::asio::placeholders::error));
	
    }else{
      delete this;
    }
  }

  void handle_write(const boost::system::error_code& error){

    if (!error){
      boost::asio::async_read_until(socket_,
				    strb_,
				    "\n",
				    boost::bind(&session::handle_read, this,
						boost::asio::placeholders::error,
						boost::asio::placeholders::bytes_transferred));

      /*
	socket_.async_read_some(boost::asio::buffer(data_, max_length),
	boost::bind(&session::handle_read, this,
	boost::asio::placeholders::error,
	boost::asio::placeholders::bytes_transferred));
      */
    }else{
      delete this;
    }
  }
};

class server{
public:
  server(boost::asio::io_service& io_service, short port)
    : io_service_(io_service),
      acceptor_(io_service, tcp::endpoint(tcp::v4(), port))
  {
    start_accept();
  }

private:
  void start_accept(){
    session* new_session = new session(io_service_);
    acceptor_.async_accept(new_session->socket(),
			   boost::bind(&server::handle_accept, this, new_session,
				       boost::asio::placeholders::error));
  }

  void handle_accept(session* new_session,
		     const boost::system::error_code& error){
    if (!error){
      new_session->start();
    }else{
      delete new_session;
    }
    start_accept();
  }

  boost::asio::io_service& io_service_;
  tcp::acceptor acceptor_;
};

int main(int argc, char* argv[]){
  try{
    if (argc != 2){
      std::cerr << "Usage: async_tcp_echo_server <port>\n";
      return 1;
    }

    //-- service d'coute TCP
    boost::asio::io_service io_service;
    using namespace std; // For atoi.
    server s(io_service, atoi(argv[1]));
    std::cerr << "Launching io_service\n" ;
    boost::thread bt(boost::bind(&boost::asio::io_service::run, &io_service));

    //-- service d'écoute udp
    cout << "launchun udp serv" << endl;
    boost::asio::io_service io_service2;
    udp_server server(io_service2);
    io_service2.run();
  }
  catch (std::exception& e){
    std::cerr << "Exception: " << e.what() << "\n";
  }
  return 0;
}
