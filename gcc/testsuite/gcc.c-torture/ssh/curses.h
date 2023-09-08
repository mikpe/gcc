/*
 * Copyright (c) 1981 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS for A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * for ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN if ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)curses.h	5.9 (Berkeley) 7/1/90
 */

/* Redefine external symbols which could potentially become ambiguous */
#define AL cu_1
#define AL_PARM cu_2
#define AM cu_3
#define BC cu_4
#define BS cu_5
#define BT cu_6
#define CA cu_7
#define CD cu_8
#define CE cu_9
#define CL cu_10
#define CM cu_11
#define COLS cu_12
#define CR cu_13
#define CS cu_14
#define DA cu_15
#define DB cu_16
#define DC cu_17
#define DL cu_18
#define DL_PARM cu_19
#define DM cu_20
#define do cu_21
#define DOWN_PARM cu_22
#define Def_term cu_23
#define ED cu_24
#define EI cu_25
#define EO cu_26
#define GT cu_27
#define HC cu_28
#define HO cu_29
#define HZ cu_30
#define IC cu_31
#define IM cu_32
#define IN cu_33
#define IP cu_34
#define K0 cu_35
#define K1 cu_36
#define K2 cu_37
#define K3 cu_38
#define K4 cu_39
#define K5 cu_40
#define K6 cu_41
#define K7 cu_42
#define K8 cu_43
#define K9 cu_44
#define KD cu_45
#define KE cu_46
#define KH cu_47
#define KL cu_48
#define KR cu_49
#define KS cu_50
#define KU cu_51
#define LEFT_PARM cu_52
#define LINES cu_53
#define LL cu_54
#define MA cu_55
#define MI cu_56
#define MS cu_57
#define My_term cu_58
#define NC cu_59
#define ND cu_60
#define NL cu_61
#define NONL cu_62
#define NS cu_63
#define OS cu_64
#define PC cu_65
#define RC cu_66
#define RIGHT_PARM cu_67
#define SC cu_68
#define SE cu_69
#define SF cu_70
#define SO cu_71
#define SR cu_72
#define TA cu_73
#define TE cu_74
#define TI cu_75
#define UC cu_76
#define UE cu_77
#define UL cu_78
#define UP cu_79
#define UPPERCASE cu_80
#define UP_PARM cu_81
#define US cu_82
#define VB cu_83
#define VE cu_84
#define VS cu_85
#define XB cu_86
#define XN cu_87
#define XS cu_88
#define XT cu_89
#define XX cu_90
#define _echoit cu_91
#define _endwin cu_92
#define _id_subwins cu_93
#define _putchar cu_94
#define _rawmode cu_95
#define _res_flg cu_96
#define _set_subwin_ cu_97
#define _sprintw cu_98
#define _sscans cu_99
#define _swflags_ cu_100
#define _tspace cu_101
#define _tty_ch cu_102
#define _unctrl cu_103
#define domvcur cu_104
#define fullname cu_105
#define gettmode cu_106
#define initscr cu_107
#define longname cu_108
#define mvprintw cu_109
#define mvscanw cu_110
#define mvwprintw cu_111
#define mvwscanw cu_112
#define normtty cu_113
#define overlay cu_114
#define overwrite cu_115
#define plodput cu_116
#define setterm cu_117
#define touchline cu_118
#define touchoverlap cu_119
#define touchwin cu_120
#define ttytype cu_121
#define waddbytes cu_122
#define waddstr cu_123
#define wclrtobot cu_124
#define wclrtoeol cu_125
#define wdeleteln cu_126
#define wgetstr cu_127
#define winsertln cu_128
#define wprintw cu_129
#define wrefresh cu_130
#define wstandend cu_131
#define wstandout cu_132

#ifndef WINDOW

#include	<stdio.h>
 
#define USE_OLD_TTY
#include	<sys/ioctl.h>
#undef USE_OLD_TTY

#define	bool	char
#define	reg	register

#define	TRUE	(1)
#define	FALSE	(0)
#define	ERR	(0)
#define	OK	(1)

#define	_ENDLINE	001
#define	_FULLWIN	002
#define	_SCROLLWIN	004
#define	_FLUSH		010
#define	_FULLLINE	020
#define	_IDLINE		040
#define	_STANDOUT	0200
#define	_NOCHANGE	-1

