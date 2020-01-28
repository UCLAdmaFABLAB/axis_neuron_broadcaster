
#ifndef NEURONDATAREADER_API_H
#define NEURONDATAREADER_API_H

#ifdef NEURONDATAREADER_STATIC_DEFINE
#  define NEURONDATAREADER_API
#  define NEURONDATAREADER_NO_EXPORT
#else
#  ifndef NEURONDATAREADER_API
#    ifdef NeuronDataReader_EXPORTS
        /* We are building this library */
#      define NEURONDATAREADER_API __attribute__((visibility("default")))
#    else
        /* We are using this library */
#      define NEURONDATAREADER_API __attribute__((visibility("default")))
#    endif
#  endif

#  ifndef NEURONDATAREADER_NO_EXPORT
#    define NEURONDATAREADER_NO_EXPORT __attribute__((visibility("hidden")))
#  endif
#endif

#ifndef NEURONDATAREADER_DEPRECATED
#  define NEURONDATAREADER_DEPRECATED __attribute__ ((__deprecated__))
#endif

#ifndef NEURONDATAREADER_DEPRECATED_EXPORT
#  define NEURONDATAREADER_DEPRECATED_EXPORT NEURONDATAREADER_API NEURONDATAREADER_DEPRECATED
#endif

#ifndef NEURONDATAREADER_DEPRECATED_NO_EXPORT
#  define NEURONDATAREADER_DEPRECATED_NO_EXPORT NEURONDATAREADER_NO_EXPORT NEURONDATAREADER_DEPRECATED
#endif

#define DEFINE_NO_DEPRECATED 0
#if DEFINE_NO_DEPRECATED
# define NEURONDATAREADER_NO_DEPRECATED
#endif

#endif
