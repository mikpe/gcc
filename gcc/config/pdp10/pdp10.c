/* Output routines for the PDP-10.
   Copyright (C) 2001, 2002 Free Software Foundation, Inc.
   Contributed by Lars Brinkhoff (lars@nocrew.org), funded by XKL, LLC.

For information about the PDP-10 archtecture, see the README file in
this directory.

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify it under
the terms of the GNU General Pubic License as published by the Free
Software Foundation; either version 2, or (at your option) any later
version.

GCC is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING.  If not, write to the Free
Software Foundation, 59 Temple Place - Suite 330, Boston, MA
02111-1307, USA.  */


/**********************************************************************

	Index

**********************************************************************/

/*	  Front Page
	  Index
	  Includes
	  To-do List
	  Miscellaneous
	  Local variables.
	  GCC target structure.
	pdp10 support:
	  Run-time Target Specification
	  Storage Layout
	  Layout of Source Language Data Types
	  Register Usage
	  Register Classes
	  Stack Layout and Calling Conventions
	  Addressing Modes
	  Dividing the Output into Sections
	  Describing Relative Costs of Operations
	  Defining the Output Assembler Language
	  Target Attributes
	  Miscellaneous Parameters
	  Special Predicates
	  Built-in Functions
	  Support for MACRO, MIDAS, and FAIL
	pdp10.md support:
	  Data Movement
	  Pointer Arithmetic
	  Unconditional Jumps
	  Conditional Jumps
	  Function Prologue and Epilogue
	junkyard:
	  Unsorted */
    

/**********************************************************************

	Includes

**********************************************************************/

#ifdef ENABLE_SVNID_TAG
# ifdef __GNUC__
#  define _unused_ __attribute__((unused))
# else
#  define _unused_  /* define for other platforms here */
# endif
  static char const *SVNID _unused_ = "$Id$";
# undef ENABLE_SVNID_TAG
#endif

#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "tm.h"
#include "rtl.h"
#include "tree.h"
#include "expr.h"
#include "optabs.h"
#include "regs.h"
#include "hard-reg-set.h"
#include "real.h"
#include "insn-config.h"
#include "conditions.h"
#include "insn-flags.h"
#include "output.h"
#include "insn-attr.h"
#include "flags.h"
#include "reload.h"
#include "toplev.h"
#include "recog.h"
#include "function.h"
#include "ggc.h"
#include "pdp10-protos.h"
#include "c-tree.h"
#include "hash.h"
#include "target.h"
#include "target-def.h"
#include "tm_p.h"
#include "c-pragma.h"
#include "langhooks.h"
#include "tree-gimple.h"
#include "df.h"


/**********************************************************************

	To-do List

**********************************************************************/

/* Only use single_push/single_pop when optimize_size ||
   !TARGET_EXTENDED?  */

/* Candidates for inlining once the code settles:

   pdp10_align_stack_pointer, const_ok_for_letter_p,
   const_double_ok_for_letter_p, extra_constraint,
   starting_frame_offset, stack_pointer_offset, first_parm_offset,
   stack_dynamic_offset, function_profiler, legitimate_address_p,
   legitimize_address, pdp10_const_int_costs,
   pdp10_const_double_costs, encode_section_info,
   asm_declare_object_name, asm_output_labelref,
   asm_output_symbol_ref, print_operand_expression,
   pdp10_output_call_as_jrst_p.  */

/* Investigate whether it's safe for expand_blt to use BLT when
   operands[2] is not constant.  */

/* Use global format indirect words?  Recognize global format indirect
   word in LEGITIMATE_ADDRESS_P?  */

/* pdp10_function_profiler.  */

/* pdp10_bytesize.  */


/**********************************************************************

	Miscellaneous

**********************************************************************/

#define SAVE		1
#define RESTORE		0

#define HWINT(X) ((HOST_WIDE_INT)(X))
#define SIGN_BIT (HWINT (1) << 35)
#if HOST_BITS_PER_WIDE_INT > 36
#define WORD_MASK ((HWINT (1) << 36) - 1)
#else
#define WORD_MASK (HWINT (-1))
#endif

#define WORD_MODE_P(MODE) \
  (GET_MODE_SIZE (MODE) >= UNITS_PER_WORD || (MODE) == BLKmode)
#define BYTE_MODE_P(MODE) (!WORD_MODE_P (MODE))

#define WORD_TYPE_P(TYPE) (pdp10_bytesize (TYPE) >= 32)
#define BYTE_TYPE_P(TYPE) (!WORD_TYPE_P (TYPE))

#define VALID_OWGBP_BYTE_SIZE_P(SIZE) \
 ((SIZE) == 6 || (SIZE) == 7 || (SIZE) == 8 || (SIZE) == 9 || (SIZE) == 18)

#define GEN_EQ gen_rtx_EQ (VOIDmode, NULL_RTX, NULL_RTX)
#define GEN_NE gen_rtx_NE (VOIDmode, NULL_RTX, NULL_RTX)

#define GEN_MEM_SP_PLUS(mode, offset) \
  gen_rtx_MEM (mode, plus_constant (stack_pointer_rtx, offset))

#define GEN_PUSH(pushop) \
	gen_PUSH(gen_rtx_MEM(SImode, gen_rtx_fmt_e(PRE_INC, SImode, stack_pointer_rtx)), pushop)

#define GEN_POP(popop) \
	gen_POP(popop, gen_rtx_MEM(SImode, gen_rtx_fmt_e(POST_DEC, SImode, stack_pointer_rtx)))

#define gen_ADDRESS(X) \
  gen_rtx_UNSPEC (Pmode, gen_rtvec (1, (X)), UNSPEC_ADDRESS)

#if !HAVE_IBP
static rtx gen_IBP (rtx op0, rtx op1);
static rtx gen_IBP (rtx op0, rtx op1)
	{
	rtx x;	
	gcc_assert(GET_MODE(op0) == GET_MODE(op1));
	switch (GET_MODE(op0))
		{
		case SImode:
			x = gen_addsi3(op0, op1, GEN_INT (1));
			break;

		case Q8Pmode:
			x = gen_addq8p3(op0, op1, GEN_INT (1));
			break;

		case Q9Pmode:
			x = gen_addq9p3(op0, op1, GEN_INT (1));
			break;

		case HPmode:
			x = gen_addhp3(op0, op1, GEN_INT (1));
			break;

		default:
			gcc_assert(false);
		}
	return x;
	}
#endif

static void	pdp10_print_number PARAMS ((FILE *stream,
					    HOST_WIDE_INT x,
					    int div,
					    int bits,
					    int zero));

#define ADDRESS_MASK (TARGET_EXTENDED ? HWINT (07777777777) : HWINT (0777777))

#define pdp10_print_address(stream, x) \
  pdp10_print_number (stream, x, 1, TARGET_EXTENDED ? 30 : 18, 0)
#define pdp10_print_offset30(stream, x) \
  pdp10_print_number (stream, x, 1, TARGET_EXTENDED ? -30 : -18, 0)
#define pdp10_print_offset(stream, x) \
  pdp10_print_number (stream, x, 1, -18, 0)
#define pdp10_print_integer(stream, x) \
  pdp10_print_number (stream, x, 1, -36, 0)
#define pdp10_print_unsigned(stream, x) \
  pdp10_print_number (stream, x, 1, 36, 0)

#define SAME_REG(op0, op1) REG_P (op1) && REGNO (op0) == REGNO (op1)


/**********************************************************************

	Local variables.

**********************************************************************/

/* The number of registers used for argument passing.  */
unsigned int pdp10_regparm = PDP10_DEFAULT_REGPARM;

/* The number of registers used to return a value.  */
unsigned int pdp10_retregs = PDP10_DEFAULT_REGPARM;

/* The number of registers that are call clobbered.  */
unsigned int pdp10_clobberedregs = 0;

/* Record if GIW is output */
int pdp10_giw = 0;
static int used_byte_subtraction_table[19];

int pdp10_outputting_debug_info = 0;


/**********************************************************************

	GCC target structure.

**********************************************************************/

const struct attribute_spec pdp10_attribute_table[];
void pdp10_asm_named_section (const char *, unsigned int, tree);
static bool	pdp10_assemble_integer PARAMS ((rtx x, unsigned intsize,
						int aligned_p));
static rtx pdp10_struct_value_rtx (tree, int);
static void pdp10_init_sections (void);
static section *pdp10_select_section (tree, int, unsigned HOST_WIDE_INT);
static section *pdp10_function_rodata_section (tree decl);
static section *pdp10_select_rtx_section (enum machine_mode, rtx, unsigned HOST_WIDE_INT);


#undef TARGET_ATTRIBUTE_TABLE
#define TARGET_ATTRIBUTE_TABLE pdp10_attribute_table

#undef TARGET_ASM_OPEN_PAREN
#define TARGET_ASM_OPEN_PAREN "<"

#undef TARGET_ASM_CLOSE_PAREN
#define TARGET_ASM_CLOSE_PAREN ">"

#undef TARGET_ASM_BYTE_OP
#define TARGET_ASM_BYTE_OP 0

#undef TARGET_ASM_INTEGER
#define TARGET_ASM_INTEGER pdp10_assemble_integer

#undef TARGET_ASM_FUNCTION_PROLOGUE
#define TARGET_ASM_FUNCTION_PROLOGUE pdp10_output_func_prologue

#undef TARGET_ASM_FUNCTION_EPILOGUE
#define TARGET_ASM_FUNCTION_EPILOGUE pdp10_output_func_epilogue

#undef TARGET_ASM_FILE_START
#define TARGET_ASM_FILE_START macro_file_start

#undef TARGET_ASM_FILE_END
#define TARGET_ASM_FILE_END macro_file_end

#undef TARGET_ASM_SELECT_SECTION
#define TARGET_ASM_SELECT_SECTION pdp10_select_section

#undef TARGET_ASM_FUNCTION_RODATA_SECTION
#define TARGET_ASM_FUNCTION_RODATA_SECTION pdp10_function_rodata_section

#undef TARGET_ASM_SELECT_RTX_SECTION
#define TARGET_ASM_SELECT_RTX_SECTION pdp10_select_rtx_section

#undef TARGET_DEFAULT_TARGET_FLAGS
#define TARGET_DEFAULT_TARGET_FLAGS TARGET_DEFAULT

#undef TARGET_INIT_BUILTINS
#define TARGET_INIT_BUILTINS pdp10_init_builtins

#undef TARGET_EXPAND_BUILTIN
#define TARGET_EXPAND_BUILTIN pdp10_expand_builtin

#undef TARGET_BUILD_BUILTIN_VA_LIST
#define TARGET_BUILD_BUILTIN_VA_LIST pdp10_build_va_list_type

#undef TARGET_INIT_LIBFUNCS
#define TARGET_INIT_LIBFUNCS pdp10_init_libfuncs

#undef TARGET_RTX_COSTS
#define TARGET_RTX_COSTS pdp10_rtx_costs

#undef TARGET_ADDRESS_COST
#define TARGET_ADDRESS_COST pdp10_address_cost

#undef TARGET_MACHINE_DEPENDENT_REORG
#define TARGET_MACHINE_DEPENDENT_REORG pdp10_machine_dependent_reorg

#undef TARGET_PROMOTE_FUNCTION_ARGS
#define TARGET_PROMOTE_FUNCTION_ARGS hook_bool_const_tree_true

/*
Caused problems in 430 update - try to see if its really needed
-mtc 12/7/2007
#undef TARGET_PROMOTE_FUNCTION_RETURN
#define TARGET_PROMOTE_FUNCTION_RETURN hook_bool_const_tree_true
*/

#undef TARGET_PROMOTE_PROTOTYPES
#define TARGET_PROMOTE_PROTOTYPES hook_bool_const_tree_true

#undef TARGET_STRUCT_VALUE_RTX
#define TARGET_STRUCT_VALUE_RTX pdp10_struct_value_rtx

#undef TARGET_STRICT_ARGUMENT_NAMING
#define TARGET_STRICT_ARGUMENT_NAMING hook_bool_CUMULATIVE_ARGS_true

#undef TARGET_GIMPLIFY_VA_ARG_EXPR
#define TARGET_GIMPLIFY_VA_ARG_EXPR pdp10_gimplify_va_arg_expr

#undef  TARGET_HAVE_SWITCHABLE_BSS_SECTIONS
#define TARGET_HAVE_SWITCHABLE_BSS_SECTIONS true

struct gcc_target targetm = TARGET_INITIALIZER;


/**********************************************************************

	Run-time Target Specifiation

**********************************************************************/

static unsigned int available_regs (void);

/* Which processor to tune for.  */
int target_tune = 0;

/* Which code model to use.  */
int target_model = MODEL_NONE;

/* A table to match the canonical -mcpu name with the numerical cpu
   type, and also two bit masks to enable or disable features.  */
static struct { const char *name; int cpu; int enable; int disable; }
cpu_table[] =
{
  /* name,	cpu,		enable,		disable */

  { "xkl1",	CPU_XKL1,	MASK_EXTENDED,	0		},
  { "xkl2",	CPU_XKL2,	MASK_EXTENDED,	0		},
};

/* function prototypes */

static int encode_ps  (int pos, int size);
static int bytesize_for_ps  (int ps);
static int convert_ps  (int oldps, int from, int to, int byte);
static void decode_plus_operand  (rtx x, int *ps, HOST_WIDE_INT *offset);
static void	normalize_bitfield_mem  (rtx *x, int *pos);
static rtx	pdp10_find_last_value  (rtx x, rtx *pinsn);
static const char *
		output_halfword_move  (rtx reg, rtx x, int reg_right,
					      int mem_right, int store,
					      int extend, int sign);
static const char *
		output_bitfield_move  (FILE *stream, rtx insn,
					      int store, int sign, rtx reg,
					      rtx x, int len, int bit, char code);
static int	use_xblt  (rtx destination, rtx source, rtx length);
static int	expand_blt  (rtx destination, rtx source, rtx length);
static int	expand_xblt  (rtx destination, rtx source, rtx length);
static int	expand_blt_clear  (rtx destination, rtx length);
static int	expand_xblt_clear  (rtx destination, rtx length);
/*
static int	expand_movslj  (rtx dest, rtx source, rtx length);
static int	expand_movst_clear  (rtx destination, rtx length);
*/
static int	clobbered_regs  (int *clobbered);
static int	index_register  (rtx x, int strict);
static const char *
		condition_string  (enum rtx_code condition);
static void	print_reg  (FILE *stream, int offset, int regno, int code,
				   enum PRINT_OP_CLASS class, enum PRINT_OP_TYPE type);
static void	print_byte_pointer  (FILE *stream, int ps, rtx x, HOST_WIDE_INT offset, int code,
				   enum PRINT_OP_CLASS class, enum PRINT_OP_TYPE type);
static void	print_word_address  (FILE *stream, rtx x, HOST_WIDE_INT offset, int code,
					    enum PRINT_OP_CLASS class, enum PRINT_OP_TYPE type);
/*
static void	print_operand_expression  (FILE *stream, rtx x, int code,
						  enum PRINT_OP_CLASS class, enum PRINT_OP_TYPE type);
*/
static int	decode_ps  (int ps, int *p, int *s);
static int	adjust_ps  (int ps, int *adjust);
static void	make_byte_type  (tree type, int precision,
					int unsignedp);
static void	layout_byte_decl  (tree decl);
static tree	pdp10_handle_size_attribute  (tree *node, tree name,
						     tree args, int flags,
						     bool *no_add_attrs);
static tree	pdp10_handle_fastcall_attribute  (tree *node, tree name,
							 tree args, int flags,
							 bool *no_add_attrs);

static void	long_long_71bit_callback  (cpp_reader *pfile);
static void	no_long_long_callback  (cpp_reader *pfile);
/*
static int	get_pragma_number  (cpp_reader *pfile,
					   const char *pragma);
*/

static rtx	expand_builtin_ffo  (tree arglist, rtx target);
static rtx	expand_builtin_fsc  (tree arglist, rtx target,
					    enum pdp10_builtins code);

static int	asciz_ok  (const char *ptr, int len);
static void	macro_output_ascii_bytes  (FILE *stream,
						  unsigned const char *ptr,
						  int len,
						  int byte_size);
static bool	maybe_output_extern  (struct hash_entry *he,
					     hash_table_key ptr);
static struct hash_entry *
		extern_symbol_newfunc  (struct hash_entry *he,
					       struct hash_table *table,
					       hash_table_key string);
static void	init_extern_symbol_table  (void);

static int	single_insn  (rtx insn, rtx label1, rtx label2);

static void	avoid_reg_equal  (rtx op0, rtx op1);
static int	maybe_expand_ibp  (int byte_size, rtx op0, rtx op1,
					  int increment);
static rtx	expand_add_or_sub_pointer  (rtx target, rtx op0,
						   tree type0, rtx op1,
						   tree type1, int add);
static rtx	expand_subtract_pointers  (enum machine_mode mode,
						  rtx target, rtx op0,
						  tree type0, rtx op1,
						  tree type1);

static void	expand_save_or_restore_return_address  (int save,
							       int clob_size);
static void	expand_save_or_restore_regs  (int save,
						     int *clobbered,
						     int clobbered_size,
						     int single);
static void	expand_set_up_frame_pointer  (void);
/* rtx convert_void_pointer (rtx op0, int to, int not_null); */
static rtx	convert_local_pointer  (rtx op0, int from,
					       int to, int stack);
/*static rtx	convert_global_pointer PARAMS ((rtx op0, int from,
						int to, int stack));*/
#if XKL_STUFF
static void	maybe_warn_about_pointer_conversion  (tree, int, int);
#endif
static tree	strip_conversions  (enum tree_code code, tree type,
					   tree exp);


/* Implement TARGET_ASM_INIT_SECTIONS.

   The PDP10 doesn't really have any noswitch sections, so we need to override or disable
   the noswitch sections the base code assumes exist.
   -mtc 10/12/2007
*/

static void
pdp10_init_sections (void)
{
  text_section = get_section (pdp10_text_psect, SECTION_CODE, 0x0);
  data_section = get_section (pdp10_data_psect, SECTION_WRITE, 0x0);
  readonly_data_section = get_section (pdp10_rodata_psect, 0, 0x0);
  bss_section = get_section (pdp10_bss_psect, SECTION_WRITE | SECTION_BSS, 0x0);

  /* these sections seem to be variants of bss so until we know more about them just map them to bss */
  tls_comm_section = bss_section;
  lcomm_section = bss_section;
  comm_section = bss_section;

  /* this noswitch section we're allowed to make null */
  bss_noswitch_section = 0x0;
}

/* Implement TARGET_ASM_SELECT_SECTION.
    based on default_select_section() except we add checking of align and setting
    SECTION_PALIGNED flag
    -mtc 11/14/2007
    add check for bss_section
    -mtc 1/18/2008
*/

section *
pdp10_select_section (tree decl, int reloc,
			unsigned HOST_WIDE_INT align)
{
  section *sect = data_section;

  if (DECL_P (decl))
    {
      if (decl_readonly_section (decl, reloc))
	sect = readonly_data_section;
      else if (categorize_decl_for_section (decl, reloc) == SECCAT_BSS)
	sect = bss_section;
    }
  else if (TREE_CODE (decl) == CONSTRUCTOR)
    {
      if (! ((flag_pic && reloc)
	     || !TREE_READONLY (decl)
	     || TREE_SIDE_EFFECTS (decl)
	     || !TREE_CONSTANT (decl)))
	sect = readonly_data_section;
    }
  else if (TREE_CODE (decl) == STRING_CST)
    sect = readonly_data_section;
  else if (! (flag_pic && reloc))
    sect = readonly_data_section;

  if (align > BITS_PER_WORD)
    sect->common.flags |= SECTION_PALIGNED;

  return sect;  
}

/* Implement TARGET_ASM_FUNCTION_RODATA_SECTION.

    -mtc 11/14/2007
*/
section *
pdp10_function_rodata_section (tree decl)
{
  if (decl != NULL_TREE && DECL_ALIGN(decl) > BITS_PER_WORD)
    {
    readonly_data_section->common.flags |= SECTION_PALIGNED;
    }

  return readonly_data_section;
}


/* Implement TARGET_ASM_SELECT_RTX_SECTION.
    for now always return readonly_data_section.
    Maybe should return data_section or somehow choose betewen them.

    -mtc 11/14/2007
*/
section *
pdp10_select_rtx_section (enum machine_mode mode ATTRIBUTE_UNUSED,
			    rtx x ATTRIBUTE_UNUSED,
			    unsigned HOST_WIDE_INT align)
{
  if (align > BITS_PER_WORD)
    readonly_data_section->common.flags |= SECTION_PALIGNED;

    return readonly_data_section;
}


/* Encode a PS value.
   Use a global format PS for extended machines if possible */
/* note that the pos and size parameters are leading bit number and bitsize
     while the encoded ps is the pdp10 byte pointer value
     -mtc 6/14/2006
*/

static int
encode_ps (int pos, int size)
{
  int ps;
  /* ensure pos is in range of 0 - 35 */
  if (pos >= 36 || pos < 0 )
    abort ();

  if (TARGET_EXTENDED)
    {
      if ((0 == (pos % size)) && VALID_OWGBP_BYTE_SIZE_P(size))
	{
	  ps = pdp10_ps_base_for_bytesize(size) + pos/size;
	  ps = ps << 6;
	  return ps;
	}
    }
  ps = ((36 - pos - size) << 6) | (size & 077);
  
  return ps;
}

/* Decode PS, the 12-bit P and S field from a byte pointer, and store
   the resulting byte position in *P and byte size in *S.  Return 0 if
   the PS represents is a legal byte, or else -1.  */
/* note that the returned position is the bit number of the leading bit of
    the bitfield
   -mtc 6/14/2006
*/
static int
decode_ps (int ps, int *pp, int *sp)
{
  int p, s;

  if (ps >= 04500)
    {
      int base;

      ps = (ps >> 6) & 077;
      if (ps == 077)
	return -1;
      
      s = bytesize_for_ps (ps);
      base = pdp10_ps_base_for_bytesize (s);
      p = s * (ps - base);
    }
  else
    {
      s = ps & 077;
      if (s > 36)
	return -1;
      p = 36 - s - (ps >> 6);
    }

  *sp = s;
  *pp = p;
  return 0;
}

/* determine the appropriate extended format PS base value for a given byte size */
int
pdp10_ps_base_for_bytesize (int size)
{
  switch (size)
    {
    case 6:
      return 046;
    case 7:
      return 062;
    case 8:
      return 055;
    case 9:
      return 070;
    case 16:
    case 18:
      return 075;
    case 32:
    case 36:
      return 0;
    default:
      abort ();
    }
}

/* deduce the byte size from a global format ps */
/* if ps is not a valid global format ps value abort
    except for a value of zero, for which we return 36 in the hope we can treat it
    as a word address
    -mtc 5/23/2006
*/
static int
bytesize_for_ps (int ps)
{
  int bs;

  bs = ps == 0 ? 36 :
    ps < 045 ? 0 :
    ps < 054 ? 6 :
    ps < 061 ? 8 :
    ps < 067 ? 7 :
    ps < 074 ? 9 :
    ps < 077 ? 18 : 0;
  
  if (!bs)
    abort();

  return bs;
}

/* convert an existing ps of width 'from' into a new ps of width 'to'.
   If 'from' is wider then 'to', restrict the offset to the maximum
   that the new ps can actually support.  If we are converting from
   a 36 bit word (old ps is therefore '0'), then use 'byte' as the 
   explicit offset we want */
/* Conversion between different sizes when neither size is 36 makes
     no sense to me.  We might consider disallowing it.
     -mtc 5/23/2006
*/

static int
convert_ps(int oldps, int from, int to, int byte)
{
  int limit = (36 / to) - 1; /* maximum character offset 'to' will support */
  int diff;
  int newps;

  if (from == to)
    return oldps;  /* nothing to do */

  if (36 == from)
    if (0 != oldps)
      abort();
    else
      diff = byte;  /* explicit selector */
  else
    diff = oldps - pdp10_ps_base_for_bytesize (from);

  if (36 == to)
    newps = 0;    /* a word pointer */
  else
    newps = pdp10_ps_base_for_bytesize (to);

  if (diff > limit)
    diff = limit; /* should warn here? */
  if (diff < 0)
    abort ();
  
  return newps + diff;
}

/* decode an operand of the form
   (plus (reg | symbol_ref | label_ref) (const_int))

   returning the symbolic address and an integer offset appropriate for the machine.
   If the expression explicitly constructs a byte pointer, extract the PS field as well */

static void decode_plus_operand(rtx x, int *ps, HOST_WIDE_INT *offset)
{
  int tmp_ps = 0;
  HOST_WIDE_INT tmp_offset = 0;

  /* prepare to discard values the caller doesn't care about */
  if (!ps) ps = &tmp_ps;
  if (!offset) offset = &tmp_offset;

  /* ensure we were called with something reasonable */
  if (PLUS != GET_CODE (x)
      || !((LABEL_REF == GET_CODE (XEXP (x, 0)) 
	    || (SYMBOL_REF == GET_CODE (XEXP (x, 0))) 
	    || ((REG == GET_CODE (XEXP (x, 0))))))
      || CONST_INT != GET_CODE (XEXP (x, 1)))
    abort ();

  (*offset) = INTVAL (XEXP (x, 1));

  /* if we came in with a ps value, just return
      -mtc 8/24/2007
  */
  if (*ps)
  	return;
  
  /* if the offset does not include a PS field, then this is indeed a word address */
  if (((*offset) & ~ADDRESS_MASK) == (((HOST_WIDE_INT)-1) & ~ADDRESS_MASK))
    {
      (*ps) = 0;
      (*offset) = -(-(*offset) & ADDRESS_MASK);
    }
  else
    {
      (*ps) = ((*offset) >> 24) & (TARGET_EXTENDED ? 07700 : 07777);
      (*offset) &= ADDRESS_MASK;
      if ((*offset) & ((HOST_WIDE_INT)1 << 29))
	{
	  if ((*ps) != 0)
/*	I think this is incorrect if !TARGET_EXTENDED
	-mtc 7/28/2006
*/
	    (*ps) += 0100;
	  (*offset) ^= (HOST_WIDE_INT)1 << 29;
	  (*offset) -= (HOST_WIDE_INT)1 << 29;
	}
    }
}

/* Establish an appropriate PS value for an operand
   based on the following considerations:

   1) if the operand explicitly constructs a constant byte pointer
   2) if the operand is a mem, and the mem mandates a byte pointer
   3) if the operand is a register, and we find that the insn that set it
      satisfies either (1) or (2).
   4) if the operand is a mem, and the mem is not word aligned.
   5) if the operand is a mem held in a register, then it must be a word pointer

   returns the PS field if it can be determined
   the illegal PS field 07700 if x is a run time evaluated byte pointer
   0 if the operand is a word address 

   If strict is set, a nonzero PS is returned only if the operand is a byte that
   is known to be referenced by a byte pointer.

   If strict is not set, a PS may be constructed for an operand if it is a byte 
   pointed to be a word pointer, */
int extract_ps (rtx x, rtx insn, int strict);

int extract_ps (rtx x, rtx insn, int strict)
{
  int ps = 0;
  rtx address = x;
  rtx mem = NULL_RTX;
  int memcount = 0;
  tree decl;
  HOST_WIDE_INT bitoffset;
  HOST_WIDE_INT bitsize;
  HOST_WIDE_INT alignment;
  enum machine_mode mode;
  HOST_WIDE_INT modebitsize;

  /* check for UNSPEC_ADJBP  
     If so, we are definitely dealing with a byte pointer whose value is known only at run time */
  if ((GET_CODE(x) == UNSPEC) && (XINT(x, 1) == UNSPEC_ADJBP))
    return 07700;
  
  /* check for UNSPEC_BP  
     If so, we are definitely dealing with an byte pointer whose value is known at compile time */
  if ((GET_CODE(x) == UNSPEC) && (XINT(x, 1) == UNSPEC_BP))
    {
      int pos = INTVAL (XVECEXP (x, 0, 1));
      int len = INTVAL (XVECEXP (x, 0, 2));
      return encode_ps(pos, len);
    }
  
  /* If a MEM, get the underlying address expression
     MEMs may be nested.  The innermost is the most interesting one */
  while (MEM == GET_CODE (address))
    {
      memcount++;
      mem = address;
      address = XEXP (address, 0);

/*
	It's not clear to me that it makes sense to walk past all the MEMs, in fact
	I'm inclined to think that the node below the first MEM is the interesting
	node and we should always stop there.  If we did that, the test further
	down would catch explicit byte pointers, but for now let's check along the
	way.  20010924-1.c is an example of a program where we fail to detect
	that an explicit byte pointer is because we're busy trying to ignore all the
	MEM nodes.
*/
      if ( GET_MODE(address) == Q9Pmode ||
	   GET_MODE(address) == Q8Pmode ||
	   GET_MODE(address) == Q7Pmode ||
	   GET_MODE(address) == Q6Pmode ||
	   GET_MODE(address) == HPmode )
	 {
	 return 07700;
	 }
    }

/* skip past a pre or post inc or dec node
    -mtc 12/13/2006
*/
  if (GET_CODE(address) == PRE_INC ||
  	GET_CODE(address) == PRE_DEC ||
  	GET_CODE(address) == POST_INC ||
  	GET_CODE(address) == POST_DEC )
  	address = XEXP(address, 0);

  /* if a CONST, get the underlying PLUS expression */
  if (CONST == GET_CODE(address))
    address = XEXP (address, 0);

  /* Is this an explicitly constructed constant byte pointer?
     If so, we have proof positive that it is a byte pointer
     If not, dig out the address, ignoring the const_int offset */

  if (PLUS == GET_CODE(address))
    {
      decode_plus_operand(address, &ps, NULL);
      if (ps) 
	return ps;
      else if (CONST_INT == GET_CODE (XEXP (address, 1)))
	address = XEXP (address, 0);
      else
	abort ();
    }

  /* Is the address simply a byte pointer? */

  if (	GET_MODE(address) == Q9Pmode ||
  	GET_MODE(address) == Q8Pmode ||
  	GET_MODE(address) == Q7Pmode ||
  	GET_MODE(address) == Q6Pmode ||
  	GET_MODE(address) == HPmode )
    {
    return 07700;
    }

/* Beyond here this is more guesswork than anything else.  If strict is set its time to give up.
    This test fixes execute/20000217-1.c and other -O1 compilation bugs
    -mtc 11/20/2006
*/

  if (strict)
  	return 0;
  
  /* If a MEM, look for a definition */

  if (mem) {
    bitoffset = (MEM_OFFSET(mem) ? INTVAL(MEM_OFFSET(mem)) *BITS_PER_UNIT : 0);
    bitsize = (MEM_SIZE(mem) ? INTVAL(MEM_SIZE(mem)) * BITS_PER_UNIT : 0);
    alignment = MEM_ALIGN(mem);
    mode = GET_MODE(mem);
    modebitsize = GET_MODE_BITSIZE(mode);
    decl = MEM_EXPR (mem);

/* If the mem attributes offer any hint that this is a byte pointer let's construct the PS based on the attribute values
    It's possible that we could grab something here that needed additional refinment from a bitfield definition, but we
    can deal with that when we encounter an example.  This definitely needs to take priority over things like records
    and unions, which may be word aligned as a whole, but we might be referencing just a portion of.
    But only if strict is not set, because in general these tests apply to byte values for which there is no explicitly
    constructed byte pointer.
    -mtc 6/21/2006
    We may want to consider returning zero if strict is set and these tests otherwise succeed.  Otherwise we're in the
    position of possibly computing ps two different ways with different non-zero results, which is an almost sure indication
    of a compiler bug.
    -mtc 7/3/2006
*/
    if (!strict && (bitoffset != 0 || bitsize < BITS_PER_WORD || alignment < BITS_PER_WORD || modebitsize < BITS_PER_WORD))
    	{
    	/* fields that cross word boundaries should be caught before getting here */
	if (bitoffset + bitsize > BITS_PER_WORD)
		abort();
	/* if the modebitsize is smaller than the field, we might as well combine the extractions into a single reference */
	if (modebitsize < bitsize)
		{
		bitoffset += bitsize - modebitsize;
		bitsize = modebitsize;
		}
	ps = encode_ps(bitoffset, bitsize);
	return ps;
    	}
	
    if (decl)
      {
	/* we have a variable declaration */
	if (DECL_P (decl))
	  {
	    tree type = TREE_TYPE (decl);
	    enum tree_code code = TREE_CODE (type);
	    
	    /* if a function argument, examine the declared type of the argument, not the passed type
                but only if there is a separate declared type
               -mtc 5/31/2006
               with 4.1.1 DECL_ARG_TYPE_AS_WRITTEN is gone so use DECL_ARG_TYPE
            */
	    if (PARM_DECL == TREE_CODE(decl) && DECL_ARG_TYPE(decl))
	      {
		type = DECL_ARG_TYPE (decl);
		code = TREE_CODE (type);
	      }
	    
	    /* check for aggregate and pointer types */
	    if (AGGREGATE_TYPE_P (type) || POINTER_TYPE_P (type))
	      {
		int size = 0;
		
		switch (code)
		  {
		  case RECORD_TYPE:
		  case UNION_TYPE:
		    /* These are always word aligned and are addressed as such */
		    return 0;
		    
		  case POINTER_TYPE:
		  case ARRAY_TYPE:
		    /* get the underlying data type and fall through */
		    type = TREE_TYPE(type);
		    /* but don't fall through if the underlying type is void */
		    if (TREE_CODE(type) == VOID_TYPE)
			return 0;
		  case REFERENCE_TYPE:
		    size = tree_low_cst(TYPE_SIZE(type), 1);
		    /*
		    Storing into a local array with an index of 0 comes through here and needs to return 0.
		    It's not clear what test really works, but I added counting the depth of mems and checking it
		    here to exclude that case.  see compile tests 920507-17.c, 950816-1.c and 20011109-1.c.
		    Could possibly restrict this change to ARRAY_TYPE.
		    Very likely this change breaks other tests.
		    Fixing this routine right needs to include removing inspecting the decl altogether as everthing
		    we need to know ought to be in the rtx.
		    */
		    if (size < 36 && memcount > 1)
		      return 07700;  /* a computed byte pointer, the PS is known only at run time */
		    else
		      return 0;      /* a word pointer */
		    break;
		    
		  default:
		    debug_tree(decl);
		    abort();
		  }
	      }
	    
#if 0
/*
	I think the following block is wrong.  A reference to a variable should be a computed
	byte pointer iff it is declared to be a pointer to a subword size.  Pointers should have been
	handled in the block above.
	-mtc 7/3/2006
*/
	    /* Do we have a scalar variable declaration? 
	       If so, determine it's alignment requirements */
	    if (VAR_DECL == TREE_CODE(decl))
	      {
		int align = DECL_ALIGN(decl);
		
		/* We have a byte pointer if it's alignment is sub-word */
		if (align < BITS_PER_WORD)
		  return 07700;  /* a computed byte pointer */
	      }
#endif		
	    
	    /* A word aligned scalar, but it may be of sub-word size.
	       we can construct a PS value unless strict is set */
	    if (strict)
	      return 0;  /* a word pointer */
	    else
	      {
		int size = tree_low_cst(TYPE_SIZE(type), 1);
		int pos = 36 - size;
		return encode_ps(pos, size);
	      }
	  }
    
	/* Not a variable declaration, so check for a reference to a component or bitfield within a struct.
	   If not strict, we can construct an appropriate PS to access the field member
	   Structs are themselves passed as word pointers.  */
	switch (TREE_CODE(decl))
	  {
	    int size, pos;
	    
	  case COMPONENT_REF:
	    {
	      /* a struct member.  
		 Tree Operand 0 refers to the entire struct or union.  
		 Tree Operand 1 is the field we want. */

	      tree field = TREE_OPERAND(decl, 1);

	      /* Its possible for the field to be a character array and its also
		  possible for the array to be dynamic in size which would make
		  the tree_low_cst abort.  But returning invalid PS causes us to
		  eventually just do the ldb or dpb ignoring the PS values, which
		  appears to be the right thing to do for the limited cases I've
		  examined */
	      if (!host_integerp (DECL_SIZE(field), 1))
		return 07700;
	      {
	      int field_offset = tree_low_cst(DECL_FIELD_OFFSET(field), 1);         /* in bytes from start of struct */
	      int field_bit_offset = tree_low_cst(DECL_FIELD_BIT_OFFSET(field), 1); /* in bits from field offset */
	      pos = field_bit_offset + (field_offset % UNITS_PER_WORD) * BITS_PER_UNIT;
	      size = tree_low_cst(DECL_SIZE(field), 1);                             /* in bits */
	      if (!strict && size < BITS_PER_WORD)
		return encode_ps(pos % BITS_PER_WORD, size);
	      else
		return 0;
	      }
	    }
	    
	  case BIT_FIELD_REF:
	    {
	      /* a bit field.  
		 Tree Operand 0 refers to the entire struct or union.  
		 Tree Operand 1 is the bit size
		 Tree Operand 2 is the bit pos from the start of the struct;
		 NOTE: TYPE_UNSIGNED determines if the bit field is unsigned or not
		 we ignore this for now, since byte pointers don't express this detail. */
	      tree field = decl;
	      size = tree_low_cst(TREE_OPERAND(field, 1), 1);
	      pos = tree_low_cst(TREE_OPERAND(field, 2), 1);
	      if (!strict && size < BITS_PER_WORD)
		return encode_ps(pos % BITS_PER_WORD, size);
	      else
		return 0;
	    }
	    
	  default:
	    break;
	  }
      }
  }
  
  /* If we have a mem that's not word aligned, then it can only be a byte pointer */
  if (mem && (MEM_ALIGN (mem) < BITS_PER_WORD))
    return 07700;
#if 0
    {
      /* deduce the ps value of the byte pointer */
      
      if (MEM_OFFSET (mem))
	offset = INTVAL (MEM_OFFSET (mem)) * MEM_ALIGN (mem);
      else
	offset = pdp10_pointer_offset (XEXP (mem, 0)) * BITS_PER_UNIT;
      offset /= MEM_ALIGN (mem);

      ps = encode_ps(offset*MEM_ALIGN (mem), MEM_ALIGN (mem)); 
      return ps;
    }
#endif

  /* if we have a mem that's a symbol ref then it's a word pointer */
  if (mem && GET_CODE(address) == SYMBOL_REF)
    return 0;

  /* if we have a mem that's a register pointer to a word aligned value,
     then it's necessarily pointing to a scalar */
  if (mem
      && BITS_PER_WORD <= MEM_ALIGN (mem)
      && REG == GET_CODE (address) 
      && REG_POINTER (address))
    return 0;

  /* if the address is held in a register, and we have the underlying insn,
     backtrack to find the insn that loaded the register and iterate
     KCG: This is getting pretty desperate, and I doubt that it works vrey often. */
  if (insn && REG_P (address))
    {
      address = pdp10_find_last_value (address, &insn);
      if (address)
	return extract_ps(address, insn, strict); /* recurse with the operand that loaded the register */
    }
  
  /* we've no clue what this is.  
     Die if strict.  If not strict, we will guess it's a word address. */
  if (strict)
    abort ();
  return 0;
}

