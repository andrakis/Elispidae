#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-Linux
CND_DLIB_EXT=so
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/PocoLithp/ELisp.o \
	${OBJECTDIR}/PocoLithp/PLint_recursive.o \
	${OBJECTDIR}/PocoLithp/PLint_stackless.o \
	${OBJECTDIR}/PocoLithp/PLinterpreter.o \
	${OBJECTDIR}/PocoLithp/PLparser.o \
	${OBJECTDIR}/PocoLithp/PLruntime.o \
	${OBJECTDIR}/PocoLithp/PLtests.o \
	${OBJECTDIR}/PocoLithp/PLtypes.o \
	${OBJECTDIR}/PocoLithp/PocoLithp.o \
	${OBJECTDIR}/PocoLithp/stdafx.o \
	${OBJECTDIR}/PocoLithp/stdlib/ELdictionary.o \
	${OBJECTDIR}/PocoLithp/stdlib/ELlists.o \
	${OBJECTDIR}/PocoLithp/stdlib/ELstrings.o \
	${OBJECTDIR}/PocoLithp/stdlib/ELthreads.o \
	${OBJECTDIR}/PocoLithp/stdlib/PLglobals.o \
	${OBJECTDIR}/contrib/linenoise-ng/src/ConvertUTF.o \
	${OBJECTDIR}/contrib/linenoise-ng/src/linenoise.o \
	${OBJECTDIR}/contrib/linenoise-ng/src/wcwidth.o \
	${OBJECTDIR}/contrib/poco/Foundation/src/ASCIIEncoding.o \
	${OBJECTDIR}/contrib/poco/Foundation/src/Ascii.o \
	${OBJECTDIR}/contrib/poco/Foundation/src/AtomicCounter.o \
	${OBJECTDIR}/contrib/poco/Foundation/src/Bugcheck.o \
	${OBJECTDIR}/contrib/poco/Foundation/src/DateTime.o \
	${OBJECTDIR}/contrib/poco/Foundation/src/DateTimeFormat.o \
	${OBJECTDIR}/contrib/poco/Foundation/src/DateTimeParser.o \
	${OBJECTDIR}/contrib/poco/Foundation/src/Debugger.o \
	${OBJECTDIR}/contrib/poco/Foundation/src/Exception.o \
	${OBJECTDIR}/contrib/poco/Foundation/src/Format.o \
	${OBJECTDIR}/contrib/poco/Foundation/src/Latin1Encoding.o \
	${OBJECTDIR}/contrib/poco/Foundation/src/Latin2Encoding.o \
	${OBJECTDIR}/contrib/poco/Foundation/src/Latin9Encoding.o \
	${OBJECTDIR}/contrib/poco/Foundation/src/LocalDateTime.o \
	${OBJECTDIR}/contrib/poco/Foundation/src/Mutex.o \
	${OBJECTDIR}/contrib/poco/Foundation/src/NumberFormatter.o \
	${OBJECTDIR}/contrib/poco/Foundation/src/NumberParser.o \
	${OBJECTDIR}/contrib/poco/Foundation/src/NumericString.o \
	${OBJECTDIR}/contrib/poco/Foundation/src/RWLock.o \
	${OBJECTDIR}/contrib/poco/Foundation/src/RefCountedObject.o \
	${OBJECTDIR}/contrib/poco/Foundation/src/TextConverter.o \
	${OBJECTDIR}/contrib/poco/Foundation/src/TextEncoding.o \
	${OBJECTDIR}/contrib/poco/Foundation/src/TextIterator.o \
	${OBJECTDIR}/contrib/poco/Foundation/src/Timespan.o \
	${OBJECTDIR}/contrib/poco/Foundation/src/Timestamp.o \
	${OBJECTDIR}/contrib/poco/Foundation/src/Timezone.o \
	${OBJECTDIR}/contrib/poco/Foundation/src/UTF16Encoding.o \
	${OBJECTDIR}/contrib/poco/Foundation/src/UTF32Encoding.o \
	${OBJECTDIR}/contrib/poco/Foundation/src/UTF8Encoding.o \
	${OBJECTDIR}/contrib/poco/Foundation/src/UTF8String.o \
	${OBJECTDIR}/contrib/poco/Foundation/src/Unicode.o \
	${OBJECTDIR}/contrib/poco/Foundation/src/UnicodeConverter.o \
	${OBJECTDIR}/contrib/poco/Foundation/src/Var.o \
	${OBJECTDIR}/contrib/poco/Foundation/src/VarHolder.o \
	${OBJECTDIR}/contrib/poco/Foundation/src/VarIterator.o \
	${OBJECTDIR}/contrib/poco/Foundation/src/Windows1250Encoding.o \
	${OBJECTDIR}/contrib/poco/Foundation/src/Windows1251Encoding.o \
	${OBJECTDIR}/contrib/poco/Foundation/src/Windows1252Encoding.o \
	${OBJECTDIR}/contrib/poco/Foundation/src/pcre_byte_order.o \
	${OBJECTDIR}/contrib/poco/Foundation/src/pcre_chartables.o \
	${OBJECTDIR}/contrib/poco/Foundation/src/pcre_compile.o \
	${OBJECTDIR}/contrib/poco/Foundation/src/pcre_config.o \
	${OBJECTDIR}/contrib/poco/Foundation/src/pcre_dfa_exec.o \
	${OBJECTDIR}/contrib/poco/Foundation/src/pcre_exec.o \
	${OBJECTDIR}/contrib/poco/Foundation/src/pcre_fullinfo.o \
	${OBJECTDIR}/contrib/poco/Foundation/src/pcre_get.o \
	${OBJECTDIR}/contrib/poco/Foundation/src/pcre_globals.o \
	${OBJECTDIR}/contrib/poco/Foundation/src/pcre_jit_compile.o \
	${OBJECTDIR}/contrib/poco/Foundation/src/pcre_maketables.o \
	${OBJECTDIR}/contrib/poco/Foundation/src/pcre_newline.o \
	${OBJECTDIR}/contrib/poco/Foundation/src/pcre_ord2utf8.o \
	${OBJECTDIR}/contrib/poco/Foundation/src/pcre_refcount.o \
	${OBJECTDIR}/contrib/poco/Foundation/src/pcre_string_utils.o \
	${OBJECTDIR}/contrib/poco/Foundation/src/pcre_study.o \
	${OBJECTDIR}/contrib/poco/Foundation/src/pcre_tables.o \
	${OBJECTDIR}/contrib/poco/Foundation/src/pcre_ucd.o \
	${OBJECTDIR}/contrib/poco/Foundation/src/pcre_valid_utf8.o \
	${OBJECTDIR}/contrib/poco/Foundation/src/pcre_version.o \
	${OBJECTDIR}/contrib/poco/Foundation/src/pcre_xclass.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk bin/elisp

