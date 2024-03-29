#*****************************************************************************
#* pce                                                                       *
#*****************************************************************************

#*****************************************************************************
#* File name:   configure.ac                                                 *
#* Created:     2002-05-20 by Hampa Hug <hampa@hampa.ch>                     *
#* Copyright:   (C) 2002-2021 Hampa Hug <hampa@hampa.ch>                     *
#*****************************************************************************

#*****************************************************************************
#* This program is free software. You can redistribute it and / or modify it *
#* under the terms of the GNU General Public License version 2 as  published *
#* by the Free Software Foundation.                                          *
#*                                                                           *
#* This program is distributed in the hope  that  it  will  be  useful,  but *
#* WITHOUT  ANY   WARRANTY,   without   even   the   implied   warranty   of *
#* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU  General *
#* Public License for more details.                                          *
#*****************************************************************************


AC_PREREQ(2.64)

AC_INIT([pce])
AC_CONFIG_SRCDIR([Makefile.in])
AC_CONFIG_HEADERS([src/config.h])


#-----------------------------------------------------------------------------
# package version

AC_MSG_CHECKING([pce version])
if test ! -r "$srcdir/version" ; then
	AC_MSG_RESULT([file 'version' not found in source directory])
	exit 1
fi

read p PCE_VERSION_MAJ PCE_VERSION_MIN PCE_VERSION_MIC dstr r < "$srcdir/version"

PCE_VERSION_STR=$dstr

if test "x$dstr" = "xscm" ; then
	if test -d "$srcdir/.git" ; then
		tmp=gitlog.$$.tmp
		( cd "$srcdir" && git log -1 --date=iso --abbrev=8 --pretty="format:%h %cd" HEAD ) > "$tmp" 2> /dev/null
		read hash date time rest < "$tmp"
		rm -f "$tmp"
		date=`echo "$date" | sed -e "s/-//g"`
		dirty=`cd "$srcdir" && git describe --tags --dirty --always 2> /dev/null`
		case "$dirty" in
		*-dirty)
			dirty=-mod
			;;
		*)
			dirty=""
			;;
		esac
		PCE_VERSION_STR="$date-$hash$dirty"
	fi
elif test "x$dstr" = "x" ; then
	PCE_VERSION_STR="$PCE_VERSION_MAJ.$PCE_VERSION_MIN.$PCE_VERSION_MIC"
fi
AC_SUBST(PCE_VERSION_MAJ)
AC_SUBST(PCE_VERSION_MIN)
AC_SUBST(PCE_VERSION_MIC)
AC_SUBST(PCE_VERSION_STR)
AC_DEFINE_UNQUOTED(PCE_VERSION_MAJ, $PCE_VERSION_MAJ)
AC_DEFINE_UNQUOTED(PCE_VERSION_MIN, $PCE_VERSION_MIN)
AC_DEFINE_UNQUOTED(PCE_VERSION_MIC, $PCE_VERSION_MIC)
AC_DEFINE_UNQUOTED(PCE_VERSION_STR, "$PCE_VERSION_STR")
AC_MSG_RESULT([$PCE_VERSION_STR ($PCE_VERSION_MAJ.$PCE_VERSION_MIN.$PCE_VERSION_MIC)])


#-----------------------------------------------------------------------------
# host

AC_CANONICAL_HOST

AC_MSG_CHECKING([host OS])
PCE_HOST_LINUX=0
PCE_HOST_WINDOWS=0
PCE_HOST_SUNOS=0
PCE_HOST_NETBSD=0
PCE_HOST_MACOS=0
case "$host_os" in
[[Ll]]inux*)
	PCE_HOST_LINUX=1
	AC_DEFINE(PCE_HOST_LINUX)
	AC_MSG_RESULT([Linux])
	;;
mingw*)
	PCE_HOST_WINDOWS=1
	AC_DEFINE(PCE_HOST_WINDOWS)
	AC_MSG_RESULT([Windows])
	;;
SunOS*)
	PCE_HOST_SUNOS=1
	AC_DEFINE(PCE_HOST_SUNOS)
	AC_MSG_RESULT([SunOS])
	;;
netbsd*)
	PCE_HOST_NETBSD=1
	AC_DEFINE(PCE_HOST_NETBSD)
	AC_MSG_RESULT([NetBSD])
	;;
darwin*)
	PCE_HOST_MACOS=1
	AC_DEFINE(PCE_HOST_MACOS)
	AC_MSG_RESULT([macOS])
	;;
*)
	AC_MSG_RESULT([unknown ($host_os)])
	;;
esac
AC_SUBST(PCE_HOST_LINUX)
AC_SUBST(PCE_HOST_WINDOWS)
AC_SUBST(PCE_HOST_SUNOS)
AC_SUBST(PCE_HOST_NETBSD)
AC_SUBST(PCE_HOST_MACOS)

AC_MSG_CHECKING([host cpu])
PCE_HOST_IA32=0
PCE_HOST_PPC=0
PCE_HOST_SPARC=0
case "$host_cpu" in
i?86)
	PCE_HOST_IA32=1
	AC_DEFINE(PCE_HOST_IA32)
	AC_MSG_RESULT([IA32])
	;;
powerpc*)
	PCE_HOST_PPC=1
	AC_DEFINE(PCE_HOST_PPC)
	AC_MSG_RESULT([powerpc])
	;;
