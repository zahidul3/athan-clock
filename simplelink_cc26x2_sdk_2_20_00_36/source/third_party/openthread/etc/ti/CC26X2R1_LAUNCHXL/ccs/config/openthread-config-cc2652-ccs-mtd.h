/* include/openthread-config-generic.h.  Generated from openthread-config-generic.h.in by configure.  */
/* include/openthread-config-generic.h.in.  Generated from configure.ac by autoheader.  */

#if OPENTHREAD_MTD
/* all is well */
#endif

#if OPENTHREAD_FTD
#error Wrong Include file, expected MTD
#endif

/* Define to 1 if your C++ compiler doesn't accept -c and -o together. */
/* #undef CXX_NO_MINUS_C_MINUS_O */

/* Define to 1 if you have the <dlfcn.h> header file. */
/* #undef HAVE_DLFCN_H */

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define to 1 if you have the `memcpy' function. */
/* #undef HAVE_MEMCPY */

/* Define to 1 if you have the <memory.h> header file. */
/* #undef HAVE_MEMORY_H */

/* Define to 1 if stdbool.h conforms to C99. */
#define HAVE_STDBOOL_H 1

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define if strlcat exists. */
/* #define HAVE_STRLCAT 1 */

/* Define if strlcpy exists. */
/* #define HAVE_STRLCPY 1 */

/* Define if strnlen exists. */
/* #define HAVE_STRNLEN 1 */

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* Define to 1 if the system has the type `_Bool'. */
#define HAVE__BOOL 1

/* Define to the sub-directory in which libtool stores uninstalled libraries.
   */
#define LT_OBJDIR ".libs/"

/* Define to 1 if your C compiler doesn't accept -c and -o together. */
/* #undef NO_MINUS_C_MINUS_O */

/* Define to 1 if you want to enable CoAP to an application. */
#define OPENTHREAD_ENABLE_APPLICATION_COAP 1

/* Define to 1 if you want to enable Border Router */
#define OPENTHREAD_ENABLE_BORDER_ROUTER 0

/* Define to 1 if you want to enable log for certification test */
#define OPENTHREAD_ENABLE_CERT_LOG 0

/* Define to 1 if you want to use child supervision feature */
#define OPENTHREAD_ENABLE_CHILD_SUPERVISION 0

/* Define to 1 to enable the commissioner role. */
#define OPENTHREAD_ENABLE_COMMISSIONER 0

/* Define to 1 if you want to enable DHCPv6 Client */
#define OPENTHREAD_ENABLE_DHCP6_CLIENT 0

/* Define to 1 if you want to enable DHCPv6 Server */
#define OPENTHREAD_ENABLE_DHCP6_SERVER 0

/* Define to 1 if you want to use diagnostics module */
#define OPENTHREAD_ENABLE_DIAG 1

/* Define to 1 if you want to enable DNS Client */
#define OPENTHREAD_ENABLE_DNS_CLIENT 0

/* Define to 1 to enable dtls support. */
#define OPENTHREAD_ENABLE_DTLS 1

/* Define to 1 if you want to use jam detection feature */
#define OPENTHREAD_ENABLE_JAM_DETECTION 0

/* Define to 1 to enable the joiner role. */
#define OPENTHREAD_ENABLE_JOINER 1

/* Define to 1 if you want to use legacy network support */
#define OPENTHREAD_ENABLE_LEGACY 0

/* Define to 1 if you want to use MAC filter feature */
#define OPENTHREAD_ENABLE_MAC_FILTER 0

/* Define to 1 to enable network diagnostic for MTD. */
#define OPENTHREAD_ENABLE_MTD_NETWORK_DIAGNOSTIC 0

/* Define to 1 if you want to enable support for multiple OpenThread
   instances. */
#define OPENTHREAD_ENABLE_MULTIPLE_INSTANCES 0