bin/elisp: ${OBJECTFILES}
	${MKDIR} -p bin
	${LINK.cc} -o bin/elisp ${OBJECTFILES} ${LDLIBSOPTIONS} -pthread -s

${OBJECTDIR}/PocoLithp/ELisp.o: PocoLithp/ELisp.cpp
	${MKDIR} -p ${OBJECTDIR}/PocoLithp
	${RM} "$@.d"
	$(COMPILE.cc) -g -DREADLINE_NG -D_DEBUG -Icontrib/poco/Foundation/include -Icontrib/linenoise-ng/include -Icontrib/Stackless/Stackless/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/PocoLithp/ELisp.o PocoLithp/ELisp.cpp

${OBJECTDIR}/PocoLithp/PLint_recursive.o: PocoLithp/PLint_recursive.cpp
	${MKDIR} -p ${OBJECTDIR}/PocoLithp
	${RM} "$@.d"
	$(COMPILE.cc) -g -DREADLINE_NG -D_DEBUG -Icontrib/poco/Foundation/include -Icontrib/linenoise-ng/include -Icontrib/Stackless/Stackless/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/PocoLithp/PLint_recursive.o PocoLithp/PLint_recursive.cpp

${OBJECTDIR}/PocoLithp/PLint_stackless.o: PocoLithp/PLint_stackless.cpp
	${MKDIR} -p ${OBJECTDIR}/PocoLithp
	${RM} "$@.d"
	$(COMPILE.cc) -g -DREADLINE_NG -D_DEBUG -Icontrib/poco/Foundation/include -Icontrib/linenoise-ng/include -Icontrib/Stackless/Stackless/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/PocoLithp/PLint_stackless.o PocoLithp/PLint_stackless.cpp

${OBJECTDIR}/PocoLithp/PLinterpreter.o: PocoLithp/PLinterpreter.cpp
	${MKDIR} -p ${OBJECTDIR}/PocoLithp
	${RM} "$@.d"
	$(COMPILE.cc) -g -DREADLINE_NG -D_DEBUG -Icontrib/poco/Foundation/include -Icontrib/linenoise-ng/include -Icontrib/Stackless/Stackless/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/PocoLithp/PLinterpreter.o PocoLithp/PLinterpreter.cpp

${OBJECTDIR}/PocoLithp/PLparser.o: PocoLithp/PLparser.cpp
	${MKDIR} -p ${OBJECTDIR}/PocoLithp
	${RM} "$@.d"
	$(COMPILE.cc) -g -DREADLINE_NG -D_DEBUG -Icontrib/poco/Foundation/include -Icontrib/linenoise-ng/include -Icontrib/Stackless/Stackless/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/PocoLithp/PLparser.o PocoLithp/PLparser.cpp

${OBJECTDIR}/PocoLithp/PLruntime.o: PocoLithp/PLruntime.cpp
	${MKDIR} -p ${OBJECTDIR}/PocoLithp
	${RM} "$@.d"
	$(COMPILE.cc) -g -DREADLINE_NG -D_DEBUG -Icontrib/poco/Foundation/include -Icontrib/linenoise-ng/include -Icontrib/Stackless/Stackless/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/PocoLithp/PLruntime.o PocoLithp/PLruntime.cpp

${OBJECTDIR}/PocoLithp/PLtests.o: PocoLithp/PLtests.cpp
	${MKDIR} -p ${OBJECTDIR}/PocoLithp
	${RM} "$@.d"
	$(COMPILE.cc) -g -DREADLINE_NG -D_DEBUG -Icontrib/poco/Foundation/include -Icontrib/linenoise-ng/include -Icontrib/Stackless/Stackless/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/PocoLithp/PLtests.o PocoLithp/PLtests.cpp

${OBJECTDIR}/PocoLithp/PLtypes.o: PocoLithp/PLtypes.cpp
	${MKDIR} -p ${OBJECTDIR}/PocoLithp
	${RM} "$@.d"
	$(COMPILE.cc) -g -DREADLINE_NG -D_DEBUG -Icontrib/poco/Foundation/include -Icontrib/linenoise-ng/include -Icontrib/Stackless/Stackless/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/PocoLithp/PLtypes.o PocoLithp/PLtypes.cpp