sparc* | sun4*)
	PCE_HOST_SPARC=1
	AC_DEFINE(PCE_HOST_SPARC)
	AC_MSG_RESULT([sparc])
	;;
*)
	AC_MSG_RESULT([unknown ($host_cpu)])
	;;
esac
AC_SUBST(PCE_HOST_IA32)
AC_SUBST(PCE_HOST_PPC)
AC_SUBST(PCE_HOST_SPARC)


#-----------------------------------------------------------------------------
# Checks for programs

AC_PROG_CC
AC_PROG_INSTALL
case "$INSTALL" in
.*)
	d=`dirname "$INSTALL"`
	f=`basename "$INSTALL"`
	INSTALL=`cd "$d" && pwd`/"$f"
	;;
esac
AC_PATH_PROG(AR, ar, ar)
AC_PROG_RANLIB
AC_PROG_MAKE_SET
AC_PROG_LN_S

AC_PATH_PROG(NASM, nasm,)
if test -x "$NASM" ; then
	PCE_HAVE_NASM=1
else
	PCE_HAVE_NASM=0
fi
AC_SUBST(PCE_HAVE_NASM)

AC_PATH_PROG(IHEX, ihex,)
if test -x "$IHEX" ; then
	PCE_HAVE_IHEX=1
else
	PCE_HAVE_IHEX=0
fi
AC_SUBST(PCE_HAVE_IHEX)


#-----------------------------------------------------------------------------
# Checks for header files.

AC_CHECK_HEADERS( \
	arpa/inet.h \
	fcntl.h \
	linux/if_tun.h \
	linux/tcp.h \
	limits.h \
	netdb.h \
	netinet/in.h \
	poll.h \
	sys/ioctl.h \
	sys/poll.h \
	sys/socket.h \
	sys/soundcard.h \
	sys/stat.h \
	sys/time.h \
	sys/types.h \
	termios.h \
	unistd.h \
	vmnet/vmnet.h
)

AC_CHECK_HEADER(stdint.h,
	[AC_DEFINE(HAVE_STDINT_H)
	PCE_HAVE_STDINT_H=1],
	PCE_HAVE_STDINT_H=0
)
AC_SUBST(PCE_HAVE_STDINT_H)

AC_CHECK_HEADER(inttypes.h,
	[AC_DEFINE(HAVE_INTTYPES_H)
	PCE_HAVE_INTTYPES_H=1],
	PCE_HAVE_INTTYPES_H=0
)
AC_SUBST(PCE_HAVE_INTTYPES_H)


#-----------------------------------------------------------------------------
# Checks for libraries

AC_FUNC_FSEEKO
AC_CHECK_FUNCS(ftruncate futimes gettimeofday nanosleep sleep usleep)

AC_SEARCH_LIBS(socket, socket)
AC_SEARCH_LIBS(accept, socket)
AC_SEARCH_LIBS(gethostbyname, nsl resolv socket)
AC_SEARCH_LIBS(inet_aton, nsl resolv socket)

AC_PATH_X
if test "x$no_x" = "xyes" ; then
	PCE_ENABLE_X11=0
	PCE_X11_CFLAGS=""
	PCE_X11_LIBS=""
else
	PCE_ENABLE_X11=1
	if test -n "$x_includes" ; then
		PCE_X11_CFLAGS="-I$x_includes"
	else
		PCE_X11_CFLAGS=""
	fi
	if test -n "$x_libraries" ; then
		PCE_X11_LIBS="-L$x_libraries -lX11"
	else
		PCE_X11_LIBS="-lX11"
	fi
	AC_DEFINE(PCE_ENABLE_X11)
fi
AC_SUBST(PCE_ENABLE_X11)
AC_SUBST(PCE_X11_CFLAGS)
AC_SUBST(PCE_X11_LIBS)


PCE_ENABLE_SDL=0
PCE_ENABLE_SDL1=0
PCE_ENABLE_SDL2=0
AC_MSG_CHECKING([for SDL])
AC_ARG_WITH(sdl,
  [AS_HELP_STRING([--with-sdl], [Build the SDL terminal [detected]])],
  [
	case "$withval" in
	"no")
		AC_MSG_RESULT([no (forced)])
		;;
	"1")
		PCE_ENABLE_SDL=1
		PCE_ENABLE_SDL1=1
		AC_MSG_RESULT([1 (forced)])
		;;
	"2")
		PCE_ENABLE_SDL=1
		PCE_ENABLE_SDL2=1
		AC_MSG_RESULT([2 (forced)])
		;;
	esac
  ],
  [
	if sdl2-config --version > /dev/null 2>&1 ; then
		PCE_ENABLE_SDL=1
		PCE_ENABLE_SDL2=1
		AC_MSG_RESULT([2 (detected)])
	elif sdl-config --version > /dev/null 2>&1 ; then
		PCE_ENABLE_SDL=1
		PCE_ENABLE_SDL1=1
		AC_MSG_RESULT([1 (detected)])
	else
		AC_MSG_RESULT([no (detected)])
	fi
  ]
)
if test "x$PCE_ENABLE_SDL1" != "x0" ; then
	PCE_SDL_CFLAGS=`sdl-config --cflags`
	PCE_SDL_LIBS=`sdl-config --libs`
	AC_DEFINE(PCE_ENABLE_SDL)
	AC_DEFINE(PCE_ENABLE_SDL1)
