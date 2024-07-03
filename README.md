# nginx-like HTTP1.1 server
HTTP1.1 server written in cpp.

## Building the source
To build the server from source, follow these steps:
```sh
git clone https://github.com/YungTatyu/webserv.git && cd webserv
make
```

## Configuration
For detailed configuration, please read [config.md](https://github.com/YungTatyu/webserv/blob/main/docs/config.md) or [the Wiki page](https://github.com/YungTatyu/webserv/wiki/config-%E5%9F%BA%E6%9C%AC%E8%A8%AD%E8%A8%88).
You can specify the configuration file path as a parameter when running the binary:
```sh
./webserv conf/webserv.conf
```