${OBJECTDIR}/PocoLithp/PocoLithp.o: PocoLithp/PocoLithp.cpp
	${MKDIR} -p ${OBJECTDIR}/PocoLithp
	${RM} "$@.d"
	$(COMPILE.cc) -g -DREADLINE_NG -D_DEBUG -Icontrib/poco/Foundation/include -Icontrib/linenoise-ng/include -Icontrib/Stackless/Stackless/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/PocoLithp/PocoLithp.o PocoLithp/PocoLithp.cpp

${OBJECTDIR}/PocoLithp/stdafx.o: PocoLithp/stdafx.cpp
	${MKDIR} -p ${OBJECTDIR}/PocoLithp
	${RM} "$@.d"
	$(COMPILE.cc) -g -DREADLINE_NG -D_DEBUG -Icontrib/poco/Foundation/include -Icontrib/linenoise-ng/include -Icontrib/Stackless/Stackless/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/PocoLithp/stdafx.o PocoLithp/stdafx.cpp

${OBJECTDIR}/PocoLithp/stdlib/ELdictionary.o: PocoLithp/stdlib/ELdictionary.cpp
	${MKDIR} -p ${OBJECTDIR}/PocoLithp/stdlib
	${RM} "$@.d"
	$(COMPILE.cc) -g -DREADLINE_NG -D_DEBUG -Icontrib/poco/Foundation/include -Icontrib/linenoise-ng/include -Icontrib/Stackless/Stackless/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/PocoLithp/stdlib/ELdictionary.o PocoLithp/stdlib/ELdictionary.cpp

${OBJECTDIR}/PocoLithp/stdlib/ELlists.o: PocoLithp/stdlib/ELlists.cpp
	${MKDIR} -p ${OBJECTDIR}/PocoLithp/stdlib
	${RM} "$@.d"
	$(COMPILE.cc) -g -DREADLINE_NG -D_DEBUG -Icontrib/poco/Foundation/include -Icontrib/linenoise-ng/include -Icontrib/Stackless/Stackless/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/PocoLithp/stdlib/ELlists.o PocoLithp/stdlib/ELlists.cpp

${OBJECTDIR}/PocoLithp/stdlib/ELstrings.o: PocoLithp/stdlib/ELstrings.cpp
	${MKDIR} -p ${OBJECTDIR}/PocoLithp/stdlib
	${RM} "$@.d"
	$(COMPILE.cc) -g -DREADLINE_NG -D_DEBUG -Icontrib/poco/Foundation/include -Icontrib/linenoise-ng/include -Icontrib/Stackless/Stackless/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/PocoLithp/stdlib/ELstrings.o PocoLithp/stdlib/ELstrings.cpp

${OBJECTDIR}/PocoLithp/stdlib/ELthreads.o: PocoLithp/stdlib/ELthreads.cpp
	${MKDIR} -p ${OBJECTDIR}/PocoLithp/stdlib
	${RM} "$@.d"
	$(COMPILE.cc) -g -DREADLINE_NG -D_DEBUG -Icontrib/poco/Foundation/include -Icontrib/linenoise-ng/include -Icontrib/Stackless/Stackless/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/PocoLithp/stdlib/ELthreads.o PocoLithp/stdlib/ELthreads.cpp

${OBJECTDIR}/PocoLithp/stdlib/PLglobals.o: PocoLithp/stdlib/PLglobals.cpp
	${MKDIR} -p ${OBJECTDIR}/PocoLithp/stdlib
	${RM} "$@.d"
	$(COMPILE.cc) -g -DREADLINE_NG -D_DEBUG -Icontrib/poco/Foundation/include -Icontrib/linenoise-ng/include -Icontrib/Stackless/Stackless/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/PocoLithp/stdlib/PLglobals.o PocoLithp/stdlib/PLglobals.cpp

${OBJECTDIR}/contrib/linenoise-ng/src/ConvertUTF.o: contrib/linenoise-ng/src/ConvertUTF.cpp
	${MKDIR} -p ${OBJECTDIR}/contrib/linenoise-ng/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -DREADLINE_NG -D_DEBUG -Icontrib/poco/Foundation/include -Icontrib/linenoise-ng/include -Icontrib/Stackless/Stackless/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/contrib/linenoise-ng/src/ConvertUTF.o contrib/linenoise-ng/src/ConvertUTF.cpp

${OBJECTDIR}/contrib/linenoise-ng/src/linenoise.o: contrib/linenoise-ng/src/linenoise.cpp
	${MKDIR} -p ${OBJECTDIR}/contrib/linenoise-ng/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -DREADLINE_NG -D_DEBUG -Icontrib/poco/Foundation/include -Icontrib/linenoise-ng/include -Icontrib/Stackless/Stackless/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/contrib/linenoise-ng/src/linenoise.o contrib/linenoise-ng/src/linenoise.cpp

${OBJECTDIR}/contrib/linenoise-ng/src/wcwidth.o: contrib/linenoise-ng/src/wcwidth.cpp
	${MKDIR} -p ${OBJECTDIR}/contrib/linenoise-ng/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -DREADLINE_NG -D_DEBUG -Icontrib/poco/Foundation/include -Icontrib/linenoise-ng/include -Icontrib/Stackless/Stackless/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/contrib/linenoise-ng/src/wcwidth.o contrib/linenoise-ng/src/wcwidth.cpp

