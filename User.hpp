#ifndef USER_HPP
#define USER_HPP

#include <iostream>
#include <vector>

class User
{
   public:
   User(int fd);

   void addInvitation(std::string channelName);
   bool isInvited(std::string channelName);

   void setUserFD(int fd);
   void setNickName(std::string nickName);
   void setUserName(std::string userName);
   void setHostName(std::string hostName);
   void setRealName(std::string realName);
   void setPassword(std::string password);

   int			getUserFD();
   std::string	getNickName();
   std::string	getUserName();
   std::string	getHostName();
   std::string	getRealName();
   std::string	getPassword();
   std::string	getSource();

   private:
   int			_userFD;
   std::string	_nickName;
   std::string	_userName;
   std::string	_hostName;
   std::string	_realName;
   std::string	_password;
   std::vector	<std::string> _channelInvitations;
};

#endif
