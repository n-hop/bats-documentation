# Quick Start

## 1. Compile pvp_game_endpoint

To build the project, run the following commands:

```bash
cmake . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
```

## 2. Run the pvp_game_endpoint

At server side, run the following command:

```bash
# Run the pvp_game_endpoint to listen on port 10011
 ./bin/pvp_game_endpoint -p 10011
```

At client side, run the following command:

```bash
./bin/pvp_game_endpoint -c 127.0.0.1 -p 10011 -i 0.001 -w 100 -l 324
```

The parameters which are used in the above command are:

- `-c` : The IP address of the pvp_game_endpoint server.
- `-p` : The port number of the pvp_game_endpoint server.
- `-i` : The message sending rate; 0.001 means 1000 message per second.
- `-w` : The number of messages to be sent in one test.
- `-l` : The length of the message to be sent. 
  