${OBJECTDIR}/contrib/poco/Foundation/src/ASCIIEncoding.o: contrib/poco/Foundation/src/ASCIIEncoding.cpp
	${MKDIR} -p ${OBJECTDIR}/contrib/poco/Foundation/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -DREADLINE_NG -D_DEBUG -Icontrib/poco/Foundation/include -Icontrib/linenoise-ng/include -Icontrib/Stackless/Stackless/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/contrib/poco/Foundation/src/ASCIIEncoding.o contrib/poco/Foundation/src/ASCIIEncoding.cpp

${OBJECTDIR}/contrib/poco/Foundation/src/Ascii.o: contrib/poco/Foundation/src/Ascii.cpp
	${MKDIR} -p ${OBJECTDIR}/contrib/poco/Foundation/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -DREADLINE_NG -D_DEBUG -Icontrib/poco/Foundation/include -Icontrib/linenoise-ng/include -Icontrib/Stackless/Stackless/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/contrib/poco/Foundation/src/Ascii.o contrib/poco/Foundation/src/Ascii.cpp

${OBJECTDIR}/contrib/poco/Foundation/src/AtomicCounter.o: contrib/poco/Foundation/src/AtomicCounter.cpp
	${MKDIR} -p ${OBJECTDIR}/contrib/poco/Foundation/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -DREADLINE_NG -D_DEBUG -Icontrib/poco/Foundation/include -Icontrib/linenoise-ng/include -Icontrib/Stackless/Stackless/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/contrib/poco/Foundation/src/AtomicCounter.o contrib/poco/Foundation/src/AtomicCounter.cpp

${OBJECTDIR}/contrib/poco/Foundation/src/Bugcheck.o: contrib/poco/Foundation/src/Bugcheck.cpp
	${MKDIR} -p ${OBJECTDIR}/contrib/poco/Foundation/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -DREADLINE_NG -D_DEBUG -Icontrib/poco/Foundation/include -Icontrib/linenoise-ng/include -Icontrib/Stackless/Stackless/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/contrib/poco/Foundation/src/Bugcheck.o contrib/poco/Foundation/src/Bugcheck.cpp

${OBJECTDIR}/contrib/poco/Foundation/src/DateTime.o: contrib/poco/Foundation/src/DateTime.cpp
	${MKDIR} -p ${OBJECTDIR}/contrib/poco/Foundation/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -DREADLINE_NG -D_DEBUG -Icontrib/poco/Foundation/include -Icontrib/linenoise-ng/include -Icontrib/Stackless/Stackless/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/contrib/poco/Foundation/src/DateTime.o contrib/poco/Foundation/src/DateTime.cpp

${OBJECTDIR}/contrib/poco/Foundation/src/DateTimeFormat.o: contrib/poco/Foundation/src/DateTimeFormat.cpp
	${MKDIR} -p ${OBJECTDIR}/contrib/poco/Foundation/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -DREADLINE_NG -D_DEBUG -Icontrib/poco/Foundation/include -Icontrib/linenoise-ng/include -Icontrib/Stackless/Stackless/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/contrib/poco/Foundation/src/DateTimeFormat.o contrib/poco/Foundation/src/DateTimeFormat.cpp

${OBJECTDIR}/contrib/poco/Foundation/src/DateTimeParser.o: contrib/poco/Foundation/src/DateTimeParser.cpp
	${MKDIR} -p ${OBJECTDIR}/contrib/poco/Foundation/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -DREADLINE_NG -D_DEBUG -Icontrib/poco/Foundation/include -Icontrib/linenoise-ng/include -Icontrib/Stackless/Stackless/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/contrib/poco/Foundation/src/DateTimeParser.o contrib/poco/Foundation/src/DateTimeParser.cpp

${OBJECTDIR}/contrib/poco/Foundation/src/Debugger.o: contrib/poco/Foundation/src/Debugger.cpp
	${MKDIR} -p ${OBJECTDIR}/contrib/poco/Foundation/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -DREADLINE_NG -D_DEBUG -Icontrib/poco/Foundation/include -Icontrib/linenoise-ng/include -Icontrib/Stackless/Stackless/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/contrib/poco/Foundation/src/Debugger.o contrib/poco/Foundation/src/Debugger.cpp

${OBJECTDIR}/contrib/poco/Foundation/src/Exception.o: contrib/poco/Foundation/src/Exception.cpp
	${MKDIR} -p ${OBJECTDIR}/contrib/poco/Foundation/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -DREADLINE_NG -D_DEBUG -Icontrib/poco/Foundation/include -Icontrib/linenoise-ng/include -Icontrib/Stackless/Stackless/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/contrib/poco/Foundation/src/Exception.o contrib/poco/Foundation/src/Exception.cpp

${OBJECTDIR}/contrib/poco/Foundation/src/Format.o: contrib/poco/Foundation/src/Format.cpp
	${MKDIR} -p ${OBJECTDIR}/contrib/poco/Foundation/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -DREADLINE_NG -D_DEBUG -Icontrib/poco/Foundation/include -Icontrib/linenoise-ng/include -Icontrib/Stackless/Stackless/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/contrib/poco/Foundation/src/Format.o contrib/poco/Foundation/src/Format.cpp

${OBJECTDIR}/contrib/poco/Foundation/src/Latin1Encoding.o: contrib/poco/Foundation/src/Latin1Encoding.cpp
	${MKDIR} -p ${OBJECTDIR}/contrib/poco/Foundation/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -DREADLINE_NG -D_DEBUG -Icontrib/poco/Foundation/include -Icontrib/linenoise-ng/include -Icontrib/Stackless/Stackless/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/contrib/poco/Foundation/src/Latin1Encoding.o contrib/poco/Foundation/src/Latin1Encoding.cpp