/* Return the number of registers available for -mregparm,
   -mreturn-regs, and -mcall-clobbered.  
   This is basically the length of the contiguous register block between
   register zero (the scratch register) and the first fixed
   use register we see.  */
static unsigned int
available_regs (void)
{
  static unsigned int n = 0;
  int i;

  if (n > 0)
    return n;

  for (i = n = ASM_SCRATCH_REGNUM + 1; i < FIRST_PSEUDO_REGISTER; i++, n++)
    if (fixed_regs[i])
      break;

  return n;
}

/* Called by the OVERRIDE_OPTIONS macro.

   This is run after all command line options has been read, but
   before actual compilation has begun.  */
void
pdp10_override_options (void)
{
  unsigned int i;

  /* the options processing script always creates signed ints, so when we want an unsigned variable */
  /* we need to copy it here */
  if (opt_pdp10_regparm >= 0)
  	pdp10_regparm = opt_pdp10_regparm;
  if (opt_pdp10_retregs >= 0)
  	pdp10_retregs = opt_pdp10_retregs;
  if (opt_pdp10_clobberedregs >= 0)
  	pdp10_clobberedregs = opt_pdp10_clobberedregs;
  



  
  /* -mcpu is short for both -march and -mtune.  */
  if (pdp10_cpu)
    {
      pdp10_arch = pdp10_cpu;
      pdp10_tune = pdp10_cpu;
    }

  if (pdp10_arch || pdp10_tune)
    {
      unsigned int i;
      int found_arch = 0;
      int found_tune = 0;
      for (i = 0; i < ARRAY_SIZE (cpu_table); i++)
	{
	  if (pdp10_arch && strcmp (pdp10_arch, cpu_table[i].name) == 0)
	    {
	      found_arch = 1;
	      target_flags |= cpu_table[i].enable;
	      target_flags &= ~cpu_table[i].disable;
	    }

	  if (pdp10_tune && strcmp (pdp10_tune, cpu_table[i].name) == 0)
	  {
	    found_tune = 1;
	    target_tune = cpu_table[i].cpu;
	  }
	}
      if (pdp10_arch && ~found_arch)
	{
	  fprintf (stderr, "Unknown CPU: %s\n", pdp10_arch);
	  exit(1);
	}
      if (pdp10_tune && ~found_tune)
	{
	  fprintf (stderr, "Unknown CPU: %s\n", pdp10_tune);
	  exit(1);
	}
    }

  /* The default processor is XKL-1.  */
  if (target_tune == 0)
    {
    target_flags |= cpu_table[0].enable;
    target_flags &= ~cpu_table[0].disable;
    target_tune = CPU_XKL1;
    }

  /* Warn that the XKL-2 is experimental.  */
  if (target_tune == CPU_XKL2)
    fprintf (stderr, "Caution: The XKL-2 instruction set is experimental!\n");

  if (pdp10_model)
    {
      if (strcmp (pdp10_model, "small") == 0)
	target_model = MODEL_SMALL;
      else if (strcmp (pdp10_model, "medium") == 0)
	target_model = MODEL_MEDIUM;
      else if (strcmp (pdp10_model, "large") == 0)
	target_model = MODEL_LARGE;
      else if (strcmp (pdp10_model, "huge") == 0)
	{
	target_model = MODEL_LARGE;
	fprintf (stderr, "Model 'huge' is not presently supported.  Using 'large'\n");
	}
      else
	fprintf (stderr, "Unknown code model: %s\n", pdp10_model);
    }

  /* The default program model is small for unextended processors,
     and large otherwise.  */
     
  if (target_model == MODEL_NONE)
    {
      if (TARGET_EXTENDED)
	target_model = MODEL_LARGE;
      else
	target_model = MODEL_SMALL;
    }

  if (TARGET_EXTENDED)
    {
      if (!pdp10_text_psect)
	pdp10_text_psect = (char *)"text";

      if (!pdp10_rodata_psect)
	pdp10_rodata_psect = (char *)"rdat";

      if (!pdp10_data_psect)
	pdp10_data_psect = (char *)"dat";

      if (!pdp10_bss_psect)
	pdp10_bss_psect = (char *)"bss";
    }

  if (pdp10_regparm >= available_regs ())
	{
	  fprintf (stderr, "Bad value for -mregparm: %d\n", pdp10_regparm);
	  exit (1);
	}


  if (pdp10_retregs >= available_regs ())
	{
	  fprintf (stderr, "Bad value for -mreturn-regs: %d\n",
		   pdp10_retregs);
	  exit (1);
	}

  if (pdp10_clobberedregs > available_regs ())
	{
	  fprintf (stderr, "Bad value for -mcall-clobbered: %d\n", pdp10_clobberedregs);
	  exit (1);
	}
  
  if ((pdp10_clobberedregs > 0) && (pdp10_clobberedregs <= pdp10_retregs))
	{
	  fprintf (stderr, "-mcall-clobbered=%d clash with -mreturn-regs=%d\n",
		   pdp10_clobberedregs, pdp10_retregs);
	  exit (1);
	}

  if (pdp10_clobberedregs > 0)
  	{
	for (i = 0; i < pdp10_clobberedregs; i++)
		call_used_regs[i] = 1;
	for (; i < FIRST_PSEUDO_REGISTER; i++)
		call_used_regs[i] = fixed_regs[i];
  	}

  if (pdp10_char_bytesize != 6 && pdp10_char_bytesize != 7 && pdp10_char_bytesize != 8 && pdp10_char_bytesize != 9)
	{
	  fprintf (stderr, "Bad value for -mchar-bytesize: %d\n",
		   pdp10_char_bytesize);
	  exit (1);
	}

  /*
   * assign floating point types to modes
   */

  REAL_MODE_FORMAT(SFmode) = &pdp10_single_format;
  REAL_MODE_FORMAT(DFmode) = &pdp10_double_format;

}


/**********************************************************************

	Register Classes

**********************************************************************/

/* Called by the CONST_OK_FOR_LETTER_P macro.

   Defines the machine-specific operand constraint letters that
   specify particular ranges of integer values.  */
int
pdp10_const_ok_for_letter_p (HOST_WIDE_INT value, int c)
{
  switch (c)
    {
    case 'I': return value >= 0 && value <= 0777777;
    case 'J': return value <= 0 && value >= -0777777;
    case 'K': return value >= -0400000 && value <= 0377777;
    case 'L': return 
		0 != value
		&& (value & 0777777) == 0
		&& pdp10_const_ok_for_letter_p ((value >> 18) & 0777777, 'I');
    case 'M': return value == -1;
    case 'N': return
		(value & 0777777) == 0777777
		&& pdp10_const_ok_for_letter_p ((value >> 18) & 0777777, 'I');
    case 'O': return value == 0;
    case 'P': return pdp10_const_ok_for_letter_p (~value, 'I');
    }
  return 0;
}

/* Called by the CONST_DOUBLE_OK_FOR_LETTER_P macro.

   Defines the machine-dependent operand constraint letters that
   specify particular ranges of CONST_DOUBLE values.  */
int
pdp10_const_double_ok_for_letter_p (rtx x, int c)
{
  REAL_VALUE_TYPE r;
  long y[3];
  
  if (c != 'G')
    return 0;
  
  REAL_VALUE_FROM_CONST_DOUBLE (r, x);
  REAL_VALUE_TO_TARGET_DOUBLE (r, y);
  
  switch (GET_MODE (x))
    {
    case VOIDmode:
      return 0;
    case SFmode:
      return
	(y[0] & 0x00003fffUL) == 0
	&& (y[1] & 0xf0000000UL) == 0;
    case DFmode:
      return
	(y[0] & 0x00003fffUL) == 0
	&& (y[1] & 0xffffffffUL) == 0
	&& (y[2] & 0xff000000UL) == 0;
    default:
      abort ();
    }
}
/* Called by the EXTRA_CONSTRAINT macro.
   
   Defines the optional machine-dependent constraint letters that can
   be used to segregate specific types of operands for the target
   machine.  */
int
pdp10_extra_constraint (rtx x, int c)
{
  switch (c)
    {
    case 'Q':  /* the operand must be the constant 1 */
      return GET_CODE (x) == CONST_INT && INTVAL (x) == 1;

    case 'R':  /* the operand must be a SF or DF mode floating zero */
      return pdp10_const_double_0_operand (x, GET_MODE (x));

    case 'S':  /* the operand must be a symbolic non-byte-pointer constant */
      {
	HOST_WIDE_INT offset = 0;
	enum rtx_code code;
	int ps = 0;

	if ((TARGET_EXTENDED  && !(TARGET_SMALL || TARGET_MEDIUM))
	    || ((code = GET_CODE (x)) != SYMBOL_REF
		&& code != LABEL_REF
		&& code != CONST))
	  return 0;

	if (code == CONST)
	  {
	    x = XEXP (x, 0);
	    offset = INTVAL (XEXP (x, 1));
	    if ((offset & ~ADDRESS_MASK)
		!= (((HOST_WIDE_INT)-1) & ~ADDRESS_MASK))
	      ps = (offset >> 24) & 07700;
	  }
	return ps == 0;
      }

    case 'T':  /* the operand must be a constant between 0 and 511 */
      return GET_CODE (x) == CONST_INT && INTVAL (x) >= 0 && INTVAL(x) <= 511;

    }
  return 0;
}


/**********************************************************************

	Stack Layout and Calling Conventions

**********************************************************************/

/* Called by the INITIAL_ELIMINATION_OFFSET macro.  */
int
pdp10_initial_elimination_offset (int from, int to)
{
  if (from == ARG_POINTER_REGNUM || from == HARD_FRAME_POINTER_REGNUM)
    {
      int clobbered[FIRST_PSEUDO_REGISTER];
      int clobbered_size = clobbered_regs (clobbered);
      if (to == STACK_POINTER_REGNUM)
	return -((get_frame_size () + current_function_outgoing_args_size
		  + clobbered_size - UNITS_PER_WORD) / UNITS_PER_WORD);
      else if (to == HARD_FRAME_POINTER_REGNUM)
	return 0;
    }
  else if (from == FRAME_POINTER_REGNUM)
    {
      if (to == STACK_POINTER_REGNUM)
	return -((get_frame_size () + current_function_outgoing_args_size
		  - UNITS_PER_WORD) / UNITS_PER_WORD);
      else if (to == HARD_FRAME_POINTER_REGNUM)
	{
	  int clobbered[FIRST_PSEUDO_REGISTER];
	  int clobbered_size = clobbered_regs (clobbered);
	  return clobbered_size / UNITS_PER_WORD;
	}
    }

  abort ();
}

/* Stack layout, before function prologue:

                ...
         [     arg 6    ]
         [     arg 5    ]
   sp -> [return address]

   Stack layout, after fixed-arg function prologue:

                ...
         [     arg 6    ]
         [     arg 5    ]
	 [return address]
   fp -> [ saved reg 16 ]
         [ saved reg  5 ]
         [ saved reg  6 ]
         [ saved reg  7 ]
         [ saved reg 10 ]
         [ saved reg 11 ]   Registers 5 - 16 are saved if clobbered by
         [ saved reg 12 ]   the function.
         [ saved reg 13 ]
         [ saved reg 14 ]
         [ saved reg 15 ]
         [ frame slot 1 ]
         [ frame slot 2 ]   Local variables.
         [ frame slot 3 ]
               ...
         [outgoing arg 3]
         [outgoing arg 2]   Reserved space for outgoing arguments.
   sp -> [outgoing arg 1]

   Stack layout, after vararg function prologue:

                ...
         [     arg 6    ]
         [     arg 5    ]
         [     arg 4    ]   Args 1-4 has been pushed to stack.
         [     arg 3    ]
         [     arg 2    ]
         [     arg 1    ]
	 [return address]   Return address has been moved.
   fp -> [ saved reg 16 ]
         [ saved reg  5 ]
         [ saved reg  6 ]
         [ saved reg  7 ]
         [ saved reg 10 ]
         [ saved reg 11 ]   Registers 5 - 16 are saved if clobbered by
         [ saved reg 12 ]   the function.
         [ saved reg 13 ]
         [ saved reg 14 ]
         [ saved reg 15 ]
         [ frame slot 1 ]
         [ frame slot 2 ]   Local variables.
         [ frame slot 3 ]
               ...
         [outgoing arg 3]
         [outgoing arg 2]   Reserved space for outgoing arguments.
   sp -> [outgoing arg 1]

   Stack layout, after vararg function epilogue, but before return:

                ...
         [     arg 6    ]
         [     arg 5    ]
   sp -> [return address]   Return address has been moved back again.
*/

/* Called by the FUNCTION_PROLOGUE macro.  */
void
pdp10_output_func_prologue (FILE *stream, HOST_WIDE_INT frame_size ATTRIBUTE_UNUSED)
{
fputs("", stream);
}

/* Called by the FUNCTION_EPILOGUE macro.  */
void
pdp10_output_func_epilogue (FILE *stream, HOST_WIDE_INT frame_size ATTRIBUTE_UNUSED)
{
/*int firstlabel = get_first_label_num();
int lastlabel = max_label_num() - 1;
int i, j;*/
fputs("\tEPILOG\n\n", stream);
/*for (i=firstlabel; i<=lastlabel; i+=10)
	{
	fprintf(stream, "\tPURGE .L%d", i);
	for(j=i+1; j<i+10 && j <= lastlabel; j++)
		fprintf(stream, ",.L%d", j);
	fprintf(stream, "\n");
	}*/

}

/* Called by the FUNCTION_PROFILER macro.  */
void
pdp10_function_profiler (FILE *file ATTRIBUTE_UNUSED, int labelno ATTRIBUTE_UNUSED)
{
  /* unimplemented */
}


/**********************************************************************

	Addressing Modes

**********************************************************************/

/* Return nonzero if X is a valid index register, zero otherwise.  If
   STRICT is nonzero, all pseudo registers qualify.  */
static int
index_register (rtx x, int strict)
{
  if (GET_CODE (x) == SUBREG)
    x = SUBREG_REG (x);
  if (!REG_P (x))
    return 0;
  if (REGNO (x) < FIRST_PSEUDO_REGISTER)
    return REGNO_REG_CLASS (REGNO (x)) == INDEX_REGS;
  else
    return !strict;
}

/* Called by the GO_IF_LEGITIMATE_ADDRESS macro.  Returns nonzero if X
   is a valid address for accessing something of mode MODE.  */
int
pdp10_legitimate_address_p (enum machine_mode mode, rtx x, int strict)
{
  /* A constant address is always ok. */
  if (CONSTANT_ADDRESS_P (x))
    return 1;

  /* A constant value is always ok. */
  if (CONSTANT_P (x))
    return 1;

  switch (GET_CODE (x))
    {
      /* a label is always a legitimate address */
    case CODE_LABEL:
      return 1;
      
      /* dig out the register, if any */
    case SUBREG:
      x = SUBREG_REG (x);
      if (GET_CODE (x) != REG)
	return 0;
      /* Fall through.  */

      /* A register without an offset is always a valid address.
	 Register 0 can be used with indirect addressing.  */
    case REG:
      return !strict || REGNO (x) < FIRST_PSEUDO_REGISTER;

      /* A sum must be an index register plus a valid constant offset.  */
    case PLUS:
      if (!index_register (XEXP (x, 0), strict))
	return 0;

      /* We can always index with the frame or stack registers,
         to produce a valid address, but not with a byte pointer. 
         Byte pointers are beasts different. */
      if (REG_P (XEXP (x,0))
          && HARD_FRAME_POINTER_REGNUM != REGNO (XEXP (x,0))
          && STACK_POINTER_REGNUM  != REGNO (XEXP (x,0))
          && BYTE_MODE_P (mode))
	return 0;

      x = XEXP (x, 1);
      switch (GET_CODE (x))
	{
	case CONST_INT:
	  /* Checking can_create_pseudo_p() ensures that a global format
	     indirect word is not valid when generating RTL, but it's
	     valid when the instruction combination pass is running.  */
	  if (!can_create_pseudo_p())
	    return 1;
	  else
	    {
	      if (TARGET_EXTENDED)
		return INTVAL (x) >= -0400000 && INTVAL (x) < 0400000;
	      else
		return INTVAL (x) > -01000000 && INTVAL (x) < 01000000;
	    }
	case CONST:
	case LABEL_REF:
	case SYMBOL_REF:
	  return !TARGET_EXTENDED || TARGET_SMALL || TARGET_MEDIUM;
	default:
	  return 0;
	}

      /* Restrict indirect addressing  */
    case MEM:
      x = XEXP (x, 0);

      /* byte pointers are ok */
      /* but only if there aren't too many levels of indirection, so handle them */
      /* with other mem pointers below */
      /* if (BYTE_MODE_P (mode))
             return pdp10_legitimate_address_p (Pmode, x, strict);
      */

      /* register pointers are ok */
      if (REG_P (x))
	return index_register (x, strict);

      /* index expressions are ok within certain limits */
      if (GET_CODE (x) == PLUS)
	return
	  index_register (XEXP (x, 0), strict)
	  && (((!TARGET_EXTENDED || TARGET_SMALL || TARGET_MEDIUM)
	       && CONSTANT_ADDRESS_P (XEXP (x, 1)))
	      || (GET_CODE (XEXP (x, 1)) == CONST_INT
		  && pdp10_const_ok_for_letter_p (INTVAL (XEXP (x, 1)), 'K')));

      /* mem expressions are limited to two deep
	 including a MEM that might be above the original x passed in */
      if (MEM == GET_CODE (x))
	return 0;
 
      return pdp10_legitimate_address_p (BYTE_MODE_P (mode) ? Pmode : mode, x, strict);

    case UNSPEC:
      switch (XINT (x, 1))
	{
	case UNSPEC_ADDRESS:
	case UNSPEC_BP:
	  return 1;
	case UNSPEC_ADJBP:
	default:
	  return 0;
	}
    default:
      return 0;
    }
}

/* Called by the LEGITIMIZE_ADDRESS macro.

   Attempts to replace X with a valid memory address for an operand of
   mode MODE.  Returns NULL_RTX if no valid memory address was found.  */
rtx
pdp10_legitimize_address (rtx x, rtx oldx ATTRIBUTE_UNUSED, enum machine_mode mode)
{
  HOST_WIDE_INT max_offset = TARGET_EXTENDED ? 0377777 : 0777777;
  HOST_WIDE_INT min_offset = TARGET_EXTENDED ? -0400000 : -0777777;
  rtx x0, x1;

  if (mode == BLKmode)
    return NULL_RTX;

  /* collapse (mem (mem (...))) to (mem (reg )) */
  if (MEM == GET_CODE (x) && MEM == GET_CODE (XEXP (x, 0)))
    {
      rtx mem;
      rtx reg = gen_reg_rtx(GET_MODE(XEXP(x, 0)));
      emit_move_insn(reg, XEXP (x, 0));
      mem = gen_rtx_MEM(GET_MODE(x), reg);
      MEM_COPY_ATTRIBUTES(mem, x);
      return mem;
    }

  if (GET_CODE (x) != PLUS)
    return NULL_RTX;

  x0 = XEXP (x, 0);
  x1 = XEXP (x, 1);

  if (REG_P (x0)
      && GET_CODE (x1) == CONST_INT
      && (INTVAL (x1) < min_offset || INTVAL (x1) > max_offset))
    {
      HOST_WIDE_INT offset = INTVAL (x1);
      HOST_WIDE_INT mask = 0777777;
      HOST_WIDE_INT sign = 0400000;
      rtx high_reg = gen_reg_rtx (Pmode);
      rtx base_reg = gen_reg_rtx (Pmode);
      rtx high_part;

      if (!TARGET_EXTENDED)
	error ("address too large for unextended mode");
      else if (offset <= -(HWINT (1) << 30)
	       || offset >= (HWINT (1) << 30))
	error ("address too large for extended mode");

      high_part = GEN_INT (trunc_int_for_mode ((offset + sign) & ~mask, Pmode));
      /*emit_insn (gen_rtx_SET (VOIDmode, high_reg, gen_ADDRESS (high_part)));*/
      emit_move_insn (high_reg, high_part);
      emit_insn (gen_addsi3 (base_reg, x0, high_reg));
      return plus_constant (base_reg,
			    trunc_int_for_mode (((offset & mask) ^ sign) - sign,
						Pmode));
    }

  return NULL_RTX;
}


/**********************************************************************

	Dividing the Output into Sections

**********************************************************************/

/* Called by the TARGET_ENCODE_SECTION_INFO hook.

   DECL is a declaration.  If it's a function definition, note that
   the function is defined in the current translation unit.  Also
   record the type of all symbols.  */
void
pdp10_encode_section_info (tree decl, rtx rtl ATTRIBUTE_UNUSED, int new_decl_p ATTRIBUTE_UNUSED)
{
  if (TREE_CODE (decl) == FUNCTION_DECL)
    {
      /* Record whether the function is known to be defined in this
         unit or not.  */
      if (!TREE_PUBLIC (decl) && !DECL_WEAK (decl))
	SYMBOL_REF_INTERNAL (XEXP (DECL_RTL (decl), 0)) = 1;
    }
}


/**********************************************************************

	Describing Relative Costs of Operations

**********************************************************************/

/* Compute a (partial) cost for rtx X.  Return true if the complete
   cost has been computed, and false if subexpressions should be
   scanned.  In either case, *TOTAL contains the cost result.  */

bool
pdp10_rtx_costs (rtx x, int code, int outer_code, int *total)
{
  switch (code)
    {
    case MEM:
	{
	*total = (GET_MODE (x) == SImode ? COSTS_N_INSNS (1) / 4 : COSTS_N_INSNS (1) / 2);
	return false;
    	}

    case CONST_INT:
	{
	*total = pdp10_const_int_costs (INTVAL (x), outer_code);
	return true;
    	}
	
    case LABEL_REF:
    case SYMBOL_REF:
	{
	*total = TARGET_EXTENDED ? COSTS_N_INSNS (1) / 2: 0;
	return true;
    	}
	
    case CONST_DOUBLE:
	{
	*total = pdp10_const_double_costs (x, code, outer_code);
	return true;
    	}

    case PLUS:
    case MINUS:
    case AND:
    case IOR:
    case XOR:
	{
        *total =
      		(GET_MODE (x) == SImode
       			? COSTS_N_INSNS (1)
       			: HAVE_adddi3 ? COSTS_N_INSNS (2) : COSTS_N_INSNS (3));
	return false;
    	}

    case REG:
    case ADDRESS:
      /* Dunno.  Try this.  */
    case ASHIFT:
    case ASHIFTRT:
    case LSHIFTRT:
    case ROTATE:
    case ROTATERT:
	{
        *total = COSTS_N_INSNS (1);
	return true;
    	}
	
    case MULT:
	{
	if (optimize_size)
	/* Shift instructions are preferred, so multiplications are
		slightly more costly than one instruction.  */
		*total = COSTS_N_INSNS (3) / 2;
	else if (GET_MODE (x) == SImode)
		*total = COSTS_N_INSNS (5);
	else if (GET_CODE (x) == DImode)
		*total = COSTS_N_INSNS (10);
	else
		*total = COSTS_N_INSNS (20);
	return false;
    	}
	
    case UDIV:
    case UMOD:
	{
	if (!TARGET_XKL2)
		{
		*total = COSTS_N_INSNS (50);
		return true;
		}
    	}
    /* Fall through.  */
    case DIV:
    case MOD:
	{
	if (optimize_size)
	/* Shift instructions are preferred, so divisions are slightly
		more costly than one instruction.  */
		*total = COSTS_N_INSNS (3) / 2;
	else
		*total = COSTS_N_INSNS (20);
	return false;
    	}
	
    case NE:
	{
        *total = COSTS_N_INSNS (2);
	return false;
    	}
	
    case EQ: case GE: case GT: case LE: case LT:
    case GEU: case GTU: case LEU: case LTU:
	{
	if (optimize_size)
        	*total = COSTS_N_INSNS (3);
	else
        	*total = COSTS_N_INSNS (2);
	return false;
    	}
	
    case UNSPEC:
	{
        switch (XINT (x, 1))
		{
		case UNSPEC_ADJBP:
			*total =  COSTS_N_INSNS (100);
		case UNSPEC_ADDRESS:
		case UNSPEC_BP:
			*total =  COSTS_N_INSNS (1);
		default:
			/* There shouldn't be any other cases unless more are added.
			     Lie to force anyone adding a new code to add a case above */
			*total =  COSTS_N_INSNS (1000);
		}
	return false;
    	}

    default:
       return false;
    }
}


/* Describes the relative consts of CONST_INT expressions.  */
int
pdp10_const_int_costs (HOST_WIDE_INT value, enum rtx_code outer_code)
{
  char c = 'I';

  switch (outer_code)
    {
      /* Shift instructions take a signed 18-bit value.  */
    case ASHIFT:
    case ASHIFTRT:
    case LSHIFTRT:
    case ROTATE:
    case ROTATERT:
      c = 'K';
      break;

      /* TLZ and TRZ take 18-bit one's complemented values.  AND takes
	 an 18-bit value.  */
    case AND:
      if (CONST_OK_FOR_LETTER_P (value, 'N')	/* TLZ */
	  || CONST_OK_FOR_LETTER_P (value, 'P'))	/* TRZ */
	return 0;
      break;

      /* TLO and TLC take an unsigned 18-bit value in the upper 18
	 bits.  IOR and XOR take an unsigned 18-bit value.  */
    case IOR:
    case XOR:
      if (CONST_OK_FOR_LETTER_P (value, 'L'))	/* TLO and TLC */
	return 0;
      break;

    case SET:
      if (CONST_OK_FOR_LETTER_P (value, 'L')	/* MOVS */
	  || CONST_OK_FOR_LETTER_P (value, 'N')	/* HRLO */
	  || CONST_OK_FOR_LETTER_P (value, 'P'))	/* HRRO */
	return 0;
      /* Fall through.  */

    case PLUS:
      if (CONST_OK_FOR_LETTER_P (value, 'J'))	/* SUBI and MOVN */
	return 0;
      /* ADDI and MOVEI take an unsigned 18-bit value.  */
      break;

    case NEG:
      /* Changing this may affect right-shifts.  */
      return 0;

    default:
      break;
    }

  return CONST_OK_FOR_LETTER_P (value, c) ? 0 : COSTS_N_INSNS (1) / 4;
}

/* Describes the relative consts of CONST_DOUBLE expressions.  */
int
pdp10_const_double_costs (rtx x, enum rtx_code  code ATTRIBUTE_UNUSED, enum rtx_code outer_code ATTRIBUTE_UNUSED)
{
  /* A floating-point immediate costs nothing.  */
  if (pdp10_const_double_ok_for_letter_p (x, 'G'))
    return 0;

  return COSTS_N_INSNS (1);
}

/* Called by the TARGET_ADDRESS_COST macro.

   Gives the cost of an addressing mode that contains X.  */
int
pdp10_address_cost (rtx x)
{
  if (GET_CODE (x) == ADDRESS)
    return 0;
  else if (CONSTANT_ADDRESS_P (x))
    return
      TARGET_EXTENDED
      && !TARGET_SMALL
      && !TARGET_MEDIUM
      && GET_CODE (x) != LABEL_REF
      && !(GET_CODE (x) == SYMBOL_REF && SYMBOL_REF_INTERNAL (x))
      ? COSTS_N_INSNS (1) / 2 : COSTS_N_INSNS (1) / 4;
  else if (REG_P (x))
    return COSTS_N_INSNS (1) / 2;
  else if (GET_CODE (x) == PLUS)
    return COSTS_N_INSNS (1) / 2;
  else if (GET_CODE (x) == PRE_INC)
    return REG_P (XEXP (x, 0)) ?
      COSTS_N_INSNS (1) : COSTS_N_INSNS (2);
  else if (GET_CODE (x) == MEM)
    {
      x = XEXP (x, 0);
      if (REG_P (x))
	return COSTS_N_INSNS (2);
      else if (GET_CODE (x) == PLUS)
	return COSTS_N_INSNS (2);
      return COSTS_N_INSNS (3);
    }
  else if (GET_CODE (x) == SUBREG && REG_P (SUBREG_REG (x)))
    return COSTS_N_INSNS (1) / 2;
  else
    return MAX_COST;

  /* Not a valid address.  */
  debug_rtx (x);
  abort ();
  return -1;
}


/**********************************************************************

	Defining the Output Assembler Language

**********************************************************************/

/* Called by the ASM_DECLARE_OBJECT_NAME macro.

   Outputs to the stdio stream STREAM any text necessary for declaring
   the name NAME of an initialized variable which is being defined.  */
void
pdp10_asm_declare_object_name (FILE *stream, const char *name, tree decl ATTRIBUTE_UNUSED)
{
  /*int byte_size;*/

  pdp10_extern_symbol (name, PDP10_SYMBOL_DEFINED);

  pdp10_output_byte (stream, 0, 0);
  ASM_OUTPUT_LABEL (stream, name);

  /* If this is a scalar integer variable smaller than a word, justify
     it to the right-most addressable position inside a word.  */
/* If we want to have small scalars right aligned in words, we need
    to figure out how to create an appropriate description in the decl
    structure and then interpret it properly when referencing variables.
    Otherwise, we initialize one part of the word, but use another.
    -mtc 8/24/2006
*/
#if 0     
  if (TREE_CODE (decl) == VAR_DECL
      && TREE_CODE (TREE_TYPE (decl)) == INTEGER_TYPE
      && (byte_size = TYPE_PRECISION (TREE_TYPE (decl))) < 36)
    {
      int pad = 36 - byte_size - 36 % byte_size;
      pdp10_output_byte (stream, const0_rtx, pad);
    }
#endif
}

/* mangle names to preserve case-sensitivity
   we are using a non-case sensitive assembler

   If the name starts with a '*', we leave it alone
   
   '_' is unconditionally converted to '.' ('_' is an operator)

   Upper case characters are converted to lower case, preceeded by '$'
   No, we can't use '.' for this too, that would cause aliasing with
   names that incorporate '_'.

   We decorate upper case characters because C programs 
   for the most part tend to be written in lower case. 

   We need to remove interior '*' characters regardless of how names start, so
   always generate a copy of the name.  Don't remove leading '*', however, and
   use it's presence to control whether other internal substitutions are done.
   -mtc 9/12/2007
   */

const char *pdp10_macro_mangle_name(const char *name)
{
  char *mn;
  char c;
  bool star;
  static char *mangled_name;

  star = (*name == '*');

  mn = mangled_name = (char *) alloca (strlen(name)*2 + 1);
  
  while ( (c = *name++) ) {
    if (c == '*')
	c = '.';
    if (!star && c == '_')
	c = '.';
    if (!star && ISUPPER(c) )
      {
	c = TOLOWER(c);
	*mn++ = '$';
      }
    *mn++ = c;
  }
  *mn='\0';

  if (star)
	*mangled_name = '*';

  return xstrdup(mangled_name);
}


/* Called by the ASM_OUTPUT_LABELREF macro.

   Outputs to the stdio stream FILE a reference in assembler syntax to
   a label named NAME. 

   Trust pdp10_macro_mangle_name to return the correct value
   -mtc 9/12/2007
   */
void
pdp10_asm_output_labelref (FILE *file, const char *name)
{
  const char *mangled_name = pdp10_macro_mangle_name(name);
  if (*mangled_name == '*')
    {
      fputs (mangled_name + 1, file);
      return;
    }

  pdp10_output_byte (file, 0, 0);
  asm_fprintf (file, "%U%s", mangled_name);
}

/* Called by the ASM_OUTPUT_SYMBOL_REF macro.

   Outputs a reference to SYMBOL_REF SYM. 

   Trust pdp10_macro_mangle_name to return the correct value
   -mtc 9/12/2007
   */
void
pdp10_asm_output_symbol_ref (FILE *stream, rtx sym)
{
  tree id;
  const char *name = XSTR (sym, 0);
  const char *mangled_name = pdp10_macro_mangle_name(name);

  id = maybe_get_identifier (name);
  if (id)
    TREE_SYMBOL_REFERENCED (id) = 1;

  pdp10_extern_symbol (name, PDP10_SYMBOL_USED);

  if (*mangled_name == '*')
    {
    /*
	I haven't seen anything where the GIWs here are not redundant
	-mtc 7/26/2006
    
      if (TARGET_EXTENDED)
       asm_fprintf (stream, "GIW(0,0,%s)", name+1);
      else */
        asm_fprintf (stream, "%s", mangled_name+1);
     }
  else
    {
    /*
      if (TARGET_EXTENDED)
        asm_fprintf (stream, "GIW(0,0,%U%s)", mangled_name);
      else */
        asm_fprintf (stream, "%U%s", mangled_name);
    }
}

/* Indentation level for the instruction being output.  Indentation is
   enabled with the -mindent-skipped option.  */
int pdp10_opcode_indentation = 0;

/* Print instruction opcode name INSN on STREAM.  */
const char *
pdp10_print_insn (FILE *stream, const char *insn)
{
  static const char spaces[] = "          ";
  const int max_spaces = sizeof spaces - 1;
  static rtx last_output_insn = NULL_RTX;

  if (TARGET_ASM_INDENT_SKIPPED)
    {
      if (pdp10_opcode_indentation > max_spaces)
	pdp10_opcode_indentation = max_spaces;
      fputs (spaces + max_spaces - pdp10_opcode_indentation, stream);
    
      if (get_attr_skip (current_output_insn) == SKIP_YES
	  && current_output_insn != last_output_insn)
	pdp10_opcode_indentation++;
      else
	pdp10_opcode_indentation = 0;
      last_output_insn = current_output_insn;
    }

  while (*insn != ' ' && *insn != 0 && *insn != '%')
    {
      int ch = *insn++;
      if (TARGET_ASM_UPPER)
	ch = TOUPPER (ch);
      fputc (ch, stream);
    }
  return insn;
}

/* Return a string for a condition, as used in many conditional PDP-10
   instructions.  */
static const char *
condition_string (enum rtx_code condition)
{
  static const char *upper[] = { "", "L", "E", "LE", "A", "GE", "N", "G" };
  static const char *lower[] = { "", "l", "e", "le", "a", "ge", "n", "g" };
  const char **string = TARGET_ASM_UPPER ? upper : lower;

  switch (condition)
    {
   /* never: return string[0]; */
    case LT: return string[1];
    case EQ: return string[2];
    case LE: return string[3];
  /* always: return string[4]; */
    case GE: return string[5];
    case NE: return string[6];
    case GT: return string[7];
    default: abort ();
    }
}

/* This routine was totally messed up.  I hope fixing doesn't break a bunch of stuff
     -mtc 12/13/2006
*/
static enum rtx_code
reverse_condition_and_operands (enum rtx_code code)
{
  switch (code)
    {
    case EQ:
      return NE;
    case NE:
      return EQ;
    case GT:
      return LE;
    case GE:
      return LT;
    case LT:
      return GE;
    case LE:
      return GT;
    case GTU:
      return LEU;
    case GEU:
      return LTU;
    case LTU:
      return GEU;
    case LEU:
      return GTU;
    case UNORDERED:
      return ORDERED;
    case ORDERED:
      return UNORDERED;

    case UNLT:
      return UNGE;
    case UNLE:
      return UNGT;
    case UNGT:
      return UNLE;
    case UNGE:
      return UNLT;
    case UNEQ:
      return LTGT;
    case LTGT:
      return UNEQ;

    default:
      abort ();
    }
}

/* Called by the PRINT_OPERAND and PRINT_OPERAND_ADDRESS macros.

   PRINT_OPERAND_ADDRESS is simply PRINT_OPERAND with the 'a' format 
   code, using class PRINT_OP_LITERAL and type PRINT_OP_DIRECT.

   Output to the stdio stream STREAM the assembler syntax for an
   instruction operand X.  */
