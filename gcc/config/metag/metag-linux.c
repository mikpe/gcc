/* Definitions of target machine for GNU compiler.
   Imagination Technologies Meta version.
   Copyright (C) 2008, 2013
   Imagination Technologies Ltd

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free
Software Foundation; either version 3, or (at your option) any later
version.

GCC is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING3.  If not see
<http://www.gnu.org/licenses/>.  */

#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "tm.h"
#include "rtl.h"
#include "tree.h"
#include "obstack.h"
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
#include "function.h"
#include "expr.h"
#include "optabs.h"
#include "toplev.h"
#include "recog.h"
#include "ggc.h"
#include "except.h"
#include "c-pragma.h"
#include "integrate.h"
#include "cfgloop.h"
#include "timevar.h"
#include "options.h"
#include "cgraph.h"
#include "target.h"
#include "tm_p.h"
#include "tm-constrs.h"
#include "target-def.h"
#include "langhooks.h"
#include "version.h"

/* --------------------------- begin target defines --------------------------*/
/* --------------------------- begin asm_out section -------------------------*/

#undef  TARGET_ASM_ALIGNED_DI_OP
#define TARGET_ASM_ALIGNED_DI_OP        "\t.quad\t"

#undef  TARGET_ASM_INTERNAL_LABEL
#define TARGET_ASM_INTERNAL_LABEL metag_internal_label

#undef  TARGET_ASM_FUNCTION_PROLOGUE
#define TARGET_ASM_FUNCTION_PROLOGUE metag_function_prologue

#undef  TARGET_ASM_FUNCTION_END_PROLOGUE
#define TARGET_ASM_FUNCTION_END_PROLOGUE metag_function_end_prologue

#undef  TARGET_ASM_FUNCTION_BEGIN_EPILOGUE
#define TARGET_ASM_FUNCTION_BEGIN_EPILOGUE metag_function_begin_epilogue

#undef  TARGET_ASM_FUNCTION_EPILOGUE
#define TARGET_ASM_FUNCTION_EPILOGUE metag_function_epilogue

static section * metag_bfd_select_section (tree, int, unsigned HOST_WIDE_INT);
#undef  TARGET_ASM_SELECT_SECTION
#define TARGET_ASM_SELECT_SECTION metag_bfd_select_section

static void metag_bfd_asm_unique_section (tree, int);
#undef TARGET_ASM_UNIQUE_SECTION
#define TARGET_ASM_UNIQUE_SECTION metag_bfd_asm_unique_section

#undef  TARGET_ASM_OUTPUT_MI_THUNK
#define TARGET_ASM_OUTPUT_MI_THUNK metag_output_mi_thunk

#undef  TARGET_ASM_CAN_OUTPUT_MI_THUNK
#define TARGET_ASM_CAN_OUTPUT_MI_THUNK metag_can_output_mi_thunk

/* A FILE comment and register declaration section must always
 *     begin the output.  */
static void metag_bfd_asm_file_start (void);
#undef TARGET_ASM_FILE_START
#define TARGET_ASM_FILE_START metag_bfd_asm_file_start

/* Tidies everything up at the end of the file.  */
static void metag_bfd_asm_file_end (void);
#undef TARGET_ASM_FILE_END
#define TARGET_ASM_FILE_END metag_bfd_asm_file_end

/* ---------------------------- end asm_out section --------------------------*/
/* ---------------------------- begin sched section --------------------------*/

#undef  TARGET_SCHED_ADJUST_COST
#define TARGET_SCHED_ADJUST_COST metag_sched_adjust_cost

/* ----------------------------- end sched section ---------------------------*/
/* ---------------------------- begin vector section -------------------------*/
/* ----------------------------- end vector section --------------------------*/
/* ------------------------------- begin section -----------------------------*/