${OBJECTDIR}/contrib/poco/Foundation/src/Latin2Encoding.o: contrib/poco/Foundation/src/Latin2Encoding.cpp
	${MKDIR} -p ${OBJECTDIR}/contrib/poco/Foundation/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -DREADLINE_NG -D_DEBUG -Icontrib/poco/Foundation/include -Icontrib/linenoise-ng/include -Icontrib/Stackless/Stackless/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/contrib/poco/Foundation/src/Latin2Encoding.o contrib/poco/Foundation/src/Latin2Encoding.cpp

${OBJECTDIR}/contrib/poco/Foundation/src/Latin9Encoding.o: contrib/poco/Foundation/src/Latin9Encoding.cpp
	${MKDIR} -p ${OBJECTDIR}/contrib/poco/Foundation/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -DREADLINE_NG -D_DEBUG -Icontrib/poco/Foundation/include -Icontrib/linenoise-ng/include -Icontrib/Stackless/Stackless/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/contrib/poco/Foundation/src/Latin9Encoding.o contrib/poco/Foundation/src/Latin9Encoding.cpp

${OBJECTDIR}/contrib/poco/Foundation/src/LocalDateTime.o: contrib/poco/Foundation/src/LocalDateTime.cpp
	${MKDIR} -p ${OBJECTDIR}/contrib/poco/Foundation/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -DREADLINE_NG -D_DEBUG -Icontrib/poco/Foundation/include -Icontrib/linenoise-ng/include -Icontrib/Stackless/Stackless/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/contrib/poco/Foundation/src/LocalDateTime.o contrib/poco/Foundation/src/LocalDateTime.cpp

${OBJECTDIR}/contrib/poco/Foundation/src/Mutex.o: contrib/poco/Foundation/src/Mutex.cpp
	${MKDIR} -p ${OBJECTDIR}/contrib/poco/Foundation/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -DREADLINE_NG -D_DEBUG -Icontrib/poco/Foundation/include -Icontrib/linenoise-ng/include -Icontrib/Stackless/Stackless/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/contrib/poco/Foundation/src/Mutex.o contrib/poco/Foundation/src/Mutex.cpp

${OBJECTDIR}/contrib/poco/Foundation/src/NumberFormatter.o: contrib/poco/Foundation/src/NumberFormatter.cpp
	${MKDIR} -p ${OBJECTDIR}/contrib/poco/Foundation/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -DREADLINE_NG -D_DEBUG -Icontrib/poco/Foundation/include -Icontrib/linenoise-ng/include -Icontrib/Stackless/Stackless/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/contrib/poco/Foundation/src/NumberFormatter.o contrib/poco/Foundation/src/NumberFormatter.cpp

${OBJECTDIR}/contrib/poco/Foundation/src/NumberParser.o: contrib/poco/Foundation/src/NumberParser.cpp
	${MKDIR} -p ${OBJECTDIR}/contrib/poco/Foundation/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -DREADLINE_NG -D_DEBUG -Icontrib/poco/Foundation/include -Icontrib/linenoise-ng/include -Icontrib/Stackless/Stackless/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/contrib/poco/Foundation/src/NumberParser.o contrib/poco/Foundation/src/NumberParser.cpp

${OBJECTDIR}/contrib/poco/Foundation/src/NumericString.o: contrib/poco/Foundation/src/NumericString.cpp
	${MKDIR} -p ${OBJECTDIR}/contrib/poco/Foundation/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -DREADLINE_NG -D_DEBUG -Icontrib/poco/Foundation/include -Icontrib/linenoise-ng/include -Icontrib/Stackless/Stackless/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/contrib/poco/Foundation/src/NumericString.o contrib/poco/Foundation/src/NumericString.cpp

${OBJECTDIR}/contrib/poco/Foundation/src/RWLock.o: contrib/poco/Foundation/src/RWLock.cpp
	${MKDIR} -p ${OBJECTDIR}/contrib/poco/Foundation/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -DREADLINE_NG -D_DEBUG -Icontrib/poco/Foundation/include -Icontrib/linenoise-ng/include -Icontrib/Stackless/Stackless/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/contrib/poco/Foundation/src/RWLock.o contrib/poco/Foundation/src/RWLock.cpp

${OBJECTDIR}/contrib/poco/Foundation/src/RefCountedObject.o: contrib/poco/Foundation/src/RefCountedObject.cpp
	${MKDIR} -p ${OBJECTDIR}/contrib/poco/Foundation/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -DREADLINE_NG -D_DEBUG -Icontrib/poco/Foundation/include -Icontrib/linenoise-ng/include -Icontrib/Stackless/Stackless/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/contrib/poco/Foundation/src/RefCountedObject.o contrib/poco/Foundation/src/RefCountedObject.cpp

${OBJECTDIR}/contrib/poco/Foundation/src/TextConverter.o: contrib/poco/Foundation/src/TextConverter.cpp
	${MKDIR} -p ${OBJECTDIR}/contrib/poco/Foundation/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -DREADLINE_NG -D_DEBUG -Icontrib/poco/Foundation/include -Icontrib/linenoise-ng/include -Icontrib/Stackless/Stackless/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/contrib/poco/Foundation/src/TextConverter.o contrib/poco/Foundation/src/TextConverter.cpp