void
pdp10_print_operand (FILE *stream, rtx x, int code, enum PRINT_OP_CLASS class, enum PRINT_OP_TYPE type)
{
  rtx address;
  HOST_WIDE_INT offset;

  switch (code)
    {
    case 0:    /* no format code at all, treat as normal operand */
    case 'W':  /* Force a word address.  */
    case 'p':  /* flag a byte instruction, the operand is already a byte pointer */
      /* no special action here, handled by low layers of formatter */
      break;

      /* Optionally print a space.  */
    case '_':
      if (TARGET_ASM_INDENT_SKIPPED)
	fputc (' ', stream);
      return;

      /* 'Optionally print implied reference to register 0.  */
    case '@':
      if (TARGET_ASM_AC0)
	fputs ("0,", stream);
      return;

      /* Start a comment.  */
    case ';':
      fputc ('\t', stream);
      fputs (ASM_COMMENT_START, stream);
      return;

    case 'a':
      /* print an address  - invoked by PRINT_OPERAND_ADDRESS macro */
      break;

      /* Print stack register in a pdp10_push_operand or
         pdp10_pop_operand.  */
    case 'P':
      if (GET_CODE (x) != MEM
	  || (GET_MODE (x) != SImode && GET_MODE (x) != Pmode))
	abort ();
      x = XEXP (x, 0);
      if (GET_CODE (x) != PRE_INC && GET_CODE (x) != POST_DEC)
	abort ();
      x = XEXP (x, 0);
      if (GET_CODE (x) != REG)
	abort ();
      print_reg (stream, 0, REGNO (x), code, class, type);
      return;

      /* Print reversed condition */
    case 'R':
      fputs (condition_string (reverse_condition (GET_CODE (x))), stream);
      return;

      /* Print reversed condition and operands  */
    case 'r':
      fputs (condition_string (reverse_condition_and_operands (GET_CODE (x))), stream);
      return;

      /* Print floating-point immediate (constraint 'G').  
	 The immediate is interpreted by the assembler as E,,0
	 eg, it sets the exponent and a 9 bit fraction */
/* print the value as octal
    -mtc 9/11/2007
*/
    case 'G':
      if (GET_MODE (x) != SFmode && GET_MODE (x) != DFmode)
	abort ();
      gcc_assert (pdp10_const_double_ok_for_letter_p(x, 'G'));
      {
	REAL_VALUE_TYPE r;
	long y[3];
        HOST_WIDE_INT z;
	
	REAL_VALUE_FROM_CONST_DOUBLE (r, x);
        REAL_VALUE_TO_TARGET_DOUBLE (r, y);
	z = (y[0] >> 14) & 0x3ffff; /* mask should be redundant but let's make absolutely sure we don't include weird bits */

        pdp10_print_unsigned (stream, z);
      }
      return;

      /* A, B, D, i literal operands.  Verify and flag as literals */
    case 'A':
      /* Print first word of an addressable double-word literal constant.  */
    case 'B':
      /* Print second word of an addressable double-word literal constant.  */
    case 'D':
      /* Print both words of an addressable double-word literal constant.  */
      if (GET_CODE (x) != CONST_INT && GET_CODE (x) != CONST_DOUBLE)
	abort ();
    case 'i':
      /* Print a constant expression (matching constraint 'i') as a literal constant */ 
      class = PRINT_OP_LITERAL;
      break;

    case 'j':
	/* Special print class for CALL instructions */
	/* This allows us to revert the 'i' changes of 8/11/2008 that were done for the CALL macro */
	/* Plus handle non-DUALSTACK differently to fix problems with outputting @ */
	/* because in single stack mode the CALL macro implicitly inserts an @ and we can't have two */
	/*    -mtc 4/27/2011 */
      if (TARGET_DUALSTACK)
      	{
	if (GET_CODE(x) != REG && GET_CODE(x) != MEM)
	        class = PRINT_OP_LITERAL;
      	}
      else
      	{
      	if (GET_CODE(x) != REG)
      		{
		class = PRINT_OP_LITERAL;
		type = PRINT_OP_POINTER;
      		}
      	}
      break;

      /* I immediate operand for instructions like shift */
    case 'I':
      /* *********************  */
      if ((GET_CODE(x) == MEM) | (GET_CODE(x) == REG))
      	{
	  	type=PRINT_OP_POINTER; /* MEM nodes will become PRINT_OP_INDIRECT in pdp10_print_operand_expression */
      	}
      break;

      /* C, N, Q, S, T, U, V, Y immediate operands.  Transform the operand and drop code */
    case 'C':
      /* Print one's complement of an immediate integer constant.  */
      if (GET_CODE (x) != CONST_INT)
	abort ();
      x = GEN_INT (~INTVAL (x));
      class = PRINT_OP_IMMEDIATE;
      code = 0;
      break;
      
    case 'N':
      /* Print negated immediate integer constant.  */
      if (GET_CODE (x) != CONST_INT)
	abort ();
      x = GEN_INT (-INTVAL (x));
      class = PRINT_OP_IMMEDIATE;
      code = 0;
      break;
      
    case 'Q':
      /* Print lower half of a immediate constant integer  */
      if (GET_CODE (x) != CONST_INT)
	abort ();
      x = GEN_INT (((INTVAL (x) & 0777777) ^ 0400000) - 0400000);
      class = PRINT_OP_IMMEDIATE;
      code = 0;
      break;
      
    case 'S':
      /* Print upper half of an immediate integer constant.  */
      /* must satisfy the 'L' constraint, eg the right half is all 0's */
      if (GET_CODE (x) != CONST_INT
	  || !pdp10_const_ok_for_letter_p (INTVAL (x), 'L'))
	abort ();
      x = GEN_INT ((INTVAL (x) >> 18) & 0777777);
      class = PRINT_OP_IMMEDIATE;
      code = 0;

      /* Handle a few values that look like specific instructions specially
          -mtc 9/25/2009
      */
      {
      int sval = INTVAL(x); 
      int icode = sval >> 9;
      int acc = (sval & 0740) >> 5;
      int ibit = (sval & 0020) >> 4;
      int xreg = sval & 0017;
      switch (icode)
      	{
	case 0254:
		if (acc == 0 && xreg == 0)
			{
			if (ibit)
				fputs ("(<jrst @>)", stream);
			else
				fputs ("(<jrst >)", stream);
			return;
			}
		break;
		
	case 0200:
		if (xreg == 0 && ibit == 0)
			{
			fprintf (stream, "(<move %s,>)", reg_names[acc]);
			return;
			}

	default:
		break;
      	}
      }
      break;
      
    case 'T':
      /* Print upper half of the complement of an immediate integer constant.  */
      /* must satisfy the 'N' constraint, eg, the right half is all 1's */
      if (GET_CODE (x) != CONST_INT
	  || !pdp10_const_ok_for_letter_p (INTVAL (x), 'N'))
	abort ();
      x = GEN_INT (((~INTVAL (x)) >> 18) & 0777777);
      class = PRINT_OP_IMMEDIATE;
      code = 0;
      break;

    case 'U':
      /* Print upper half of an immediate integer constant  */
      if (GET_CODE (x) != CONST_INT)
	abort ();
      x = GEN_INT ((((INTVAL (x) >> 18) & 0777777) ^ 04000000) - 04000000);
      class = PRINT_OP_IMMEDIATE;
      code = 0;
      break;

    case 'V':
      /* Print 72 bit canonical rotate count.  */
    case 'Y':
      /* Print 36 bit canonical rotate count.  */
      if (GET_CODE (x) != CONST_INT)
	abort ();
      {
	HOST_WIDE_INT i = INTVAL (x);
	HOST_WIDE_INT n = code == 'Y' ? 36 : 72;
	i %= n;
	if (i < 0)
	  i += n;
	if (i > n / 2)
	  i = i - n;
	x = GEN_INT (i);
      }
      class = PRINT_OP_IMMEDIATE;
      code = 0;
      break;

    case 'X':
      /* print an operand as a word pointer (for xmovei/movei) */
      type = PRINT_OP_POINTER;
      code = 'W';
      break;

      /* Print next location, either register or memory  */
    case 'Z':
      switch (GET_CODE (x))
	{
	case REG:
	  print_reg (stream, 0, (REGNO (x) + 1) % 020, code, class, type);
	  return;

	case MEM:
	  /* can't handle (mem (mem)) */
	  if (GET_CODE (XEXP (x, 0)) == MEM)
	    abort ();
	  /* might already have a (PLUS (address) (const_int)), look for it
	     If we find it, just add one more to the const_int */
	 /* test the right node for CONST_INT
	     -mtc 12/13/2007
	 */
	  if (GET_CODE (XEXP (x, 0)) == PLUS && GET_CODE (XEXP (XEXP (x, 0), 1)) == CONST_INT)
	    {
	      address = XEXP (XEXP (x, 0), 0);
	      offset = INTVAL (XEXP (XEXP (x, 0), 1));
	    }
	  else
	    {
	      address = XEXP(x, 0);
	      offset = 0;
	    }
	  offset += 1;
	  address = plus_constant (address, offset);
	  x = gen_rtx_MEM(GET_MODE(x),  address);
	  break;
	  
	default:
	  debug_rtx (x);
	  abort ();
	}
      break;

    default:
      abort ();
    }

/* handle register renaming here even though it ought to be filtered out upstream somewhere
    -mtc 8/23/2007
    modify test to handle lowpart references as well as same size renaming
    -mtc 8/24/2007
    handle (some cases of) multi-word inner mode size
    -mtc 5/3/2011
*/
  while (GET_CODE(x) == SUBREG)
  	{
  	if (SUBREG_BYTE(x) == subreg_lowpart_offset(GET_MODE(x), GET_MODE(SUBREG_REG(x))))
  		{
  		if (GET_MODE_SIZE(GET_MODE(SUBREG_REG(x))) > UNITS_PER_WORD)
  			{
  			if (GET_CODE(SUBREG_REG(x)) != REG)
				abort();
			int regno = REGNO(SUBREG_REG(x));
			int outerwordsize = (GET_MODE_SIZE(GET_MODE(x)) + UNITS_PER_WORD - 1) / UNITS_PER_WORD;
			int innerwordsize = (GET_MODE_SIZE(GET_MODE(SUBREG_REG(x))) + UNITS_PER_WORD - 1) / UNITS_PER_WORD;
			print_reg (stream, 0, (regno + innerwordsize - outerwordsize) % 020, code, class, type);
			return;
  			}
		else
			x = SUBREG_REG(x);
  		}
	else
		abort();
  	}

  switch (GET_CODE (x))
    {

    case LT: case EQ: case LE: case GE: case NE: case GT:
      fputs (condition_string (GET_CODE (x)), stream);
      break;

    case CONST_DOUBLE:
      class = PRINT_OP_LITERAL;
      type = PRINT_OP_DIRECT;
      /* fall through */
    case REG:
    case MINUS:
    case CODE_LABEL:
      code = 'W';
      /* fall through */
    case CONST_INT:
    case PLUS:
    case CONST:
    case LABEL_REF:
    case SYMBOL_REF:
    case MEM:
    case UNSPEC:
      pdp10_print_operand_expression (stream, x, code, class, type);
      break;

    default:
      debug_rtx (x);
      abort ();
    }
}

/* Print a register expression 

   Note:  If dealing with a byte pointer (code 'p') then
   we must remove a level of indirection from the operand as
   byte instructions inherently perform a level of indirection themselves */

static void
print_reg (FILE *stream, int offset, int regno, int code, enum PRINT_OP_CLASS class, enum PRINT_OP_TYPE type)
{
  int global;

  /* class IMMEDIATE adorns an operand for use as immediate data.
     class LITERAL adorns an operand for use as a literal value.
     class OPERAND adorns an operand for normal use.
     
                            ---local---           ---global---  

     class immediate &&
     type direct        ==> @Rn                    @Rn
     type pointer       ==>  LIW(0,Rn,offset18)     GIW(0,Rn,offset30)
     type indirect      ==>  LIW(1,Rn,offset18)     GIW(1,Rn,offset30)

     class literal &&
     type direct        ==> [LIW(0,Rn,0)]          [GIW(0,Rn,0)]
     type pointer       ==> [LIW(0,Rn,offset18)]   [GIW(0,Rn,offset30)]
     type indirect      ==> [LIW(1,Rn,offset18)]   [GIW(1,Rn,offset30)]

     class operand &&
     type direct        ==>  Rn                     Rn
     type pointer       ==>  offset18(Rn)         @[GIW(0,Rn,offset30)]
     type indirect      ==> @offset18(Rn)         @[GIW(1,Rn,offset30)] 

     LIW is a compiler macro for a local format indirect word.
     GIW is a compiler macro for a global format indirect word.

     This routine deals with offsets of up to 30 significant bits.
     If it's given an offset greater than 30 bits, it will abort

     The operand/direct requires that the offset be 0 */

  const char *(prefix[2/*local/global*/][PRINT_OP_CLASSMAX][PRINT_OP_TYPEMAX]) = {
    { {"@", "LIW(0,", "LIW(1," }, {"[LIW(0,", "[LIW(0,", "[LIW(1,"},  {"", "", "@"} },
    { {"@", "GIW(0,", "GIW(1," }, {"[GIW(0,", "[GIW(0,", "[GIW(1," }, {"", "@[GIW(0,", "@[GIW[1,"} }
  };
  const char *(suffix[2/*local/global*/][PRINT_OP_CLASSMAX][PRINT_OP_TYPEMAX]) = {
    { {"", ")", ")" }, {")]", ")]", ")]"}, {"", ")", ")"} },
    { {"", ")", ")" }, {")]", ")]", ")]"}, {"", ")]", ")]"} }
  };
  int reg_index[2/*local/global*/][PRINT_OP_CLASSMAX][PRINT_OP_TYPEMAX] = {
    { {0, 0, 0}, {0, 0, 0}, {0, 1, 1} },
    { {0, 0, 0}, {0, 0, 0}, {0, 0, 0} }
  };
  int is_macro[2/*local/global*/][PRINT_OP_CLASSMAX][PRINT_OP_TYPEMAX] = {
    { {0, 1, 1}, {1, 1, 1}, {0, 0, 0} },
    { {0, 1, 1}, {1, 1, 1}, {0, 1, 1} }
  };

  /* verify that regno is valid */
  if (regno >= FIRST_PSEUDO_REGISTER)
  	abort();

  /* compensate for byte instruction intrinsic indirection */
  if ('p' == code)
  	{
	if (PRINT_OP_DIRECT < type)
		type -= 1;
	else
		abort ();
  	}
  /* if the offset is not expressible as an 18 bit signed, use global format */
  global = 0;
  if ((offset < ((HOST_WIDE_INT) -0400000)) || (offset > ((HOST_WIDE_INT) 0377777)))
    global = 1;
  /* if the offset is not expressible as a 30 bit signed, we are screwed */
  else if ((offset < ((HOST_WIDE_INT) -04000000000)) || (offset > ((HOST_WIDE_INT) 03777777777)))
    abort ();

  /* If we can't do global addressing, then force local addressing */
  if ((!TARGET_EXTENDED) || TARGET_SMALL || TARGET_MEDIUM)
    global = 0;
 
  /* check arguments */
  /* Direct usage requires a zero offset */

  if (offset && (type == PRINT_OP_DIRECT))
    abort ();

  /* print prefix, "offset("register -or- register",offset", suffix */

  fputs(prefix[global][class][type], stream);

  if (reg_index[global][class][type])
    {
      pdp10_print_integer (stream, offset);
      fputc ('(', stream);
    }
  
#ifdef REGISTER_PREFIX
  fputs (REGISTER_PREFIX, stream);
#endif
  fputs (reg_names[regno], stream);
  
  if (is_macro[global][class][type])
    {
      fputc (',', stream);
      if (global)
        pdp10_print_number (stream, offset, 1, 30, 0);
      else
        pdp10_print_number (stream, offset, 1, 18, 0);
    }
  
  fputs (suffix[global][class][type], stream);

  /* record use of global formats */
  if (global)
      pdp10_giw +=1; 
}

/* Output a byte pointer referencing the pos,size byte at address offset + x.
   NOTE: byte pointers are normally INDIRECT, but may also be POINTER */
static void
print_byte_pointer (FILE *stream, int ps, rtx x, HOST_WIDE_INT offset, int code ATTRIBUTE_UNUSED,
					enum PRINT_OP_CLASS class, enum PRINT_OP_TYPE type)
{
  int pos = 0;
  int size = 0;

  /* class IMMEDIATE adorns an operand for use an immediate value.
     class LITERAL adorns an operand for use as an literal value.
     class OPERAND adorns an operand for normal use.

                              ---local---                 ---global---  

     class immediate &&
     type direct           ==> Invalid                       Invalid
     type pointer         ==> Invalid                       Invalid
     type indirect        ==> POINT(p,s,addr_expr)          OWGBP(p,s,addr_expr)       ==> one word format
                                                            TWGBP(p,s,addr_expr)       ==> two word format
     class literal &&
     type direct           ==> Invalid                       Invalid
     type pointer         ==> [POINT(p,s,addr_expr)]       [OWGBP(ps,addr_expr)]       ==> one word format
                                                           [TWGBP(p,s,addr_expr)]      ==> two word format
     type indirect        ==> [POINT(p,s,addr_expr)]       [OWGBP(ps,addr_expr)]       ==> one word format
                                                           [TWGBP(p,s,addr_expr)]      ==> two word format
     class operand &&
     type direct           ==> Invalid                       Invalid
     type pointer         ==> Invalid                       Invalid
     type indirect        ==> [POINT(p,s,@addr_expr)]     @[OWGBP(ps,addr_expr)]       ==> one word format
                                                          @[TWGBP(p,s,addr_expr)]      ==> two word format

     addr_expr is an immediate mode address expression output by print_word_address

     POINT is an assembler built-in
     OWGBP is a compiler macro for One Word Global Byte Pointers
     TWGBP is a compiler macro for Two Word Global Byte Pointers
  */

  /* trap illegal flags and values */
  if (!x || PRINT_OP_DIRECT == type || 0 == ps)
    abort();

  /* PRINT_OP_POINTER means we need to construct a pointer literal that references a register */
  if (type== PRINT_OP_POINTER && ((class != PRINT_OP_LITERAL) || (offset != 0)))
    abort();
  
  /* decode the ps value */
  if (-1 == decode_ps (ps, &pos, &size))
    abort ();

  if (class == PRINT_OP_OPERAND)
    {
    fputc ('@', stream);
    fputc ('[', stream);
    }
  else if (class == PRINT_OP_LITERAL)
    fputc ('[', stream);

  if (TARGET_EXTENDED)
    {
      /* Use a global byte pointer, either one or two word

	 A one word global byte pointer has many limitations
	 Among the less obvious ones is that the address cannot be indexed with a register
	 Also, we must force a 30 bit address (using 'w') with a OWGBP */
      if ((REG != GET_CODE(x)) && (ps > 04400))
	{
	  fprintf (stream, "OWGBP(%02o,",ps>>6);
	  if (type == PRINT_OP_POINTER)
	  	{
	  	if (REGNO(x) >= FIRST_PSEUDO_REGISTER)
			abort();
	  	fprintf(stream, "LIW(0, 0,%s)", reg_names[REGNO(x)]);
	  	}
	  else
	  	/* This offsettting of the type is weird.  Probably it would be better to change
	  	     the places print_byte_pointer is called to not increment type when calling it
	  	     and to fix up all the comments.  Then type values of PRINT_OP_DIRECT,
	  	     PRINT_OP_POINTER and PRINT_OP_DIRECT would all be possible.
	  	     Right now, it's possible to have a value 1 greater than PRINT_OP_INDIRECT!
	  	     -mtc 12/5/2006
	  	     Change this to just always use PRINT_OP_DIRECT and avoid the GIW() wrapper
	  	     in print_word_address().  Actually, if we get here, type should be guaranteed to be
	  	     PRINT_OP_INDIRECT.  The other uses of type-1 below should probably also be reviewed and
	  	     changed to the appropriate constant values.
	  	     -mtc 9/20/2007
	  	*/
		print_word_address (stream, x, offset, 'w', PRINT_OP_IMMEDIATE, PRINT_OP_DIRECT);
	  fputc (')', stream);
	}
      else
	{
	  if (type == PRINT_OP_POINTER)
	  	{
       	fprintf (stream, "TWGBP(%02o,%02o,", 36 - pos - size, size);
	  	if (REGNO(x) >= FIRST_PSEUDO_REGISTER)
			abort();
	  	fprintf(stream, "LIW(0,0,%s)", reg_names[REGNO(x)]);
	  	}
	  else
	  	{
	  	/* This is very much a kludge.  20060412-1.c at level -O2 generates a constant expression that 
	  	    looks like an explicit byte point creation to pdp10_print_operand_expression().
	  	    What's really wanted is a one word literal with an offset and the high bit set, but it gets decoded
	  	    as a ps and print_byte_pointer gets called.  For the specific case, outputting OWGBP here should
	  	    be OK since it generates the correct bits, but there are at least two possible problems:
	  	    1) Not all 12-bit ps values are representable.  Some will error out because decode_ps() fails, 
	  	    others will have the low 6 bits of ps non-zero.
	  	    2) If print_byte_pointer() is sometimes called from somewhere else that legitimately requires TWGBP
	  	    with a size of 0, we'll output the wrong code here.
	  	    In either case, we'll need to change pdp10_print_operand_expression() to handle the missing or ambiguous
	  	    cases
	  	    -mtc 1/16/2008
	  	    Add check for REG_P(x) because OWGBPs can't express indirection with anything other than a constant or
	  	    symbol.
	  	    -mtc 1/29/2008
	  	*/
	  	if ((ps & 077)  == 0 && !REG_P(x))
	  		{
	       	fprintf (stream, "OWGBP(%02o,",ps>>6);
			print_word_address (stream, x, offset, 'W', PRINT_OP_IMMEDIATE, type-1);
	  		}
		else
			{
       		fprintf (stream, "TWGBP(%02o,%02o,", 36 - pos - size, size);
			print_word_address (stream, x, offset, 'W', PRINT_OP_IMMEDIATE, type-1);
			}
	  	}
	  fputc (')', stream);
	}
      pdp10_giw++;
    }
  else
    {
      /* Use a local byte pointer.  */
      fprintf (stream, "POINT %d,", size);
      if (type == PRINT_OP_POINTER)
	abort(); /* handle this if and when it comes up */
      else
	print_word_address (stream, x, offset, 'W', PRINT_OP_IMMEDIATE, type-1);
      fprintf (stream, ",%d", pos + size - 1);
    }
  if ((class == PRINT_OP_OPERAND) || (class == PRINT_OP_LITERAL))
    fputc(']',stream);
}

/* Appropriately adorn and print offset + x, a word pointer
   X must be a register, label_ref or symbol_ref */

static void
print_word_address (FILE *stream, rtx x, HOST_WIDE_INT offset, int code, enum PRINT_OP_CLASS class, enum PRINT_OP_TYPE type)
{
  int global = 0;      /* 1 ==> global format */
  rtx addr = x;
  HOST_WIDE_INT sign, mask;

  /* class IMMEDIATE adorns an operand for use as an immediate value
     class LITERAL adorns an operand for use as an literal value.
     class OPERAND adorns an operand for normal use.

                               ---local---         ---global---
     class immediate &&
     type direct          ==>  constant18           constant36  
     type pointer         ==>  LIW(0,0,address18)   GIW(0,0,address30)
     type indirect        ==>  LIW(1,0,address18)   GIW(1,0,address30)
     
     class literal &&
     type direct          ==> [constant18]          [GAW(constant36)]
     type pointer         ==> [LIW(0,0,address18)]  [GIW(0,0,address30)]
     type indirect        ==> [LIW(1,0,address18)]  [GIW(1,0,address30)]

     
     class operand &&
     type direct          ==>  constant18           [constant36]


BUG:  Should this be?
     type direct          ==>  constant18           @[constant36]
-mtc 9/11/2006

     
     type pointer         ==>  address18           @[GIW(0,0,address30)]
     type indirect        ==> @address18           @[GIW(1,0,address30)]

     This routine detects if it is given a register based address expression and
     defers to print_register for formatting if so.

     constant consists of an offset value only, a label or symbol is not allowed.
     address consists of a symbol or label plus an optional offset */

  /* TODO: Some of the formatting decisions might be better migrated to using the
	formatting code specifier capability of the machine definition.  In any case, the
	valid formats are heavily constrained by the actual machine instruction being
	generated.  Note that the use of class and type are PDP-10 specific. */
  
  const char *(prefix[2/*local*/][PRINT_OP_CLASSMAX][PRINT_OP_TYPEMAX]) = {
    { {"", "LIW(0,0,", "LIW(1,0," }, {"[",     "[LIW(0,0,", "[LIW(1,0,"}, {"",  "",       "@"} },
    { {"", "GAW(",     "GIW(1,0," }, {"[GAW(", "[GAW(",     "[GIW(1,0,"}, {"[", "@[GAW(", "@[GIW(1,0,"} }
  };
  const char *(suffix[2/*local*/][PRINT_OP_CLASSMAX][PRINT_OP_TYPEMAX]) = {
    { {"", ")", ")" }, {"]", ")]", ")]"}, {"", "", ""} },
    { {"", ")", ")" }, {")]", ")]", ")]"}, {"]", ")]", ")]"} }
  };

  /* adjust address class/type to reflect the underlying nature of the operator and/or operand */

  /* If we were given a register expression, immediately defer to print_register */
  if (x && (REG == GET_CODE (x))) {
    print_reg (stream, offset, REGNO(x), code, class, type);
    return;
  }

  /* account for byte instruction intrinsic indirection */
  if ('p' == code)
      {
      if (PRINT_OP_DIRECT < type)
	type -= 1;
      else
	abort ();
      }

#if 0
  /* bare symbol_ref expressions are interpreted as follows:
     (mem (symbol_ref))       is not pointer,  it is direct.
     (mem (mem (symbol_ref))) is not indirect, it is pointer

     I don't know why the compiler represents them this way, but it does */

  if (x && (SYMBOL_REF == GET_CODE (x)) && (0 == offset))
    {
      if (PRINT_OP_DIRECT < type)
	type -= 1;
    }
#endif

  /* compel a word address if %W is used, discarding any irrelevant bits of offset
     This will strip away byte pointer PS fields if they are present */
  if ('W' == code)
    {
      sign = offset & (((HOST_WIDE_INT) 1) << ((TARGET_EXTENDED) ? 29 : 17));
      mask = ~(sign - 1);
      offset |= mask;
    }
  
  /* Determine if the address expression is local or not

     Symbols are global if not defined within this compilation unit
     Labels are always local to the compilation unit.
     
     BUG1488 indicates that Labels should be referenced as globals
     If we need to be more discerning we could look at the format code, which is 'i' for
     that case, or perhaps something more.  If necessary, we could introduce more specific
     format letters in the .md file.
     
     If we were given anything else, abort now as we cannot handle it */
  if (x)
    {
    if ( GET_CODE (x) == SYMBOL_REF  || GET_CODE(x) == LABEL_REF)
	{
	global = 1;
	/* Also force SYMBOL_REF and LABEL_REF to not be PRINT_OP_DIRECT
	    symbols inherently need to be adjusted and cannot be literal 36-bit values
	    -mtc 3/9/2010
	    Limit forcing PRINT_OP_POINTER for SYMBOL_REFs to when the reference crosses
	    section boundaries because the linker will implicitly fix these up and we want
	    to avoid overloading the linker as much as we can.
	    -mtc 3/15/2010
	*/
	if (type == PRINT_OP_DIRECT)
		{
		if (GET_CODE(x) == LABEL_REF)
			type = PRINT_OP_POINTER;
		else
			{
			tree xtree = SYMBOL_REF_DECL(x);
			if ((xtree == NULL) || (pdp10_select_section(xtree,0,0) == in_section))
				type = PRINT_OP_POINTER;
			}
		}
	}

/*	variables are in a different .Psect and can be far away from the code segment even if they are 'local' so the following optimization attempt doesn't work
  	global = (SYMBOL_REF_INTERNAL (x) || pdp10_lookup_extern_symbol(XSTR (x, 0), PDP10_SYMBOL_DEFINED)) ? 0 : 1;
*/
      else if (GET_CODE(x) == CODE_LABEL) 
	global = 0;
      else
	abort ();
    }

  /* if we have an address expression (eg, we were given an 'x' along with an offset)
     the offset must be checked */
  if (x)
    {
      /* if we have both an offset (or a forcing code 'w') and a symbol, we must use global format
	 as there's no way to ensure the sum is itself valid within 18 bits.
	 We assume that label_ref will always be within 18 bits. */
      if (('w' == code || offset) && (SYMBOL_REF == GET_CODE(x)))
	global = 1;
      
      /* if the offset is not expressible as an 18 bit signed, use global format */
      if ((offset < ((HOST_WIDE_INT) -0400000)) || (offset > ((HOST_WIDE_INT) 0377777)))
	global = 1;
      
      /* if the offset is not expressible as a 30 bit signed, we are screwed */
      if ((offset < ((HOST_WIDE_INT) -04000000000)) || (offset > ((HOST_WIDE_INT) 03777777777)))
	abort ();
    }

/* This just keeps getting more and more convoluted.  {*movesi} template "move %0, %i1" means that operand 1
    is a constant that should be output in format "[constant]" regardless of the size of the value.  There aren't any
    cases that match that in here, but if we lie and say the value is not global, I think the format comes out right.
    If this doesn't work or conflicts with other case, its time to redefine PRINT_OPERATOR and start siphoning any
    problematic cases out at the top by casing on the format code and operand code.
    -mtc 11/15/2006
*/

  else if (code != 'i')
    {
      /* we were given nothing but an offset, which is considered a global value.
	 if it's wider than 18 bits.  One might expect the type to be inherently direct
	 but one counter example is function pointers type cast from literals. */
     {
	/* TODO: the following test for global seems peculiar and probably wrong to
	  me.  Why not use the tests above?  And don't we need to test for 30 bit values
	  just as above */
	/* yup, this test is not working right, so try replacing it with the one above which look right to me
	  -mtc 9/8/2006
	  We are expected to handle both signed and unsigned offsets for immediate mode
	  expressions, so we need to be more permissive.  If this creates problems, the fix will probably
	  have to be to not flow so many different cases through this one formatting routine
	*/
	/*temp = (offset < 0) ? (offset ^ 0400000) - 0400000 : offset;
	global = 0 != (temp & (HOST_WIDE_INT) 0777777000000);*/
	/*  Add some checks for outputting constant addresses that exceed the 30-bit address space.
	  This can happen when dereferencing a constant pointer initialized to a bogus constant value.
	  -mtc 10/1/2012
	*/
      if ((offset < ((HOST_WIDE_INT) -0400000)) || (offset > ((HOST_WIDE_INT) 0777777)))
      	{
	global = 1;
	if (class == PRINT_OP_OPERAND && type == PRINT_OP_POINTER &&  offset != (offset & (HOST_WIDE_INT) 07777777777))
		{
		offset = offset & (HOST_WIDE_INT) 07777777777;
		warning (0, "constant address exceeds addressable memory");
		}
      	}
      }
    }

  /* If we can't use global addressing, force local addressing */
  /* TODO: If we decided we needed to use global addressing, I would think it would
	be an error to force it back to false.  Should we abort in that case? */
  if ((!TARGET_EXTENDED) || TARGET_SMALL || TARGET_MEDIUM)
      global = 0;

  /* print prefix string */
  fputs (prefix[global][class][type], stream);

  /* print an address expression consisting of "name" or "name+" */
  if (x)
    {
      if (LABEL_REF == GET_CODE(x) || SYMBOL_REF == GET_CODE(x))
	{
	  output_addr_const (stream, x);
	  if (offset)
	    fputc ('+', stream);
	}
      else
	abort ();
      x = NULL_RTX;  /* we have disposed of the address expression */
    }

  /* print the offset, or "0" if an offset is all we were given */
  if (offset || (NULL_RTX == addr))
      pdp10_print_integer (stream, offset);
  
  fputs (suffix[global][class][type], stream);
  
  /* record use of global formats */
  if (global)
    pdp10_giw++;
}

/* Called by pdp10_print_operand after handling details specific to all %<x> format directives */

void
pdp10_print_operand_expression (FILE *stream, rtx x, int code, enum PRINT_OP_CLASS class, enum PRINT_OP_TYPE type)
{
  int ps = 0;
  HOST_WIDE_INT offset = 0;
  HOST_WIDE_INT low, high;
  int unspec_bp_ps = 0;
  int intoffset = 0;
  
/* modify handling of offset and intoffset so that when we traverse a tree downward, intoffset represents
    the cumulative offset in words and offset represents the cumulative additional offset in bytes
    not yet incorporated into ps.
    -mtc 6/16/2008
*/

/* it would be nice to check that we have a legitimate operand, but that isn't the
    same as a legitimate address 
    if (!pdp10_legitimate_address_p (GET_MODE (x), x, 1))
      {
        debug_rtx (x);
        abort ();
      }
*/

  while (1) {
    switch (GET_CODE (x))
      {

	/* deal with address expressions */

      case CONST:
	x = XEXP (x, 0);
	if (PLUS != GET_CODE (x))
	  abort ();
	/* fall through */
	
      case PLUS:
	/* (plus (x) (const_int)) is the only address expression we can deal with
	   if we see something else, trap it */
	if (CONST_INT != GET_CODE (XEXP (x, 1)))
	  abort ();

	if (!PTR_MODE_P(GET_MODE(x)))
		abort();

	if (GET_MODE(x) == Pmode)
		{
		/* Usually, the expression here is an explicitly constructed byte pointer,
		    but its possible for it to simply be a literal with an offset.
		    So we really need to add a print_literal(stream, rtx, offset) routine to output
		    [GAW(%symbol)+offset]
		    for SYMBOL_REF and LABEL_REF cases.
		    But when it's representable as a byte pointer, the assembly is generally more readable as
		    [OWGBP(ps, GIW(0,0,%symbol + offset)]
		    So we probably want some logic here to make sure the decoded ps value won't eventually
		    attempt a TWGBP and to continue handling those cases as we presently do.
		    I think that the REG case should only get here if it really is a byte pointer operand
		    [OWGBP(ps, LIW(0, reg, offset)]
		    with TWGBP either impossible or only if set before reaching here
		    -mtc 1/17/2008
		*/
		gcc_assert (offset == 0);
		decode_plus_operand(x, &ps, &offset);
		intoffset = intoffset + offset;
		offset = 0;
		x = XEXP (x, 0);
		if (GET_CODE(x) != SYMBOL_REF &&
			GET_CODE(x) != REG &&
			GET_CODE(x) != LABEL_REF)
			abort();
		break;
		}

	/*	now we should have a new pointer mode plus node
		the RHS should be a bonafide explicit pointer conversion
		the LHS should be a Q9P expression of some sort
		but perhaps is a SYMBOL_REF, REG, or LABEL_REF
		more checks would be good
		-mtc 7/28/2006
	*/
	/*offset =INTVAL( XEXP(x, 1));
	ps = (offset >> 24) & 07700;
	offset &= ADDRESS_MASK;
	offset ^= (HOST_WIDE_INT)1 << 29;
	offset -= (HOST_WIDE_INT)1 << 29;
	if (offset < 0)
		ps += 0100;
	x = XEXP(x, 0);*/

	/*	PTR mode expressions now mean what they say
		LHS should be a pointer expression of the given pointer type and the
		RHS should be an offset in units of the type pointed to by the pointer
		
		-mtc 7/2/2007
	*/
	offset = offset + INTVAL(XEXP(x, 1));
	ps = adjust_ps(ps_base_for_ptr_mode(GET_MODE(x)) << 6, (int *)&offset);
	intoffset = intoffset + offset;
	offset = 0;
	x = XEXP(x, 0);
	break;

      case SYMBOL_REF:
      case REG:
      case LABEL_REF:
	/* if we've an unspec_bp_ps, we had better not have a ps expression as well */
	if (ps && unspec_bp_ps)
	    abort ();
	if (unspec_bp_ps)
	    ps = unspec_bp_ps;

	/* if we have a pointer make sure ps gets set
	    note that SImode will end up returning a ps of 0, so we don't need to explicitly exclude it
	    -mtc 7/2/2007
	    a REG with a pointer mode should mean that it's a pre-existing byte pointer, so don't
	    fabricate a ps
	    -mtc 7/26/2007
	*/
	if (PTR_MODE_P(GET_MODE(x)) && !ps && GET_CODE(x) != REG)
	    ps = ps_base_for_ptr_mode(GET_MODE(x)) << 6;

	/* offset should have been rolled into ps
	    -mtc 6/16/2008
	*/
	gcc_assert (offset == 0);

	/* We have a byte instruction */
	if ('p' == code)
	  {
	    if (ps) /* with an explicitly constructed byte pointer */
		     /* the pointer may reference either memory or a register */	
	      print_byte_pointer (stream, ps, x, intoffset, code, PRINT_OP_LITERAL, type+1);
	    else    /* with a pre-existing byte pointer held in a word */
	      print_word_address (stream, x, intoffset, code, class, type);
	  }
	/* we have an explicitly constructed byte pointer to be used with a word instruction,
	   and are not forcing the use of a word addresses.  The byte pointer is simply data */
	else if (ps && ('W' != code))
	  print_byte_pointer (stream, ps, x, intoffset, code, class, PRINT_OP_INDIRECT);
	
	/* we have a word address with a word instruction */
	else
	    print_word_address (stream, x, intoffset, code, class, type);
	return;
	
      case MINUS:
	/* difference two symbolic addresses, resulting in an immediate value */
	if (GET_CODE (XEXP (x, 0)) != SYMBOL_REF
	    || GET_CODE (XEXP (x, 1)) != SYMBOL_REF
	    || PRINT_OP_OPERAND != class
	    || PRINT_OP_DIRECT != type)
	  abort ();
	fputs ("@[", stream);
	pdp10_print_operand_expression (stream, XEXP (x, 0), code, PRINT_OP_IMMEDIATE, type);
	fputc ('-', stream);
	pdp10_print_operand_expression (stream, XEXP (x, 1), code, PRINT_OP_IMMEDIATE, type);
	fputc (']', stream);
	return;
	
      case PRE_INC:
	/* PRE_INC is used with the ILDB and IDPB instructions.  */
	if (!PTR_MODE_P (GET_MODE (x)))
	  abort ();
	x = XEXP (x, 0);
	break;

	/*	This will need tweaking.  Inserted to handle labels-2.c test case
		Handle subreg expressions that cast to a pointer mode.  Do we need to do conversion to
		a pointer, or just force interpretation of the underlying data as a pointer?
		-mtc 7/20/2006
	*/
      case SUBREG:
	if (	(GET_MODE_SIZE(GET_MODE(x)) == GET_MODE_SIZE(GET_MODE(XEXP(x,0)))) &&
		(SUBREG_BYTE(x) == 0) &&
		PTR_MODE_P(GET_MODE(x))	)
		{
		if (!ps)
			ps = ps_base_for_ptr_mode(GET_MODE(x)) << 6;
		x = XEXP(x,0);
		break;
		}
	else
		abort();

	/*	expand_expr() can insert ZERO_EXTEND and SIGN_EXTEND to represent type conversions
		when modifier is EXPAND_INITIALIZER.
		Handle the special case here that conversion to a pointer has been done.
		-mtc 9/28/2006
	*/
      case ZERO_EXTEND:
      case SIGN_EXTEND:
	if (	(GET_MODE_SIZE(GET_MODE(x)) == GET_MODE_SIZE(GET_MODE(XEXP(x,0)))) &&
		PTR_MODE_P(GET_MODE(x))	)
		{
		if (!ps)
			ps = ps_base_for_ptr_mode(GET_MODE(x)) << 6;
		x = XEXP(x,0);
		break;
		}
	else
		abort();

      case CODE_LABEL:
	/* a label */
	output_asm_label (x);
	return;
	
      case CONST_INT:
	/* a standalone single/doubleword integer constant, with the width determined by the code
	   (the mode of const_int is always VOIDmode, which does not help us very much)
	   
	   We handle double ints directly (code, A, B, D), print_word_address can handle single ints */

	offset = INTVAL (x);
	
	/* we handle double integers here, print_word_address can deal with the rest by itself
	   class may be either IMMEDIATE or LITERAL, normally the type will be DIRECT
	   but it can be something like a function pointer as well*/
	if ('A' != code && 'B' != code && 'D' != code)
	  {
	    print_word_address(stream, NULL_RTX, offset, code, class, type);
	    return;
	  }

	/* NOTE: Constants already accomodate the extra sign bit if we're in long-long-71bit mode,
	   so we don't need to consider that here */
	high = (offset >> 36) & WORD_MASK;
	low = offset & WORD_MASK;

	if (PRINT_OP_INDIRECT == code || PRINT_OP_DIRECT != type)
	  abort ();
	
	if (PRINT_OP_LITERAL == class)
	  fputc ('[', stream);

	fputs ("EXP <", stream);
	switch (code)
	  {
	  case 'D':
	    pdp10_print_unsigned (stream, high);
	    fputs (">,<", stream);
	    pdp10_print_unsigned (stream, low);
	    break;
		
	  case 'A':
	    pdp10_print_unsigned (stream, high);
	    break;

	  case 'B':
	    pdp10_print_unsigned (stream, low);
	    break;
	  }
	
	fputc ('>', stream);
	if (PRINT_OP_LITERAL == class)
	  fputc (']', stream);
	return;
	
      case CONST_DOUBLE:
	/* print doubles as direct literals */
	if (PRINT_OP_LITERAL == class)
	  fputc ('[', stream);
	if (GET_MODE (x) == VOIDmode)
	  {
	    unsigned HOST_WIDE_INT high = CONST_DOUBLE_HIGH (x);
	    unsigned HOST_WIDE_INT low = CONST_DOUBLE_LOW (x);
	    
	    if (code != 'A' && code != 'B')
	      fputs ("EXP ", stream);
	    if (code != 'B')
	      pdp10_print_unsigned (stream, high & WORD_MASK);
	    if (code != 'A' && code != 'B')
	      fputc (',', stream);
	    if (code != 'A')
	      pdp10_print_unsigned (stream, low & WORD_MASK);
	  }
	else if (GET_MODE (x) == SFmode)
	  {
	    REAL_VALUE_TYPE r;
	    
	    long y[3];
	    HOST_WIDE_INT z;
	    
	    REAL_VALUE_FROM_CONST_DOUBLE (r, x);
	    REAL_VALUE_TO_TARGET_DOUBLE (r, y);
	    
	    z = (HWINT (y[0]) << 4)
	      + HWINT ((y[1] >> 28) & 0xfUL);
	    pdp10_print_unsigned (stream, z);
	  }
	else if (GET_MODE (x) == DFmode)
	  {
	    REAL_VALUE_TYPE r;
	    HOST_WIDE_INT z;
	    long y[3];
	    
	    REAL_VALUE_FROM_CONST_DOUBLE (r, x);
	    REAL_VALUE_TO_TARGET_DOUBLE (r, y);
	    
	    if (code != 'A' && code != 'B')
	      fputs ("EXP ", stream);
	    
	    if (code != 'B')
	      {
		z = (HWINT (y[0]) << 4)
		  + HWINT ((y[1] >> 28) & 0xfUL);
		pdp10_print_unsigned (stream, z);
	      }
	    
	    if (code != 'A' && code != 'B')
	      fputc (',', stream);
	    
	    if (code != 'A')
	      {
/* Fix the mask value
-mtc 9/28/2011
*/
		z = ((HWINT (y[1]) << 8) & 0xfffffff00ULL)
		  + HWINT (((y[2] >> 24) & 0xffUL));
		pdp10_print_unsigned (stream, z);
	      }
	  }
	else
	  abort ();
	if (PRINT_OP_LITERAL == class)
	  fputc (']', stream);
	return;

	/* deal with raw addresses */

      case UNSPEC:
	switch (XINT (x, 1))
	  {
	  case UNSPEC_ADDRESS:
	    x = XVECEXP (x, 0, 0);
	    break;

	  case UNSPEC_BP:
	    {
	      int pos = INTVAL (XVECEXP (x, 0, 1));
	      int len = INTVAL (XVECEXP (x, 0, 2));
	      unspec_bp_ps = encode_ps(pos, len);
	      x = XVECEXP (x, 0, 0);
	      class = PRINT_OP_LITERAL;
	      /* there's some confusion here about level of indirection and what type*/
	      /* be.  we'll probably want to revisit this, but if we do need to handle    */
	      /* figuring the type, should do it here instead of going to the middle of */
	      /* the mem code */
	      break;
            }

	  case UNSPEC_ADJBP:
	      x = XVECEXP (x, 0, 0);
	      break;

	  default:
	    abort ();
	  }
	break;

      case MEM:
	/* detect indirect references (mem (mem))
	   we can only express a depth of two in some cases */

	/* check for MEM:QnI(..:SI) which the front end should really avoid, but
	    which we will interpret as meaning to reference the sub-word field at the
	    beginning of the specified address
	    -mtc 11/2/2006
	    when creating a ps, include the memoffset value
	    -mtc 1/10/2008
	*/
	if ((GET_MODE_BITSIZE(GET_MODE(x)) < BITS_PER_WORD) && 
		(GET_MODE(XEXP(x,0)) == SImode) &&
		(ps == 0) && (unspec_bp_ps == 0))
		{
		int memoffset = (MEM_OFFSET(x) ? INTVAL(MEM_OFFSET(x)) : 0);
		ps = (pdp10_ps_base_for_bytesize(GET_MODE_PRECISION(GET_MODE(x))) + memoffset) << 6;
		}

	x = XEXP (x, 0);
 
	if (PRINT_OP_INDIRECT > type)
	  type += 1;
	else 
	  abort();
	break;


      default:
	abort ();
      }
  }
}
  
  
/**********************************************************************

	Target Attributes

**********************************************************************/

