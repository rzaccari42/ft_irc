This project has been created as part of the 42 curriculum by razaccar, nvignal

# FT_IRC

## Description
ft_irc is an implementation of an **Internet Relay Chat (IRC) server** written in **C++98**.

The goal of this project is to gain a deeper understanding of **network programming** and **communication protocols** by implementing a real IRC server that can interact with an existing IRC client reference.  
The server handles multiple clients simultaneously using **non-blocking I/O** and a single event loop based on `poll()` following the Reactor design pattern.

This IRC server supports client authentication, nickname and username registration, channel management, private messaging, and operator-specific channel commands.


## Instruction
### Build
Compile and the project using the provided `Makefile`:
```bash
make            # Compile ./ircserv executable
make clean      # Remove compilation files
make fclean     # Remove compilation files and executable
make re         # Recompile ./ircserv executable
```

### Usage
**Running server**
`./ircserv <port> <password>`
- <port>: The port number on which the server will listen for incoming connections.
- <password>: The connection password required by clients to authenticate.

**Connecting to server**
- netcat network utility
```bash
nc <IP> <port> 
```
- irssi client (reference)
```
irssi
#inside client
/connect <IP> <port>
```

## Resources

### References 
[RFC - Internet Relay Chat Protocol](https://www.rfc-editor.org/rfc/rfc1459#section-4.2.3)
[Wikipedia - Reactor Pattern](https://en.wikipedia.org/wiki/Reactor_pattern)
[PDF - Reactor event handling pattern](https://en.wikipedia.org/wiki/Reactor_pattern)
[Irssi - IRC client reference](https://irssi.org/)

### AI usage