#undef  TARGET_DEFAULT_TARGET_FLAGS
#ifdef MINIM_DEFAULT
#define TARGET_DEFAULT_TARGET_FLAGS     ((MASK_COND_EXEC) | (MASK_MINIM) | (MASK_MINIM_OPTIMISE) | (MASK_FLUSH_TO_ZERO))
#else
#define TARGET_DEFAULT_TARGET_FLAGS     ((MASK_COND_EXEC) | (MASK_MINIM_OPTIMISE) | (MASK_FLUSH_TO_ZERO))
#endif

#undef  TARGET_HANDLE_OPTION
#define TARGET_HANDLE_OPTION metag_handle_option

#undef  TARGET_MERGE_DECL_ATTRIBUTES
#define TARGET_MERGE_DECL_ATTRIBUTES metag_merge_decl_attributes

#undef  TARGET_MERGE_TYPE_ATTRIBUTES
#define TARGET_MERGE_TYPE_ATTRIBUTES metag_merge_type_attributes

#undef  TARGET_ATTRIBUTE_TABLE
#define TARGET_ATTRIBUTE_TABLE metag_attribute_table

#undef  TARGET_COMP_TYPE_ATTRIBUTES
#define TARGET_COMP_TYPE_ATTRIBUTES metag_comp_type_attributes

#undef  TARGET_INIT_BUILTINS
#define TARGET_INIT_BUILTINS  metag_init_builtins

#undef  TARGET_EXPAND_BUILTIN
#define TARGET_EXPAND_BUILTIN metag_expand_builtin

#undef  TARGET_FUNCTION_OK_FOR_SIBCALL
#define TARGET_FUNCTION_OK_FOR_SIBCALL metag_function_ok_for_sibcall

#undef  TARGET_ENCODE_SECTION_INFO
#define TARGET_ENCODE_SECTION_INFO metag_encode_section_info

static const char *metag_bfd_strip_name_encoding (const char *);
#undef  TARGET_STRIP_NAME_ENCODING
#define TARGET_STRIP_NAME_ENCODING metag_bfd_strip_name_encoding

#undef  TARGET_SCALAR_MODE_SUPPORTED_P
#define TARGET_SCALAR_MODE_SUPPORTED_P metag_scalar_mode_supported_p

#undef  TARGET_VECTOR_MODE_SUPPORTED_P
#define TARGET_VECTOR_MODE_SUPPORTED_P metag_vector_mode_supported_p

#undef  TARGET_RTX_COSTS
#define TARGET_RTX_COSTS metag_rtx_costs

#undef  TARGET_ADDRESS_COST
#define TARGET_ADDRESS_COST metag_address_cost

#undef  TARGET_MACHINE_DEPENDENT_REORG
#define TARGET_MACHINE_DEPENDENT_REORG metag_machine_dependent_reorg

#undef  TARGET_GIMPLIFY_VA_ARG_EXPR
#define TARGET_GIMPLIFY_VA_ARG_EXPR metag_gimplify_va_arg_expr

#undef  TARGET_INVALID_WITHIN_DOLOOP
#define TARGET_INVALID_WITHIN_DOLOOP metag_invalid_within_doloop

/* -------------------------------- end section ------------------------------*/
/* ---------------------------- begin calls section --------------------------*/

#undef  TARGET_PROMOTE_FUNCTION_ARGS
#define TARGET_PROMOTE_FUNCTION_ARGS hook_bool_tree_true

#undef  TARGET_PROMOTE_FUNCTION_RETURN
#define TARGET_PROMOTE_FUNCTION_RETURN hook_bool_tree_true

#undef  TARGET_FUNCTION_VALUE
#define TARGET_FUNCTION_VALUE metag_function_value

#undef  TARGET_PROMOTE_PROTOTYPES
#define TARGET_PROMOTE_PROTOTYPES hook_bool_tree_true

#undef  TARGET_PASS_BY_REFERENCE
#define TARGET_PASS_BY_REFERENCE metag_pass_by_reference

#undef  TARGET_SETUP_INCOMING_VARARGS
#define TARGET_SETUP_INCOMING_VARARGS metag_setup_incoming_varargs

