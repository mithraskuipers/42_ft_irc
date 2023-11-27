#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <iostream>

class Channel
{
   public:
   Channel(std::string channelName);

   //tmp check
   void printChannelPrivates(); // REMOVE LATER

   // bool findUser(int userFD);
   // bool findBannedUser(int userFD);

   // canon
   // Channel();
   // Channel(const Channel &Channel);
   // Channel &operator=(const Channel &Channel);
   // ~Channel();

   private:
   std::string _channelName;
   bool _isInviteOnly;
   // std::vector<int> _joinedUserFDs;
   // std::vector<int> _bannedUserFDs;
};

#endif