elif test "x$PCE_ENABLE_SDL2" != "x0" ; then
	PCE_SDL_CFLAGS=`sdl2-config --cflags`
	PCE_SDL_LIBS=`sdl2-config --libs`
	AC_DEFINE(PCE_ENABLE_SDL)
	AC_DEFINE(PCE_ENABLE_SDL2)
fi
AC_SUBST(PCE_ENABLE_SDL)
AC_SUBST(PCE_ENABLE_SDL1)
AC_SUBST(PCE_ENABLE_SDL2)
AC_SUBST(PCE_SDL_CFLAGS)
AC_SUBST(PCE_SDL_LIBS)


#-----------------------------------------------------------------------------
# Checks for options

AC_ARG_ENABLE(largefile,
  [AS_HELP_STRING([--disable-largefile], [Disable large file support [no]])],
  opt_large=$enableval
)
if test "$opt_large" = "no" ; then
	PCE_LARGE_FILE="0"
else
	AC_DEFINE(PCE_LARGE_FILE)
	PCE_LARGE_FILE="1"
fi
AC_SUBST(PCE_LARGE_FILE)


if test -d "$srcdir/src/arch/ibmpc"; then have_ibmpc=1; else have_ibmpc=0; fi
if test -d "$srcdir/src/arch/macplus"; then have_macplus=1; else have_macplus=0; fi
if test -d "$srcdir/src/arch/rc759"; then have_rc759=1; else have_rc759=0; fi
if test -d "$srcdir/src/arch/sim405"; then have_sim405=1; else have_sim405=0; fi
if test -d "$srcdir/src/arch/simarm"; then have_simarm=1; else have_simarm=0; fi
if test -d "$srcdir/src/arch/sims32"; then have_sims32=1; else have_sims32=0; fi
if test -d "$srcdir/src/arch/vic20"; then have_vic20=1; else have_vic20=0; fi


AC_MSG_CHECKING([whether to build the Atari ST emulator])
AC_ARG_ENABLE(atari-st,
	[AS_HELP_STRING([--enable-atari-st], [Enable the Atari ST emulator [yes]])],
	[
		if test "x$enableval" = "xyes" ; then
			PCE_BUILD_ATARIST=1
			AC_MSG_RESULT([yes (forced)])
		else
			PCE_BUILD_ATARIST=0
			AC_MSG_RESULT([no (forced)])
		fi
	],
	[
		PCE_BUILD_ATARIST=1
		AC_MSG_RESULT([yes (default)])
	]
)
AC_SUBST(PCE_BUILD_ATARIST)
if test $PCE_BUILD_ATARIST -eq 1 ; then
	AC_DEFINE(PCE_BUILD_ATARIST)
fi


AC_MSG_CHECKING([whether to build the CPM-80 emulator])
AC_ARG_ENABLE(cpm80,
  [AS_HELP_STRING([--enable-cpm80], [Enable the CPM-80 emulator [yes]])],
  [
	if test "x$enableval" = "xyes" ; then
		PCE_BUILD_CPM80=1
		AC_MSG_RESULT([yes (forced)])
	else
		PCE_BUILD_CPM80=0
		AC_MSG_RESULT([no (forced)])
	fi
  ],
  [
	PCE_BUILD_CPM80=1
	AC_MSG_RESULT([yes (default)])
  ]
)
AC_SUBST(PCE_BUILD_CPM80)
if test "x$PCE_BUILD_CPM80" = "x1" ; then
	AC_DEFINE(PCE_BUILD_CPM80)
fi


AC_MSG_CHECKING([whether to build the IBM PC emulator])
if test "x$have_ibmpc" != "x1" ; then
	PCE_BUILD_IBMPC=0
	AC_MSG_RESULT([no (no source)])
else
	AC_ARG_ENABLE(ibmpc,
	  [AS_HELP_STRING([--enable-ibmpc], [Enable the IBM PC emulator [yes]])],
	  [
		if test "x$enableval" = "xyes" ; then
			PCE_BUILD_IBMPC=1
			AC_MSG_RESULT([yes (forced)])
		else
			PCE_BUILD_IBMPC=0
			AC_MSG_RESULT([no (forced)])
		fi
	  ],
	  [
		PCE_BUILD_IBMPC=1
		AC_MSG_RESULT([yes (default)])
	  ]
	)
fi
AC_SUBST(PCE_BUILD_IBMPC)
if test $PCE_BUILD_IBMPC -eq 1 ; then
	AC_DEFINE(PCE_BUILD_IBMPC)
fi


AC_MSG_CHECKING([whether to build the IBM PC ROM])
AC_ARG_ENABLE(ibmpc-rom,
	[AS_HELP_STRING([--enable-ibmpc-rom], [Build the IBM PC ROM from source [no]])],
	[
		if test "x$enableval" = "xyes" ; then
			PCE_BUILD_IBMPC_ROM=1
			AC_MSG_RESULT([yes])
		else
			PCE_BUILD_IBMPC_ROM=0
			AC_MSG_RESULT([no])
		fi
	],
	[
		PCE_BUILD_IBMPC_ROM=0
		AC_MSG_RESULT([no (default)])
	]
)
AC_SUBST(PCE_BUILD_IBMPC_ROM)


AC_MSG_CHECKING([whether to build the Mac Plus emulator])
if test "x$have_macplus" != "x1" ; then
	PCE_BUILD_MACPLUS=0
	AC_MSG_RESULT([no (no source)])