#undef  TARGET_MUST_PASS_IN_STACK
#define TARGET_MUST_PASS_IN_STACK metag_must_pass_in_stack

#undef  TARGET_ARG_PARTIAL_BYTES
#define TARGET_ARG_PARTIAL_BYTES metag_arg_partial_bytes

/* ----------------------------- end calls section ---------------------------*/
/* ------------------------------- begin section -----------------------------*/

#undef  TARGET_SECONDARY_RELOAD
#define TARGET_SECONDARY_RELOAD metag_secondary_reload

/* -------------------------------- end section ------------------------------*/
/* ----------------------------- begin cxx section ---------------------------*/

/* C++ specific macros */

/* ------------------------------ end cxx section ----------------------------*/
/* ------------------------------- begin section -----------------------------*/

#undef TARGET_HAVE_NAMED_SECTIONS
#define TARGET_HAVE_NAMED_SECTIONS true

#undef  TARGET_HAVE_SWITCHABLE_BSS_SECTIONS
#define TARGET_HAVE_SWITCHABLE_BSS_SECTIONS false

/* -------------------------------- end section ------------------------------*/

#undef TARGET_CANNOT_FORCE_CONST_MEM
#define TARGET_CANNOT_FORCE_CONST_MEM metag_bfd_tls_referenced_p

struct gcc_target targetm = TARGET_INITIALIZER;

/* ---------------------------- end target defines ---------------------------*/

static void
metag_bfd_asm_file_start (void)
{
  fprintf (asm_out_file, "%s Generated by gcc %s for Meta(Linux)/elf\n",
           ASM_COMMENT_START, version_string);

  fputc ('\n', asm_out_file);
  output_file_directive (asm_out_file, main_input_filename);

  fputc ('\n', asm_out_file);
  META_IDENTIFY_CPU (asm_out_file);
  fputc ('\n', asm_out_file);
}

static void
metag_bfd_asm_file_end (void)
{
  return;
}

static const char *
metag_bfd_strip_name_encoding (const char *str)
{
  gcc_assert (str[0] != '&');

  return str + (str[0] == '*');
}

/* Construct a unique section name based on the decl name and the
   categorization performed above.  */

static void
metag_bfd_asm_unique_section (tree decl, int reloc)
{
  /* We only need to use .gnu.linkonce if we don't have COMDAT groups.  */
  bool one_only = DECL_ONE_ONLY (decl) && !HAVE_COMDAT_GROUP;
  const char *prefix, *name;
  size_t nlen, plen;
  char *string;

  switch (categorize_decl_for_section (decl, reloc))
    {
    case SECCAT_TEXT:
      prefix = one_only ? ".gnu.linkonce.t." : ".text.";
      break;
    case SECCAT_RODATA:
      prefix = one_only ? ".gnu.linkonce.r." : ".rodata.";
      break;
    case SECCAT_RODATA_MERGE_STR:
    case SECCAT_RODATA_MERGE_STR_INIT:
    case SECCAT_RODATA_MERGE_CONST:
      prefix = one_only ? ".gnu.linkonce.r." : ".rodata.";
      break;
    case SECCAT_SRODATA:
      prefix = one_only ? ".gnu.linkonce.s2." : ".sdata2.";
      break;
    case SECCAT_DATA:
      prefix = one_only ? ".gnu.linkonce.d." : ".data.";
      break;
    case SECCAT_DATA_REL:
      prefix = one_only ? ".gnu.linkonce.d.rel." : ".data.rel.";
      break;
    case SECCAT_DATA_REL_LOCAL:
      prefix = one_only ? ".gnu.linkonce.d.rel.local" : ".data.rel.local.";
      break;
    case SECCAT_DATA_REL_RO:
      prefix = one_only ? ".gnu.linkonce.d.rel.ro." : ".data.rel.ro.";
      break;
    case SECCAT_DATA_REL_RO_LOCAL:
      prefix = one_only ? ".gnu.linkonce.d.rel.ro.local." : ".data.rel.ro.local.";
      break;
    case SECCAT_SDATA:
      prefix = one_only ? ".gnu.linkonce.s." : ".sdata.";
      break;
    case SECCAT_BSS:
      prefix = one_only ? ".gnu.linkonce.d." : ".data.";
      break;
    case SECCAT_SBSS:
      prefix = one_only ? ".gnu.linkonce.s." : ".sdata.";
      break;
    /* Add default handlers to deal with tdata and tbss sections */
    case SECCAT_TDATA:
      prefix = one_only ? ".gnu.linkonce.td." : ".tdata.";
      break;
    case SECCAT_TBSS:
      prefix = one_only ? ".gnu.linkonce.tb." : ".tbss.";
      break;
    default:
      gcc_unreachable ();
  }
  plen = strlen (prefix);

  name = IDENTIFIER_POINTER (DECL_ASSEMBLER_NAME (decl));
  name = targetm.strip_name_encoding (name);
  nlen = strlen (name);

  string = alloca (nlen + plen + 1);
  memcpy (string, prefix, plen);
  memcpy (string + plen, name, nlen + 1);

  DECL_SECTION_NAME (decl) = build_string (nlen + plen, string);
}

