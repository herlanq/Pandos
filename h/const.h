#ifndef CONSTS
#define CONSTS

/**************************************************************************** 
 *
 * This header file contains utility constants & macro definitions.
 * 
 ****************************************************************************/

/* Hardware & software constants */
#define PAGESIZE		  4096			/* page size in bytes	*/
#define WORDLEN			  4				  /* word size in bytes	*/

#define TLBRTRAP 1
#define PROGTRAP 2
#define SYSTRAP  3



/* timer, timescale, TOD-LO and other bus regs */
#define RAMBASEADDR		0x10000000
#define RAMBASESIZE		0x10000004
#define TODLOADDR		0x1000001C
#define INTERVALTMR		0x10000020	
#define TIMESCALEADDR	0x10000024


/* utility constants */
#define	TRUE			    1
#define	FALSE			    0
#define HIDDEN			  static
#define EOS				    '\0'

#define NULL 			  ((void *)0xFFFFFFFF)
#define MAXPROC	          20
#define MAXINT            0xFFFFFFFF
/* Sem number
 * 8 disks
 * 8 usb
 * 8 network
 * 8 printer
 * 16 terminal(one for read and one for write)
 * +1 for sudo
 * 49 total device semaphores */
#define SEMNUM           49

#define DEVINTNUM		  5		  /* interrupt lines used by devices */
#define DEVPERINT		  8		  /* devices per interrupt line */
#define DEVICECNT         (DEVPERINT * DEVINTNUM)
#define DEVREGLEN		  4		  /* device register field length in bytes, and regs per dev */	
#define DEVREGSIZE	  16 		/* device register size in bytes */

/* system call codes */
#define	CREATETHREAD	1	/* create thread */
#define	TERMINATETHREAD	2	/* terminate thread */
#define	PASSERN			3	/* P a semaphore */
#define	VERHOGEN		4	/* V a semaphore */
#define	WAITIO			5	/* delay on a io semaphore */
#define	GETCPUTIME		6	/* get cpu time used to date */
#define	WAITCLOCK		7	/* delay on the clock semaphore */
#define	GETSPTPTR		8	/* return support structure ptr. */

/* Define Devices and Device interrupts  */
#define PLT 1
#define TIMER 2
#define DISK 3
#define FLASH 4
#define NETWORK 5
#define PRINTER 6
#define TERMINAL 7

#define DEV0 0x00000001
#define DEV1 0x00000002
#define DEV2 0x00000004
#define DEV3 0x00000008
#define DEV4 0x00000010
#define DEV5 0x00000020
#define DEV6 0x00000040
#define DEV7 0x00000080

#define PLTINT 0x00000200 /* clock 1 interrupt */
#define TIMERINT 0x00000400 /* clock 2 interrupt */
#define DISKINT 0x00000800  /* disk interrupt */
#define FLASHINT 0x00001000  /* flash interrupt */
#define NETWORKINT 0x00002000   /* network interrupt */
#define PRINTERINT 0x00004000  /* printer interrupt */
#define TERMINT 0x00008000  /* terminal interrupt */

#define SHIFT 2
#define IOINTERRUPT 0
#define TLBEXCEPTION 3
#define SYSEXCEPTION 8

#define FIRSTBIT 0x00000001
#define DEVPHYS 0x10000000
#define TOTALDEVICES 8

/* device register field number for non-terminal devices */
#define STATUS			  0
#define COMMAND			  1
#define DATA0			    2
#define DATA1			    3

/* device register field number for terminal devices */
#define RECVSTATUS  	0
#define RECVCOMMAND 	1
#define TRANSTATUS  	2
#define TRANCOMMAND 	3

/* device common STATUS codes */
#define UNINSTALLED		0
#define READY			    1
#define BUSY			    3

/* device common COMMAND codes */
#define RESET			    0
#define ACK				    1
#define ON                  1
#define OFF                 0

/* Memory related constants */
#define KSEG0           0x00000000
#define KSEG1           0x20000000
#define KSEG2           0x40000000
#define KUSEG           0x80000000
#define RAMSTART        0x20000000
#define BIOSDATAPAGE    0x0FFFF000
#define	PASSUPVECTOR	0x0FFFF900
#define RAMTOP          0x20001000


/* operations */
#define	MIN(A,B)		((A) < (B) ? A : B)
#define MAX(A,B)		((A) < (B) ? B : A)
#define	ALIGNED(A)		(((unsigned)A & 0x3) == 0)

#define IOCLOCK 100000
#define QUANTUM 5000
#define PSUEDOCLOCKTIME 100000
#define MAXTIME 0xFFFFFFFF


/* ON/OFF bit manipulation */
#define ALLOFF  0x00000000
#define UMON    0x00000008     /* User Mode ON */
#define UMOFF   0x00000008       /* User Mode OFF (kernel on) */
#define IMON    0x0000FF00      /* Interrupt Masked */
#define IEPON	0x00000004      /* Turn interrupts ON*/
#define IECON	0x00000001      /* Turn interrupts current ON */
#define TEBITON	0x08000000
#define CAUSE	0x0000007C		/* Turn on the cause bits for exception handling */

/* Macro to load the Interval Timer */
#define LDIT(T)	((* ((cpu_t *) INTERVALTMR)) = (T) * (* ((cpu_t *) TIMESCALEADDR))) 

/* Macro to read the TOD clock */
#define STCK(T) ((T) = ((* ((cpu_t *) TODLOADDR)) / (* ((cpu_t *) TIMESCALEADDR))))

#endif
