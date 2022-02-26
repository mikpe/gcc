.macro linkset_check
	DEFR		D0FrT, TXSTAT 
	ANDT		D0FrT, D0FrT, #HI(0x3F000000)
	CMPT		D0FrT, #HI(0x02000000)
.endm

.macro func_start func_name
	.text 
	.global \func_name
	.type \func_name,function 
	.align 2 
\func_name:
1:
.endm

.macro func_end func_name
	linkset_check
	BNE		1b 
	cache_flush
	2: MOV		PC, D1RtP 
	.size \func_name, .-\func_name
.endm

.macro cache_flush
	MOV		D0FrT, #0
	DCACHE		[D1Ar1], D0FrT
.endm

.macro sync_fetch_and_op op op_name n mode
func_start ___sync_fetch_and_\op_name\()_\n
	LNKGET\mode	D0Re0, [D1Ar1] 
	\op		D0FrT, D0Re0, D0Ar2 
	LNKSET\mode	[D1Ar1], D0FrT 
func_end ___sync_fetch_and_\op_name\()_\n
.endm

.macro sync_fetch_and_op_8 op op_name 
func_start ___sync_fetch_and_\op_name\()_8
	LNKGETL		D0Re0, D1Re0, [D1Ar1] 
	\op		A0.2, D0Re0, D0Ar4
	\op		A1.2, D1Re0, D1Ar3 
	LNKSETL		[D1Ar1], A0.2, A1.2
func_end ___sync_fetch_and_\op_name\()_8
.endm

.macro sync_fetch_and_alu_op_8 op op_name 
func_start ___sync_fetch_and_\op_name\()_8
	LNKGETL		D0Re0, D1Re0, [D1Ar1] 
	\op\()S		A0.2, D0Re0, D0Ar4
	\op		A1.2, D1Re0, D1Ar3 
	\op\()CS	A1.2, A1.2, #1
	LNKSETL		[D1Ar1], A0.2, A1.2
func_end ___sync_fetch_and_\op_name\()_8
.endm

.macro sync_op_and_fetch op op_name n mode
func_start ___sync_\op_name\()_and_fetch_\n
	LNKGET\mode	D0Re0, [D1Ar1] 
	\op		D0Re0, D0Re0, D0Ar2 
	LNKSET\mode	[D1Ar1], D0Re0 
func_end ___sync_\op_name\()_and_fetch_\n
.endm

.macro sync_op_and_fetch_8 op op_name 
func_start ___sync_\op_name\()_and_fetch_8
	LNKGETL		D0Re0, D1Re0, [D1Ar1] 
	\op		D0Re0, D0Re0, D0Ar4
	\op		D1Re0, D1Re0, D1Ar3 
	LNKSETL		[D1Ar1], D0Re0, D1Re0
func_end ___sync_\op_name\()_and_fetch_8
.endm

.macro sync_alu_op_and_fetch_8 op op_name 
func_start ___sync_\op_name\()_and_fetch_8
	LNKGETL		D0Re0, D1Re0, [D1Ar1] 
	\op\()S		D0Re0, D0Re0, D0Ar4
	\op		D1Re0, D1Re0, D1Ar3 
	\op\()CS	D1Re0, D1Re0, #1
	LNKSETL		[D1Ar1], D0Re0, D1Re0
func_end ___sync_\op_name\()_and_fetch_8
.endm

.macro sync_fetch_and_op_neg op op_name n mode
func_start ___sync_fetch_and_\op_name\()_\n
	LNKGET\mode	D0Re0, [D1Ar1] 
	\op		D0FrT, D0Re0, D0Ar2 
	XOR		D0FrT, D0FrT, #-1
	LNKSET\mode	[D1Ar1], D0FrT 
func_end ___sync_fetch_and_\op_name\()_\n
.endm

.macro sync_fetch_and_op_neg_8 op op_name 
func_start ___sync_fetch_and_\op_name\()_8
	LNKGETL		D0Re0, D1Re0, [D1Ar1] 
	\op		D0Ar6, D0Re0, D0Ar4
	\op		D1Ar5, D1Re0, D1Ar3 
	XOR		D0Ar6, D0Ar6, #-1
	XOR		D1Ar5, D1Ar5, #-1
	LNKSETL		[D1Ar1], D0Ar6, D1Ar5
func_end ___sync_fetch_and_\op_name\()_8
.endm

.macro sync_op_neg_and_fetch op op_name n mode
func_start ___sync_\op_name\()_and_fetch_\n
	LNKGET\mode	D0Re0, [D1Ar1] 
	\op		D0Re0, D0Re0, D0Ar2 
	XOR		D0Re0, D0Re0, #-1
	LNKSET\mode	[D1Ar1], D0Re0
func_end ___sync_\op_name\()_and_fetch_\n
.endm

