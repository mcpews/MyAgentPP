#pragma once
#include <string>
# if defined __cplusplus ? __GNUC_PREREQ (2, 6) : __GNUC_PREREQ (2, 4)
#   define __ASSERT_FUNCTION	__PRETTY_FUNCTION__
# else
#  if defined __STDC_VERSION__ && __STDC_VERSION__ >= 199901L
#   define __ASSERT_FUNCTION	__func__
#  else
#   define __ASSERT_FUNCTION	((const char *) 0)
#  endif
# endif

#define __FILENAME__ (strrchr(__FILE__, '/') ? (strrchr(__FILE__, '/') + 1):__FILE__)

#  define assert_n(expr,session)							\
     (static_cast <bool> (expr)						\
      ? void (0)							\
      : session->sendText(std::string(__FILENAME__)+std::string(":")+std::to_string(__LINE__)+std::string(":")+std::string(__ASSERT_FUNCTION)+ std::string(": Assertion of \"" #expr "\" failed."),true));\
     (static_cast <bool> (expr)						\
      ? void (0)							\
      : throw 1.23)