/* Define to 1 to enable the NCP SPI interface. */
#define OPENTHREAD_ENABLE_NCP_SPI 0

/* Define to 1 if using NCP Spinel Encrypter */
#define OPENTHREAD_ENABLE_NCP_SPINEL_ENCRYPTER 0

/* Define to 1 to enable the NCP UART interface. */
#define OPENTHREAD_ENABLE_NCP_UART 1

/* Define to 1 if using NCP vendor hook */
#define OPENTHREAD_ENABLE_NCP_VENDOR_HOOK 0

/* Define to 1 if you want to enable raw link-layer API */
#define OPENTHREAD_ENABLE_RAW_LINK_API 1

/* Define to 1 if you want to enable Service */
#define OPENTHREAD_ENABLE_SERVICE 0

/* Define to 1 to enable the TMF proxy feature. */
#define OPENTHREAD_ENABLE_TMF_PROXY 0

/* OpenThread examples */
#define OPENTHREAD_EXAMPLES cc2652

/* Define to 1 if you want to use cc1352 examples */
/* #undef OPENTHREAD_EXAMPLES_CC1352 */

/* Define to 1 if you want to use cc2538 examples */
/* #undef OPENTHREAD_EXAMPLES_CC2538 */

/* Define to 1 if you want to use cc2650 examples */
/* #undef OPENTHREAD_EXAMPLES_CC2650 */

/* Define to 1 if you want to use cc2652 examples */
#define OPENTHREAD_EXAMPLES_CC2652 1

/* Define to 1 if you want to use da15000 examples */
/* #undef OPENTHREAD_EXAMPLES_DA15000 */

/* Define to 1 if you want to use efr32 examples */
/* #undef OPENTHREAD_EXAMPLES_EFR32 */

/* Define to 1 if you want to use emsk examples */
/* #undef OPENTHREAD_EXAMPLES_EMSK */

/* Define to 1 if you want to use gp712 examples */
/* #undef OPENTHREAD_EXAMPLES_GP712 */

/* Define to 1 if you want to use kw41z examples */
/* #undef OPENTHREAD_EXAMPLES_KW41Z */

/* Define to 1 if you want to use nrf52840 examples */
/* #undef OPENTHREAD_EXAMPLES_NRF52840 */

/* Define to 1 if you want to use posix examples */
/* #undef OPENTHREAD_EXAMPLES_POSIX */

/* Define to 1 if you want to use samr21 examples */
/* #undef OPENTHREAD_EXAMPLES_SAMR21 */

/* Name of package */
#define PACKAGE "ti-openthread"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT "http://e2e.ti.com"

/* Define to the full name of this package. */
#define PACKAGE_NAME "TI-OPENTHREAD"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "TI-OPENTHREAD " PACKAGE_VERSION

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "openthread"

/* Define to the home page for this package. */
#define PACKAGE_URL "http://www.ti.com/tool/LAUNCHXL-CC26X2R1"

/* Define to the version of this package. */
#define PACKAGE_VERSION "1.2.0.0"

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Version number of package */
#define VERSION "1.2.0.0"

/* Define for Solaris 2.5.1 so the uint32_t typedef from <sys/synch.h>,
   <pthread.h>, or <semaphore.h> is not used. If the typedef were allowed, the
   #define below would cause a syntax error. */
/* #undef _UINT32_T */

/* Define for Solaris 2.5.1 so the uint64_t typedef from <sys/synch.h>,
   <pthread.h>, or <semaphore.h> is not used. If the typedef were allowed, the
   #define below would cause a syntax error. */
/* #undef _UINT64_T */

/* Define for Solaris 2.5.1 so the uint8_t typedef from <sys/synch.h>,
   <pthread.h>, or <semaphore.h> is not used. If the typedef were allowed, the
   #define below would cause a syntax error. */
/* #undef _UINT8_T */

/* Define to the type of a signed integer type of width exactly 16 bits if
   such a type exists and the standard includes do not define it. */