/* Table of machine attributes.  */
const struct attribute_spec pdp10_attribute_table[] =
{
  /* { name, min_len, max_len, decl_req, type_req, fn_type_req, handler } */

  /* Specifies the size, in bits, of a type or variable.  */
  { "size",	1, 1, true, false, false, pdp10_handle_size_attribute },

  /* Specifies the size, in bits, of a type or variable.  */
  { "fastcall",	0, 0, false, true, true, pdp10_handle_fastcall_attribute },

  { NULL,           0, 0, false, false, false, NULL }
};

tree char6_signed_type_node;
tree char6_unsigned_type_node;
tree char7_signed_type_node;
tree char7_unsigned_type_node;
tree char8_signed_type_node;
tree char8_unsigned_type_node;
tree char9_signed_type_node;
tree char9_unsigned_type_node;
tree short16_integer_type_node;
tree short16_unsigned_type_node;
tree int32_integer_type_node;
tree int32_unsigned_type_node;

/* Build all PDP-10 byte types.  */
void
pdp10_build_machine_types (void)
{
  static int done = 0;

  if (done)
    return;

/* Does it makes sense to set char_type_node, signed_char_type_node and unsigned_char_type_node to
    be the same as some of the type nodes just set?  They are already set based on CHAR_TYPE_SIZE which
    is set based on pdp10_char_bytesize.  char_type_node is equivalent but different from one of the other
    nodes.  For now leave em all separate.
    -mtc 9/18/2007
    Switch to using gcc's standard make_signed_type() instead of make_byte_type().
    Also switch to mapping the pdp10 types to whichever of char_type_node, unsigned_char_type_node or signed_char_type_node is
    the best equivalent, if any.  This makes types defined with __attribute__ ((size(n))) equivalent to an existing char mode.
    I think doing the equivalence this way is better than changing the gcc types because other things have already been created
    that reference them by the time we get here
    -mtc 10/3/2007
*/

  if (CHAR_TYPE_SIZE == 6)
  	{
  	if (TYPE_UNSIGNED(char_type_node))
  		{
  		char6_signed_type_node = signed_char_type_node;
		char6_unsigned_type_node = char_type_node;
  		}
	else
		{
  		char6_signed_type_node = char_type_node;
		char6_unsigned_type_node = unsigned_char_type_node;
		}
  	}
  else
  	{
  	char6_signed_type_node = make_signed_type (6);
  	char6_unsigned_type_node = make_unsigned_type (6);
  	}


  if (CHAR_TYPE_SIZE == 7)
  	{
  	if (TYPE_UNSIGNED(char_type_node))
  		{
  		char7_signed_type_node = signed_char_type_node;
		char7_unsigned_type_node = char_type_node;
  		}
	else
		{
  		char7_signed_type_node = char_type_node;
		char7_unsigned_type_node = unsigned_char_type_node;
		}
  	}
  else
  	{
  	char7_signed_type_node = make_signed_type (7);
  	char7_unsigned_type_node = make_unsigned_type (7);
  	}


  if (CHAR_TYPE_SIZE == 8)
  	{
  	if (TYPE_UNSIGNED(char_type_node))
  		{
  		char8_signed_type_node = signed_char_type_node;
		char8_unsigned_type_node = char_type_node;
  		}
	else
		{
  		char8_signed_type_node = char_type_node;
		char8_unsigned_type_node = unsigned_char_type_node;
		}
  	}
  else
  	{
  	char8_signed_type_node = make_signed_type (8);
  	char8_unsigned_type_node = make_unsigned_type (8);
  	}

  if (CHAR_TYPE_SIZE == 9)
  	{
  	if (TYPE_UNSIGNED(char_type_node))
  		{
  		char9_signed_type_node = signed_char_type_node;
		char9_unsigned_type_node = char_type_node;
  		}
	else
		{
  		char9_signed_type_node = char_type_node;
		char9_unsigned_type_node = unsigned_char_type_node;
		}
  	}
  else
  	{
  	char9_signed_type_node = make_signed_type (9);
  	char9_unsigned_type_node = make_unsigned_type (9);
  	}


  short16_integer_type_node = make_node (INTEGER_TYPE);
  make_byte_type (short16_integer_type_node, 16, 0);

  short16_unsigned_type_node = make_node (INTEGER_TYPE);
  make_byte_type (short16_unsigned_type_node, 16, 1);

  int32_integer_type_node = make_node (INTEGER_TYPE);
  make_byte_type (int32_integer_type_node, 32, 0);

  int32_unsigned_type_node = make_node (INTEGER_TYPE);
  make_byte_type (int32_unsigned_type_node, 32, 1);

  done = 1;
}

/* Initialize the type in TYPE.  The new type will be suitable as a
   byte of PRECISION bits pointed to by a PDP-10 global byte pointer,
   and will be unsigned if UNSIGNEDP is nonzero.  */
static void
make_byte_type (tree type, int precision, int unsignedp)
{
  TYPE_PRECISION (type) = precision;

  if (unsignedp)
    {
      TYPE_MIN_VALUE (type) = size_int (0);
      TYPE_MAX_VALUE (type)
	= build_int_cst_wide (integer_type_node, precision - HOST_BITS_PER_WIDE_INT >= 0
		       ? -1 : (HWINT (1) << precision) - 1,
		       precision - HOST_BITS_PER_WIDE_INT > 0
		       ? ((unsigned HOST_WIDE_INT) ~0
			  >> (HOST_BITS_PER_WIDE_INT
			      - (precision - HOST_BITS_PER_WIDE_INT)))
		       : 0);
    }
  else
    {
      TYPE_MIN_VALUE (type)
	= build_int_cst_wide (integer_type_node, (precision - HOST_BITS_PER_WIDE_INT > 0
			? 0 : HWINT (-1) << (precision - 1)),
		       ((HWINT (-1)
			 << (precision - HOST_BITS_PER_WIDE_INT - 1 > 0
			     ? precision - HOST_BITS_PER_WIDE_INT - 1
			     : 0))));
      TYPE_MAX_VALUE (type)
	= build_int_cst_wide (integer_type_node, (precision - HOST_BITS_PER_WIDE_INT > 0
			? -1 : (HWINT (1) << (precision - 1)) - 1),
		       (precision - HOST_BITS_PER_WIDE_INT - 1 > 0
			? ((HWINT (1)
			    << (precision - HOST_BITS_PER_WIDE_INT - 1))) - 1
			: 0));
    }

/* constant nodes are cached by type, so it's unsafe to change their type
    build_int_cst_wide will create constant nodes of type integer_type_node
    unless we pass in type as the first parameter, but if we do that, we'll need
    to make sure its sufficiently initialized first.  For example, TYPE_UNSIGNED
    definitely needs to be set first.
    For now, it seems reasonable to have the min and max values of byte types
    be integer nodes
    -mtc 4/10/2007
    in 430 update build_int_cst_wide was changed to require that a type be passed in
    -mtc 12/4/2007

  TREE_TYPE (TYPE_MIN_VALUE (type)) = type;
  TREE_TYPE (TYPE_MAX_VALUE (type)) = type;
*/

  TYPE_UNSIGNED (type) = unsignedp;

  TYPE_MODE (type) = smallest_mode_for_size (TYPE_PRECISION (type),
					     MODE_INT);
  TYPE_SIZE (type) = bitsize_int (TYPE_PRECISION (type));
  TYPE_SIZE_UNIT (type)
    = size_int ((TYPE_PRECISION (type) + BITS_PER_UNIT - 1) / BITS_PER_UNIT);

  TYPE_ALIGN (type) = precision == 32 ? 36 : precision;
}

/* Lay out a PDP-10 byte variable in memory.  */
static void
layout_byte_decl (tree decl)
{
  register tree type = TREE_TYPE (decl);
  register enum tree_code code = TREE_CODE (decl);

  DECL_UNSIGNED (decl) = TYPE_UNSIGNED (type);
  DECL_MODE (decl) = TYPE_MODE (type);
  DECL_SIZE (decl) = TYPE_SIZE (type);
  DECL_SIZE_UNIT (decl) = TYPE_SIZE_UNIT (type);

  /* Force alignment required for the data type.
     But if the decl itself wants greater alignment, don't override that.
     Likewise, if the decl is packed, don't override it.  */
  if (!(code == FIELD_DECL && DECL_BIT_FIELD (decl))
      && (DECL_ALIGN (decl) == 0
	  || (!(code == FIELD_DECL && DECL_PACKED (decl))
	      && TYPE_ALIGN (type) > DECL_ALIGN (decl))))
    {	      
      DECL_ALIGN (decl) = TYPE_ALIGN (type);
      DECL_USER_ALIGN (decl) = TYPE_USER_ALIGN (type);
    }

  /* For fields, set the bit field type and update the alignment.  */
  if (code == FIELD_DECL)
    {
      DECL_BIT_FIELD_TYPE (decl) = DECL_BIT_FIELD (decl) ? type : 0;
      if (maximum_field_alignment != 0)
	DECL_ALIGN (decl) = MIN (DECL_ALIGN (decl), maximum_field_alignment);
      else if (DECL_PACKED (decl))
	DECL_USER_ALIGN (decl) = 0;
    }

  /* Evaluate nonconstant size only once, either now or as soon as safe.  */
  if (DECL_SIZE (decl) != 0 && TREE_CODE (DECL_SIZE (decl)) != INTEGER_CST)
    DECL_SIZE (decl) = variable_size (DECL_SIZE (decl));
  if (DECL_SIZE_UNIT (decl) != 0
      && TREE_CODE (DECL_SIZE_UNIT (decl)) != INTEGER_CST)
    DECL_SIZE_UNIT (decl) = variable_size (DECL_SIZE_UNIT (decl));

  /* If requested, warn about definitions of large data objects.  */
  if (warn_larger_than
      && (code == VAR_DECL || code == PARM_DECL)
      && !DECL_EXTERNAL (decl))
    {
      tree size = DECL_SIZE_UNIT (decl);

      if (size != 0 && TREE_CODE (size) == INTEGER_CST
	  && compare_tree_int (size, larger_than_size) > 0)
	{
	  unsigned int size_as_int = TREE_INT_CST_LOW (size);

	  if (compare_tree_int (size, size_as_int) == 0)
	    warning (0, "size of %q+D is %d bytes", decl, size_as_int);
	  else
	    warning (0, "size of %q+D is larger than %u bytes",
			       decl, (unsigned int) larger_than_size);
	}
    }
}

#define PDP10_TYPE_FROM_SIZE(SIZE, UNSIGNEDP)				    \
  ((SIZE) == 6 ?							    \
   ((UNSIGNEDP) ? char6_unsigned_type_node : char6_signed_type_node) :	    \
   (SIZE) == 7 ?							    \
   ((UNSIGNEDP) ? char7_unsigned_type_node : char7_signed_type_node) :	    \
   (SIZE) == 8 ?							    \
   ((UNSIGNEDP) ? char8_unsigned_type_node : char8_signed_type_node) :	    \
   (SIZE) == 9 ?							    \
   ((UNSIGNEDP) ? char9_unsigned_type_node : char9_signed_type_node) :	    \
   (SIZE) == 16 ?							    \
   ((UNSIGNEDP) ? short16_unsigned_type_node : short16_integer_type_node) : \
   (SIZE) == 18 ?							    \
   ((UNSIGNEDP) ? short_unsigned_type_node : short_integer_type_node) :	    \
   (SIZE) == 32 ?							    \
   ((UNSIGNEDP) ? int32_unsigned_type_node : int32_integer_type_node) :	    \
   (SIZE) == 36 ?							    \
   ((UNSIGNEDP) ? unsigned_type_node : integer_type_node) :		    \
   NULL_TREE)

/* Function to handle the size attribute.  NODE points to the node to
   which the attribute is to be applied.  If a DECL, it should be
   modified in place; if a TYPE, a copy should be created.  NAME is
   the name of the attribute (possibly with leading or trailing __).
   ARGS is the TREE_LIST of the arguments (which may be NULL).  FLAGS
   gives further information about the context of the attribute.
   Afterwards, the attributes will be added to the DECL_ATTRIBUTES or
   TYPE_ATTRIBUTES, as appropriate, unless *NO_ADD_ATTRS is set to
   true (which should be done on error, as well as in any other cases
   when the attributes should not be added to the DECL or TYPE).
   Depending on FLAGS, any attributes to be applied to another type or
   DECL later may be returned; otherwise the return value should be
   NULL_TREE.  This pointer may be NULL if no special handling is
   required beyond the checks implied by the rest of this structure.  */
static tree
pdp10_handle_size_attribute (tree *node, tree name ATTRIBUTE_UNUSED, tree args, int flags ATTRIBUTE_UNUSED, bool *no_add_attrs)
{
  enum tree_code code = TREE_CODE (*node);

  pdp10_build_machine_types ();

  *no_add_attrs = 1;

  if (code == INTEGER_TYPE
      || code == VAR_DECL
      || code == PARM_DECL
      || code == TYPE_DECL
      || (code == FIELD_DECL && !DECL_BIT_FIELD (*node)))
    {
      int size = 0;

      if (args && TREE_VALUE (args)
	  && TREE_CODE (TREE_VALUE (args)) == INTEGER_CST)
	size = TREE_INT_CST_LOW (TREE_VALUE (args));

      if (!VALID_OWGBP_BYTE_SIZE_P (size)
	  && size != 16 && size != 32 && size != 36)
	{
	  error ("invalid value for `size' attribute");
	  return NULL_TREE;
	}

      if ((code != INTEGER_TYPE
	   && TREE_CODE (TREE_TYPE (*node)) != INTEGER_TYPE)
	  || TREE_CHAIN (args) != NULL_TREE
	  || size == 0)
	return NULL_TREE;

      if (code == INTEGER_TYPE)
	make_byte_type (*node, size, TYPE_UNSIGNED (*node));
      else
	{
	  int unsignedp
	    = (code ==FIELD_DECL ? DECL_UNSIGNED(*node) : TYPE_UNSIGNED( TREE_TYPE (*node)));
	  TREE_TYPE (*node) = PDP10_TYPE_FROM_SIZE (size, unsignedp);
	  layout_byte_decl (*node);
	}

      *no_add_attrs = 0;
    }

  return NULL_TREE;
}

static tree
pdp10_handle_fastcall_attribute 
	(tree *node, 
	tree name ATTRIBUTE_UNUSED, 
	tree args ATTRIBUTE_UNUSED, 
	int flags ATTRIBUTE_UNUSED, 
	bool *no_add_attrs)
{
  if (TREE_CODE (*node) != FUNCTION_TYPE
      && TREE_CODE (*node) != METHOD_TYPE
      && TREE_CODE (*node) != FIELD_DECL
      && TREE_CODE (*node) != TYPE_DECL)
    {
      warning (0, "`%s' attribute only applies to functions",
	       IDENTIFIER_POINTER (name));
      *no_add_attrs = true;
      return NULL_TREE;
    }

  return NULL_TREE;
}


/**********************************************************************

	Miscellaneout Parameters

**********************************************************************/

static void
no_long_long_callback ( cpp_reader *pfile ATTRIBUTE_UNUSED)
{
  target_flags &= ~MASK_71BIT;
  warning (0, "#pragma no_long_long doesn't work properly.  "
	   "Use -mno-long-long instead.");
}

static void
long_long_71bit_callback (cpp_reader *pfile ATTRIBUTE_UNUSED)
{
  target_flags |= MASK_71BIT;
  warning (0, "#pragma long_long_71bit doesn't work properly.  "
	   "Use -mlong-long-71bit instead.");
}

/*
static int
get_pragma_number (cpp_reader *pfile, const char *pragma) 
{
  const cpp_token *token = cpp_get_token (pfile);
  char *text = (char *)token->val.str.text;
  char *end;
  int n;

  if (token->type != CPP_NUMBER)
    {
      warning (0, "#pragma %s takes a numeric argument", pragma);
      return -1;
    }

  end = text + token->val.str.len;
  n = strtol (text, &end, 10);

  if (end == text)
    return -1;
  else
    return n;
}
*/

/* Called by the REGISTER_TARGET_PRAGMAS macro.  */
void
pdp10_register_target_pragmas (void)
{
   c_register_pragma (NULL, "long_long_71bit",  long_long_71bit_callback);
   c_register_pragma (NULL, "no_long_long", no_long_long_callback);
}

/* Called by the TARGET_MACHINE_DEPENDENT_REORG macro.

   This is ran as the last optimization pass.  (If there was a delayed
   branch scheduling pass in the PDP-10 compiler, it would run after
   this pass.)  */
void
pdp10_machine_dependent_reorg (void)
{
  rtx insn;

  if (!optimize)
    return;

  for (insn = get_insns(); insn; insn = NEXT_INSN (insn))
    {
      /* We only care about INSNs, JUMP_INSNs, and CALL_INSNs.
	 Also ignore any special USE insns.  */
      if (! INSN_P(insn)
	  || GET_CODE (PATTERN (insn)) == USE
	  || GET_CODE (PATTERN (insn)) == CLOBBER
	  || GET_CODE (PATTERN (insn)) == ADDR_VEC
	  || GET_CODE (PATTERN (insn)) == ADDR_DIFF_VEC)
	continue;

      if (get_attr_reorg_type (insn) == REORG_TYPE_LDB
	  || get_attr_reorg_type (insn) == REORG_TYPE_DPB)
	{
	/* With the 430 update LOG_LINKS is gone, so rewrite this, borrowing loosely from 
	    sh_reorg() which seems to be the only other architecture that used LOG_LINKS
	    -mtc 12/3/2007
	*/
	rtx scan, reg = NULL, pattern, insn_set_src, insn_set_dest, pre_inc;
	unsigned regno;
	pattern = PATTERN(insn);

	if (GET_CODE(pattern) != SET)
		continue;
	insn_set_src = SET_SRC (PATTERN (insn));
	insn_set_dest = SET_DEST (PATTERN (insn));
	if (GET_CODE (insn_set_src) == ZERO_EXTEND
		|| GET_CODE (insn_set_src) == SIGN_EXTEND)
		insn_set_src = XEXP (insn_set_src, 0);

	if (get_attr_reorg_type (insn) == REORG_TYPE_DPB)
		{
		if (GET_CODE (insn_set_dest) == MEM
			&& GET_CODE (XEXP (insn_set_dest, 0)) == REG)
			reg = XEXP(insn_set_dest, 0);
		}
	
	else if (get_attr_reorg_type (insn) == REORG_TYPE_LDB)
		{
		if (GET_CODE (insn_set_src) == MEM
			&& GET_CODE (XEXP (insn_set_src, 0)) == REG)
			reg = XEXP(insn_set_src, 0);
		}

	if (!reg)
		continue;
	regno = REGNO(reg);
	
	/* scan backward to find where the register is set.  */
	for (scan = PREV_INSN (insn);
	       scan && GET_CODE (scan) != CODE_LABEL;
	       scan = PREV_INSN (scan))
	      {
	      if (! INSN_P (scan))
	        continue;

	      if (! reg_mentioned_p (reg, scan))
	        continue;

	      if (get_attr_reorg_type (scan) != REORG_TYPE_IBP)
		break;

	      if (REGNO (XEXP (SET_DEST (PATTERN (scan)), 0)) != regno)
		break;

	      pre_inc = gen_rtx_PRE_INC (Pmode, reg);

	      if (get_attr_reorg_type (insn) == REORG_TYPE_DPB)
		  XEXP (insn_set_dest, 0) = pre_inc;
	      else if (get_attr_reorg_type (insn) == REORG_TYPE_LDB)
		  XEXP (insn_set_src, 0) = pre_inc;
	      delete_insn(scan);
	      break;
	      }
	}
    }
}


/**********************************************************************

	Special Predicates

**********************************************************************/

/* Return the known alignment, in storage units, of pointer X.  */
unsigned int
pdp10_pointer_alignment (rtx x)
{
  /* This function must be prepared to handle anything
     GO_IF_LEGITIMATE_ADDRESS says is a valid address.  */

  switch (GET_CODE (x))
    {
    case CONST_INT:
      return 1;
    case CONST:
      if (!TARGET_EXTENDED)
	abort ();
      switch ((int)(INTVAL (XEXP (XEXP (x, 0), 1)) >> 30) & 077)
	{
	case 0:
	case 075:
	case 070:
	  return 4;
	case 076:
	case 072:
	  return 2;
	case 071:
	case 073:
	  return 1;
	default:
	  abort ();
	}
    case LABEL_REF:
    case SYMBOL_REF:
      /* Symbols are always word-aligned.  */
      return UNITS_PER_WORD;
    case PLUS:
      if (!CONSTANT_ADDRESS_P (XEXP (x, 1)))
	{
	  debug_rtx (x);
	  abort ();
	}
      x = XEXP (x, 0);
      /* Fall through.  */
    case REG:
    reg:
      if (REG_POINTER (x))
	return REGNO_POINTER_ALIGN (REGNO (x)) / BITS_PER_UNIT;
      else
	return 1;
    case PRE_INC:
      x = XEXP (x, 0);
      if (REG_P (x))
	goto reg;
      return 1;
    case MEM:
      return 1;
    case SUBREG:
	x = SUBREG_REG (x);
	if (GET_CODE (x) == REG)
		goto reg;
        debug_rtx (x);
        abort ();

    case UNSPEC:
      switch (XINT (x, 1))
	{
	case UNSPEC_ADJBP:
		return 1;
	case UNSPEC_ADDRESS:
		return 4;
	case UNSPEC_BP:
		return 1;
	default:
		debug_rtx (x);
		abort ();
	}
      
    default:
      debug_rtx (x);
      abort ();
    }
}



/**********************************************************************

	Built-in Functions

**********************************************************************/

/* Called by the TARGET_INIT_LIBFUNCS macro

    Mark optab entries implemented with library calls
*/
void
pdp10_init_libfuncs (void)
{
  /* change these function calls, it appears "*" doesn't do what */
  /* we want anymore */
  /* -mtc 10/4/2010 */
 /*      set_optab_libfunc (umod_optab, DImode, "*umoddi3");
      set_optab_libfunc (udiv_optab, DImode, "*udivdi3");
 */
      set_optab_libfunc (umod_optab, DImode, "__umoddi3");
      set_optab_libfunc (udiv_optab, DImode, "__udivdi3");

      /* gcc keeps changing their model for how optables are set up and initialized
          as of gcc430, it appears that the general way to add umul in init_libfuncs()
          is something like:

	umul_optab->libcall_basename = "umul";
	umul_optab->libcall_suffix = '3';
	umul_optab->libcall_gen = gen_int_libfunc;

	 except that we only really want to set it for SImode and to do that it appears
	 we need to add a variant of gen_int_libfunc().

	 One other open question is handling of names.  As best I can tell the above
	 will create a name that begins with double _.  I'm not sure what the effect of
	 beginning names with '*' as above for umoddi3 and udivdi3.

	 -mtc 11/20/2007
      */
      set_optab_libfunc (umul_optab, SImode, "__umulsi3");

}

/* Called by the MD_INIT_BUILTINS macro.

   Performs the necessary setup for machine-specific builtin
   functions.  */
void
pdp10_init_builtins (void)
{
#define PTRTYPE(a)	built_pointer_type (a)
#define NIL		void_list_node
#define CONS(a,b)	tree_cons (NULL_TREE, a, b)
#define FUNTYPE(a,b)	build_function_type (a, b)
#define FUNTYPE0(a)	FUNTYPE (a, NIL)
#define FUNTYPE1(a,b)	FUNTYPE (a, CONS (b, NIL))
#define FUNTYPE2(a,b,c)	FUNTYPE (a, CONS (b, CONS (c, NIL)))
#define VOID		void_type_node
#define VOIDP		ptr_type_node
#define INT		integer_type_node
#define CHAR		char_type_node
#define LONGLONG	long_long_integer_type_node
#define FLOAT		float_type_node
#define DOUBLE		double_type_node

  add_builtin_function ("__builtin_fsc",
		    FUNTYPE2 (FLOAT, FLOAT, INT),
		    PDP10_BUILTIN_FSC,
		    BUILT_IN_MD,
		    0,0);

  add_builtin_function ("__builtin_dfsc",
		    FUNTYPE2 (DOUBLE, DOUBLE, INT),
		    PDP10_BUILTIN_DFSC,
		    BUILT_IN_MD,
		    0,0);

#ifdef HAVE_FFO
  add_builtin_function ("__builtin_ffo",
		    FUNTYPE1 (INT, INT),
		    PDP10_BUILTIN_FFO,
		    BUILT_IN_MD,
		    0,0);
#endif

#undef PTRTYPE
#undef NIL
#undef CONS
#undef FUNTYPE0
#undef FUNTYPE1
#undef FUNTYPE2
#undef VOID
#undef VOIDP
#undef INT
#undef CHAR
#undef LONGLONG
#undef FLOAT
#undef DOUBLE
}

#ifdef HAVE_FFO
/* Expand a call to __builtin_ffo.  */
static rtx
expand_builtin_ffo (tree arglist, rtx target)
{
  tree arg0;
  rtx op0, pat;

  arg0 = TREE_VALUE (arglist);
  op0 = expand_expr (arg0, NULL_RTX, VOIDmode, 0);
  op0 = force_reg (SImode, op0);

  if (!target)
    target = gen_reg_rtx (SImode);

  pat = gen_FFO (target, op0);
  if (!pat)
    return 0;
  emit_insn (pat);

  return target;
}
#endif

/* Expand a call to __builtin_(d)fsc.  */
static rtx
expand_builtin_fsc (tree arglist, rtx target, enum pdp10_builtins code)
{
  tree arg0, arg1;
  rtx op0, op1, pat;

  arg0 = TREE_VALUE (arglist);
  arg1 = TREE_VALUE (TREE_CHAIN (arglist));
  op0 = expand_expr (arg0, NULL_RTX, VOIDmode, 0);
  op1 = expand_expr (arg1, NULL_RTX, VOIDmode, 0);
  op0 = force_reg (DFmode, op0);
  op1 = force_not_mem (op1);

  if (!target)
    target = gen_reg_rtx (code == PDP10_BUILTIN_FSC ? SFmode : DFmode);

  if (code == PDP10_BUILTIN_FSC)
    pat = gen_FSC (target, op0, op1);
  else if (TARGET_GFLOAT)
    pat = gen_GFSC (target, op0, op1);
  else
    pat = gen_DFSC (target, op0, op1);
  if (!pat)
    return 0;
  emit_insn (pat);

  return target;
}

/* Called by the MD_EXPAND_BUILTIN macro.

   Expand a call to a machine specific builtin that was set up by
   MD_INIT_BUILTINS.  EXP is the expression for the function call; the
   result should go to TARGET if that is convenient, and have mode
   MODE if that is convenient.  SUBTARGET may be used as the target
   for computing one of EXP's operands. IGNORE is nonzero if the value
   is to be ignored.  */
rtx
pdp10_expand_builtin (tree exp, 
							rtx target, 
							rtx subtarget ATTRIBUTE_UNUSED, 
							enum machine_mode mode ATTRIBUTE_UNUSED, 
							int ignore ATTRIBUTE_UNUSED)
{
  tree arglist = TREE_OPERAND (exp, 1);
  enum pdp10_builtins code =
    DECL_FUNCTION_CODE (TREE_OPERAND (TREE_OPERAND (exp, 0), 0));

  switch (code)
    {
#ifdef HAVE_FFO
    case PDP10_BUILTIN_FFO:
      return expand_builtin_ffo (arglist, target);
#endif
    case PDP10_BUILTIN_FSC:
    case PDP10_BUILTIN_DFSC:
      return expand_builtin_fsc (arglist, target, code);
    default:
      abort ();
    }
}

tree
pdp10_build_va_list_type (void)
{
  return build_pointer_type (integer_type_node);
}

void
pdp10_expand_builtin_va_start (tree valist, rtx nextarg)
{
  tree t;

  t = build2 (MODIFY_EXPR, TREE_TYPE (valist), valist,
	     make_tree (integer_type_node, nextarg));
  TREE_SIDE_EFFECTS (t) = 1;

  expand_expr (t, const0_rtx, VOIDmode, EXPAND_NORMAL);
}

rtx
pdp10_expand_builtin_va_arg (tree valist, tree type)
{
  tree addr_tree, t;
  HOST_WIDE_INT align;
  HOST_WIDE_INT rounded_size;
  rtx addr;

  /* Compute the rounded size of the type.  */
  align = PARM_BOUNDARY / BITS_PER_UNIT;
  rounded_size = (((int_size_in_bytes (type) + align - 1) / align) * align);

  /* Get AP.  */
  addr_tree = valist;

  /* Compute new value for AP.  */
  t = build2 (MODIFY_EXPR, TREE_TYPE (valist), valist,
	     build2 (PLUS_EXPR, TREE_TYPE (valist), valist,
		    size_int (-rounded_size / 4)));
  TREE_SIDE_EFFECTS (t) = 1;
  expand_expr (t, const0_rtx, VOIDmode, EXPAND_NORMAL);

  if (1) /* FIXME: was PAD_VARARGS_DOWN) */
    {
      /* Small args are padded downward.  */

      HOST_WIDE_INT adj
	= rounded_size > align ? rounded_size : int_size_in_bytes (type);

      addr_tree = build2 (PLUS_EXPR, TREE_TYPE (addr_tree), addr_tree,
			 size_int (rounded_size - adj));
    }

  addr = expand_expr (addr_tree, NULL_RTX, Pmode, EXPAND_NORMAL);
  addr = copy_to_reg (addr);

  return addr;
}


/* Worker function for TARGET_STRUCT_VALUE_RTX.  */

static rtx
pdp10_struct_value_rtx (tree fntype ATTRIBUTE_UNUSED,
		       int incoming ATTRIBUTE_UNUSED)
{
  return gen_rtx_REG (Pmode, PDP10_STRUCT_VALUE_REGNUM);
}

/**********************************************************************

	Support for MACRO

        TODO: [KCG] if/when we get around to supporting GNU as (GAS)
              extract all MACRO support to a new file (macro.c?)
              to go along with the already existing file macro.h

**********************************************************************/

static const char *section_name = NULL;
static unsigned int section_flags = 0;

struct extern_symbol_entry
{
  struct hash_entry he;
  const char *name;
  int flags;
};

static struct hash_table extern_symbol_table;

/* Called by the TARGET_ASM_FILE_START macro.

   Outputs to the stdio stream asm_out_file some appropriate text to go at
   the start of an assembler file.  */