else
	AC_ARG_ENABLE(macplus,
	  [AS_HELP_STRING([--enable-macplus], [Enable the Mac Plus emulator [yes]])],
	  [
		if test "x$enableval" = "xyes" ; then
			PCE_BUILD_MACPLUS=1
			AC_MSG_RESULT([yes (forced)])
		else
			PCE_BUILD_MACPLUS=0
			AC_MSG_RESULT([no (forced)])
		fi
	  ],
	  [
		PCE_BUILD_MACPLUS=1
		AC_MSG_RESULT([yes (default)])
	  ]
	)
fi
AC_SUBST(PCE_BUILD_MACPLUS)
if test $PCE_BUILD_MACPLUS -eq 1 ; then
	AC_DEFINE(PCE_BUILD_MACPLUS)
fi


AC_MSG_CHECKING([whether to build the Mac Plus ROM])
AC_ARG_ENABLE(macplus-rom,
	[AS_HELP_STRING([--enable-macplus-rom], [Build the Mac Plus ROM from source [no]])],
	[
		if test "x$enableval" = "xyes" ; then
			PCE_BUILD_MACPLUS_ROM=1
			AC_MSG_RESULT([yes])
		else
			PCE_BUILD_MACPLUS_ROM=0
			AC_MSG_RESULT([no])
		fi
	],
	[
		PCE_BUILD_MACPLUS_ROM=0
		AC_MSG_RESULT([no (default)])
	]
)
AC_SUBST(PCE_BUILD_MACPLUS_ROM)


AC_MSG_CHECKING([whether to build the RC759 Piccoline])
if test "x$have_rc759" != "x1" ; then
	PCE_BUILD_RC759=0
	AC_MSG_RESULT([no (no source)])
else
	AC_ARG_ENABLE(rc759,
	  [AS_HELP_STRING([--enable-rc759], [Enable the RC759 Piccoline [yes]])],
	  [
		if test "x$enableval" = "xyes" ; then
			PCE_BUILD_RC759=1
			AC_MSG_RESULT([yes (forced)])
		else
			PCE_BUILD_RC759=0
			AC_MSG_RESULT([no (forced)])
		fi
	  ],
	  [
		PCE_BUILD_RC759=1
		AC_MSG_RESULT([yes (default)])
	  ]
	)
fi
AC_SUBST(PCE_BUILD_RC759)
if test $PCE_BUILD_RC759 -eq 1 ; then
	AC_DEFINE(PCE_BUILD_RC759)
fi


AC_MSG_CHECKING([whether to build the PPC405 simulator])
if test "x$have_sim405" != "x1" ; then
	PCE_BUILD_SIM405=0
	AC_MSG_RESULT([no (no source)])
else
	AC_ARG_ENABLE(sim405,
	  [AS_HELP_STRING([--enable-sim405], [Enable the PPC405 simulator [yes]])],
	  [
		if test "x$enableval" = "xyes" ; then
			PCE_BUILD_SIM405=1
			AC_MSG_RESULT([yes (forced)])
		else
			PCE_BUILD_SIM405=0
			AC_MSG_RESULT([no (forced)])
		fi
	  ],
	  [
	  	PCE_BUILD_SIM405=1
		AC_MSG_RESULT([yes (default)])
	  ]
	)
fi
AC_SUBST(PCE_BUILD_SIM405)
if test "x$PCE_BUILD_SIM405" = "x1" ; then
	AC_DEFINE(PCE_BUILD_SIM405)
fi


AC_MSG_CHECKING([whether to build the sparc32 simulator])
if test "x$have_sims32" != "x1" ; then
	PCE_BUILD_SIMS32=0
	AC_MSG_RESULT([no (no source)])
else
	AC_ARG_ENABLE(sims32,
	  [AS_HELP_STRING([--enable-sims32], [Enable the sparc32 simulator [yes]])],
	  [
		if test "x$enableval" = "xyes" ; then
			PCE_BUILD_SIMS32=1
			AC_MSG_RESULT([yes (forced)])
		else
			PCE_BUILD_SIMS32=0
			AC_MSG_RESULT([no (forced)])
		fi
	  ],
	  [
		PCE_BUILD_SIMS32=1
		AC_MSG_RESULT([yes (default)])
	  ]
	)
fi
AC_SUBST(PCE_BUILD_SIMS32)
if test "x$PCE_BUILD_SIMS32" = "x1" ; then
	AC_DEFINE(PCE_BUILD_SIMS32)
fi


AC_MSG_CHECKING([whether to build the arm simulator])
if test "x$have_simarm" != "x1" ; then
	PCE_BUILD_SIMARM=0
	AC_MSG_RESULT([no (no source)])
else
	AC_ARG_ENABLE(simarm,
	  [AS_HELP_STRING([--enable-simarm], [Enable the arm simulator [yes]])],
	  [
		if test "x$enableval" = "xyes" ; then
			PCE_BUILD_SIMARM=1
			AC_MSG_RESULT([yes (forced)])
		else
			PCE_BUILD_SIMARM=0
			AC_MSG_RESULT([no (forced)])
		fi
	  ],
	  [
		PCE_BUILD_SIMARM=1
		AC_MSG_RESULT([yes (default)])
	  ]
	)
fi
AC_SUBST(PCE_BUILD_SIMARM)
if test "x$PCE_BUILD_SIMARM" = "x1" ; then
	AC_DEFINE(PCE_BUILD_SIMARM)
fi