${OBJECTDIR}/contrib/poco/Foundation/src/TextEncoding.o: contrib/poco/Foundation/src/TextEncoding.cpp
	${MKDIR} -p ${OBJECTDIR}/contrib/poco/Foundation/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -DREADLINE_NG -D_DEBUG -Icontrib/poco/Foundation/include -Icontrib/linenoise-ng/include -Icontrib/Stackless/Stackless/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/contrib/poco/Foundation/src/TextEncoding.o contrib/poco/Foundation/src/TextEncoding.cpp

${OBJECTDIR}/contrib/poco/Foundation/src/TextIterator.o: contrib/poco/Foundation/src/TextIterator.cpp
	${MKDIR} -p ${OBJECTDIR}/contrib/poco/Foundation/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -DREADLINE_NG -D_DEBUG -Icontrib/poco/Foundation/include -Icontrib/linenoise-ng/include -Icontrib/Stackless/Stackless/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/contrib/poco/Foundation/src/TextIterator.o contrib/poco/Foundation/src/TextIterator.cpp

${OBJECTDIR}/contrib/poco/Foundation/src/Timespan.o: contrib/poco/Foundation/src/Timespan.cpp
	${MKDIR} -p ${OBJECTDIR}/contrib/poco/Foundation/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -DREADLINE_NG -D_DEBUG -Icontrib/poco/Foundation/include -Icontrib/linenoise-ng/include -Icontrib/Stackless/Stackless/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/contrib/poco/Foundation/src/Timespan.o contrib/poco/Foundation/src/Timespan.cpp

${OBJECTDIR}/contrib/poco/Foundation/src/Timestamp.o: contrib/poco/Foundation/src/Timestamp.cpp
	${MKDIR} -p ${OBJECTDIR}/contrib/poco/Foundation/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -DREADLINE_NG -D_DEBUG -Icontrib/poco/Foundation/include -Icontrib/linenoise-ng/include -Icontrib/Stackless/Stackless/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/contrib/poco/Foundation/src/Timestamp.o contrib/poco/Foundation/src/Timestamp.cpp

${OBJECTDIR}/contrib/poco/Foundation/src/Timezone.o: contrib/poco/Foundation/src/Timezone.cpp
	${MKDIR} -p ${OBJECTDIR}/contrib/poco/Foundation/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -DREADLINE_NG -D_DEBUG -Icontrib/poco/Foundation/include -Icontrib/linenoise-ng/include -Icontrib/Stackless/Stackless/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/contrib/poco/Foundation/src/Timezone.o contrib/poco/Foundation/src/Timezone.cpp

${OBJECTDIR}/contrib/poco/Foundation/src/UTF16Encoding.o: contrib/poco/Foundation/src/UTF16Encoding.cpp
	${MKDIR} -p ${OBJECTDIR}/contrib/poco/Foundation/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -DREADLINE_NG -D_DEBUG -Icontrib/poco/Foundation/include -Icontrib/linenoise-ng/include -Icontrib/Stackless/Stackless/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/contrib/poco/Foundation/src/UTF16Encoding.o contrib/poco/Foundation/src/UTF16Encoding.cpp

${OBJECTDIR}/contrib/poco/Foundation/src/UTF32Encoding.o: contrib/poco/Foundation/src/UTF32Encoding.cpp
	${MKDIR} -p ${OBJECTDIR}/contrib/poco/Foundation/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -DREADLINE_NG -D_DEBUG -Icontrib/poco/Foundation/include -Icontrib/linenoise-ng/include -Icontrib/Stackless/Stackless/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/contrib/poco/Foundation/src/UTF32Encoding.o contrib/poco/Foundation/src/UTF32Encoding.cpp

${OBJECTDIR}/contrib/poco/Foundation/src/UTF8Encoding.o: contrib/poco/Foundation/src/UTF8Encoding.cpp
	${MKDIR} -p ${OBJECTDIR}/contrib/poco/Foundation/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -DREADLINE_NG -D_DEBUG -Icontrib/poco/Foundation/include -Icontrib/linenoise-ng/include -Icontrib/Stackless/Stackless/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/contrib/poco/Foundation/src/UTF8Encoding.o contrib/poco/Foundation/src/UTF8Encoding.cpp

${OBJECTDIR}/contrib/poco/Foundation/src/UTF8String.o: contrib/poco/Foundation/src/UTF8String.cpp
	${MKDIR} -p ${OBJECTDIR}/contrib/poco/Foundation/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -DREADLINE_NG -D_DEBUG -Icontrib/poco/Foundation/include -Icontrib/linenoise-ng/include -Icontrib/Stackless/Stackless/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/contrib/poco/Foundation/src/UTF8String.o contrib/poco/Foundation/src/UTF8String.cpp

${OBJECTDIR}/contrib/poco/Foundation/src/Unicode.o: contrib/poco/Foundation/src/Unicode.cpp
	${MKDIR} -p ${OBJECTDIR}/contrib/poco/Foundation/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -DREADLINE_NG -D_DEBUG -Icontrib/poco/Foundation/include -Icontrib/linenoise-ng/include -Icontrib/Stackless/Stackless/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/contrib/poco/Foundation/src/Unicode.o contrib/poco/Foundation/src/Unicode.cpp

${OBJECTDIR}/contrib/poco/Foundation/src/UnicodeConverter.o: contrib/poco/Foundation/src/UnicodeConverter.cpp
	${MKDIR} -p ${OBJECTDIR}/contrib/poco/Foundation/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -DREADLINE_NG -D_DEBUG -Icontrib/poco/Foundation/include -Icontrib/linenoise-ng/include -Icontrib/Stackless/Stackless/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/contrib/poco/Foundation/src/UnicodeConverter.o contrib/poco/Foundation/src/UnicodeConverter.cpp

