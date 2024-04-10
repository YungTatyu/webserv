#! /usr/bin/python3

import os

def	printEnvVar(env_name):
      env_value = os.environ.get(env_name)
      if env_value is None:
             env_value = "not found"
      print(env_name + "=" + env_value, end="\n")

def	main():
      print("\n", end="")
      env_list = [
            "AUTH_TYPE",
            "CONTENT_LENGTH",
            "CONTENT_TYPE",
            "GATEWAY_INTERFACE",
            "PATH_INFO",
            "PATH_TRANSLATED",
            "QUERY_STRING",
            "REMOTE_ADDR",
            "REMOTE_HOST",
            "REQUEST_METHOD",
            "SCRIPT_NAME",
            "SERVER_NAME",
            "SERVER_PORT",
            "SERVER_PROTOCOL",
            "SERVER_SOFTWARE"
     ]
 
      for env in env_list:
             printEnvVar(env)

if __name__ == "__main__":
       main()