AC_MSG_CHECKING([whether to build the VIC20 emulator])
AC_ARG_ENABLE(vic20,
  [AS_HELP_STRING([--enable-vic20], [Enable the VIC20 emulator [yes]])],
  [
	if test "x$enableval" = "xyes" ; then
		PCE_BUILD_VIC20=1
		AC_MSG_RESULT([yes (forced)])
	else
		PCE_BUILD_VIC20=0
		AC_MSG_RESULT([no (forced)])
	fi
  ],
  [
	PCE_BUILD_VIC20=1
	AC_MSG_RESULT([yes (default)])
  ]
)
AC_SUBST(PCE_BUILD_VIC20)
if test $PCE_BUILD_VIC20 -eq 1 ; then
	AC_DEFINE(PCE_BUILD_VIC20)
fi


AC_MSG_CHECKING([whether to enable tun])
AC_ARG_ENABLE(tun,
  [AS_HELP_STRING([--enable-tun], [Enable tun networking [guessed]])],
  [
	if test "x$enableval" = "xyes" ; then
		PCE_ENABLE_TUN=1
		AC_MSG_RESULT([yes])
	else
		PCE_ENABLE_TUN=0
		AC_MSG_RESULT([no])
 	fi
  ],
  [
	h1=$ac_cv_header_linux_if_tun_h
	h2=$ac_cv_header_sys_ioctl_h
	h3=$ac_cv_header_sys_poll_h
	h4=$ac_cv_header_sys_socket_h
	if test "x$h1" = "xyes" -a "x$h2" = "xyes" -a "x$h3" = "xyes" -a "x$h4" = "xyes" ; then
		PCE_ENABLE_TUN=1
		AC_MSG_RESULT([yes (guess)])
	else
		PCE_ENABLE_TUN=0
		AC_MSG_RESULT([no (guess)])
	fi
  ]
)
AC_SUBST(PCE_ENABLE_TUN)
if test "x$PCE_ENABLE_TUN" = "x1" ; then
	AC_DEFINE(PCE_ENABLE_TUN)
fi


AC_MSG_CHECKING([whether to enable vmnet])
AC_ARG_ENABLE(vmnet,
  [AS_HELP_STRING([--enable-vmnet], [Enable vmnet networking [guessed]])],
  [
	if test "x$enableval" = "xyes" ; then
		PCE_ENABLE_VMNET=1
		AC_MSG_RESULT([yes])
	else
		PCE_ENABLE_VMNET=0
		AC_MSG_RESULT([no])
 	fi
  ],[
	PCE_ENABLE_VMNET=1
	test "x$ac_cv_header_vmnet_vmnet_h" = "xyes" || PCE_ENABLE_VMNET=0
	if test "x$PCE_ENABLE_VMNET" = "x1" ; then
		AC_MSG_RESULT([yes (guess)])
	else
		AC_MSG_RESULT([no (guess)])
	fi
  ]
)
AC_SUBST(PCE_ENABLE_VMNET)
if test "x$PCE_ENABLE_VMNET" = "x1" ; then
	AC_DEFINE(PCE_ENABLE_VMNET)
fi


AC_MSG_CHECKING([whether to enable the posix file character driver])
AC_ARG_ENABLE(char-posix,
  [AS_HELP_STRING([--enable-char-posix], [Enable the posix file character driver [guessed]])],
  [
	if test "x$enableval" = "xyes" ; then
		PCE_ENABLE_CHAR_POSIX=1
		AC_MSG_RESULT([yes])
	else
		PCE_ENABLE_CHAR_POSIX=0
		AC_MSG_RESULT([no])
	fi
  ],[
	PCE_ENABLE_CHAR_POSIX=1
	test "x$ac_cv_header_fcntl_h" = "xyes" || PCE_ENABLE_CHAR_POSIX=0
	test "x$ac_cv_header_poll_h" = "xyes" || PCE_ENABLE_CHAR_POSIX=0
	test "x$ac_cv_header_sys_stat_h" = "xyes" || PCE_ENABLE_CHAR_POSIX=0
	test "x$ac_cv_header_unistd_h" = "xyes" || PCE_ENABLE_CHAR_POSIX=0
	if test "x$PCE_ENABLE_CHAR_POSIX" = "x1" ; then
		AC_MSG_RESULT([yes (guess)])
	else
		AC_MSG_RESULT([no (guess)])
	fi
  ]
)
AC_SUBST(PCE_ENABLE_CHAR_POSIX)
if test "x$PCE_ENABLE_CHAR_POSIX" = "x1" ; then
	AC_DEFINE(PCE_ENABLE_CHAR_POSIX)
fi


AC_MSG_CHECKING([whether to enable the PPP character driver])
AC_ARG_ENABLE(char-ppp,
  [AS_HELP_STRING([--enable-char-ppp], [Enable the PPP character driver [guessed]])],
  [
	if test "x$enableval" = "xyes" ; then
		PCE_ENABLE_CHAR_PPP=1
		AC_MSG_RESULT([yes])
	else
		PCE_ENABLE_CHAR_PPP=0
		AC_MSG_RESULT([no])
	fi
  ],[
  	if test "x$PCE_ENABLE_TUN" = "x1" ; then
	  	PCE_ENABLE_CHAR_PPP=1
		AC_MSG_RESULT([yes (guess)])
	else
	  	PCE_ENABLE_CHAR_PPP=0
		AC_MSG_RESULT([no (guess)])
	fi
  ]
)
AC_SUBST(PCE_ENABLE_CHAR_PPP)
if test "x$PCE_ENABLE_CHAR_PPP" = "x1" ; then
	AC_DEFINE(PCE_ENABLE_CHAR_PPP)
