        TITLE	CRTINI

	;;
	;; A bit of silliness to define the global symbol 'ONE'.
	;; 
	;; 'ONE' ensures that any addresses expressed by the GIW macro 
	;; MUST be evaluated by the linker in their full 30 bit glory.
	;;
	
	INTERN	ONE
ONE=1
	;; 
	;; Establish the default start addresses of all program sections
	;; The defaults should work for quite large programs as is.
	;; 
	;; The user can override these assignments (and probably should
	;; to fine tune a program) using linker options.
	;;
	;; This module MUST be loaded first so it can establish starting
	;; .PSECT addresses before any code/data is loaded.
	;; 

	;; 
	;; low .PSECT - low core
	;; Precisely 1 page.
	;;

	.PSECT	low/ronly/overlaid/paligned,<1,,000000>
	INTERN	low.e
	BLOCK	1000
low.e:	
	.ENDPS

	;;
	;; text .PSECT - program code.
	;; Immediately follows low core.
	;; 
	
	.PSECT	text/ronly/concatenate/paligned,<1,,001000>
	.ENDPS	

	;;
	;; rdat .PSECT - read only data
	;; The PDV is written by the linker at the end of this .PSECT
	;;
	
	.PSECT	rdat/ronly/concatenate/paligned,<2,,000000>
	.ENDPS
	
	;;
	;; dat .PSECT - read/write data
	;;

	.PSECT	dat/rwrite/concatenate/paligned,<3,,000000>
	.ENDPS
	
	;;
	;; bss .PSECT - uninitialized data
	;;

	.PSECT	bss/rwrite/concatenate/paligned,<4,,000000>
	.ENDPS
	
	;;
	;; symb .PSECT - program symbol table
	;; Can be nuked by .crt0 to free up memory
	;; 
	
	.PSECT	symb/ronly/concatenate/paligned,<5,,000000>
	.ENDPS

	;; 
	;; stak .PSECT - program stack
	;; Can be nuked and relocated by .crt0 to a dynamic area

	.PSECT	stak/overlaid/rwrite/paligned,<6,,000000>
	.ENDPS

#ifdef __DUALSTACK__
	;; 
	;; ctrl .PSECT - program stack
	;; Can be nuked and relocated by .crt0 to a dynamic area

	.PSECT	ctrl/overlaid/rwrite/paligned,<7,,000000>
	.ENDPS
#endif

	END
