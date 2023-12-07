#ifndef USER_HPP
#define USER_HPP

#include <iostream>
#include <vector>

class User
{
   public:
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

   void addInvitation(std::string channelName);
   void addJoinedChannel(std::string channelName);

   bool isInvited(std::string channelName);
   bool isInChannel(std::string channelName);

   void setUserFD(int fd);
   void setNickName(std::string nickName);
   void setUserName(std::string userName);
   void setHostName(std::string hostName);
   void setRealName(std::vector<std::string> splitArgs);
   void setIsOperator(bool isOperator);

   int         getUserFD();
   std::string getNickName();
   std::string getUserName();
   std::string getHostName();
   std::string getRealName();
   bool        getIsOperator();
   std::string getSource();

   private:
   int         _userFD;
   std::string _nickName;
   std::string _userName;
   std::string _hostName;
   std::string _realName;
   bool        _isOperator;
   std::vector <std::string> _channelInvitations;
   std::vector <std::string> _joinedChannels;
};

#endif
