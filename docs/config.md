# .config仕様書

## 言葉の定義
- context
  - 基本的にcontextは`{}`で囲まれる
  - main-contextのみ`{}`で囲まれない
  - contextのレベル: 高い順
    - main
    - http: **main-context** に一つだけ存在する
    - events: **main-context** に一つだけ存在する
    - server: **http-context** に複数存在可
    - location: **server-context** に複数存在可
    - limit_except: **location-context** に複数存在可
- directive
  - contextの中に表記される
  - セミコロン`;`で終わる
  - 同じdirectiveは複数存在可
 
## context
- contextは`{}`で囲まれる
```
# main-context
http {
  # http-context
  server {
    # server-context
    location / { 
      # location-context
      limit_except GET {
        # limit_except-context
       }
     }
  server {
    # server-context
    location / { 
      # location-context
      limit_except PUT {
        # limit_except-context
       }
     }
  }
  server {
  }
}

events {
  # events-context
}
```

## directive
- error_log
  - syntax: file [level]
  - default: error_log logs/error.log error;
  - context: **main**, **http**, **server**, **location**
  - 重複可否: yes
    - 重複する場合、より低いレベルのcontextがerror_logの対象となる
    - 同じcontextレベルで複数ある場合、それら全てにログが書き込まれる
  - 出力レベル: 指定されたレベル以上を出力（以下レベル低い順）
    - debug
    - info
    - notice
    - warn
    - error
    - crit
    - alert
    - emerg 

```
error_log	/Users/tatyu/Documents/nginx/logs/error.log emerg;
http {
  server {
   location / {
      error_log	/Users/tatyu/Documents/nginx/logs/index_error.log;
    }
  location /styles/ {
      error_log	/Users/tatyu/Documents/nginx/logs/styles_error.log;
  }
  server {
    error_log	/Users/tatyu/Documents/nginx/logs/index_error.log;
  }
}
```

- worker_connections
  - syntax:  positive-number
  - default: 512
  - context: **events**
  - 重複可否: no

```
events {
  worker_connections 1000;
}
```

- use
  - syntax: method
  - default: -
  - context: **events**
  - 重複可否: no

```
events {
  use epoll;
}
```
```
events {
  use kqueue;
}
```

- listen
  - syntax: address | port, address:port
  - default: 127.0.0.1:80, 127.0.0.1:8000
  - context: **server**
  - 重複可否: yes ただし同じcontext内には一つのみ
 
```
http {
  server {
     listen 8080;
     listen 80;
     listen [::]:10;
   }
}
```

- server_name
  - syntax: name
  - default: ""
  - context: **server**
  - 重複可否: yes
    - 重複する場合の挙動は不明
```
http {
  server {
    server_name www.tachu.com tachu.com;
  }
}
```

- send_timeout
  - syntax: positive-number[ ms / s / m / h ]
  - default: 60s
  - context: **http**, **server**, **location**
  - 重複可否: no

```
http {
  send_timeout 1000s;
}
```

- receive_timeout
  - syntax: positive-number[ ms / s / m / h ]
  - default: 60s
  - context: **http**, **server**, **location**
  - 重複可否: no

```
http {
  receive_timeout 1000s;
}
```

- keepalive_timeout
  - syntax: positive-number[ ms / s / m / h ]
  - default: 60s
  - context: **http**, **server**, **location**
  - 重複可否: yes 同context内での重複はno

```
http {
  keepalive_timeout 2m;
}
```

- error_page
  - syntax: code uri
  - default: -
  - context: **http**, **server**, **location**
  - 重複可否: yes
    - 同じcontextレベルで重複した場合、先に設定してものが適応される
    - contextレベルが低い設定が優先される

```
http {
  root /Users/me/server;
  error_page 403 404 /error/error.html;
  server / {}
  server /user {
    error_page 403 404 /error/error2.html;  
  }
}
```

- root
  - syntax: path
  - default: html
  - context: **http**, **server**, **location**
  - 重複可否: yes 同context内での重複はno
    - contextレベルが低い設定が優先される

```
http {
  root /Users/name/server;
  
  location / {
    root /Users/name/server/profile;
  }

}
```

- client_max_body_size
  - syntax: size[k / K / m / M]
  - default: 1m
  - context: **http**, **server**, **location**
  - 重複可否: yes 同context内での重複はno
    - contextレベルが低い設定が優先される

```
http {
  client_max_body_size 2m;
}
```

- allow
  - syntax: address | all
  - default: -
  - context: **http**, **server**, **location**, **limit_except**
  - 重複可否: yes

- deny
  - syntax: address | all
  - default: -
  - context: **http**, **server**, **location**, **limit_except**
  - 重複可否: yes

```
http {
  allow 192.168.1.1/24;
  deny all;
}
```

- autoindex
  - syntax: on | off
  - default: off
  - context: **http**, **server**, **location**
  - 重複可否: yes 同context内での重複はno

```
http {
  server {
    autoindex on;
  }
}
```

- index
  - syntax: file
  - default: index.html
  - context: **http**, **server**, **location**
  - 重複可否: yes
    - 重複している場合、先に指定されたものが優先される

```
http {
  server {
    index index.html;
  }
}
```

- return
  - syntax: code | code URL | URL
  - default: -
  - context: **location**
  - 重複可否: yes
    - 重複している場合、先に指定されたものが優先される

```
http {
  server {
    location /redirect/ {
      return 301 https://google.com;
    }
  }
}
```