fi


AC_MSG_CHECKING([whether to enable the pty character driver])
AC_ARG_ENABLE(char-pty,
  [AS_HELP_STRING([--enable-char-pty], [Enable the pty character driver [guessed]])],
  [
	if test "x$enableval" = "xyes" ; then
		PCE_ENABLE_CHAR_PTY=1
		AC_MSG_RESULT([yes])
	else
		PCE_ENABLE_CHAR_PTY=0
		AC_MSG_RESULT([no])
	fi
  ],[
	PCE_ENABLE_CHAR_PTY=1
	test "x$ac_cv_header_fcntl_h" = "xyes" || PCE_ENABLE_CHAR_PTY=0
	test "x$ac_cv_header_termios_h" = "xyes" || PCE_ENABLE_CHAR_PTY=0
	test "x$ac_cv_header_unistd_h" = "xyes" || PCE_ENABLE_CHAR_PTY=0
	if test "x$PCE_ENABLE_CHAR_PTY" = "x1" ; then
		AC_MSG_RESULT([yes (guess)])
	else
		AC_MSG_RESULT([no (guess)])
	fi
  ]
)
AC_SUBST(PCE_ENABLE_CHAR_PTY)
if test "x$PCE_ENABLE_CHAR_PTY" = "x1" ; then
	AC_DEFINE(PCE_ENABLE_CHAR_PTY)
fi


AC_MSG_CHECKING([whether to enable the SLIP character driver])
AC_ARG_ENABLE(char-slip,
  [AS_HELP_STRING([--enable-char-slip], [Enable the SLIP character driver [guessed]])],
  [
	if test "x$enableval" = "xyes" ; then
		PCE_ENABLE_CHAR_SLIP=1
		AC_MSG_RESULT([yes])
	else
		PCE_ENABLE_CHAR_SLIP=0
		AC_MSG_RESULT([no])
	fi
  ],[
  	if test "x$PCE_ENABLE_TUN" = "x1" ; then
	  	PCE_ENABLE_CHAR_SLIP=1
		AC_MSG_RESULT([yes (guess)])
	else
	  	PCE_ENABLE_CHAR_SLIP=0
		AC_MSG_RESULT([no (guess)])
	fi
  ]
)
AC_SUBST(PCE_ENABLE_CHAR_SLIP)
if test "x$PCE_ENABLE_CHAR_SLIP" = "x1" ; then
	AC_DEFINE(PCE_ENABLE_CHAR_SLIP)
fi


AC_MSG_CHECKING([whether to enable the TCP character driver])
AC_ARG_ENABLE(char-tcp,
  [AS_HELP_STRING([--enable-char-tcp], [Enable the TCP character driver [guessed]])],
  [
	if test "x$enableval" = "xyes" ; then
		PCE_ENABLE_CHAR_TCP=1
		AC_MSG_RESULT([yes])
	else
		PCE_ENABLE_CHAR_TCP=0
		AC_MSG_RESULT([no])
	fi
  ],[
	PCE_ENABLE_CHAR_TCP=1
	test "x$ac_cv_header_arpa_inet_h" = "xyes" || PCE_ENABLE_CHAR_TCP=0
	test "x$ac_cv_header_fcntl_h" = "xyes" || PCE_ENABLE_CHAR_TIOS=0
	test "x$ac_cv_header_netinet_in_h" = "xyes" || PCE_ENABLE_CHAR_TCP=0
	test "x$ac_cv_header_netdb_h" = "xyes" || PCE_ENABLE_CHAR_TCP=0
	test "x$ac_cv_header_sys_socket_h" = "xyes" || PCE_ENABLE_CHAR_TCP=0
	test "x$ac_cv_header_poll_h" = "xyes" || PCE_ENABLE_CHAR_TCP=0
	test "x$ac_cv_header_unistd_h" = "xyes" || PCE_ENABLE_CHAR_TCP=0
	if test "x$PCE_ENABLE_CHAR_TCP" = "x1" ; then
		AC_MSG_RESULT([yes (guess)])
	else
		AC_MSG_RESULT([no (guess)])
	fi
  ]
)
AC_SUBST(PCE_ENABLE_CHAR_TCP)
if test "x$PCE_ENABLE_CHAR_TCP" = "x1" ; then
	AC_DEFINE(PCE_ENABLE_CHAR_TCP)
fi


AC_MSG_CHECKING([whether to enable the termios character driver])
AC_ARG_ENABLE(char-termios,
  [AS_HELP_STRING([--enable-char-termios], [Enable the termios character driver [guessed]])],
  [
	if test "x$enableval" = "xyes" ; then
		PCE_ENABLE_CHAR_TIOS=1
		AC_MSG_RESULT([yes])
	else
		PCE_ENABLE_CHAR_TIOS=0
		AC_MSG_RESULT([no])
	fi
  ],[
	PCE_ENABLE_CHAR_TIOS=1
	test "x$ac_cv_header_fcntl_h" = "xyes" || PCE_ENABLE_CHAR_TIOS=0
	test "x$ac_cv_header_sys_ioctl_h" = "xyes" || PCE_ENABLE_CHAR_TIOS=0
	test "x$ac_cv_header_sys_poll_h" = "xyes" || PCE_ENABLE_CHAR_TIOS=0
	test "x$ac_cv_header_termios_h" = "xyes" || PCE_ENABLE_CHAR_TIOS=0
	test "x$ac_cv_header_unistd_h" = "xyes" || PCE_ENABLE_CHAR_TIOS=0
	if test "x$PCE_ENABLE_CHAR_TIOS" = "x1" ; then
		AC_MSG_RESULT([yes (guess)])
	else
		AC_MSG_RESULT([no (guess)])
	fi
  ]
)
AC_SUBST(PCE_ENABLE_CHAR_TIOS)
if test "x$PCE_ENABLE_CHAR_TIOS" = "x1" ; then
	AC_DEFINE(PCE_ENABLE_CHAR_TIOS)