static section *
metag_bfd_select_section (tree decl, int reloc, unsigned HOST_WIDE_INT align ATTRIBUTE_UNUSED)
{
  const char *sname;

  switch (categorize_decl_for_section (decl, reloc))
    {
    case SECCAT_TEXT:
      /* We're not supposed to be called on FUNCTION_DECLs.  */
      gcc_unreachable ();
    case SECCAT_RODATA:
      return readonly_data_section;
    case SECCAT_RODATA_MERGE_STR:
      return mergeable_string_section (decl, align, 0);
    case SECCAT_RODATA_MERGE_STR_INIT:
      return mergeable_string_section (DECL_INITIAL (decl), align, 0);
    case SECCAT_RODATA_MERGE_CONST:
      return mergeable_constant_section (DECL_MODE (decl), align, 0);
    case SECCAT_SRODATA:
      sname = ".sdata2";
      break;
    case SECCAT_DATA:
      return data_section;
    case SECCAT_DATA_REL:
      sname = ".data.rel";
      break;
    case SECCAT_DATA_REL_LOCAL:
      sname = ".data.rel.local";
      break;
    case SECCAT_DATA_REL_RO:
      sname = ".data.rel.ro";
      break;
    case SECCAT_DATA_REL_RO_LOCAL:
      sname = ".data.rel.ro.local";
      break;
    case SECCAT_SDATA:
      sname = ".sdata";
      break;
    /* Add default handler to deal with tdata sections */
    case SECCAT_TDATA:
      sname = ".tdata";
      break;
    case SECCAT_BSS:
      return data_section;
    case SECCAT_SBSS:
      return sdata_section;
    /* Add default handler to deal with tbss sections */
    case SECCAT_TBSS:
      sname = ".tbss";
      break;
    default:
      gcc_unreachable ();
    }
  if (!DECL_P (decl))
    decl = NULL_TREE;
  return get_named_section (decl, sname, reloc);
}

bool
metag_handle_option_per_os (size_t code ATTRIBUTE_UNUSED, const char *arg ATTRIBUTE_UNUSED, int value ATTRIBUTE_UNUSED)
{
  return true;
}

void
metag_override_options_per_os (void)
{
  if (TARGET_MTX)
    error ("Meta Linux does not have MTX support");

  if (metac_target == METAC_1_0_ID
      || metac_target == METAC_1_1_ID
      || metac_target == METAC_0_1_ID)
    error ("Meta Linux does not have support for the specified core");
}

/* Return 1 if X is a thread local symbol */

