# nginx-like HTTP1.1 server
HTTP1.1 server written in cpp.
<!-- シールド一覧 -->
<!-- 該当するプロジェクトの中から任意のものを選ぶ-->
<p style="display: inline">
  <!-- ソースコードで使われている技術 -->
  <img src="https://img.shields.io/badge/-C++-00599C.svg?logo=c%2B%2B&style=flat">
  <!-- テストで使われている技術 -->
 <img src="https://img.shields.io/badge/-Python-F9DC3E.svg?logo=python&style=flat">
  <img src="https://img.shields.io/badge/-Shell_Script-red.svg?logo=Shell&style=flat">
  <img src="https://img.shields.io/badge/-Docker-EEE.svg?logo=docker&style=flat">
  <img src="https://img.shields.io/badge/-githubactions-black.svg?logo=github-actions&style=flat">
</p>

## Building the source
To build the server from source, follow these steps:
```sh
git clone https://github.com/YungTatyu/webserv.git && cd webserv
make
```

## Configuration
For detailed configuration, please read [config.md](https://github.com/YungTatyu/webserv/blob/main/docs/config.md) or [the Wiki page](https://github.com/YungTatyu/webserv/wiki).
You can specify the configuration file path as a parameter when running the binary:
```sh
./webserv conf/webserv.conf
```
