# betaProxy
A proxy server with epoll, simple and lightweight. Support Linux Only.

1) Help you fight against network attcks.

2) Help you balance the network load.

# How to Use
make the source code, set the config.ini like below, then run the proxy

#listen ip and port

listen = 127.0.0.1:8888

#destination server to connect, start with 'des' mark

des_1	= 127.0.0.1:9001

des_2	= 127.0.0.1:9002


# More
You can modify the codes to suit yourself 

You can modify the code in main.cpp file.