#define	_puts(s)	tputs(s, 0, _putchar)

typedef	struct sgttyb	SGTTY;

/*
 * Capabilities from termcap
 */

extern bool     AM, BS, CA, DA, DB, EO, HC, HZ, IN, MI, MS, NC, NS, OS, UL,
		XB, XN, XT, XS, XX;
extern char	*AL, *BC, *BT, *CD, *CE, *CL, *CM, *CR, *CS, *DC, *DL,
		*DM, *do, *ED, *EI, *K0, *K1, *K2, *K3, *K4, *K5, *K6,
		*K7, *K8, *K9, *HO, *IC, *IM, *IP, *KD, *KE, *KH, *KL,
		*KR, *KS, *KU, *LL, *MA, *ND, *NL, *RC, *SC, *SE, *SF,
		*SO, *SR, *TA, *TE, *TI, *UC, *UE, *UP, *US, *VB, *VS,
		*VE, *AL_PARM, *DL_PARM, *UP_PARM, *DOWN_PARM,
		*LEFT_PARM, *RIGHT_PARM;
extern char	PC;

/*
 * From the tty modes...
 */

extern bool	GT, NONL, UPPERCASE, normtty, _pfast;

struct _win_st {
	short		_cury, _curx;
	short		_maxy, _maxx;
	short		_begy, _begx;
	short		_flags;
	short		_ch_off;
	bool		_clear;
	bool		_leave;
	bool		_scroll;
	char		**_y;
	short		*_firstch;
	short		*_lastch;
	struct _win_st	*_nextp, *_orig;
};

#define	WINDOW	struct _win_st

extern bool	My_term, _echoit, _rawmode, _endwin;

extern char	*Def_term, ttytype[];

extern int	LINES, COLS, _tty_ch, _res_flg;

extern SGTTY	_tty;

extern WINDOW	*stdscr, *curscr;

/*
 *	Define VOID to stop lint from generating "null effect"
 * comments.
 */
#ifdef lint
int	__void__;
#define	VOID(x)	(__void__ = (int) (x))
#else
#define	VOID(x)	(x)
#endif

/*
 * psuedo functions for standard screen
 */
#define	addch(ch)	VOID(waddch(stdscr, ch))
#define	getch()		VOID(wgetch(stdscr))
#define	addbytes(da,co)	VOID(waddbytes(stdscr, da,co))
#define	addstr(str)	VOID(waddbytes(stdscr, str, strlen(str)))
#define	getstr(str)	VOID(wgetstr(stdscr, str))
#define	move(y, x)	VOID(wmove(stdscr, y, x))
#define	clear()		VOID(wclear(stdscr))
#define	erase()		VOID(werase(stdscr))
#define	clrtobot()	VOID(wclrtobot(stdscr))
#define	clrtoeol()	VOID(wclrtoeol(stdscr))
#define	insertln()	VOID(winsertln(stdscr))
#define	deleteln()	VOID(wdeleteln(stdscr))
#define	refresh()	VOID(wrefresh(stdscr))
#define	inch()		VOID(winch(stdscr))
#define	insch(c)	VOID(winsch(stdscr,c))
#define	delch()		VOID(wdelch(stdscr))
#define	standout()	VOID(wstandout(stdscr))
#define	standend()	VOID(wstandend(stdscr))

/*
 * mv functions
 */
#define	mvwaddch(win,y,x,ch)	VOID(wmove(win,y,x)==ERR?ERR:waddch(win,ch))
#define	mvwgetch(win,y,x)	VOID(wmove(win,y,x)==ERR?ERR:wgetch(win))
#define	mvwaddbytes(win,y,x,da,co) \
		VOID(wmove(win,y,x)==ERR?ERR:waddbytes(win,da,co))
#define	mvwaddstr(win,y,x,str) \
		VOID(wmove(win,y,x)==ERR?ERR:waddbytes(win,str,strlen(str)))