void
macro_file_start (void)
{
  FILE *file = asm_out_file;
  pdp10_build_machine_types ();

  if (main_input_filename)
    {
      const char *title = main_input_filename + strlen (main_input_filename);
      const char *prefix = pdp10_module_title_prefix;
      while (title != main_input_filename && !IS_DIR_SEPARATOR (title[-1]))
	title--;
      fputs ("\tTITLE\t", file);

      if (prefix)
	{
	  while (*prefix)
	    {
	      char c = *prefix++;
	      if (ISDIGIT (c) || ISALPHA (c))
		fputc (c, file);
	    }
	  fputs ("%", file);
	}
	  
      while (*title && memcmp (title, ".c", 3) != 0)
	{
	  char c = *title++;
	  if (ISDIGIT (c) || ISALPHA (c))
	    fputc (c, file);
	}
      fputs ("\n", file);
    }

  fputs ("\n\t.DIRECTIVE\tKL10\n\n", file);

  if (pdp10_macro_file_start_text)
	fprintf (file, "%s\n", pdp10_macro_file_start_text);
  	
  if (pdp10_macro_file_start_include)
  	{
	FILE * macrofile = fopen(pdp10_macro_file_start_include, "r");
	if (macrofile)
		{
		int c;
		while ((c=getc(macrofile)) != EOF)
			fputc((char) c, file);

		fputs ("\n", file);
		fclose(macrofile);
		}
	else
		fprintf (file, "%s\n", pdp10_macro_file_start_include);
  	}

  fprintf (file, ".IFN %s,SYMBOL,<%s=:0>\t%s Scratch, USED by compiler\n",   reg_names[0], reg_names[0], ASM_COMMENT_START);
  fprintf (file, ".IFN %s,SYMBOL,<%s=:1>\t%s Argument register 1\n",         reg_names[1], reg_names[1], ASM_COMMENT_START);
  fprintf (file, ".IFN %s,SYMBOL,<%s=:2>\t%s Argument register 2\n",         reg_names[2], reg_names[2], ASM_COMMENT_START);
  fprintf (file, ".IFN %s,SYMBOL,<%s=:3>\t%s Argument register 3\n",         reg_names[3], reg_names[3], ASM_COMMENT_START);
  fprintf (file, ".IFN %s,SYMBOL,<%s=:4>\t%s Argument register 4\n",         reg_names[4], reg_names[4], ASM_COMMENT_START);
  fprintf (file, ".IFN %s,SYMBOL,<%s=:5>\t%s Function scratch register 1\n", reg_names[5], reg_names[5], ASM_COMMENT_START);
  fprintf (file, ".IFN %s,SYMBOL,<%s=:6>\t%s Function scratch register 2\n", reg_names[6], reg_names[6], ASM_COMMENT_START);
  fprintf (file, ".IFN %s,SYMBOL,<%s=:7>\t%s Function scratch register 3\n", reg_names[7], reg_names[7], ASM_COMMENT_START);
  fprintf (file, ".IFN %s,SYMBOL,<%s=:10>\t%s Preserved register 1\n",         reg_names[8], reg_names[8], ASM_COMMENT_START);
  fprintf (file, ".IFN %s,SYMBOL,<%s=:11>\t%s Preserved register 2\n",         reg_names[9], reg_names[9], ASM_COMMENT_START);
  fprintf (file, ".IFN %s,SYMBOL,<%s=:12>\t%s Preserved register 3\n",         reg_names[10], reg_names[10], ASM_COMMENT_START);
  fprintf (file, ".IFN %s,SYMBOL,<%s=:13>\t%s Preserved register 4\n",         reg_names[11], reg_names[11], ASM_COMMENT_START);
  fprintf (file, ".IFN %s,SYMBOL,<%s=:14>\t%s Static Chain Pointer\n",         reg_names[12], reg_names[12], ASM_COMMENT_START);
  fprintf (file, ".IFN %s,SYMBOL,<%s=:15>\t%s Frame Pointer\n",                reg_names[13], reg_names[13], ASM_COMMENT_START);
  fprintf (file, ".IFN %s,SYMBOL,<%s=:16>\t%s RESERVED for PIC code\n",        reg_names[14], reg_names[14], ASM_COMMENT_START);
  fprintf (file, ".IFN %s,SYMBOL,<%s=:17>\t%s Stack Pointer\n\n",              reg_names[15], reg_names[15], ASM_COMMENT_START);
  
  fprintf (file, ".IFN LAW,MACRO,\t\t<DEFINE LAW(Y)\t\t<Y>>\t\t%s Local Address Word\n", ASM_COMMENT_START);
  fprintf (file, ".IFN LIW,MACRO,\t\t<DEFINE LIW(I,X,Y)\t<2B1!<I>B13!<X>B17!<Y&777777>>>\t\t%s Local Indirect Word\n", ASM_COMMENT_START);

  if (TARGET_EXTENDED)
    {
      fprintf (file, ".IFN GAW,MACRO,\t\t<DEFINE GAW(Y)\t\t<<Y>*ONE>>\t%s Global Address Word\n", ASM_COMMENT_START);
      fprintf (file, ".IFN GIW,MACRO,\t\t<DEFINE GIW(I,X,Y)\t<<I>B1!<X>B5!GAW(Y)>>\t%s Global Indirect Word\n", ASM_COMMENT_START);
      fprintf (file, ".IFN OWGBP,MACRO,\t<DEFINE OWGBP(PS,Y)\t<<PS>B5!GAW(Y)>>\t%s One Word Global Byte Pointer\n", ASM_COMMENT_START);
      fprintf (file, ".IFN TWGBP,MACRO,\t<DEFINE TWGBP(P,S,Y)\t<EXP <P>B5!<S>B11!1B12,<Y>>>\t%s Two Word Global Byte Pointer\n", ASM_COMMENT_START);
    }

  fprintf (file, ".IFN ALIGN,MACRO,\t<DEFINE ALIGN(N)\t<RELOC <.+<1_N-1>>&^-<1_N-1>>>\t%s Align PC to 2**N word boundary\n", ASM_COMMENT_START);

/* Use of BSS is now obsolete
    -mtc 8/26/2008
  fprintf (file, ".IFN BSS,MACRO,\t\t<DEFINE BSS(A)\t\t<\t%s%s\n\t\tSALL\n\t\tREPEAT A,<EXP 0>\n\t\tXALL>>\n", ASM_COMMENT_START, " Zero initialized memory");
*/

  if (!TARGET_DUALSTACK)
    {
      fprintf (file, ".IFN $PSECT,MACRO,<\n");
      fprintf (file, "\tDEFINE $PSECT(NAME,PARAMS)<%s Set PSECT\n", ASM_COMMENT_START);
      fprintf (file, "\t\t.PSECT NAME'PARAMS\n");
      fprintf (file, "\t>%sDEFINE $PSECT\n", ASM_COMMENT_START);
      fprintf (file, ">%s.IFN $PSECT,MACRO\n", ASM_COMMENT_START);

      fprintf (file, ".IFN CALL,MACRO,\t<DEFINE CALL(A)\t\t<pushj %s,@A>>\t\t%s Call a procedure\n", reg_names[STACK_POINTER_REGNUM], ASM_COMMENT_START);
      fprintf (file, ".IFN CALLRET,MACRO,\t<DEFINE CALLRET(A)\t<jrst @A>>\t\t%s Call and Return from a procedure\n", ASM_COMMENT_START);
      fprintf (file, ".IFN RET,MACRO,\t\t<DEFINE RET\t\t<popj %s,>>\t\t%s Return from procedure\n", reg_names[STACK_POINTER_REGNUM], ASM_COMMENT_START);
  
    } else {

      fprintf (file, ".IFN FRM.PC,SYMBOL,<FRM.PC=:\t^D-10>\t%s procedure return address\n", ASM_COMMENT_START);
      fprintf (file, ".IFN FRM.ET1,SYMBOL,<FRM.ET1=:\t^D-9>\t%s __FILE__\n", ASM_COMMENT_START);
      fprintf (file, ".IFN FRM.ET2,SYMBOL,<FRM.ET2=:\t^D-8>\t%s __LINE__\n", ASM_COMMENT_START);
      fprintf (file, ".IFN FRM.EP1,SYMBOL,<FRM.EP1=:\t^D-7>\t%s exception handler's preserved register set\n", ASM_COMMENT_START);
      fprintf (file, ".IFN FRM.EP2,SYMBOL,<FRM.EP2=:\t^D-6>\t%s .\n", ASM_COMMENT_START);
      fprintf (file, ".IFN FRM.EP3,SYMBOL,<FRM.EP3=:\t^D-5>\t%s .\n", ASM_COMMENT_START);
      fprintf (file, ".IFN FRM.EP4,SYMBOL,<FRM.EP4=:\t^D-4>\t%s .\n", ASM_COMMENT_START);
      fprintf (file, ".IFN FRM.ESC,SYMBOL,<FRM.ESC=:\t^D-3>\t%s .\n", ASM_COMMENT_START);
      fprintf (file, ".IFN FRM.EFP,SYMBOL,<FRM.EFP=:\t^D-2>\t%s .\n", ASM_COMMENT_START);
      fprintf (file, ".IFN FRM.ESP,SYMBOL,<FRM.ESP=:\t^D-1>\t%s exception handler's SP\n", ASM_COMMENT_START);
      fprintf (file, ".IFN FRM.EPC,SYMBOL,<FRM.EPC=:\t^D-0>\t%s address of exception handler\n", ASM_COMMENT_START);
      fprintf (file, ".IFN FRM.SZ,SYMBOL,<FRM.SZ=:\tFRM.EPC - FRM.PC + 1>\n");

      fprintf (file, ".IFN $PSECT,MACRO,<\n");
      fprintf (file, "\tDEFINE $PSECT(NAME,PARAMS)<%s Set PSECT, define current CALL, RET and CALLRET\n", ASM_COMMENT_START);
      fprintf (file, "\t\tDEFINE CALL(A)<\n");
      fprintf (file, "\t\t\t.IFN .CALL'NAME,SYMBOL,<EXTERN .CALL'NAME>\n");
      fprintf (file, "\t\t\tPUSH %s,A\n", reg_names[STACK_POINTER_REGNUM]);
      fprintf (file, "\t\t\tPUSHJ %s,.CALL'NAME\n", reg_names[14]);
      fprintf (file, "\t\t>%sDEFINE CALL\n", ASM_COMMENT_START);
      fprintf (file, "\t\tDEFINE RET<\n");
      fprintf (file, "\t\t\t.IFN .RET'NAME,SYMBOL,<EXTERN .RET'NAME>\n");
      fprintf (file, "\t\t\tJRST .RET'NAME\n");
      fprintf (file, "\t\t>%sDEFINE RET\n", ASM_COMMENT_START);
      fprintf (file, "\t\tDEFINE CALLRET(A)<\n");
      fprintf (file, "\t\t\t.IFN .CRET'NAME,SYMBOL,<EXTERN .CRET'NAME>\n");
      fprintf (file, "\t\t\tPUSH %s,A\n", reg_names[STACK_POINTER_REGNUM]);
      fprintf (file, "\t\t\tJRST .CRET'NAME\t\t%s recycle slot and call the procedure\n", ASM_COMMENT_START);
      fprintf (file, "\t\t>%sDEFINE CALLRET\n", ASM_COMMENT_START);
      fprintf (file, "\t\t.PSECT NAME'PARAMS\n");
      fprintf (file, "\t>%sDEFINE $PSECT\n", ASM_COMMENT_START);
      fprintf (file, ">%s.IFN $PSECT,MACRO\n", ASM_COMMENT_START);
	  
      fprintf (file, ".IFN CALL,MACRO,<DEFINE CALL(A)\t<\n");
      fprintf (file, "\t\t.IFN .call,SYMBOL,<EXTERN .call>%s declare extern if needed\n", ASM_COMMENT_START);
      fprintf (file, "\t\tpush\t%s,A\t\t\t%s Save register\n", reg_names[STACK_POINTER_REGNUM],ASM_COMMENT_START);
      fprintf (file, "\t\tpushj\t%s,@[GAW(.call)]\t%s call the procedure\n", reg_names[14], ASM_COMMENT_START);
      fprintf (file, "\t\t>>\n");
      
      fprintf (file, ".IFN CALLRET,MACRO,<DEFINE CALLRET(A)\t<\n");
      fprintf (file, "\t\t.IFN .cret,SYMBOL,<EXTERN .cret>%s declare extern if needed\n", ASM_COMMENT_START);
      fprintf (file, "\t\tpush\t%s,A\t\t\t%s Save register\n", reg_names[STACK_POINTER_REGNUM],ASM_COMMENT_START);
      fprintf (file, "\t\tjrst\t@[GAW(.cret)]\t\t%s recycle slot and call the procedure\n", ASM_COMMENT_START);
      fprintf (file, "\t\t>>\n");
      
      fprintf (file, ".IFN RET,MACRO,<DEFINE RET\t\t<.IFN .ret,SYMBOL,<EXTERN .ret>\t\t%s declare extern if needed\n", ASM_COMMENT_START);
      fprintf (file, "\t\t\t\t\t\tjrst\t@[GAW(.ret)]\t\t%s remove slot and return\n", ASM_COMMENT_START);
      fprintf (file, "\t\t\t\t\t\t>>\n");
    }

  fprintf (file, ".IFN EPILOG,MACRO,\t<DEFINE EPILOG\t\t<>>\t\t%s end of procedure hook\n", ASM_COMMENT_START);
  fprintf (file, ".IFN LINENO,MACRO,\t<DEFINE LINENO(A)\t<>>\t\t%s source line number debug hook\n", ASM_COMMENT_START);
  fputs ("\n", file);
}

static bool
maybe_output_extern (struct hash_entry *he, hash_table_key ptr ATTRIBUTE_UNUSED)
{
  struct extern_symbol_entry *entry = (struct extern_symbol_entry *) he;
  if (entry->flags == (PDP10_SYMBOL_USED | PDP10_SYMBOL_EXTERN))
    {
      fputs ("\tEXTERN\t", asm_out_file);
      assemble_name (asm_out_file, entry->name);
      fputc ('\n', asm_out_file);
    }
  return true;
}

/* Called by the TARGET_ASM_FILE_END macro.

   Outputs to the stdio stream asm_out_file some appropriate text to go at
   the end of an assembler file.  */
void
macro_file_end (void)
{
  FILE *file = asm_out_file;
  int i, j;

  macro_section_end(file);
  fputs ("\n\n", file);

  if (TARGET_EXTENDED || pdp10_giw)
    fputs ("\tEXTERN\tONE\n", file);

  for (i = 6; i <= 18; i++)
    if (used_byte_subtraction_table[i])
      fprintf (file, "\tEXTERN\tBAD%c%c\n",
	       TARGET_EXTENDED ? 'X' : 'L', i == 18 ? 'H' : '0' + i);

  hash_traverse (&extern_symbol_table, maybe_output_extern, NULL);
  fputc ('\n', file);

  if (TARGET_PURGELABELS)
  	{
	fprintf(file, "IF2,<\n");
	for (i=1; i<max_label_num(); i+=10)
		{
		fprintf(file, "\tPURGE .L%d", i);
		for(j=i+1; j<i+10 && j < max_label_num(); j++)
			fprintf(file, ",.L%d", j);
		fprintf(file, "\n");
		}
	fprintf(file, ">\n\n");
  	}

  fputs ("\n\tEND\n", file);
}

void
macro_section_end (FILE *stream)
{
  if (TARGET_EXTENDED)
    {
     /* possibly temporary fix until we straighten out the new section handling
         we need to make sure byte data output is finished
         -mtc 10/24/2007
     */
     pdp10_output_byte(stream, 0, 0);

      if (section_name)
	{
	  if (strncmp (".db", section_name, 3) == 0)
	    pdp10_align_with_pad (0400);
	  pdp10_output_byte (stream, 0, 0);
	  fprintf (stream, "\t.ENDPS\t; %s\n\n", section_name);
	  section_name = NULL;
	}
    }
}

/* Print an ASCII (or SIXBIT) string with characters of BYTE_SIZE
   bits, without using the ASCIZ pseudo.  SIXBIT is output if
   BYTE_SIZE is 6.  */

static bool macro_ascii;

static void
macro_output_ascii_bytes (FILE *stream, const unsigned char *ptr, int len, int byte_size)
{
  if (len == 0)
    return;

  macro_ascii = 8;

  while (len--)
    {
      int c = *ptr++;
      if (byte_size == 6)
	{
	  c = TOUPPER (c) - 040;
	  if (c < 0 || c > 077)
	    c = 0;
	}
      pdp10_output_byte (stream, GEN_INT (c), byte_size);
      pdp10_byte_count++;
    }

  macro_ascii = 0;
}

/* Return separator character if the string can be output using the
   ASCIZ pseudo, or 0 otherwise.  */
static int
asciz_ok (const char *ptr, int len)
{
  char *separators = xstrdup ("/$|%!");
  int c, i;
  char *p;

  while (len--)
    {
      c = *ptr++;
      if (c && (p = strchr (separators, c)))
	*p = 'x';
      if (c >= 32 && c <= 126)
	continue;

      if (c == '\r' && len > 0 && *ptr == '\n')
	{
	  ptr++;
	  len--;
	  continue;
	}

      if (c == 0 && len == 0)
	continue;

      free (separators);
      return 0;
    }

  c = 0;
  for (i = 0; i < (int) strlen (separators); i++)
    if (separators[i] != 'x')
      {
	c = separators[i];
	break;
      }

  free (separators);
  return c;
}

/* Called by the ASM_OUTPUT_ASCII macro.

   Outputs to the stdio stream STREAM an assembler instruction to
   assemble a string constant.  */
void
macro_output_ascii (FILE *stream, const char *ptr, int len)
{
  int byte_size = pdp10_char_bytesize;
  int separator;
  int c;

  /* If the ASCIZ pseudo can't be used, output the string byte by byte.  */
  if (byte_size != 7 || (separator = asciz_ok (ptr, len)) == 0)
    {
      /* Note that 6-bit characters are converted to SIXBIT.  */
      macro_output_ascii_bytes (stream, (unsigned char *)ptr, len, byte_size);
      return;
    }

  fputs ("\tASCIZ\t", stream);
  fputc (separator, stream);

  while (len--)
    {
      c = *ptr++;
      if (c == '\r')
	{
	  len--;
	  ptr++;
	  fputc ('\n', stream);
	}
      else if (c == 0 && len == 0)
	;
      else
	fputc (c, stream);
    }

  fputc (separator, stream);
  fputc ('\n', stream);
}

/* Static variables for pdp10_output_byte.  */
static int bits_in_word;
static rtx last_output_byte = NULL_RTX;
static int last_byte_size;

int
pdp10_in_output_byte (void)
{
  return (last_output_byte != NULL_RTX && bits_in_word != 0 && bits_in_word != BITS_PER_WORD);
}

/* Called primarily by the ASM_OUTPUT_CHAR and ASM_OUTPUT_SHORT
   macros, but also from many other places.

   If EXP is NULL, then output a newline; the next byte will be put in
   a new word.  Otherwise, output the value of EXP as a byte of
   BYTE_SIZE bits.  If the byte doesn't fit in the same word as the
   previous byte, output a newline and put the byte in a new word.  */
void
pdp10_output_byte (FILE *stream, rtx exp, int byte_size)
{
  int mask;

  if (last_output_byte == NULL_RTX)
    {
      if (exp == NULL_RTX || byte_size == 0)
	return;
      fprintf (stream, "\tBYTE (%d)", byte_size);
      mask = (1 << byte_size) - 1;
      /*pdp10_print_integer (stream, INTVAL (exp) & mask);*/
      pdp10_print_number (stream, INTVAL (exp) & mask, 1, byte_size, 0);
      bits_in_word = byte_size;
    }
  else if (exp)
    {
      if (bits_in_word + byte_size > 36)
	{
	  pdp10_output_byte (stream, 0, 0);
	  pdp10_output_byte (stream, exp, byte_size);
	  return;
	}
      if (byte_size == last_byte_size)
	fputc (',', stream);
      else
	fprintf (stream, "(%d)", byte_size);
      mask = (1 << byte_size) - 1;
      /*pdp10_print_integer (stream, INTVAL (exp) & mask);*/
      pdp10_print_number (stream, INTVAL (exp) & mask, 1, byte_size, 0);
      bits_in_word += byte_size;
    }
  else
    {
      bits_in_word = 0;
      fputc ('\n', stream);
    }

  last_byte_size = byte_size;
  last_output_byte = exp;
}

/* Reworked to check and use last_byte_size when outputting pad chars
    Also, change to void routine
    -mtc 5/4/2009
*/
void
pdp10_align_with_pad (int pad)
{
  rtx x = GEN_INT (pad);

  if (bits_in_word > 0)
    {
    while ((bits_in_word + last_byte_size) <= 36)
	pdp10_output_byte (asm_out_file, x, last_byte_size);
    pdp10_output_byte(asm_out_file, 0, 0);
    }
}


/* Called by the ASM_OUTPUT_SKIP macro
     Implemented as a procedure instead of a complex macro because rewritting required
     accessing static variable bits_in_word
    Don't use pdp10_align_with_pad()
    Use pdp10_char_bytesize instead of 9
    Pad to end of word by using pdp10_output_byte(...,0,0)
     -mtc 5/5/2009
*/
void
pdp10_output_skip (FILE *stream, int nbytes)
{
  /* Output as many bytes as will fit in the current word */
  if (bits_in_word)
    {
    if (((36 - bits_in_word) / pdp10_char_bytesize) <= nbytes)
  	{
	nbytes -= (36 - bits_in_word) / pdp10_char_bytesize;
  	pdp10_output_byte (stream, 0, 0);
  	}
    else  while (nbytes)
  	{
  	nbytes--;
  	pdp10_output_byte (stream, const0_rtx, pdp10_char_bytesize);
  	}
    }

  /* Output any whole words */
  if (nbytes >= (36 / pdp10_char_bytesize))
  	{
	fprintf (stream, "\tBLOCK\t%o\n", nbytes / (36 / pdp10_char_bytesize));
	nbytes %= (36 / pdp10_char_bytesize);
  	}

  /* Finish by outputting any trailing bytes */
  if (nbytes)
  	{
  	pdp10_output_byte(stream, const0_rtx, nbytes*pdp10_char_bytesize);
  	}
}


/* Called by the TARGET_ASM_GLOBALIZE_LABEL macro.

   Outputs to the stdio stream STREAM some commands that will make the
   label NAME global; that is, available for reference from other
   files.  */
void
macro_globalize_label (FILE *stream, const char *name)
{
  pdp10_output_byte (stream, 0, 0);

  /* Need to distinguish between variables and entry points.
     Check to see if we're in a code section or not...

     This is NOT entirely correct and will probably need to be revisited */
 /* if (section_name && (TARGET_BUILDLIB) && (0 == strcmp(section_name, pdp10_text_psect)))*/
  if (section_name && (TARGET_BUILDLIB))
      fputs ("\n\n\tENTRY\t", stream);
  else
      fputs ("\n\n\tINTERN\t", stream);

  assemble_name (stream, name);
  fputc ('\n', stream);
}

void
macro_output_def (FILE *stream, const char *label, const char *value)
{
  assemble_name (stream, label);
  fputs ("=", stream);
  assemble_name (stream, value);
  fprintf (stream, "\n");

  pdp10_extern_symbol (label, PDP10_SYMBOL_DEFINED);
}

/* Switch to an arbitrary section NAME with attributes as specified by
   FLAGS.  */
/* Change to $PSECT instead of .PSECT and other formatting changes to allow use of a macro to implement
    some call and ret changes
    -mtc 9/12/2008
*/
void
pdp10_asm_named_section	(const char *name, unsigned int flags, tree decl ATTRIBUTE_UNUSED)
{
  /* if changing to the same section, do nothing */
  if (section_name && 0 == strcmp(section_name, name) && section_flags == flags)
      return;
    else
      macro_section_end(asm_out_file);

  section_name = name;
  section_flags = flags;

  fputs ("\n\t$PSECT\t", asm_out_file);
  fputs (name, asm_out_file);
  fputs (",<", asm_out_file);
  if (flags & SECTION_WRITE)
    fputs ("/rwrite", asm_out_file);
  else
    fputs ("/ronly", asm_out_file);
  
/*  fputs ("/concatenate", asm_out_file);*/

  if (flags & SECTION_PALIGNED)
  	fputs("/paligned", asm_out_file);

  fputs (">", asm_out_file);

  if (flags & (SECTION_CODE |  SECTION_DEBUG | SECTION_BSS
	       | SECTION_SMALL | SECTION_LINKONCE | SECTION_FORGET))
    {
      fputs ("\t;", asm_out_file);
      if (flags & SECTION_CODE)
	fputs (" code", asm_out_file);
      if (flags & SECTION_DEBUG)
	fputs (" debug", asm_out_file);
      if (flags & SECTION_LINKONCE)
	fputs (" linkonce", asm_out_file);
      if (flags & SECTION_SMALL)
	fputs (" small", asm_out_file);
      if (flags & SECTION_BSS)
	fputs (" bss", asm_out_file);
      if (flags & SECTION_FORGET)
	fputs (" forget", asm_out_file);
    }

  fputs ("\n", asm_out_file);
}

/* Create a new entry for the extern_symbol_table hash table.  */
static struct hash_entry *
extern_symbol_newfunc (struct hash_entry *he, struct hash_table *table, hash_table_key string ATTRIBUTE_UNUSED)
{
  struct extern_symbol_entry *entry = (struct extern_symbol_entry *) he;

  if (entry == NULL)
    {
      entry = ((struct extern_symbol_entry *)
	       hash_allocate (table, sizeof (struct extern_symbol_entry)));
      if (entry == NULL)
	return NULL;
    }

  entry->name = NULL;
  entry->flags = 0;
  return (struct hash_entry *) entry;
}

static void
init_extern_symbol_table (void)
{
  static int inited = 0;

  if (!inited)
    {
      hash_table_init (&extern_symbol_table,
		       extern_symbol_newfunc,
		       string_hash,
		       string_compare);
      inited = 1;
    }
}

bool
pdp10_lookup_extern_symbol (const char *name, int flags)
{
  struct extern_symbol_entry *entry;

  init_extern_symbol_table ();

  entry = ((struct extern_symbol_entry *)
	   hash_lookup (&extern_symbol_table, (const hash_table_key) name, 
			false, NULL));
  if (entry && (0 != (entry->flags & flags)))
    return true;
  return false;
}

void
pdp10_extern_symbol (const char *name, int flags)
{
  struct extern_symbol_entry *entry;

  init_extern_symbol_table ();

  entry = ((struct extern_symbol_entry *)
	   hash_lookup (&extern_symbol_table, (const hash_table_key) name, 
			true, string_copy));
  if (!entry->name)
    entry->name = xstrdup (name);
  entry->flags |= flags;
}

int pdp10_byte_count = 0;
int pdp10_bits_per_unit = BITS_PER_UNIT;

static bool
pdp10_assemble_integer (rtx x, unsigned int size, int aligned_p)
{
  if (!aligned_p && size > 1)
    return false;

  /* constant expressions can be much more complex than we can handle, but simplification helps
      -mtc 5/4/2007
  */
  if (BINARY_P(x))
  	{
  	rtx y = simplify_binary_operation(GET_CODE(x), GET_MODE(x), XEXP(x, 0), XEXP(x, 1));
	if (y)
		x=y;
  	}

  switch (size)
    {
    case 1:
    case 2:
      pdp10_output_byte (asm_out_file, x, pdp10_bits_per_unit * size);
      break;
    case 4:
    case 8:
      if (aligned_p)
	{
	  pdp10_output_byte (asm_out_file, 0, 0);
	  fputc ('\t', asm_out_file);
	  
	  /*  Symbols need to be output decorated so that they will be treated as 30-bit addresses
	  	It's possible that LABEL_REFs will need the same treatment.
	  	-mtc 10/18/2006
	  */
	  pdp10_print_operand_expression (asm_out_file, x, (size == 8 ? 'D' : 0), PRINT_OP_IMMEDIATE, 
	  	((GET_CODE(x) == SYMBOL_REF)  ? PRINT_OP_POINTER : PRINT_OP_DIRECT));
	  
	  fputc ('\n', asm_out_file);
	  break;
	}
    default:
      return false;
    }

  pdp10_byte_count += size;

  return true;
}


/**********************************************************************

	Optimizations

**********************************************************************/

/* Helper function for pdp10_output_range_compare.  Examines whether a
   single instruction is suitable to replace the JRST in the range
   comparision.  */
static int
single_insn (rtx insn, rtx label1, rtx label2)
{
  rtx jump;

  if (get_attr_length (insn) != 1
      || get_attr_skip (insn) == SKIP_YES)
    return 0;

  jump = next_nonnote_insn (insn);
  return (jump == NULL_RTX
	  || (GET_CODE (jump) == BARRIER && !simplejump_p (insn))
	  || (GET_CODE (jump) == CODE_LABEL && jump == label2)
	  || (simplejump_p (jump)
	      && label1 == XEXP (SET_SRC (PATTERN (jump)), 0)));
}

/* Output instructions to conditionally jump to operands[3] depending
   on the result of a comparison of the register in operands[0]
   against a range represented by operands[1] and operands[2].  Return
   an empty string.

   The RTL insn being output looks like this:
   
	(set (pc)
	     (if_then_else
	      (ge (xor:SI (plus:SI
			   (match_operand:SI 1 "register_operand" "r")
			   (match_operand:SI 2 "const_int_operand" "i"))
			  (const_int SIGNBIT))
		  (match_operand:SI 3 "const_int_operand" "i"))
	      (label_ref (match_operand 4 "" ""))
	      (pc)))

   And is actually a combination of three insns:

	;; Subtract the low end of the range from operands[0].
	(set (reg:SI X)
	     (plus:SI (match_operand:SI 1 "register_operand" "r")
		      (match_operand:SI 2 "const_int_operand" "i")))
	
	;; Complement the sign bit in preparation for an
	;; unsigned comparison.
	(set (reg:SI Y)
	     (xor:SI (reg:SI X) (const_int SIGNBIT)))
	
	;; Compare the result against the high end of the range,
	;; and jump if it's larger or equal.
	(set (pc)
	     (if_then_else
	      (ge (reg:SI Y)
		  (match_operand:SI 3 "const_int_operand" "i"))
	      (label_ref (match_operand 4 "" ""))
	      (pc)))

   If the comparison if reversed (jump if operands[1] is inside the
   range), the comparison is "less than" instead, and TRNA is nonzero.

   The output looks something like this:

	CAIL op1,-op2			; skip if op1 less than low end
	 CAIL op1,op3^SIGNBIT - op2	; skip if op1 less than high end
	  JRST op4			; jump

   CAML may be used instead of CAIL if necessary.  A TNRA is output
   last if the comparison should be inverted.  If the JRST jump target
   is just one instruction, the JRST may be replaced by that
   instruction.  */
const char *
pdp10_output_range_compare (rtx insn, rtx *ops, int trna)
{
  rtx jump_target, label;
  HOST_WIDE_INT op2;
  rtx operands[5];

  memcpy (operands, ops, sizeof operands);

  /* Compare against the low end of the range.  */
  op2 = INTVAL (operands[2]);
  if (GET_CODE (operands[1]) == MEM)
    {
      if (op2 != 0)
	abort ();
      output_asm_insn ("skipl %0,%1", operands);
    }
  else
    {
      operands[2] = GEN_INT (trunc_int_for_mode (-op2, SImode));
      if (INTVAL (operands[2]) & ~HWINT (0777777))
	output_asm_insn ("caml %0,%i2", operands);
      else
	output_asm_insn ("cail %0,%a2", operands);
    }

  /* Compare against the high end of the range.  */
  operands[3] = GEN_INT (trunc_int_for_mode ((INTVAL (operands[3]) ^ SIGN_BIT) - op2, SImode));
  if (INTVAL (operands[3]) & ~HWINT (0777777))
    output_asm_insn ("%_caml %0,%i3", operands);
  else
    output_asm_insn ("%_cail %0,%a3", operands);

  if (trna)
    {
      /* Output a TRNA instrcution unless the next instruction is a
	 single instruction, in which case the comparison is reversed
	 (by not emitting the TRNA) and no jump instruction is output.  */

      rtx next_insn = next_real_insn (insn);

      if (!single_insn (next_insn, NULL_RTX, operands[4]))
	output_asm_insn ("%_%_trna", operands);
      else
	{
	  if (LABEL_NUSES (operands[4]) == 1)
	    INSN_DELETED_P (operands[4]) = 1;
	  pdp10_opcode_indentation = 2;
	  return "";
	}
    }

  label = next_nonnote_insn (insn);
  jump_target = next_nonnote_insn (operands[4]);

  /* Ouput the JRST instruction unless the jump target is a single
     instruction (and perhaps a jump back).  If so, replace the JRST
     by that instruction.  */
  if (LABEL_NUSES (operands[4]) == 1
      && single_insn (jump_target, label, NULL_RTX))
    {
      rtx jump_back = next_nonnote_insn (jump_target);

      if (LABEL_NUSES (operands[4]) == 1)
	INSN_DELETED_P (operands[4]) = 1;
      if (GET_CODE (jump_back) != BARRIER)
	{
	  INSN_DELETED_P (jump_back) = 1;
	  if (LABEL_NUSES (label) == 1)
	    INSN_DELETED_P (label) = 1;
	}
      pdp10_opcode_indentation = 2 + trna;
      final_scan_insn (jump_target, asm_out_file, optimize, 0, 0);
      INSN_DELETED_P (jump_target) = 1;
    }
  else
    /* TODO: pdp10_output_jrst (operands[4]); */
    output_asm_insn ("%_%_%_jrst %4" + (trna ? 0 : 2), operands);

  return "";
}


/**********************************************************************

	Data Movement

**********************************************************************/

/* Return the offset from the base pointer.  */
int
pdp10_pointer_offset (rtx x)
{
  /* This function must be prepared to handle anything
     GO_IF_LEGITIMATE_ADDRESS says is a valid address.  */
  /* I think this routine needs to closely parallel pdp10_pointer_alignment
	I added handling ofSUBREG, which is just a variant of REG to be like REG
	on the assumption this is dealing with the offset of the data referenced by
	the expression, not where the address itself is stored.
	I also added UNSPEC and have it return 0 based on the assumption that any
	offset there is built into the pointer itself */
  /* I don't understand the PLUS case code.  I think in this case, the constant
	value is actually a PS mask that can be used to convert a word pointer to a
	byte pointer, which is not really an offset at all.  Need to see what is actually
	done with the results of this routine.*/

  switch (GET_CODE (x))
    {
    case SYMBOL_REF:
    case LABEL_REF:
    case SUBREG:
    case REG:
    case PRE_INC:
    case MEM:
    case UNSPEC:
      return 0;
    case CONST_INT:
      return INTVAL (x) & ~((HOST_WIDE_INT)(UNITS_PER_WORD - 1));
    case CONST:
      x = XEXP (x, 0);
      if (GET_CODE (x) != PLUS)
	abort ();
      /* Fall through.  */
    case PLUS:
      if (GET_CODE (XEXP (x, 1)) != CONST_INT)
	abort ();
      return INTVAL (XEXP (x, 1));
    default:
      abort ();
    }
}

/* Return or output a single instruction to load an unsigned byte into
   a register.  Called to output a (I)LDB(I) insn.  */
const char *
pdp10_output_load_unsigned_byte (rtx insn, rtx *operands)
{
  int pos;
  int size = 9;
  int ps = 0;
  int strict;
  char code = 'W';

  ps = extract_ps(operands[1], insn, strict = 0);

  if (0 == ps) /* a scalar byte */
    pos = 27;
  else
    {
    decode_ps(ps, &pos, &size);
    if (extract_ps(operands[1], insn, strict=1) != 0)
		code = 'p';
    }

/* It's hokey, but extract_ps tells us that we have an explicitly constructed
byte pointer by returning non-zero when strict is clear and zero when strict is set.
It's not necessarily reliable.  Eventually we will want to mark byte pointer values
with a distinct memory mode.
	-mtc 6/21/2006
*/
  return output_bitfield_move (asm_out_file, insn, 0, 0, operands[0],
			       operands[1], size, pos, code);
}

/* Return or output a single instruction to load a signed byte into a
   register.  Called to output a (I)LDBE(I) insn.  */
const char *
pdp10_output_load_signed_byte (rtx insn, rtx *operands)
{
  int pos;
  int size = 9;
  int ps = 0;
  int strict;
  char code = 'W';

  ps = extract_ps(operands[1], insn, strict = 0);

  if (0 == ps) /* a scalar byte */
    pos = 27;
  else
    {
    decode_ps(ps, &pos, &size);
    if (extract_ps(operands[1], insn, strict=1) != 0)
		code = 'p';
    }

/* It's hokey, but extract_ps tells us that we have an explicitly constructed
byte pointer by returning non-zero when strict is clear and zero when strict is set.
It's not necessarily reliable.  Eventually we will want to mark byte pointer values
with a distinct memory mode.
	-mtc 6/21/2006
*/
  return output_bitfield_move (asm_out_file, insn, 0, 1, operands[0],
			       operands[1], size, pos, code);
}

/* Return or output a single instruction to store a byte from a
   register.  Called to output an (I)DPB(I) insn.  */
const char *
pdp10_output_store_byte (rtx insn, rtx *operands)
{
  int pos;
  int size = 9;
  int ps;
  int strict;
  char code = 'W';

  ps = extract_ps(operands[0], insn, strict = 0);

  if (0 == ps) /* a scalar byte */
    pos = 27;
  else
    {
    decode_ps(ps, &pos, &size);
    if (extract_ps(operands[0], insn, strict=1) != 0)
		code = 'p';
    }

/* It's hokey, but extract_ps tells us that we have an explicitly constructed
byte pointer by returning non-zero when strict is clear and zero when strict is set.
It's not necessarily reliable.  Eventually we will want to mark byte pointer values
with a distinct memory mode.
	-mtc 6/21/2006
*/
  return output_bitfield_move (asm_out_file, insn, 1, 0, operands[1],
			       operands[0], size, pos, code);
}

/* Return or output a single half-word instruction.  REG is the
   accumulator operand and X is the memory operand.  REG_RIGHT should
   be nonzero if the right half of the accumulator is to be used.
   MEM_RIGHT should should be nonzero if X is an accumulator and the
   right half should be used.  STORE should be 1 for store
   instructions and 0 for load instrucitons.  EXTEND should be 1 for
   sign- or zero-extending loads, and 0 for non-extending loads.  SIGN
   should be 1 for sign- extending loads, and 0 for zero-extending
   loads.  */
static const char *
output_halfword_move (rtx reg, rtx x, int reg_right, int mem_right, int store, int extend, int sign)
{
  rtx operands[2];
  char format[20];
  char *p = format;

  /* reg_right of a subreg means we should use the subreg_byte to determine left/right */
  /* and use the subreg_reg */
  /* subreg with reg_right==0 will sometimes mean constant 0, or might be meaningless */
  /* for now don't handle it */
  /* mtc 3/16/2011 */
  if (GET_CODE(reg) == SUBREG && reg_right)
  	{
  	reg_right = (SUBREG_BYTE(reg) >=2);
	reg = SUBREG_REG(reg);
  	}

  if (REG != GET_CODE(reg))
    {
      debug_rtx (reg);
      debug_rtx (x);
      abort ();
    }

  *p++ = 'h';

  if (store)
    {
      *p++ = reg_right ? 'r' : 'l';
      *p++ = mem_right ? 'r' : 'l';
    }
  else
    {
      *p++ = mem_right ? 'r' : 'l';
      *p++ = reg_right ? 'r' : 'l';
    }
  if (extend)
    *p++ = sign ? 'e' : 'z';

  if (store)
    *p++ = 'm';

  *p++ = ' ';
  *p = 0;

  strcat (format, "%0,%1");

/* Since the fact we're doing a halfword operation is now captured in the instruction itself, we need to
    change the arguments to be ordinary word addresses so that the operand printing routines don't
    pull them apart again.
    -mtc 10/5/2007
*/
  operands[0] = reg;
  operands[1] = shallow_copy_rtx(x);
  PUT_MODE(operands[1], SImode);
  MEM_ATTRS(operands[1]) = 0;

  output_asm_insn (format, operands);

  return "";
}

/* Return or output a single instruction to move a half-word.  Called
   to output a movhi pattern.  SIGN_EXTEND should be 1 if this is a
   sign-extending load.  */
const char *
pdp10_output_movhi (rtx *operands, int sign_extend)
{
  int store = GET_CODE (operands[0]) == MEM;
  rtx x = store ? operands[0] : operands[1];
  rtx reg = store ? operands[1] : operands[0];
  rtx address = XEXP (x, 0);
  rtx memoffset = MEM_OFFSET(x);
  int memoffsetvalue = memoffset ? INTVAL(memoffset) : 0;
  unsigned int memsize = (MEM_SIZE(x) ? INTVAL(MEM_SIZE(x)) : 0);
  int mem_right = 0;
  
  int ps = 0;
  int strict;

  /* increment operations imply we have a byte pointer, so use byte instructions */

  if (GET_CODE (address) == PRE_INC)
    {
      if (store)
	return "idpb %1,%p0";
      else
	return sign_extend ? "extend %0,[ildbe %p1]" : "ildb %0,%p1";
    }
  else if (GET_CODE (address) == POST_INC)
    {
      if (store)
	return "extend %1,[dpbi %p0]";
      else
	return sign_extend ? "extend %0,[ldbei %p1]" : "extend %0,[ldbi %p1]";
    }

  /* evaluate the operand to determine if it is explicitly a byte pointer.
     If so, issue a byte instruction */
  /*if the size of the item is not less than a word, then its not a byte pointer so don't
       bother calling extract_ps, which is unreliable and can claim its a byte pointer
       when it isn't
       -mtc 6/12/2006, 7/6/2006
  */
  ps = (memsize < UNITS_PER_WORD ? extract_ps(x, NULL_RTX, strict = 1) : 0);

  if (ps)
    {
    if (store)
      return "dpb %1,%p0";
    else
      return sign_extend ? "extend %0,[ldbe %p1]" : "ldb %0,%p1";
    }

    /* we are dealing with a word address.
         decide which half we want by looking at the offset
         Note that only offsets of 0 or 2 are expected,
         but treat 0 or 1 as left and 2 or 3 as right
    */
  mem_right = memoffsetvalue / 2;
  return output_halfword_move (reg, x, 1, mem_right, store, !store, sign_extend);
} 

/* Expand an unsigned bit-field load.  Called to expand the "extzv"
   pattern in some special cases.  Return zero if nothing special has
   to be done, nonzero otherwise.  */