fi


AC_MSG_CHECKING([whether to enable the windows COM character driver])
AC_ARG_ENABLE(char-wincom,
  [AS_HELP_STRING([--enable-char-wincom], [Enable the windows COM character driver [guessed]])],
  [
	if test "x$enableval" = "xyes" ; then
		PCE_ENABLE_CHAR_WINCOM=1
		AC_MSG_RESULT([yes])
	else
		PCE_ENABLE_CHAR_WINCOM=0
		AC_MSG_RESULT([no])
	fi
  ],[
	if test "x$PCE_HOST_WINDOWS" = "x1" ; then
		PCE_ENABLE_CHAR_WINCOM=1
		AC_MSG_RESULT([yes (guess)])
	else
		PCE_ENABLE_CHAR_WINCOM=0
		AC_MSG_RESULT([no (guess)])
	fi
  ]
)
AC_SUBST(PCE_ENABLE_CHAR_WINCOM)
if test "x$PCE_ENABLE_CHAR_WINCOM" = "x1" ; then
	AC_DEFINE(PCE_ENABLE_CHAR_WINCOM)
fi


AC_MSG_CHECKING([whether to enable the OSS sound driver])
AC_ARG_ENABLE(sound-oss,
  [AS_HELP_STRING([--enable-sound-oss], [Enable the OSS sound driver driver [guessed]])],
  [
	if test "x$enableval" = "xyes" ; then
		PCE_ENABLE_SOUND_OSS=1
		AC_MSG_RESULT([yes])
	else
		PCE_ENABLE_SOUND_OSS=0
		AC_MSG_RESULT([no])
	fi
  ],[
	PCE_ENABLE_SOUND_OSS=1
	test "x$ac_cv_header_fcntl_h" = "xyes" || PCE_ENABLE_SOUND_OSS=0
	test "x$ac_cv_header_sys_ioctl_h" = "xyes" || PCE_ENABLE_SOUND_OSS=0
	test "x$ac_cv_header_sys_soundcard_h" = "xyes" || PCE_ENABLE_SOUND_OSS=0
	if test "x$PCE_ENABLE_SOUND_OSS" = "x1" ; then
		AC_MSG_RESULT([yes (guess)])
	else
		AC_MSG_RESULT([no (guess)])
	fi
  ]
)
AC_SUBST(PCE_ENABLE_SOUND_OSS)
if test "x$PCE_ENABLE_SOUND_OSS" = "x1" ; then
	AC_DEFINE(PCE_ENABLE_SOUND_OSS)
fi


AC_MSG_CHECKING([whether to enable readline])
AC_ARG_ENABLE(readline,
  [AS_HELP_STRING([--enable-readline], [Enable readline [guessed]])],
  [
	if test "x$enableval" = "xyes" ; then
		PCE_ENABLE_READLINE=1
		PCE_READLINE_LIBS="-lreadline -lhistory -lncurses"
		AC_MSG_RESULT([yes])
	else
		PCE_ENABLE_READLINE=0
		PCE_READLINE_LIBS=""
		AC_MSG_RESULT([no])
	fi
  ],
  [
	AC_MSG_RESULT([maybe])
	AC_CHECK_LIB(ncurses, initscr, ok=1, ok=0)
	if test "x$ok" = "x1" ; then
		AC_CHECK_LIB(readline, rl_bind_key, ok=1, ok=0, -lncurses)
	fi
	if test "x$ok" = "x1" ; then
		AC_CHECK_LIB(history, add_history, ok=1, ok=0)
	fi
	AC_MSG_CHECKING([whether to enable readline])
	if test "x$ok" = "x1" ; then
		PCE_ENABLE_READLINE=1
		PCE_READLINE_LIBS="-lreadline -lhistory -lncurses"
		AC_MSG_RESULT([yes (guess)])
	else
		PCE_ENABLE_READLINE=0
		PCE_READLINE_LIBS=""
		AC_MSG_RESULT([no (guess)])
	fi
  ]
)
AC_SUBST(PCE_ENABLE_READLINE)
AC_SUBST(PCE_READLINE_LIBS)
if test "x$PCE_ENABLE_READLINE" = "x1" ; then
	AC_DEFINE(PCE_ENABLE_READLINE)
fi


eval "pce_etcdir=$sysconfdir"
eval "pce_etcdir=$pce_etcdir"

eval "pce_datdir=$datadir"
eval "pce_datdir=$pce_datdir"

PCE_DIR_ETC=$pce_etcdir
PCE_DIR_DATA=$pce_datdir

AC_SUBST(PCE_DIR_ETC)
AC_SUBST(PCE_DIR_DATA)
AC_DEFINE_UNQUOTED(PCE_DIR_ETC, "$pce_etcdir")
AC_DEFINE_UNQUOTED(PCE_DIR_DATA, "$pce_datdir")