.macro sync_op_neg_and_fetch_8 op op_name 
func_start ___sync_\op_name\()_and_fetch_8
	LNKGETL		D0Re0, D1Re0, [D1Ar1] 
	\op		D0Re0, D0Re0, D0Ar4
	\op		D1Re0, D1Re0, D1Ar3 
	XOR		D0Re0, D0Re0, #-1
	XOR		D1Re0, D1Re0, #-1
	LNKSETL		[D1Ar1], D0Re0, D1Re0
func_end ___sync_\op_name\()_and_fetch_8
.endm

.macro sync_val_compare_and_swap n mode
func_start ___sync_val_compare_and_swap_\n
	LNKGET\mode		D0Re0, [D1Ar1] 
	CMP			D0Re0, D0Ar2 
	LNKSET\mode\()EQ	[D1Ar1], D1Ar3
	BNE			2f
func_end ___sync_val_compare_and_swap_\n
.endm

func_start ___sync_val_compare_and_swap_8
	LNKGETL		D0Re0, D1Re0, [D1Ar1] 
	CMP		D0Re0, D0Ar4
	CMPEQ		D1Re0, D1Ar3 
	LNKSETLEQ	[D1Ar1], D0Ar6, D1Ar5
	BNE		2f
func_end ___sync_val_compare_and_swap_8

.macro sync_bool_compare_and_swap n mode
func_start ___sync_bool_compare_and_swap_\n
	MOV			D0Re0, #1
	1: LNKGET\mode		D0FrT, [D1Ar1] 
	CMP			D0FrT, D0Ar2 
	LNKSET\mode\()EQ	[D1Ar1], D1Ar3
	XORNE			D0Re0, D0Re0, D0Re0
	BNE			2f
func_end ___sync_bool_compare_and_swap_\n
.endm

func_start ___sync_bool_compare_and_swap_8
	MOV		D0Re0, #1
	1: LNKGETL	A0.2, A1.2, [D1Ar1] 
	CMP		D0Ar4, A0.2
	CMPEQ		D1Ar3, A1.2
	LNKSETLEQ	[D1Ar1], D0Ar6, D1Ar5
	XORNE		D0Re0, D0Re0, D0Re0
	BNE		2f
func_end ___sync_bool_compare_and_swap_8

.macro sync_lock_test_and_set n mode
func_start ___sync_lock_test_and_set_\n
	LNKGET\mode	D0Re0, [D1Ar1] 
	LNKSET\mode	[D1Ar1], D0Ar2
func_end ___sync_lock_test_and_set_\n
.endm

func_start ___sync_lock_test_and_set_8
	LNKGETL		D0Re0, D1Re0, [D1Ar1] 
	LNKSETL		[D1Ar1], D0Ar4, D1Ar3
func_end ___sync_lock_test_and_set_8

.macro sync_fetch_and_op_size op op_name
	sync_fetch_and_op \op \op_name 1 B
	sync_fetch_and_op \op \op_name 2 W
	sync_fetch_and_op \op \op_name 4 D

	sync_op_and_fetch \op \op_name 1 B
	sync_op_and_fetch \op \op_name 2 W
	sync_op_and_fetch \op \op_name 4 D
.endm

.macro sync_fetch_and_op_neg_size op op_name
	sync_fetch_and_op_neg \op \op_name 1 B
	sync_fetch_and_op_neg \op \op_name 2 W
	sync_fetch_and_op_neg \op \op_name 4 D
	sync_fetch_and_op_neg_8 \op \op_name

	sync_op_neg_and_fetch \op \op_name 1 B
	sync_op_neg_and_fetch \op \op_name 2 W
	sync_op_neg_and_fetch \op \op_name 4 D
	sync_op_neg_and_fetch_8 \op \op_name
.endm

sync_fetch_and_op_size ADD add
sync_fetch_and_op_size SUB sub
sync_fetch_and_op_size OR or
sync_fetch_and_op_size AND and
sync_fetch_and_op_size XOR xor

sync_fetch_and_op_8 OR or
sync_fetch_and_op_8 AND and
sync_fetch_and_op_8 XOR xor

sync_op_and_fetch_8 OR or
sync_op_and_fetch_8 AND and
sync_op_and_fetch_8 XOR xor

sync_fetch_and_alu_op_8 ADD add
sync_fetch_and_alu_op_8 SUB sub

sync_alu_op_and_fetch_8 ADD add
sync_alu_op_and_fetch_8 SUB sub

sync_fetch_and_op_neg_size AND nand

sync_val_compare_and_swap 1 B
sync_val_compare_and_swap 2 W
sync_val_compare_and_swap 4 D

sync_bool_compare_and_swap 1 B
sync_bool_compare_and_swap 2 W
sync_bool_compare_and_swap 4 D

sync_lock_test_and_set 1 B
sync_lock_test_and_set 2 W
sync_lock_test_and_set 4 D