#define mvwgetstr(win,y,x,str)  VOID(wmove(win,y,x)==ERR?ERR:wgetstr(win,str))
#define	mvwinch(win,y,x)	VOID(wmove(win,y,x) == ERR ? ERR : winch(win))
#define	mvwdelch(win,y,x)	VOID(wmove(win,y,x) == ERR ? ERR : wdelch(win))
#define	mvwinsch(win,y,x,c)	VOID(wmove(win,y,x) == ERR ? ERR:winsch(win,c))
#define	mvaddch(y,x,ch)		mvwaddch(stdscr,y,x,ch)
#define	mvgetch(y,x)		mvwgetch(stdscr,y,x)
#define	mvaddbytes(y,x,da,co)	mvwaddbytes(stdscr,y,x,da,co)
#define	mvaddstr(y,x,str)	mvwaddstr(stdscr,y,x,str)
#define mvgetstr(y,x,str)       mvwgetstr(stdscr,y,x,str)
#define	mvinch(y,x)		mvwinch(stdscr,y,x)
#define	mvdelch(y,x)		mvwdelch(stdscr,y,x)
#define	mvinsch(y,x,c)		mvwinsch(stdscr,y,x,c)

/*
 * psuedo functions
 */

#define	clearok(win,bf)	 (win->_clear = bf)
#define	leaveok(win,bf)	 (win->_leave = bf)
#define	scrollok(win,bf) (win->_scroll = bf)
#define flushok(win,bf)	 (bf ? (win->_flags |= _FLUSH):(win->_flags &= ~_FLUSH))
#define	getyx(win,y,x)	 y = win->_cury, x = win->_curx
#define	winch(win)	 (win->_y[win->_cury][win->_curx] & 0177)

#define raw()	 (_tty.sg_flags|=RAW, _pfast=_rawmode=TRUE, \
	ioctl(_tty_ch, TIOCSETP, (char *)&_tty))
#define noraw()	 (_tty.sg_flags&=~RAW,_rawmode=FALSE,\
	_pfast=!(_tty.sg_flags&CRMOD),ioctl(_tty_ch, TIOCSETP, (char *)&_tty))
#define cbreak() (_tty.sg_flags |= CBREAK, _rawmode = TRUE, \
	ioctl(_tty_ch, TIOCSETP, (char *)&_tty))
#define nocbreak() (_tty.sg_flags &= ~CBREAK,_rawmode=FALSE, \
	ioctl(_tty_ch, TIOCSETP, (char *)&_tty))
#define crmode() cbreak()	/* backwards compatability */
#define nocrmode() nocbreak()	/* backwards compatability */
#define echo()	 (_tty.sg_flags |= ECHO, _echoit = TRUE, \
	ioctl(_tty_ch, TIOCSETP, (char *)&_tty))
#define noecho() (_tty.sg_flags &= ~ECHO, _echoit = FALSE, \
	ioctl(_tty_ch, TIOCSETP, (char *)&_tty))
#define nl()	 (_tty.sg_flags |= CRMOD,_pfast = _rawmode, \
	ioctl(_tty_ch, TIOCSETP, (char *)&_tty))
#define nonl()	 (_tty.sg_flags &= ~CRMOD, _pfast = TRUE, \
	ioctl(_tty_ch, TIOCSETP, (char *)&_tty))
#define	savetty() ((void) ioctl(_tty_ch, TIOCGETP, (char *)&_tty), \
	_res_flg = _tty.sg_flags)
#define	resetty() (_tty.sg_flags = _res_flg, \
	_echoit = ((_res_flg & ECHO) == ECHO), \
	_rawmode = ((_res_flg & (CBREAK|RAW)) != 0), \
	_pfast = ((_res_flg & CRMOD) ? _rawmode : TRUE), \
	(void) ioctl(_tty_ch, TIOCSETP, (char *)&_tty))

#define	erasechar()	(_tty.sg_erase)
#define	killchar()	(_tty.sg_kill)
#define baudrate()	(_tty.sg_ospeed)

/* Function prototypes */
#if defined(__STDC__) || defined(__cplusplus)
#include <stdarg.h>
# define P_(s) s
#else
# define P_(s) ()
#endif

/* addbytes.c */
extern int waddbytes P_((reg WINDOW *win,reg char *bytes,reg int count));

/* addch.c */
extern int waddch P_((WINDOW *win,char c));

/* addstr.c */
extern int waddstr P_((reg WINDOW *win,reg char *str));

/* box.c */
extern int box P_((reg WINDOW *win,int vert,int hor));

