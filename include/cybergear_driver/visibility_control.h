#ifndef CYBERGEAR_DRIVER__VISIBILITY_CONTROL_H_
#define CYBERGEAR_DRIVER__VISIBILITY_CONTROL_H_

// This logic was borrowed (then namespaced) from the examples on the gcc wiki:
//     https://gcc.gnu.org/wiki/Visibility

#if defined _WIN32 || defined __CYGWIN__
  #ifdef __GNUC__
    #define CYBERGEAR_DRIVER_EXPORT __attribute__ ((dllexport))
    #define CYBERGEAR_DRIVER_IMPORT __attribute__ ((dllimport))
  #else
    #define CYBERGEAR_DRIVER_EXPORT __declspec(dllexport)
    #define CYBERGEAR_DRIVER_IMPORT __declspec(dllimport)
  #endif
  #ifdef CYBERGEAR_DRIVER_BUILDING_LIBRARY
    #define CYBERGEAR_DRIVER_PUBLIC CYBERGEAR_DRIVER_EXPORT
  #else
    #define CYBERGEAR_DRIVER_PUBLIC CYBERGEAR_DRIVER_IMPORT
  #endif
  #define CYBERGEAR_DRIVER_PUBLIC_TYPE CYBERGEAR_DRIVER_PUBLIC
  #define CYBERGEAR_DRIVER_LOCAL
#else
  #define CYBERGEAR_DRIVER_EXPORT __attribute__ ((visibility("default")))
  #define CYBERGEAR_DRIVER_IMPORT
  #if __GNUC__ >= 4
    #define CYBERGEAR_DRIVER_PUBLIC __attribute__ ((visibility("default")))
    #define CYBERGEAR_DRIVER_LOCAL  __attribute__ ((visibility("hidden")))
  #else
    #define CYBERGEAR_DRIVER_PUBLIC
    #define CYBERGEAR_DRIVER_LOCAL
  #endif
  #define CYBERGEAR_DRIVER_PUBLIC_TYPE
#endif

#endif  // CYBERGEAR_DRIVER__VISIBILITY_CONTROL_H_
