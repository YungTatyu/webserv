#ifndef CONF_HPP
#define CONF_HPP

#if defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__) || defined(__APPLE__)
#define KQUEUE_AVAILABLE
#endif

#if defined(__linux__)
#define EPOLL_AVAILABLE
#endif

#include <string>

namespace config {

/**
 *        AAAA  number of arguments
 *      FF      command flags
 *    TT        command type, i.e. HTTP "location" or "server" command
 */

/**
 * CONF_MAIN                0x01000000   00000001 00000000 ...
 * CONF_HTTP                0x02000000   00000010 00000000 ...
 * CONF_EVENTS              0x04000000   00000100 00000000 ...
 * CONF_HTTP_SERVER         0x08000000   00001000 00000000 ...
 * CONF_HTTP_LOCATION       0x10000000   00010000 00000000 ...
 * CONF_HTTP_LIMIT_EXCEPT   0x20000000   00100000 00000000 ...
 */
enum CONTEXT {
  CONF_MAIN = 0x01000000,
  CONF_HTTP = 0x02000000,
  CONF_EVENTS = 0x04000000,
  CONF_HTTP_SERVER = 0x08000000,
  CONF_HTTP_LOCATION = 0x10000000,
  CONF_HTTP_LIMIT_EXCEPT = 0x20000000
};

/**
 * CONF_NOARGS      0x00000001   ... 00000000 00000001
 * CONF_TAKE1       0x00000002   ... 00000000 00000010
 * CONF_TAKE2       0x00000004   ... 00000000 00000100
 * CONF_TAKE3       0x00000008   ... 00000000 00001000
 * CONF_TAKE4       0x00000010   ... 00000000 00010000
 * CONF_TAKE5       0x00000020   ... 00000000 00100000
 * CONF_TAKE6       0x00000040   ... 00000000 01000000
 * CONF_TAKE7       0x00000080   ... 00000000 10000000
 * CONF_1MORE       0x00000100   ... 00000001 00000000
 * CONF_2MORE       0x00000200   ... 00000010 00000000
 */
enum ARGS {
  CONF_NOARGS = 0x00000001,  // takes no arguments
  CONF_TAKE1 = 0x00000002,   // takes one argument
  CONF_TAKE2 = 0x00000004,   // takes two arguments
  CONF_TAKE3 = 0x00000008,
  CONF_TAKE4 = 0x00000010,
  CONF_TAKE5 = 0x00000020,
  CONF_TAKE6 = 0x00000040,
  CONF_TAKE7 = 0x00000080,
  CONF_1MORE = 0x00000100,                              // takes one or more arguments
  CONF_2MORE = 0x00000200,                              // takes two or more arguments
  CONF_TAKE12 = CONF_TAKE1 | CONF_TAKE2,                // takes one or two arguments
  CONF_TAKE123 = CONF_TAKE1 | CONF_TAKE2 | CONF_TAKE3,  // takes one, two or three arguments
  CONF_TAKE1234 =
      CONF_TAKE1 | CONF_TAKE2 | CONF_TAKE3 | CONF_TAKE4,  // takes one, two, three or four arguments
  NGX_CONF_MAX_ARGS = 8
};

enum UNIQUE {
  CONF_UNIQUE = 0x00010000,
  CONF_NOT_UNIQUE = 0x00020000
};

struct Main;
Main* initConfig(const std::string& file_path);

}  // namespace config

#endif