int
pdp10_expand_extzv (rtx *operands)
{
  rtx op1 = copy_rtx (operands[1]);
  int len = INTVAL (operands[2]);
  int pos = INTVAL (operands[3]);
  HOST_WIDE_INT mask;
  rtx temp;

  mask = ((HOST_WIDE_INT) 1 << len) - 1;

  PUT_MODE (op1, SImode);

  temp = gen_reg_rtx (SImode);
  emit_move_insn (temp, op1);

  if (pos == 0) /* && len == 32) */
      emit_insn (gen_lshrsi3 (temp, temp, GEN_INT (BITS_PER_WORD - len - pos))); /* MJD add - pos */
  else if (pos + len == BITS_PER_WORD)
      emit_insn (gen_andsi3 (temp, temp, GEN_INT (trunc_int_for_mode (mask, SImode))));
  else
    {
      emit_insn (gen_lshrsi3 (temp, temp, GEN_INT (BITS_PER_WORD - len - pos))); /* MJD add - pos */
      emit_insn (gen_andsi3 (temp, temp, GEN_INT (trunc_int_for_mode (mask, SImode))));
    }
  emit_move_insn (operands[0], temp);
  return 1;
}

/* Expand a signed bit-field load.  Called to expand an "extv" pattern
   if the LDBE instruction is not available.  */
int
pdp10_expand_extv (rtx *operands)
{
  int len = INTVAL (operands[2]);
  int pos = INTVAL (operands[3]);
  rtx op1 = copy_rtx (operands[1]);

  if (len == 18 && (pos == 0 || pos == 18))
    return 0;

  if (GET_CODE (op1) == MEM)
    normalize_bitfield_mem (&op1, &pos);
  PUT_MODE (op1, SImode);

  if (len == 32 && pos == 0)
    {
      emit_move_insn (operands[0], op1);
      emit_insn (gen_ashrsi3 (operands[0], operands[0], GEN_INT (4)));
      return 1;
    }

  return 0;
}

/* *X is a (9-bit) byte in memory and *POS is the first bit of a
   bitfield starting in *X.  Modify *X and *POS such that *X is word
   aligned and *POS specifies the first bit, in the word that starts
   at *X, of the bitfield.  */
static void
normalize_bitfield_mem (rtx *x, int *pos)
{
  rtx mem = *x, y;

  if (GET_CODE (mem) != MEM)
    return;

  y = XEXP (mem, 0);
  if (GET_CODE (y) == CONST)
    y = XEXP (y, 0);

  if (GET_CODE (y) == PLUS)
    {
      HOST_WIDE_INT i = INTVAL (XEXP (y, 1));
      HOST_WIDE_INT j = i % UNITS_PER_WORD;

      if (j != 0)
	{
	  if (j < 0)
	    j += UNITS_PER_WORD;

	  *x = gen_rtx_MEM (GET_MODE (mem), 
			    plus_constant (XEXP (y, 0), i - j));
	  MEM_COPY_ATTRIBUTES(*x, mem);
	  *pos += BITS_PER_UNIT * j;
	}
    }
}

/* This is an adaption of find_last_vaue in rtanal.c.  Return the last
   thing that X was assigned from before *PINSN.  If we found an
   assignment, update *PINSN to point to it.  */
static rtx
pdp10_find_last_value (rtx x, rtx *pinsn)
{
  rtx p;

  for (p = PREV_INSN (*pinsn); p && GET_CODE (p) != CODE_LABEL;
       p = PREV_INSN (p))
    if (INSN_P (p))
      {
	rtx set = single_set (p);
	rtx note = find_reg_note (p, REG_EQUAL, NULL_RTX);
	rtx dest = (set ? SET_DEST(set) : NULL_RTX);

	if (set && REG_P (dest)
	    && REGNO (dest) == REGNO (x))
	  {
	    rtx src = SET_SRC (set);

	    if (note && GET_CODE (XEXP (note, 0)) != EXPR_LIST)
	      src = XEXP (note, 0);

/* If the destination of the assignment we found is smaller than X then src isn't really the last value of X.
     (Note that src and dest should have the same mode.)
     But in practice, this is only a problem when src and dest are the same regno, because that will tend to
     be optimized as a NOP and the high order bits of X cannot be inferred from src.
     -mtc 3/30/2011
*/
	    if (GET_MODE_SIZE(GET_MODE(dest)) < GET_MODE_SIZE(GET_MODE(x)))
		if ((REG_P(src) && REGNO(src) == REGNO(dest)) ||
			(GET_CODE(src) == SUBREG && REG_P(SUBREG_REG(src)) && REGNO(SUBREG_REG(src)) == REGNO(dest)))
			return NULL_RTX;

	    *pinsn = p;
	    return src;
	  }
	  
	/* If set in non-simple way, we don't have a value.  */
	if (reg_set_p (x, p))
	  break;
      }

  return NULL_RTX;
}

#define INTVAL_OR_ELSE(X, Y)						\
  (GET_CODE (X) == CONST_INT ? INTVAL (X)				\
   : GET_CODE (X) == NEG						\
     && GET_CODE (XEXP ((X), 0)) == CONST_INT ? -INTVAL (XEXP ((X), 0))	\
   : (Y))

/* Return nonzero iff the last value X was set to before INSN had
   BITS zero-extended bits.  */
int
zero_extended_p (rtx x, int bits, rtx insn)
{
  HOST_WIDE_INT w;
  int i, n;

  do
    {
      while (REG_P (x))
	{
	  x = pdp10_find_last_value (x, &insn);
	  if (x == NULL_RTX)
	    return 0;
	}

      switch (GET_CODE (x))
	{
	case LSHIFTRT:
	  n = INTVAL_OR_ELSE (XEXP (x, 1), 0);
	  x = XEXP (x, 0);
	  break;
	case ZERO_EXTRACT:
	  n = BITS_PER_WORD - INTVAL_OR_ELSE (XEXP (x, 1), BITS_PER_WORD);
	  x = NULL_RTX;
	  break;
	case AND:
	  w = INTVAL_OR_ELSE (XEXP (x, 1), HWINT (-1));
	  for (i = BITS_PER_WORD - 1; i >= 0; i--)
	    {
	      if (w & ((HWINT (1)) << i))
		break;
	    }
	  n = BITS_PER_WORD - i;
	  x = XEXP (x, 0);
	  break;
	case SUBREG:
	  n = BITS_PER_WORD - GET_MODE_BITSIZE (GET_MODE (x));
	  break;
	case MEM:
	  /* A QI could be as small as 6 bits.  */
	  n = GET_MODE (x) == QImode ? 30
	    : BITS_PER_WORD - GET_MODE_BITSIZE (GET_MODE (x));
	  return n >= bits;
	case UNSPEC:
	  switch (XINT (x, 1))
	    {
	    case UNSPEC_ADDRESS:
	      return TARGET_EXTENDED ? 0 : 18;
	    }
	  /* Fall through.  */
	default:
	  return 0;
	}

      if (n >= bits)
	return 1;

      bits -= n;
    }
  while (x);

  return 0;
}

/* Return nonzero iff the last value X was set to before INSN had
   BITS sign-extended bits.  */
int
sign_extended_p (rtx x, int bits, rtx insn)
{
  HOST_WIDE_INT w;
  int i, n;

  do
    {
      while (REG_P (x))
	{
	  x = pdp10_find_last_value (x, &insn);
	  if (x == NULL_RTX)
	    return 0;
	}

      switch (GET_CODE (x))
	{
	case ASHIFTRT:
	  n = INTVAL_OR_ELSE (XEXP (x, 1), 0);
	  x = XEXP (x, 0);
	  break;
	case SIGN_EXTRACT:
	  n = BITS_PER_WORD - INTVAL_OR_ELSE (XEXP (x, 1), BITS_PER_WORD);
	  x = NULL_RTX;
	  break;
	case IOR:
	  w = INTVAL_OR_ELSE (XEXP (x, 1), 0);
	  for (i = BITS_PER_WORD - 1; i >= 0; i--)
	    {
	      if (!(w & ((HWINT (1)) << i)))
		break;
	    }
	  n = BITS_PER_WORD - i;
	  x = XEXP (x, 0);
	  break;
	default:
	  return 0;
	}

      if (n >= bits)
	return 1;

      bits -= n;
    }
  while (x);

  return 0;
}

/* Return or output to STREAM a single instruction, INSN, to move a
   bit-field.  If STORE is nonzero, store REG in X, otherwise load X
   into REG.  If SIGN is nonzero, loads should sign-extend, otherwise
   loads should zero-extend.  LEN and POS specifies the length and the
   bit position of the bitfield.  Note that this function will output
   XKL-2 extended instructions if necessary.  */
static const char *
output_bitfield_move (FILE *stream, rtx insn, int store, int sign, rtx reg, rtx x, int len, int pos, char code)
{
  rtx mem;
  int extend = sign || (GET_CODE (x) == MEM
			&& GET_CODE (XEXP (x, 0)) == POST_INC);

  if ( REG != GET_CODE(reg) || (extend && !TARGET_XKL2))
    {
      debug_rtx (insn);
      abort ();
    }

  /* if we have a word address, first attempt to use word instructions */
  if ('W' == code)
    {    
      /* Can this be output as a half-word move instruction?  */
      if ((len == 18) && (pos == 0 || pos == 18))
	{
	  int reg_right = 1;
	  int mem_right = pos == 18;
	  
	  /* if x is actually a register rather than memory, figure out which half we want
	     I don't quite grok this bit of code.  It seems to needlessly turn the whole operation around
	     for register to register operations */
	  
	  switch (GET_CODE (x))
	    {
	    case MEM:
	      break;
	    case REG:
	      if (store || GET_CODE (reg) != REG)
		{
		  rtx tmp = reg; reg = x; x = tmp;
		  reg_right = mem_right;
		  if (GET_CODE (x) == MEM)
		    {
		      if (MEM_OFFSET (x) != 0)
			mem_right = INTVAL (MEM_OFFSET (x));
		      else
			mem_right =
			  pdp10_pointer_offset (XEXP (x, 0)) % UNITS_PER_WORD != 0;
		    }
		  else
		    mem_right = 1;
		  store = !store && GET_CODE (x) != REG;
		}
	      break;
	    default:
	      abort ();
	    }
	  
	  return
	    output_halfword_move (reg, x, reg_right, mem_right, store, !store, sign);
	}
      
      /* Can this be output as a single shift instruction?  */
      if (pos >= 0
	  && REG_P (x)
	  && REGNO (x) == REGNO (reg)
	  && len + pos <= BITS_PER_WORD
	  && ((sign && sign_extended_p (reg, pos, insn))
	      || (!sign && zero_extended_p (reg, pos, insn))))
	{
	  fputc ('\t', stream);
	  pdp10_print_insn (stream, sign ? "ash" : "lsh");
	  pdp10_print_tab (stream);
	  print_reg (stream, 0, REGNO (reg), 'W', PRINT_OP_OPERAND, PRINT_OP_DIRECT);
	  fprintf (stream, ",-%o\n", BITS_PER_WORD - len - pos);
	  return "";
	}
      
      /* Can we use extv_sequence or extzv_sequence to load a register? */
      if (!store
	  && pos >= 0
	  && REG_P (reg)
	  && len + pos <= BITS_PER_WORD)
	{
	  rtx operands[4];
	  operands[0]  = reg;
	  operands[1] = x;
	  operands[2] = GEN_INT(len);
	  operands[3] = GEN_INT(pos);

	  if (sign)
	    output_asm_insn (pdp10_output_extv_sequence (operands), operands);
	  else 
	    output_asm_insn ( pdp10_output_extzv_sequence (operands), operands);
	  return "";
	}
    }
  
  /* Nope, we must use a byte instruction */
  
  fputc ('\t', stream);
  if (extend)
    {
      pdp10_print_insn (stream, "extend");
      pdp10_print_tab (stream);
      print_reg (stream, 0, REGNO (reg), 'W', PRINT_OP_OPERAND, PRINT_OP_DIRECT);
      fputs (",[", stream);
    }

  if (GET_CODE (x) == MEM && GET_CODE (XEXP (x, 0)) == PRE_INC)
    {
      fputc (TARGET_ASM_UPPER ? 'I' : 'i', stream);
      mem = gen_rtx_MEM (GET_MODE (x), XEXP (XEXP (x, 0), 0));
      MEM_COPY_ATTRIBUTES(mem, x);
      x = mem;
    }
  pdp10_print_insn (stream, store ? "dpb" : "ldb");
  
  if (sign)
    fputc (TARGET_ASM_UPPER ? 'E' : 'e', stream);
  if (GET_CODE (x) == MEM && GET_CODE (XEXP (x, 0)) == POST_INC)
    {
      fputc (TARGET_ASM_UPPER ? 'I' : 'i', stream);
      mem = gen_rtx_MEM (GET_MODE (x), XEXP (XEXP (x, 0), 0));
      MEM_COPY_ATTRIBUTES(mem, x);
      x = mem;
    }
  fputc (TARGET_ASM_TAB && !extend ? '\t' : ' ', stream);

  if (!extend)
    {
      print_reg (stream, 0, REGNO (reg), 'W', PRINT_OP_OPERAND, PRINT_OP_DIRECT);
      fputc (',', stream);
    }

  /* If we have a word address, generate an UNSPEC_BP to describe the desired byte pointer for print_operand.
     If we have a byte pointer already, we are set.
     
     NOTE: This unspec should probably be used everywhere in the compiler that a byte pointer is formed
     Between UNSPEC_BP and UNSPEC_ADJBP we should be able to track all byte pointers
     far more effectively than by depending on the current lame mechanism for detecting byte
     pointers as PLUS address expressions involving const_int PS fields. Yuk. */

  if ('W' == code)
    x = gen_rtx_UNSPEC (Pmode, gen_rtvec(3, x, GEN_INT(pos) , GEN_INT(len)), UNSPEC_BP);
  
  PRINT_OPERAND (stream, x, 'p');
  
  if (extend)
    fputc (']', stream);

  fputc ('\n', stream);

  return "";
}

/* Return or output a single instruction to load a register from a
   signed bit-field.  Called to output an extv pattern.  */
const char *
pdp10_output_extv (rtx insn, rtx *operands)
{
  return output_bitfield_move (asm_out_file, insn, 0, 1, operands[0],
			       operands[1], INTVAL (operands[2]),
			       INTVAL (operands[3]), 'W');
}

/* Return or output a sequence of at most three instructions to load a
   register from a signed bit-field.  Called to output an extv
   pattern.  */
const char *
pdp10_output_extv_sequence (rtx *operands)
{
  int len = INTVAL (operands[2]);
  int pos = INTVAL (operands[3]);

  operands[3] = GEN_INT (len - BITS_PER_WORD);
  if (pos == 0)
    {
      if (len == 18)
	return "hlre %0,%W1";
      else if (SAME_REG (operands[0], operands[1]))
	return "ash %0,%a3";
      else
	return "move %0,%W1\n\tash %0,%a3";
    }
  else if (pos == 18)
    {
      if (len == 18)
	return "hrre %0,%W1";
      else
	{
	  operands[2] = GEN_INT (len - 18);
	  return "hrre %0,%W1\n\tash %0,%a2";
	}
    }
  else
    {
      operands[2] = GEN_INT (pos);
      if (SAME_REG (operands[0], operands[1]))
	return "lsh %0,%a2\n\tash %0,%a3";
      else
	return "move %0,%W1\n\tlsh %0,%a2\n\tash %0,%a3";
    }
}

/* Return or output a sequence of at most three instructions to load a
   register from an unsigned bit-field.  Called to output an extzv
   pattern.  */
const char *
pdp10_output_extzv_sequence (rtx *operands)
{
  int len = INTVAL (operands[2]);
  int pos = INTVAL (operands[3]);
  operands[2] = GEN_INT (len + pos - BITS_PER_WORD); 
  if (pos == 0)
    {
      if (len == 18)
	return "hlrz %0,%W1";
      else if (SAME_REG (operands[0], operands[1]))
	return "lsh %0,%a2";
      else
	return "move %0,%W1\n\tlsh %0,%a2";
    }
  else if (pos == 18)
    {
      if (len == 18)
	return "hrrz %0,%W1";
      else
	{
	  operands[2] = GEN_INT (len - 18);
	  return "hrrz %0,%W1\n\tlsh %0,%a2";
	}
    }
  else
    {
      unsigned HOST_WIDE_INT mask;

      mask = ((HOST_WIDE_INT)1 << len) - 1;
      if (len <= 18)
	operands[3] = GEN_INT (trunc_int_for_mode (mask, SImode));
      else
	operands[3] = GEN_INT (trunc_int_for_mode ((~mask & WORD_MASK) >> 18, SImode));

      if (SAME_REG (operands[0], operands[1]))
	{
	  if (len + pos == 36)
	    return len <= 18 ? "andi %0,%a3" : "tlz %0,%3";
	  else
	    return len <= 18
	      ? "lsh %0,%a2\n\tandi %0,%a3"
	      : "lsh %0,%a2\n\ttlz %0,%3";
	}
      else if (len + pos == 36)
	return len <= 18
	  ? "move %0,%W1\n\tandi %0,%a3"
	  : "move %0,%W1\n\ttlz %0,%3";
      else if (len + pos == 18)
	return "hlrz %0,%W1\n\tandi %0,%a3";
      else
	return len <= 18
	  ? "move %0,%W1\n\tlsh %0,%a2\n\tandi %0,%a3"
	  : "move %0,%W1\n\tlsh %0,%a2\n\ttlz %0,%3";
    }
}

/* Return or output a single instruction to load a register from an
   unsigned bit-field.  Called to output an extzv pattern.  */
const char *
pdp10_output_extzv (rtx insn, rtx *operands)
{
  /* add asserts and select a value for code rather than always passing 'W'
      -mtc 1/23/2008
  */
  char code = 'W';
  if (GET_CODE(operands[1]) == MEM)
  	{
	gcc_assert (ptr_mode_for_mode(GET_MODE(operands[1])) == GET_MODE(XEXP(operands[1], 0))
				|| GET_MODE(XEXP(operands[1], 0)) == VOIDmode);
	if (GET_MODE(XEXP(operands[1], 0)) != SImode
		&& GET_MODE(XEXP(operands[1], 0)) != VOIDmode)
		code = 0;
  	}
  
  return output_bitfield_move (asm_out_file, insn, 0, 0, operands[0],
			       operands[1], INTVAL (operands[2]),
			       INTVAL (operands[3]), code);
}

#if 0
const char *
pdp10_output_extzv_foo (insn, operands)
     rtx insn;
     rtx *operands;
{
  return output_bitfield_move (asm_out_file, insn, 0, 0, operands[0],
			       operands[1], 6, 3);
}
#endif

/* Return or output a single instruction to store a register into a
   bit-field.  Called to output an insv pattern.  */
const char *
pdp10_output_insv (rtx insn, rtx *operands)
{
  /* add asserts and select a value for code rather than always passing 'W'
      -mtc 1/23/2008
  */
  char code = 'W';
  if (GET_CODE(operands[0]) == MEM)
  	{
	gcc_assert (ptr_mode_for_mode(GET_MODE(operands[0])) == GET_MODE(XEXP(operands[0], 0))
				|| GET_MODE(XEXP(operands[0], 0)) == VOIDmode);
	if (GET_MODE(XEXP(operands[0], 0)) != SImode
		&& GET_MODE(XEXP(operands[0], 0)) != VOIDmode)
		code = 0;
  	}
  
  return output_bitfield_move (asm_out_file, insn, 1, 0, operands[3],
			       operands[0], INTVAL (operands[1]),
			       INTVAL (operands[2]), code);
}

/* Emit instructions to move four words.  Return nonzero on success.  */
int
pdp10_expand_move_4 (rtx destination, rtx source)
{
  int units_per_move = MOVE_MAX;
  enum machine_mode mode = mode_for_size (BITS_PER_UNIT * units_per_move,
					  MODE_INT, 0);
  int moves = 4 * UNITS_PER_WORD / units_per_move;
  rtx destinations[4];
  rtx sources[4];
  int i;

  /* A simple optimization to avoid some problematic rtx
      -mtc 11/30/2006
  */
  if (destination == source)
  	return 1;

  sources[0] = NULL_RTX;

  if (GET_CODE (destination) == MEM)
    {
      for (i = 0; i < moves; i++)
	destinations[i] = gen_rtx_MEM (mode,
				       plus_constant (XEXP (destination, 0),
						      i * units_per_move));
    }
  else
    {
      for (i = 0; i < moves; i++)
	destinations[i] = gen_rtx_SUBREG (mode, destination,
					  i * units_per_move);
    }

  if (GET_CODE (source) == CONST_INT || GET_CODE (source) == CONST_DOUBLE)
    {
      unsigned HOST_WIDE_INT high, low;

      if (GET_CODE (source) == CONST_INT)
	{
	  high = (INTVAL (source) >> 36) & 0777777777777ULL;
	  low = INTVAL (source) & 0777777777777ULL;
	}
      else if (GET_MODE (source) == VOIDmode)
	{
	  high = ((CONST_DOUBLE_HIGH (source) << 28)
		  + ((CONST_DOUBLE_LOW (source) >> 36) & 01777777777ULL));
	  low = CONST_DOUBLE_LOW (source) & 0777777777777ULL;
	}
      else
	{
	  REAL_VALUE_TYPE r;
	  long y[3];

	  REAL_VALUE_FROM_CONST_DOUBLE (r, source);
	  REAL_VALUE_TO_TARGET_DOUBLE (r, y);

	  high = (((HOST_WIDE_INT)y[0] << 4)
		  + (HOST_WIDE_INT)((y[1] >> 28) & 0xfUL));
	  low = ((((HOST_WIDE_INT)y[1] << 8) & 0xfffffffUL)
		 + (HOST_WIDE_INT)((y[2] >> 24) & 0xffUL));
	}

      sources[0] = const0_rtx;
      sources[1] = const0_rtx;
      sources[2] = GEN_INT (trunc_int_for_mode (high, SImode));
      sources[3] = GEN_INT (trunc_int_for_mode (low, SImode));
    }
  else if (GET_CODE (source) == MEM)
    {
      for (i = 0; i < moves; i++)
	sources[i] = gen_rtx_MEM (mode,
				  plus_constant (XEXP (source, 0),
						 i * units_per_move));
    }
  else
    {
      for (i = 0; i < moves; i++)
	sources[i] = simplify_gen_subreg (mode, source, GET_MODE(source), i * units_per_move);
    }

  for (i = 0; i < moves; i++)
    emit_move_insn (destinations[i], sources[i]);

  return 1;
}

/* Return nonzero if XBLT should be used instead of BLT.  */
static int
use_xblt (rtx destination, rtx source, rtx length)
{
  if (!HAVE_XBLT || !TARGET_EXTENDED)
    return 0;

  if (GET_CODE (destination) != CONST_INT
      || (source && GET_CODE (source) != CONST_INT)
      || GET_CODE (length) != CONST_INT)
    return 1;

  if (INTVAL (length) >> 32 > 0)
    return 1;

  if (source)
    {
      HOST_WIDE_INT destination_section = INTVAL (destination) >> 32;
      HOST_WIDE_INT source_section = INTVAL (source) >> 32;

      if (destination_section != source_section)
	return 1;
    }

  return 0;
}

/* Try to emit instructions to BLT a memory block of LENGTH storage
   units from SOURCE to DESTINATION.  Return nonzero on success.  */
static int
expand_blt (rtx destination, rtx source, rtx length)
{
  rtx ac, e;

  if (GET_CODE (length) != CONST_INT
      || INTVAL (length) % UNITS_PER_WORD != 0)
    return 0;

  ac = gen_reg_rtx (Pmode);
  if (CONSTANT_ADDRESS_P (destination)
      && CONSTANT_ADDRESS_P (source))
    {
      emit_insn (gen_move_two_halves (ac, source, destination));
    }
  else
    {
    /* Calls to gen_MOVEI are problematic if the destination is not very simple, and in particular if it is a PLUS node.
        The alternatives are to wrap destination with force_reg(SImode, destination), or if that fails because this might
        be called too late, to use gen_addsi3(ac, lhs(destination), rhs(destination))
        -mtc 11/29/2006
    */
      emit_insn (gen_MOVEI (ac, destination));
      emit_insn (gen_HRLI (ac, source));
    }

  {
    HOST_WIDE_INT len = INTVAL (length) - UNITS_PER_WORD;
    if (CONSTANT_ADDRESS_P (destination))
      e = plus_constant (destination, len);
    else
      {
	e = gen_reg_rtx (Pmode);
	emit_insn (gen_addsi3 (e, destination, GEN_INT (len)));
      }
  }

  /* Beginning at the location addressed by the left half-word of AC,
     move words to another area beginning at the location addressed by
     the right half-word of AC.  Continue until a word is moved to
     location E.  */
  emit_insn (gen_BLT (ac, gen_rtx_MEM (BLKmode, e)));

  return 1;
}

/* Try to emit instructions to XBLT a memory block of LENGTH storage
   units from SOURCE to DESTINATION.  Return nonzero on success.  */
static int
expand_xblt (rtx destination, rtx source, rtx length)
{
  rtx temp, mem, acs;
  int i, n;

  if (GET_CODE (length) != CONST_INT)
    return 0;

  n = INTVAL (length);

  switch (n / UNITS_PER_WORD)
    {
    case 0:
      break;

    case 2:
    case 3:
      temp = gen_reg_rtx (DImode);
      for (i = 0; i < n / 8; i++)
	{
	  emit_move_insn (temp, gen_rtx_MEM (DImode,
					     plus_constant (source, i)));
	  emit_move_insn (gen_rtx_MEM (DImode,
				       plus_constant (destination, i)), temp);
	}
      /* Fall through.  */

    case 1:
      if (n % 8 >= 4)
	{
	  int m = 2 * (n / 8);
	  temp = gen_reg_rtx (SImode);
	  emit_move_insn (temp, gen_rtx_MEM (SImode,
					     plus_constant (source, m)));
	  emit_move_insn (gen_rtx_MEM (SImode,
				       plus_constant (destination, m)),
			  temp);
	}
      break;

    default:
      acs = gen_reg_rtx (TImode);
      emit_move_insn (gen_rtx_SUBREG (SImode, acs, 0),
		      GEN_INT (INTVAL (length) / UNITS_PER_WORD));
      emit_move_insn (gen_rtx_SUBREG (Pmode, acs, 4), source);
      emit_move_insn (gen_rtx_SUBREG (Pmode, acs, 8), destination);
      emit_insn (gen_XBLT (acs));
      break;
    }

  switch (n % UNITS_PER_WORD)
    {
    case 0:
      break;

    case 1:
      temp = gen_reg_rtx (QImode);
      mem = gen_rtx_MEM (QImode, plus_constant (source, n / 4));
      set_mem_align (mem, BITS_PER_WORD);
      emit_move_insn (temp, mem);
      mem = gen_rtx_MEM (QImode, plus_constant (destination, n / 4));
      set_mem_align (mem, BITS_PER_WORD);
      emit_move_insn (mem, temp);
      break;

    case 2:
      temp = gen_reg_rtx (HImode);
      mem = gen_rtx_MEM (HImode, plus_constant (source, n / 4));
      set_mem_align (mem, BITS_PER_WORD);
      emit_move_insn (temp, mem);
      mem = gen_rtx_MEM (HImode, plus_constant (destination, n / 4));
      set_mem_align (mem, BITS_PER_WORD);
      emit_move_insn (mem, temp);
      break;

    case 3:
      temp = gen_reg_rtx (SImode);
      emit_insn (gen_extzv (temp, gen_rtx_MEM (SImode,
					       plus_constant (source, n / 4)),
			    GEN_INT (27), GEN_INT (0)));
      emit_insn (gen_insv (gen_rtx_MEM (SImode,
					plus_constant (destination, n / 4)),
			   GEN_INT (27), GEN_INT (0), temp));
      break;
    }

  return 1;
}

/* Try to emit instructions to MOVSLJ a memory block of LENGTH storage
   units from SOURCE to DESTINATION.  Return nonzero on success.  */
/*
static int
expand_movslj (rtx destination ATTRIBUTE_UNUSED, rtx source ATTRIBUTE_UNUSED, rtx length ATTRIBUTE_UNUSED)
{
  abort ();
}
*/

/* Emit instructions to copy a memory block.  Return nonzero on
   success.  */
int
pdp10_expand_movstrsi (rtx *operands)
{
  /* operands[0] is the block destination address
     operands[1] is the block source address
     operands[2] is the block length
     operands[3] is the known alignment.  */

  operands[0] = XEXP (operands[0], 0);
  operands[1] = XEXP (operands[1], 0);

  if (GET_CODE (operands[3]) != CONST_INT
      || INTVAL (operands[3]) != UNITS_PER_WORD)
    return 0;

  if (GET_CODE (operands[1]) == CONST
      && GET_CODE (XEXP (operands[1], 0)) == PLUS
      && GET_CODE (XEXP (XEXP (operands[1], 0), 1)) == CONST_INT)
    {
      rtx x = XEXP (operands[1], 0);
      HOST_WIDE_INT offset = INTVAL (XEXP (x, 1));
      operands[1] = plus_constant (XEXP (x, 0), offset & ADDRESS_MASK);
    }

  if (use_xblt (operands[0], operands[1], operands[2]))
    return expand_xblt (operands[0], operands[1], operands[2]);
  else
    return expand_blt (operands[0], operands[1], operands[2]);
}

/* Emit instructions to use BLT to clear a memory block of LENGTH
   storage units from SOURCE to DESTINATION.  Return nonzero on
   success.  */
static int
expand_blt_clear (rtx destination, rtx length)
{
  HOST_WIDE_INT len;
  rtx ac, e;

  if (GET_CODE (length) != CONST_INT
      || INTVAL (length) % UNITS_PER_WORD != 0)
    return 0;

  len = INTVAL (length);
  ac = gen_reg_rtx (Pmode);

  if (CONSTANT_ADDRESS_P (destination))
    {
      emit_insn (gen_move_two_halves (ac,
				      destination,
				      plus_constant (destination,
						     UNITS_PER_WORD)));
      e = plus_constant (destination, len - UNITS_PER_WORD);
    }
  else
    {
      emit_insn (gen_MOVSI (ac, destination));
      emit_insn (gen_HRRI (ac, plus_constant (destination, UNITS_PER_WORD)));
      e = gen_reg_rtx (Pmode);
      emit_insn (gen_addsi3 (e, destination,
			     GEN_INT (len - UNITS_PER_WORD)));
    }

  emit_move_insn (gen_rtx_MEM (SImode, destination), const0_rtx);

  /* Beginning at the location addressed by the left half-word of AC,
     move words to another area beginning at the location addressed by
     the right half-word of AC.  Continue until a word is moved to
     location E.  */
  emit_insn (gen_BLT (ac, gen_rtx_MEM (BLKmode, e)));

  return 1;
}

/* Emit instructions to use XBLT to clear a memory block of LENGTH
   storage units from SOURCE to DESTINATION.  Return nonzero on
   success.  */
static int
expand_xblt_clear (rtx destination, rtx length)
{
  rtx temp, acs;
  rtx dest_reg, dest_reg_plus1;
  unsigned int i, n;

  if (GET_CODE (length) != CONST_INT)
    return 0;

  n = INTVAL (length);

  switch (n / UNITS_PER_WORD)
    {
    case 0:
      break;

    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
      for (i = 0; i < n / UNITS_PER_WORD; i++)
	emit_move_insn (gen_rtx_MEM (SImode, plus_constant (destination, i)),
			const0_rtx);
      break;

    default:
      acs = gen_reg_rtx (TImode);
      dest_reg = force_reg (SImode, destination);
      dest_reg_plus1 = force_reg(SImode, plus_constant(dest_reg, 1));
      emit_move_insn (gen_rtx_SUBREG (SImode, acs, 0),
		      GEN_INT (n / UNITS_PER_WORD - 1));
      emit_move_insn (gen_rtx_SUBREG (SImode, acs, 4), dest_reg);
      emit_insn (gen_MOVEI (gen_rtx_SUBREG (SImode, acs, 8), dest_reg_plus1));
      emit_move_insn (gen_rtx_MEM (SImode, destination), const0_rtx);
      emit_insn (gen_XBLT (acs));
    }

  switch (n % UNITS_PER_WORD)
    {
    case 0:
      break;

    case 1:
      temp = gen_rtx_MEM (QImode, plus_constant (destination, n / 4));
      set_mem_align (temp, BITS_PER_WORD);
      emit_move_insn (temp, const0_rtx);
      break;

    case 2:
      temp = gen_rtx_MEM (HImode, plus_constant (destination, n / 4));
      set_mem_align (temp, BITS_PER_WORD);
      emit_move_insn (temp, const0_rtx);
      break;

    case 3:
      temp = gen_reg_rtx (SImode);
      emit_move_insn (temp, GEN_INT (0777));
      emit_insn (gen_andsi3 (gen_rtx_MEM (SImode,
					  plus_constant (destination, n / 4)),
			     gen_rtx_MEM (SImode,
					  plus_constant (destination, n / 4)),
			     temp));
      break;
    }

  return 1;
}

/* Emit instructions to use MOVST to clear a memory block of LENGTH
   storage units from SOURCE to DESTINATION.  Return nonzero on
   success.  */
/*
static int
expand_movst_clear (rtx destination ATTRIBUTE_UNUSED, rtx length ATTRIBUTE_UNUSED)
{
  abort ();
}
*/

/* Emit instructions to clear a memory block.  Return nonzero on
   success.  */
int
pdp10_expand_clrstrsi (rtx *operands)
{
  /* operands[0] is the block destination address
     operands[1] is the block length
     operands[2] is the known alignment.  */

  if (GET_CODE (operands[2]) != CONST_INT
      || INTVAL (operands[2]) != UNITS_PER_WORD)
    return 0;

  operands[0] = XEXP (operands[0], 0);

  if (use_xblt (operands[0], NULL_RTX, operands[1]))
    return expand_xblt_clear (operands[0], operands[1]);
  else
    return expand_blt_clear (operands[0], operands[1]);
}

/* Emit instructions to compare two memory blocks.  Return nonzero on
   success.  */
int
pdp10_expand_cmpstrsi (rtx *operands ATTRIBUTE_UNUSED)
{
  /* operands[0] is the result
     operands[1] is the address of the first string
     operands[2] is the address of the second string
     operands[3] is the string length
     operands[4] is the known alignmen.  */

    return 0;
}


/**********************************************************************

	Pointer Arithmetic

**********************************************************************/

/* Return the value of the S field in byte pointers used to point to
   variables of type TYPE.  A return value of 36 means that a word
   pointer should be used.  */
int
pdp10_bytesize (tree type)
{
  int size=36;  /* unless we figure otherwise, it's a word */

/*
I think using the type of the array elements is a mistake here
A pointer to an array is not the same thing as a pointer to an element of
the array, although in common usage it will be coerced before it gets
dereferenced.  And what about arrays of arrays?
-mtc 7/11/2006
*/
  if (TREE_CODE (type) == ARRAY_TYPE)
    type = TREE_TYPE (type);

  if (VOID_TYPE_P (type))
    size = 9;
  else if (INTEGRAL_TYPE_P (type))
    size = tree_low_cst (TYPE_SIZE (type), 1);

  if (size > 36)
    size = 36;

  return size == 16 ? 18 : size == 32 ? 36 : size;
}

/* Emit a dummy move instruction which will be optimized away later.
   The purpose of this instruction is to fool GCC into not generating
   a REG_EQUAL note for a previously emitted (sequence of)
   instruction(s).

   OP0 and OP1 are two operands that, if convenient, can be used in
   the dummy move.  */
static void
avoid_reg_equal (rtx op0, rtx op1)
{
  rtx temp;

  if (GET_CODE (op0) == REG)
    temp = op0;
  else if (GET_CODE (op1) == REG)
    temp = op1;
  else
    temp = gen_reg_rtx (SImode);

  emit_move_insn (temp, temp);
}

/* Try to emit instructions to use IBP to add INCREMENT to OP1 and
   store the result in OP0.  Return nonzero on success.  */
static int
maybe_expand_ibp (int byte_size, rtx op0, rtx op1, int increment)
{
  if (increment == 0)
    return 1;

  if (HAVE_ADJBP && increment != 1)
    return 0;

  {
    int bytes_per_word;
    int addi, ibp;
    int i;

    bytes_per_word = BITS_PER_WORD / byte_size;

    if (increment > 0)
      addi = increment / bytes_per_word;
    else
      addi = (increment - bytes_per_word + 1) / bytes_per_word;
    ibp = increment - bytes_per_word * addi;
 
   if (addi != 0)
      emit_insn (gen_addsi3 (op0, op1, GEN_INT (addi)));
    else
      {
	if (!reg_or_mem_operand (op1, GET_MODE(op1)))
	  op1 = force_reg (GET_MODE(op1), op1);
	emit_insn (gen_IBP (op0, op1));
	ibp--;
      }

    for (i = 0; i < ibp; i++)
      emit_insn (gen_IBP (op0, op0));
  }

  return 1;
}

/* Emit instructions to add OP1 to the byte pointer in OP0 that points
   to items of type TYPE.  Store the result in TARGET if that's
   convenient.  Return the result.  */