dnl --------------------------------------------------------------------------

emu1=""
emu2=""

if test "x$PCE_BUILD_ATARIST" = "x1" ; then
	emu1="$emu1 atarist"
else
	emu2="$emu2 atarist"
fi

if test "x$PCE_BUILD_CPM80" = "x1" ; then
	emu1="$emu1 cpm80"
else
	emu2="$emu2 cpm80"
fi

if test "x$PCE_BUILD_IBMPC" = "x1" ; then
	emu1="$emu1 ibmpc"
else
	emu2="$emu2 ibmpc"
fi

if test "x$PCE_BUILD_MACPLUS" = "x1" ; then
	emu1="$emu1 macplus"
else
	emu2="$emu2 macplus"
fi

if test "x$PCE_BUILD_RC759" = "x1" ; then
	emu1="$emu1 rc759"
else
	emu2="$emu2 rc759"
fi

if test "x$PCE_BUILD_SIM405" = "x1" ; then
	emu1="$emu1 sim405"
else
	emu2="$emu2 sim405"
fi

if test "x$PCE_BUILD_SIMARM" = "x1" ; then
	emu1="$emu1 simarm"
else
	emu2="$emu2 simarm"
fi

if test "x$PCE_BUILD_SIMS32" = "x1" ; then
	emu1="$emu1 sims32"
else
	emu2="$emu2 sims32"
fi

if test "x$PCE_BUILD_VIC20" = "x1" ; then
	emu1="$emu1 vic20"
else
	emu2="$emu2 vic20"
fi


romext1=""
romext2=""

if test "x$PCE_BUILD_IBMPC_ROM" = "x1" ; then
	romext1="$romext1 ibmpc"
else
	romext2="$romext2 ibmpc"
fi

if test "x$PCE_BUILD_MACPLUS_ROM" = "x1" ; then
	romext1="$romext1 macplus"
else
	romext2="$romext2 macplus"
fi


term1=" null"
term2=""

if test "x$PCE_ENABLE_X11" = "x1" ; then
	term1="$term1 x11"
else
	term2="$term2 x11"
fi

if test "x$PCE_ENABLE_SDL1" = "x1" ; then
	term1="$term1 sdl1"
else
	term2="$term2 sdl1"
fi

if test "x$PCE_ENABLE_SDL2" = "x1" ; then
	term1="$term1 sdl2"
else
	term2="$term2 sdl2"
fi


char1=" null stdio"
char2=""

if test "x$PCE_ENABLE_CHAR_POSIX" = "x1" ; then
	char1="$char1 posix"
else
	char2="$char2 posix"
fi

if test "x$PCE_ENABLE_CHAR_PPP" = "x1" ; then
	char1="$char1 ppp"
else
	char2="$char2 ppp"
fi

if test "x$PCE_ENABLE_CHAR_PTY" = "x1" ; then
	char1="$char1 pty"
else
	char2="$char2 pty"
fi

if test "x$PCE_ENABLE_CHAR_SLIP" = "x1" ; then
	char1="$char1 slip"
else
	char2="$char2 slip"
fi

if test "x$PCE_ENABLE_CHAR_TCP" = "x1" ; then
	char1="$char1 tcp"
else
	char2="$char2 tcp"
fi

if test "x$PCE_ENABLE_CHAR_TIOS" = "x1" ; then
	char1="$char1 termios"
else
	char2="$char2 termios"
fi

if test "x$PCE_ENABLE_CHAR_WINCOM" = "x1" ; then
	char1="$char1 wincom"
else
	char2="$char2 wincom"
fi


sound1=" null wav"
sound2=""

if test "x$PCE_ENABLE_SOUND_OSS" = "x1" ; then
	sound1="$sound1 oss"
else
	sound2="$sound2 oss"
fi

if test "x$PCE_ENABLE_SDL" = "x1" ; then
	sound1="$sound1 sdl"
else
	sound2="$sound2 sdl"
fi


option1=""
option2=""

if test "x$PCE_ENABLE_READLINE" = "x1" ; then
	option1="$option1 readline"
else
	option2="$option2 readline"
fi

if test "x$PCE_ENABLE_TUN" = "x1" ; then
	option1="$option1 tun"
else
	option2="$option2 tun"
fi

if test "x$PCE_ENABLE_VMNET" = "x1" ; then
	option1="$option1 vmnet"
else
	option2="$option2 vmnet"
fi


AC_CONFIG_FILES([Makefile Makefile.inc src/config.inc])
AC_OUTPUT


#-----------------------------------------------------------------------------
# info

echo ""
echo "pce $PCE_VERSION_STR is now configured:"

echo "  CC:                      $CC $CFLAGS"
echo "  LD:                      $CC $LDFLAGS"
echo
echo "  prefix:                  $prefix"
echo
echo "  Emulators built:        $emu1"
echo "  Emulators not built:    $emu2"
echo "  ROMs built:             $romext1"
echo "  ROMs not built:         $romext2"
echo "  Terminals built:        $term1"
echo "  Terminals not built:    $term2"
echo "  Char drivers built:     $char1"
echo "  Char drivers not built: $char2"
echo "  Sound drivers built:    $sound1"
echo "  Sound drivers not built:$sound2"
echo "  Enabled options:        $option1"
echo "  Disabled options:       $option2"
