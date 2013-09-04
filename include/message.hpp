//
// chat_message.hpp
// ~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2012 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "./messageprocessor.hpp"

//-- constante utile pour les type de message
const int MESS_TYPE_LISTEWINDOWS = 1;
const int MESS_TYPE_STARTSWITCHWIN = 2;
const int MESS_TYPE_KILLSWITCHWIN = 3;
const int MESS_TYPE_VOLUMEUP = 4;
const int MESS_TYPE_VOLUMEDOWN = 9;
const int MESS_TYPE_SLEEP = 5;
const int MESS_TYPE_SHUTDOWN = 6;
const int MESS_TYPE_SWITCHRIGHT = 7;
const int MESS_TYPE_SWITCHLEFT = 8;
const int MESS_TYPE_LISTFILES = 10;


//-- les type d'exception
const int EXCEPTION_UNHANDELED_MSG_TYPE = 1;

class message{
public:
  enum { header_length = 4 };
  enum { max_body_length = 512 };
  messageprocessor _messproc;  

  //-- constructeur
  message()
    : _body_length(0)
  {
  }

  const char* data() const{
    return _data;
  }

  char* data(){
    return _data;
  }

  size_t length() const{
    return header_length + _body_length;
  }

  const char* body() const{
    return _data + header_length;
  }

  char* body(){
    return _data + header_length;
  }

  size_t body_length() const{
    return _body_length;
  }

  void body_length(size_t new_length){
    _body_length = new_length;
    if (_body_length > max_body_length)
      _body_length = max_body_length;
  }

  //-- decodage formatge -- gestion es exception
  void init(std::string str){
    _body = "";
    using namespace std;
    cout << "decodeing string to init message : ";
    cout << str << endl;
    const char* tmp = "00";
    //-- extraction des deux premiers charateres pas tres propore xD
    tmp = str.substr(0,2).c_str();
    _type = atoi(tmp);

    //printf("Message type: %d \n",_type);

    //-- en fonction du type on va faire differente action
    switch(_type){
      case MESS_TYPE_LISTEWINDOWS:
        cout << "List winows type" << endl;
	_body = _messproc.listwindows();
        break;
      case MESS_TYPE_STARTSWITCHWIN:
        cout << "Start switchmode" << endl;
	_messproc.enterswitchmod();
	_body = "Entered switching mode\n\r";
        break;
      case MESS_TYPE_KILLSWITCHWIN:
        cout << "Stop switchmode" << endl;
	_messproc.quitswitchmod();
	_body = "Exited switching mode\n\r";
        break;
      case MESS_TYPE_SWITCHLEFT:
        cout << "Start switchmode" << endl;
	_messproc.switchleft();
	_body = "Switched left\n\r";
        break;
      case MESS_TYPE_SWITCHRIGHT:
        cout << "Stop switchmode" << endl;
	_messproc.switchright();
	_body = "Switched right\n\r";
        break;
      case MESS_TYPE_VOLUMEUP:
        cout << "Change volume" << endl;
	_messproc.volumeup();
	_body = "Volume UP\n\r";
        break;
      case MESS_TYPE_VOLUMEDOWN:
        cout << "Change volume" << endl;
	_messproc.volumedown();
	_body = "Volume DOWN\n\r";
        break;
      case MESS_TYPE_SLEEP:
        cout << "Put to sleep" << endl;
	_messproc.puttosleep();
        break;
      case MESS_TYPE_SHUTDOWN:
        cout << "Shutdown" << endl;
	_messproc.shutdown();
        break;
    case MESS_TYPE_LISTFILES:
        if(str.size()>4){
	  _body = _messproc.listfiles(str.substr(2,str.size()-4));
	  cout << "list files: " << str  << endl;
	}else{
	  cout << "Message too short" << endl;
	  throw EXCEPTION_UNHANDELED_MSG_TYPE;
	}
	break;
      default :
	throw EXCEPTION_UNHANDELED_MSG_TYPE;
	break;
    }
  }

  //-- message renvoyé auc client
  std::string giveoutput(){
    return _body;
  }

  std::string gesbody(){
    return _body;
  }


  bool decode_header(){
    using namespace std; // For strncat and atoi.
    char header[header_length + 1] = "";
    strncat(header, _data, header_length);
    _body_length = atoi(header);
    if (_body_length > max_body_length)
      {
	_body_length = 0;
	return false;
      }
    return true;
  }

  /*
  void encode_header(){
    using namespace std; // For sprintf and memcpy.
    char header[header_length + 1] = "";
    sprintf(header, "%4d", _body_length);
    memcpy(_data, header, header_length);
  }
  */

private:
  char _data[header_length + max_body_length];
  std::string _body;
  size_t _body_length;
  int _type;
};

#endif // MESSAGE_HPP