/*	The type parameter is unnecessary here and should eventually
	go away.  All the information about what kind of pointer we have
	should be part of op0, probably just as part of the mode.
	There's also some inconsistency in what units the index op1 is in.
	It ought to be consistently either in bytes, the indexed object size, or
	in the pointer type target size.  There are almost certainly bugs here.
	-mtc 7/18/2006
	931013-2.c seems to demonstarte that expression like T + const, where
	T is an array name are parsed such that the index in the expression tree
	is stored as a byte offset, so begin making changes here on the
	assumption that op1 is in bytes.
	-mtc 7/24/2006
	optimization to use ADDI or SUBI is only valid if we will end up with
	reg_or_mem_operand() operands.  Add a check to try to assure this.
	-mtc 11/8/2010
*/
rtx
pdp10_expand_addbp3 (tree type, rtx target, rtx op0, rtx op1)
{
  HOST_WIDE_INT increment;

  /* First, try to use ADDI or SUBI.  */
  if (GET_CODE (op1) == CONST_INT
      && (increment = INTVAL (op1)) % UNITS_PER_WORD == 0
	  && reg_or_mem_operand(op0, VOIDmode))
    {
      int n = increment / UNITS_PER_WORD;
      if (CONSTANT_P (op0))
	return plus_constant (op0, n);
      else
      	{
      	/*
      	May want to be more sophisticated for MEM nodes and simply refer to memory as a
      	different type.
      	-mtc 7/24/2006
      	*/
      	rtx target_wrapper = simplify_gen_subreg(SImode, target, GET_MODE(target), 0);
	rtx op0_wrapper = simplify_gen_subreg(SImode, op0, GET_MODE(op0), 0);
	emit_insn (gen_addsi3 (target_wrapper, op0_wrapper, GEN_INT (n)));
      	}
      return target;
    }

  /* Second, try to use IBP.  */
  if (GET_CODE (op1) == CONST_INT
      && maybe_expand_ibp (pdp10_bytesize (type), target, op0, increment))
    return target;

  /* If we get here, either the increment is not a constant, or a
     sequence of IBP instructions is too costly.  */

  if (HAVE_ADJBP)
    {
      /* we need target in a reg, but using force_reg like this is probably wrong, since
        if force_reg creates a register, we aren't supposed to modify it subsequently
        -mtc 4/12/2007
        */
      if (target == op0)
	target = gen_reg_rtx (GET_MODE(op0));
      else
	target = force_reg (GET_MODE(target), target);
	  
      /* op1 is the index.  copy it to the target pretending the target is SImode.
      */
      if (!general_operand (op1, GET_MODE(op1)))
	op1 = force_reg (GET_MODE(op1), op1);
      if (GET_MODE(op1) != VOIDmode && GET_MODE(op1) != SImode)
	op1 = simplify_gen_subreg(SImode, op1, GET_MODE(op1), subreg_lowpart_offset(SImode,GET_MODE(op1)));
      emit_move_insn (simplify_gen_subreg(SImode, target, GET_MODE(target), 0), op1);

      /* if op0 is too complex force it into a register
      */
      if (!general_operand (op0, GET_MODE(op0)))
	op0 = force_reg (GET_MODE(op0), op0);
      gcc_assert(GET_MODE(target) == GET_MODE(op0));
	  
      /*	Awkwardly, the first and third arguments to gen_ADJBP need to be the same while the
      		first argument should be a byte pointer and the third an integer.  If we get into trouble
      		we can try some subreg renamings, and if that fails, we may need to redefine the instruction.
      		-mtc 7/27/2006
      		Try the subreg technique 8/1/2006
      		Rework this with 4.1.1 update, switch to different gens for every pointer mode
      		Each mode needs some corresponding patterns in pdp10.md
      		-mtc 4/12/2007
      */
      switch (GET_MODE(target))
      	{
	case SImode:
	      emit_insn (gen_ADJBP (target, op0, target));
	      break;
	case Q8Pmode:
	      emit_insn (gen_ADJBP_Q8P (target, op0, target));
	      break;
	case Q9Pmode:
	      emit_insn (gen_ADJBP_Q9P (target, op0, target));
	      break;
	case HPmode:
	      emit_insn (gen_ADJBP_HP (target, op0, target));
	      break;
	default:
		/* need to add handling of another modes that arrive here */
		gcc_assert(false);
      	}
	  
      return target;
    }
  else
    abort ();
}

/* Emit instructions to add/substract to/from a pointer.  */
static rtx
expand_add_or_sub_pointer (rtx target, rtx op0, tree type0, rtx op1, tree type1, int add)
{
  int neg = 0;
  /*	 use of this type parameter should eventually go away
  	-mtc 7/17/2006
  */
  tree type = TREE_TYPE(POINTER_TYPE_P (type0) ? type0 : type1);
  HOST_WIDE_INT target_size;
  enum machine_mode pointer_mode;

  gcc_assert ((POINTER_TYPE_P(type0) || POINTER_TYPE_P(type1)) && 
  			(!POINTER_TYPE_P(type0) || !POINTER_TYPE_P(type1)));

  /*	we've lost track of which operator is the pointer and which is the offset, so figuring out
  	what type of pointer we're dealing with is a bit convoluted
	-mtc 7/17/2006
  */
  /*if (GET_MODE(op0) != Pmode && PTR_MODE_P(GET_MODE(op0)))
  	pointer_mode = GET_MODE(op0);
  else if (GET_MODE(op1) != Pmode && PTR_MODE_P(GET_MODE(op1)))
	pointer_mode = GET_MODE(op1);
  else
  	pointer_mode = Pmode;*/

  /*  it turns out we can't rely on op0 and op1 to have the correct type info
       for example, a character array ref will just be a symbol ref and we need to look at
       type to determine what kind of pointer it should be interpreted as
       -mtc 5/1/2007
  */
  if (POINTER_TYPE_P (type0))
  	pointer_mode = pdp10_mode_for_type(type0);
  else if (POINTER_TYPE_P (type1))
  	pointer_mode = pdp10_mode_for_type(type1);
  else
  	gcc_assert(0);
  
  /* normalize so that op0 is always the pointer operand */
  if (POINTER_TYPE_P(type1))
  	{
	rtx xtemp;
	tree ttemp;
	xtemp = op0; ttemp = type0;
	op0 = op1; type0 = type1;
	op1 = xtemp; type1 = ttemp;

	/* negating a pointer doesn't make sense */
	gcc_assert (add);
  	}

/* Handle the case that the pointer is a constant.  Optimization of this should be possible, but
    for now just force to a register.  Note the existence of similar code in expand_subtract_pointers().
    -mtc 8/28/2007
*/
  if (GET_MODE(op0) == VOIDmode)
	op0 = force_reg (pdp10_mode_for_type(type0), op0);

  /* convert the pointer if necessary into the appropriate type */
  if (GET_MODE(op0) != pointer_mode)
  	{
  	op0 = pdp10_convert_ptr (NULL, op0, NULL, ptr_mode_target_size(GET_MODE(op0)), ptr_mode_target_size(pointer_mode), 1);
  	}

/*
The following comment out block should never happen any more
-mtc 4/17/2007
*/
	gcc_assert(GET_CODE(op0) != CONST_INT);
/*
  if (!add && GET_CODE (op0) == CONST_INT)
    {
      rtx tmp = GEN_INT (-INTVAL (op0));
      op0 = op1;
      op1 = tmp;
      neg = 1;
      add = 1;
    }
*/

/* In addition to checking if target is specified, also check whether it is in the
    proper mode.
    -mtc 4/22/2009
*/
  if (!target || GET_MODE(target) != pointer_mode)
    target = gen_reg_rtx (pointer_mode);


/* If the target size doesn't match the pointer mode size, we need to adjust the index
    accordingly
    There are probably problems here when we have unaligned arrays of 8 bit characters
    -mtc 7/16/2007
    Add check for target_size == -1 to handle pointers to VOID
    -mtc 7/17/2007
    Test was wrong because it confused bits and bytes.
    Not sure what to do if sub-byte size doesn't match pointer mode so just assert
    -mtc 8/15/2007
    Allow pointer mode size to not divide into the target size if the target size is a word multiple
    -mtc 3/18/2009
    Split how we handle target size >= a word and target size < a word and correct the formula
    for when target size >= word to correctly handle the byte pointer pad bits.
    -mtc 4/6/2010
*/
  target_size = int_size_in_bytes (type);
  if (target_size == -1)
  	target_size = UNITS_PER_WORD;
  if (target_size == 1)
  	{
  	gcc_assert (TYPE_PRECISION(type) == ptr_mode_target_size(pointer_mode));
  	}
  else if (target_size * BITS_PER_UNIT != ptr_mode_target_size(pointer_mode))
  	{
	if (target_size >= UNITS_PER_WORD)
		{
		gcc_assert(target_size % UNITS_PER_WORD == 0);
		op1 = simplify_gen_binary (MULT, 
			(GET_MODE(op1) ? GET_MODE(op1) : SImode), 
			op1, GEN_INT((target_size / UNITS_PER_WORD) * (BITS_PER_WORD / ptr_mode_target_size(pointer_mode))));
		}
	else
		{
		gcc_assert((target_size * BITS_PER_UNIT) % ptr_mode_target_size(pointer_mode) == 0);
		op1 = simplify_gen_binary (MULT, 
			(GET_MODE(op1) ? GET_MODE(op1) : SImode), 
			op1, GEN_INT((target_size * BITS_PER_UNIT) / ptr_mode_target_size(pointer_mode)));
		}
  	}

  if (ptr_mode_target_size(pointer_mode) >= BITS_PER_WORD)
    {
      rtx increment;

      if (GET_CODE (op1) == CONST_INT)
	{
	  HOST_WIDE_INT i = INTVAL (op1);
	  HOST_WIDE_INT sign = TARGET_EXTENDED ?
	    (HWINT (1) << 29) : (HWINT (1) << 17);

	  if (CONSTANT_ADDRESS_P (op0))
	    {
	      emit_move_insn (target, plus_constant (op0, add ? i : -i));
	      if (neg)
		target = gen_rtx_NEG (GET_MODE(target), target);
	      return target;
	    }

	  i &= ADDRESS_MASK;
	  i = (i ^ sign) - sign;
	  if (!add)
	    {
	      i = -i;
	      add = 1;
	    }
	  increment = GEN_INT (i);
	}
      else if (GET_MODE(op1) == SImode || GET_MODE(op1) == VOIDmode)
	increment = op1;
      else
      	increment = simplify_gen_subreg(SImode, op1, GET_MODE(op1), subreg_lowpart_offset(SImode, GET_MODE(op1)));

      /* Make sure that the first operand is a register, if possible.  */
      if (add && GET_CODE (op0) != REG && GET_CODE (increment) == REG)
	{
	  rtx tmp = op0;
	  op0 = increment;
	  increment = tmp;
	}

      if (!register_operand (op0, GET_MODE(op0)))
	op0 = force_reg (GET_MODE(op0), op0);
      if (!reg_or_mem_operand (increment, GET_MODE(increment)))
	increment = force_reg ((GET_MODE(increment) != VOIDmode ? GET_MODE(increment) : SImode), increment);

      if (add)
	emit_insn (gen_addsi3 (target, op0, increment));
      else
	emit_insn (gen_subsi3 (target, op0, increment));
    }
  else
    {
      rtx increment = op1;

      if (!add)
	{
	  if (GET_CODE (increment) == CONST_INT)
	    increment = GEN_INT (-INTVAL (increment));
	  else
	    {
	      rtx temp = gen_reg_rtx (GET_MODE(increment));
	      emit_insn (gen_negsi2 (temp, increment));
	      increment = temp;
	    }
	}

      target = pdp10_expand_addbp3 (type, target, op0, increment);
    }

  /* This is to fool GCC into not generating a REG_EQUAL note for this
     sequence.  */
  avoid_reg_equal (op0, op1);

  if (neg)
    return expand_simple_unop (GET_MODE(target), NEG, target, NULL_RTX, 0);

  return target;
}

/* Emit instructions to subtract two byte pointers.  */
static rtx
expand_subtract_pointers (enum machine_mode mode, rtx target, rtx op0, tree type0, rtx op1, tree type1)
{
  tree type = TREE_TYPE (type0);
  HOST_WIDE_INT typesize = int_size_in_bytes (type);

  if (!target)
    target = gen_reg_rtx (Pmode);

  if (WORD_TYPE_P (type))
    {
      /* Calculate the difference between two word pointers by
	 subtracting  */
	/*
	We want word and bigger pointer offsets to be done in words and smaller
	pointers to be done in bytes, so don't multiply by 4.
	If op0 and op1 refer to the same variable, we could optimize this to be the
	constant value 0, but probably this gets optimized by non-zero optimization
	anyhow
	-mtc 6/30/2006
	pointer difference includes an implicit division by the the size of the referenced objects
	that size is always in bytes, so first adjust by UNITS_PER_WORD
	note that word pointers are inherently word aligned, so the size must be a word multiple
	- mtc 7/18/2007
	*/
      gcc_assert (typesize % UNITS_PER_WORD == 0);
      typesize /= UNITS_PER_WORD;

      if (typesize == 1)
	  emit_insn (gen_subsi3 (target, op0, op1));
      else
      	{
      	rtx temp = gen_reg_rtx(SImode);
        emit_insn (gen_subsi3(temp, op0, op1));
        emit_insn (gen_divsi3(target, temp, GEN_INT(typesize)));
      	}
    }

  /*
  Pointers to VOID are special.  It's unclear exactly when this can happen since operating on VOID
  pointers is illegal, but we seem to at least get the case where the difference of two addresses is taken
  -mtc 7/24/2007
  Change handling of remaining VOID pointer differences to report an error and generate a result value of 0
  -mtc 2/18/2010
  */
  else if (TREE_CODE (type) == VOID_TYPE)
    {
	  if ((GET_CODE(op0) != LABEL_REF) ||
		  (GET_CODE(op1) != LABEL_REF) ||
		  (GET_MODE(op0) != GET_MODE(op1)))
		{
		  error("Invalid VOID pointer arithmetic");
		  emit_move_insn(target, const0_rtx);
		}
	  else
		emit_insn (gen_subsi3 (target, op0, op1));
    }
  
  else
    {
      /*
      since the parser no longer generates a divide by the referenced object size we need
      to adjust here.
      for now just assert the adjustment is by 1 because I want to look at examples where it
      isn't
      -mtc 7/18/2007
      modify assert to check HPmode differently.  There are probably still cases where the referenced
      object size differs from the inherent pointer reference size in which case I expect we need
      to divide by the object size divided by the pointer reference size
      -mtc 7/24/2007
      it's possible for op0 or op1 to be a const_int which has mode VOIDmode if an explicit conversion
      from a constant or and assignment from a constant is done.  The pointer difference of these values
      could probably be optimized, but for now just force these values into registers so the rest of the
      code works.  I don't think we want to encourage this type of programming anyhow.
      -mtc 8/28/2007
      forcing constants into registers doesn't actually work.  We need to do pointer conversion to get
      the actual bits right
      Also add check for diff of two constants.  Do we need to verify that modes match or adjust for mode differences?
      -mtc 11/27/2007
      */
      if (GET_MODE(op0) == VOIDmode &&
	  GET_MODE(op1) == VOIDmode &&
	  GET_CODE(op0) == CONST_INT &&
	  GET_CODE(op1) == CONST_INT)
      	{
	rtx diff = GEN_INT(INTVAL(op0) - INTVAL(op1));
	emit_move_insn (target, diff);
	if (GET_MODE (target) != mode)
	    target = convert_to_mode (mode, target, 0);
	return target;
      	}

      if (GET_MODE(op0) == VOIDmode)
      	{
      	if (GET_CODE(op0) == CONST_INT)
      		{
		HOST_WIDE_INT val = INTVAL(op0);
		HOST_WIDE_INT wordpart = val / UNITS_PER_WORD;
		HOST_WIDE_INT extrabytes = val % UNITS_PER_WORD;
	
		op0 = convert_global_pointer (GEN_INT(wordpart), 36, ptr_mode_target_size (pdp10_mode_for_type(type0)), extrabytes);
      		}
	else
		op0 = force_reg (pdp10_mode_for_type(type0), op0);
      	}
	  
      if (GET_MODE(op1) == VOIDmode)
      	{
      	if (GET_CODE(op1) == CONST_INT)
      		{
		HOST_WIDE_INT val = INTVAL(op1);
		HOST_WIDE_INT wordpart = val / UNITS_PER_WORD;
		HOST_WIDE_INT extrabytes = val % UNITS_PER_WORD;
	
		op1 = convert_global_pointer (GEN_INT(wordpart), 36, ptr_mode_target_size (pdp10_mode_for_type(type1)), extrabytes);
      		}
	else
		op1 = force_reg (pdp10_mode_for_type(type1), op1);
      	}


      if (GET_MODE(op0) == HPmode)
	  	gcc_assert (typesize == 2);
      else
      		gcc_assert (typesize == 1);
	  
      gcc_assert (PTR_MODE_P(GET_MODE(op0)));
      gcc_assert (GET_MODE(op0) == GET_MODE(op1));
      if (HAVE_SUBBP)
	{
	  if (!register_operand (op0, VOIDmode))
	    op0 = force_reg (GET_MODE(op0), op0);
	  if (!reg_or_mem_operand (op1, VOIDmode))
	    op1 = force_reg (GET_MODE(op1), op1);
	  emit_insn (gen_SUBBP (target, op0, op1));
	}
      else
	{
	  /* Calculate the difference between two byte pointers.

	     This algorithm is copied from KCC with some small
	     modifications.  Basically, the steps are:

		; Subtract the pointers.
		SUB op0,op1

		; Multiply the difference by the number of bytes per
		; word and a magic shift factor.
		MULI op0,<bytes per word>*<2^shift>

		; Shift back the lower word of the product to
		; compensate for the shift factor.  op0+1 will be an
		; approximation of the final result.
		ASH op0+1,-shift

		; Add a magic value from a table to adjust for the
		; difference in byte position.  The table is indexed
		; by op0 which is the upper bits of the result of
		; the multiplication, and thus depends on the byte
		; positions in the pointers.
		ADD op0+1,table(op0)

	     The result is in op0+1.  */

	  int bytesize = pdp10_bytesize (type);
	  rtx temp = gen_reg_rtx (DImode), diff;
	  rtx a = gen_rtx_SUBREG (SImode, temp, 0);
	  char *str;
	  int shift, mul;
	  rtx sym, mem;

	  if (bytesize == 16)
	    bytesize = 18;

	  /* ADDRESS: the code differs slightly in extended and
	     unextended mode.  */

	  if (TARGET_EXTENDED)
	    {
	      if (bytesize == 18 || bytesize == 7)
		shift = 5;
	      else
		shift = 4;
	    }
	  else
	    shift = 1;
	  mul = (BITS_PER_WORD / bytesize) << shift;

	  str = xstrdup ("*BAD..");
	  str[4] = TARGET_EXTENDED ? 'X' : 'L';
	  str[5] = bytesize == 18 ? 'H' : '0' + bytesize;
	  sym = gen_rtx_SYMBOL_REF (Pmode, str);
	  used_byte_subtraction_table[bytesize] = 1;

	  diff = gen_reg_rtx (SImode);

	  if (GET_MODE (op0) != SImode)
	    {
	    if (!REG_P(op0))
			op0 = force_reg(GET_MODE(op0), op0);
	    op0 = convert_to_mode (SImode, op0, 1);
	    }
	  if (GET_MODE (op1) != SImode)
	    {
	    if (!REG_P(op1))
			op1 = force_reg(GET_MODE(op1), op1);
	    op1 = convert_to_mode (SImode, op1, 1);
	    }

	  emit_insn (gen_subsi3 (a, op0, op1));
	  emit_insn (gen_MUL (temp, a, GEN_INT (mul)));
	  emit_move_insn (diff, gen_rtx_SUBREG (SImode, temp, 4));
	  emit_insn (gen_ashrsi3 (diff, diff, GEN_INT (shift)));

	  if (TARGET_EXTENDED && !(TARGET_SMALL || TARGET_MEDIUM))
	    {
	      emit_insn (gen_addsi3 (a, a, sym));
	      mem = gen_rtx_MEM (SImode, a);
	    }
	  else
	    mem = gen_rtx_MEM (SImode, gen_rtx_PLUS (Pmode, a, sym));

	  emit_insn (gen_addsi3 (diff, diff, mem));

/* Deleted this apparently misguided attempt to adjust for half-word pointers
    We want pointer difference to the number of referenced items difference
    so no adjustment should ever happen here
    -mtc 7/30/2007
    
	  if (bytesize == 18)
	    emit_insn (gen_ashlsi3(diff, diff, GEN_INT (1)));
*/

	  emit_move_insn (target, diff);
	}
    }

  /* This is to fool GCC into not generating a REG_EQUAL note for this
     sequence.  */
  avoid_reg_equal (op0, op1);

  if (GET_MODE (target) != mode)
    target = convert_to_mode (mode, target, 0);

  return target;
}


/**********************************************************************

	Unconditional Jumps

**********************************************************************/

/* Return nonzero if the call instruction can be output as JRST.  */
int
pdp10_output_call_as_jrst_p (rtx insn)
{
  return SIBLING_CALL_P (insn) || find_reg_note (insn, REG_NORETURN, 0);
}

int
pdp10_output_call_as_jsp_p (rtx sym)
{
  tree id = maybe_get_identifier (XSTR (sym, 0));
  tree fn, type, attr;

  return
    id != NULL_TREE
    && (fn = lookup_name (id)) != NULL_TREE
    && TREE_CODE (TREE_TYPE (fn)) == FUNCTION_DECL
    && (type = TREE_TYPE (fn)) != NULL_TREE
    && (attr = TYPE_ATTRIBUTES (type)) != NULL_TREE
    && lookup_attribute ("fastcall", attr);
}


/**********************************************************************

	Conditional Jumps

**********************************************************************/

/* Output a JRST instruction */
const char *
pdp10_output_jrst (rtx label)
{
  output_asm_insn ("jrst %0", &label);
  return "";
}

/* Generate code to invert the sign bit of X and return the result.
   Called to prepare operands for an unsigned comparison.  */
rtx
pdp10_flip_sign_bit (rtx x)
{
  if (GET_CODE (x) == CONST_INT)
    return GEN_INT (trunc_int_for_mode (INTVAL (x) ^ SIGN_BIT, SImode));
  else
    {
      rtx y = gen_reg_rtx (SImode);
      if (GET_MODE (x) == DImode)
	x = gen_rtx_SUBREG (SImode, x, 0);
      else if (!register_operand (x, SImode))
	x = force_reg (SImode, x);
      emit_insn (gen_xorsi3 (y, x, GEN_INT (trunc_int_for_mode (SIGN_BIT, SImode))));
      return y;
    }
}

/* Return a suitable T[RLD]N[EN] instruction.  OPERANDS are the
   operands and LENGTH is the RTL insn length.  */
const char *
pdp10_output_test_and_skip (rtx insn, rtx *operands)
{
/* removed [] in alternative 2
    case in CMD_Parse() in main.c DXM code shows that this is redundant with what
    the operand output code already generates.
    -mtc 11/14/2007
    that removed [] sometime, so add another case for constants and use format 'i'
    which comments in print_word_address indicate forces []
    -mtc 11/19/2007
*/
  static const char *asms[] =
  { "trn%0 %1,%2",
    "tln%0 %1,%S2",
    "tdn%0 %1,%i2",
    "tdn%0 %1,%2" };
  int which_alternative = 3;

  if (GET_CODE (operands[2]) == CONST_INT)
    {
      HOST_WIDE_INT i = INTVAL (operands[2]);

      if (CONST_OK_FOR_LETTER_P (i, 'I'))
	which_alternative = 0;
      else if (CONST_OK_FOR_LETTER_P (i, 'L'))
	which_alternative = 1;
      else
  	which_alternative = 2;
    }

  if (get_attr_length (insn) == 1)
    return asms[which_alternative];
  else
    {
      /* Reverse the skip condition and output a JRST.  */
      operands[0] = gen_rtx_fmt_ee (reverse_condition (GET_CODE (operands[0])),
				    VOIDmode, NULL_RTX, NULL_RTX);
      output_asm_insn (asms[which_alternative], operands);
      return pdp10_output_jrst (operands[3]);
    }
}


/**********************************************************************

	Function Prologue and Epilogue

**********************************************************************/

#define emit_frame_insn(PATTERN) RTX_FRAME_RELATED_P (emit_insn (PATTERN)) = 1

#define emit_eliminable_frame_insn(PATTERN) \
do { \
  rtx insn = emit_insn ((PATTERN)); \
  RTX_FRAME_RELATED_P (insn) = 1; \
  /* deleted with 430 update 12/4/2007  REG_NOTES (insn) = gen_rtx_EXPR_LIST (REG_MAYBE_DEAD, NULL_RTX, NULL); */\
} while (0)

/* Return the number of clobbered registers that the current function
   should save on stack, and mark those registers in the CLOBBERED
   array.  */
static int
clobbered_regs (int *clobbered)
{
  int i, n;

  for (i = 0; i < FIRST_PSEUDO_REGISTER; i++)
    clobbered[i] = df_regs_ever_live_p(i) && !call_used_regs[i];

  clobbered[STACK_POINTER_REGNUM] = 0;
  clobbered[HARD_FRAME_POINTER_REGNUM] = frame_pointer_needed;

  n = 0;
  for (i = 0; i < FIRST_PSEUDO_REGISTER; i++)
    {
      if (clobbered[i])
	n += UNITS_PER_WORD;
    }

  return n;
}

/* Emit instructions to save or restore the return address on stack.
   This only happens when the register arguments must be stored in the
   stack.  */
static void
expand_save_or_restore_return_address (int save, int clobbered_size)
{
  int offset1, offset2;

  if (!current_function_pretend_args_size)
    return;

  /* If we need to store pretend args, move the return address out of
     their way.  */

  offset1 = - (current_function_outgoing_args_size +
	       get_frame_size () +
	       clobbered_size +
	       current_function_pretend_args_size) / UNITS_PER_WORD;
  offset2 = - (current_function_outgoing_args_size +
	       get_frame_size () +
	       clobbered_size) / UNITS_PER_WORD;

  if (!save)
    {
      int tmp = offset1;
      offset1 = offset2;
      offset2 = tmp;
    }

  emit_move_insn (gen_rtx_REG (SImode, 0),
		  GEN_MEM_SP_PLUS (SImode, offset1));
  emit_move_insn (GEN_MEM_SP_PLUS (SImode, offset2),
		  gen_rtx_REG (SImode, 0));
}

/* Save or restore clobbered callee-saved registers.  SAVE is nonzero
   when saving, and zero when restoring.  CLOBBERED indicates
   clobbered registers.  SINGLE is nonzero when a single PUSH or POP
   instruction can be used. */

static void
expand_save_or_restore_regs (int save, int *clobbered, int clobbered_size, int single)
{
  int fp_clobbered = 0;
  int go_back = 1;
  int max_move;
  int min_blt;
  int offset;
  int regs;
  int i, j, k;
  
  if (clobbered_size == 0)
    return;
  
  offset = -((current_function_outgoing_args_size +
	      get_frame_size () +
	      clobbered_size)
	     / UNITS_PER_WORD) + 1;
  
  max_move = MOVE_MAX / UNITS_PER_WORD;
  
  /* ADDRESS: Don't use BLT when generating multi-section code.
     Otherwise, use BLT if using (D)MOVE instructions would generate
     more code.  */
  if (TARGET_EXTENDED)
    min_blt = 1000;
  else
    min_blt = 3 * max_move + 1;
  
  /* Start with the frame pointer.  */
  for (i = HARD_FRAME_POINTER_REGNUM; i < 16;
       (i == HARD_FRAME_POINTER_REGNUM && go_back) ? (i = go_back = ASM_SCRATCH_REGNUM) : (i += regs))
    {
      regs = 1;
      if (!clobbered[i])
	continue;
      
      /* Find a consecutive block of clobbered registers.
	 
      ADDRESS: When extended addressing is in use, the stack can
      span multiple sections, so BLT can't be used.  In this case,
      the search is limited to just examine the next register.  */

      if (i == HARD_FRAME_POINTER_REGNUM)
	{
	  k = i + 1;
	  fp_clobbered = 1;
	  clobbered[i] = 0;
	}
      else if (TARGET_EXTENDED
	       && !TARGET_SMALL
	       && i <= FIRST_PSEUDO_REGISTER - 2)
	k = i + 2;
      else
	k = FIRST_PSEUDO_REGISTER;
      
      for (j = i + 1; j < k; j++)
	{
	  if (clobbered[j])
	    regs++;
	  else
	    break;
	}
      
      if (regs >= min_blt)
	{
	  /* Use a BLT instruction.  */
	  
	  rtx from_start, to_start, to_end;
	  rtx scratch = gen_rtx_REG (Pmode, ASM_SCRATCH_REGNUM);
	  
	  if (save)
	    {
	      from_start = GEN_INT (i);
	      to_start = plus_constant (stack_pointer_rtx, offset);
	      to_end
		= GEN_MEM_SP_PLUS (BLKmode, offset + regs - 1);
	    }
	  else
	    {
	      from_start = plus_constant (stack_pointer_rtx, offset);
	      to_start = GEN_INT (i);
	      to_end
		= gen_rtx_MEM (BLKmode, GEN_INT (i + regs - 1));
	    }
/* The call to gen_MOVEI will be problematic if to_start is not a general_operand, and
     in particular if it is a PLUS node, as the code in the (save) case above seems to create it
     If this proves true, first try wrapping it with force_reg() and if that fails substitute gen_addsi()
     -mtc 11/29/2006
*/
	  emit_frame_insn (gen_MOVEI (scratch, to_start));
	  emit_frame_insn (gen_HRLI (scratch, from_start));
	  emit_frame_insn (gen_BLT (scratch, to_end));
	}
      else if (single)
	{
	  /* Use a single PUSH or POP instruction.  */
	  /* Single should only be set when there is exactly one register to save and SP has not already */
	  /* been adjusted.  With a little work this could actually be made to work for multiple PUSHes */
	  /* making it work for multiple POPs would require reversing the order of the loop */
	  /* -mtc 3/8/2013 */
	  if (save)
	    emit_insn(GEN_PUSH (gen_rtx_REG (SImode, i)));
	  else
	    emit_insn(GEN_POP (gen_rtx_REG (SImode, i)));
      	}
      else
	{
	  /* Use a single MOVE or DMOVE instruction.  */
	  
	  enum machine_mode mode;
	  rtx insn;
	  
	  if (regs > max_move)
	    regs = max_move;
	  mode = (regs == 1 ? SImode : DImode);
	  
	  if (save)
	    insn = emit_move_insn (GEN_MEM_SP_PLUS (mode, offset),
				   gen_rtx_REG (mode, i));
	  else
	    insn = emit_move_insn (gen_rtx_REG (mode, i),
				   GEN_MEM_SP_PLUS (mode, offset));
	  
	  /* Mark the register save insn as frame related, but eliminable 
	     if the register turns out to not be clobbered after optimization */
	  
	  RTX_FRAME_RELATED_P (insn) = 1;

	  /* REG_MAYBE_DEAD notes removed in 430 update
	      -mtc 12/4/2007
          REG_NOTES (insn) = gen_rtx_EXPR_LIST (REG_MAYBE_DEAD, NULL_RTX, NULL);
          */
	}
      offset += regs;
    }
  clobbered[HARD_FRAME_POINTER_REGNUM] = fp_clobbered;
}

/* Emit an instruction to set up the frame pointer.  */
static void
expand_set_up_frame_pointer (void)
{
  int offset;

  if (!frame_pointer_needed)
    return;

  INITIAL_ELIMINATION_OFFSET (HARD_FRAME_POINTER_REGNUM, STACK_POINTER_REGNUM,
			      offset);
/* Just use gen_movsi() instead of gen_MOVEI()
     -mtc 3/12/2013
*/
  emit_eliminable_frame_insn (gen_movsi (hard_frame_pointer_rtx, 
  							plus_constant(stack_pointer_rtx, offset)));
}

/* Emit an instruction to adjust the stack pointer by an amount
   specified by ADJUST.  */
rtx
pdp10_gen_stack_adjust (rtx adjust)
{
  rtx insn;

  /* ADDRESS: Use addition in extended mode, ADJSP otherwise.  */
  if (TARGET_EXTENDED)
    insn = gen_addsi3 (stack_pointer_rtx,
			   stack_pointer_rtx,
			   adjust);
  else
      insn = gen_ADJSP (stack_pointer_rtx, adjust);

  return insn;
}

/* Emit function prologue instructions.  */
void
pdp10_expand_prologue (void)
{
  int clobbered[FIRST_PSEUDO_REGISTER];
  int clobbered_size;
  int stack_adjust;
  int single_push;

  clobbered_size = clobbered_regs (clobbered);

  /* If only one word is to be saved on stack, it can be stored with a
     single PUSH instruction.  */
  single_push = 
    clobbered_size == UNITS_PER_WORD
    && current_function_outgoing_args_size == 0
    && current_function_pretend_args_size == 0
    && get_frame_size () == 0;

  stack_adjust =
    current_function_outgoing_args_size
    + get_frame_size ()
    + clobbered_size
    + current_function_pretend_args_size;
  if (!single_push && stack_adjust > 0)
    emit_frame_insn (pdp10_gen_stack_adjust (GEN_INT (stack_adjust / 4)));

  expand_save_or_restore_return_address (SAVE, clobbered_size);
  expand_save_or_restore_regs (SAVE, clobbered, clobbered_size, single_push);
  expand_set_up_frame_pointer ();
}

/* Emit function epilogue instructions.  Emit the final popj
   instruction only if POPJ is nonzero.  */
void
pdp10_expand_epilogue (int popj)
{
  int clobbered[FIRST_PSEUDO_REGISTER];
  int clobbered_size;
  int stack_adjust;
  int single_pop;

  clobbered_size = clobbered_regs (clobbered);

  /* If only one word was saved on stack, it can be restored with a
     single POP instruction.  */
  single_pop =
    clobbered_size == UNITS_PER_WORD
    && current_function_outgoing_args_size == 0
    && current_function_pretend_args_size == 0
    && get_frame_size () == 0;

  expand_save_or_restore_regs (RESTORE, clobbered, clobbered_size, single_pop);
  expand_save_or_restore_return_address (RESTORE, clobbered_size);

  stack_adjust =
    current_function_outgoing_args_size
    + get_frame_size ()
    + clobbered_size
    + current_function_pretend_args_size;
  if (!single_pop && stack_adjust > 0)
    emit_frame_insn (pdp10_gen_stack_adjust (GEN_INT (-stack_adjust / 4)));

  if (popj)
    emit_jump_insn (gen_POPJ_17 ());
}

const char *
pdp10_output_return (void)
{
  if (lookup_attribute ("fastcall",
			TYPE_ATTRIBUTES (TREE_TYPE (current_function_decl))))
    return "jrst (?)";
  else
    return "RET";
}


/**********************************************************************

	Unsorted

**********************************************************************/

rtx pdp10_compare_op0, pdp10_compare_op1;

/* Print a number to the stdio stream STREAM.  X is the number to
   print, DIV is a divisor to divide by, BITS is the precision in
   number of bits.  ZERO is zero if zero should be printend, and
   nonzero if zero should be supressed.  */
static void
pdp10_print_number (FILE *stream, HOST_WIDE_INT x, int div, int bits, int zero)
{
  HOST_WIDE_INT mask;

  if (x < 0)
    x -= div - 1;
  x /= div;

  if (bits < 0)
    {
      bits = -bits;
      if (x < 0)
	if (x >= -01000000)
	  {
	    fputc ('-', stream);
	    x = -x;
	  }
    }

  mask = HWINT (-1) << bits;
  if ((x & mask) != 0 && (x & mask) != mask)
    fprintf (stream, "pdp10.c:%d:TOOBIG:", __LINE__);

  x &= ~mask;

  if (zero == 0 || x > 0)
    {
      if (bits == 9)
	{
	  fprintf (stream, "%03lo", (unsigned HOST_WIDE_INT) x);
	  return;
	}
      if (macro_ascii)
      if (bits == 8)
      if (x >= ' ')
      if (x <= '~')
	{
	  if (x == '"')
	    fprintf (stream, "\"\"\"\"");
	  else
	    fprintf (stream, "\"%c\"", (char) x);
	  return;
	}
      fprintf (stream, HOST_WIDE_INT_PRINT_OCT, x);
    }
}

/* Return the expression EXP hiding inside NOP_EXPR and CONVERT_EXPR.  */
/* But don't go past a node with type POINTER_TYPE_P */
/* Note that this routine is strictly for the use of pdp10_expand_binop when handing a pointer difference */
/* -mtc 10/16/2006 */
static tree
strip_conversions (enum tree_code code, tree type, tree exp)
{
  if ((TREE_CODE (exp) == NOP_EXPR || TREE_CODE (exp) == CONVERT_EXPR)
      && ((type == ptrdiff_type_node && TREE_TYPE (exp) == ptrdiff_type_node)
	  || (code == MINUS_EXPR && POINTER_TYPE_P (type) &&
	      POINTER_TYPE_P (TREE_TYPE (exp)))))
    {
      while ((TREE_CODE (exp) == NOP_EXPR || TREE_CODE (exp) == CONVERT_EXPR)
	  	&& !POINTER_TYPE_P(TREE_TYPE(exp)))
	exp = TREE_OPERAND (exp, 0);
    }
  return exp;
}

/* Expand RTL instructions from a binary parse tree expression.  */
rtx
pdp10_expand_binop (enum machine_mode mode,
							optab binoptab,
							rtx op0,
							rtx op1,
							rtx target,
							int unsignedp,
							enum optab_methods methods,
							tree exp)
{
  enum tree_code code = TREE_CODE (exp);
  tree exp0unstripped = NULL, exp1unstripped = NULL, exp0 = NULL, exp1 = NULL, type0 = NULL, type1 = NULL;
  int ptr0 = 0, ptr1 = 0, add = 0;
  int mode0, mode1;
  int unsignedp0, unsignedp1;

  /* GCC parses a pointer subtraction like this:
        T *x, *y;
        int n = x - y;
     as:
	int n = (ptrdiff_t)x - (ptrdiff_t)y;
     Therefore, we need to strip away the ptrdiff_t conversions from the
     operands in order to see their real types.  */

  exp0unstripped = TREE_OPERAND(exp, 0);
  exp0 = strip_conversions (code, TREE_TYPE (exp), exp0unstripped);
  type0 = TREE_TYPE (exp0);
  ptr0 = POINTER_TYPE_P (type0);
  mode0 = pdp10_mode_for_type(type0);
  unsignedp0 = TYPE_UNSIGNED(type0);

  if (TREE_CODE_CLASS (code) == tcc_comparison || TREE_CODE_CLASS (code) == tcc_binary)
    {
      exp1unstripped = TREE_OPERAND(exp, 1);
      exp1 = strip_conversions (code, TREE_TYPE (exp), exp1unstripped);
      type1 = TREE_TYPE (exp1);
      ptr1 = POINTER_TYPE_P (type1);
      mode1 = pdp10_mode_for_type(type1);
      unsignedp1 = TYPE_UNSIGNED(type1);
    }

  switch (code)
    {
    case POINTER_PLUS_EXPR:
	gcc_assert(ptr0 && !ptr1);
	return expand_add_or_sub_pointer (target, op0, type0, op1, type1, 1);
	break;
		
    case PLUS_EXPR:
    case MINUS_EXPR:
	/*
	If either operand is a pointer expressions, the opn we were passed is probably wrong
	so recompute it.
	Note that fixing the parser to not convert the operands of a ptrdiff to type ptrdiff would
	eliminate the need for both this and the bizzare strip_conversions calls
	-mtc 10/16/2006
	Limit recomputation to when the strip_conversions() call actually did something.
	-mtc 12/19/2006
	Parser should also be fixed now, so these tests shouldn't ever execute.
	-mtc 12/19/2006
	*/
	if (ptr0 & (exp0 != exp0unstripped))
		op0 = expand_expr(exp0, NULL_RTX, VOIDmode, EXPAND_NORMAL);
	if (ptr1 & (exp1 != exp1unstripped))
		op1 = expand_expr(exp1, NULL_RTX, VOIDmode, EXPAND_NORMAL);


      add = (code == PLUS_EXPR);
      if (GET_CODE (op0) == NEG)
	{
	  op0 = XEXP (op0, 0);
	  add = !add;
	}
      if (GET_CODE (op1) == NEG)
	{
	  op1 = XEXP (op1, 0);
	  add = !add;
	}
      if (!add && ptr0 && ptr1)
	/*
	We've done some clever manipulation to map all the different cases into whether
	this is an add or a subtract, but unfortunately we've lost track of whether the
	subtraction gives us the desired result or negative of the desired result.
	I wonder if the situation that the result is wrong occurs.
	-mtc 6/30/2006
	*/
	return expand_subtract_pointers (mode, target, op0, type0, op1, type1);
      if (ptr0 || ptr1)
	{

	/* 
	This bit of chicanery causes problems in the 4.1.1 update
	I don't see anything to be gained by passing type values that don't match the op values,
	especially now that the modes and types properly describe the operands.
	-mtc 4/17/2007
	
	  if (POINTER_TYPE_P (TREE_TYPE (exp)))
	    {
	      type0 = TREE_TYPE (exp);
	      type1 = NULL_TREE;
	    }
	*/

	/*
	adding two pointers makes no sense
	-mtc 4/17/2007
	*/
	gcc_assert(!ptr0 || !ptr1);
	
	  return expand_add_or_sub_pointer (target, op0, type0, op1, type1,
					    add);
	}
      break;

    case PREINCREMENT_EXPR:
    case PREDECREMENT_EXPR:
    case POSTINCREMENT_EXPR:
    case POSTDECREMENT_EXPR:
      if (ptr0)
	return expand_add_or_sub_pointer (target, op0, type0, op1, type1, 1);
      break;

    default:
      break;
    }

/* Before calling expand_binop() check to see if it is likely to need to do some mode conversions and
    if so and there's a sign/unsigned mismatch do them now so we get sign/zero extend correct
    -mtc 4/9/2010
*/
  if ((mode0 != (int) mode) && (unsignedp0 != unsignedp))
  	op0 = convert_modes(mode, mode0, op0, unsignedp0);
  if ((mode1 != (int) mode) && (unsignedp1 != unsignedp))
  	op1 = convert_modes(mode, mode1, op1, unsignedp1);

  return expand_binop (mode, binoptab, op0, op1, target, unsignedp, methods);
}