static int
metag_tls_symbol_ref_1 (rtx *x, void *data ATTRIBUTE_UNUSED)
{
  if (GET_CODE (*x) == SYMBOL_REF)
    return SYMBOL_REF_TLS_MODEL (*x) != 0;
  
  /* Don't recurse into UNSPEC_TLS looking for TLS symbols; these are
     TLS offsets, not real symbol references.  */
  if (GET_CODE (*x) == UNSPEC
      && XINT (*x, 1) >= UNSPEC_FIRST_TLS 
      && XINT (*x, 1) <= UNSPEC_LAST_TLS)
    return -1; 

  return 0;
}

/* Return 1 if X contains a thread-local symbol.  */

bool
metag_bfd_tls_referenced_p (rtx x)
{
  if (!METAG_HAVE_TLS)
    return false;

  return for_each_rtx (&x, &metag_tls_symbol_ref_1, 0);
}

bool
metag_function_ok_for_sibcall_per_os (tree fndecl ATTRIBUTE_UNUSED, tree exp ATTRIBUTE_UNUSED)
{
  return true;
}

void
metag_pad_function_call (rtx symbol_ref ATTRIBUTE_UNUSED)
{
  return;
}

bool
metag_tbiassert_p (rtx symbol_ref ATTRIBUTE_UNUSED)
{
  return false;
}

/* Construct the SYMBOL_REF for the __tls_get_addr function.  */

static GTY(()) rtx metag_tls_symbol;

static rtx
metag_tls_get_addr (void)
{
  if (!metag_tls_symbol)
    metag_tls_symbol = gen_rtx_SYMBOL_REF (Pmode, "__tls_get_addr");

  return metag_tls_symbol;
}

/* Construct the SYMBOL_REF for the __metag_load_tp function.  */

static GTY(()) rtx metag_tp_symbol;

static rtx
metag_load_tp (void)
{
  if (!metag_tp_symbol)
    metag_tp_symbol = gen_rtx_SYMBOL_REF (Pmode, "__metag_load_tp");

  return metag_tp_symbol;
}

/* Generates rtl to call the metag_load_tp function to get the address of the 
   thread pointer.  The rtx representing the register containing this address 
   is then returned.  */

static rtx
gen_metag_load_tp (void)
{
  rtx ret0, ret, tp, tmp, insn, eqv;
  ret0 = gen_rtx_REG (Pmode, D0Re0_REG);
  ret = gen_reg_rtx (Pmode);
  tp = gen_reg_rtx (Pmode);

  start_sequence ();
  tmp = gen_rtx_MEM (QImode, metag_load_tp ());
  insn = gen_call_value (ret0, tmp, const0_rtx);
  insn = emit_call_insn (insn);
  CONST_OR_PURE_CALL_P (insn) = 1;
  insn = get_insns ();
  end_sequence ();

  /* We need this equivalence expression so that the RTL optimiser
     can figure out it only needs to place one call to
     __metag_load_tp regardless of how many thread-local variables
     are present.  */
  eqv = gen_rtx_UNSPEC (VOIDmode, 
                        gen_rtvec (1, metag_load_tp ()),
                        UNSPEC_TLS); 
  emit_libcall_block (insn, tp, ret0, eqv);
  
  return tp;
}

/* Return the TLS type for TLS symbols, 0 otherwise.  */

bool
tls_symbolic_operand_p (rtx op)
{
  return ((GET_CODE (op) == SYMBOL_REF) && (SYMBOL_REF_TLS_MODEL (op) != 0));
}

/* These functions are part of a framework to allow the support of OS 
   specific builtin functions within GCC. 
   The only builtin function for META Linux is __builtin_thread_pointer
   which returns the address of the thread pointer. */

void
metag_init_builtins_per_os (void)
{
  tree nothrow = tree_cons (get_identifier ("nothrow"), NULL, NULL);
  tree ftmetag_tp = build_function_type (ptr_type_node, void_list_node);

  lang_hooks.builtin_function ("__builtin_thread_pointer",
                               ftmetag_tp,
                               METAG_BUILTIN_THREAD_POINTER,
                               BUILT_IN_MD,
                               NULL, 
                               nothrow);
}