${OBJECTDIR}/contrib/poco/Foundation/src/Var.o: contrib/poco/Foundation/src/Var.cpp
	${MKDIR} -p ${OBJECTDIR}/contrib/poco/Foundation/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -DREADLINE_NG -D_DEBUG -Icontrib/poco/Foundation/include -Icontrib/linenoise-ng/include -Icontrib/Stackless/Stackless/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/contrib/poco/Foundation/src/Var.o contrib/poco/Foundation/src/Var.cpp

${OBJECTDIR}/contrib/poco/Foundation/src/VarHolder.o: contrib/poco/Foundation/src/VarHolder.cpp
	${MKDIR} -p ${OBJECTDIR}/contrib/poco/Foundation/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -DREADLINE_NG -D_DEBUG -Icontrib/poco/Foundation/include -Icontrib/linenoise-ng/include -Icontrib/Stackless/Stackless/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/contrib/poco/Foundation/src/VarHolder.o contrib/poco/Foundation/src/VarHolder.cpp

${OBJECTDIR}/contrib/poco/Foundation/src/VarIterator.o: contrib/poco/Foundation/src/VarIterator.cpp
	${MKDIR} -p ${OBJECTDIR}/contrib/poco/Foundation/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -DREADLINE_NG -D_DEBUG -Icontrib/poco/Foundation/include -Icontrib/linenoise-ng/include -Icontrib/Stackless/Stackless/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/contrib/poco/Foundation/src/VarIterator.o contrib/poco/Foundation/src/VarIterator.cpp

${OBJECTDIR}/contrib/poco/Foundation/src/Windows1250Encoding.o: contrib/poco/Foundation/src/Windows1250Encoding.cpp
	${MKDIR} -p ${OBJECTDIR}/contrib/poco/Foundation/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -DREADLINE_NG -D_DEBUG -Icontrib/poco/Foundation/include -Icontrib/linenoise-ng/include -Icontrib/Stackless/Stackless/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/contrib/poco/Foundation/src/Windows1250Encoding.o contrib/poco/Foundation/src/Windows1250Encoding.cpp

${OBJECTDIR}/contrib/poco/Foundation/src/Windows1251Encoding.o: contrib/poco/Foundation/src/Windows1251Encoding.cpp
	${MKDIR} -p ${OBJECTDIR}/contrib/poco/Foundation/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -DREADLINE_NG -D_DEBUG -Icontrib/poco/Foundation/include -Icontrib/linenoise-ng/include -Icontrib/Stackless/Stackless/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/contrib/poco/Foundation/src/Windows1251Encoding.o contrib/poco/Foundation/src/Windows1251Encoding.cpp

${OBJECTDIR}/contrib/poco/Foundation/src/Windows1252Encoding.o: contrib/poco/Foundation/src/Windows1252Encoding.cpp
	${MKDIR} -p ${OBJECTDIR}/contrib/poco/Foundation/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -DREADLINE_NG -D_DEBUG -Icontrib/poco/Foundation/include -Icontrib/linenoise-ng/include -Icontrib/Stackless/Stackless/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/contrib/poco/Foundation/src/Windows1252Encoding.o contrib/poco/Foundation/src/Windows1252Encoding.cpp

${OBJECTDIR}/contrib/poco/Foundation/src/pcre_byte_order.o: contrib/poco/Foundation/src/pcre_byte_order.c
	${MKDIR} -p ${OBJECTDIR}/contrib/poco/Foundation/src
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/contrib/poco/Foundation/src/pcre_byte_order.o contrib/poco/Foundation/src/pcre_byte_order.c

${OBJECTDIR}/contrib/poco/Foundation/src/pcre_chartables.o: contrib/poco/Foundation/src/pcre_chartables.c
	${MKDIR} -p ${OBJECTDIR}/contrib/poco/Foundation/src
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/contrib/poco/Foundation/src/pcre_chartables.o contrib/poco/Foundation/src/pcre_chartables.c

${OBJECTDIR}/contrib/poco/Foundation/src/pcre_compile.o: contrib/poco/Foundation/src/pcre_compile.c
	${MKDIR} -p ${OBJECTDIR}/contrib/poco/Foundation/src
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/contrib/poco/Foundation/src/pcre_compile.o contrib/poco/Foundation/src/pcre_compile.c

${OBJECTDIR}/contrib/poco/Foundation/src/pcre_config.o: contrib/poco/Foundation/src/pcre_config.c
	${MKDIR} -p ${OBJECTDIR}/contrib/poco/Foundation/src
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/contrib/poco/Foundation/src/pcre_config.o contrib/poco/Foundation/src/pcre_config.c

${OBJECTDIR}/contrib/poco/Foundation/src/pcre_dfa_exec.o: contrib/poco/Foundation/src/pcre_dfa_exec.c
	${MKDIR} -p ${OBJECTDIR}/contrib/poco/Foundation/src
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/contrib/poco/Foundation/src/pcre_dfa_exec.o contrib/poco/Foundation/src/pcre_dfa_exec.c

${OBJECTDIR}/contrib/poco/Foundation/src/pcre_exec.o: contrib/poco/Foundation/src/pcre_exec.c
	${MKDIR} -p ${OBJECTDIR}/contrib/poco/Foundation/src
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/contrib/poco/Foundation/src/pcre_exec.o contrib/poco/Foundation/src/pcre_exec.c