/* clear.c */
extern int wclear P_((reg WINDOW *win));

/* clrtobot.c */
extern int wclrtobot P_((reg WINDOW *win));

/* clrtoeol.c */
extern int wclrtoeol P_((reg WINDOW *win));

/* cr_put.C */
extern int mvcur P_((int ly,int lx,int y,int x));
extern int fgoto P_((void));
extern int plodput P_((int c));
extern int plod P_((int cnt));
extern int tabcol P_((int col,int ts));

/* cr_tty.C */
extern int gettmode P_((void));
extern int setterm P_((reg char *type));
extern int zap P_((void));
extern char *getcap P_((char *name));

/* curses.c */

/* delch.c */
extern int wdelch P_((reg WINDOW *win));

/* deleteln.c */
extern int wdeleteln P_((reg WINDOW *win));

/* delwin.c */
extern int delwin P_((reg WINDOW *win));

/* endwin.c */
extern int endwin P_((void));

/* erase.c */
extern int werase P_((reg WINDOW *win));

/* fullname.c */
extern char *fullname P_((reg char *bp,reg char *def));

/* getch.c */
extern int wgetch P_((reg WINDOW *win));

/* getstr.c */
extern int wgetstr P_((reg WINDOW *win,reg char *str));

/* idlok.c */
extern int idlok P_((WINDOW *win,bool bf));

/* id_subwins.C */
extern int _id_subwins P_((WINDOW *orig));

/* initscr.c */
extern WINDOW *initscr P_((void));

/* insch.c */
extern int winsch P_((reg WINDOW *win,int c));

/* insertln.c */
extern int winsertln P_((reg WINDOW *win));

/* longname.c */
extern char *longname P_((reg char *bp,reg char *def));

/* move.c */
extern int wmove P_((reg WINDOW *win,reg int y,reg int x));

/* mvprintw.c */
extern int mvprintw P_((reg int y,reg int x,const char *fmt,...));
extern int mvwprintw P_((reg WINDOW *win, reg int y, reg int x, const char *fmt, ...));

/* mvscanw.c */
extern int mvscanw P_((reg int y, reg int x, const char *fmt, ...));
extern int mvwscanw P_((reg WINDOW *win, reg int y, reg int x, const char *fmt, ...));

/* mvwin.c */
extern int mvwin P_((reg WINDOW *win,reg int by,reg int bx));

/* newwin.c */
extern WINDOW *newwin P_((int num_lines,int num_cols,int begy,int begx));
extern WINDOW *subwin P_((reg WINDOW *orig,int num_lines,int num_cols,int begy,int begx));
extern int _set_subwin_ P_((WINDOW *orig,WINDOW *win));
extern int _swflags_ P_((WINDOW *win));

/* overlay.c */
extern int overlay P_((reg WINDOW *win1,reg WINDOW *win2));

/* overwrite.c */
extern int overwrite P_((reg WINDOW *win1,reg WINDOW *win2));

/* printw.c */
extern int printw P_((const char *fmt, ...));
extern int wprintw P_((WINDOW *win, const char *fmt, ...));
extern int _sprintw P_((WINDOW *win,const char *fmt,va_list ap));

/* putchar.c */
extern int _putchar P_((reg int c));

/* refresh.c */
extern int wrefresh P_((reg WINDOW *win));

/* scanw.c */
extern int scanw P_((const char *fmt, ...));
extern int wscanw P_((WINDOW *win, const char *fmt, ...));
extern int _sscans P_((WINDOW *win,const char *fmt,va_list ap));

/* scroll.c */
extern int scroll P_((WINDOW *win));

/* standout.c */
extern char *wstandout P_((reg WINDOW *win));
extern char *wstandend P_((reg WINDOW *win));

/* toucholap.c */
extern int touchoverlap P_((reg WINDOW *win1,reg WINDOW *win2));

/* touchwin.c */
extern int touchwin P_((WINDOW *win));
extern int touchline P_((WINDOW *win,int y,int sx,int ex));

/* tstp.c */
extern void tstp P_((int sig));

/* unctrl.c */

#undef P_

/*
 * used to be in unctrl.h.
 */
#define	unctrl(c)	_unctrl[(c) & 0177]
extern char *_unctrl[];
#endif