#define emit_TLZ(X, Y, Z) \
  emit_insn (gen_andsi3 (X, Y, GEN_INT (trunc_int_for_mode (~(HWINT (Z) << 18), SImode))))
#define emit_TLO(X, Y, Z) \
  emit_insn (gen_iorsi3 (X, Y, GEN_INT (trunc_int_for_mode (HWINT (Z) << 18, SImode))))
#define emit_TLC(X, Y, Z) \
  emit_insn (gen_xorsi3 (X, Y, GEN_INT (trunc_int_for_mode (HWINT (Z) << 18, SImode))))
#define emit_ADD(X, Y, Z) \
  emit_insn (gen_addsi3 (X, Y, GEN_INT (trunc_int_for_mode (HWINT (Z) << 18, SImode))))
#define emit_SUB(X, Y, Z) \
  emit_insn (gen_subsi3 (X, Y, GEN_INT (trunc_int_for_mode (HWINT (Z) << 18, SImode))))
#define emit_TLNE(X, Y, L) \
  emit_jump_insn (gen_test_and_skip (GEN_EQ, X, GEN_INT (trunc_int_for_mode (HWINT (Y)<<18, SImode)), L))
#define emit_TLNN(X, Y, L) \
  emit_jump_insn (gen_test_and_skip (GEN_NE, X, GEN_INT (trunc_int_for_mode (HWINT (Y)<<18, SImode)), L))
#define emit_TLZN(X, Y, L) \
  emit_jump_insn (gen_TLZN (X, GEN_INT (trunc_int_for_mode (HWINT (Y) << 18, SImode)), L))


/* Convert a void pointer in OP0 to a global pointer with byte size TO.
    TO must be a valid pointer reference size or we may abort or generate bad code.
    6,7,8,9,18, or 36
    Return the result. 
    NEWVOID*
    -mtc 11/12/2010
   */
rtx
convert_void_pointer (rtx op0, int to, int not_null)
{
	enum machine_mode newmode = ptr_mode_for_size(to);
	rtx newop = gen_reg_rtx(newmode);
	rtx newop_subreg = simplify_gen_subreg(Pmode, newop, GET_MODE(newop), 0);

	/* op0 needs to be in a reg for the branch and TLNN code to work */
	/* (void *) should be Pmode or a constant compatible with Pmode */
	op0 = copy_to_mode_reg(Pmode, op0);
	
	if (to == 36)
		{
		emit_TLZ (op0, op0, 0770000);
		}
	else
		{
		rtx label = gen_label_rtx ();
		rtx label2 = gen_label_rtx ();
		int psmask = pdp10_ps_base_for_bytesize(to) << 12;

		/* check for NULL unless pointer is guaranteed not_null */
		if (!not_null)
			emit_jump_insn (gen_cbranchsi (label, op0, GEN_INT (0), GEN_EQ));

		/* basic conversion -- retain any existing PS value, otherwise insert mask for TO */
		emit_TLNN (op0, 0770000, label2);
		emit_TLO  (op0, op0, psmask);
		emit_label (label2);

		if (!not_null)
			emit_label (label);
		}
	
	/* get the pointer into a reg rtx with the correct result mode */
	emit_move_insn (newop_subreg, op0);

	return newop;
}



/* Convert a global pointer in OP0 with byte size FROM to a global
   pointer with byte size TO.  Byte size 36 means word pointer.
   Return the result.  */
/*	Need to go through call the cases and make sure arithmetic is done on
	a proper arithmetic mode, even though op0 and the result should be in
	the proper mode for the pointer size.  Should handle generically enough
	to accomodate a pointer mode being the same as the arithmetic mode.
*/
rtx
convert_global_pointer (rtx op0, int from, int to, int byte)
{
  rtx newop;
  rtx label, label2;
  rtx newop_subreg, op0_subreg;
  enum machine_mode oldmode = ptr_mode_for_size(from);
  enum machine_mode newmode = ptr_mode_for_size(to);
  
  /* tolerate conversion from CONST_INT
      -mtc 11/27/2007
  */
  if (oldmode != GET_MODE(op0) && GET_MODE(op0) != VOIDmode)
  	abort();
  
  if (oldmode == newmode)
  	newop = op0;
  else
  	newop = gen_reg_rtx(newmode);

  switch (to)
    {
    case 6:
      switch (from)
	{
	case 7:
	  emit_SUB (newop, op0, 0140000);
	  break;
	case 8:
	  emit_SUB (newop, op0, 0070000);
	  break;
	case 9:
	  emit_SUB (newop, op0, 0220000);
	  break;
	case 36:
	  emit_TLO (newop, op0, 0460000 + byte * 0010000);
	  break;
	default:
	  abort ();
	}
      break;

    case 7:
      switch (from)
	{
	case 8:
	  emit_ADD (newop, op0, 0050000);
	  break;
	case 9:
	  emit_SUB (newop, op0, 0060000);
	  break;
	case 36:
	  emit_TLO (newop, op0, 0620000 + byte * 0010000);
	  break;
	default:
	  abort ();
	}
      break;

    case 8:
      switch (from)
	{
	case 9:
	  emit_SUB (simplify_gen_subreg(Pmode, newop, GET_MODE(newop), 0), simplify_gen_subreg(Pmode, op0, GET_MODE(op0), 0),  0130000);
	  break;
	case 18:
	  /* Questionable: we're converting a half word pointer to an 8 bit pointer by pointing to the closes location its possible for a 8-bit OWGBP 
	      to point to, even though it might be outside of the location pointed to by the original pointer 
	      -mtc 2/26/2008 */
	  emit_TLO (simplify_gen_subreg(Pmode, newop, GET_MODE(newop), 0), simplify_gen_subreg(Pmode, op0, GET_MODE(op0), 0), 0010000);
	  emit_TLZ (simplify_gen_subreg(Pmode, newop, GET_MODE(newop), 0), simplify_gen_subreg(Pmode, newop, GET_MODE(newop), 0), 0200000);
	  break;
	case 36:
	  if (!reg_or_mem_operand(op0, Pmode))
	  	op0 = force_reg(Pmode, op0);
	  emit_TLO (simplify_gen_subreg(Pmode, newop, GET_MODE(newop), 0), op0, 0550000 + byte * 0010000);
	  break;
	default:
	  abort ();
	}
      break;

    case 9:
      switch (from)
	{
	case 7:
	  emit_ADD (newop, op0, 0060000);
	  break;
	case 8:
	  emit_ADD (simplify_gen_subreg(Pmode, newop, GET_MODE(newop), 0), simplify_gen_subreg(Pmode, op0, GET_MODE(op0), 0), 0130000);
	  break;
	case 18:
	  emit_TLZ (simplify_gen_subreg(Pmode, newop, GET_MODE(newop), 0), simplify_gen_subreg(Pmode, op0, GET_MODE(op0), 0), 0050000);
	  break;
	case 36:
	  if (!reg_or_mem_operand(op0, Pmode))
	  	op0 = force_reg(Pmode, op0);
	  emit_TLO (simplify_gen_subreg(Pmode, newop, GET_MODE(newop), 0), op0, 0700000 + byte * 0010000);
	  break;
	default:
	  abort ();
	}
      break;

    case 18:
      switch (from)
	{
	case 8:
	  op0_subreg = simplify_gen_subreg(Pmode, op0, GET_MODE(op0), 0);
	  newop_subreg = simplify_gen_subreg(Pmode, newop, GET_MODE(newop), 0);
	  emit_ADD (newop_subreg, op0_subreg, 0170000);
	 /*emit_insn (gen_TLNE_TLZA_TLO (newop, GEN_INT (0020000),
					GEN_INT (0010000), GEN_INT (0010000)));*/
	  emit_TLNE(newop_subreg, 0020000, label = gen_label_rtx());
	  emit_TLZ(newop_subreg, newop_subreg, 0010000);
	  emit_jump(label2 = gen_label_rtx());
	  emit_label(label);
	  emit_TLO(newop_subreg, newop_subreg, 0010000);
	  emit_label(label2);
	  break;
	case 9:
	  op0_subreg = simplify_gen_subreg(Pmode, op0, GET_MODE(op0), 0);
	  newop_subreg = simplify_gen_subreg(Pmode, newop, GET_MODE(newop), 0);
	  emit_TLO  (newop_subreg, op0_subreg, 0050000);
	  emit_TLNE (newop_subreg, 0020000, label = gen_label_rtx ());
	  emit_TLZ  (newop_subreg, newop_subreg, 0010000);
	  emit_label (label);
	  break;
	case 36:
	  emit_TLO (simplify_gen_subreg(oldmode, newop, GET_MODE(newop), 0), op0, 0750000 + byte * 0010000);
	  break;
	default:
	  abort ();
	}
      break;

    case 36:
	/* avoid spurious 36 -> 36 bit pointer conversion
	    -mtc 8/16/2007
	*/
      if (from != 36)
        emit_TLZ (newop, simplify_gen_subreg(newmode, op0, GET_MODE(op0), 0), 0770000);
      break;

    default:
      abort ();
    }

  return newop;
}

/* Convert a local pointer in OP0 with byte size FROM to a local
   pointer with byte size TO.  Byte size 36 means word pointer.
   Return the result.  */
static rtx
convert_local_pointer (rtx op0, int from, int to, int byte)
{
  rtx reg = gen_reg_rtx (Pmode);
  rtx label;
  rtx temp;

  switch (to)
    {
    case 6:
      switch (from)
	{
	case 7:
	  emit_TLZ  (reg, op0, 0010100);
	  emit_ADD  (reg, reg, 0020000);
	  emit_TLNN (reg, 0160000, label = gen_label_rtx ());
	  emit_TLO  (reg, reg, 0020000);
	  emit_label (label);
	  emit_TLNN (reg, 0200000, label = gen_label_rtx ());
	  emit_TLC  (reg, reg, 0060000);
	  emit_label (label);
	  break;
	case 8:
	  emit_TLC  (reg, op0, 0001600);
	  emit_TLZ  (reg, reg, 0060000);
	  emit_TLNE (reg, 0200000, label = gen_label_rtx ());
	  emit_TLO  (reg, reg, 0020000);
	  emit_label (label);
	  emit_TLNE (reg, 0100000, label = gen_label_rtx ());
	  emit_TLO  (reg, reg, 0040000);
	  emit_label (label);
	  break;
	case 9:
	  emit_TLC  (reg, op0, 0041700);
	  emit_TLZN (reg, 0010000, label = gen_label_rtx ());
	  emit_TLO  (reg, reg, 0040000);
	  emit_label (label);
	  break;
	case 36:
	  emit_TLO (reg, op0, 0360600 - byte * 0060000);
	  break;
	default:
	  abort ();
	}
      break;

    case 7:
      switch (from)
	{
	case 8:
	  emit_TLC (reg, op0, 0001700);
	  emit_TLZ (reg, op0, 0070000);
	  temp = gen_reg_rtx (Pmode);
	  emit_move_insn (temp, reg);
	  emit_insn (gen_rotlsi3 (temp, temp, GEN_INT (3)));
	  emit_insn (gen_ANDCx (temp, temp, GEN_INT (7)));
	  emit_insn (gen_addsi3 (temp, temp, GEN_INT (1)));
	  emit_insn (gen_rotrsi3 (temp, temp, GEN_INT (6)));
	  emit_insn (gen_iorsi3 (reg, reg, temp));
	  break;
	case 9:
	  emit_TLC (reg, op0, 0071600);
	  emit_ADD (reg, reg, 0010000);
	  break;
	case 36:
	  emit_TLO (reg, op0, 0350700 - byte * 0070000);
	  break;
	default:
	  abort ();
	}
      break;

    case 8:
      switch (from)
	{
	case 9:
	  emit_TLO (reg, op0, 0040000);
	  emit_TLZ (reg, reg, 0030100);
	  break;
	case 18:
	  emit_TLO (reg, op0, 0141000);
	  emit_TLZ (reg, reg, 0022200);
	  break;
	case 36:
	  emit_TLO (reg, op0, 0341000 - byte * 0100000);
	  break;
	default:
	  abort ();
	}
      break;

    case 9:
      switch (from)
	{
	case 8:
	  emit_TLC  (reg, op0, 0040100);
	  emit_TLNE (reg, 0100000, label = gen_label_rtx ());
	  emit_TLO  (reg, reg, 0010000);
	  emit_label (label);
	  emit_TLNE (reg, 0200000, label = gen_label_rtx ());
	  emit_TLO  (reg, reg, 0020000);
	  emit_label (label);
	  break;
	case 18:
	  emit_TLC (reg, op0, 0113300);
	  break;
	case 36:
	  emit_TLO (reg, op0, 0331100 - byte * 0110000);
	  break;
	default:
	  abort ();
	}
      break;

    case 18:
      switch (from)
	{
	case 8:
	  emit_TLZ  (reg, op0, 0141000);
	  emit_TLO  (reg, reg, 0002200);
	  emit_TLNE (reg, 0200000, label = gen_label_rtx ());
	  emit_TLO  (reg, reg, 0020000);
	  emit_label (label);
	  break;
	case 9:
	  emit_TLC (reg, op0, 0003300);
	  emit_TLZ (reg, reg, 0110000);
	  break;
	case 36:
	  emit_TLO (reg, op0, 0222200 - byte * 0220000);
	  break;
	default:
	  abort ();
	}
      break;

    case 36:
      emit_TLZ (reg, op0, 0777777);
      break;

    default:
      abort ();
    }

  return reg;
}

#if XKL_STUFF
static void
maybe_warn_about_pointer_conversion (tree exp, int from, int to)
{
/* Treat any conversion between different sub-word sizes as an error.
    This make most of the subsequent code redundant, but leave it as is for now.
    -mtc 12/21/2009
*/
  if (from != to && from != 36 && to !=36)
  	{
	warning (0, "converting %d-bit byte pointer to %d-bit byte pointer ", from, to);
  	return;
	}

/* Don't bother warning about conversion from a void pointer.  I'm not sure
    how a void pointer can also be a 8 or 9 bit size item pointer, but that's what
    the original test here was for.  Something very confused in our understanding
    of what pointers are, no doubt.  For now, just properly protect the test against
    dereferencing a null on the way to making the final test(s). */
  if ( (exp == NULL) ||
	(TREE_CODE(exp) == VAR_DECL) || 	/* this will need expanding as test cases appear */
	(TREE_TYPE(exp) == NULL) ||
	(TREE_OPERAND(exp, 0) == NULL) ||
	(TREE_TYPE(TREE_OPERAND(exp, 0)) == NULL) ||
	(TREE_TYPE(TREE_TYPE(TREE_OPERAND(exp, 0))) == NULL) ||
	(!POINTER_TYPE_P(TREE_TYPE(exp))) ||
	(!POINTER_TYPE_P(TREE_TYPE(TREE_OPERAND(exp, 0)))) ||
	(VOID_TYPE_P(TREE_TYPE(TREE_TYPE(TREE_OPERAND(exp, 0)))))
      )
    return;

  /* We used to try to avoid redundant warnings by checking the global lineno, but with 4.1.1 upgrade
      that has disappeared.  Instead source_lineno and source_filename exist, but it's not clear what to check
      within the source_filename struct to see if we're in the same file, and anyhow the mechanism is incorrect
      since how far the parser has advanced may have nothing to do with what we are reporting on.
      -mtc 4/6/2007
  */
  if (from <= 18 && to > from && to >= 18)
    {
      if (to == 36)
	warning (0, "converting %d-bit byte pointer to word address", from);
      else
	warning (0, "converting %d-bit byte pointer to %d-bit byte pointer",
		 from, to);
    }

}
#else
#define maybe_warn_about_pointer_conversion(EXP, FROM, TO)
#endif

static rtx
convert_pointer (tree exp, rtx op0, rtx target ATTRIBUTE_UNUSED, int from, int to, int stack, int not_null)
{
  rtx old;
  enum machine_mode oldmode;
  enum machine_mode newmode = ptr_mode_for_size(to);
  int emit_jump;
  rtx label = gen_label_rtx ();
  rtx label2;
  int fromvoidpointer = (exp && IS_EXPR_CODE_CLASS(TREE_CODE_CLASS(TREE_CODE(exp))) &&
  						POINTER_TYPE_P(TREE_TYPE(TREE_OPERAND(exp, 0))) && 
						VOID_TYPE_P(TREE_TYPE(TREE_TYPE(TREE_OPERAND(exp, 0)))));
  int tovoidpointer = (exp && POINTER_TYPE_P(TREE_TYPE(exp)) && VOID_TYPE_P(TREE_TYPE(TREE_TYPE(exp))));
  int voidconversion = tovoidpointer ||fromvoidpointer;

/* Replace parameter verification test to more clearly test for what constitutes correct from and to values
     -mtc 11/17/2010
*/
  if ((from != 6 && from != 7 && from != 8 && from != 9 && from != 18 && from != 36) ||
	(to != 6 && to != 7 && to != 8 && to != 9 && to != 18 && to != 36))
  	abort();

  if (!op0)
    {
    /* Try to optimize the special case where we're converting an address of something into a different pointer type.
    	In that case prefer to just get the address and convert only once.
    	In either case, reset from to be whatever the type of pointer we got back points to.
    	-mtc 7/20/2006 
    */
    if ((TREE_CODE(exp) == NOP_EXPR || TREE_CODE(exp) == CONVERT_EXPR) && TREE_CODE(TREE_OPERAND(exp, 0)) == ADDR_EXPR)
    	op0 = expand_expr(TREE_OPERAND(exp, 0), NULL_RTX, Pmode, 0);
    else
	op0 = expand_expr (TREE_OPERAND (exp, 0), NULL_RTX, pdp10_mode_for_type(TREE_TYPE(TREE_OPERAND(exp, 0))), 0);
    from = ptr_mode_target_size(GET_MODE(op0));
    }

/* If op0 is a constant, it won't have a mode, which is a bad thing when we call emit_move_insn.  Assume the appropriate mode
is what matches the from size.  Eventually we probably want to special case constants and create the correct pointer value
directly.  Otherwise, we may end up doing a lot of run-time manipulations of constant values.
-mtc 8/1/2006
Rearrange the order slightly so we can use this oldmode calculation for both conversion to voidpointer and force_reg() call
-mtc 11/18/2010
*/
  oldmode = GET_MODE(op0);
  if (!oldmode)
  	oldmode = ptr_mode_for_size(from);

/* void conversion can be active, test for this first because void and word pointers will both have size 36 */
  if (voidconversion)
  	{
  	/* from void* to void* is a NOP */
	if (fromvoidpointer && tovoidpointer)
		return op0;

	if (fromvoidpointer)
		return convert_void_pointer(op0, to, not_null);

	/* converting to voidpointer */
  	return simplify_gen_subreg (newmode, op0, oldmode, 0);
  	}

if (from == to)
    return op0;

/* Use force_reg instead of calling emit_move_insn() directly.  This avoids unnecessary creation of regs and permits some
optimizations by marking the register equivalent to a constant source.
-mtc 11/18/2010
*/
  op0 = force_reg(oldmode, op0);


/* If the pointer conversions return anything other than the op0 passed to them, we need
    to do some shuffling to get the final value into the same rtx for the non-conversion case
    -mtc 7/24/2007
*/
  old = op0;

  emit_jump = !(not_null
		|| to == 36
		|| (to == 9 && from == 18 && TARGET_EXTENDED));
  if (emit_jump)
    emit_jump_insn (gen_cbranchsi (label, op0, GEN_INT (0), GEN_EQ));

  maybe_warn_about_pointer_conversion (exp, from, to);

  op0 = convert_global_pointer (op0, from, to, 0);
    

  if (emit_jump && old != op0)
  	{
  	label2 = gen_label_rtx ();
	emit_jump_insn (gen_jump (label2));
	emit_barrier();
  	}

  if (emit_jump)
    emit_label (label);

  if (emit_jump && old != op0)
	{
	emit_move_insn (op0, ((GET_MODE(op0) == GET_MODE(old)) ? old : simplify_gen_subreg (GET_MODE(op0), old, GET_MODE(old), 0)));
	emit_label (label2);
	}

  return op0;
}

/* Emit instructions to perform the pointer conversion in EXP.  Store
   the result in TARGET if convenient, and return the result.  If OP0
   is not NULL_RTX, it's RTL for the unconverted pointer.  */
rtx
pdp10_convert_pointer (tree exp, rtx op0, rtx target ATTRIBUTE_UNUSED, int mod, int stack, int not_null)
{
  enum expand_modifier modifier = mod;
  int from, to;

  if (stack)
    not_null = 1;

  if (modifier == EXPAND_INITIALIZER)
    return NULL_RTX;

  if (!(POINTER_TYPE_P (TREE_TYPE (exp))
	 && (stack || POINTER_TYPE_P (TREE_TYPE (TREE_OPERAND (exp, 0))))))
    return NULL_RTX;

  to = ptr_mode_target_size (TYPE_MODE (TREE_TYPE (exp)));
  from = ptr_mode_target_size (TYPE_MODE (TREE_TYPE (TREE_OPERAND (exp, 0))));

  return convert_pointer (exp, op0, target, from, to, stack, not_null);
}

/* Emit instructions to convert the pointer in EXP or OP0 (at least
   one must be non-NULL) from a pointer to bytes of FROM bits to a
   pointer to bytes of TO bits.  Store the result in TARGET if
   convenient, and return the result.  */
rtx
pdp10_convert_ptr (tree exp, rtx op0, rtx target ATTRIBUTE_UNUSED, int from, int to, int not_null)
{
  int stack;
  return convert_pointer (exp, op0, target, from, to, stack=0, not_null);
}

/* Return the appropriate mode for a pointer to an object of size bitsize
     */
enum machine_mode
ptr_mode_for_size (unsigned int bitsize)
{
	switch (bitsize)
		{
		case BITS_PER_WORD:
			/* eventually this should be WPmode
			    -mtc 7/18/2006
			*/
			return SImode;
		case BITS_PER_WORD / 2:
			return HPmode;
		case BITS_PER_UNIT:
			return Q9Pmode;
		case 8:
			return Q8Pmode;
		case 7:
			return Q7Pmode;
		case 6:
			return Q6Pmode;
		/* How to handle (VOID *) ? */
		/* For now use SImode, which is what all pointers were. */
		/* If there are problems, we should consider introducing VPmode */
		case 0:
			return SImode;
		default:
			if (bitsize > BITS_PER_WORD / 2)
				/* eventually this should be WPmode
				    -mtc 7/18/2006
				*/
				return SImode;
			if (bitsize > BITS_PER_UNIT)
				return HPmode;
			/* Q6Pmode might make more sense, but that's an unusual mode
			     so for now let's return the default byte pointer mode
			*/
			return Q9Pmode;
		}
}

/* Return the appropriate mode for a pointer to an object of MODE
    Mostly this depends on the size of the mode, but special modes that have
    no setting for their size need special handling.

    Fix handling of sub-unit size modes
    - mtc 8/22/2007
     */
enum machine_mode
ptr_mode_for_mode (enum machine_mode mode)
{
	int size;

	switch (mode)
		{
		case VOIDmode:
			size = 0;
			break;
			
		case BLKmode:
			size = BITS_PER_WORD;
			break;
			
		default:
			size = GET_MODE_BITSIZE(mode);
			if (size <= BITS_PER_UNIT)
				size = GET_MODE_PRECISION(mode);
			break;
		}

	return ptr_mode_for_size(size);
}

/* Return the appropriate mode for a pointer to an object of TYPE
    The pointer mode depends primarily on the size of the object pointed to
    but pointers to void are special.
    Currently we treat pointer to void if its a pointer to a char.
    RECORD_TYPE, ARRAY_TYPE, UNION_TYPE, etc
    are problematic since we can be asked for the mode of a
    pointer to them before their declaration is done.  For now, treat all these pointers
    as word pointers.
    Eventually may want more specific tests and base things on the alignment or the
    types of the items they contain.
    Abort for any other cases where the size of the thing pointed at is unknown
    so we can figure out what's going on.
     */
enum machine_mode
ptr_mode_for_type (tree type)
{
	tree treesize = TYPE_SIZE(type);
	unsigned int size = BITS_PER_UNIT;
	unsigned int align = TYPE_ALIGN(type);

	switch (TREE_CODE(type))
		{
		case VOID_TYPE:
			size = 0;
			break;

		case ARRAY_TYPE:
			/* Split ARRAY_TYPE out by itself.
			    char arrays seem to have align of 9 regardless of the size of the chars
			    Try instead to have the ptr mode for an array be a pointer to an array element.
			    It's not clear of the way I've done this will run afoul of incomplete array types, but we'll find out.
			    -mtc 6/11/2008
			*/
			return ptr_mode_for_mode(TYPE_MODE(TREE_TYPE(type)));

		case RECORD_TYPE:
		case UNION_TYPE:
		case ENUMERAL_TYPE:
			/*size = BITS_PER_WORD;*/


			/* case when declaration is incomplete probably needs more work
			    can we count on align being set properly?  
			    -mtc 7/16/2007
			*/
			if (treesize && host_integerp(treesize, 1))
				size = tree_low_cst(treesize, 1);
			else
				size = BITS_PER_WORD;

			/* unaligned arrays can only be referenced by pointers that can reference the alignment boundary */
			/* limit this adjustment to arrays, since there are issues with incomplete record types.
			    checking if TYPE_MODE(type) == VOID is another possibility to investigate
			    - mtc 7/23/2007
			*/
			if ((align < size) && (TREE_CODE(type) == ARRAY_TYPE))
				size = align;

			break;

		default:
			/* sub-unit char types will have a TYPE_SIZE of 9, so we need to look at the precision instead
			    -mtc 9/19/2007
			*/
			if (treesize && host_integerp(treesize, 1))
				{
				size = tree_low_cst(treesize, 1);
				if (size == BITS_PER_UNIT)
					size = TYPE_PRECISION(type);
				}
			else
				abort();
			break;
		}

	return ptr_mode_for_size(size);
}

/* Return the appropriate machine mode for an object of TYPE
    Normally this is just TYPE_MODE(type), but the PDP10 needs a bunch
    of different QI modes for the various small pointer modes to reference.
    We may eventually change how types are set so that TYPE_MODE(type)
    returns the correct value, but to avoid disrupting too much at once, for now
    call this routine where it seems necessary.
     */
enum machine_mode
pdp10_mode_for_type (tree type)
{
	enum machine_mode mode = TYPE_MODE(type);
	tree treesize = TYPE_SIZE(type);
	int size = BITS_PER_UNIT;

	if (mode != QImode)
		return mode;

	if (treesize && host_integerp(treesize, 1))
		{
		size = tree_low_cst(treesize, 1);
		/* for now only substitute a QnI mode when the size is strictly less than a byte
		    and big enough to fill one of the QnI modes */
		if ((6 <= size) && (size < BITS_PER_UNIT))
			mode = really_smallest_int_mode_for_size(size);
		}
	else
		abort();

	return mode;
	
}

/* Return the bitsize of an object pointed to by a pointer of mode MODE
     */
int
ptr_mode_target_size (enum machine_mode mode)
{
	switch(mode)
		{
		case Q6Pmode:
			return 6;
		case Q7Pmode:
			return 7;
		case Q8Pmode:
			return 8;
		case Q9Pmode:
			return BITS_PER_UNIT;
		case HPmode:
			return BITS_PER_UNIT * 2;
		case WPmode:
		/* SImode is used as a pointer mode for historical reasons, even though it is also an integer mode */
		case SImode:
			return BITS_PER_WORD;
		/* No other modes are pointer modes, although we might add a VPmode for VOID pointers  */
		/* VOID pointers should have a target size of 0 since they shouldn't ever be dereferenced */
		default:
			abort();
		/* Explicit cast of a constant can cause a constant expression, which has mode VOIDmode to */
		/* be treated as a pointer.  Treat such things as pointers to words.  */
		case VOIDmode:
			return BITS_PER_WORD;
		}
}

/* Return whether PTRMODE is suitable for referencing something of mode MODE
*/
int
ptr_mode_ok_for_mode(enum machine_mode ptrmode, enum machine_mode mode)
{
	/* if ptrmode is the standard mode for referencing mode its certainly fine */
	if (ptr_mode_for_mode(mode) == ptrmode)
		return 1;
	
	/* all the QnP modes are ok for referencing QImode */
	if (mode == QImode)
		return (ptr_mode_target_size(ptrmode) <= BITS_PER_UNIT);
	
	/* otherwise its a nogo */
	return 0;
}

/* Return the ps value used to create a pointer of mode MODE
     */
int
ps_base_for_ptr_mode (enum machine_mode mode)
{
	return pdp10_ps_base_for_bytesize (ptr_mode_target_size(mode));
}

/* Adjust the 12-bit P and S field (or PS code) in PS by *ADJUST
   bytes.  Return the new P and S field (or -1 on failure), and the
   store the number of words to adjust a word pointer in *ADJUST.  */
static int
adjust_ps (int ps, int *adjust)
{
  int s, n;

  if (ps >= 04600)
    {
      int base, oldps;

      if (ps & 077)
	return -1;

      ps = ps >> 6;
      s = bytesize_for_ps(ps);
      base = pdp10_ps_base_for_bytesize(s);

      n = BITS_PER_WORD / s;
      oldps = ps - base;
      ps = (*adjust + oldps) % n;
      if (ps < 0)
	{
	  ps += n;
	  *adjust -= n;
	}
      else if (ps < oldps)
	{
	  *adjust += n;
	}
      ps += base;
      ps <<= 6;
    }
  else
    {
      int p = ps >> 6;
      s = ps & 077;

      if (!VALID_OWGBP_BYTE_SIZE_P (s)
	  || p % s != 0)
	return -1;

      n = BITS_PER_WORD / s;
      p -= s * (*adjust % n);
      if (p < 0)
	p += s;
      ps = (p << 6) + s;
    }

  *adjust /= n;
  return ps;
}

rtx
pdp10_simplify_unspec (rtx x, rtx insn)
{
  switch (XINT (x, 1))
    {
    case UNSPEC_ADJBP:
      {
	HOST_WIDE_INT offset;
	rtx pointer = XVECEXP (x, 0, 0);
	rtx address = NULL_RTX;
	rtx adj = XVECEXP (x, 0, 1);
	int ps, adjust;

	/* If the pointer adjustment is a register, find the last
	   value that register was set to.  */
	if (GET_CODE (adj) == REG)
	  {
	    rtx i = insn;
	    if (insn == NULL_RTX)
	      break;
	    adj = pdp10_find_last_value (adj, &i);
	  }
	if (adj == NULL_RTX || GET_CODE (adj) != CONST_INT)
	  break;

	adjust = INTVAL (adj);
	if (adjust == 0)
	  return pointer;

	/* If the pointer to be adjusted is a register, find the last
	   value that register was set to.  */
	if (GET_CODE (pointer) == REG)
	  {
	    if (insn == NULL_RTX)
	      break;
	    pointer = pdp10_find_last_value (pointer, &insn);
	    if (pointer == NULL_RTX)
	      break;
	  }

	if (GET_CODE (pointer) == CONST)
	  pointer = XEXP (pointer, 0);

	/* The pointer should look like (plus ptr (const_int n)).
	   Extract the PS field and offset from n.  */
	if (GET_CODE (pointer) != PLUS
	    || GET_CODE (XEXP (pointer, 1)) != CONST_INT)
	  break;
	offset = INTVAL (XEXP (pointer, 1));
	ps = (offset >> 24) & (TARGET_EXTENDED ? 07700 : 07777);
	offset &= ADDRESS_MASK;
	pointer = XEXP (pointer, 0);

	if (offset & ((HOST_WIDE_INT)1 << 29))
	  {
	    ps += 0100;
	    offset ^= (HOST_WIDE_INT)1 << 29;
	    offset -= (HOST_WIDE_INT)1 << 29;
	  }

	/* If the pointer inside the plus expression is a register,
	   find the last value that register was set to.  */
	if (GET_CODE (pointer) == REG)
	  {
	    address = pdp10_find_last_value (pointer, &insn);
	    if (address == NULL_RTX
		|| GET_CODE (address) != UNSPEC
		|| XINT (address, 1) != UNSPEC_ADDRESS)
	      break;
	  }

	/* Adjust the PS field.  */
	ps = adjust_ps (ps, &adjust);
	if (ps == -1)
	  break;

	/* If there was a word offset, add it to the pointer.  */
	offset += adjust;
	/* offset &= ADDRESS_MASK; */
	if (offset)
	  pointer = plus_constant (pointer, offset);

	return plus_constant (pointer, 
			      trunc_int_for_mode ((HOST_WIDE_INT)ps << 24, Pmode));
      }
      break;
    }

  return NULL_RTX;
}

/* Similar to smallest_mode_for_size int() in stor-layout.c,  but specifically for MODE_INT
    and start at Q6Imode instead of QImode */
/* If we changed class_narrowest_mode[] to start at Q6Imode this might be unnecessary
    but that might have more sweeping consequences */
enum machine_mode
really_smallest_int_mode_for_size (unsigned int size)
{
  enum machine_mode mode;

  /* Get the first integer mode which has at least this size */
  for (mode = Q6Imode; mode != VOIDmode;
       mode = GET_MODE_WIDER_MODE (mode))
    if (GET_MODE_PRECISION (mode) >= size)
      return mode;

  abort ();
}



/* pdp10 customized implementation of va_arg: read the value from the
   current (padded) address and increment by the (padded) size.  */

tree
pdp10_gimplify_va_arg_expr (tree valist, tree type, tree *pre_p, tree *post_p)
{
  tree addr, t, type_size, rounded_size/*, valist_tmp*/;
  unsigned HOST_WIDE_INT align, boundary;
  bool indirect;

/* The issue of how va_arg is implemented on the PDP10 requires much more study
     We'll need to debug through here with more examples to figure out both the general
     and specific design.  This is copied and customizated from std_gimplify_va_arg_expr
     -mtc 5/2/2007
*/

  indirect = pass_by_reference (NULL, TYPE_MODE (type), type, false);
  if (indirect)
    type = build_pointer_type (type);

  align = PARM_BOUNDARY / BITS_PER_UNIT;
  boundary = FUNCTION_ARG_BOUNDARY (TYPE_MODE (type), type) / BITS_PER_UNIT;

  /* Hoist the valist value into a temporary for the moment.  */
  /* valist_tmp = get_initialized_tmp_var (valist, pre_p, NULL); */

  /* va_list pointer is aligned to PARM_BOUNDARY.  If argument actually
     requires greater alignment, we must perform dynamic alignment.  */
  if (boundary > align
      && !integer_zerop (TYPE_SIZE (type)))
    {
    	/* bit manipulation of pdp10 pointers is a bad idea */
	gcc_assert(false);
    }
  else
    boundary = align;

  /* If the actual alignment is less than the alignment of the type,
     adjust the type accordingly so that we don't assume strict alignment
     when deferencing the pointer.  */
  boundary *= BITS_PER_UNIT;
  if (boundary < TYPE_ALIGN (type))
    {
      type = build_variant_type_copy (type);
      TYPE_ALIGN (type) = boundary;
    }

  /* Compute the rounded size of the type.  */
  type_size = size_in_bytes (type);
  rounded_size = round_up (type_size, align);

  /* Reduce rounded_size so it's sharable with the postqueue.  */
  gimplify_expr (&rounded_size, pre_p, post_p, is_gimple_val, fb_rvalue);

  /* Get AP.  */
  addr = valist;
  #if 0
  if (PAD_VARARGS_DOWN && !integer_zerop (rounded_size))
    {
      /* Small args are padded downward.  */
      t = fold_build2 (GT_EXPR, sizetype, rounded_size, size_int (align));
      t = fold_build3 (COND_EXPR, sizetype, t, size_zero_node,
		       size_binop (MINUS_EXPR, rounded_size, type_size));
      t = size_binop (EXACT_DIV_EXPR, t, size_int (UNITS_PER_WORD));
      addr = fold_build2 (PLUS_EXPR, TREE_TYPE (addr), addr, t);
    }
  #endif

  /* Compute new value for AP.  */
  t = rounded_size;
  t = size_binop (EXACT_DIV_EXPR, t, size_int (UNITS_PER_WORD));
  t = fold_build1 (NEGATE_EXPR, TREE_TYPE(t), t);
  t = build2 (POINTER_PLUS_EXPR, TREE_TYPE (valist), valist, t);
  t = build2 (MODIFY_EXPR, TREE_TYPE (valist), valist, t);
  gimplify_and_add (t, pre_p);

  addr = fold_convert (build_pointer_type (type), addr);

  if (indirect)
    addr = build_va_arg_indirect_ref (addr);

  return build_va_arg_indirect_ref (addr);
}


