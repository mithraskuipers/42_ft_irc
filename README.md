# FT_IRC

<p align="center">  <img src="assets/welcome.png" width="55%"> </p>
---

## :school_satchel: About this project

The goal of this project is to replicate the functionality of an IRC server.

###  _ :crystal_ball: Definition

> "IRC (Internet Relay Chat) is a **protocol for real-time text messaging between internet-connected computers created in 1988**. It is mainly used for group discussion in chat rooms called “channels” although it supports private messages between two users, data transfer, and various server-side and client-side commands. As of April 2011, the top 100 IRC networks served over 500,000 users at a time on hundreds of thousands of channels." ([source](https://www.radware.com/security/ddos-knowledge-center/ddospedia/irc-internet-relay-chat/))

<img src="assets/irc-shema.png">

### _ :floppy_disk: Subject requirements

1. Code the IRC server in C++98.
2. Your binary should look like this: `./ircserv <port> <password>`.
3. You should use `poll()` and only **once**.
4. The client/server communication will be TCP/IP (v4 or v6).
5. The following features are mandatory:
   - i. You should be able to register, i.e., to define a nickname, a username.
   - ii. You should be able to join a channel, send, or receive private messages.
   - iii. Every message sent by a client in a channel must be received by all of the clients who have joined this channel.
   - iv. You should have normal users and operators, and implement the operators' specific commands.
6. No leaks (even still reachables) and handle the signals (partial messages with `CTRL+D`, suspended process with `CTRL+Z`, and obviously SIGINT (`CTRL+C`)).
7. (Bonuses) Implement a bot + file transfer.

---

### :high_brightness: Commands that we implemented

| Command | Description |
| :-----------: | :----------- |
| Invite | The `INVITE` command invites a user to a channel. |
| Join | The `JOIN` command indicates that the client wants to join the given channel(s), each channel using the given key for it. |
| Kick | The `KICK` command requests the forced removal of a user from a channel. |
| Kill | The `KILL` command closes the connection between a given client and the server they are connected to. `KILL` is a privileged command and is available only to IRC Operators. |
| List | The `LIST` command gets a list of channels along with some information about each channel. |
| Mode | The `MODE` command sets or removes options (or modes) from a given target. User modes: i, o. Channel modes: b, k, m, o, p, s, t, v. |
| Motd | The `MOTD` command gets the “Message of the Day” of the given server. |
| Names | The `NAMES` command views the nicknames joined to a channel and their channel membership prefixes. |
| Nick | The `NICK` command gives the client a nickname or changes the previous one. |
| Notice | The `NOTICE` command sends notices between users and to channels. Automatic replies must never be sent in response to a `NOTICE` message.  |
| Oper | The `OPER` command is used by a normal user to obtain IRC operator privileges.  |
| Part | The `PART` command removes the client from the given channel(s). |
| Pass | The `PASS` command sets a ‘connection password’. If set, the password must be set before any attempt to register the connection is made. |
| Ping | The `PING` command is sent by either clients or servers to check the other side of the connection is still connected and/or to check for connection latency, at the application layer. |
| Privmsg | The `PRIVMSG` command sends private messages between users and to channels. |
| Quit | The `QUIT` command terminates a client’s connection to the server. The server acknowledges this by replying with an `ERROR` message and closing the connection to the client. |
| Topic | The `TOPIC` command changes or views the topic of the given channel. |
| User | The `USER` command specifies the username and realname of a new user. |

---

## :gift: Useful Resources

### _ :books: Helpful links

- **To understand what exactly is an IRC and begin the project**: [Chirc](http://chi.cs.uchicago.edu/chirc/irc.html) (An IRC subject from a Chicago uni) and the subsequent pages.
- **Regarding the client/server connection**: [Beej's Guide to network programming](https://beej.us/guide/bgnet/pdf/bgnet_a4_c_1.pdf). This is super helpful for understanding sockets, the `poll()` function, system calls, and their order.
- **A link to the IRC Client Protocol with good formatting**: [Modern IRC Client Protocol](https://modern.ircdocs.horse/). This provides descriptions of all the commands with correct syntax and expected Numerical Replies.

<p align="center"> <img src="assets/network.png"> </p>

### _ :mailbox: Tips

- **A suggested project roadmap**: 
  1. Start by establishing a working client/server socket connection.
  2. Build on that: add signals handling (`CTRL+C`, `CTRL+D`, `CTRL+Z`).
  3. Then begin the client registration system (`NICK`, `USER`, `PASS`), 
  4. Implement server commands (`PING`, `OPER`, `KILL`, etc.).
  5. Finally, add channel operations commands (`JOIN`, `PART`, `INVITE`, `KICK`, `PRIVMSG`, `NOTICE`, etc.).
