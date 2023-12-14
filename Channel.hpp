#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <iostream>
#include <vector>

class Channel
{
   public:
   Channel(std::string channelName);

   //tmp check
   void printChannelPrivates(); // REMOVE LATER

   void msgAllInChannel(std::string msg);

   void addToChannel(int userFD);
   void rmvFromChannel(int userFD);
   void addToBanned(int userFD);
   void rmvFromBanned(int userFD);

   bool isInChannel(int userFD);
   bool isBanned(int userFD);

   std::string getTopic();
   std::string getChannelName();
   std::string getActiveModes();
   bool        getIsInviteOnly();
   std::vector<int> getJoinedUserFDs();
   std::vector<int> getBannedUserFDs();

   void setTopic(std::string topic);
   void setChannelName(std::string channelName);
   void setActiveModes(std::string modes);
   void setIsInviteOnly(bool inviteBool);
   
   // canon
   // Channel();
   // Channel(const Channel &Channel);
   // Channel &operator=(const Channel &Channel);
   // ~Channel();

   private:
   std::string _topic;
   std::string _channelName;
   std::string _activeModes;
   int         _intarr[4];
   bool        _isInviteOnly;
   std::vector<int> _joinedUserFDs;
   std::vector<int> _bannedUserFDs;
};

#endif