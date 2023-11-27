#ifndef USER_HPP
#define USER_HPP

#include <iostream>

class User
{
   public:
   // User(int fd, std::string userName, std::string nickName, std::string realName, bool isOperator);
   User(int fd);
   
   // tmp check
   void  printUserPrivates(); // REMOVE LATER

   // bool findChannel(std::string channelToFind);
   // bool findChannelInvitation(std::string channelToFind)

   // canon
   // User();
   // User(const User &User);
   // User &operator=(const User &User);
   // ~User();

   private:
   int         _userFD;
   std::string _userName;
   std::string _nickName;
   std::string _realName;
   bool        _isOperator;
   // std::vector <std::string> _channelInvitations;
   // std::vector <std::string> _joinedChannels;
};

#endif