/* Performs the expansion of the META Linux builtin functions */

rtx
metag_expand_builtin_per_os (tree exp, rtx target)
{
  tree fndecl = TREE_OPERAND (TREE_OPERAND (exp, 0), 0);
  int fcode   = DECL_FUNCTION_CODE (fndecl);

  switch(fcode)
    {
    case METAG_BUILTIN_THREAD_POINTER: /* void * __builtin_thread_pointer (void) */
      target = gen_metag_load_tp (); 
      return target;
    default:
      break;
    }

  return NULL_RTX;
}

/* Adds code to calculate the address of a TLS operand */

rtx
metag_bfd_legitimize_tls_address (rtx addr)
{
  rtx tmp, tga, ret, tp, arg1, insn, ret0, eqv;
  
  gcc_assert (!no_new_pseudos);
  gcc_assert (SYMBOL_REF_P(addr));

  switch (SYMBOL_REF_TLS_MODEL (addr))
    {
    case TLS_MODEL_GLOBAL_DYNAMIC:

      current_function_uses_pic_offset_table = 1;
      arg1 = gen_rtx_REG (Pmode, D1Ar1_REG);
      ret0 = gen_rtx_REG (Pmode, D0Re0_REG);
      ret = gen_reg_rtx (Pmode);

      start_sequence ();
      emit_insn (gen_tls_gd (arg1, pic_offset_table_rtx, addr));
      tga = gen_rtx_MEM (QImode, metag_tls_get_addr ());
      insn = gen_call_value (ret0, tga, const0_rtx);
      insn = emit_call_insn (insn);
      CONST_OR_PURE_CALL_P (insn) = 1;
      use_reg (&CALL_INSN_FUNCTION_USAGE (insn), arg1);
      insn = get_insns ();
      end_sequence ();

      emit_libcall_block (insn, ret, ret0, addr);

      return ret;

    case TLS_MODEL_LOCAL_DYNAMIC:

      current_function_uses_pic_offset_table = 1;
      arg1 = gen_rtx_REG (Pmode, D1Ar1_REG);
      ret0 = gen_rtx_REG (Pmode, D0Re0_REG);
      tmp = gen_reg_rtx (Pmode);
      ret = gen_reg_rtx (Pmode);

      start_sequence ();
      emit_insn (gen_tls_ldm (arg1, pic_offset_table_rtx, addr));
      tga = gen_rtx_MEM (QImode, metag_tls_get_addr ());
      insn = gen_call_value (ret0, tga, const0_rtx);
      insn = emit_call_insn (insn);
      CONST_OR_PURE_CALL_P (insn) = 1;
      use_reg (&CALL_INSN_FUNCTION_USAGE (insn), arg1);
      insn = get_insns ();
      end_sequence ();

      /* We need this equivalence expression so that the RTL optimiser
         can figure out it only needs to place one call to
         __tls_get_addr regardless of how many local-dynamic model
         variables are present.  */
      eqv = gen_rtx_UNSPEC (VOIDmode, 
                            gen_rtvec (1, metag_tls_get_addr ()),
                            UNSPEC_TLSLDM);

      emit_libcall_block (insn, tmp, ret0, eqv);

      insn = gen_tls_ldo (ret, tmp, addr);
      emit_insn (insn);

      return ret;

    case TLS_MODEL_INITIAL_EXEC:

      tmp = gen_reg_rtx (Pmode);

      current_function_uses_pic_offset_table = 1;
      emit_insn (gen_tls_ie (tmp, pic_offset_table_rtx, addr));

      tp = gen_metag_load_tp ();

      return gen_rtx_PLUS (Pmode, tp, tmp);

    case TLS_MODEL_LOCAL_EXEC:

      tmp = gen_reg_rtx (Pmode);
      tp = gen_metag_load_tp ();

      emit_insn (gen_tls_le (tmp, tp, addr));

      return tmp;

    default:
      gcc_unreachable ();
    }
}
