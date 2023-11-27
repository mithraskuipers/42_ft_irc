#ifndef COMMANDS_HPP
#define COMMANDS_HPP

#include <iostream>

// COMMANDS
// - USER-chan
// QUIT
// NOTICE
// - OPERATOR-san
// KICK
// INVITE
// TOPIC
// MODE
// i: set/remove invite-only channel
// t: set/remove restriction TOPIC operators
// k: set/remove channel key
// o: set/remove operator
// l: set/remove user limit to channel

class Commands
{
   public:
   Commands();

   // tmp check
   void printCommandsPrivates(); // REMOVE LATER

   // tmp checkfunction
   // bool checkIfOperator(int userFD);
   // void runCommand(int userFD, std::string cmdToFind, std::vector<std::string> cmdArgs);

   // all users
   // void cmdQuit();
   // void cmdNotice();

   // operator-only
   // void cmdKick();
   // void cmdInvite();
   // void cmdTopic();
   // void cmdMode();

   // canon
   // Commands(const Commands &Commands);
   // Commands &operator=(const Commands &Commands);
   // ~Commands();
   
   private:
   // std::vector<int> _operatorFDs;
};

#endif