/* #undef int16_t */

/* Define to the type of a signed integer type of width exactly 32 bits if
   such a type exists and the standard includes do not define it. */
/* #undef int32_t */

/* Define to the type of a signed integer type of width exactly 64 bits if
   such a type exists and the standard includes do not define it. */
/* #undef int64_t */

/* Define to the type of a signed integer type of width exactly 8 bits if such
   a type exists and the standard includes do not define it. */
/* #undef int8_t */

/* Define to the type of an unsigned integer type of width exactly 16 bits if
   such a type exists and the standard includes do not define it. */
/* #undef uint16_t */

/* Define to the type of an unsigned integer type of width exactly 32 bits if
   such a type exists and the standard includes do not define it. */
/* #undef uint32_t */

/* Define to the type of an unsigned integer type of width exactly 64 bits if
   such a type exists and the standard includes do not define it. */
/* #undef uint64_t */

/* Define to the type of an unsigned integer type of width exactly 8 bits if
   such a type exists and the standard includes do not define it. */
/* #undef uint8_t */

#define ENABLE_ALL_LOGS_TO_UART 0
#if   ENABLE_ALL_LOGS_TO_UART
/* Enable volumous logging via the below */

/* Where should logs go?
 * For details & options see:
 * See:  ${openthread}/src/core/openthread-core-default-config.h
 * Note: Default is: OPENTHREAD_CONFIG_LOG_OUTPUT_NONE
 */
#define OPENTHREAD_CONFIG_LOG_OUTPUT              OPENTHREAD_CONFIG_LOG_OUTPUT_DEBUG_UART
/* Details of the debug uart can be found in the file: 'platform/dbg_uart.c' file within the examples.
 */

/*
 * This enables the Debug Uart, by default it is disabled.
 */
#define OPENTHREAD_CONFIG_ENABLE_DEBUG_UART       1


/*
 * Openthread has multiple log levels, DEBG is the highest.
 * For details & selections:
 * See ${openthread}/include/openthread/platform/logging.h  (note: Single ".h" here)
 */
#define OPENTHREAD_CONFIG_LOG_LEVEL               OT_LOG_LEVEL_DEBG

/* Each significant area openthread can be configured to log, or not log
 * The areas are enabled via various #defines, see below for a partial list.
 * For more details, see the openthread internal ".hpp"
 *
 * See:  ${openthread}/openthread/src/core/common/logging.hpp
 * Also note above level is defined in the single ".h" log header
 * Where as each area is controlled by the C++ header ".hpp" log header.
 */
#define OPENTHREAD_CONFIG_LOG_API                 1
#define OPENTHREAD_CONFIG_LOG_ARP                 1
#define OPENTHREAD_CONFIG_LOG_CLI                 1
#define OPENTHREAD_CONFIG_LOG_COAP                1
#define OPENTHREAD_CONFIG_LOG_ICMP                1
#define OPENTHREAD_CONFIG_LOG_IP6                 1
#define OPENTHREAD_CONFIG_LOG_MAC                 1
#define OPENTHREAD_CONFIG_LOG_MEM                 1
#define OPENTHREAD_CONFIG_LOG_MLE                 1
#define OPENTHREAD_CONFIG_LOG_NETDATA             1
#define OPENTHREAD_CONFIG_LOG_NETDIAG             1
#define OPENTHREAD_CONFIG_LOG_PKT_DUMP            1
#define OPENTHREAD_CONFIG_LOG_PLATFORM            1
#define OPENTHREAD_CONFIG_LOG_PREPEND_LEVEL       1
#define OPENTHREAD_CONFIG_LOG_PREPEND_REGION      1

#endif

/* match the output buffer size to Diag porcess max buffer.
 * size = 256. Otherwise some output chars are chopped
 */
#define OPENTHREAD_CONFIG_CLI_MAX_LINE_LENGTH     256