${OBJECTDIR}/contrib/poco/Foundation/src/pcre_fullinfo.o: contrib/poco/Foundation/src/pcre_fullinfo.c
	${MKDIR} -p ${OBJECTDIR}/contrib/poco/Foundation/src
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/contrib/poco/Foundation/src/pcre_fullinfo.o contrib/poco/Foundation/src/pcre_fullinfo.c

${OBJECTDIR}/contrib/poco/Foundation/src/pcre_get.o: contrib/poco/Foundation/src/pcre_get.c
	${MKDIR} -p ${OBJECTDIR}/contrib/poco/Foundation/src
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/contrib/poco/Foundation/src/pcre_get.o contrib/poco/Foundation/src/pcre_get.c

${OBJECTDIR}/contrib/poco/Foundation/src/pcre_globals.o: contrib/poco/Foundation/src/pcre_globals.c
	${MKDIR} -p ${OBJECTDIR}/contrib/poco/Foundation/src
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/contrib/poco/Foundation/src/pcre_globals.o contrib/poco/Foundation/src/pcre_globals.c

${OBJECTDIR}/contrib/poco/Foundation/src/pcre_jit_compile.o: contrib/poco/Foundation/src/pcre_jit_compile.c
	${MKDIR} -p ${OBJECTDIR}/contrib/poco/Foundation/src
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/contrib/poco/Foundation/src/pcre_jit_compile.o contrib/poco/Foundation/src/pcre_jit_compile.c

${OBJECTDIR}/contrib/poco/Foundation/src/pcre_maketables.o: contrib/poco/Foundation/src/pcre_maketables.c
	${MKDIR} -p ${OBJECTDIR}/contrib/poco/Foundation/src
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/contrib/poco/Foundation/src/pcre_maketables.o contrib/poco/Foundation/src/pcre_maketables.c

${OBJECTDIR}/contrib/poco/Foundation/src/pcre_newline.o: contrib/poco/Foundation/src/pcre_newline.c
	${MKDIR} -p ${OBJECTDIR}/contrib/poco/Foundation/src
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/contrib/poco/Foundation/src/pcre_newline.o contrib/poco/Foundation/src/pcre_newline.c

${OBJECTDIR}/contrib/poco/Foundation/src/pcre_ord2utf8.o: contrib/poco/Foundation/src/pcre_ord2utf8.c
	${MKDIR} -p ${OBJECTDIR}/contrib/poco/Foundation/src
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/contrib/poco/Foundation/src/pcre_ord2utf8.o contrib/poco/Foundation/src/pcre_ord2utf8.c

${OBJECTDIR}/contrib/poco/Foundation/src/pcre_refcount.o: contrib/poco/Foundation/src/pcre_refcount.c
	${MKDIR} -p ${OBJECTDIR}/contrib/poco/Foundation/src
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/contrib/poco/Foundation/src/pcre_refcount.o contrib/poco/Foundation/src/pcre_refcount.c

${OBJECTDIR}/contrib/poco/Foundation/src/pcre_string_utils.o: contrib/poco/Foundation/src/pcre_string_utils.c
	${MKDIR} -p ${OBJECTDIR}/contrib/poco/Foundation/src
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/contrib/poco/Foundation/src/pcre_string_utils.o contrib/poco/Foundation/src/pcre_string_utils.c

${OBJECTDIR}/contrib/poco/Foundation/src/pcre_study.o: contrib/poco/Foundation/src/pcre_study.c
	${MKDIR} -p ${OBJECTDIR}/contrib/poco/Foundation/src
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/contrib/poco/Foundation/src/pcre_study.o contrib/poco/Foundation/src/pcre_study.c

${OBJECTDIR}/contrib/poco/Foundation/src/pcre_tables.o: contrib/poco/Foundation/src/pcre_tables.c
	${MKDIR} -p ${OBJECTDIR}/contrib/poco/Foundation/src
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/contrib/poco/Foundation/src/pcre_tables.o contrib/poco/Foundation/src/pcre_tables.c

${OBJECTDIR}/contrib/poco/Foundation/src/pcre_ucd.o: contrib/poco/Foundation/src/pcre_ucd.c
	${MKDIR} -p ${OBJECTDIR}/contrib/poco/Foundation/src
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/contrib/poco/Foundation/src/pcre_ucd.o contrib/poco/Foundation/src/pcre_ucd.c

${OBJECTDIR}/contrib/poco/Foundation/src/pcre_valid_utf8.o: contrib/poco/Foundation/src/pcre_valid_utf8.c
	${MKDIR} -p ${OBJECTDIR}/contrib/poco/Foundation/src
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/contrib/poco/Foundation/src/pcre_valid_utf8.o contrib/poco/Foundation/src/pcre_valid_utf8.c

${OBJECTDIR}/contrib/poco/Foundation/src/pcre_version.o: contrib/poco/Foundation/src/pcre_version.c
	${MKDIR} -p ${OBJECTDIR}/contrib/poco/Foundation/src
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/contrib/poco/Foundation/src/pcre_version.o contrib/poco/Foundation/src/pcre_version.c

${OBJECTDIR}/contrib/poco/Foundation/src/pcre_xclass.o: contrib/poco/Foundation/src/pcre_xclass.c
	${MKDIR} -p ${OBJECTDIR}/contrib/poco/Foundation/src
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/contrib/poco/Foundation/src/pcre_xclass.o contrib/poco/Foundation/src/pcre_xclass.c

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
