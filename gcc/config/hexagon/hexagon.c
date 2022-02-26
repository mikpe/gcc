
/* HEXAGON specific functions
   Copyright (C) 1998, 1999, 2000, 2001, 2002 Free Software Foundation, Inc.

   This file is part of GCC.

   GCC is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   GCC is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with GCC; see the file COPYING.  If not, write to
   the Free Software Foundation, 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

#include "config.h"

#include <string.h>
#include <assert.h>

#include "system.h"
#include "diagnostic-core.h"
#include "coretypes.h"
#include "tm.h"
#include "rtl.h"
#include "regs.h"
#include "hard-reg-set.h"
#include "basic-block.h"
#include "real.h"
#include "insn-config.h"
#include "conditions.h"
#include "insn-attr.h"
#include "flags.h"
#include "recog.h"
#include "tree.h"
#include "tree-pass.h"
#include "output.h"
#include "expr.h"
#include "obstack.h"
#include "except.h"
#include "function.h"
#include "toplev.h"
#include "options.h"
#include "tm_p.h"
#include "target.h"
#include "target-def.h"
#include "reload.h"
#include "optabs.h"
#include "ggc.h"
#include "integrate.h"
#include "debug.h"
#include "langhooks.h"
#include "df.h"
#include "sched-int.h"
#include "cfgloop.h"
#include "hw-doloop.h"
#include "params.h"
#include "gimple.h"




enum hexagon_architecture hexagon_arch = HEXAGON_ARCH_UNSPECIFIED;
enum hexagon_abi hexagon_abi = HEXAGON_ABI_UNSPECIFIED;
int hexagon_features;
enum hexagon_falign hexagon_falign = HEXAGON_NO_FALIGN;
bool hexagon_dual_memory_accesses = true;

/* Used for qdsp6 compatibility */
int hexagon_qdsp6_compat = 0;

static GTY(()) struct hexagon_packet_info *hexagon_head_packet;
static GTY(()) struct hexagon_packet_info *hexagon_tail_packet;
static GTY(()) struct hexagon_bb_aux_info *hexagon_head_bb_aux;




static bool hexagon_handle_option(size_t code, const char *arg, int value);

static struct machine_function *hexagon_init_machine_status(void);

static int hexagon_array_alignment(tree type, int align);
static bool hexagon_align_anon_bitfield(void);

static enum machine_mode hexagon_vectorize_preferred_simd_mode (
  enum machine_mode mode);

static bool hexagon_default_short_enums(void);

static bool hexagon_frame_pointer_required (void);
static bool hexagon_can_eliminate (const int, const int);

static reg_class_t hexagon_secondary_reload (bool in_p,
                                             rtx x,
                                             reg_class_t reload_class,
                                             enum machine_mode reload_mode,
                                             secondary_reload_info *sri);

static bool hexagon_save_register_p(unsigned int regno);
static void hexagon_make_prologue_epilogue_decisions(
              struct hexagon_frame_info *info);
struct hexagon_frame_info *hexagon_frame_info(void);
static void
compute_pipeline_state (struct hexagon_packet_info *packet, bool count_duplexes,
                        state_t* pipeline_state);

static bool hexagon_must_pass_in_stack(enum machine_mode mode, const_tree type);

static bool hexagon_vector_mode_supported_p(enum machine_mode mode);

static unsigned int hexagon_function_arg_boundary (enum machine_mode mode,
                                                   const_tree type);

static rtx hexagon_function_value(
             const_tree ret_type,
             const_tree fn_decl_or_type,
             bool outgoing);

static bool hexagon_return_in_memory(const_tree type, const_tree fntype);

static void hexagon_asm_function_prologue(FILE *file, HOST_WIDE_INT size);

static bool hexagon_function_ok_for_sibcall(tree decl, tree exp);

#if LINUX
void hexagon_setup_incoming_varargs (CUMULATIVE_ARGS *cum,
                                     enum machine_mode mode,
                                     tree type,
                                     int *pretend_size,
                                     int no_rtl ATTRIBUTE_UNUSED);
void hexagon_va_start (tree valist, rtx nextarg);
#endif

#if HEXAGON_DINKUMWARE || HEXAGON_NEWLIB
static void hexagon_init_libfuncs(void);
#endif /* HEXAGON_DINKUMWARE  || HEXAGON_NEWLIB */

static tree hexagon_vectorize_builtin_mask_for_load(void);

static bool hexagon_rtx_costs(
              rtx x,
              int code,
              int outer_code,
              int *total,
              bool speed);
bool hexagon_rtx_costs_debug(
       rtx x,
       int code,
       int outer_code,
       int *total,
       bool speed);
static int hexagon_address_cost(rtx address, bool speed);
int hexagon_address_cost_debug(rtx address, bool speed);

static int hexagon_sched_issue_rate(void);
static void hexagon_sched_dependencies_eval (rtx, rtx);
static int hexagon_sched_first_cycle_multipass_dfa_lookahead(void);

static void hexagon_load_pic_register (void);

static void hexagon_output_operand(FILE *f, rtx x, int code);

static void hexagon_fixup_cfg(void);
static void hexagon_machine_dependent_reorg(void);
static void hexagon_local_combine_pass(void);
static void hexagon_local_combine_immediate_pass(void);

static void hexagon_init_builtins(void);
static rtx expand_one_builtin(
             enum insn_code icode,
             rtx target,
             tree exp,
             int nargs);
static rtx hexagon_expand_builtin(
             tree exp,
             rtx target,
             rtx subtarget,
             enum machine_mode mode,
             int ignore);
static const char *hexagon_invalid_within_doloop(const_rtx insn);
static void hexagon_allocate_stack(
              unsigned HOST_WIDE_INT size,
              int allocate_stack_insn);
static void hexagon_deallocate_stack(unsigned HOST_WIDE_INT size);
static tree hexagon_special_case_memcpy_fn(const char *name);
static void hexagon_emit_special_case_memcpy_fn(
              tree *fn,
              const char *name,
              rtx dst,
              rtx src,
              rtx size
#if 0
              ,bool tailcall
#endif
);
static void hexagon_expand_movmem_inline(rtx dst_0, rtx src_0, rtx nbytes,
                             rtx min_align, bool volatile_p);

void hexagon_print_insn_info(FILE *file, struct hexagon_insn_info *insn_info);
void hexagon_debug_insn_info(struct hexagon_insn_info *insn_info);
void hexagon_print_packet(FILE *file, struct hexagon_packet_info *packet);
void hexagon_print_packets(FILE *file, struct hexagon_packet_info *packet);
void hexagon_debug_packet(struct hexagon_packet_info *packet);
void hexagon_debug_packets(void);
void hexagon_print_bb_packets(FILE *file, basic_block bb);
void hexagon_debug_all_bb_packets(void);
void hexagon_debug_bb_packets(basic_block bb);

static int  hexagon_get_flags(rtx insn);
static struct hexagon_reg_access *hexagon_add_reg_access(
                                  struct hexagon_reg_access *accesses,
                                  rtx reg,
                                  int flags);
static struct hexagon_mem_access *hexagon_add_mem_access(
                                  struct hexagon_mem_access *accesses,
                                  rtx mem,
                                  int flags);
static int  hexagon_record_writes(rtx *x, void *insn_info);
static int  hexagon_record_reads(rtx *x, void *insn_info);
static struct hexagon_insn_info *hexagon_get_insn_info(rtx insn);
static struct hexagon_packet_info *hexagon_start_new_packet(void);
static void hexagon_add_insn_to_packet(
              struct hexagon_packet_info *packet,
              struct hexagon_insn_info *insn_info,
              bool add_to_beginning);
static void hexagon_remove_insn_from_packet(
              struct hexagon_packet_info *packet,
              struct hexagon_insn_info *insn_info);
static bool hexagon_can_speculate_p(
              struct hexagon_insn_info *insn_info,
              basic_block bb);
static void hexagon_add_live_out(struct hexagon_insn_info *insn_info, basic_block bb);
static bool hexagon_predicable_p(struct hexagon_insn_info *insn_info);
static struct hexagon_insn_info *hexagon_predicate_insn(
                                 struct hexagon_insn_info *insn_info,
                                 struct hexagon_insn_info *jump_insn_info,
                                 bool invert_condition);
static bool hexagon_gpr_dot_newable_p(
              struct hexagon_insn_info *producer,
              struct hexagon_insn_info *consumer,
              struct hexagon_dependence *dependence);
static int hexagon_dot_newify_rtx(rtx *x, void *voidp_use);
static struct hexagon_insn_info *hexagon_dot_newify_gpr(
                                 struct hexagon_insn_info *insn_info,
                                 struct hexagon_dependence *dependence);
static bool hexagon_mem_dot_newable_p(
              struct hexagon_insn_info *producer,
              struct hexagon_insn_info *consumer,
              struct hexagon_dependence *dependence);
static struct hexagon_insn_info *hexagon_dot_newify_mem(
                                 struct hexagon_insn_info *insn_info,
                                 struct hexagon_dependence *dependence);
static int hexagon_find_new_value(rtx *x, void *y);
static void hexagon_dot_oldify_insn(struct hexagon_insn_info *insn_info);
static bool hexagon_predicate_dot_newable_p(struct hexagon_insn_info *producer,
                                            struct hexagon_insn_info *insn_info);
static struct hexagon_insn_info *hexagon_dot_newify_predicate(
                                 struct hexagon_insn_info *insn_info);
static bool hexagon_prologue_insn_p(struct hexagon_insn_info *insn_info);
static struct hexagon_dependence *hexagon_add_dependence(
                                  struct hexagon_dependence *dependencies,
                                  enum hexagon_dependence_type type,
                                  rtx set,
                                  rtx use);
static struct hexagon_dependence *hexagon_remove_dependence(
                                  struct hexagon_dependence *dependencies,
                                  struct hexagon_dependence *dependence);
static struct hexagon_dependence *hexagon_concat_dependencies(
                                  struct hexagon_dependence *a,
                                  struct hexagon_dependence *b);
static struct hexagon_dependence *hexagon_control_dependencies(
                                  struct hexagon_insn_info *first,
                                  struct hexagon_insn_info *second);
static struct hexagon_dependence *hexagon_true_dependencies(
                                  struct hexagon_insn_info *writer,
                                  struct hexagon_insn_info *reader);
static struct hexagon_dependence *hexagon_output_dependencies(
                                  struct hexagon_insn_info *insn0,
                                  struct hexagon_insn_info *insn1);
static struct hexagon_dependence *hexagon_anti_dependencies(
                                  struct hexagon_insn_info *earlier,
                                  struct hexagon_insn_info *later);
static GTY(()) struct hexagon_dependence *anti_dependencies;
static GTY(()) struct hexagon_dependence *true_dependencies;
static GTY(()) struct hexagon_dependence *control_dependencies;
static GTY(()) struct hexagon_dependence *dependencies;
static bool hexagon_packet_insn_dependence_p(
              struct hexagon_packet_info *packet,
              struct hexagon_insn_info **insn_info,
              bool ignore_jumps);
static bool hexagon_packet_insn_internal_dependence_p(
              struct hexagon_packet_info *packet,
              struct hexagon_insn_info *insn_info);
static bool hexagon_insn_fits_in_packet_p(
              struct hexagon_insn_info *insn,
              struct hexagon_packet_info *packet);
static bool hexagon_can_add_insn_to_packet_p(
              struct hexagon_insn_info **insn,
              struct hexagon_packet_info *packet);
static rtx  hexagon_bb_real_head_insn(basic_block bb);
static rtx  hexagon_bb_real_end_insn(basic_block bb);
static void hexagon_create_bb_sentinel_packet(struct hexagon_packet_info *packet);
static void hexagon_finalize_transformations(
              struct hexagon_packet_info *first,
              struct hexagon_packet_info *last);
static void hexagon_pack_insns(bool need_bb_info);
static void hexagon_gen_hintjr(void);
static void hexagon_move_insn(
              struct hexagon_insn_info *old_insn,
              struct hexagon_packet_info *from_packet,
              struct hexagon_insn_info *new_insn,
              struct hexagon_packet_info *to_packet,
              bool move_to_beginning);
static void hexagon_sanity_check_cfg_packet_info(void);
static void hexagon_pull_up_insns(void);
static void hexagon_init_packing_info(bool need_bb_info);
static void hexagon_remove_new_values(void);
static void hexagon_free_packing_info(bool free_bb_info);
static void hexagon_packet_optimizations(void);
static bool hexagon_move_insn_down_p(struct hexagon_insn_info* insn,
             struct hexagon_packet_info* pred_packet,
             struct hexagon_packet_info* succ_packet);

static void hexagon_final_pack_insns(void);
static void hexagon_pack_duplex_insns(void);


static bool hexagon_can_be_new_value_store_p(
  struct hexagon_insn_info *producer,
  struct hexagon_insn_info *consumer,
  struct hexagon_dependence *dependencer);

static bool hexagon_can_be_new_value_p(
  struct hexagon_insn_info *producer,
  struct hexagon_insn_info *consumer,
  struct hexagon_dependence *dependencer);

/*generate a new value nump for V4 */
static bool hexagon_can_be_new_value_jump_p(
  struct hexagon_insn_info *consumer,
  struct hexagon_dependence *dependencer);
static void hexagon_new_value_jump(void );
static struct hexagon_insn_info * hexagon_nvj_query_jump(
              struct hexagon_packet_info *packet,
              bool *dot_new_predicate,
              basic_block bb);
static struct hexagon_insn_info * hexagon_nvj_query_compare (
              struct hexagon_packet_info *packet,
              struct hexagon_insn_info *jump_p_insn_info);
static bool hexagon_used_operands(
              struct hexagon_packet_info *packet,
              struct hexagon_insn_info *jump_p_insn_info,
              struct hexagon_insn_info *compare_insn_info);
static bool hexagon_register_defined(
              struct hexagon_insn_info *insn,
              unsigned int reg_no );
static bool hexagon_register_used(
              struct hexagon_insn_info *insn,
              unsigned int reg_no  );
static struct hexagon_packet_info * hexagon_get_source_packet(
              struct hexagon_packet_info *packet,
              rtx feeder_reg,
              struct hexagon_insn_info *d_insn,
              struct hexagon_insn_info **s_insn);
static bool hexagon_move_insn_across_packet_down(
              struct hexagon_packet_info *from_packet,
              struct hexagon_packet_info *to_packet,
              struct hexagon_insn_info *insn);
static bool hexagon_nvj_move_possible(
              struct hexagon_packet_info *candidate_packet,
              struct hexagon_packet_info *prev_packet,
              struct hexagon_insn_info *jump_p_insn_info,
              struct hexagon_insn_info *compare_info_info,
              rtx *operator,
              rtx *pred,
              rtx *op1,
              rtx *op2,
              rtx *b_label,
              struct hexagon_packet_info **src_packet,
              struct hexagon_insn_info **src_insn,
              int *op_cnt);

static bool hexagon_nvj_compare_p(rtx insn);
static int  hexagon_get_reads_count(struct hexagon_insn_info *insn_info);

static bool hexagon_nvj_new_gpr_p(
              struct hexagon_insn_info *consumer,
              struct hexagon_dependence *dependence);
static bool hexagon_nvj_check_resources (
              struct hexagon_insn_info *q_insn,
              struct hexagon_packet_info *nvj_packet,
              struct hexagon_insn_info *comp_insn,
              struct hexagon_insn_info *jump_insn,
              rtx nvj_insn_rtx );
static bool hexagon_in_bb_live_out(
              basic_block bb,
              struct hexagon_insn_info *q_insn);
static void hexagon_remove_empty_packet(
              basic_block bb,
              struct hexagon_packet_info *packet);
static bool hexagon_predicate_use_DImode( struct hexagon_insn_info *insn_info);
static rtx hexagon_nvj_swap_operands(rtx insn);
static rtx hexagon_nvj_get_compare( rtx nvj_insn, int elem_num);
static rtx hexagon_nvj_get_operand( rtx insn, int op_count);
#if 0
void
hexagon_duplicate_doloop_begin(basic_block condition_bb, struct loop *loop);
#endif

static unsigned int smallest_accessable_entity_in_declaration (tree);
static void hexagon_load_tls_register (void);
void require_pic_register (void);
void require_tls_register (void);
bool hexagon_legitimate_pic_operand_p(rtx operand);

/* Set default optimization options. */
static const struct default_options hexagon_option_optimization_table[] =
{
    { OPT_LEVELS_SIZE, OPT_mextended_crossjumping, NULL, 1 },
    { OPT_LEVELS_SIZE, OPT_mlocal_combine, NULL, 1 },
    { OPT_LEVELS_SIZE, OPT_mduplex_scheduling, NULL, 1 },
    { OPT_LEVELS_1_PLUS, OPT_fomit_frame_pointer, NULL, 1 },
    { OPT_LEVELS_1_PLUS, OPT_mmod_hw_loops, NULL, 1 },
    { OPT_LEVELS_2_PLUS, OPT_mpackets, NULL, 1},
    { OPT_LEVELS_2_PLUS, OPT_mpullup, NULL, 1},
    { OPT_LEVELS_2_PLUS, OPT_maggregate_access, NULL, 1},
    { OPT_LEVELS_2_PLUS, OPT_mlocal_combine_immediate, NULL, 1},
    { OPT_LEVELS_2_PLUS, OPT_muse_common_prologue_epilogue_functions, NULL, 1},
    { OPT_LEVELS_1_PLUS_SPEED_ONLY, OPT_mfalign_loops, NULL, 1},
    { OPT_LEVELS_3_PLUS, OPT_mfalign_all, NULL, 1},
    { OPT_LEVELS_NONE, 0, NULL, 0 }
};

/* Implement TARGET_OPTION_DEFAULT_PARAMS.  */
static void
hexagon_option_default_params (void)
{
  set_default_param_value (PARAM_MIN_CROSSJUMP_INSNS, 2);
  set_default_param_value (PARAM_EARLY_INLINING_INSNS, 20);
  set_default_param_value (PARAM_MAX_COMPLETELY_PEELED_INSNS, 200);
}

/*---------------------------
Run-time Target Specification
---------------------------*/

static const struct hexagon_arch_table_entry hexagon_arch_table[NUM_HEXAGON_ARCH] =
  HEXAGON_ARCH_TABLE_INITIALIZER;

static const struct hexagon_abi_table_entry hexagon_abi_table[NUM_HEXAGON_ABI] =
  HEXAGON_ABI_TABLE_INITIALIZER;

/* Implements hook TARGET_HANDLE_OPTION

   Process command-line options such as -march. */

static bool
hexagon_handle_option(size_t code, const char *arg, int value ATTRIBUTE_UNUSED)
{
  int i;

  switch(code){
    case OPT_march_:
    case OPT_mcpu_:
      for(i = 0; i < NUM_HEXAGON_ARCH; i++){
        if(strcmp(arg, hexagon_arch_table[i].name) == 0){
          hexagon_arch = hexagon_arch_table[i].arch;
          hexagon_features = hexagon_arch_table[i].features;
          return true;
        }
      }
      error("bad value (%s) for -m%s= option",
            arg, code == OPT_march_ ? "arch" : "cpu");
      return false;
    case OPT_mabi_:
      for(i = 0; i < NUM_HEXAGON_ABI; i++){
        if(strcmp(arg, hexagon_abi_table[i].name) == 0){
          hexagon_abi = hexagon_abi_table[i].abi;
          return true;
        }
      }
      error("bad value (%s) for -mabi= option", arg);
      return false;
    case OPT_mno_falign:
      hexagon_falign = HEXAGON_NO_FALIGN;
      return true;
    case OPT_mfalign_loops:
      hexagon_falign = HEXAGON_FALIGN_LOOPS;
      return true;
    case OPT_mfalign_labels:
      hexagon_falign = HEXAGON_FALIGN_LABELS;
      return true;
    case OPT_mfalign_all:
      hexagon_falign = HEXAGON_FALIGN_ALL;
      return true;
    case OPT_mqdsp6_compat:
      hexagon_qdsp6_compat = 1;
      return true;
  }
  return true;
}




/* Implements macro OVERRIDE_OPTIONS

   Make any necessary final decisions about command-line options. */

void
hexagon_override_options(void)
{
  int arch_index;
  int abi_index;

  if(hexagon_arch == HEXAGON_ARCH_UNSPECIFIED){
    arch_index = HEXAGON_ARCH_TABLE_DEFAULT_INDEX;
    hexagon_arch = hexagon_arch_table[arch_index].arch;
    hexagon_features = hexagon_arch_table[arch_index].features;
  }

  if(hexagon_arch < HEXAGON_ARCH_V4){
    error("Architectures earlier than hexagonv4 are no longer supported.");
  }

  if(hexagon_abi == HEXAGON_ABI_UNSPECIFIED){
    abi_index = HEXAGON_ABI_TABLE_DEFAULT_INDEX;
    hexagon_abi = hexagon_abi_table[abi_index].abi;
  }

  if(TARGET_UNCACHED_DATA){
    if(hexagon_arch == HEXAGON_ARCH_V1 || hexagon_arch == HEXAGON_ARCH_V2){
      hexagon_dual_memory_accesses = false;
    }
    else {
      warning(0, "Ignoring the -mv1-v2-uncached-data option as it is only valid for V1 an V2.");
    }
  }

  if(!TARGET_PACKETS){
    target_flags &= ~MASK_PULLUP;
  }

  if(!optimize){
    target_flags &= ~MASK_PACKETS;
    target_flags &= ~MASK_PULLUP;
    hexagon_falign = HEXAGON_NO_FALIGN;
  }

  if (flag_pic){
    /* Make -fpic imply -G 0 */
    g_switch_value = 0;
  }
  else if(!global_options_set.x_g_switch_value){
#ifdef LINUX
  /* On Linux, use -G0 as the default */
    g_switch_value = 0;
#else
    g_switch_value = 8;
#endif
  }

  /* Linking objects built with different -G values is unsupported, and
     only -G0 and -G8 multilibs are provided. Section sorting assumes
     a single didgit g_switch_value. */
  if (g_switch_value > 8)
    sorry ("-G%d\n", g_switch_value);

  /* Align functions to 16-byte boundaries to prevent
     target-packet-crosses-16-byte-boundary (falign) stalls. */
  if(align_functions == 0){
    align_functions = 16;
  }

  if(!TARGET_CONST32 || !TARGET_LITERAL_POOL_ADDRESSES || hexagon_arch >= HEXAGON_ARCH_V4)
    target_flags &= ~MASK_AGGREGATE_ACCESS;

  /* Taxis code degraded immensely because of the subreg1 pass.  The pass splits
     double word opcodes into single words, which resulted in larger packet
     size.  See Qualcomm Bugzilla #3659 for the details.  To enable the subreg1
     pass, use -menable-subreg1 */
  if(!TARGET_ENABLE_SUBREG1){
    flag_split_wide_types = false;
  }
}




/*---------------------------------------------------
Defining data structures for per-function information
---------------------------------------------------*/

/* Implements macro INIT_EXPANDERS */

void
hexagon_init_expanders(void)
{
  init_machine_status = hexagon_init_machine_status;
}




/* initializes the machine specific per-function data */

static struct machine_function *
hexagon_init_machine_status(void)
{
  struct machine_function *mf;
  mf = ggc_alloc_cleared_machine_function ();
  return mf;
}




/*------------
Storage Layout
------------*/

/* Return the TYPE of the elements comprising
   the innermost dimension of ARRAY.  */

static tree
get_inner_array_type (const_tree array)
{
  tree type = TREE_TYPE (array);

  while (TREE_CODE (type) == ARRAY_TYPE)
    type = TREE_TYPE (type);

  return type;
}

/* Implements hook TARGET_VECTORIZE_PREFERRED_SIMD_MODE

   HEXAGON can operate on doubleword vectors, i.e. 8 units. */

static enum machine_mode
hexagon_vectorize_preferred_simd_mode (enum machine_mode mode)
{
  switch (mode)
    {
      case QImode: return V8QImode;
      case HImode: return V4HImode;
      case SImode: return V2SImode;
      default: return word_mode;
    }
}

/* Helper function for the alignment routines

   Aligns arrays whose element sizes are <= BITS_PER_WORD
   to BIGGEST_ALIGNMENT.  This facilitates vectorization. */

static int
hexagon_array_alignment(tree type, int align)
{
  tree elem_type = get_inner_array_type(type);

  if(TYPE_SIZE (elem_type)
     && TREE_CODE (TYPE_SIZE (elem_type)) == INTEGER_CST
     && TREE_INT_CST_LOW (TYPE_SIZE (elem_type)) <= BITS_PER_WORD){
    return MAX (align, BIGGEST_ALIGNMENT);
  }
  else {
    return align;
  }
}




/* Implements macro DATA_ALIGNMENT

   Align a static variable
   Doubleword align arrays whose element size <= 4 bytes to facilitate
   vectorization. */

int
hexagon_data_alignment(tree type, int align)
{
  if(TREE_CODE (type) == ARRAY_TYPE){
    return hexagon_array_alignment(type, align);
  }

  return align;
}




/* Implements macro CONSTANT_ALIGNMENT

   Doubleword align constant strings. */

int
hexagon_constant_alignment(tree constant, int align)
{
  if(!optimize_size && TREE_CODE (constant) == STRING_CST
     && TREE_STRING_LENGTH (constant) >= BIGGEST_ALIGNMENT
     && (align) < BIGGEST_ALIGNMENT){
    return MAX (align, BIGGEST_ALIGNMENT);
  }

  return align;
}


/* Implements macro TARGET_CANNOT_FORCE_CONST_MEM.  */

static bool
hexagon_cannot_force_const_mem (rtx x)
{
  return hexagon_tls_referenced_p (x);
}

/* Implements macro LOCAL_ALIGNMENT

   Align a local variable on the stack
   Doubleword align arrays whose element size <= 4 bytes to facilitate
   vectorization. */

unsigned int
hexagon_local_alignment(tree type, int align)
{
  if(TREE_CODE (type) == ARRAY_TYPE){
    return hexagon_array_alignment(type, align);
  }

  return align;
}




/* Implements hook TARGET_ALIGN_ANON_BITFIELD

   Unnamed (anonymous) bitfields affect alignment and padding the same way named
   bitfields do. */

static bool
hexagon_align_anon_bitfield(void)
{
  return true;
}




/*----------------------------------
Layout of Source Language Data Types
----------------------------------*/

/* Implements hook TARGET_DEFAULT_SHORT_ENUMS

   The default ABI specifies that enumerated types behave like the smallest
   integer type that can hold all of the enumerated constants associated with
   the enumerated type.  Linux requires its ABI to specify non-short enums. */

static bool
hexagon_default_short_enums(void)
{
  return hexagon_abi != HEXAGON_ABI_LINUX;
}




/*--------------------------------
Basic Characteristics of Registers
--------------------------------*/

/* Implements macro CONDITIONAL_REGISTER_USAGE

   HEXAGON ABI 2 increases the number of callee save registers from 4 to 12. */

void
hexagon_conditional_register_usage(void)
{
  int i;
  if(hexagon_abi != HEXAGON_ABI_1){
    for(i = 16; i < 24; i++){
      if(!fixed_regs[i]){
        call_used_regs[i] = 0;
      }
    }
  }

  /* For PIC, we reserve a register for the GOT pointer */
  if (flag_pic) {
    fixed_regs[PIC_OFFSET_TABLE_REGNUM] = 1;
    call_used_regs[PIC_OFFSET_TABLE_REGNUM] = 1;
  }
}




/*--------------
Register Classes
--------------*/

/* Implements macro CANNOT_CHANGE_MODE_CLASS

   Prevent predicates from being represented as QImode SUBREGs of SImode. */

bool
hexagon_cannot_change_mode_class(
  enum machine_mode from,
  enum machine_mode to,
  enum reg_class rclass
)
{
  if(from == to){
    return false;
  }

  if(rclass == PREDICATE_REGS
     && from == SImode
     && to == QImode){
    return true;
  }

  return false;
}


/* True for integers that can be formed using combine(#s8,#s8).  */

bool
const_ok_for_s8s8 (HOST_WIDE_INT val)
{
  HOST_WIDE_INT low, high;

  high = trunc_int_for_mode (val >> 32ULL, SImode);
  low = trunc_int_for_mode (val, SImode);

  return ((high & 0xffffff00) == (high & 0x80 ? 0xffffff00 : 0))
         && ((low & 0xffffff00) == (low & 0x80 ? 0xffffff00 : 0));
}

/* True for integers that can be formed using combine(#s32,#s8).  */

bool
const_ok_for_s32s8 (HOST_WIDE_INT val)
{
  val = trunc_int_for_mode (val, SImode);

  return (val & 0xffffff00) == (val & 0x80 ? 0xffffff00 : 0);
}

/* True for integers that can be formed using combine(#s8,#s32).  */

bool
const_ok_for_s8s32 (HOST_WIDE_INT val)
{
  val = trunc_int_for_mode (val >> 32ULL, SImode);

  return (val & 0xffffff00) == (val & 0x80 ? 0xffffff00 : 0);
}

/* True for integers that can be formed using combine(#s32,#s16).  */

bool
const_ok_for_s32s16 (HOST_WIDE_INT val)
{
  val = trunc_int_for_mode (val, SImode);

  return (val & 0xffff0000) == (val & 0x8000 ? 0xffff0000 : 0);
}

/* True for integers that can be formed using combine(#s16,#s32).  */

bool
const_ok_for_s16s32 (HOST_WIDE_INT val)
{
  val = trunc_int_for_mode (val >> 32ULL, SImode);

  return (val & 0xffff0000) == (val & 0x8000 ? 0xffff0000 : 0);
}

/* True for power-of-2 integers that can be represented in 32 bits.  */

bool
const_ok_for_onehot (HOST_WIDE_INT val)
{
  return val && (val == (val & -val));
}

/* Implements hook TARGET_SECONDARY_RELOAD

   Predicate registers can not be directly loaded from or stored to memory.
   The value must first be moved to a general purpose register. */

static reg_class_t
hexagon_secondary_reload (bool in_p ATTRIBUTE_UNUSED,
                          rtx x,
                          reg_class_t reload_class,
                          enum machine_mode reload_mode ATTRIBUTE_UNUSED,
                          secondary_reload_info *sri ATTRIBUTE_UNUSED)
{
  switch(reload_class){
    case PREDICATE_REGS:
      if(true_regnum(x) < 0){
        return GENERAL_REGS;
      }
      break;
    default:
      break;
  }
  return NO_REGS;
}

/*---------------------------------------
Eliminating Frame Pointer and Arg Pointer
---------------------------------------*/

/* Implement TARGET_FRAME_POINTER_REQUIRED.  */
bool
hexagon_frame_pointer_required (void)
{
  /* Without this, the nonlocal goto/longjmp code fails to restore
     the hard frame pointer correctly.  */
  return cfun->has_nonlocal_label;
}

/* Implement TARGET_CAN_ELIMINATE.  */
bool
hexagon_can_eliminate (const int from ATTRIBUTE_UNUSED, const int to)
{
  /* If we need a frame pointer, we want to eliminate to
     HARD_FRAME_POINTER_REGNUM, not STACK_POINTER_REGNUM.  */
  if (to == STACK_POINTER_REGNUM && frame_pointer_needed)
    return false;
  else
    return true;
}


/*----------------
Basic Stack Layout
----------------*/

/* Implements macro RETURN_ADDR_RTX

   Return an RTX indicating where the return address can be found given FRAME,
   the address of the stack frame COUNT frames above the current one. */

rtx
hexagon_return_addr_rtx(int count, rtx frame)
{
  rtx return_addr;

  cfun->machine->calls_builtin_return_address = true;

  if(count == 0){
    frame = gen_rtx_REG(Pmode, HARD_FRAME_POINTER_REGNUM);
  }

  return_addr = memory_address(Pmode,
                               plus_constant(frame, GET_MODE_SIZE (Pmode)));
  return_addr = gen_rtx_MEM (Pmode, return_addr);
  set_mem_alias_set(return_addr, get_frame_alias_set());

  return return_addr;
}




/*---------------------------------------
Eliminating Frame Pointer and Arg Pointer
---------------------------------------*/

/* HEXAGON stack frames look like:

             Before call                       After call


        +-----------------------+       +-----------------------+
        | 4 bytes save area for |       | 4 bytes save area for |
        | each of LR and prevFP |       | each of LR and prevFP |
        | (8 bytes total)       |       | (8 bytes total)       |
  FP+0->+-----------------------+       +-----------------------+
        |                       |       |                       |
   high |  local variables,     |       |  local variables,     |
   mem  |  reg save area, etc.  |       |  reg save area, etc.  |
        |                       |       |                       |
        +-----------------------+       +-----------------------+
        |                       |       |                       |
        |  arguments on stack.  |       |  arguments on stack.  |
        |                       |       |                       |
  SP+0->+-----------------------+       +-----------------------+
                                        | 4 bytes save area for |
                                        | each of LR and prevFP |
                                        | (8 bytes total)       |
                                  FP+0->+-----------------------+
                                        |                       |
                                        |  register save area   |
                                        |  (callee saved)       |
                                        |                       |
                                        +-----------------------+
                                        |                       |
                                        |  local variables      |
                                        |                       |
                                        +-----------------------+
                                        |                       |
                                        |  alloca allocations   |
                                        |                       |
                                        +-----------------------+
                                        |                       |
                 arguments grow updards |  arguments on stack   |
                                        |                       |
                                  SP+0->+-----------------------+
   low
   memory

*/

/* Helper function for computing the frame info

   True if register REGNO should be saved/restored in the prologue/epilogue. */

static bool
hexagon_save_register_p(unsigned int regno)
{
  unsigned int eh_regno;
  unsigned int i;
  struct hexagon_frame_info *info;

  info = &cfun->machine->frame_info;

  if(regno >= FIRST_PSEUDO_REGISTER){
    return false;
  }

  if(crtl->calls_eh_return){
    i = 0;
    do {
      eh_regno = EH_RETURN_DATA_REGNO (i);
      if(eh_regno == regno){
        return true;
      }
      i++;
    }while(eh_regno != INVALID_REGNUM);
  }

  if (regno == TLS_REGNUM &&
      (info->tls_set || df_regs_ever_live_p(regno) || TARGET_SAVE_RESTORE_R25))
    return true;

  /* Handle the pic register differently since it is a call_used_reg
     and a fixed_reg.  Also, check both df_regs_ever_live_p and
     uses_pic_offset_table in the odd case that uses_pic_offset_table
     is set, but df_regs_ever_live_p is not. */
  if (flag_pic && regno == PIC_OFFSET_TABLE_REGNUM &&
      (df_regs_ever_live_p(regno)
       || crtl->uses_pic_offset_table
       || crtl->saves_all_registers)) {
    return true;
  }

  /* The return address and frame pointer are treated separately.  Don't
     consider them here. */
  return regno != LINK_REGNUM
      && regno != HARD_FRAME_POINTER_REGNUM
      && !call_used_regs[regno]
      && (df_regs_ever_live_p(regno)
          || crtl->calls_eh_return);
}




/* The allocframe instruction takes an unsigned,
   double word aligned, 14-bit immediate */
#define MAX_ALLOCFRAME_IMMED ((1U << 14) - 8)
/* The add instruction takes a signed 16-bit immediate */
#define MIN_ADD_IMMED        (-(1 << 15))
#define MAX_ADD_IMMED        ( (1 << 15) - 1)
/* The mem* instructions take a signed 11-bit immediate scaled by access size */
#define MAX_MEMW_OFFSET      ( (1 << 12) - 4)
#define MAX_MEMD_OFFSET      ( (1 << 13) - 8)

#define HEXAGON_BIGGEST_ALIGN(SIZE) \
  (((SIZE) + BIGGEST_ALIGNMENT / 8 - 1) & ~(BIGGEST_ALIGNMENT / 8 - 1))

/* Helper function for hexagon_frame_info

   Make all of the decisions about how to generate the prologue and epilogue. */

static void
hexagon_make_prologue_epilogue_decisions(struct hexagon_frame_info *info)
{
  static struct {
    enum insn_code prologue_function;
    unsigned int num_function_saved_pairs;
    enum insn_code epilogue_function;
    unsigned int num_function_restored_pairs;
    enum insn_code sibcall_epilogue_function;
    unsigned int num_sibcall_function_restored_pairs;
  }
    prologue_epilogue_functions_abi1[] =
      {
        {
          CODE_FOR_nothing, 0,
          CODE_FOR_deallocframe_function, 0,
          CODE_FOR_nothing, 0
        },
        {
          CODE_FOR_nothing, 0,
          CODE_FOR_restore_r24_through_r25_and_deallocframe, 1,
          CODE_FOR_restore_r24_through_r25_and_deallocframe_before_sibcall, 1
        },
        {
          CODE_FOR_save_r24_through_r27, 2,
          CODE_FOR_restore_r24_through_r27_and_deallocframe, 2,
          CODE_FOR_restore_r24_through_r27_and_deallocframe_before_sibcall, 2
        }
      },
    prologue_epilogue_functions_abi2[] =
      {
        {
          CODE_FOR_nothing, 0,
          CODE_FOR_deallocframe_function, 0,
          CODE_FOR_nothing, 0
        },
        {
          CODE_FOR_nothing, 0,
          CODE_FOR_restore_r16_through_r17_and_deallocframe, 1,
          CODE_FOR_restore_r16_through_r17_and_deallocframe_before_sibcall, 1
        },
        {
          CODE_FOR_save_r16_through_r19, 2,
          CODE_FOR_restore_r16_through_r19_and_deallocframe, 2,
          CODE_FOR_restore_r16_through_r19_and_deallocframe_before_sibcall, 2
        },
        {
          CODE_FOR_save_r16_through_r21, 3,
          CODE_FOR_restore_r16_through_r21_and_deallocframe, 3,
          CODE_FOR_restore_r16_through_r21_and_deallocframe_before_sibcall, 3
        },
        {
          CODE_FOR_save_r16_through_r23, 4,
          CODE_FOR_restore_r16_through_r23_and_deallocframe, 4,
          CODE_FOR_restore_r16_through_r23_and_deallocframe_before_sibcall, 4
        },
        {
          CODE_FOR_save_r16_through_r25, 5,
          CODE_FOR_restore_r16_through_r25_and_deallocframe, 5,
          CODE_FOR_restore_r16_through_r25_and_deallocframe_before_sibcall, 5
        },
        {
          CODE_FOR_save_r16_through_r27, 6,
          CODE_FOR_restore_r16_through_r27_and_deallocframe, 6,
          CODE_FOR_restore_r16_through_r27_and_deallocframe_before_sibcall, 6
        }
      },
    *prologue_epilogue_functions;

  static const enum insn_code
    allocframe_insn[] = {
      CODE_FOR_allocframe,
      CODE_FOR_allocframe_and_save_r16,
      CODE_FOR_allocframe_and_save_r16_and_r17
    },
    allocate_stack_insn[] = {
      CODE_FOR_increment_stack_pointer,
      CODE_FOR_allocate_stack_and_save_r16,
      CODE_FOR_allocate_stack_and_save_r16_and_r17,
      CODE_FOR_allocate_stack_and_save_r16_through_r18,
      CODE_FOR_allocate_stack_and_save_r16_through_r19
    };

  bool use_common_functions;
  unsigned int max_function_saved_pairs = 0;
  unsigned int first_callee_save_regno = 0;
  unsigned int num_regs_to_save_while_allocating = 0;
  unsigned int regno;
  unsigned int i = 0;
  unsigned int j;

  /* Select the set of prologue and epilogue functions for the target ABI. */
  if(hexagon_abi != HEXAGON_ABI_1){
    prologue_epilogue_functions = prologue_epilogue_functions_abi2;
    max_function_saved_pairs = ARRAY_SIZE (prologue_epilogue_functions_abi2)
                               - 1;
    first_callee_save_regno = 16;
  }
  else {
    prologue_epilogue_functions = prologue_epilogue_functions_abi1;
    max_function_saved_pairs = ARRAY_SIZE (prologue_epilogue_functions_abi1)
                               - 1;
    first_callee_save_regno = 24;
  }
  prologue_epilogue_functions[0].epilogue_function
    = CODE_FOR_deallocframe_function;

  /* Can we omit the allocframe and deallocframe instructions? */
  if(!info->use_allocframe){
    info->allocframe_size = 0;
    info->sp_adjustment = info->frame_size;
    info->base_reg = stack_pointer_rtx;
    info->offset = info->frame_size;
  }
  else {
    /* Can we allocate the entire stack frame with one allocframe
       instruction? */
    if(info->frame_size <= MAX_ALLOCFRAME_IMMED){
      info->allocframe_size = info->frame_size;
      info->sp_adjustment = 0;
    }
    else {
      info->allocframe_size = MAX_ALLOCFRAME_IMMED;
      info->sp_adjustment = info->frame_size - MAX_ALLOCFRAME_IMMED;
    }
    /* Use SP as the base if the offset would be small, making the loads and
       stores candidates for duplexes. */
    if(info->frame_size <= 256){
      info->base_reg = stack_pointer_rtx;
      info->offset = info->frame_size;
    }
    else {
      info->base_reg = hard_frame_pointer_rtx;
      info->offset = 0;
    }
  }

#ifdef LINUX
  /* Bugfix 4799: Do not emit common functions for Linux. May result in
     incorrect behavior in the presence of PLT trampolines */
  use_common_functions = false;
#else
  use_common_functions = !flag_pic &&
                         TARGET_USE_COMMON_PROLOGUE_EPILOGUE_FUNCTIONS &&
                         (optimize_size || optimize == 2) &&
                         info->use_allocframe &&
                         !crtl->calls_eh_return;
#endif

  /* If we are using a function to save or restore callee-save registers or we
     are forming packets, then maximize the number of registers saved as pairs,
     since the common prologue and epilogue functions only save registers as
     pairs and doubleword store subinstructions have larger offsets than word
     store subinstructions. */
  if((use_common_functions || TARGET_PACKETS)
     && info->num_saved_singles % 2 == 1){

    /* Count the number of contiguous callee-save register pairs being saved. */
    for(i = 0, regno = first_callee_save_regno;
        i < info->num_saved_pairs && info->saved_pairs[i] == regno;
        i++, regno += 2);

    /* If the first single callee-save register is part of the next pair that
       could be saved, then remove it from the list of single callee-save
       registers and insert it into the list of paired callee-save
       registers. */
    if((   info->saved_singles[0] == regno
        || info->saved_singles[0] == regno + 1)
       && !call_used_regs[regno]
       && !call_used_regs[regno + 1]){

      for(j = 0; j < info->num_saved_singles - 1; j++){
        info->saved_singles[j] = info->saved_singles[j + 1];
      }
      for(j = info->num_saved_pairs; j > i; j--){
        info->saved_pairs[j] = info->saved_pairs[j - 1];
      }
      info->saved_pairs[i] = regno;
      info->num_saved_pairs++;
      info->num_saved_singles--;

      /* The callee-save registers being saved must still fit in the space
         allocated. */
      gcc_assert(info->reg_size >= 2 * UNITS_PER_WORD
                                     * info->num_saved_pairs
                                   + UNITS_PER_WORD
                                     * info->num_saved_singles);
    }
  }

  info->prologue_function = CODE_FOR_nothing;
  info->epilogue_function = CODE_FOR_nothing;
  info->sibcall_epilogue_function = CODE_FOR_nothing;

  /* Determine whether we can save code size by using functions to implement
     common prologue or epilogue sequences. */
  if(use_common_functions){

    /* Count the number of callee-save register pairs that can be saved or
       restored by function calls in the prologue and epilogue. */
    for(i = 0, regno = first_callee_save_regno;
        i < info->num_saved_pairs && info->saved_pairs[i] == regno;
        i++, regno += 2);
    if(i > max_function_saved_pairs){
      i = max_function_saved_pairs;
    }

    /* At -O2, use the common prologe and epilogue functions only if they save
       and restore 3 or more pairs of callee-save registers. */
    if(optimize_size || i >= 3){
      #define SET_PROLOGUE_EPILOGUE_FUNCTION_INFO(FIELD) \
        info->FIELD = prologue_epilogue_functions[i].FIELD
      SET_PROLOGUE_EPILOGUE_FUNCTION_INFO (prologue_function);
      SET_PROLOGUE_EPILOGUE_FUNCTION_INFO (num_function_saved_pairs);
      SET_PROLOGUE_EPILOGUE_FUNCTION_INFO (epilogue_function);
      SET_PROLOGUE_EPILOGUE_FUNCTION_INFO (num_function_restored_pairs);
      SET_PROLOGUE_EPILOGUE_FUNCTION_INFO (sibcall_epilogue_function);
      SET_PROLOGUE_EPILOGUE_FUNCTION_INFO (num_sibcall_function_restored_pairs);
    }
    info->num_specially_saved_pairs = info->num_function_saved_pairs;
  }

  info->allocframe_insn = CODE_FOR_allocframe;
  info->allocate_stack_insn = CODE_FOR_increment_stack_pointer;

  /* If possible, save one or more callee-save registers while allocating
     stack. */
  if(TARGET_PACKETS
     && (info->use_allocframe || info->sp_adjustment != 0)
     && info->num_function_saved_pairs == 0){

    if(TARGET_V4_FEATURES && !info->use_allocframe){
      switch(info->num_saved_pairs){
        case 0:
          if(info->num_saved_singles == 1 && info->saved_singles[0] == 16){
            num_regs_to_save_while_allocating = 1;
          }
          break;
        case 1:
          if(info->saved_pairs[0] == 16){
            if(info->num_saved_singles == 1 && info->saved_singles[0] == 18){
              num_regs_to_save_while_allocating = 3;
            }
            else {
              num_regs_to_save_while_allocating = 2;
            }
          }
          break;
        default:
          if(info->saved_pairs[0] == 16){
            if(info->saved_pairs[1] == 18){
              num_regs_to_save_while_allocating = 4;
            }
            else {
              num_regs_to_save_while_allocating = 2;
            }
          }
          break;
      }
    }
    else if(TARGET_V4_FEATURES || !info->use_allocframe){
      if(info->num_saved_pairs == 0){
        if(info->num_saved_singles == 1 && info->saved_singles[0] == 16){
          num_regs_to_save_while_allocating = 1;
        }
      }
      else if(info->saved_pairs[0] == 16){
        num_regs_to_save_while_allocating = 2;
      }
    }

    if(info->use_allocframe){
      info->allocframe_insn
        = allocframe_insn[num_regs_to_save_while_allocating];
    }
    else {
      info->allocate_stack_insn
        = allocate_stack_insn[num_regs_to_save_while_allocating];
    }
    info->num_specially_saved_pairs = num_regs_to_save_while_allocating / 2;
    info->num_specially_saved_singles = num_regs_to_save_while_allocating % 2;
  }
}




/* Fills in the frame info in the frame_info structure
   and returns a pointer to that structure */

struct hexagon_frame_info *
hexagon_frame_info(void)
{
  struct hexagon_frame_info *info;
  unsigned int regno;

  info = &cfun->machine->frame_info;

  if(info->computed){
    return info;
  }

  /* Space for outgoing arguments */
  info->args_size = HEXAGON_BIGGEST_ALIGN (crtl->outgoing_args_size);

  /* Space for local variables, temporaries, and spills */
  info->var_size = HEXAGON_BIGGEST_ALIGN (get_frame_size());

  /* Space needed for callee-save registers */
  info->num_saved_pairs = 0;
  info->num_saved_singles = 0;
  for(regno = 0; regno < FIRST_PSEUDO_REGISTER; regno += 2){
    if(hexagon_save_register_p(regno)){
      if(hexagon_save_register_p(regno + 1)){
        info->saved_pairs[info->num_saved_pairs++] = regno;
      }
      else {
        info->saved_singles[info->num_saved_singles++] = regno;
      }
    }
    else if(hexagon_save_register_p(regno + 1)){
      info->saved_singles[info->num_saved_singles++] = regno + 1;
    }
  }
  info->reg_size = 2 * UNITS_PER_WORD * info->num_saved_pairs
                   + UNITS_PER_WORD * info->num_saved_singles;
  info->reg_size = HEXAGON_BIGGEST_ALIGN (info->reg_size);

  info->frame_size = info->args_size + info->var_size + info->reg_size;


  /* We omit the allocframe/deallocframe instructions in leaf functions when
     possible. */
  if(optimize
     && current_function_is_leaf
     && !frame_pointer_needed
     && !cfun->machine->calls_builtin_return_address
     && !crtl->calls_eh_return
     && !crtl->profile
     && !df_regs_ever_live_p(HARD_FRAME_POINTER_REGNUM)
     && !df_regs_ever_live_p(LINK_REGNUM)
     /* If the callee-save register area of the stack frame would be out of
        range of SP using a base+offset store, then use the allocframe and
        deallocframe instructions so the callee-save register area can be
        addressed via FP. */
     && !((info->num_saved_pairs || info->num_saved_singles)
          && info->frame_size - (info->num_saved_pairs
                                 ? 2 * UNITS_PER_WORD : UNITS_PER_WORD)
             > (info->num_saved_singles
                ? MAX_MEMW_OFFSET : MAX_MEMD_OFFSET))
     /* If the epilogue is more than a return instruction, then we can save
        code size by using one of the common epilogue functions, which require
        the frame pointer to point to the current frame. */
     /* ??? unless all function exits are tail calls and no callee-save
        registers are allocated. */
     && !(optimize_size && info->frame_size != 0)){
    info->lrfp_size = 0;
  }
  else {
    info->lrfp_size = 2 * UNITS_PER_WORD;
  }
  info->use_allocframe = info->lrfp_size != 0;

  info->total_size = info->frame_size + info->lrfp_size;

  if(reload_completed){
    hexagon_make_prologue_epilogue_decisions(info);
  }

  info->computed = reload_completed != 0;

  gcc_assert(info->frame_size % 8 == 0);

  return info;
}




/* Implements macro INITIAL_ELIMINATION_OFFSET

   Returns the number of bytes offset between FROM_REG and TO_REG
   for the current function. */

HOST_WIDE_INT
hexagon_initial_elimination_offset(int from_reg, int to_reg)
{
  struct hexagon_frame_info *frame;

  frame = hexagon_frame_info();

  if(      from_reg == ARG_POINTER_REGNUM
          && to_reg == STACK_POINTER_REGNUM){
    return frame->total_size;
  }
  else if( from_reg == FRAME_POINTER_REGNUM
          && to_reg == STACK_POINTER_REGNUM){
    return frame->args_size + frame->var_size;
  }
  else if( from_reg == HARD_FRAME_POINTER_REGNUM
          && to_reg == STACK_POINTER_REGNUM){
    return frame->frame_size;
  }
  else if( from_reg == ARG_POINTER_REGNUM
          && to_reg == HARD_FRAME_POINTER_REGNUM){
    return frame->lrfp_size;
  }
  else if( from_reg == FRAME_POINTER_REGNUM
          && to_reg == HARD_FRAME_POINTER_REGNUM){
    return -(frame->reg_size);
  }

  return 0;
}




/*----------------------------
Passing Arguments in Registers
----------------------------*/

static tree
hexagon_gimplify_va_arg_expr (tree valist, tree type, gimple_seq *pre_p,
			      gimple_seq *post_p)
{
  tree addr, t, type_size, rounded_size;
  unsigned HOST_WIDE_INT align, boundary;
  bool indirect;
  tree sav, sav_end, ovf, f_sav, f_sav_end, f_ovf;
  tree sav_tmp, sav_end_tmp, ovf_tmp;
  unsigned int candidate;
  tree lab_done, t2;
  tree recordtype = TREE_TYPE (va_list_type_node);

  f_sav = TYPE_FIELDS (recordtype);
  f_sav_end = DECL_CHAIN (f_sav);
  f_ovf = DECL_CHAIN (f_sav_end);

  valist = build_simple_mem_ref (valist);
  TREE_TYPE (valist) = recordtype;

  sav = build3 (COMPONENT_REF, TREE_TYPE (f_sav), unshare_expr (valist),
		f_sav, NULL_TREE);
  sav_end = build3 (COMPONENT_REF, TREE_TYPE (f_sav_end), unshare_expr (valist),
		    f_sav_end, NULL_TREE);
  ovf = build3 (COMPONENT_REF, TREE_TYPE (f_ovf), unshare_expr (valist),
		f_ovf, NULL_TREE);

  indirect = pass_by_reference (NULL, TYPE_MODE (type), type, false);
  if (indirect)
    type = build_pointer_type (type);

  gcc_assert (TREE_CODE (TYPE_SIZE (type)) == INTEGER_CST);

  candidate = 
    TREE_INT_CST_LOW (TYPE_SIZE_UNIT (type)) > 0
    && TREE_INT_CST_LOW (TYPE_SIZE_UNIT (type)) <= 2 * UNITS_PER_WORD;
  if (!candidate)
    return std_gimplify_va_arg_expr (ovf, type, pre_p, post_p);

  align = PARM_BOUNDARY / BITS_PER_UNIT;
  boundary = targetm.calls.function_arg_boundary (TYPE_MODE (type), type);

  /* When we align parameter on stack for caller, if the parameter
     alignment is beyond MAX_SUPPORTED_STACK_ALIGNMENT, it will be
     aligned at MAX_SUPPORTED_STACK_ALIGNMENT.  We will match callee
     here with caller.  */
  if (boundary > MAX_SUPPORTED_STACK_ALIGNMENT)
    boundary = MAX_SUPPORTED_STACK_ALIGNMENT;

  boundary /= BITS_PER_UNIT;

  /* Hoist the valist value into a temporary for the moment.  */
  sav_tmp = get_initialized_tmp_var (sav, pre_p, NULL);
  sav_end_tmp = get_initialized_tmp_var (sav_end, pre_p, NULL);
  ovf_tmp = get_initialized_tmp_var (ovf, pre_p, NULL);

  /* va_list pointer is aligned to PARM_BOUNDARY.  If argument actually
     requires greater alignment, we must perform dynamic alignment.  */
  if (boundary > align
      && !integer_zerop (TYPE_SIZE (type)))
    {
      t = build2 (MODIFY_EXPR, TREE_TYPE (ovf), ovf_tmp,
		  fold_build2 (POINTER_PLUS_EXPR,
			       TREE_TYPE (ovf_tmp),
			       ovf_tmp, size_int (boundary - 1)));
      gimplify_and_add (t, pre_p);

      t = fold_convert (sizetype, ovf_tmp);
      t = build2 (MODIFY_EXPR, TREE_TYPE (ovf_tmp), ovf_tmp,
		  fold_convert (TREE_TYPE (ovf_tmp),
				fold_build2 (BIT_AND_EXPR, sizetype, t,
					     size_int (-boundary))));
      gimplify_and_add (t, pre_p);
    }
  else
    boundary = align;

  /* If the actual alignment is less than the alignment of the type,
     adjust the type accordingly so that we don't assume strict alignment
     when dereferencing the pointer.  */
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

  /* Skip over odd register slots.  */
  if (TREE_INT_CST_LOW (TYPE_SIZE_UNIT (type)) > UNITS_PER_WORD)
    {
      unsigned int reg_boundary = 2 * UNITS_PER_WORD;
      t = build2 (MODIFY_EXPR, TREE_TYPE (sav_tmp), sav_tmp,
		  fold_build2 (POINTER_PLUS_EXPR,
			       TREE_TYPE (sav_tmp),
			       sav_tmp, size_int (reg_boundary - 1)));
      gimplify_and_add (t, pre_p);

      t = fold_convert (sizetype, sav_tmp);
      t = build2 (MODIFY_EXPR, TREE_TYPE (sav_tmp), sav_tmp,
		  fold_convert (TREE_TYPE (sav_tmp),
				fold_build2 (BIT_AND_EXPR, sizetype, t,
					     size_int (-reg_boundary))));
      gimplify_and_add (t, pre_p);
    }

  /* Compare sav with sav_end. */
  lab_done = create_artificial_label (UNKNOWN_LOCATION);

  /* handle arg in sav */

  addr = get_initialized_tmp_var (sav_tmp, pre_p, post_p);
  
  t = build_int_cstu (sizetype,
		      TREE_INT_CST_LOW (TYPE_SIZE_UNIT (type)) > UNITS_PER_WORD 
		      ? UNITS_PER_WORD * 2
		      : UNITS_PER_WORD);
  t = build2 (POINTER_PLUS_EXPR, TREE_TYPE (sav_tmp), sav_tmp, t);
  t = build2 (MODIFY_EXPR, TREE_TYPE (sav_tmp), sav, t);
  gimplify_and_add (t, pre_p);

  t = build2 (LE_EXPR, boolean_type_node, sav , sav_end);
  t2 = build1 (GOTO_EXPR, void_type_node, lab_done);
  t = build3 (COND_EXPR, void_type_node, t, t2, NULL_TREE);
  gimplify_and_add (t, pre_p);  

  t = build2 (MODIFY_EXPR, TREE_TYPE (ovf_tmp), addr, ovf_tmp);
  gimplify_and_add (t, pre_p);

  t = build2 (POINTER_PLUS_EXPR, TREE_TYPE (ovf_tmp), ovf_tmp, rounded_size);
  t = build2 (MODIFY_EXPR, TREE_TYPE (ovf_tmp), ovf, t);
  gimplify_and_add (t, pre_p);

  gimple_seq_add_stmt (pre_p, gimple_build_label (lab_done));

  addr = fold_convert (build_pointer_type (type), addr);

  if (indirect)
    addr = build_va_arg_indirect_ref (addr);

  return build_va_arg_indirect_ref (addr);
}

/* Implements macro FUNCTION_ARG

   Determine whether a function argument is passed in a register,
   and if so, which register. */

rtx
hexagon_function_arg(
  CUMULATIVE_ARGS cum,
  enum machine_mode mode,
  tree type,
  int named
)
{
  int size;

  /* For the Linux ABI we do not necessarily pass the first unnamed
   parameter on the stack */
  if (hexagon_abi != HEXAGON_ABI_LINUX) {
    if(named == 0){
      return NULL_RTX;
    }
  }

  if(hexagon_must_pass_in_stack(mode, type)){
    return NULL_RTX;
  }

  /* Check for even/odd pairs when size > 32 bits */
  if(type && mode == BLKmode){
    size = int_size_in_bytes(type);
  }
  else {
    size = GET_MODE_SIZE (mode);
  }

  if(size > UNITS_PER_WORD){
    /* If not an even register number, bump the counter */
    if(cum & 1){
      cum++;
    }
  }

  if(cum >= HEXAGON_NUM_ARG_REGS){
    return NULL_RTX;
  }

  return gen_rtx_REG (mode, FIRST_ARG_REGNUM + cum);
}




/* Implements hook TARGET_MUST_PASS_IN_STACK

   True if we do not know how to pass TYPE solely in registers.
   We cannot do so in the following cases:

   - if the type has variable size
   - if the type is marked as addressable (it is required to be constructed into
     the stack)
   - if the type is > 8 bytes. */

static bool
hexagon_must_pass_in_stack(enum machine_mode mode ATTRIBUTE_UNUSED, const_tree type)
{
  return type && TYPE_SIZE (type)
         && (TREE_CODE (TYPE_SIZE (type)) != INTEGER_CST
             || TREE_ADDRESSABLE (type)
             || int_size_in_bytes(type) > 8);
}




/* Implements macro FUNCTION_ARG_ADVANCE

   Advance the cumulative arg pointer past the current argument. */

int
hexagon_function_arg_advance(
  CUMULATIVE_ARGS cum,
  enum machine_mode mode,
  tree type,
  int named
)
{
  int size;

  /* For the Linux ABI we do not necessarily pass the first unnamed
   parameter on the stack */
  if (hexagon_abi != HEXAGON_ABI_LINUX) {
    if(named == 0){
      return cum;
    }
  }

  if(type && mode == BLKmode){
    size = int_size_in_bytes(type);
  }
  else {
    size = GET_MODE_SIZE (mode);
  }

  if(hexagon_must_pass_in_stack(mode, type)){
    struct hexagon_frame_info *info = hexagon_frame_info();
    info->named_arg_stack_words += (size + UNITS_PER_WORD - 1) / UNITS_PER_WORD;

    return cum;
  }

  /* Check for even/odd pairs when size > 32 bits */
  if(size > UNITS_PER_WORD){
    /* If not an even register number, bump the counter */
    if(cum & 1){
      cum++;
    }
  }

  return cum + (size + UNITS_PER_WORD - 1) / UNITS_PER_WORD;
}

/* Implements hook TARGET_VECTOR_MODE_SUPPORTED_P */

static bool
hexagon_vector_mode_supported_p(enum machine_mode mode)
{
  return    mode == V2SImode
         || mode == V4HImode
         || mode == V8QImode
         || mode == V2HImode
         || mode == V4QImode;
}

/* Implements hook TARGET_FUNCTION_ARG_BOUNDARY

   Align stack arguments to the minimum of PARM_BOUNDARY and the
   alignment of the type.*/

static unsigned int
hexagon_function_arg_boundary (enum machine_mode mode, const_tree type)
{

  if (type)
    {
      if (TYPE_ALIGN (type) <= PARM_BOUNDARY)
	return PARM_BOUNDARY;
      else
        return TYPE_ALIGN (type);
    }
  else
    {
      if (GET_MODE_ALIGNMENT (mode) <= PARM_BOUNDARY)
        return PARM_BOUNDARY;
      else
        return GET_MODE_ALIGNMENT (mode);
    }
  gcc_unreachable ();
}




/*-------------------------------------
How Scalar Function Values Are Returned
-------------------------------------*/

/* Implements hook TARGET_FUNCTION_VALUE */

static rtx
hexagon_function_value(
  const_tree ret_type,
  const_tree fn_decl_or_type ATTRIBUTE_UNUSED,
  bool outgoing ATTRIBUTE_UNUSED
)
{
  enum machine_mode mode;
  int unsignedp ATTRIBUTE_UNUSED;

  mode = TYPE_MODE (ret_type);
  mode = promote_mode (ret_type, mode, &unsignedp);
  return gen_rtx_REG (mode, RETURN_VALUE_REGNUM);
}




/*---------------------------
How Large Values Are Returned
---------------------------*/

/* Implements hook TARGET_RETURN_IN_MEMORY

   Return true if function return value returned in memory.
   Return false if it is in a register. */

static bool
hexagon_return_in_memory(const_tree type, const_tree fntype ATTRIBUTE_UNUSED)
{
  enum machine_mode mode;
  HOST_WIDE_INT byte_size;

  mode = TYPE_MODE (type);
  byte_size = GET_MODE_SIZE (mode);
  if(mode == BLKmode){
    byte_size = int_size_in_bytes(type);
  }

  return !IN_RANGE (byte_size, 0, 8);
}




/*---------------------
Function Entry and Exit
---------------------*/

/* Implements hook TARGET_ASM_FUNCTION_PROLOGUE */

static void
hexagon_asm_function_prologue(
  FILE *file,
  HOST_WIDE_INT size ATTRIBUTE_UNUSED
)
{
  struct hexagon_frame_info *frame;
  frame = hexagon_frame_info();
  fprintf(file, "\t%s pretend args regs size (bytes) = "
                "%u\n"
                "\t%s saved LR + FP regs size (bytes) = "
                HOST_WIDE_INT_PRINT_UNSIGNED "\n"
                "\t%s callee saved regs size (bytes) = "
                HOST_WIDE_INT_PRINT_UNSIGNED "\n"
                "\t%s local vars size (bytes) = "
                HOST_WIDE_INT_PRINT_UNSIGNED "\n"
                "\t%s fixed args size (bytes) = "
                HOST_WIDE_INT_PRINT_UNSIGNED "\n",
                ASM_COMMENT_START, frame->pretend_size,
                ASM_COMMENT_START, frame->lrfp_size,
                ASM_COMMENT_START, frame->reg_size,
                ASM_COMMENT_START, frame->var_size,
                ASM_COMMENT_START, frame->args_size
         );

  hexagon_final_pack_insns();
}




/*-------------------
Permitting tail calls
-------------------*/

/* Implements hook TARGET_FUNCTION_OK_FOR_SIBCALL */

static bool
hexagon_function_ok_for_sibcall(tree decl, tree exp ATTRIBUTE_UNUSED)
{
  if(TARGET_LONG_CALLS && !TARGET_V4_FEATURES){
    return false;
  }
  return decl != NULL;
}


/*-----------------------------
Implement the varargs macros; needed only by the Linux ABI.
-----------------------------*/
#if LINUX
/* Taken from arc */
void
hexagon_setup_incoming_varargs (CUMULATIVE_ARGS *cum,
                                enum machine_mode mode,
                                tree type,
                                int *pretend_size,
                                int no_rtl ATTRIBUTE_UNUSED)
{
  int first_anon_arg, align_slop;
  struct hexagon_frame_info *frame =  hexagon_frame_info();

  /* Skip the last named argument.  */
  first_anon_arg = hexagon_function_arg_advance (*cum, mode, type, 1);

  /* Extra slop to keep stack eight byte aligned.  */
  align_slop = (HEXAGON_NUM_ARG_REGS - first_anon_arg) & 1;

  if (first_anon_arg < HEXAGON_NUM_ARG_REGS)
    *pretend_size = (HEXAGON_NUM_ARG_REGS - first_anon_arg
                     + align_slop) * UNITS_PER_WORD;

  frame->pretend_size = *pretend_size;
  frame->first_anon_arg = first_anon_arg;
  frame->named_stack_words = frame->named_arg_stack_words;
}


/* Implement the stdarg va_start macro.
   VALIST is the tree of the va_list variable to initialize.
   NEXTARG is the machine independent notion of the 'next' argument after
   the variable arguments.  */

void
hexagon_va_start (tree valist, rtx nextarg)
{
  rtx va_sav, va_sav_end, va_ovf;
  tree sav, sav_end, ovf, f_sav, f_sav_end, f_ovf;
  unsigned int saved_regs;
  int align_slop;
  struct hexagon_frame_info *frame =  hexagon_frame_info();
  tree recordtype = TREE_TYPE (va_list_type_node);

  saved_regs = ( HEXAGON_NUM_ARG_REGS > frame->first_anon_arg
		 ? HEXAGON_NUM_ARG_REGS - frame->first_anon_arg
		 : 0);

  /* va_start may need to account for the padding to ensure 8-byte alignment
     at the end of the varargs pushed */
  align_slop = saved_regs & 1;
  if (align_slop)
    nextarg = plus_constant (nextarg, UNITS_PER_WORD);

  f_sav = TYPE_FIELDS (recordtype);
  f_sav_end = DECL_CHAIN (f_sav);
  f_ovf = DECL_CHAIN (f_sav_end);

  valist = build_simple_mem_ref (valist);
  TREE_TYPE (valist) = recordtype;

  sav = build3 (COMPONENT_REF, TREE_TYPE (f_sav), unshare_expr (valist),
		f_sav, NULL_TREE);
  sav_end = build3 (COMPONENT_REF, TREE_TYPE (f_sav_end), unshare_expr (valist),
		    f_sav_end, NULL_TREE);
  ovf = build3 (COMPONENT_REF, TREE_TYPE (f_ovf), unshare_expr (valist),
		f_ovf, NULL_TREE);

  va_sav = expand_expr (sav, NULL_RTX, VOIDmode, EXPAND_WRITE);
  va_sav_end = expand_expr (sav_end, NULL_RTX, VOIDmode, EXPAND_WRITE);
  va_ovf = expand_expr (ovf, NULL_RTX, VOIDmode, EXPAND_WRITE);

  convert_move (va_sav, nextarg, 0);

  if (saved_regs)
    nextarg = plus_constant (nextarg, saved_regs * UNITS_PER_WORD);

  convert_move (va_sav_end, nextarg, 0);
  convert_move (va_ovf, nextarg, 0);
}

static tree
hexagon_build_builtin_va_list (void)
{
  tree f_ovf, f_sav, f_sav_end, record, type_decl;

  record = lang_hooks.types.make_type (RECORD_TYPE);
  type_decl = build_decl (BUILTINS_LOCATION,
			  TYPE_DECL, get_identifier ("__va_list_tag"), record);

  f_sav = build_decl (BUILTINS_LOCATION,
		      FIELD_DECL, get_identifier ("reg_save_area"),
		      ptr_type_node);
  f_sav_end = build_decl (BUILTINS_LOCATION,
		      FIELD_DECL, get_identifier ("reg_save_area_end"),
		      ptr_type_node);
  f_ovf = build_decl (BUILTINS_LOCATION,
		      FIELD_DECL, get_identifier ("overflow_arg_area"),
		      ptr_type_node);

  DECL_FIELD_CONTEXT (f_sav) = record;
  DECL_FIELD_CONTEXT (f_sav_end) = record;
  DECL_FIELD_CONTEXT (f_ovf) = record;

  TYPE_STUB_DECL (record) = type_decl;
  TYPE_NAME (record) = type_decl;

  TYPE_FIELDS (record) = f_sav;
  DECL_CHAIN (f_sav) = f_sav_end;
  DECL_CHAIN (f_sav_end) = f_ovf;

  layout_type (record);

  return build_array_type (record, build_index_type (size_zero_node));
}

#endif

/*--------------------------------
Implicit Calls to Library Routines
--------------------------------*/

#if HEXAGON_DINKUMWARE
/* Some Hexagon library functions have "fast" variants, that are
   gated by flag_unsafe_math_optimizations. While other functions
   (e.g sqrt) are only available when flag_unsafe_math_optimizations
   passed. Represent this as a table, which is walked by
   hexagon_init_libfuncs. */

struct hexagon_fast_libfunc {
  optab optab;
  enum machine_mode mode;
  int v5;  /* Helper is needed on Hexagon V5.  */
  const char *name;
  const char *fast_name;
};

static struct hexagon_fast_libfunc hexagon_fast_libfuncs[] = {
  { add_optab, DFmode, 1, "__hexagon_adddf3", "__hexagon_fast_adddf3" },
  { sub_optab, DFmode, 1, "__hexagon_subdf3", "__hexagon_fast_subdf3" },
  { smul_optab, DFmode, 1, "__hexagon_muldf3", "__hexagon_fast_muldf3" },
  { sdiv_optab, DFmode, 1, "__hexagon_divdf3", "__hexagon_fast_divdf3" },
  { neg_optab, DFmode, 1, "__hexagon_negdf2", "__hexagon_fast_negdf2" },
  { lt_optab, DFmode, 0, "__hexagon_ltdf2", "__hexagon_fast_ltdf2" },
  { gt_optab, DFmode, 0, "__hexagon_gtdf2", "__hexagon_fast_gtdf2" },
  { sqrt_optab, DFmode, 1, 0, "__hexagon_fast_sqrt_df" },
  { 0, 0, 0, 0 }
};

/* Implements hook TARGET_INIT_LIBFUNCS */
static void
hexagon_init_libfuncs (void)
{
  set_optab_libfunc(sdiv_optab, SImode, "__hexagon_divsi3");
  set_optab_libfunc(sdiv_optab, DImode, "__hexagon_divdi3");

  set_optab_libfunc(udiv_optab, SImode, "__hexagon_udivsi3");
  set_optab_libfunc(udiv_optab, DImode, "__hexagon_udivdi3");

  set_optab_libfunc(smod_optab, SImode, "__hexagon_modsi3");
  set_optab_libfunc(smod_optab, DImode, "__hexagon_moddi3");

  set_optab_libfunc(umod_optab, SImode, "__hexagon_umodsi3");
  set_optab_libfunc(umod_optab, DImode, "__hexagon_umoddi3");

  set_optab_libfunc(sdivmod_optab, SImode, "__hexagon_divmodsi4");
  set_optab_libfunc(sdivmod_optab, DImode, "__hexagon_divmoddi4");

  set_optab_libfunc(udivmod_optab, SImode, "__hexagon_udivmodsi4");
  set_optab_libfunc(udivmod_optab, DImode, "__hexagon_udivmoddi4");

  /* Hexagon floating point unit doesn't have a division instruction,
     so always initialize floating point division functions regardless
     of the availability of floating point hardware. */
  set_optab_libfunc(sdiv_optab, SFmode, "__hexagon_divsf3");
  set_optab_libfunc(sdiv_optab, DFmode, "__hexagon_divdf3");

  if (TARGET_SOFT_FLOAT || TARGET_V5_FEATURES)
    {
      int i;
      for (i = 0; hexagon_fast_libfuncs[i].optab != 0; i++)
        {
          struct hexagon_fast_libfunc fn = hexagon_fast_libfuncs[i];
	  if (TARGET_V5_FEATURES && !fn.v5)
	    continue;
          if (flag_unsafe_math_optimizations && fn.fast_name)
            set_optab_libfunc (fn.optab, fn.mode, fn.fast_name);
          else if (fn.name)
            set_optab_libfunc (fn.optab, fn.mode, fn.name);
        }
    }

  if (TARGET_SOFT_FLOAT)
    {
      set_optab_libfunc(add_optab, SFmode, "__hexagon_addsf3");

      set_optab_libfunc(sub_optab, SFmode, "__hexagon_subsf3");

      set_optab_libfunc(smul_optab, SFmode, "__hexagon_mulsf3");

      set_optab_libfunc(neg_optab, SFmode, "__hexagon_negsf3");

      set_optab_libfunc(cmp_optab, SFmode, "__hexagon_cmpsf2");
      set_optab_libfunc(cmp_optab, DFmode, "__hexagon_cmpdf2");

      set_optab_libfunc(unord_optab, SFmode, "__hexagon_unordsf2");
      set_optab_libfunc(unord_optab, DFmode, "__hexagon_unorddf2");

      set_optab_libfunc(eq_optab, SFmode, "__hexagon_eqsf2");
      set_optab_libfunc(eq_optab, DFmode, "__hexagon_eqdf2");

      set_optab_libfunc(ne_optab, SFmode, "__hexagon_nesf2");
      set_optab_libfunc(ne_optab, DFmode, "__hexagon_nedf2");

      set_optab_libfunc(ge_optab, SFmode, "__hexagon_gesf2");
      set_optab_libfunc(ge_optab, DFmode, "__hexagon_gedf2");

      set_optab_libfunc(lt_optab, SFmode, "__hexagon_ltsf2");

      set_optab_libfunc(le_optab, SFmode, "__hexagon_lesf2");
      set_optab_libfunc(le_optab, DFmode, "__hexagon_ledf2");

      set_optab_libfunc(gt_optab, SFmode, "__hexagon_gtsf2");

      set_conv_libfunc(sext_optab, DFmode, SFmode, "__hexagon_extendsfdf2");
      set_conv_libfunc(trunc_optab, SFmode, DFmode, "__hexagon_truncdfsf2");

      set_conv_libfunc(sfix_optab, SImode, SFmode, "__hexagon_fixsfsi");
      set_conv_libfunc(sfix_optab, DImode, SFmode, "__hexagon_fixsfdi");

      set_conv_libfunc(sfix_optab, SImode, DFmode, "__hexagon_fixdfsi");
      set_conv_libfunc(sfix_optab, DImode, DFmode, "__hexagon_fixdfdi");

      set_conv_libfunc(ufix_optab, SImode, SFmode, "__hexagon_fixunssfsi");
      set_conv_libfunc(ufix_optab, DImode, SFmode, "__hexagon_fixunssfdi");

      set_conv_libfunc(ufix_optab, SImode, DFmode, "__hexagon_fixunsdfsi");
      set_conv_libfunc(ufix_optab, DImode, DFmode, "__hexagon_fixunsdfdi");

      set_conv_libfunc(sfloat_optab, SFmode, SImode, "__hexagon_floatsisf");
      set_conv_libfunc(sfloat_optab, SFmode, DImode, "__hexagon_floatdisf");

      set_conv_libfunc(sfloat_optab, DFmode, SImode, "__hexagon_floatsidf");
      set_conv_libfunc(sfloat_optab, DFmode, DImode, "__hexagon_floatdidf");

      set_conv_libfunc(ufloat_optab, SFmode, SImode, "__hexagon_floatunsisf");
      set_conv_libfunc(ufloat_optab, SFmode, DImode, "__hexagon_floatundisf");

      set_conv_libfunc(ufloat_optab, DFmode, SImode, "__hexagon_floatunsidf");
      set_conv_libfunc(ufloat_optab, DFmode, DImode, "__hexagon_floatundidf");
    }
}
#endif /* HEXAGON_DINKUMWARE */




/*--------------
Addressing Modes
--------------*/

static bool
hexagon_address_register_p (rtx x, bool strict_p)
{
  int regno;

  if (!REG_P (x))
    return false;

  regno = REGNO (x);

  if (strict_p)
    return REGNO_OK_FOR_BASE_P (regno);

  return regno < 32
         || regno >= FIRST_PSEUDO_REGISTER
	 || regno == FRAME_POINTER_REGNUM
	 || regno == ARG_POINTER_REGNUM;
}

/* Indirect address with signed immediate offset of -BITS bits (when BITS is
   negative), or an unsigned offset of BITS bits (when BITS is positive).
   32-bit offsets are permitted when EXTENDED is true.  */

static bool
hexagon_ind_with_offset_address_p (enum machine_mode mode, rtx x, int bits,
				   bool extended, bool strict_p)
{
  HOST_WIDE_INT offset, range, min_offset, max_offset;
  unsigned HOST_WIDE_INT lowbit_mask = GET_MODE_SIZE (mode) - 1;

  if (bits < 0)
    {
      bits = -bits;
      range = (HOST_WIDE_INT) 1 << (bits - 1);
      min_offset = -range * GET_MODE_SIZE (mode);
      max_offset = range * GET_MODE_SIZE (mode);
    }
  else
    {
      min_offset = 0;
      max_offset = ((HOST_WIDE_INT) 1 << bits) * GET_MODE_SIZE (mode);
    }

  if (GET_CODE (x) == PLUS
      && hexagon_address_register_p (XEXP (x, 0), strict_p)
      && GET_CODE (XEXP (x, 1)) == CONST_INT)
    {
      offset = INTVAL (XEXP (x, 1));

      if (TARGET_V4_FEATURES && extended)
        return (((unsigned HOST_WIDE_INT) offset) & lowbit_mask) == 0;
      else
	return offset >= min_offset && offset < max_offset
	       && (((unsigned HOST_WIDE_INT) offset) & lowbit_mask) == 0;
    }

  return false;
}

/* Indirect address with register offset (i.e. base+index addressing).  */

static bool
hexagon_ind_with_reg_offset_address_p (rtx x, bool strict_p)
{
  return TARGET_V4_FEATURES && GET_CODE (x) == PLUS
	 && hexagon_address_register_p (XEXP (x, 0), strict_p)
	 && hexagon_address_register_p (XEXP (x, 1), strict_p);
}

/* Indirect address with scaled register offset.
   Note: assumes MULT is canonical, not ASHIFT.  */

static bool
hexagon_ind_with_scaled_reg_offset_address_p (rtx x, bool strict_p)
{
  if (TARGET_V4_FEATURES && GET_CODE (x) == PLUS
      && GET_CODE (XEXP (x, 0)) == MULT
      && hexagon_address_register_p (XEXP (XEXP (x, 0), 0), strict_p)
      && GET_CODE (XEXP (XEXP (x, 0), 1)) == CONST_INT
      && hexagon_address_register_p (XEXP (x, 1), strict_p))
    {
      HOST_WIDE_INT scale = INTVAL (XEXP (XEXP (x, 0), 1));

      return scale == 1 || scale == 2 || scale == 4 || scale == 8;
    }

  return false;
}

/* Indirect address with post-increment (or post-decrement) immediate,
   or post-modify.  */

static bool
hexagon_post_modify_address_p (enum machine_mode mode, rtx x, bool strict_p)
{
  /* Indirect with post-increment (or post-decrement) immediate.  */
  if ((GET_CODE (x) == POST_INC || GET_CODE (x) == POST_DEC)
      && hexagon_address_register_p (XEXP (x, 0), strict_p))
    return true;

  /* Indirect with post-modify (same addressing mode in hardware as
     post-increment/post-decrement, but with a non-size-of-access offset).  */
  if (GET_CODE (x) == POST_MODIFY
      && hexagon_address_register_p (XEXP (x, 0), strict_p)
      && GET_CODE (XEXP (x, 1)) == PLUS
      && rtx_equal_p (XEXP (x, 0), XEXP (XEXP (x, 1), 0))
      && GET_CODE (XEXP (XEXP (x, 1), 1)) == CONST_INT)
    {
      HOST_WIDE_INT offset = INTVAL (XEXP (XEXP (x, 1), 1));
      HOST_WIDE_INT min_offset, max_offset;
      unsigned HOST_WIDE_INT lowbit_mask = GET_MODE_SIZE (mode) - 1;

      min_offset = -8 * GET_MODE_SIZE (mode);
      max_offset = 8 * GET_MODE_SIZE (mode);

      return offset >= min_offset && offset < max_offset
	     && (((unsigned HOST_WIDE_INT) offset) & lowbit_mask) == 0;
    }

  return false;
}

/* Absolute address.  */

static bool
hexagon_absolute_address_p (enum machine_mode mode, rtx x, bool extended)
{
  unsigned HOST_WIDE_INT offset;
  unsigned HOST_WIDE_INT lowbit_mask = GET_MODE_SIZE (mode) - 1;

  if (TARGET_V4_FEATURES && extended && CONSTANT_P (x))
    {
      /* If we have a simple const_int, rather than a symbolic address for
         instance, ensure that we have a naturally-aligned offset.  */
      if (GET_CODE (x) == CONST_INT)
        {
          offset = INTVAL (x);
	  return (offset & lowbit_mask) == 0;
	}

      if (!flag_pic && !hexagon_tls_referenced_p (x))
	return true;
    }

  return false;
}

/* Absolute address with register or scaled register offset.  */

static bool
hexagon_absolute_with_scaled_reg_address_p (enum machine_mode mode, rtx x,
					    bool extended, bool strict_p)
{
  unsigned HOST_WIDE_INT lowbit_mask = GET_MODE_SIZE (mode) - 1;

  /* Absolute with register offset.  */
  if (TARGET_V4_FEATURES && extended && GET_CODE (x) == PLUS
      && hexagon_address_register_p (XEXP (x, 0), strict_p)
      && hexagon_absolute_address_p (mode, XEXP (x, 1), extended))
    {
      unsigned HOST_WIDE_INT offset;

      offset = INTVAL (XEXP (x, 1));
      return (offset & lowbit_mask) == 0;
    }

  /* Absolute with scaled register offset.
     Note: assumes MULT is canonical, not ASHIFT.  */
  if (TARGET_V4_FEATURES && extended && GET_CODE (x) == PLUS
      && GET_CODE (XEXP (x, 0)) == MULT
      && hexagon_address_register_p (XEXP (XEXP (x, 0), 0), strict_p)
      && GET_CODE (XEXP (XEXP (x, 0), 1)) == CONST_INT
      && hexagon_absolute_address_p (mode, XEXP (x, 1), extended))
    {
      HOST_WIDE_INT scale = INTVAL (XEXP (XEXP (x, 0), 1));
      unsigned HOST_WIDE_INT offset = INTVAL (XEXP (x, 1));

      return (scale == 1 || scale == 2 || scale == 4 || scale == 8)
	     && (offset & lowbit_mask) == 0;
    }

  return false;
}

/* True for valid addresses for normal loads/stores, with or without
   EXTENDED.  Don't call this function with EXTENDED true on pre-V4
   architectures.  */

static bool
hexagon_address_p (enum machine_mode mode, rtx x, bool extended, bool strict_p)
{
  gcc_assert (TARGET_V4_FEATURES || !extended);

  if (hexagon_address_register_p (x, strict_p))
    return true;

  /* Indirect with offset.  */
  if (hexagon_ind_with_offset_address_p (mode, x, -11, extended, strict_p))
    return true;

  /* Indirect with register offset.  */
  if (hexagon_ind_with_reg_offset_address_p (x, strict_p))
    return true;

  /* Indirect with scaled register offset.  */
  if (hexagon_ind_with_scaled_reg_offset_address_p (x, strict_p))
     return true;

  /* Post-modified address.  */
  if (hexagon_post_modify_address_p (mode, x, strict_p))
    return true;

  /* GP-relative address.  */
  if (sdata_symbolic_operand (x, Pmode))
    return true;

  /* Absolute.  */
  if (hexagon_absolute_address_p (mode, x, extended))
    return true;

  /* Absolute plus register/scaled register address.  */
  if (hexagon_absolute_with_scaled_reg_address_p (mode, x, extended, strict_p))
    return true;

  /* The following addressing modes are not supported:
     - Indirect with post-modify register (memXX(Rx++Mu))
     - Circular with auto-increment immediate
     - Circular with auto-increment register
     - Bit-reversed with auto-increment register.  */

  return false;
}

/* True for valid mems for normal loads/stores, without EXTENDED bits.  */

bool
hexagon_noext_mem_operand (enum machine_mode mode, rtx x, bool strict_p)
{
  gcc_assert (GET_CODE (x) == MEM);

  return hexagon_address_p (mode, XEXP (x, 0), false, strict_p);
}

/* Implement TARGET_LEGITIMATE_ADDRESS_P hook.  */

bool
hexagon_legitimate_address_p (enum machine_mode mode, rtx x, bool strict_p)
{
  /* Only permit extended immediate offsets for architecture v4.  */
  return hexagon_address_p (mode, x, TARGET_V4_FEATURES, strict_p);
}

/* A mem suitable for conditional load and store instructions, with or
   without an immediate extender.  */

bool
hexagon_cond_mem_operand (enum machine_mode mode, rtx x, bool extended)
{
  gcc_assert (GET_CODE (x) == MEM);

  if (extended && !TARGET_V4_FEATURES)
    return false;

  x = XEXP (x, 0);

  /* Absolute. */
  if (hexagon_absolute_address_p (mode, x, extended))
    return true;

  /* Indirect.  */
  if (hexagon_address_register_p (x, false))
    return true;

  /* Indirect with offset.  */
  if (hexagon_ind_with_offset_address_p (mode, x, 6, extended, false))
    return true;

  /* Indirect with post-increment (or post-decrement), or post-modify.  */
  if (hexagon_post_modify_address_p (mode, x, false))
    return true;

  /* Indirect with register offset.  */
  if (hexagon_ind_with_reg_offset_address_p (x, false))
    return true;

  /* Indirect with scaled register offset.  */
  if (hexagon_ind_with_scaled_reg_offset_address_p (x, false))
    return true;

  return false;
}

/* An mem suitable for store-immediate instructions (and also the conditional
   versions of same).  */

bool
hexagon_storeimm_mem_operand (enum machine_mode mode, rtx x)
{
  gcc_assert (GET_CODE (x) == MEM);

  x = XEXP (x, 0);

  if (!TARGET_V4_FEATURES)
    return false;

  if (hexagon_address_register_p (x, false))
    return true;

  /* Store-immediate instructions can have an extension, but it applies to the
     immediate, not the offset, so use FALSE for the extension here.  */
  if (hexagon_ind_with_offset_address_p (mode, x, 6, false, false))
    return true;

  return false;
}

/* A mem suitable for read-modify-write operations.  */

bool
hexagon_rmw_mem_operand (enum machine_mode mode, rtx x, bool extended)
{
  gcc_assert (GET_CODE (x) == MEM);

  x = XEXP (x, 0);

  if (!TARGET_MEMOPS || !TARGET_V4_FEATURES)
    return false;

  if (hexagon_address_register_p (x, false))
    return true;

  if (hexagon_ind_with_offset_address_p (mode, x, 6, extended, false))
    return true;

  return false;
}

/* A mem suitable for sub-instructions with an unsigned BITS-sized offset.  */

bool
hexagon_subinst_mem_operand (enum machine_mode mode, rtx x, int bits)
{
  gcc_assert (GET_CODE (x) == MEM);

  x = XEXP (x, 0);

  if (hexagon_address_register_p (x, false))
    return true;

  if (hexagon_ind_with_offset_address_p (mode, x, bits, false, false))
    return true;

  return false;
}

/* A mem suitable for sub-instructions with an unsigned BITS-sized offset.  */

bool
hexagon_subinst_stack_mem_operand (enum machine_mode mode, rtx x, int bits)
{
  gcc_assert (GET_CODE (x) == MEM);

  x = XEXP (x, 0);

  /* We need to check for the physical stack pointer register here.  Not
     entirely sure if this is correct though (maybe we should return true?).  */
  if (!reload_completed)
    return false;

  if (REG_P (x) && REGNO (x) == STACK_POINTER_REGNUM)
    return true;

  if (GET_CODE (x) == PLUS && REG_P (XEXP (x, 0))
      && REGNO (XEXP (x, 0)) == STACK_POINTER_REGNUM
      && hexagon_ind_with_offset_address_p (mode, x, bits, false, false))
    return true;

  return false;
}

/* True if second (SImode) address follows directly on from first address.  */

bool
hexagon_address_adjacent_p (rtx first, rtx second)
{
  HOST_WIDE_INT offset;
  rtx first_reg;

  if (!MEM_P (first) || !MEM_P (second))
    return false;
  
  first = XEXP (first, 0);
  second = XEXP (second, 0);
  
  if (REG_P (first))
    {
      first_reg = first;
      offset = 0;
    }
  else if (GET_CODE (first) == PLUS && REG_P (XEXP (first, 0))
	   && GET_CODE (XEXP (first, 1)) == CONST_INT)
    {
      first_reg = XEXP (first, 0);
      offset = INTVAL (XEXP (first, 1));
    }
  else
    return false;
  
  if (GET_CODE (second) == PLUS && REG_P (XEXP (second, 0))
      && GET_CODE (XEXP (second, 1)) == CONST_INT)
    return rtx_equal_p (first_reg, XEXP (second, 0))
	   && INTVAL (XEXP (second, 1)) == offset + 4;
  
  return false;
}

/* Make some adjustments to the operands of binary operation: force references
   to invalid immediates into a register, and attempt to alter addresses to
   work better with read-modify-write instructions, where appropriate.  */

void
hexagon_expand_binary_operator (enum rtx_code code, enum machine_mode mode,
				rtx operands[], int immop,
				int (*regop_imm_predicate)
				  (rtx, enum machine_mode),
				int (*memop_imm_predicate)
				  (rtx, enum machine_mode),
				bool commutative)
{
  bool is_memop = false;
  rtx dst = operands[0], op;
  enum machine_mode orig_mode = mode;

  /* We can't do anything after reload.  */
  if (reload_completed)
    {
      op = gen_rtx_SET (VOIDmode, operands[0],
			gen_rtx_fmt_ee (code, mode, operands[1],
					operands[2]));
      emit_insn (op);
      return;
    }

  /* We can't have two source memory operands.  */
  if (MEM_P (operands[1]) && MEM_P (operands[2]))
    operands[2] = force_reg (mode, operands[2]);

  if (TARGET_MEMOPS && TARGET_V4_FEATURES)
    {
      /* If we have a commutative operation, and memory operands for operands 0
	 and 2, try to swap operands 1 and 2 in case the memory addresses turn
	 out to be equivalent.  */
      if (commutative && MEM_P (operands[0]) && !MEM_P (operands[1])
	  && MEM_P (operands[2]))
	{
	  rtx tmp = operands[2];
	  operands[2] = operands[1];
	  operands[1] = tmp;
	}
      
      if (MEM_P (operands[0])
	  && gr_reg_or_rmw_mem_operand (operands[0], mode)
	  && rtx_equal_p (operands[0], operands[1]))
	is_memop = true;
      /* Check if operands[0] is equal to operands[1], and rewrite it if it's
	 something unsupported by read-modify-write memops.  Note that this
	 doesn't catch all cases: sometimes operands 0 and 1 look different,
	 but nevertheless represent the same address.  We try to handle that
	 below.  */
      else if (MEM_P (operands[0])
	       && rtx_equal_p (operands[0], operands[1])
	       && !gr_reg_or_rmw_mem_operand (operands[0], mode))
	{
	  rtx addr = force_reg (Pmode, XEXP (operands[0], 0));

	  dst = operands[0] = operands[1]
	    = replace_equiv_address (operands[0], addr);

	  is_memop = true;
	}
      else if (MEM_P (operands[0]) && MEM_P (operands[1]))
	{
	  /* We have MEMs for operands 0 and 1: try to adjust addresses to
	     those accepted by read-modify-write insns.  This makes the
	     predicates match for the insns in question, but not necessarily
	     the constraints: IRA/reload can handle the latter, however.  */
	  if (!gr_reg_or_rmw_mem_operand (operands[0], mode))
	    {
	      rtx addr = force_reg (Pmode, XEXP (operands[0], 0));
	      dst = operands[0] = replace_equiv_address (operands[0], addr);
	    }
	  
	  if (!gr_reg_or_rmw_mem_operand (operands[1], mode))
	    {
	      rtx addr = force_reg (Pmode, XEXP (operands[1], 0));
	      operands[1] = replace_equiv_address (operands[1], addr);
	    }

	  is_memop = true;
	}
    }

  /* If we don't have operands suitable for a read-modify-write memop, or memops
     are not enabled, do everything in registers.  */
  if (!is_memop)
    {
      if (MEM_P (operands[1]))
	operands[1] = force_reg (mode, operands[1]);

      if (MEM_P (operands[2]))
	operands[2] = force_reg (mode, operands[2]);

      if (MEM_P (operands[0]))
	dst = gen_reg_rtx (mode);
    }

  /* Force invalid immediates into a register.  */
  if (immediate_operand (operands[immop], mode)
      && (is_memop && !memop_imm_predicate (operands[immop], mode))
          || (!TARGET_V4_FEATURES
	      && !regop_imm_predicate (operands[immop], mode)))
    operands[immop] = force_reg (mode, operands[immop]);

  /* Do non-memop operations in SImode.  */
  if (!is_memop && mode != SImode)
    {
      mode = SImode;
      operands[1] = gen_lowpart (mode, operands[1]);
      operands[2] = gen_lowpart (mode, operands[2]);
      dst = gen_reg_rtx (mode);
    }

  op = gen_rtx_SET (VOIDmode, dst,
		    gen_rtx_fmt_ee (code, mode, operands[1],
				    operands[2]));
  emit_insn (op);

  if (dst != operands[0])
    {
      if (mode == orig_mode)
	emit_move_insn (operands[0], dst);
      else
	emit_move_insn (operands[0], gen_lowpart (orig_mode, dst));
    }
}

/*
 ---------------------------------------------
    Begin Position Independent Code Support
 ---------------------------------------------
*/

/*
 *  Helper function for hexagon_legitimate_pic_operand_p()
 *  Return TRUE if X references a symbol
 *  Simple proposition for now; can be expanded
 */
int
symbol_mentioned_p (rtx x)
{
  return (GET_CODE (x) == SYMBOL_REF);
}


/*
 *  Helper function for hexagon_legitimate_pic_operand_p()
 *  Return TRUE if X references a label
 *  Simple proposition for now; can be expanded
 */
int
label_mentioned_p (rtx x)
{
  return (GET_CODE (x) == LABEL_REF);
}


/*
 * Implements macro LEGITIMATE_PIC_OPERAND_P
 */
bool
hexagon_legitimate_pic_operand_p(rtx operand)
{
  /* Taken from the ARM code.  It recogonizes the case when a pic symbol
   * is used within an expression, which we disallow because the symbol
   * will not be generated with a @GOT/GOTOFF extension.
   */
  if (symbol_mentioned_p (operand)
	 || (GET_CODE (operand) == CONST
	     && GET_CODE (XEXP (operand, 0)) == PLUS
	     && symbol_mentioned_p (XEXP (XEXP (operand, 0), 0)))) {
    return 0;
  }

  return (!(symbol_mentioned_p (operand)
	    || label_mentioned_p (operand)
	    || (GET_CODE (operand) == SYMBOL_REF
		&& CONSTANT_POOL_ADDRESS_P (operand)
		&& (symbol_mentioned_p (get_pool_constant (operand))
		    || label_mentioned_p (get_pool_constant (operand))))));
}


/*
 * Mark that the compiled function accesses the PIC register. This implies that
 * code must be added in the prologue to set up the PIC register
 */
void
require_pic_register (void)
{
  /* Set gcc global flag */
  /* Do not mark that the function access the pic register when called as
     part of the cost-estimation process.  Otherwise, we'll generate the pic
     set-up code when we may not need it.  Arm also does this. */
  if (current_ir_type () != IR_GIMPLE || currently_expanding_to_rtl)
    {
      crtl->uses_pic_offset_table = 1;

      if (!cfun->machine->pic_reg)
        cfun->machine->pic_reg = gen_rtx_REG (Pmode, PIC_REGNUM);
    }
}

/*
 * Make that compiled function access the TLS register
 */

void
require_tls_register (void)
{
  struct hexagon_frame_info *frame;

  frame = &cfun->machine->frame_info;

  frame->tls_offset_table_rtx = gen_rtx_REG (SImode, TLS_REGNUM);
  frame->tls_set = 1;

  /* Burn r25 as TLS register.*/
  fixed_regs[TLS_REGNUM] = 1;
  call_used_regs[TLS_REGNUM] = 1;
}

/*
  There must be a better way to do this. We need a register to use as a temporary
  while saving and restoring gp. We cannot create a new virtual register since
  the save and restore code is invoked after reload has finished.
  The temporary register must be a caller-saved register and not an argument
  register
*/
#define NON_ARG_CALLER_SAVE_REGISTER_1 10

/*
 * Setup the pic register in the function prologue.  This function
 * assumes the pic option has been set.
 */
static void
hexagon_load_pic_register (void) {

  rtx label;

  if (crtl->uses_pic_offset_table == 0)
    return;

  gcc_assert (flag_pic);

  /* Materialize GOT base for PIC */

  if(TARGET_V4_FEATURES){
    emit_insn(gen_compute_got_base_v4(pic_offset_table_rtx));
  }
  else {
    rtx temp_reg;
    label = gen_label_rtx();
    temp_reg = gen_rtx_REG (SImode, NON_ARG_CALLER_SAVE_REGISTER_1);
    /* emit insn sequence for computing the GOT base */
    emit_insn(gen_compute_got_base(pic_offset_table_rtx, label));
    emit_insn(gen_pic_movsi_hi_gotoff(temp_reg,
                                      gen_rtx_LABEL_REF(SImode, label)));
    emit_insn(gen_pic_movsi_lo_gotoff(temp_reg, temp_reg,
                                      gen_rtx_LABEL_REF(SImode, label)));
    emit_insn(gen_subsi3(pic_offset_table_rtx, pic_offset_table_rtx, temp_reg));
  }

  emit_use (cfun->machine->pic_reg);
}


/*
 * Setup the tls register in the function prologue.  This function
 * assumes the tls_set in the frame info has been set.
 */
static void
hexagon_load_tls_register (void) {

  struct hexagon_frame_info *frame;
  /* emit insn tls r25 = ugp */
  frame = &cfun->machine->frame_info;
  emit_insn (gen_compute_tls_base (frame->tls_offset_table_rtx));
}


/*
 * Helper function for hexagon_legitimize_address() for PIC compilation
 * Make addresses conform to PIC mode
 */
rtx
legitimize_pic_address(rtx orig, enum machine_mode mode ATTRIBUTE_UNUSED,
                       rtx reg)
{

  if (GET_CODE (orig) == SYMBOL_REF
      || GET_CODE (orig) == LABEL_REF)
    {
      rtx pic_ref, address;
      rtx insn;

      /* module_owns: set if this module owns the data (orig) */
      int module_owns = 0;
      int subregs = 0;

      /* Record that we generate a pic reference  */
      require_pic_register ();

      /* TODO: why do we need the following 2 if clauses? */
      if (reg == 0)
	{
	  gcc_assert (can_create_pseudo_p());
	  reg = gen_reg_rtx (Pmode);

	  subregs = 1;
	}

      if (subregs)
	{
	  address = gen_reg_rtx (Pmode);
	}
      else
	{
	  address = reg;
	}


      /*
       * Normalize the address to a PIC address via @GOT or @GOTOFF
       */

      /* Check if the module owns this data. The module always owns a label */
      module_owns = (GET_CODE (orig) == LABEL_REF) || SYMBOL_REF_LOCAL_P(orig);

      if(TARGET_V4_FEATURES)
        {
          if(module_owns)
            {
              insn = emit_insn (gen_pic_movsi_pcrel(reg, orig));
            }
          else
            {
              insn = emit_insn (gen_pic_movsi_got_v4(reg, pic_offset_table_rtx, orig));
            }
        }
      else
        {
          /* 1. Load GOT-relative address */
          if (module_owns) {
            /* rx.h = #HI(addr@GOTOFF); rx.l = #LO(addr@GOTOFF) */
            emit_insn (gen_pic_movsi_hi_gotoff(address, orig));
            emit_insn (gen_pic_movsi_lo_gotoff(address, address, orig));
            /* address = CONST32(#address@GOTOFF) */
          }
          else {
            if (flag_pic == 1) {
              /* rx = #addr@GOT */
              emit_insn (gen_pic_movsi(address, gen_rtx_CONST(HImode, orig)));
            }
            else {
              /* rx.h = #HI(addr@GOT); rx.l = #LO(addr@GOT) */
              emit_insn(gen_pic_movsi_hi_got(address, orig));
              emit_insn(gen_pic_movsi_lo_got(address, address, orig));
              /* address = CONST32(#address@GOT) */
            }
          }

          /* 2. Add the absolute address of the GOT to the GOT-relative address */
          /* reg = add(address, pic_reg) */
          /*      emit_insn(gen_addsi3(reg, pic_offset_table_rtx, address)); */
          if (module_owns) {
            /* If the module owns this data, then we are done. reg holds the
               gotoff-adjusted address */
            pic_ref = gen_rtx_PLUS(Pmode, pic_offset_table_rtx, address);
          }
          else {
            /* reg = memw(address) */
            /* emit_move_insn(reg, gen_rtx_MEM(SImode, reg)); */
            pic_ref = gen_const_mem (Pmode,
                                     gen_rtx_PLUS (Pmode, pic_offset_table_rtx,
                                                   address));
          }
          insn = emit_move_insn (reg, pic_ref);
          set_unique_reg_note (insn, REG_EQUAL, orig);
        }

      return reg;
    }
  else if (CONSTANT_P(orig))
    {
     /* Adopted from arm code */
     if (GET_CODE (orig) == CONST)
       {
         if (GET_CODE (XEXP (orig, 0)) == PLUS)
	   {
	     rtx base, offset;
	     base = legitimize_pic_address (XEXP (XEXP (orig, 0), 0), Pmode, reg);
	     offset = legitimize_pic_address (XEXP (XEXP (orig, 0), 1), Pmode,
					      base == reg ? 0 : reg);
             if (reg == 0)
               {
                 gcc_assert (can_create_pseudo_p());
                 reg = gen_reg_rtx (Pmode);
               }
	     emit_insn(gen_addsi3(reg, base, offset));
             return reg;
	   }
       }
    }

  return orig;
}


/*
 * Implements macro LEGITIMIZE_ADDRESS
 * Disabled for now. We may need to enable this
 */
rtx
hexagon_legitimize_address (rtx x, rtx old_x, enum machine_mode mode)
{
  if (hexagon_tls_symbol_p(x))
  {
    return legitimize_tls_address(x);
  }

  if (flag_pic)
    {
      rtx pic_x = legitimize_pic_address(old_x, mode, NULL_RTX);
      if (old_x != pic_x)
	{
	  x = pic_x;
	}
    }
  return(x);
}


/*
 ---------------------------------------------
    End Position Independent Code Support
 ---------------------------------------------
*/


/*
 ---------------------------------------------
    Begin Thread Local Storage Support
 ---------------------------------------------
*/

/* Return a valid memory address corresponding to invalid TLS memory
   address X.  */

rtx
legitimize_tls_address (rtx x)
{
  rtx address, ret, eqv, seq, tmp;
  enum tls_model model = SYMBOL_REF_TLS_MODEL (x);

  address = gen_reg_rtx (Pmode);

  switch (model)
  {
    case TLS_MODEL_GLOBAL_DYNAMIC:
      {
        emit_insn (gen_load_tls_hi (address, x));
        emit_insn (gen_load_tls_lo (address, address, x));
	if (flag_pic)
 	  {
 	    require_pic_register ();
 	    emit_insn (gen_addsi3 (address, address, pic_offset_table_rtx));
 	  }
 	else
 	  {
 	    rtx got = gen_rtx_SYMBOL_REF (Pmode, "_GLOBAL_OFFSET_TABLE_");
 	    emit_insn (gen_addsi3 (address, address, got));
 	  }

	start_sequence ();
	tmp = emit_library_call_value (x, NULL_RTX, LCT_CONST, Pmode, 1,
				       address, Pmode);
	seq = get_insns ();
	end_sequence ();

	ret = gen_reg_rtx (Pmode);
	emit_libcall_block (seq, ret, tmp, x);

        return ret;
      }

    case TLS_MODEL_LOCAL_DYNAMIC:
      {
	emit_insn (gen_load_tls_hi (address, x));
	emit_insn (gen_load_tls_lo (address, address, x));
	if (flag_pic)
  	  {
	    require_pic_register ();
	    emit_insn (gen_addsi3 (address, address, pic_offset_table_rtx));
  	  }
	else
	  {
	    rtx got = gen_rtx_SYMBOL_REF (Pmode, "_GLOBAL_OFFSET_TABLE_");
	    emit_insn (gen_addsi3 (address, address, got));
	  }
  
	eqv = gen_rtx_UNSPEC (Pmode, gen_rtvec (1, const0_rtx),
			      UNSPEC_LD_TLS_INDEX);
  
	start_sequence ();
	tmp = emit_library_call_value (x, NULL_RTX, LCT_CONST, Pmode, 1,
  				       address, Pmode);
	seq = get_insns ();
	end_sequence ();

	ret = gen_reg_rtx (Pmode);
	emit_libcall_block (seq, ret, tmp, eqv);

        address = gen_reg_rtx (Pmode);
        emit_insn (gen_load_tls_tprel_hi (address, x));
        emit_insn (gen_load_tls_tprel_lo (address, address, x));
        emit_insn (gen_addsi3 (address, address, ret));
        return address;
      }

    case TLS_MODEL_INITIAL_EXEC:
      {
        struct hexagon_frame_info *frame = &cfun->machine->frame_info;

        if (!frame->tls_set)
        {
          require_tls_register ();
        }

        emit_insn (gen_load_tls_hi (address, x));
        emit_insn (gen_load_tls_lo (address, address, x));
        if (flag_pic) {
          require_pic_register ();
          emit_insn (gen_addsi3 (address, address, pic_offset_table_rtx));
        }
        emit_move_insn (address, gen_const_mem (SImode, address));
        emit_insn (gen_addsi3 (address, address, frame->tls_offset_table_rtx));

        return address;
      }

    case TLS_MODEL_LOCAL_EXEC:
      {
        struct hexagon_frame_info *frame = &cfun->machine->frame_info;
        if (!frame->tls_set)
        {
          require_tls_register ();
        }

        emit_insn (gen_load_tls_hi (address, x));
        emit_insn (gen_load_tls_lo (address, address, x));
        emit_insn (gen_addsi3 (address, address, frame->tls_offset_table_rtx));

        return address;
      }
    default:
      abort();
  }

}

/* Implement TARGET_ASM_EXTERNAL_LIBCALL hook.  */
 
static void
hexagon_asm_external_libcall  (rtx fun)
{
  if (SYMBOL_REF_TLS_MODEL (fun) != TLS_MODEL_NONE)
    {
      /* If this is a call to a TLS symbol foo, then this is a hidden call to
	 __tls_get_addr. Don't do anything for foo as an external libcall, it's
	 already declared as a variable.  */
      return;
    }

  default_external_libcall (fun);
}

/* Returns true if X is a TLS symbol.  */

bool
hexagon_tls_symbol_p (rtx x)
{
  if (!TARGET_HAVE_TLS)
    return false;

  if (GET_CODE (x) != SYMBOL_REF)
    return false;

  return SYMBOL_REF_TLS_MODEL (x) != 0;
}


static int
hexagon_tls_operand_p_1 (rtx *x, void *data ATTRIBUTE_UNUSED)
{
  if (GET_CODE (*x) == SYMBOL_REF)
    return (SYMBOL_REF_TLS_MODEL (*x) != 0 &&
            SYMBOL_REF_TLS_MODEL (*x) != 1);

  /* Don't recurse into UNSPEC_TLS or other UNPSEC mentioned below,
     looking for TLS symbols; these are TLS offsets,
     not real symbol references.  */
  if (GET_CODE (*x) == CALL_INSN ||
       (GET_CODE (*x) == UNSPEC &&
          (XINT (*x, 1) == UNSPEC_TLS            ||
           XINT (*x, 1) == UNSPEC_TPREL_TLS      ||
           XINT (*x, 1) == UNSPEC_PIC_SYM_GOTOFF ||
           XINT (*x, 1) == UNSPEC_PIC_SYM_GOT )))
    return -1;

  return 0;
}

bool
hexagon_tls_referenced_p (rtx x)
{
  if (!TARGET_HAVE_TLS)
    return false;

  return for_each_rtx(&x, hexagon_tls_operand_p_1, NULL);
}

/*
 ---------------------------------------------
    End Thread Local Storage Support
 ---------------------------------------------
*/

unsigned HOST_WIDE_INT
sdata_symbolic_operand_size (rtx op)
{
  unsigned HOST_WIDE_INT size = 16;
  tree t;

  switch(GET_CODE (op)){
    case CONST:
      op = XEXP (op, 0);
      if(!(GET_CODE (op) == PLUS
           && GET_CODE (XEXP (op, 0)) == SYMBOL_REF
           && GET_CODE (XEXP (op, 1)) == CONST_INT)){
        return size;
      }
      op = XEXP (op, 0);
      /* FALL THROUGH */
    case SYMBOL_REF:
      if(CONSTANT_POOL_ADDRESS_P (op))
        size = GET_MODE_SIZE (get_pool_mode(op));
      else if (!SYMBOL_REF_SMALL_P (op))
        return size;
      else {
        t = SYMBOL_REF_DECL (op);
        if (DECL_P (t))
          t = DECL_SIZE_UNIT (t);
        else
          t = TYPE_SIZE_UNIT (TREE_TYPE (t));

        if (t && host_integerp(t, 0))
          {
            size = tree_low_cst(t, 0);
          }
      }
    default:
      return size;
  }
}


static GTY(()) tree hexagon_builtin_mask_for_load;

/* Implements hook TARGET_VECTORIZE_BUILTIN_MASK_FOR_LOAD */

static tree
hexagon_vectorize_builtin_mask_for_load(void)
{
  return hexagon_builtin_mask_for_load;
}




/*-------------------------------------
Describing Relative Costs of Operations
-------------------------------------*/

/* Implements hook STORE_BY_PIECES_P */

bool
hexagon_store_by_pieces_p(unsigned HOST_WIDE_INT size, unsigned int alignment)
{
  unsigned HOST_WIDE_INT ninsns;
  unsigned int store_width = MIN (MAX (alignment, 1), 4);

  ninsns = size / store_width * (TARGET_V4_FEATURES ? 2 : 3);
  size %= store_width;
  if(optimize_size){
    while(size){
      store_width /= 2;
      ninsns += size / store_width * 2;
      size %= store_width;
    }
    return ninsns <= 3;
  }
  else {
    return ninsns <= 16;
  }
}

/* The GCC internals manual suggests that COSTS_N_INSNS (1) is the cost of
   a "fast" instruction, but that's likely to be too small.  */
#define FAST_INSN_COST COSTS_N_INSNS(2)
#define SLOW_INSN_COST COSTS_N_INSNS(5)

/* An immediate constant is one that can be encoded directly in the insn.  Like
   a register operand, it's essentially "free".  */
#define IMMED_CONSTANT_COST 0

/* A constant that requires an extender requires extra code size equal
   to one fast insn.  It might seem like a constant extender ought to have
   negligible impact on speed, but experimentally giving the same penalty
   for speed as for size produces the best results.  */
#define CONSTANT_EXTENDER_COST(SPEED) \
  ((SPEED) ? FAST_INSN_COST : FAST_INSN_COST)

/* We really want to discourage constant extenders in addresses.  It's better
   to force the address to a reg, which also allows CSE to find a common base
   reg for accesses to multiple slots/array elements in the same object.  */
#define ADDRESS_CONSTANT_EXTENDER_COST(SPEED) \
  (CONSTANT_EXTENDER_COST (SPEED) * 2)

/* Base cost of a memory operation.  You might think that the size cost
   should be smaller than the speed cost (since the speed cost ought
   to take into consideration memory latency too), but experiment
   shows that this number is about right for both size and speed.  Bumping
   it up further helped some benchmarks but caused others to regress.  */
#define MEMORY_COST(SPEED) \
  ((SPEED) ? COSTS_N_INSNS (2) : COSTS_N_INSNS (2))

/* Cost of an RTX operator implemented as a library call.  These numbers
   are somewhat arbitrary, but the idea is that libcalls are very slow
   but only require a moderate size hit.  */
#define LIBCALL_COST(SPEED) \
  ((SPEED) ? COSTS_N_INSNS (40) : SLOW_INSN_COST)

/* Helper function for hexagon_rtx_costs.   */
static int
hexagon_constant_cost (rtx x, int outer_code, int speed)
{
  /* Note that the mode of a CONST_INT is not really usable, but it
     happens to be ignored in all the places we use it anyway.  If the mode
     really were accurate, we could use it to e.g. assign higher costs
     to DImode constants.  */
  enum machine_mode mode = GET_MODE (x);
  int immed = IMMED_CONSTANT_COST;
  int extended = CONSTANT_EXTENDER_COST (speed);

  /* These are approximately correct for the most common cases.  */
  switch (outer_code)
    {
    case SET:
      /* We do a better job with estimating SET costs in
	 hexagon_rtx_costs, where we know both the mode and destination
         (reg/mem).  Here just make a guess.  */
      if (s8_const_int_operand (x, mode))
	return immed;
      else if (TARGET_V4_FEATURES)
	return extended;
      else
	break;
      
    case PLUS:
      if (s16_const_int_operand (x, mode))
	return immed;
      else if (TARGET_V4_FEATURES)
	return extended;
      else
	break;

    case MINUS:
      if (s10_const_int_operand (x, mode))
	return immed;
      else if (TARGET_V4_FEATURES)
	return extended;
      else
	break;

    case MULT:
      if (m9_const_int_operand (x, mode))
	return immed;
      else if (TARGET_V4_FEATURES)
	/* We have to discount the cost of a constant extender here,
	   otherwise GCC tries to generate sequences of shifts and adds
	   intead.  */
	return CONSTANT_EXTENDER_COST (speed) / 2;
      else
	break;

    case AND:
    case IOR:
      if (s10_const_int_operand (x, mode))
	return immed;
      else if (TARGET_V4_FEATURES)
	return extended;
      else
	break;

    case IF_THEN_ELSE:  /* Mux */
      if (s8_const_int_operand (x, mode))
	return immed;
      else if (TARGET_V4_FEATURES)
	return extended;
      else
	break;

    case EQ:
    case NE:
    case GT:
    case LE:
      if (s16_const_int_operand (x, mode))
	return immed;
      else if (TARGET_V4_FEATURES)
	return extended;
      else
	break;

    case LT:
    case GE:
      if (!TARGET_V4_FEATURES)
	break;
      else if (s10_const_int_operand (x, mode))
	return immed;
      else
	return extended;

    case GTU:
    case GEU:
    case LTU:
    case LEU:
      if (!TARGET_V4_FEATURES)
	break;
      else if (u9_const_int_operand (x, mode))
	return immed;
      else
	return extended;

    case ASHIFT:
    case ASHIFTRT:
    case LSHIFTRT:
      return immed;

    default:
      /* Wild guess for everything else.  */
      if (s8_const_int_operand (x, mode))
	return immed;
    }

  /* If we got here, assume that the constant must be loaded into a
     register.  */
  if (s16_const_int_operand (x, mode))
    return FAST_INSN_COST + immed;
  else if (TARGET_V4_FEATURES)
    return FAST_INSN_COST + extended;
  else
    /* No extenders, must load from fast memory.  */
    return FAST_INSN_COST + MEMORY_COST (speed);
}

/* Return the address for accessing X in MODE.  SPEED is true if optimizing
   for speed, false if optimizing for space.  */
static int
hexagon_address_cost_internal (rtx x, enum machine_mode mode, bool speed)
{
  /* Ivopts tries to "correct" address costs of zero, which screws up relative
     cost measures pretty badly.  Therefore bias the result by including the
     memory access cost in the address cost.  We compensate by not adding the
     access cost in computing the cost of a MEM expression.  */
  int base = MEMORY_COST (speed);
  int extender = ADDRESS_CONSTANT_EXTENDER_COST (speed);
  /* Try to discount more complex addressing modes.  The idea is to tell the
     compiler that it is cheaper to put a constant offset or scale in the
     address computation than to try to CSE it.  See should_replace_address
     in fwprop.c.  */
  int discount = -1;

  /* Indirect.  */
  if (hexagon_address_register_p (x, false))
    return base;

  /* Indirect with offset.  */
  if (hexagon_ind_with_offset_address_p (mode, x, 6, false, false))
    return base + discount;
  if (hexagon_ind_with_offset_address_p (mode, x, -11, false, false))
    return base + discount;
  if (TARGET_V4_FEATURES
      && hexagon_ind_with_offset_address_p (mode, x, -11, true, false))
    return base + extender;

  /* Indirect with register offset.  */
  if (hexagon_ind_with_reg_offset_address_p (x, false))
    return base;

  /* Indirect with scaled register offset.  */
  if (hexagon_ind_with_scaled_reg_offset_address_p (x, false))
    return base + discount;

  /* Post-modified address.  */
  if (hexagon_post_modify_address_p (mode, x, false))
    return base + discount;

  /* GP-relative address.  */
  if (sdata_symbolic_operand (x, Pmode))
    return base;

  /* Absolute.  */
  if (TARGET_V4_FEATURES
      && hexagon_absolute_address_p (mode, x, true))
    return base + extender;

  /* Absolute plus register/scaled register address.  */
  if (TARGET_V4_FEATURES
      && hexagon_absolute_with_scaled_reg_address_p (mode, x, true, false))
    return base + extender + discount;

  /* The following addressing modes are not supported:
     - Indirect with post-modify register (memXX(Rx++Mu))
     - Circular with auto-increment immediate
     - Circular with auto-increment register
     - Bit-reversed with auto-increment register.  */

  /* For anything else, estimate the cost as
     (expression cost + cost to move to reg + cost of indirect address)  */
  return rtx_cost (x, SET, speed) + COSTS_N_INSNS (1) + base;
}


/* Implements hook TARGET_RTX_COSTS.  */
static bool
hexagon_rtx_costs(rtx x, int code, int outer_code, int *total, bool speed)
{
  enum machine_mode mode = GET_MODE (x);

  if (outer_code == MEM)
    {
      *total = hexagon_address_cost (x, speed);
      return true;
    }

  switch(code){

    case CONST_INT:
      *total = hexagon_constant_cost (x, outer_code, speed);
      return true;

    case CONST_DOUBLE:
      /* Typically a fast memory reference.  */
      *total = MEMORY_COST (speed);
      return true;

    case CONST:
    case HIGH:
    case SYMBOL_REF:
      *total = ADDRESS_CONSTANT_EXTENDER_COST (speed);
      return true;

    case MEM:
      *total = hexagon_address_cost_internal (XEXP (x, 0), mode, speed);
      return true;

    case UNSPEC:
      /* Make r.new cost the same as a reg.  */
      if (XINT (x, 1) == UNSPEC_NEW_VALUE)
	{
	  *total = 0;
	  return true;
	}
      /* FIXME:  anything else we should check for here?  */
      return false;

    case SET:
      {
	rtx x0 = XEXP (x, 0);
	rtx x1 = XEXP (x, 1);
	enum rtx_code code1 = GET_CODE (x1);

	/* Try to get better cost estimates for constant load/stores.  */
	if (code1 == CONST_INT && (REG_P (x0) || MEM_P (x0)))
	  {
	    if (mode == DImode)
	      /* DImode constants are loaded from memory, e.g. using
		 GP-relative addressing.  */
	      *total += MEMORY_COST (speed);
	    else if ((REG_P (x0) && s16_const_int_operand (x1, mode))
		     || s8_const_int_operand (x1, mode))
	      *total += IMMED_CONSTANT_COST;
	    else
	      *total += CONSTANT_EXTENDER_COST (speed);
	    *total += rtx_cost (x0, SET, speed);
	    return true;
	  }

	/* If this is a MEMOP, make the mem store in the SET free.  */
	if (TARGET_V4_FEATURES
	    && MEM_P (x0)
	    && hexagon_rmw_mem_operand (mode, x0, true)
	    && (code1 == PLUS || code1 == MINUS || code1 == AND || code1 == IOR)
	    && rtx_equal_p (x0, XEXP (x1, 0)))
	  {
	    *total = rtx_cost (x1, SET, speed);
	    return true;
	  }

	/* Give all jumps (conditional or not) the same cost for now.
	   Jumps should have a fairly high cost so that conditional
	   execution instructions are cheap by comparison.  */
	if (GET_CODE (x0) == PC)
	  {
	    *total = SLOW_INSN_COST;
	    return true;
	  }

	/* Default handling.  */
	return false;
      }

    case PLUS:
    case MINUS:
      if(mode == DFmode || mode == SFmode){
	if (TARGET_HARD_FLOAT && (!TARGET_V5_FEATURES || mode == SFmode))
	  *total = COSTS_N_INSNS (1);
	else
	  *total = LIBCALL_COST (speed);
      }
      else 
	{
	  rtx x0 = XEXP (x, 0);
	  rtx x1 = XEXP (x, 1);
	  
	  /* Look for add/sub and accumulate.  This pattern is fairly
	     restrictive because in the general case nested adds/subtracts
	     should be optimized to something else.  Multiply-and-accumulate
	     and shift-and-accumulate patterns are more general and are
	     handled in a bottom-up way.  */
	  if (mode == SImode && outer_code == SET)
	    {
	      if ((GET_CODE (x0) == PLUS || GET_CODE (x0) == MINUS)
		  && REG_P (XEXP (x0, 0))
		  && REG_P (XEXP (x0, 1))
		  && (REG_P (x1) || CONST_INT_P (x1)))
		{
		  *total = 0;
		  return false;
		}
	      /* Some negative patterns have reversed operands.  */
	      if ((GET_CODE (x1) == PLUS || GET_CODE (x1) == MINUS)
		  && REG_P (XEXP (x1, 0))
		  && REG_P (XEXP (x1, 1))
		  && (REG_P (x0) || CONST_INT_P (x0)))
		{
		  *total = 0;
		  return false;
		}
	    }
	  *total = COSTS_N_INSNS (1);
	}
      return false;

    case MULT:
      if(mode == DFmode || mode == SFmode){
	if (TARGET_HARD_FLOAT && (!TARGET_V5_FEATURES || mode == SFmode))
	  *total = COSTS_N_INSNS (1);
	else
	  *total = LIBCALL_COST (speed);
      }
      else if (mode == DImode)
	/* DImode multiply expands into a series of 3 insns.  This is
	   probably an understatement of the actual cost, but making it
	   higher encourages GCC to try to turn multiplies into even
	   less-efficient shifts and adds.  */
        *total = FAST_INSN_COST * 3;
      else if(outer_code == PLUS || outer_code == MINUS){
        /* MAC */
        *total = 0;
      }
      else {
        *total = COSTS_N_INSNS (1);
      }
      return false;

    case UDIV:
    case UMOD:
    case DIV:
    case MOD:
      *total = LIBCALL_COST (speed);
      return false;

    case CLZ:
    case FFS:
      *total = COSTS_N_INSNS(1);
      return false;

    case NOT:
    case AND:
    case IOR:
    case XOR:
    case SMIN:
    case UMIN:
    case SMAX:
    case UMAX:
    case ZERO_EXTRACT:
      *total = COSTS_N_INSNS (1);
      return false;

    case ASHIFT:
    case ASHIFTRT:
    case LSHIFTRT:
      if (outer_code == PLUS
	  || outer_code == MINUS
	  || outer_code == AND
	  || outer_code == IOR)
	*total = 0;  
      else
	*total = COSTS_N_INSNS (1);
      return false;

    case SIGN_EXTEND:
    case ZERO_EXTEND:
      if (GET_CODE (XEXP (x, 0)) == MEM)
        *total = 0;
      else
        *total = COSTS_N_INSNS (1);
      return false;

    case NEG:
    case ABS:
      /* Yes, even float mode */
      *total = COSTS_N_INSNS (1);
      return false;

    case EQ:
    case NE:
      /* Look for the tstbit pattern.  */
      {
	rtx x0 = XEXP (x, 0);
	rtx x1 = XEXP (x, 1);
	if (GET_MODE (x0) == SImode
	    && GET_CODE (x0) == ZERO_EXTRACT
	    && CONST_INT_P (XEXP (x0, 1))
	    && INTVAL (XEXP (x0, 1)) == 1
	    && CONST_INT_P (x1)
	    && INTVAL (x1) == 0)
	  {
	    *total += rtx_cost (XEXP (x0, 0), ZERO_EXTRACT, speed);
	    *total += rtx_cost (XEXP (x0, 2), ZERO_EXTRACT, speed);
	    return true;
	  }
	return false;
      }

    case COMPARE:
    case LT:
    case LTU:
    case LE:
    case LEU:
    case GT:
    case GTU:
    case GE:
    case GEU:
    case UNORDERED:
    case LTGT:
      /* Use the first operand's mode, not the result mode.  */
      mode = GET_MODE (XEXP (x, 0));
      if(mode == DFmode || mode == SFmode){
	if (TARGET_HARD_FLOAT)
	  *total = COSTS_N_INSNS (1);
	else
	  *total = LIBCALL_COST (speed);
      }
      else {
        *total = COSTS_N_INSNS (1);
      }
      return false;

    case FLOAT:
    case UNSIGNED_FLOAT:
    case FIX:
    case UNSIGNED_FIX:
    case FLOAT_TRUNCATE:
    case FLOAT_EXTEND:
      if (TARGET_HARD_FLOAT)
	*total = COSTS_N_INSNS (1);
      else
	*total = LIBCALL_COST (speed);
      return false;

    case COND_EXEC:
      /* Conditional execution doesn't cost more than the base insn.  */
      *total = rtx_cost (XEXP (x, 1), COND_EXEC, speed);
      return true;

    case IF_THEN_ELSE:
      /* Is this a mux insn?  */
      {
	rtx x0 = XEXP (x, 0);
	rtx x1 = XEXP (x, 1);
	rtx x2 = XEXP (x, 2);
	if ((GET_CODE (x0) == NE || GET_CODE (x0) == EQ)
	    && GET_MODE (XEXP (x0, 0)) == BImode
	    && CONST_INT_P (XEXP (x0, 1)))
	  {
	    *total = (COSTS_N_INSNS (1)
		      + rtx_cost (XEXP (x0, 0), IF_THEN_ELSE, speed)
		      + rtx_cost (x1, IF_THEN_ELSE, speed)
		      + rtx_cost (x2, IF_THEN_ELSE, speed));
	    return true;
	  }
	return false;
      }

    default:
      /* Guess is good enough */
      return false;
  }
  return false;
}




bool
hexagon_rtx_costs_debug(rtx x, int code, int outer_code, int *total, bool speed)
{
  bool retval;

  debug_rtx(x);
  retval = hexagon_rtx_costs(x, code, outer_code, total, speed);
  fprintf(stderr, "rtx_cost:  %d, %s\n\n", *total, retval ? "true" : "false");

  return retval;
}




/* Implements hook TARGET_ADDRESS_COST */

static int
hexagon_address_cost(rtx address, bool speed)
{
  /* Hmmm, we don't know what mode to use, so choose a representative one.  */
  return hexagon_address_cost_internal (address, SImode, speed);
}



int
hexagon_address_cost_debug(rtx address, bool speed)
{
  int retval;

  debug_rtx(address);
  retval = hexagon_address_cost(address, speed);
  fprintf(stderr, "address_cost:  %d\n\n", retval);

  return retval;
}




/*---------------------------------
Adjusting the Instruction Scheduler
---------------------------------*/

/* Implements hook TARGET_SCHED_ISSUE_RATE */

static int
hexagon_sched_issue_rate(void)
{
  return 6; /* ??? 4 or 6? */
}

static tree
get_mem_expr_from_mem_rtx (rtx mem)
{
  tree mem_expr;
  mem_expr = MEM_EXPR (mem);
  if (!mem_expr)
    {
      rtx addr, reg;
      reg = NULL_RTX;
      addr = XEXP (mem, 0);
      if (GET_CODE (addr) == PLUS)
        reg = XEXP (addr, 0);
      else if (GET_CODE (addr) == REG)
        reg = addr;

      if (reg)
        mem_expr = REG_EXPR (reg);
   }
  return mem_expr;
}

/*
  ---------------------------------------------------------------
 | * - \ | / - \ | / - \ | / - \ | / - \ | / - \ | / - \ | / - * |
 | |  -------------------------------------------------------  | |
 | / |    The Instruction Scheduler calculates Instruction   | \ |
 | - |        Dependencies.  Evaluate the dependencies       | - |
 | \ |      calculated by the Scheduler. Specifically,       | / |
 | | |     remove dependencies between stores to addresses   | | |
 | / |    based on __restrict qualified pointers and loads   | \ |
 | - |                    from addresses.                    | - |
 | \ |                       --==*==--                       | / |
 | |  -------------------------------------------------------  | |
 | * - / | \ - / | \ - / | \ - / | \ - / | \ - / | \ - / | \ - * |
  ---------------------------------------------------------------
*/

static inline unsigned int
get_pair_comp_reg (unsigned int reg)
{
    return (reg & 0x1) ? reg - 1 : reg + 1;
}

static inline bool
pair_match (unsigned int x_reg,int x_pair,unsigned int y_reg,int y_pair)
{
     /*  x) r0     y) r0   */
    if (x_reg == y_reg)  return true;

    if (y_pair)
      {
        if (x_pair)
          { /*   x) r1:0    y) r0:1    */
            if (get_pair_comp_reg(x_reg) == get_pair_comp_reg(y_reg)
                || get_pair_comp_reg(x_reg) == y_reg
                || get_pair_comp_reg(y_reg) == x_reg)   return true;
          }
        else
          { /*   x) r1      y) r0:1    */
            if (get_pair_comp_reg(y_reg) == x_reg)   return true;
          }
    }
    else
      {
        if (x_pair)
          { /*   x) r1:0    y) r1    */
            if (get_pair_comp_reg(x_reg) == y_reg)   return true;
          }
      }

    return false;
}

/* FIXME: Helper function for is_restrict_qualified sections */
#if 0
static bool
get_usable_reg_num
(rtx dst_0,unsigned int *reg_number, int *updated, int *reg_pair)
{
    if (!dst_0)  return false;

    switch (GET_CODE (dst_0))
      {
        case REG:
            if (GET_MODE(dst_0) == DImode)   (*reg_pair)++;
            *reg_number = REGNO(dst_0);
            return true;
        break;
        case POST_INC:
        case POST_DEC:
        case PRE_INC:
        case PRE_DEC:
        case POST_MODIFY:
        case PLUS:
        case MINUS:
            (*updated)++;
            return get_usable_reg_num(XEXP (dst_0 ,0), reg_number,
                                      updated, reg_pair);
        break;
        default:
            return false;
        break;
    }
    return false;
}
#endif

static void
hexagon_sched_dependencies_eval (rtx head, rtx tail)
{
  rtx insn, next_tail;

  if (!TARGET_RESOLVE_RESTRICT_ALIASING)
    return;

  next_tail = NEXT_INSN (tail);
  for (insn = head; insn != next_tail; insn = NEXT_INSN (insn))
    {
      rtx x;

      if (!INSN_P (insn))
        continue;
      x = PATTERN (insn);

      /* We are interested if the insn is a store insn. */
      if (GET_CODE (x) == SET)
        {
          rtx x_dst  = SET_DEST(x);
          rtx x_src  = SET_SRC(x);

          if (MEM_P (x_dst))
            {
              tree base1;
/* FIXME declerations only used in restrict_qualified section below */
#if 0
              tree base2;
              unsigned int    y_dst_regno   = 0;
              unsigned int    y_src_regno   = 0;
              unsigned int    x_dst_regno   = 0;
              unsigned int    x_src_regno   = 0;
              int             y_updates_reg = 0;
              int             x_updates_reg = 0;
              int             x_src_pair    = 0;
              int             y_src_pair    = 0;
              int             x_dst_pair    = 0;
              int             y_dst_pair    = 0;
#endif
              base1 = get_mem_expr_from_mem_rtx (x_dst);
              /* If mem_expr is still NULL or source of the mem op is
                 not a register, then give up. */
              if (!base1 || !REG_P(x_src))
                continue;
              while (handled_component_p (base1))
                base1 = TREE_OPERAND (base1, 0);
/* FIXME support for is_restrict_qualified support no longer exists in alias.c.
   This section requires a review. */
#if 0
              /* Things become intesting only if this is an
                 access via a restrict pointer.
               */
              if (is_restrict_qualified (base1))
                {
                  sd_iterator_def sd_it;
                  dep_t dep;
                  /*  Go through all the dependent ops,
                      and delete qualified
                      memory accesses
                   */

                  for (sd_it = sd_iterator_start (insn, SD_LIST_FORW);
                       sd_iterator_cond (&sd_it, &dep);)
                    {
                      rtx y = PATTERN (DEP_CON (dep));

                      /* Naming convention:
                         x) x_dst = x_src
                         y) y_dst = y_src
                       */

                      if (GET_CODE (y) == SET)
                        {
                          rtx y_dst = SET_DEST (y);
                          rtx y_src = SET_SRC (y);

                          if (MEM_P (y_dst))
                            {
                              tree mem_expr_dep =
                                get_mem_expr_from_mem_rtx (y_dst);

                              if (!mem_expr_dep)
                                {
                                  /*  If no info exist, nothing else
                                      we can do here
                                      if mem() operand list is empty,
                                      I also should
                                      not reason about it.*/
                                  sd_iterator_next (&sd_it);
                                  continue;
                                }

                              /* This is the case of store-to-store
                                 dependency
                                 x) mem(r0) = r0;
                                 y) mem(r0++) = r3;
                                 or
                                 x) mem(r0++) = r0;
                                 y) mem(r0) = r3;
                                 Here we might have an update of some sort
                                 in XY making it more interdependent
                                 then simple memory dependency would suggest
                               */

                              /* Try to obtain register information
                                 including potential
                                 pair register updates/uses
                               */

                              if (!get_usable_reg_num (y_src, &y_src_regno,
                                                       &y_updates_reg,
                                                       &y_src_pair)
                                  || !get_usable_reg_num (XEXP ((x_dst),0),
                                                          &x_dst_regno,
                                                          &x_updates_reg,
                                                          &x_dst_pair)
                                  || !get_usable_reg_num (XEXP ((y_dst),0),
                                                          &y_dst_regno,
                                                          &y_updates_reg,
                                                          &y_dst_pair)
                                  || !get_usable_reg_num (x_src, &x_src_regno,
                                                          &x_updates_reg,
                                                          &x_src_pair))
                                {
                                  /*  At any failure we simply move on
                                      without modifying existent dependency
                                   */
                                  sd_iterator_next (&sd_it);
                                  continue;
                                }

                              /* Here I know the reg numbers.
                                 Analyze all combinations */
                              if (((x_updates_reg || y_updates_reg)
                                    && pair_match (y_dst_regno, y_dst_pair,
                                                   x_dst_regno, x_dst_pair))
                                  /*  x) mem(r0) = r1;       <-- Use r0
                                      y) mem(r0++) = r2;     <-- Update/Use r0
                                      or
                                      x) mem(r0++) = r1;     <-- Update/Use r0
                                      y) mem(r0) = r2;       <--  Use r0
                                   */
                                ||  (y_updates_reg
                                     && pair_match (y_dst_regno, y_dst_pair,
                                                    x_src_regno, x_src_pair))
                                  /*  x) mem(r1) = r0;       <-- Use r0
                                      y) mem(r0++) = r2;     <-- Update/Use r0
                                   */
                                ||  (x_updates_reg
                                     && pair_match (y_src_regno, y_src_pair,
                                                    x_dst_regno, x_dst_pair)))
                                  /*  x) mem(r0++) = r1;     <-- Update/Use r0
                                      y) mem(r2) = r0;       <-- Use r0

                                      The following case:
                                      pair_match (y_src_regno, y_src_pair,
                                      x_src_regno, x_src_pair)
                                      x) mem(r1) = r0;       <-- Use r0
                                      y) mem(r2) = r0;       <-- Use r0
                                      Without a possibility of an update
                                      presents no interest
                                  */
                                {
                                  sd_iterator_next (&sd_it);
                                  continue;
                                }

                                /* Now once register dependencies are rooled
                                   out, carry on
                                   with memory dependency analysis
                                */
                                base2 = mem_expr_dep;
                                while (handled_component_p (base2))
                                    base2 = TREE_OPERAND (base2, 0);

                                if (operand_equal_p (base1, base2, 0))
                                  {
                                    sd_iterator_next (&sd_it);
                                    continue;
                                  }
                                else
                                  {
                                    /* If base1 is restrict qualified and
                                       base1 != base2 then remove this
                                       dependency */
                                    sd_delete_dep (sd_it);
                                    continue;
                                  }
                            }
                          else if (MEM_P (y_src))
                            {
                              tree mem_expr_dep =
                                get_mem_expr_from_mem_rtx (y_src);

                              if (!mem_expr_dep)
                                {
                                  sd_iterator_next (&sd_it);
                                  continue;
                                }

                              /* This is the case of a load-to-store dependency:
                                 x) mem(r0) = r0;     <-- Use r0
                                 y) r0 = mem(r2);     <-- Update/Use r0
                                 or
                                 x) mem(r0) = r0;    <-- Use r0
                                 y) r2 = mem(r0++);  <-- Update/Use r0

                                 In this case y_dst cannot be a memory location
                                 Similarly to the previous case,
                                 extract reg info
                               */

                              if (!get_usable_reg_num (XEXP ((y_src),0),
                                                       &y_src_regno,
                                                       &y_updates_reg,
                                                       &y_src_pair)
                                  || !get_usable_reg_num (XEXP ((x_dst),0),
                                                          &x_dst_regno,
                                                          &x_updates_reg,
                                                          &x_dst_pair)
                                  || !get_usable_reg_num (y_dst, &y_dst_regno,
                                                          &y_updates_reg,
                                                          &y_dst_pair)
                                  || !get_usable_reg_num (x_src, &x_src_regno,
                                                          &x_updates_reg,
                                                          &x_src_pair))
                                {
                                  sd_iterator_next (&sd_it);
                                  continue;
                                }

                              if (pair_match (y_dst_regno, y_dst_pair,
                                              x_dst_regno, x_dst_pair)
                                 /*  x) mem(r0) = r1;    <-- Use r0
                                     y) r0 = mem(r2);    <-- Update r0
                                  */
                                  || pair_match (y_dst_regno, y_dst_pair,
                                                 x_src_regno, x_src_pair)
                                  /*  x) mem(r1) = r0;    <-- Use r0
                                      y) r0 = mem(r2);    <-- Update r0
                                   */
                                  || ((y_updates_reg || x_updates_reg)
                                      && pair_match (y_src_regno, y_src_pair,
                                                     x_dst_regno, x_dst_pair))
                                  /*  x) mem(r0) = r1;    <-- Use r0
                                      y) r2 = mem(r0++);  <-- Update/Use r0
                                      or
                                      x) mem(r0++) = r1;  <-- Update/Use r0
                                      y) r2 = mem(r0);    <-- Use r0
                                   */
                                  || (y_updates_reg
                                      && pair_match (y_src_regno, y_src_pair,
                                                     x_src_regno, x_src_pair)))
                                  /*  x) mem(r1) = r0;    <-- Use r0
                                      y) r2 = mem(r0++);  <-- Update/Use r0
                                   */
                                  {
                                    sd_iterator_next (&sd_it);
                                    continue;
                                  }

                                /* Ok, now we have ruled out register
                                   dependency,
                                   proceed with
                                   memory dependency analysis */

                                base2 = mem_expr_dep;
                                while (handled_component_p (base2))
                                    base2 = TREE_OPERAND (base2, 0);

                                if (operand_equal_p (base1, base2, 0))
                                  {
                                    sd_iterator_next (&sd_it);
                                    continue;
                                  }
                                else
                                  {
                                /*  If base1 is restrict qualified and
                                    base1 != base2 then remove this
                                    dependency
                                 */
                                    sd_delete_dep (sd_it);
                                    continue;
                                  }
                            }
                          else  /* !MEM_P (SET_DEST (y)) &
                                   !MEM_P (SET_SRC (y)) */
                            sd_iterator_next (&sd_it);
                        }
                      else  /* GET_CODE (y) != SET */
                        sd_iterator_next (&sd_it);
                    } /* End FOR LOOP */
                } /* is_restrict_qualified (base1) */
#endif
            } /* MEM_P (SET_DEST (x)) */
          else if (MEM_P (x_src))
            {
              /* X is a load */

              tree base1;
/* FIXME declerations only used in is_restrict_qualified section below */
#if 0
              tree base2;
              unsigned int    y_dst_regno   = 0;
              unsigned int    y_src_regno   = 0;
              unsigned int    x_dst_regno   = 0;
              unsigned int    x_src_regno   = 0;
              int             y_updates_reg = 0;
              int             x_updates_reg = 0;
              int             x_src_pair    = 0;
              int             y_src_pair    = 0;
              int             x_dst_pair    = 0;
              int             y_dst_pair    = 0;
#endif
              base1 = get_mem_expr_from_mem_rtx (x_src);
              /* If mem_expr is still NULL or source of the mem op is
                 not a register, then give up. */
              if (!base1 || !REG_P(x_dst))
                continue;
              while (handled_component_p (base1))
                base1 = TREE_OPERAND (base1, 0);

/* FIXME support for is_restrict_qualified support no longer exists in alias.c.
   This section requires a review. */
#if 0
              /* Things become intesting only if this is an
                 access via a restrict pointer. */
              if (is_restrict_qualified (base1))
                {
                  sd_iterator_def sd_it;
                  dep_t dep;
                  /* Go through all the dependent ops,
                     and delete qualified
                     memory accesses */

                  for (sd_it = sd_iterator_start (insn, SD_LIST_FORW);
                       sd_iterator_cond (&sd_it, &dep);)
                    {
                      rtx y = PATTERN (DEP_CON (dep));

                      if (GET_CODE (y) == SET)
                        {
                          rtx y_dst = SET_DEST (y);
                          rtx y_src = SET_SRC (y);

                          if (MEM_P (y_dst))
                            {
                              tree mem_expr_dep =
                                get_mem_expr_from_mem_rtx (y_dst);

                              if (!mem_expr_dep)
                                {
                                  sd_iterator_next (&sd_it);
                                  continue;
                                }

                              /*  This is the case of load-to-store
                                  dependency
                                  x) r0 = mem(r0) ;
                                  y) mem(r0++) = r3;
                                  or
                                  x) r0 = mem(r0++) ;
                                  y) mem(r0) = r3;
                                  Here again we might have an
                                  update of some sort
                                  X or Y
                               */


                              if (!get_usable_reg_num (y_src, &y_src_regno,
                                                       &y_updates_reg,
                                                       &y_src_pair)
                                  || !get_usable_reg_num (x_dst, &x_dst_regno,
                                                          &x_updates_reg,
                                                          &x_dst_pair)
                                  || !get_usable_reg_num (XEXP ((y_dst),0),
                                                          &y_dst_regno, &y_updates_reg,
                                                          &y_dst_pair)
                                  || !get_usable_reg_num (XEXP ((x_src),0),
                                                          &x_src_regno, &x_updates_reg,
                                                          &x_src_pair))
                                {
                                  sd_iterator_next (&sd_it);
                                  continue;
                                }

                              /* I know the reg numbers that X/Y  updates */
                              if (pair_match (y_dst_regno, y_dst_pair,
                                              x_dst_regno, x_dst_pair)
                                 /*  x) r0 = mem(r1);    <-- Update r0
                                     y) mem(r0++) = r2;  <-- Update/Use r0
                                  */
                                  || ((y_updates_reg || x_updates_reg)
                                  && pair_match (y_dst_regno, y_dst_pair,
                                                 x_src_regno, x_src_pair))
                                  /*  x) r1 = mem(r0);    <-- Use r0
                                      y) mem(r0++) = r2;  <-- Update r0
                                      or
                                      x) r1 = mem(r0++);  <-- Update r0
                                      y) mem(r0) = r2;    <-- Use r0
                                   */
                                  || pair_match (y_src_regno, y_src_pair,
                                                 x_dst_regno, x_dst_pair)
                                  /*  x) r0 = mem(r1) ;   <-- Update r0
                                      y) mem(r2) = r0;    <-- Use r0
                                   */
                                  || (x_updates_reg
                                  && pair_match (y_src_regno, y_src_pair,
                                                 x_src_regno, x_src_pair)))
                                  /*  x) r1 = mem(r0++);  <-- Update/Use r0
                                      y) mem(r2) = r0;    <-- Use r0
                                  */
                                  {
                                    sd_iterator_next (&sd_it);
                                    continue;
                                  }

                                base2 = mem_expr_dep;
                                while (handled_component_p (base2))
                                    base2 = TREE_OPERAND (base2, 0);

                                if (operand_equal_p (base1, base2, 0))
                                  {
                                    sd_iterator_next (&sd_it);
                                    continue;
                                  }
                                else
                                  {
                                    /* If base1 is restrict qualified and
                                       base1 != base2 then remove this
                                       dependency
                                     */
                                    sd_delete_dep (sd_it);
                                    continue;
                                  }
                            }
                          else if (MEM_P (y_src))
                            {
                              tree mem_expr_dep =
                                get_mem_expr_from_mem_rtx (y_src);

                              if (!mem_expr_dep)
                                {
                                  sd_iterator_next (&sd_it);
                                  continue;
                                }

                              /*  This is the case of a load-to-load
                                  dependency:
                                  x) r0 = mem(r0) ;   <-- Use r0
                                  y) r0 = mem(r2);    <-- Update r0
                                  or
                                  x) r0 = mem(r0) ;   <-- Use r0
                                  y) r2 = mem(r0++);  <-- Update/Use r0
                               */

                              if(!get_usable_reg_num (XEXP ((y_src),0),
                                                      &y_src_regno,
                                                      &y_updates_reg,
                                                      &y_src_pair)
                                 || !get_usable_reg_num (x_dst, &x_dst_regno,
                                                         &x_updates_reg,
                                                         &x_dst_pair)
                                 || !get_usable_reg_num (y_dst, &y_dst_regno,
                                                         &y_updates_reg,
                                                         &y_dst_pair)
                                 || !get_usable_reg_num (XEXP ((x_src),0),
                                                         &x_src_regno,
                                                         &x_updates_reg,
                                                         &x_src_pair))
                                {
                                  sd_iterator_next (&sd_it);
                                  continue;
                                }

                              /* Analyze potential register deps:
                                 First combination:
                                 pair_match (y_dst_regno, y_dst_pair,
                                 x_dst_regno, x_dst_pair)
                                 x) r0 = mem(r1);    <-- Update r0
                                 y) r0 = mem(r2);    <-- Update r0

                                 We do not really care about this case
                                 Output dep with no update possible
                               */
                                if(pair_match (y_dst_regno, y_dst_pair,
                                               x_src_regno, x_src_pair)
                                 /*  x) r1 = mem(r0) ;   <-- Uses r0
                                     y) r0 = mem(r2);    <-- Update r0
                                  */
                                   || pair_match (y_src_regno, y_src_pair,
                                                  x_dst_regno, x_dst_pair)
                                 /*  x) r0 = mem(r1) ;   <-- Update r0
                                     y) r2 = mem(r0);    <-- Uses r0
                                  */
                                   || ((y_updates_reg || x_updates_reg)
                                      && pair_match (y_src_regno, y_src_pair,
                                                     x_src_regno, x_src_pair)))
                                 /*  x) r1 = mem(r0);    <-- Use r0
                                     y) r2 = mem(r0++);  <-- Update/Use r0
                                     or
                                     x) r1 = mem(r0++);  <-- Update/Use r0
                                     y) r2 = mem(r0);    <-- Use r0
                                  */
                                  {
                                    sd_iterator_next (&sd_it);
                                    continue;
                                  }

                                /*  Ok, now we have ruled out register
                                    dependency,
                                    proceed with
                                    memory dependency analysis */

                                base2 = mem_expr_dep;
                                while (handled_component_p (base2))
                                    base2 = TREE_OPERAND (base2, 0);

                                if (operand_equal_p (base1, base2, 0))
                                  {
                                    sd_iterator_next (&sd_it);
                                    continue;
                                  }
                                else
                                  {
                                    /*  If base1 is restrict qualified and
                                        base1 != base2 then remove this
                                        dependency
                                    */
                                    sd_delete_dep (sd_it);
                                    continue;
                                  }
                            }
                          else  /* !MEM_P (SET_DEST (y)) &
                                   !MEM_P (SET_SRC (y))  */
                            sd_iterator_next (&sd_it);
                        }
                      else    /* GET_CODE (y) != SET */
                        sd_iterator_next (&sd_it);
                    } /* End FOR LOOP */
                } /* is_restrict_qualified (base1) */
#endif
            }
        } /* GET_CODE (x) == SET */
    } /* END MAIN FOR LOOP */
}



/* Implements hook TARGET_SCHED_FIRST_CYCLE_MULTIPASS_DFA_LOOKAHEAD */

static int
hexagon_sched_first_cycle_multipass_dfa_lookahead(void)
{
  return 8; /* ??? dunno */
}




/*--------------------------------------------------
Dividing the Output into Sections (Texts, Data, ...)
--------------------------------------------------*/

enum hexagon_section_types {HEXAGON_SDATA_SECTION, HEXAGON_SBSS_SECTION};

/* Return the section in which to place data with the access size in bytes
   (or smallest required access size if dealing with structures),
   and alignment requirements (bytes) specified by size and align.
   type indicates which base section the returned section should be a
   variant of. decl may be the decl of the data or NULL_TREE. */
static section *
hexagon_select_small_data_section (enum hexagon_section_types type,
                                   unsigned int size,
                                   unsigned int align,
                                   tree decl)
{
  char *sname;
  unsigned int flags = SECTION_WRITE | SECTION_NAMED | SECTION_SMALL;

  gcc_assert (size <= (unsigned HOST_WIDE_INT) g_switch_value);

  if (!TARGET_SECTION_SORTING
      || align > (unsigned HOST_WIDE_INT) g_switch_value)
    switch (type)
      {
        case HEXAGON_SDATA_SECTION: return sdata_section;
        case HEXAGON_SBSS_SECTION: return sbss_section;
        default: gcc_unreachable ();
      }

  switch (type)
    {
      case HEXAGON_SDATA_SECTION:
        sname = XALLOCAVEC (char, strlen (".sdata.N.N") + 1);
        sprintf (sname, ".sdata.%d.%d", size, align);
        break;

      case HEXAGON_SBSS_SECTION:
        flags |= SECTION_BSS;
        sname = XALLOCAVEC (char, strlen (".sbss.N.N") + 1);
        sprintf (sname, ".sbss.%d.%d", size, align);
        break;

      default: gcc_unreachable ();
    }

  return get_section (sname, flags, decl);
}

/* Implements hook TARGET_IN_SMALL_DATA_P */
static bool
hexagon_in_small_data_p (const_tree exp)
{
  /* We want to merge strings, so we never consider them small data. */
  if (TREE_CODE (exp) == STRING_CST)
    return false;

  /* Functions are never small data. */
  if (TREE_CODE (exp) == FUNCTION_DECL)
    return false;

  if (TREE_CODE (exp) == VAR_DECL && DECL_SECTION_NAME (exp))
    {
      const char *section = TREE_STRING_POINTER (DECL_SECTION_NAME (exp));
      if(!strcmp (section, ".sdata")
         || !strncmp (section, ".sdata.", 7)
         || !strncmp (section, ".gnu.linkonce.s.", 16)
         || !strcmp (section, ".sbss")
         || !strncmp (section, ".sbss.", 6)
         || !strncmp (section, ".gnu.linkonce.sb.", 17))
        return true;
    }

   else if (TREE_TYPE (exp))
    {
      unsigned HOST_WIDE_INT size = int_size_in_bytes (TREE_TYPE (exp));
      if ((size > 0) && (size <= (unsigned HOST_WIDE_INT) g_switch_value))
        return true;
    }

  return false;
}

/* Implements hook TARGET_ASM_SELECT_SECTION */
static section *
hexagon_asm_select_section (tree exp, int reloc, unsigned HOST_WIDE_INT align)
{
  if (hexagon_in_small_data_p (exp))
    {
      int access_size = smallest_accessable_entity_in_declaration (exp);
      return hexagon_select_small_data_section (
                               HEXAGON_SDATA_SECTION,
                               access_size,
                               align / BITS_PER_UNIT,
                               exp);
    }
  else
    return default_elf_select_section (exp, reloc, align);
}

/* Implements hook TARGET_ASM_SELECT_RTX_SECTION */
static section *
hexagon_asm_select_rtx_section (enum machine_mode mode, rtx x,
                                unsigned HOST_WIDE_INT align)
{
  if (GET_MODE_SIZE (mode) > 0
      && GET_MODE_SIZE (mode) <= (unsigned HOST_WIDE_INT) g_switch_value)
    return hexagon_select_small_data_section (HEXAGON_SDATA_SECTION,
                                          GET_MODE_SIZE (mode),
                                          align / BITS_PER_UNIT,
                                          NULL_TREE);
  else
    return default_elf_select_rtx_section (mode, x, align);
}

/* Implements hook TARGET_ASM_UNIQUE_SECTION */
static void
hexagon_asm_unique_section (tree decl, int reloc)
{
  const char *section_name;
  bool one_only = DECL_ONE_ONLY (decl) && !HAVE_COMDAT_GROUP;
  const char *sdata_prefix = (one_only) ? ".gnu.linkonce.s" : ".sdata";
  const char *sbss_prefix = (one_only) ? ".gnu.linkonce.sb" : ".sbss";
  const char *prefix = NULL;
  char *access_size_str = XALLOCAVEC (char, strlen ("N.N") + 1);

  default_unique_section (decl, reloc);
  section_name = TREE_STRING_POINTER (DECL_SECTION_NAME (decl));

  if (!strncmp (section_name, sdata_prefix, strlen (sdata_prefix)))
    prefix = sdata_prefix;

  if (!strncmp (section_name, sbss_prefix, strlen (sbss_prefix)))
    prefix = sbss_prefix;

  if (prefix)
    {
      char *string;
      int access_size = smallest_accessable_entity_in_declaration (decl);
      if (DECL_ALIGN_UNIT (decl) <= (unsigned HOST_WIDE_INT) g_switch_value)
        {
          sprintf (access_size_str, "%d.%d", access_size,
                                    DECL_ALIGN_UNIT (decl));
          string = ACONCAT ((prefix,
                             ".", access_size_str,
                             section_name + strlen (prefix), NULL));
          DECL_SECTION_NAME (decl) = build_string (strlen (string), string);
        }
    }
}

static void
hexagon_asm_named_section (const char *name, unsigned int flags, tree decl)
{
  default_elf_asm_named_section (name, flags, decl);
  if (TARGET_SECTION_SORTING
      && decl
      && hexagon_in_small_data_p (decl)
      && smallest_accessable_entity_in_declaration (decl) > 1)
     fprintf (asm_out_file, "\t.subsection\t-%d\n",
              floor_log2(smallest_accessable_entity_in_declaration(decl)));
}

static unsigned int
smallest_access_size_in_type (tree type)
{
  if (TYPE_PACKED (type))
    return 1;

  switch (TREE_CODE (type))
   {
    case POINTER_TYPE:
    case REFERENCE_TYPE:
      return POINTER_SIZE / BITS_PER_UNIT;

    case RECORD_TYPE:
    case UNION_TYPE:
    case QUAL_UNION_TYPE:
      {
        tree field;
        unsigned int smallest = TREE_INT_CST_LOW (TYPE_SIZE_UNIT (type));
        for (field = TYPE_FIELDS (type); field; field = DECL_CHAIN (field))
          {
            unsigned int small;
            if (TREE_CODE (field) == FIELD_DECL)
              {
                small = smallest_access_size_in_type (TREE_TYPE (field));
                if (small < smallest)
                  smallest = small;
              }
          }
        return smallest;
      }

    case ARRAY_TYPE:
    case VECTOR_TYPE:
      return  smallest_access_size_in_type (TREE_TYPE (type));

    default:
      return TREE_INT_CST_LOW (TYPE_SIZE_UNIT (type));
    }
}

/* smallest_accessable_entity_in_declaration designed to identify
   the smallest addressable entity in a declaration. Used for sdata
   elements sorting */
static unsigned int
smallest_accessable_entity_in_declaration (tree decl)
{
  return smallest_access_size_in_type (TREE_TYPE (decl));
}

/*-------------------------------
Output of Uninitialized Variables
-------------------------------*/

/* Implements macros ASM_OUTPUT_ALIGNED_DECL_COMMON and
   ASM_OUTPUT_ALIGNED_DECL_LOCAL */
void
hexagon_asm_output_aligned_decl_common (FILE *stream,
                                        tree decl,
                                        const char *name,
                                        unsigned HOST_WIDE_INT size,
                                        unsigned HOST_WIDE_INT alignment,
                                        bool local)
{
  HOST_WIDE_INT access_size;

  if (TARGET_SECTION_SORTING
      && decl
      && hexagon_in_small_data_p (decl))
    access_size = smallest_accessable_entity_in_declaration (decl);
  else
    access_size = size;

  if (decl && sdata_symbolic_operand(XEXP (DECL_RTL (decl), 0), Pmode))
    {
      switch_to_section (
        hexagon_select_small_data_section (HEXAGON_SBSS_SECTION,
                                           access_size,
                                           alignment / BITS_PER_UNIT,
                                           decl));
    }
  else
    switch_to_section (
      get_section (".bss", SECTION_WRITE | SECTION_BSS | SECTION_NAMED, decl));

  if (local)
    fprintf (stream, "\t.lcomm\t");
  else
    fprintf (stream, "\t.comm\t");

  assemble_name(stream, name);

  if (TARGET_SECTION_SORTING
      && decl
      && hexagon_in_small_data_p (decl))
    fprintf(stream, "," HOST_WIDE_INT_PRINT_UNSIGNED
                    "," HOST_WIDE_INT_PRINT_UNSIGNED
                    "," HOST_WIDE_INT_PRINT_UNSIGNED "\n",
                    size, alignment / BITS_PER_UNIT,
                    access_size);
  else
    fprintf(stream, "," HOST_WIDE_INT_PRINT_UNSIGNED
                    "," HOST_WIDE_INT_PRINT_UNSIGNED "\n",
                    size, alignment / BITS_PER_UNIT);
}


/*------------------------------
Output of Assembler Instructions
------------------------------*/

/* Helper function for hexagon_asm_output_opcode */

static void
hexagon_output_operand(FILE *f, rtx x, int code ATTRIBUTE_UNUSED)
{
  if(x && GET_CODE (x) == SUBREG){
    x = alter_subreg(&x);
  }

  /* X must not be a pseudo reg. */
  gcc_assert(!x || !REG_P (x) || REGNO (x) < FIRST_PSEUDO_REGISTER);

  PRINT_OPERAND (f, x, code);
}

/* Used by macro ASM_OUTPUT_OPCODE

   Outputs the assembly corresponding to the current insn
   and adds brackets as needed */

const char *
hexagon_asm_output_opcode(FILE *f, const char *ptr)
{
  struct hexagon_final_info *final_info;
  int c;
  int oporder[MAX_RECOG_OPERANDS];
  char opoutput[MAX_RECOG_OPERANDS];
  int ops = 0;

  if(!TARGET_PACKETS){
    return ptr;
  }

  final_info = &cfun->machine->final_info;

  /* Ignore the output template for this insn. */
  if(!final_info->print_insn){
    for(; *ptr; ptr++);
  }

  /* Begin packet. */
  if(final_info->start_packet){
    fputs("{\n\t", f);
  }

  if (TARGET_ANNOTATE_DUPLEX && final_info->duplex) {
    fputs("//duplex\n\t", f);
  }

  if(final_info->indent_insn){
    fputc('\t', f);
  }

  memset(opoutput, 0, sizeof(opoutput));
  /* Output the assembly insn.  Copied from output_asm_insn(). */
  while((c = *ptr)){
    ptr++;
    switch(c){
      case '\n':
        ops = 0;
        memset(opoutput, 0, sizeof(opoutput));

        putc(c, f);
        if(final_info->indent_insn){
          putc('\t', f);
        }
        break;

      case '%':
        /* %% outputs a single %. */
        if(*ptr == '%'){
          ptr++;
          putc(c, f);
        }
        /* % followed by a letter and some digits
           outputs an operand in a special way depending on the letter.
           Letters `acln' are implemented directly.
           Other letters are passed to `hexagon_output_operand' so that
           the PRINT_OPERAND macro can define them. */
        else if(ISALPHA (*ptr)){
          int letter = *ptr++;
          unsigned long opnum;
          char *endptr;

          opnum = strtoul(ptr, &endptr, 10);

          if(endptr == ptr){
            output_operand_lossage("operand number missing after %%-letter");
          }
          else if(letter == 'l'){
            output_asm_label(final_info->insn_ops[opnum]);
          }
          else if(letter == 'a'){
            output_address(final_info->insn_ops[opnum]);
          }
          else if(letter == 'c'){
            if(CONSTANT_ADDRESS_P (final_info->insn_ops[opnum])){
              output_addr_const(f, final_info->insn_ops[opnum]);
            }
            else {
              hexagon_output_operand(f, final_info->insn_ops[opnum], 'c');
            }
          }
          else if(letter == 'n'){
            if(GET_CODE (final_info->insn_ops[opnum]) == CONST_INT){
              fprintf(f, HOST_WIDE_INT_PRINT_DEC,
                      -INTVAL (final_info->insn_ops[opnum]));
            }
            else {
              putc('-', f);
              output_addr_const(f, final_info->insn_ops[opnum]);
            }
          }
          else {
            hexagon_output_operand(f, final_info->insn_ops[opnum], letter);
          }

          if(!opoutput[opnum]){
            oporder[ops++] = opnum;
          }
          opoutput[opnum] = 1;

          ptr = endptr;
          c = *ptr;
        }
        /* % followed by a digit outputs an operand the default way. */
        else if(ISDIGIT (*ptr)){
          unsigned long opnum;
          char *endptr;

          opnum = strtoul(ptr, &endptr, 10);
          hexagon_output_operand(f, final_info->insn_ops[opnum], 0);

          if(!opoutput[opnum]){
            oporder[ops++] = opnum;
          }
          opoutput[opnum] = 1;

          ptr = endptr;
          c = *ptr;
        }
        else {
          output_operand_lossage("invalid %%-code");
        }
        break;

      default:
        putc (c, f);
    }
  }

  /* End packet. */
  if(final_info->print_endloop0){
    fputs("}:endloop0 // start=", f);
    output_asm_label(final_info->endloop_label);
  }
  else if(final_info->print_endloop1){
    if(final_info->endloop0){
      fputc(' ', f);
    }
    else {
      fputc('}', f);
    }
    fputs(":endloop1 // start=", f);
    output_asm_label(final_info->endloop_label);
  }
  else if(final_info->end_packet){
    fputs("\n\t}", f);
  }

  if(final_info->print_falign){
    fputs("\n\t.falign", f);
  }

  return ptr;
}




/* Implements macro FINAL_PRESCAN_INSN

   Looks at the current and next insns to determine how to format the next insn
   to be printed and whether or not to print open or close curly brackets in
   order to begin or end a packet. */

void
hexagon_final_prescan_insn(
  rtx insn,
  rtx *ops,
  int numops ATTRIBUTE_UNUSED
)
{
  static struct hexagon_packet_info *current_packet = NULL;
  static int current_insn = 0;

  struct hexagon_final_info *final_info;
  int i;

  if(!TARGET_PACKETS){
    return;
  }

  final_info = &cfun->machine->final_info;

  final_info->duplex = (get_attr_duplex(insn) == DUPLEX_YES);

  final_info->insn_ops = ops;
  final_info->start_packet = false;
  final_info->end_packet = false;
  final_info->indent_insn = true;
  final_info->print_insn = true;
  final_info->print_endloop0 = false;
  final_info->print_endloop1 = false;
  final_info->print_falign = false;
  final_info->dot_new_gpr_p = false;
  final_info->dot_new_mem_p = false;
  final_info->dot_new_predicate_p = false;

  gcc_assert(hexagon_head_packet && hexagon_head_packet->num_insns > 0);

  /* If CURRENT_PACKET is NULL, then we are at the start of a new function. */
  if(!current_packet){
    current_packet = hexagon_head_packet;
    current_insn = 0;
  }

  /* Skip ASM_INPUTs and empty packets.  */
  while(current_packet->num_insns == 0
        || GET_CODE (PATTERN (current_packet->insns[current_insn]->insn))
           == ASM_INPUT){
    /* Advance to the next insn. */
    current_insn++;
    if (current_packet->num_insns == 0
        || current_insn == current_packet->num_insns){
      /* Advance to the next packet. */
      current_packet = current_packet->next;
      current_insn = 0;
    }
    gcc_assert(current_packet);
  }

  if(!INSN_P (insn) || get_attr_type(insn) == TYPE_MULTIPLE){
    final_info->indent_insn = false;
  }

  /* Skip over any insns not considered part of a packet. */
  if(insn != current_packet->insns[current_insn]->insn){
    return;
  }

  if(HEXAGON_NEW_PREDICATE_P (current_packet->insns[current_insn])){
    final_info->dot_new_predicate_p = true;
  }
  if(HEXAGON_NEW_GPR_P (current_packet->insns[current_insn])){
    final_info->dot_new_gpr_p = true;
  }
  if(HEXAGON_NEW_MEM_P (current_packet->insns[current_insn])){
    final_info->dot_new_mem_p = true;
  }

  /* If this is the first insn in a packet, then start the packet. */
  if(current_insn == 0){
    final_info->start_packet = true;
    final_info->endloop0 = false;
  }

  /* Print endloops specially instead of using the template in the machine
     description. */
  if(INSN_CODE (insn) == CODE_FOR_endloop0){
    /* Record that this packet ends an inner hardware loop. */
    final_info->endloop0 = true;
    final_info->indent_insn = false;
    final_info->print_insn = false;
    final_info->print_endloop0 = true;
    final_info->endloop_label = ops[0];
  }
  if(INSN_CODE (insn) == CODE_FOR_endloop1){
    final_info->indent_insn = false;
    final_info->print_insn = false;
    final_info->print_endloop1 = true;
    final_info->endloop_label = ops[0];
  }

  /* If we are fetch-aligning packets following calls, and this is the last insn
     in the current packet, and this is not the end of the function, and the
     next insn is not already an .falign, and the current packet contains a
     call, then print an .falign directive after the current packet. */
  if(hexagon_falign == HEXAGON_FALIGN_ALL
     && current_insn + 1 == current_packet->num_insns
     && current_packet->next
     && INSN_CODE (current_packet->next->insns[0]->insn) != CODE_FOR_falign){
    for(i = 0; i < current_packet->num_insns; i++){
      if(HEXAGON_CALL_P (current_packet->insns[i])
         || HEXAGON_EMULATION_CALL_P (current_packet->insns[i])){
        final_info->print_falign = true;
        break;
      }
    }
  }

  /* Advance to the next insn. */
  current_insn++;

  /* If this is the last insn in a packet, then end the packet. */
  if(current_insn == current_packet->num_insns){
    final_info->end_packet = true;

    /* Advance to the next packet. */
    current_packet = current_packet->next;
    current_insn = 0;
  }

  /* If this is not actually a packet but a sequence of serial instructions,
     then it should not be grouped with any other insns, and it should not
     have curly brackets printed around it. */
  if(get_attr_type(insn) == TYPE_MULTIPLE){
    final_info->print_insn = true;
    final_info->indent_insn = false;
    final_info->start_packet = false;
    final_info->end_packet = false;
    final_info->print_endloop0 = false;
    final_info->print_endloop1 = false;
  }

  /* If there is no next packet, then we are done with the current function. */
  if(!current_packet){
    hexagon_free_packing_info(false);
  }
}




/* globals used to pass information from hexagon_print_operand to
   hexagon_print_operand_address */

/* whether the current operand contains an extended constant */
static bool hexagon_extended_constant;

/* the mode of the current address being printed */
static enum machine_mode hexagon_address_mode;

/* Implements macro PRINT_OPERAND */

void
hexagon_print_operand(FILE *stream, rtx x, int code)
{
  hexagon_extended_constant = false;

  switch(code){
    case 'P':
      /* Output a register pair. */
      if(G_REG_P (x) && REGNO (x) % 2 == 0){
        fprintf(stream, "%s:%d", reg_names[REGNO (x) + 1], REGNO (x));
      }
      else {
        output_operand_lossage("hexagon_print_operand: invalid operand for %%P");
      }
      return;

    case 'H':
      /* Output the second register of a register pair. */
      if(G_REG_P (x) && REGNO (x) % 2 == 0){
        fputs(reg_names[REGNO (x) + 1], stream);
      }
      else {
        output_operand_lossage("hexagon_print_operand: invalid operand for %%H");
      }
      return;

    case 'L':
      /* Output the first register of a register pair. */
      if(G_REG_P (x) && REGNO (x) % 2 == 0){
        fputs(reg_names[REGNO (x)], stream);
      }
      else {
        output_operand_lossage("hexagon_print_operand: invalid operand for %%L");
      }
      return;

    case 'C':
    case 'I':
      if((GET_CODE (x) == NE || GET_CODE (x) == EQ)
         && P_REG_P (XEXP (x, 0)) && XEXP (x, 1) == const0_rtx){
        if((GET_CODE (x) == EQ) ^ (code == 'I')){
          fputc('!', stream);
        }
        fputs(reg_names[REGNO (XEXP (x, 0))], stream);
        if(cfun->machine->final_info.dot_new_predicate_p){
          fputs(".new", stream);
        }
      }
      else {
        if(code == 'C'){
          output_operand_lossage("hexagon_print_operand: invalid operand for %%C");
        }
        else {
          output_operand_lossage("hexagon_print_operand: invalid operand for %%I");
        }
      }
      return;

    case 'h':
      /* Output a branch hint if the current insn is predicated on a newly
         generated predicate or a compare with a newly generated value. */
      if(GET_CODE (x) == CONST_INT){
        if(   cfun->machine->final_info.dot_new_gpr_p
           || cfun->machine->final_info.dot_new_predicate_p){
          if(INTVAL (x)){
            fputs(":t", stream);
          }
          else {
            fputs(":nt", stream);
          }
        }
      }
      else {
        output_operand_lossage("hexagon_print_operand: invalid operand for %%h");
      }
      return;

    case 'J':
      {
        int log = exact_log2(INTVAL (x) & 0xFFFFFFFFULL);
        if(log == -1){
          output_operand_lossage("hexagon_print_operand: invalid operand for %%J");
        }
        else {
          fprintf(stream, "%d", log);
        }
      }
      return;

    case 'K':
      {
        int log = exact_log2(~INTVAL (x) & 0xFFFFFFFFULL);
        if(log == -1){
          output_operand_lossage("hexagon_print_operand: invalid operand for %%K");
        }
        else {
          fprintf(stream, "%d", log);
        }
      }
      return;

    /* Low byte of constant.  */
    case 'b':
      fprintf (stream, HOST_WIDE_INT_PRINT_DEC, INTVAL (x) & 0xff);
      return;
    
    case 'E':
      gcc_assert(TARGET_V4_FEATURES);
      hexagon_extended_constant = true;
      /* handled below */
      break;

    case 'p':
      {
       /* output the pic register */
       fprintf(stream, "%s", reg_names[PIC_OFFSET_TABLE_REGNUM]);
      }
      return;

    case 0:
      /* handled below */
      break;

    default:
      output_operand_lossage("hexagon_print_operand: unknown code, 0x%x", code);
      return;
  }

  switch(GET_CODE (x)){
    case REG:
      {
        if(REGNO (x) < FIRST_PSEUDO_REGISTER){
          if(G_REG_P (x) && GET_MODE_SIZE (GET_MODE (x)) > UNITS_PER_WORD){
            fprintf(stream, "%s:%d", reg_names[REGNO (x) + 1], REGNO (x));
          }
          else {
            fputs(reg_names[REGNO (x)], stream);
          }
        }
        else {
          output_operand_lossage("hexagon_print_operand: invalid register");
        }
      }
      return;

    case MEM:
      hexagon_address_mode = GET_MODE (x);
      output_address(XEXP (x, 0));
      return;

    case CONST_INT:
      if(INTVAL (x) < -0xFFFFFFFFLL){
        fprintf(stream, HOST_WIDE_INT_PRINT_HEX, INTVAL (x));
      }
      else {
        fprintf(stream, HOST_WIDE_INT_PRINT_DEC, INTVAL (x));
      }
      return;

    case CONST_DOUBLE:
      if(GET_MODE (x) == SFmode){
        REAL_VALUE_TYPE rv;
        long l;

        REAL_VALUE_FROM_CONST_DOUBLE (rv, x);
        REAL_VALUE_TO_TARGET_SINGLE (rv, l);
        l &= 0xFFFFFFFFUL;
        fprintf(stream, "0x%08lx", l);
        return;
      }
      else if(GET_MODE (x) == DFmode){
        REAL_VALUE_TYPE rv;
        long l[2];

        REAL_VALUE_FROM_CONST_DOUBLE (rv, x);
        REAL_VALUE_TO_TARGET_DOUBLE (rv, l);
        l[0] &= 0xFFFFFFFFUL;
        l[1] &= 0xFFFFFFFFUL;
        fprintf(stream, "0x%08lx%08lx", l[1], l[0]);
        return;
      }
      /* FALL THROUGH.  Let output_addr_const deal with it. */

    default:
      output_addr_const(stream, x);
      break;
  }
}




/* Implements macro PRINT_OPERAND_ADDRESS

   Print a memory address as an operand to reference that memory location. */

void
hexagon_print_operand_address(FILE *stream, rtx x)
{
  switch(GET_CODE (x)){
    case REG:
      if(G_REG_P (x)){
        fputs(reg_names[REGNO (x)], stream);
        fputs("+#0", stream);
      }
      else {
        output_operand_lossage("hexagon_print_operand_address: invalid base register");
      }
      break;

    case PLUS:
      {
        rtx base = XEXP (x, 0);
        rtx offset = XEXP (x, 1);
        rtx index = NULL_RTX;
        rtx scale = NULL_RTX;

        if(!(REG_P (base)
             || (CONSTANT_P (base) && GET_CODE (base) != CONST_INT))
           || (CONSTANT_P (offset) && GET_CODE (offset) != CONST_INT)){
          base = XEXP (x, 1);
          offset = XEXP (x, 0);
        }
        if(GET_CODE (offset) == MULT || GET_CODE (offset) == ASHIFT){
          index = XEXP (offset, 0);
          scale = XEXP (offset, 1);
          if(!REG_P (index)){
            index = XEXP (offset, 1);
            scale = XEXP (offset, 0);
          }
        }

        if(G_REG_P (base)){
          fprintf(stream, "%s", reg_names[REGNO (base)]);
          fputc('+', stream);
        }
        else if(!CONSTANT_P (base)){
          output_operand_lossage("hexagon_print_operand_address: invalid base+index base");
          break;
        }

        if(CONSTANT_P (offset)){
          fputc('#', stream);
          if(hexagon_extended_constant){
            fputc('#', stream);
          }
          output_addr_const(stream, offset);
        }
        else if(REG_P (offset)){
          fprintf(stream, "%s<<#0", reg_names[REGNO (offset)]);
        }
        else if(   GET_CODE (offset) == MULT
                && REG_P (index)
                && GET_CODE (scale) == CONST_INT){
          int log = exact_log2(INTVAL (scale) & 0xFFFFFFFFULL);
          if(log == -1){
            output_operand_lossage("hexagon_print_operand_address: invalid base+index scale amount");
          }
          else {
            fprintf(stream, "%s<<#%d", reg_names[REGNO (index)], log);
          }
        }
        else if(   GET_CODE (offset) == ASHIFT
                && REG_P (index)
                && GET_CODE (scale) == CONST_INT){
          fprintf(stream, "%s<<#" HOST_WIDE_INT_PRINT_DEC, reg_names[REGNO (index)], INTVAL (scale));
        }
        else {
          output_operand_lossage("hexagon_print_operand_address: invalid base+index index");
        }

        if(CONSTANT_P (base)){
          fputc('+', stream);
          fputc('#', stream);
          if(hexagon_extended_constant){
            fputc('#', stream);
          }
          output_addr_const(stream, base);
        }
        break;
      }

    case POST_INC:
      if(G_REG_P (XEXP (x, 0))){
        fprintf(stream, "%s++#%d", reg_names[REGNO (XEXP (x, 0))], GET_MODE_SIZE (hexagon_address_mode));
      }
      else {
        output_operand_lossage("hexagon_print_operand_address: invalid post increment base register");
      }
      break;

    case POST_DEC:
      if(G_REG_P (XEXP (x, 0))){
        fprintf(stream, "%s++#-%d", reg_names[REGNO (XEXP (x, 0))], GET_MODE_SIZE (hexagon_address_mode));
      }
      else {
        output_operand_lossage("hexagon_print_operand_address: invalid post decrement base register");
      }
      break;

    case POST_MODIFY:
      {
        rtx basereg = XEXP (x, 0);
        rtx mod = XEXP (x, 1);
        if(G_REG_P (XEXP (x, 0))){
          fprintf(stream, "%s++#" HOST_WIDE_INT_PRINT_DEC, reg_names[REGNO (basereg)], INTVAL (XEXP (mod, 1)));
        }
        else {
          output_operand_lossage("hexagon_print_operand_address: invalid post modify base register");
        }
      }
      break;

    case SYMBOL_REF:
    case CONST:
    case CONST_INT:
    case LABEL_REF:
      if(sdata_symbolic_operand(x, Pmode) && !hexagon_extended_constant){
        fputc('#', stream);
      }
      else if(CONSTANT_P (x)){
        fputs("##", stream);
      }
      output_addr_const(stream, x);
      break;

    default:
      output_operand_lossage("hexagon_print_operand_address: invalid address code 0x%x", GET_CODE (x));
      break;
  }
}

static bool
hexagon_output_addr_const_extra (FILE *fp, rtx x)
{
  if (GET_CODE (x) == UNSPEC && XINT (x, 1) == UNSPEC_PIC_SYM_GOTOFF)
    {
      output_addr_const (fp, XVECEXP (x, 0, 0));
      fputs ("@GOTOFF", fp);
      return TRUE;
    }

  return FALSE;
}


/*----------------------
Miscellaneous Parameters
----------------------*/

static void
hexagon_fixup_cfg(void)
{
  basic_block bb;
  rtx head_insn;
  rtx end_insn;
  rtx insn;

  FOR_EACH_BB(bb){
    head_insn = BB_HEAD (bb);
    end_insn = BB_END (bb);
    insn = end_insn;
    while(insn && NEXT_INSN (insn) != head_insn){
      if(control_flow_insn_p(insn)){
        if(insn != end_insn){
          if(PREV_INSN (insn)){
            NEXT_INSN (PREV_INSN (insn)) = NEXT_INSN (insn);
          }
          PREV_INSN (NEXT_INSN (insn)) = PREV_INSN (insn);
          PREV_INSN (insn) = end_insn;
          NEXT_INSN (insn) = NEXT_INSN (end_insn);
          NEXT_INSN (end_insn) = insn;
          if(NEXT_INSN (insn)){
            PREV_INSN (NEXT_INSN (insn)) = insn;
          }
          BB_END (bb) = insn;
        }
        break;
      }
      insn = PREV_INSN (insn);
    }
  }
}

#if 0
/* FIXME: This code is unused at present, and changes to hardware loop support
   have made it bitrotten.  If we still need this later on, it will need
   attention.  */

/* ggan:
 * Called in sms_schedule(), after loop_version.
 */
void
hexagon_duplicate_doloop_begin(basic_block condition_bb, struct loop *loop)
{

    edge true_edge = NULL;
    edge false_edge  = NULL;
    rtx begin_loop_insn = NULL;
    rtx end_loop_insn = NULL;
    rtx insn = NULL;
    int opcode = 0;
    int begin_tag = 0;
    int end_tag = 0;
    rtx new_begin_loop_insn = NULL;
    basic_block loop_setup_bb, new_loop_header, new_loop_preheader;

    gcc_assert (condition_bb != NULL);

    /* Get TRUE edge and FALSE edge of the cond bb */
    extract_cond_bb_edges (condition_bb, &true_edge, &false_edge);

    gcc_assert (true_edge != NULL);
    gcc_assert (false_edge != NULL);

    /* Get the bb that has the doloop_begin insn */
    loop_setup_bb = single_pred (condition_bb);

    /* Get the loop body bb of the duplicated loop. Because it is a
     * single bb loop, so loop header bb is the loop body
     */
    new_loop_header = get_bb_copy (loop->header);

    /* Get the preheader bb of the duplicated loop */
    new_loop_preheader = false_edge->dest;

    /* find out the endloop0 insn */
    FOR_BB_INSNS_REVERSE(new_loop_header, insn)
    {
        if (!INSN_P (insn)) continue;

        opcode = recog_memoized(insn);

        /* SWP is only performed on Single BB Loop.
         * It is not possible to have nested loops
         */
        if (opcode == CODE_FOR_endloop0)
        {
            end_loop_insn = insn;
            break;
        }
    }

    gcc_assert (end_loop_insn != NULL);

    /* find out the loop0 insn */
    FOR_BB_INSNS_REVERSE(loop_setup_bb, insn)
    {
        if (!INSN_P (insn)) continue;

        if (INSN_CODE (insn) == CODE_FOR_doloop_begin0 ||
            INSN_CODE (insn) == CODE_FOR_doloop_begin0_pic)
        {
            begin_loop_insn = insn;
            break;
        }
    }

    gcc_assert (begin_loop_insn != NULL);

    /* extract the tag for the original doloop */
    begin_tag = INTVAL (SET_SRC (XVECEXP (PATTERN (begin_loop_insn), 0, 1)));
    end_tag = INTVAL (XVECEXP (XVECEXP (PATTERN (end_loop_insn), 0, 2), 0, 1));
    gcc_assert (begin_tag == end_tag);

    /* insert the copy of the loop0 insn into the new preheader bb */
    new_begin_loop_insn = emit_copy_of_insn_after (begin_loop_insn, BB_END(new_loop_preheader));

#define DOLOOP_TAG_OFFSET   100000

    /* adjust the tag to make sure it is unique
     * currently, we just offset the original tag by 100000
     */
    SET_SRC (XVECEXP (PATTERN (new_begin_loop_insn), 0, 1)) = GEN_INT (end_tag + DOLOOP_TAG_OFFSET);
    XVECEXP (XVECEXP (PATTERN (end_loop_insn), 0, 2), 0, 1) = GEN_INT (end_tag + DOLOOP_TAG_OFFSET);

    return;
}
#endif

/* Determine if there might be a .falign inserted before INSN.  This will tend
   to overestimate the number of .faligns for HEXAGON_FALIGN_LOOPS --
   hexagon_insert_faligns only inserts them before labels which are
   backwards-jump targets (an approximation to labels which are loop headers). 
   That is OK for our purposes though (estimating code size in
   hwloop_optimize).  */

static bool
hexagon_falign_maybe_precedes_insn_p (rtx insn)
{
  if ((hexagon_falign == HEXAGON_FALIGN_LABELS
       || hexagon_falign == HEXAGON_FALIGN_ALL
       || hexagon_falign == HEXAGON_FALIGN_LOOPS)
      && LABEL_P (insn))
    {
      rtx next = next_real_insn (insn);

      if (next && !JUMP_TABLE_DATA_P (next))
        return true;
    }
  
  return false;
}

/* Maximum distance between loopN instruction and loop start label (if we don't
   have a scratch register).  This is a little approximate, but matches the
   setting in the loop0 (etc.) patterns.  */
#define MAX_LOOP_SETUP_OFFSET 200

/* Maximum nesting for hardware loops.  */
#define MAX_LOOP_DEPTH 2

/* Return true if saN/lcN are used in the given BB.  */
static bool
salc_used (hwloop_info loops, hwloop_info subject, basic_block bb)
{
  hwloop_info loop;

  if (dump_file)
    fprintf (dump_file, ";; salc_used: checking BB %d at depth %d\n",
	     bb->index, subject->depth);

  for (loop = loops; loop; loop = loop->next)
    {
      if (loop->loop_no == subject->loop_no
	  || loop->bad)
	continue;

      if (dump_file)
	fprintf (dump_file, ";;\tchecking loop %d with depth %d\n",
		 loop->loop_no, loop->depth);

      /* The depth of the subject loop is known.  If this loop's depth is
	 known and does not match the subject loop, there is no clobber.  */
      if (loop->depth != 0 && loop->depth != subject->depth)
	{
	  if (dump_file)
	    fprintf (dump_file, ";;\t\tdifferent depth\n");
	  continue;
	}

      if (loop->depth == 0)
	{
	  /* This loop's depth is not known yet.
	     If this loop  contains the subject loop, they are at different
	     depths and there is no clobber.  */
	  int ix;
	  hwloop_info inner;
	  bool contains = false;

	  /* This is normally a very short vector.  */
	  for (ix = 0; VEC_iterate (hwloop_info, loop->loops, ix, inner); ++ix)
	    contains |= inner->loop_no ==  subject->loop_no;

	  if (contains)
	    {
	      if (dump_file)
		fprintf (dump_file, ";;\t\tcontains\n");
	      continue;
	    }
	}

      if (bitmap_bit_p (loop->block_bitmap, bb->index))
	{
	  if (dump_file)
	    {
	      fprintf (dump_file,
		       ";;\t\tsa%d/lc%d are already in use\n",
		       subject->depth - 1, subject->depth - 1);
	    }
	  return true;
	}
    }
  return false;
}

/* This function implements hardware loop-conversion for Hexagon.  It borrows
   heavily from the function of the same name in bfin.c.  */

static bool
hwloop_optimize (hwloop_info loops, hwloop_info loop)
{
  basic_block bb;
  rtx iter_reg, start_label;
  bool clobber0, clobber1;
  rtx loop_init, seq, seq_end;
  rtx insn, last_insn, iter_init, iter_init_insn;
  rtx lc_reg, sa_reg, scratchreg, alt_scratchreg;
  bool scratch_reg_needed = false;
  int i;

  if (dump_file)
    fprintf (dump_file, ";; hwloop_optimize: loop %d at depth %d\n",
	     loop->loop_no, loop->depth);
  if (loop->depth > MAX_LOOP_DEPTH)
    {
      if (dump_file)
        fprintf (dump_file, ";; loop %d too deep\n", loop->loop_no);
      return false;
    }

  if (loop->has_call || loop->has_asm)
    return false;

  /* Check that none of the blocks use iter_reg.  */
  if (loop->iter_reg_used || loop->iter_reg_used_outside)
    {
      if (dump_file)
        fprintf (dump_file, ";; loop %d uses iterator\n", loop->loop_no);
      return false;
    }

  /* Check that the BB containing the loop setup (loopN) is not already using
     the required saN / lcN control regs (Issues 15728 & 15820).  */
  if (loop->incoming_src)
    {
      if (salc_used (loops, loop, loop->incoming_src))
	return false;
    }
  else
    {
      edge e;
      edge_iterator ei;

      FOR_EACH_EDGE (e, ei, loop->incoming)
	{
	  if (salc_used (loops, loop, e->src))
	    return false;
	}
    }
  if (dump_file)
    fprintf (dump_file, ";; loop %d no sa/lc clobber\n", loop->loop_no);

  /* Get the loop iteration register.  */
  iter_reg = loop->iter_reg;
  gcc_assert (REG_P (iter_reg));
  if (dump_file)
    fprintf (dump_file, ";; loop %d iter_reg=%d\n",
	     loop->loop_no, REGNO (iter_reg));

  scratchreg = NULL_RTX;
  alt_scratchreg = NULL_RTX;

  /* Try to scrounge a register to use as a scratch for initialising the saN
     register when it is out of range of the loopN instruction.  If there is
     none, we can return false and fall back to the non-hardware loop
     implementation.
     If we have a register-register move of the iteration count which is
     redundant and therefore can be deleted -- we can use the source register
     directly to set lcN -- that source register might be the same as the
     scratch we obtain here.  In that case, try to get an alternative register
     to use instead.  */

  if (loop->incoming_src)
    {
      basic_block bb_in = loop->incoming_src;

      for (i = 0; i < 32; i++)
	{
	  if ((df_regs_ever_live_p (i) || call_used_regs[i])
	      && !REGNO_REG_SET_P (df_get_live_out (bb_in), i))
	    {
	      if (scratchreg == NULL_RTX)
	        scratchreg = gen_rtx_REG (SImode, i);
	      else
	        {
		  alt_scratchreg = gen_rtx_REG (SImode, i);
	          break;
		}
	    }
	}
    }

  if (dump_file)
    {
      if (alt_scratchreg)
	fprintf (dump_file, ";; scrounged register %d (or alternatively %d) "
		 "for scratch in loop %d\n", REGNO (scratchreg),
		 REGNO (alt_scratchreg), loop->loop_no);
      else if (scratchreg)
	fprintf (dump_file, ";; scrounged register %d for scratch in loop %d\n",
		 REGNO (scratchreg), loop->loop_no);
      else
        fprintf (dump_file, ";; no register available for scratch in loop %d\n",
		 loop->loop_no);
    }

  /* Try to figure out the offset between the loopN instruction and the
     loop's start label.  */
  if (loop->incoming_src)
    {
      int length = 0;

      insn = BB_END (loop->incoming_src);

      /* If we have to insert the loopN before a jump, count that jump in the
	 length.  */
      if (VEC_length (edge, loop->incoming) > 1
	  || !(VEC_last (edge, loop->incoming)->flags & EDGE_FALLTHRU))
	{
	  /* Note that EDGE_FALLTHRU may not be reliable because of a loopN
	     appended to the source block.  So we test explicitly for a jump.
	     See Issue 15711.  */
	  if (JUMP_P (insn))
	    insn = PREV_INSN (insn);
	  else if (dump_file)
	    {
	      fprintf (dump_file, ";; loop %d incoming src is NOT JUMP_P\n",
		       loop->loop_no);
	    }
	}

      for (; insn && insn != loop->start_label; insn = NEXT_INSN (insn))
	{
	  if (hexagon_falign_maybe_precedes_insn_p (insn))
	    length += 12;
	  if (NONDEBUG_INSN_P (insn))
	    length += get_attr_length (insn);
	}

      if (!insn)
        {
	  if (dump_file)
	    fprintf (dump_file,
		     ";; loop %d loopN instruction not before loop_start\n",
		     loop->loop_no);
	  return false;
	}

      if (dump_file)
        fprintf (dump_file,
		 ";; loop %d loopN instruction %d bytes away from loop_start\n",
		 loop->loop_no, length);

      scratch_reg_needed = (length > MAX_LOOP_SETUP_OFFSET);

      if (!scratchreg && scratch_reg_needed)
        {
	  if (dump_file)
	    fprintf (dump_file, ";; loopN is out of range for loop %d and "
		     "we have no scratch register\n", loop->loop_no);
	  return false;
	}
    }

  iter_init = iter_reg;
  iter_init_insn = NULL_RTX;

  /* Find where the iteration register was set (if to a constant), in case we
     can load the value as an immediate in the loopN instruction.  If the
     iteration register was set to the value of another register, try to use
     the latter directly.  */
  if (loop->incoming_src)
    {
      basic_block bb_in = loop->incoming_src;

      for (insn = BB_END (bb_in); insn != BB_HEAD (bb_in);
	   insn = PREV_INSN (insn))
	{
	  rtx set, x;
	  if (NOTE_P (insn))
	    continue;
	  if (BARRIER_P (insn) || JUMP_P (insn))
	    break;
	  set = single_set (insn);
	  x = PATTERN (insn);
	  if (set && rtx_equal_p (SET_DEST (set), iter_reg))
	    {
	      if (REG_P (SET_SRC (set))
		  || (GET_CODE (SET_SRC (set)) == CONST_INT
		      && INTVAL (SET_SRC (set)) >= 0
		      && INTVAL (SET_SRC (set)) < 1024))
	        {
		  iter_init = SET_SRC (set);
		  iter_init_insn = insn;
		}
	      break;
	    }
	  else if (GET_CODE (x) == PARALLEL
		   && XVECLEN (x, 0) == 2
		   && GET_CODE (XVECEXP (x, 0, 0)) == SET
		   && GET_CODE (XVECEXP (x, 0, 1)) == SET
		   && (rtx_equal_p (SET_DEST (XVECEXP (x, 0, 0)), iter_reg)
		       || rtx_equal_p (SET_DEST (XVECEXP (x, 0, 1)), iter_reg)))
	    {
	      /* The loop iteration count initialisation operation is often
	         combined with another, unrelated constant load or register
		 move: if we can load the constant directly in the loop0/loop1
		 instruction, we can replace the combine operation with a
		 simple set for the other part.  */

	      if (rtx_equal_p (SET_DEST (XVECEXP (x, 0, 0)), iter_reg))
	        set = XVECEXP (x, 0, 0);
	      else
	        set = XVECEXP (x, 0, 1);

	      if (dump_file)
		fprintf (dump_file, ";; iter count via combine r%d = %c%lld\n",
			 REGNO (SET_DEST (set)),
			 REG_P (SET_SRC (set)) ? 'r' : '#',
			 REG_P (SET_SRC (set)) ?
			   (long long) REGNO (SET_SRC (set)) :
			   INTVAL (SET_SRC (set)));

	      /* If the source is a reg that is not also set by this combine
		 or is a small constant, we can use this source.  */
	      if ((REG_P (SET_SRC (set))
		   && REGNO (SET_DEST (XVECEXP (x, 0, 0))) != REGNO (SET_SRC (set))
		   && REGNO (SET_DEST (XVECEXP (x, 0, 1))) != REGNO (SET_SRC (set)))
		  || (GET_CODE (SET_SRC (set)) == CONST_INT
		      && INTVAL (SET_SRC (set)) >= 0
		      && INTVAL (SET_SRC (set)) < 1024))
	        {
	          iter_init = SET_SRC (set);
		  iter_init_insn = insn;
		}
	      break;
	    }
	  else if (reg_mentioned_p (iter_reg, PATTERN (insn)))
	    break;
	}

      /* Make sure the register used to initialise the loop iteration count
	 isn't clobbered before we reach the loopN instruction.  */
      if (iter_init_insn && REG_P (iter_init)
	  && reg_set_between_p (iter_init, iter_init_insn,
				NEXT_INSN (BB_END (loop->incoming_src))))
	{
	  if (dump_file)
	    fprintf (dump_file, ";; iteration count register clobbered before "
		     "loopN insn for loop %d, will not delete initializer\n",
		     loop->loop_no);
	  iter_init_insn = NULL;
	  iter_init = iter_reg;
	}

      /* Make sure there's no collision between the register we're trying to
         copy-propagate and the scratch register we chose above.  */
      if (iter_init_insn && REG_P (iter_init) && scratchreg != NULL_RTX
	  && rtx_equal_p (scratchreg, iter_init))
	{
	  if (dump_file)
	    fprintf (dump_file, ";; scratch register %d for loop %d collides "
		     "with copy-propagated iteration count:\n",
		     REGNO (scratchreg), loop->loop_no);

	  /* There are several strategies for dealing with this issue --
	     in order of preference...  */
	  if (!scratch_reg_needed)
	    {
	      if (dump_file)
	        fprintf (dump_file, ";; discarding unneeded scratch "
			 "register\n");
	      /* We don't need the scratch register (the loop start label is
	         in range of the loopN instruction), but we can't leave it in
	         the insn pattern else constraints (which use an earlyclobber
		 for the operand in question) will fail to match.  */
	      scratchreg = NULL_RTX;
	    }
	  else if (alt_scratchreg != NULL_RTX)
	    {
	      if (dump_file)
	        fprintf (dump_file, ";; using alternative scratch "
			 "register %d\n", REGNO (alt_scratchreg));
	      scratchreg = alt_scratchreg;
	    }
	  else
	    {
	      if (dump_file)
	        fprintf (dump_file, ";; no alternative scratch register, "
			 "undoing copy propagation\n");
	      /* This is the only case which leads to worse code.  It probably
	         won't trigger much in practice.  */
	      iter_init_insn = NULL;
	      iter_init = iter_reg;
	    }
	}
    }

  clobber0 = (TEST_HARD_REG_BIT (loop->regs_set_in_loop, LC0_REGNUM)
	      || TEST_HARD_REG_BIT (loop->regs_set_in_loop, SA0_REGNUM));
  clobber1 = (TEST_HARD_REG_BIT (loop->regs_set_in_loop, LC1_REGNUM)
	      || TEST_HARD_REG_BIT (loop->regs_set_in_loop, SA1_REGNUM));
  if ((loop->depth == 1 && clobber0) || (loop->depth == 2 && clobber1))
    {
      if (dump_file)
        fprintf (dump_file, ";; loop %d clobbers hw looping regs\n",
		 loop->loop_no);
      return false;
    }

  bb = loop->tail;
  last_insn = PREV_INSN (loop->loop_end);

  while (1)
    {
      for (; last_insn != BB_HEAD (bb); last_insn = PREV_INSN (last_insn))
	if (NONDEBUG_INSN_P (last_insn))
	  break;

      if (last_insn != BB_HEAD (bb))
        break;

      if (single_pred_p (bb)
	  && (single_pred_edge (bb)->flags & EDGE_FALLTHRU)
	  && single_pred (bb) != ENTRY_BLOCK_PTR)
	{
	  bb = single_pred (bb);
	  last_insn = BB_END (bb);
	  continue;
	}
      else
        {
	  last_insn = NULL_RTX;
	  break;
	}
    }

  if (!last_insn)
    {
      if (dump_file)
        fprintf (dump_file, ";; loop %d has no last instruction\n",
		 loop->loop_no);
      return false;
    }

  loop->last_insn = last_insn;

  /* The loop is good for replacement.  */
  start_label = loop->start_label;
  iter_reg = loop->iter_reg;

  /* Create a sequence containing the loop setup.  */
  start_sequence ();

  if (loop->depth == 1)
    {
      lc_reg = gen_rtx_REG (SImode, LC0_REGNUM);
      sa_reg = gen_rtx_REG (SImode, SA0_REGNUM);
    }
  else
    {
      lc_reg = gen_rtx_REG (SImode, LC1_REGNUM);
      sa_reg = gen_rtx_REG (SImode, SA1_REGNUM);
    }

  loop_init = gen_rtx_PARALLEL (VOIDmode, rtvec_alloc (flag_pic ? 4 : 3));

  XVECEXP (loop_init, 0, 0) = gen_rtx_SET (VOIDmode, lc_reg, iter_init);
  XVECEXP (loop_init, 0, 1) = gen_rtx_SET (VOIDmode, sa_reg,
				gen_rtx_LABEL_REF (Pmode, start_label));
  XVECEXP (loop_init, 0, 2) = gen_rtx_CLOBBER (SImode,
				scratchreg ? scratchreg
					   : gen_rtx_SCRATCH (SImode));
  if (flag_pic)
    XVECEXP (loop_init, 0, 3) = gen_rtx_USE (VOIDmode,
					     gen_rtx_REG (SImode, PIC_REGNUM));

  seq_end = emit_insn (loop_init);

  if (dump_file)
    {
      fprintf (dump_file, ";; replacing loop %d initializer with\n",
	       loop->loop_no);
      print_rtl_single (dump_file, loop_init);
    }

  /* If the loop isn't entered at the top, also create a jump to the entry
     point.  */
  if (!loop->incoming_src && loop->head != loop->incoming_dest)
    {
      rtx label = BB_HEAD (loop->incoming_dest);
      emit_jump_insn (gen_jump (label));
      seq_end = emit_barrier ();
    }

  seq = get_insns ();
  end_sequence ();

  if (loop->incoming_src)
    {
      rtx prev = BB_END (loop->incoming_src);
      if (VEC_length (edge, loop->incoming) > 1
	  || !(VEC_last (edge, loop->incoming)->flags & EDGE_FALLTHRU))
	{
	  /* The loop setup will be before the incoming edge.  */
	  if (JUMP_P (prev))
	    prev = PREV_INSN (prev);
	}

      for (; prev != BB_HEAD (loop->incoming_src); prev = PREV_INSN (prev))
	if (NONDEBUG_INSN_P (prev))
	  break;

      emit_insn_after (seq, prev);
    }
  else
    {
      basic_block new_bb;
      edge e;
      edge_iterator ei;

#ifdef ENABLE_CHECKING
      if (loop->head != loop->incoming_dest)
	{
	  /* We aren't entering the loop at the top.  Since we've established
	     that the loop is entered only at one point, this means there
	     can't be fallthru edges into the head.  Any such fallthru edges
	     would become invalid when we insert the new block, so verify
	     that this does not in fact happen.  */
	  FOR_EACH_EDGE (e, ei, loop->head->preds)
	    gcc_assert (!(e->flags & EDGE_FALLTHRU));
	}
#endif

      /* In this case, we assume that the loopN instruction will be within
         range of the loop_start label.  */

      emit_insn_before (seq, BB_HEAD (loop->head));
      seq = emit_label_before (gen_label_rtx (), seq);

      new_bb = create_basic_block (seq, seq_end, loop->head->prev_bb);
      FOR_EACH_EDGE (e, ei, loop->incoming)
        {
	  if (!(e->flags & EDGE_FALLTHRU)
	      || e->dest != loop->head)
	    redirect_edge_and_branch_force (e, new_bb);
	  else
	    redirect_edge_succ (e, new_bb);
	}
      make_edge (new_bb, loop->head, 0);
    }

  /* Replace the loop end sequence.  */
  if (loop->depth == 1)
    insn = gen_endloop0 (start_label);
  else
    insn = gen_endloop1 (start_label);

  if (dump_file)
    {
      fprintf (dump_file, ";; replacing loop %d terminator with\n",
	       loop->loop_no);
      print_rtl_single (dump_file, insn);
    }

  emit_jump_insn_before (insn, loop->loop_end);

  delete_insn (loop->loop_end);

  return true;
}

static void
hwloop_fail (hwloop_info loop)
{
  rtx insn, test, pred;

  gcc_assert (recog_memoized (loop->loop_end) == CODE_FOR_doloop_end_internal);

  extract_insn (loop->loop_end);

  pred = recog_data.operand[3];

  gcc_assert (REG_P (pred));

  /* TODO: We could do slightly better than this on Hexagon v4, by using
     "new value" register comparison-with-zero instructions.  Unfortunately
     the label range on those is quite short, so we wouldn't be able to use
     them unconditionally.  */

  emit_insn_before (gen_addsi3 (loop->iter_reg, loop->iter_reg, constm1_rtx),
		    loop->loop_end);
  emit_insn_before (gen_rtx_SET (VOIDmode, pred,
				 gen_rtx_GTU (BImode, loop->iter_reg,
				 const0_rtx)),
		    loop->loop_end);
  test = gen_rtx_SET (VOIDmode, pc_rtx,
	   gen_rtx_IF_THEN_ELSE (VOIDmode,
				 gen_rtx_NE (VOIDmode, pred, const0_rtx),
				 gen_rtx_LABEL_REF (Pmode, loop->start_label),
				 pc_rtx));
  insn = emit_jump_insn_before (test, loop->loop_end);
  JUMP_LABEL (insn) = loop->start_label;
  /* The jump is probably taken (see also hexagon_branch_hint).  */
  add_reg_note (insn, REG_BR_PROB, GEN_INT (REG_BR_PROB_BASE));
  LABEL_NUSES (loop->start_label)++;
  delete_insn (loop->loop_end);
}

static rtx
hwloop_pattern_reg (rtx insn)
{
  rtx pat, reg;

  if (!JUMP_P (insn) || recog_memoized (insn) != CODE_FOR_doloop_end_internal)
    return NULL_RTX;

  pat = PATTERN (insn);
  reg = SET_DEST (XVECEXP (PATTERN (insn), 0, 1));

  if (!REG_P (reg))
    return NULL_RTX;

  return reg;
}

static struct hw_doloop_hooks hexagon_doloop_hooks =
{
  hwloop_pattern_reg,
  hwloop_optimize,
  hwloop_fail
};

static void
hexagon_reorg_loops (FILE *dump_file ATTRIBUTE_UNUSED)
{
  reorg_loops (true, &hexagon_doloop_hooks);
}

/* 1 if RTX is a code_label that is a target for a loop jump.  */
#define LABEL_START_LOOP(RTX)                                           \
  (RTL_FLAG_CHECK2("LABEL_START_LOOP", (RTX), CODE_LABEL, NOTE)->return_val)

static void
hexagon_insert_faligns(void)
{
  rtx insn;
  rtx next;
  rtx label;

  if(hexagon_falign == HEXAGON_FALIGN_LABELS || hexagon_falign == HEXAGON_FALIGN_ALL){
    for(insn = get_insns(); insn; insn = NEXT_INSN (insn)){
      if(LABEL_P (insn)){
        if(!(next = next_real_insn(insn))){
          break;
        }
        if(!JUMP_TABLE_DATA_P (next)){
          emit_insn_before(gen_falign(), insn);
        }
        insn = next;
      }
    }
  }
  else if(hexagon_falign == HEXAGON_FALIGN_LOOPS){

    /* Go backwards and falign only labels that are targets for backward
       jumps. */
    for(insn = get_last_insn(); insn; insn = PREV_INSN (insn)){
      if(JUMP_P (insn)){
        /* If label first seen in jump, mark it */
        label = JUMP_LABEL (insn);
        if(label){
          LABEL_START_LOOP (label) = 1;
        }
      }
      else if(LABEL_P (insn)){
        if(LABEL_START_LOOP (insn)
           && !JUMP_TABLE_DATA_P (next_real_insn(insn))){
          emit_insn_before(gen_falign(), insn);
        }
      }
    }
  }
}

#undef LABEL_START_LOOP


/* Implements hook TARGET_MACHINE_DEPENDENT_REORG */

static void
hexagon_machine_dependent_reorg(void)
{
  compute_bb_for_insn();

#if 0
  verify_flow_info();
#endif /* 0 */

  if(cfun->machine->has_hardware_loops
     || TARGET_PACKET_OPTIMIZATIONS){
    hexagon_fixup_cfg();
  }

  if (TARGET_LOCAL_COMBINE)
    {
      hexagon_local_combine_pass();
    }

  if (TARGET_LOCAL_COMBINE_IMMEDIATE)
    {
      hexagon_local_combine_immediate_pass();
    }

  if(TARGET_PACKET_OPTIMIZATIONS){
    hexagon_packet_optimizations();
  }

  df_analyze ();

  if (TARGET_HARDWARE_LOOPS && cfun->machine->has_hardware_loops)
    hexagon_reorg_loops (dump_file);

  /* Insert .faligns. */
  if(hexagon_falign != HEXAGON_NO_FALIGN){
    hexagon_insert_faligns();
  }

  if (flag_var_tracking)
    {
      timevar_push (TV_VAR_TRACKING);
      variable_tracking_main ();
      timevar_pop (TV_VAR_TRACKING);
    }
  df_finish_pass (false);

#if 0
  verify_flow_info();
#endif /* 0 */
}




#define def_builtin(NAME, TYPE, CODE) \
  do { \
    tree t; \
    t = add_builtin_function(NAME, TYPE, END_BUILTINS + 1 + CODE, \
                             BUILT_IN_MD, NULL, \
                             tree_cons(get_identifier("const"), \
                                       NULL_TREE, NULL_TREE));  \
    if (CODE == HEXAGON_BUILTIN_val_for_valignb) { \
      hexagon_builtin_mask_for_load = t; \
    } \
  } while (0);
#define def_builtin_nonconst(NAME, TYPE, CODE) \
  do { \
    tree t; \
    t = add_builtin_function(NAME, TYPE, END_BUILTINS + 1 + CODE, \
                             BUILT_IN_MD, NULL, NULL_TREE);  \
    if (CODE == HEXAGON_BUILTIN_val_for_valignb) { \
      hexagon_builtin_mask_for_load = t; \
    } \
  } while (0);

#define PTR_type_node ptr_type_node
#define BI_type_node boolean_type_node
#define QI_type_node char_type_node
#define HI_type_node short_integer_type_node
#define SI_type_node integer_type_node
#define USI_type_node unsigned_type_node
#define DI_type_node long_long_integer_type_node
#define UDI_type_node long_long_unsigned_type_node

#define SF_type_node float_type_node
#define DF_type_node double_type_node

/* Implements hook TARGET_INIT_BUILTINS */

static void
hexagon_init_builtins(void)
{
  tree endlink ATTRIBUTE_UNUSED;
  tree QI_ftype_MEM      ATTRIBUTE_UNUSED;
  tree void_ftype_SI     ATTRIBUTE_UNUSED;
  tree void_ftype_SISI   ATTRIBUTE_UNUSED;
  tree void_ftype_SISISI ATTRIBUTE_UNUSED;
  tree HI_ftype_SI       ATTRIBUTE_UNUSED;
  tree SI_ftype_SI       ATTRIBUTE_UNUSED;
  tree SI_ftype_SISI     ATTRIBUTE_UNUSED;
  tree USI_ftype_SISI    ATTRIBUTE_UNUSED;
  tree SI_ftype_SISISI   ATTRIBUTE_UNUSED;
  tree DI_ftype_SI       ATTRIBUTE_UNUSED;
  tree DI_ftype_SISI     ATTRIBUTE_UNUSED;
  tree UDI_ftype_SISI    ATTRIBUTE_UNUSED;
  tree DI_ftype_SIDI     ATTRIBUTE_UNUSED;
  tree DI_ftype_SISISI   ATTRIBUTE_UNUSED;
  tree SI_ftype_DI       ATTRIBUTE_UNUSED;
  tree SI_ftype_DISI     ATTRIBUTE_UNUSED;
  tree SI_ftype_DISISI   ATTRIBUTE_UNUSED;
  tree DI_ftype_DI       ATTRIBUTE_UNUSED;
  tree DI_ftype_DISI     ATTRIBUTE_UNUSED;
  tree DI_ftype_DISISI   ATTRIBUTE_UNUSED;
  tree SI_ftype_SIDI     ATTRIBUTE_UNUSED;
  tree SI_ftype_DIDI     ATTRIBUTE_UNUSED;
  tree DI_ftype_DIDI     ATTRIBUTE_UNUSED;
  tree UDI_ftype_DIDI    ATTRIBUTE_UNUSED;
  tree SI_ftype_SIDISI   ATTRIBUTE_UNUSED;
  tree DI_ftype_DIDISI   ATTRIBUTE_UNUSED;
  tree SI_ftype_SIDIDI   ATTRIBUTE_UNUSED;
  tree DI_ftype_DIDIDI   ATTRIBUTE_UNUSED;
  tree SI_ftype_SISISISI ATTRIBUTE_UNUSED;
  tree DI_ftype_DIDISISI ATTRIBUTE_UNUSED;
  tree SI_ftype_SISIDI   ATTRIBUTE_UNUSED;

  tree QI_ftype_SISI     ATTRIBUTE_UNUSED;
  tree QI_ftype_SIDI     ATTRIBUTE_UNUSED;
  tree QI_ftype_DISI     ATTRIBUTE_UNUSED;
  tree QI_ftype_DIDI     ATTRIBUTE_UNUSED;
  tree QI_ftype_QI       ATTRIBUTE_UNUSED;
  tree QI_ftype_SI       ATTRIBUTE_UNUSED;
  tree QI_ftype_DI       ATTRIBUTE_UNUSED;
  tree QI_ftype_QIQI     ATTRIBUTE_UNUSED;
  tree QI_ftype_QIQIQI   ATTRIBUTE_UNUSED;
  tree SI_ftype_QIQI     ATTRIBUTE_UNUSED;
  tree SI_ftype_QISISI   ATTRIBUTE_UNUSED;
  tree SI_ftype_QISI     ATTRIBUTE_UNUSED;
  tree DI_ftype_QISISI   ATTRIBUTE_UNUSED;
  tree DI_ftype_QIDIDI   ATTRIBUTE_UNUSED;
  tree DI_ftype_QI       ATTRIBUTE_UNUSED;
  tree SI_ftype_QI       ATTRIBUTE_UNUSED;
  tree DI_ftype_DIDIQI   ATTRIBUTE_UNUSED;

  tree DF_ftype_DF ATTRIBUTE_UNUSED;
  tree DF_ftype_DFDF ATTRIBUTE_UNUSED;
  tree DF_ftype_DFDFDF ATTRIBUTE_UNUSED;
  tree DF_ftype_DFDFDFQI ATTRIBUTE_UNUSED;
  tree DF_ftype_SF ATTRIBUTE_UNUSED;
  tree DF_ftype_SI ATTRIBUTE_UNUSED;
  tree QI_ftype_DFDF ATTRIBUTE_UNUSED;
  tree QI_ftype_DFSF ATTRIBUTE_UNUSED;
  tree QI_ftype_DFSI ATTRIBUTE_UNUSED;
  tree QI_ftype_SFSF ATTRIBUTE_UNUSED;
  tree QI_ftype_SFSI ATTRIBUTE_UNUSED;
  tree SF_ftype_SF ATTRIBUTE_UNUSED;
  tree SF_ftype_DF ATTRIBUTE_UNUSED;
  tree SF_ftype_SI ATTRIBUTE_UNUSED;
  tree SF_ftype_SFSF ATTRIBUTE_UNUSED;
  tree SF_ftype_SFSFSF ATTRIBUTE_UNUSED;
  tree SF_ftype_SFSFSFQI ATTRIBUTE_UNUSED;

  tree SF_ftype_DI ATTRIBUTE_UNUSED;
  tree DF_ftype_DI ATTRIBUTE_UNUSED;
  tree SI_ftype_SF ATTRIBUTE_UNUSED;
  tree DI_ftype_SF ATTRIBUTE_UNUSED;
  tree SI_ftype_DF ATTRIBUTE_UNUSED;
  tree DI_ftype_DF ATTRIBUTE_UNUSED;

  tree PTR_ftype_PTRPTRSISI   ATTRIBUTE_UNUSED;

  tree PTR_ftype_PTRDISISI   ATTRIBUTE_UNUSED;
  tree PTR_ftype_PTRSISISI   ATTRIBUTE_UNUSED;
  tree PTR_ftype_PTRHISISI   ATTRIBUTE_UNUSED;
  tree PTR_ftype_PTRQISISI   ATTRIBUTE_UNUSED;

  tree PTR_ftype_PTRPTRSI   ATTRIBUTE_UNUSED;

  tree PTR_ftype_PTRDISI   ATTRIBUTE_UNUSED;
  tree PTR_ftype_PTRSISI   ATTRIBUTE_UNUSED;
  tree PTR_ftype_PTRHISI   ATTRIBUTE_UNUSED;
  tree PTR_ftype_PTRQISI   ATTRIBUTE_UNUSED;

  endlink = void_list_node;
  QI_ftype_MEM =      build_function_type( QI_type_node,
                        tree_cons(NULL_TREE, ptr_type_node,
                        endlink));
  void_ftype_SI =     build_function_type( void_type_node,
                        tree_cons(NULL_TREE, SI_type_node,
                        endlink));
  void_ftype_SISI =   build_function_type( void_type_node,
                        tree_cons(NULL_TREE, SI_type_node,
                        tree_cons(NULL_TREE, SI_type_node,
                        endlink)));
  void_ftype_SISISI = build_function_type( void_type_node,
                        tree_cons(NULL_TREE, SI_type_node,
                        tree_cons(NULL_TREE, SI_type_node,
                        tree_cons(NULL_TREE, SI_type_node,
                        endlink))));
  HI_ftype_SI =       build_function_type(   HI_type_node,
                        tree_cons(NULL_TREE, SI_type_node,
                        endlink));
  SI_ftype_SI =       build_function_type(   SI_type_node,
                        tree_cons(NULL_TREE, SI_type_node,
                        endlink));
  SI_ftype_SISI =     build_function_type(   SI_type_node,
                        tree_cons(NULL_TREE, SI_type_node,
                        tree_cons(NULL_TREE, SI_type_node,
                        endlink)));
  USI_ftype_SISI =    build_function_type(  USI_type_node,
                        tree_cons(NULL_TREE, SI_type_node,
                        tree_cons(NULL_TREE, SI_type_node,
                        endlink)));
  SI_ftype_QIQI =     build_function_type(   SI_type_node,
                        tree_cons(NULL_TREE, QI_type_node,
                        tree_cons(NULL_TREE, QI_type_node,
                        endlink)));
  SI_ftype_SISISI =   build_function_type(   SI_type_node,
                        tree_cons(NULL_TREE, SI_type_node,
                        tree_cons(NULL_TREE, SI_type_node,
                        tree_cons(NULL_TREE, SI_type_node,
                        endlink))));
  DI_ftype_SI =       build_function_type(   DI_type_node,
                        tree_cons(NULL_TREE, SI_type_node,
                        endlink));
  DI_ftype_SISI =     build_function_type(   DI_type_node,
                        tree_cons(NULL_TREE, SI_type_node,
                        tree_cons(NULL_TREE, SI_type_node,
                        endlink)));
  UDI_ftype_SISI =    build_function_type(  UDI_type_node,
                        tree_cons(NULL_TREE, SI_type_node,
                        tree_cons(NULL_TREE, SI_type_node,
                        endlink)));
  DI_ftype_SISISI =   build_function_type(   DI_type_node,
                        tree_cons(NULL_TREE, SI_type_node,
                        tree_cons(NULL_TREE, SI_type_node,
                        tree_cons(NULL_TREE, SI_type_node,
                        endlink))));
  DI_ftype_SIDI =     build_function_type(   DI_type_node,
                        tree_cons(NULL_TREE, SI_type_node,
                        tree_cons(NULL_TREE, DI_type_node,
                        endlink)));
  SI_ftype_DI =       build_function_type(   SI_type_node,
                        tree_cons(NULL_TREE, DI_type_node,
                        endlink));
  SI_ftype_DISI =     build_function_type(   SI_type_node,
                        tree_cons(NULL_TREE, DI_type_node,
                        tree_cons(NULL_TREE, SI_type_node,
                        endlink)));
  SI_ftype_DISISI =   build_function_type(   SI_type_node,
                        tree_cons(NULL_TREE, DI_type_node,
                        tree_cons(NULL_TREE, SI_type_node,
                        tree_cons(NULL_TREE, SI_type_node,
                        endlink))));
  DI_ftype_DI =       build_function_type(   DI_type_node,
                        tree_cons(NULL_TREE, DI_type_node,
                        endlink));
  DI_ftype_DISI =     build_function_type(   DI_type_node,
                        tree_cons(NULL_TREE, DI_type_node,
                        tree_cons(NULL_TREE, SI_type_node,
                        endlink)));
  DI_ftype_DISISI =   build_function_type(   DI_type_node,
                        tree_cons(NULL_TREE, DI_type_node,
                        tree_cons(NULL_TREE, SI_type_node,
                        tree_cons(NULL_TREE, SI_type_node,
                        endlink))));
  SI_ftype_SIDI =     build_function_type(   SI_type_node,
                        tree_cons(NULL_TREE, SI_type_node,
                        tree_cons(NULL_TREE, DI_type_node,
                        endlink)));
  SI_ftype_DIDI =     build_function_type(   SI_type_node,
                        tree_cons(NULL_TREE, DI_type_node,
                        tree_cons(NULL_TREE, DI_type_node,
                        endlink)));
  DI_ftype_DIDI =     build_function_type(   DI_type_node,
                        tree_cons(NULL_TREE, DI_type_node,
                        tree_cons(NULL_TREE, DI_type_node,
                        endlink)));
  UDI_ftype_DIDI =    build_function_type(  UDI_type_node,
                        tree_cons(NULL_TREE, DI_type_node,
                        tree_cons(NULL_TREE, DI_type_node,
                        endlink)));
  SI_ftype_SIDISI =   build_function_type(   SI_type_node,
                        tree_cons(NULL_TREE, SI_type_node,
                        tree_cons(NULL_TREE, DI_type_node,
                        tree_cons(NULL_TREE, SI_type_node,
                        endlink))));
  DI_ftype_DIDISI =   build_function_type(   DI_type_node,
                        tree_cons(NULL_TREE, DI_type_node,
                        tree_cons(NULL_TREE, DI_type_node,
                        tree_cons(NULL_TREE, SI_type_node,
                        endlink))));
  SI_ftype_SIDIDI =   build_function_type(   SI_type_node,
                        tree_cons(NULL_TREE, SI_type_node,
                        tree_cons(NULL_TREE, DI_type_node,
                        tree_cons(NULL_TREE, DI_type_node,
                        endlink))));
  DI_ftype_DIDIDI =   build_function_type(   DI_type_node,
                        tree_cons(NULL_TREE, DI_type_node,
                        tree_cons(NULL_TREE, DI_type_node,
                        tree_cons(NULL_TREE, DI_type_node,
                        endlink))));
  SI_ftype_SISISISI = build_function_type(   SI_type_node,
                        tree_cons(NULL_TREE, SI_type_node,
                        tree_cons(NULL_TREE, SI_type_node,
                        tree_cons(NULL_TREE, SI_type_node,
                        tree_cons(NULL_TREE, SI_type_node,
                        endlink)))));
  DI_ftype_DIDISISI = build_function_type(   DI_type_node,
                        tree_cons(NULL_TREE, DI_type_node,
                        tree_cons(NULL_TREE, DI_type_node,
                        tree_cons(NULL_TREE, SI_type_node,
                        tree_cons(NULL_TREE, SI_type_node,
                        endlink)))));
  SI_ftype_SISIDI =   build_function_type(   SI_type_node,
                        tree_cons(NULL_TREE, SI_type_node,
                        tree_cons(NULL_TREE, SI_type_node,
                        tree_cons(NULL_TREE, DI_type_node,
                        endlink))));

  QI_ftype_SISI   =   build_function_type(   QI_type_node,
                        tree_cons(NULL_TREE, SI_type_node,
                        tree_cons(NULL_TREE, SI_type_node,
                        endlink)));

  QI_ftype_SIDI   =   build_function_type(   QI_type_node,
                        tree_cons(NULL_TREE, SI_type_node,
                        tree_cons(NULL_TREE, DI_type_node,
                        endlink)));

  QI_ftype_DISI   =   build_function_type(   QI_type_node,
                        tree_cons(NULL_TREE, DI_type_node,
                        tree_cons(NULL_TREE, SI_type_node,
                        endlink)));

  QI_ftype_DIDI   =   build_function_type(   QI_type_node,
                        tree_cons(NULL_TREE, DI_type_node,
                        tree_cons(NULL_TREE, DI_type_node,
                        endlink)));

  QI_ftype_QI     =   build_function_type(   QI_type_node,
                        tree_cons(NULL_TREE, QI_type_node,
                        endlink));

  QI_ftype_SI     =   build_function_type(   QI_type_node,
                        tree_cons(NULL_TREE, SI_type_node,
                        endlink));

  QI_ftype_DI     =   build_function_type(   QI_type_node,
                        tree_cons(NULL_TREE, DI_type_node,
                        endlink));

  QI_ftype_QIQI   =   build_function_type(   QI_type_node,
                        tree_cons(NULL_TREE, QI_type_node,
                        tree_cons(NULL_TREE, QI_type_node,
                        endlink)));

  QI_ftype_QIQIQI =   build_function_type(   QI_type_node,
                        tree_cons(NULL_TREE, QI_type_node,
                        tree_cons(NULL_TREE, QI_type_node,
                        tree_cons(NULL_TREE, QI_type_node,
                        endlink))));

  SI_ftype_QISISI =   build_function_type(   SI_type_node,
                        tree_cons(NULL_TREE, QI_type_node,
                        tree_cons(NULL_TREE, SI_type_node,
                        tree_cons(NULL_TREE, SI_type_node,
                        endlink))));

  SI_ftype_QISI   =   build_function_type(   SI_type_node,
                        tree_cons(NULL_TREE, QI_type_node,
                        tree_cons(NULL_TREE, SI_type_node,
                        endlink)));

  DI_ftype_QISISI =   build_function_type(   DI_type_node,
                        tree_cons(NULL_TREE, QI_type_node,
                        tree_cons(NULL_TREE, SI_type_node,
                        tree_cons(NULL_TREE, SI_type_node,
                        endlink))));

  DI_ftype_QIDIDI =   build_function_type(   DI_type_node,
                        tree_cons(NULL_TREE, QI_type_node,
                        tree_cons(NULL_TREE, DI_type_node,
                        tree_cons(NULL_TREE, DI_type_node,
                        endlink))));

  DI_ftype_QI     =   build_function_type(   DI_type_node,
                        tree_cons(NULL_TREE, QI_type_node,
                        endlink));

  SI_ftype_QI     =   build_function_type(   SI_type_node,
                        tree_cons(NULL_TREE, QI_type_node,
                        endlink));

  DI_ftype_DIDIQI =   build_function_type(   DI_type_node,
                        tree_cons(NULL_TREE, DI_type_node,
                        tree_cons(NULL_TREE, DI_type_node,
                        tree_cons(NULL_TREE, QI_type_node,
                        endlink))));

  DF_ftype_DF =  build_function_type(   DF_type_node,
                         tree_cons(NULL_TREE, DF_type_node,
                         endlink));

  DF_ftype_DFDF =  build_function_type(   DF_type_node,
                         tree_cons(NULL_TREE, DF_type_node,
                         tree_cons(NULL_TREE, DF_type_node,
                         endlink)));

  DF_ftype_DFDFDF =  build_function_type(   DF_type_node,
                         tree_cons(NULL_TREE, DF_type_node,
                         tree_cons(NULL_TREE, DF_type_node,
                         tree_cons(NULL_TREE, DF_type_node,
                         endlink))));

  DF_ftype_DFDFDFQI =  build_function_type(   DF_type_node,
                         tree_cons(NULL_TREE, DF_type_node,
                         tree_cons(NULL_TREE, DF_type_node,
                         tree_cons(NULL_TREE, DF_type_node,
                         tree_cons(NULL_TREE, QI_type_node,
                         endlink)))));

  DF_ftype_SF =  build_function_type(   DF_type_node,
                         tree_cons(NULL_TREE, SF_type_node,
                         endlink));

  DF_ftype_SI =  build_function_type(   DF_type_node,
                         tree_cons(NULL_TREE, SI_type_node,
                         endlink));

  QI_ftype_DFDF =  build_function_type(   QI_type_node,
                         tree_cons(NULL_TREE, DF_type_node,
                         tree_cons(NULL_TREE, DF_type_node,
                         endlink)));

  QI_ftype_DFSF =  build_function_type(   QI_type_node,
                         tree_cons(NULL_TREE, DF_type_node,
                         tree_cons(NULL_TREE, SF_type_node,
                         endlink)));

  QI_ftype_DFSI =  build_function_type(   QI_type_node,
                         tree_cons(NULL_TREE, DF_type_node,
                         tree_cons(NULL_TREE, SI_type_node,
                         endlink)));

  QI_ftype_SFSF =  build_function_type(   QI_type_node,
                         tree_cons(NULL_TREE, SF_type_node,
                         tree_cons(NULL_TREE, SF_type_node,
                         endlink)));

  QI_ftype_SFSI =  build_function_type(   QI_type_node,
                         tree_cons(NULL_TREE, SF_type_node,
                         tree_cons(NULL_TREE, SI_type_node,
                         endlink)));

  SF_ftype_SF =  build_function_type(   SF_type_node,
                         tree_cons(NULL_TREE, SF_type_node,
                         endlink));

  SF_ftype_DF =  build_function_type(   SF_type_node,
                         tree_cons(NULL_TREE, DF_type_node,
                         endlink));

  SF_ftype_SI =  build_function_type(   SF_type_node,
                         tree_cons(NULL_TREE, SI_type_node,
                         endlink));

  SF_ftype_SFSF =  build_function_type(   SF_type_node,
                         tree_cons(NULL_TREE, SF_type_node,
                         tree_cons(NULL_TREE, SF_type_node,
                         endlink)));

  SF_ftype_SFSFSF =  build_function_type(   SF_type_node,
                         tree_cons(NULL_TREE, SF_type_node,
                         tree_cons(NULL_TREE, SF_type_node,
                         tree_cons(NULL_TREE, SF_type_node,
                         endlink))));

  SF_ftype_SFSFSFQI =  build_function_type(   SF_type_node,
                         tree_cons(NULL_TREE, SF_type_node,
                         tree_cons(NULL_TREE, SF_type_node,
                         tree_cons(NULL_TREE, SF_type_node,
                         tree_cons(NULL_TREE, QI_type_node,
                         endlink)))));

  SF_ftype_DI = build_function_type (SF_type_node,
                                     tree_cons (NULL_TREE, DI_type_node,
                                     endlink));

  DF_ftype_DI = build_function_type (DF_type_node,
                                     tree_cons (NULL_TREE, DI_type_node,
                                     endlink));

  SI_ftype_SF = build_function_type (SI_type_node,
                                     tree_cons (NULL_TREE, SF_type_node,
                                     endlink));

  DI_ftype_SF = build_function_type (DI_type_node,
                                     tree_cons (NULL_TREE, SF_type_node,
                                     endlink));

  SI_ftype_DF = build_function_type (SI_type_node,
                                     tree_cons (NULL_TREE, DF_type_node,
                                     endlink));

  DI_ftype_DF = build_function_type (DI_type_node,
                                     tree_cons (NULL_TREE, DF_type_node,
                                     endlink));

  PTR_ftype_PTRPTRSISI =   build_function_type(   PTR_type_node,
                        tree_cons(NULL_TREE, PTR_type_node,
                        tree_cons(NULL_TREE, PTR_type_node,
                        tree_cons(NULL_TREE, SI_type_node,
                        tree_cons(NULL_TREE, SI_type_node,
                        endlink)))));

  PTR_ftype_PTRDISISI =   build_function_type(   PTR_type_node,
                        tree_cons(NULL_TREE, PTR_type_node,
                        tree_cons(NULL_TREE, DI_type_node,
                        tree_cons(NULL_TREE, SI_type_node,
                        tree_cons(NULL_TREE, SI_type_node,
                        endlink)))));

  PTR_ftype_PTRSISISI =   build_function_type(   PTR_type_node,
                        tree_cons(NULL_TREE, PTR_type_node,
                        tree_cons(NULL_TREE, SI_type_node,
                        tree_cons(NULL_TREE, SI_type_node,
                        tree_cons(NULL_TREE, SI_type_node,
                        endlink)))));

  PTR_ftype_PTRHISISI =   build_function_type(   PTR_type_node,
                        tree_cons(NULL_TREE, PTR_type_node,
                        tree_cons(NULL_TREE, HI_type_node,
                        tree_cons(NULL_TREE, SI_type_node,
                        tree_cons(NULL_TREE, SI_type_node,
                        endlink)))));

  PTR_ftype_PTRQISISI =   build_function_type(   PTR_type_node,
                        tree_cons(NULL_TREE, PTR_type_node,
                        tree_cons(NULL_TREE, QI_type_node,
                        tree_cons(NULL_TREE, SI_type_node,
                        tree_cons(NULL_TREE, SI_type_node,
                        endlink)))));

  PTR_ftype_PTRPTRSI =   build_function_type(   PTR_type_node,
                        tree_cons(NULL_TREE, PTR_type_node,
                        tree_cons(NULL_TREE, PTR_type_node,
                        tree_cons(NULL_TREE, SI_type_node,
                        endlink))));

  PTR_ftype_PTRDISI =   build_function_type(   PTR_type_node,
                        tree_cons(NULL_TREE, PTR_type_node,
                        tree_cons(NULL_TREE, DI_type_node,
                        tree_cons(NULL_TREE, SI_type_node,
                        endlink))));

  PTR_ftype_PTRSISI =   build_function_type(   PTR_type_node,
                        tree_cons(NULL_TREE, PTR_type_node,
                        tree_cons(NULL_TREE, SI_type_node,
                        tree_cons(NULL_TREE, SI_type_node,
                        endlink))));

  PTR_ftype_PTRHISI =   build_function_type(   PTR_type_node,
                        tree_cons(NULL_TREE, PTR_type_node,
                        tree_cons(NULL_TREE, HI_type_node,
                        tree_cons(NULL_TREE, SI_type_node,
                        endlink))));

  PTR_ftype_PTRQISI =   build_function_type(   PTR_type_node,
                        tree_cons(NULL_TREE, PTR_type_node,
                        tree_cons(NULL_TREE, QI_type_node,
                        tree_cons(NULL_TREE, SI_type_node,
                        endlink))));

#define BUILTIN_INFO(TAG, TYPE, NARGS) \
  def_builtin("__builtin_" #TAG, TYPE, HEXAGON_BUILTIN_##TAG);
#define BUILTIN_INFO_NONCONST(TAG, TYPE, NARGS) \
  def_builtin_nonconst("__builtin_" #TAG, TYPE, HEXAGON_BUILTIN_##TAG);
#include "builtins.def"
#include "manual_builtins.def"
#undef BUILTIN_INFO
#undef BUILTIN_INFO_NONCONST

  gcc_assert(hexagon_builtin_mask_for_load);
}




/* Helper function for hexagon_expand_builtin */

static rtx
expand_one_builtin(enum insn_code icode, rtx target, tree exp, int nargs)
{
  int i;
  rtx patrick; /* maybe he doesn't like being called "pat" */
  tree arg[5];
  rtx op[5];
  enum machine_mode mode[5];
  enum machine_mode tmode;
  int opi;
  tmode = insn_data[icode].operand[0].mode;
  opi = insn_data[icode].operand[0].constraint[0] == '=' ? 1 : 0;
  for(i = 0; i < nargs; i++) {
    arg[i] = CALL_EXPR_ARG (exp, i);
    mode[i] = insn_data[icode].operand[opi + i].mode;
    op[i] = expand_expr(arg[i], NULL_RTX, mode[i], 0);
    if(!(*insn_data[icode].operand[opi + i].predicate)(op[i], mode[i])){
      op[i] = copy_to_mode_reg(mode[i], op[i]);
    }
    if(GET_CODE (op[i]) == CONST_INT){
      /* do nothing */
    }
    else if(GET_MODE (op[i]) != mode[i]){
      abort();
    }
    if(!(*insn_data[icode].operand[opi + i].predicate)(op[i], mode[i])){
      copy_to_mode_reg(mode[i], op[i]);
    }
  }
  if(insn_data[icode].operand[0].constraint[0] == '='){
    if(target == 0
       || GET_MODE (target) != tmode
       || !(*insn_data[icode].operand[0].predicate)(target, tmode)){
      target = gen_reg_rtx(tmode);
    }
  }
  else {
    target = 0;
  }
  switch(nargs){
    case 0:
      patrick = GEN_FCN (icode)(target);
      break;
    case 1:
      if(target) {
        patrick = GEN_FCN (icode)(target, op[0]);
      }
      else {
        patrick = GEN_FCN (icode)(op[0]);
      }
      break;
    case 2:
      if(target) {
        patrick = GEN_FCN (icode)(target, op[0], op[1]);
      }
      else {
        patrick = GEN_FCN (icode)(op[0], op[1]);
      }
      break;
    case 3:
      if(target) {
        patrick = GEN_FCN (icode)(target, op[0], op[1], op[2]);
      }
      else {
        patrick = GEN_FCN (icode)(op[0], op[1], op[2]);
      }
      break;
    case 4:
      if(target) {
        patrick = GEN_FCN (icode)(target, op[0], op[1], op[2], op[3]);
      }
      else {
        patrick = GEN_FCN (icode)(op[0], op[1], op[2], op[3]);
      }
      break;
    default:
      abort();
  }
  if(!patrick) {
    /* Oh, Patrick, where have you gone to? */
    return 0;
  }
  emit_insn(patrick);
  return target;
}




/* Implements hook TARGET_EXPAND_BUILTIN */

static rtx
hexagon_expand_builtin(
  tree exp,
  rtx target,
  rtx subtarget ATTRIBUTE_UNUSED,
  enum machine_mode mode ATTRIBUTE_UNUSED,
  int ignore ATTRIBUTE_UNUSED
)
{
  tree fndecl = TREE_OPERAND (CALL_EXPR_FN (exp), 0);
  int fcode = DECL_FUNCTION_CODE (fndecl);
  switch(fcode - (END_BUILTINS + 1)){
#define BUILTIN_INFO(TAG, TYPE, NARGS) \
    case HEXAGON_BUILTIN_##TAG: \
      return expand_one_builtin(CODE_FOR_hexagon_builtin_##TAG, \
                                target, exp, NARGS);
#define BUILTIN_INFO_NONCONST(TAG, TYPE, NARGS) \
    case HEXAGON_BUILTIN_##TAG: \
      return expand_one_builtin(CODE_FOR_hexagon_builtin_##TAG, \
                                target, exp, NARGS);
#include "builtins.def"
#include "manual_builtins.def"
#undef BUILTIN_INFO
#undef BUILTIN_INFO_NONCONST
    default:
      abort();
      break;
  }
}


/*------------------------------
Trampolines for Nested Functions
------------------------------*/

/* The trampoline consists of four instructions followed by two data words,
   CXT and FUN.  */

#define TRAMPOLINE_CST_SIZE (4 * 4)
#define CXTSIZE 4
#define FUNSIZE 4
#define TRAMPOLINE_VAR_SIZE (CXTSIZE + FUNSIZE)
#define CXTOFF (TRAMPOLINE_CST_SIZE)
#define FUNOFF (CXTOFF + CXTSIZE)

int
hexagon_trampoline_size (void)
{
  return TRAMPOLINE_CST_SIZE + TRAMPOLINE_VAR_SIZE;
}

/* Output assembly code for the trampoline template to stdio stream FILE.  */

static void
hexagon_trampoline_template (FILE *file)
{
  int scr = STATIC_CHAIN_REGNUM;

  asm_fprintf (file, "\t" "r12 = pc\n");
  asm_fprintf (file, "\t" "r%u = memw(r12 + #%u)\n", scr, CXTOFF);
  asm_fprintf (file, "\t" "r11 = memw(r12 + #%u)\n", FUNOFF);
  asm_fprintf (file, "\t" "jumpr r11\n");

  ASM_OUTPUT_SKIP (file, (HOST_WIDE_INT)TRAMPOLINE_VAR_SIZE);
}

/* Emit RTL insns to initialize the variable parts of a trampoline.  MEMADDR is
   the address of the trampoline.  FNDECL is declaration of the called function.
   CXT is an RTX for the static chain value for the function.  */

static void
hexagon_initialize_trampoline (rtx memaddr, tree fndecl, rtx cxt)
{
  rtx addr = XEXP (memaddr, 0);
  rtx fnaddr = XEXP (DECL_RTL (fndecl), 0);

  emit_block_move (memaddr, assemble_trampoline_template (),
		   GEN_INT (TRAMPOLINE_CST_SIZE), BLOCK_OP_NORMAL);

  emit_move_insn (gen_rtx_MEM (Pmode,
         memory_address (Pmode,
         plus_constant (addr, CXTOFF))), cxt);

  emit_move_insn (gen_rtx_MEM (Pmode,
         memory_address (Pmode,
         plus_constant (addr, FUNOFF))), fnaddr);

  emit_insn (gen_clean_cache (addr));
  emit_insn (gen_clean_cache (force_reg(SImode, plus_constant (addr, 32))));
}

#undef CXTOFF
#undef FUNOFF
#undef CXTSIZE
#undef FUNSIZE
#undef TRAMPOLINE_VAR_SIZE
#undef TRAMPOLINE_CST_SIZE


/* Implements hook TARGET_INVALID_WITHIN_DOLOOP

   Don't form a hardware loop around a call or a set or clobber of a loop
   register.

   FIXME: May no longer be necessary.  */

static const char *
hexagon_invalid_within_doloop(const_rtx insn)
{
  rtx side_effect;
  rtx parallel;
  rtx reg;
  int i;

  /* Notes and labels are fine. */
  if(!INSN_P (insn)){
    return NULL;
  }

  /* Don't allow calls because the loop registers are call used. */
  if(CALL_P (insn)){
    return "The loop contains a call.";
  }

  i = 0;
  parallel = NULL_RTX;
  /* We can't form a loop around something that changes the loop registers. */
  side_effect = PATTERN (insn);
  switch(GET_CODE (side_effect)){
    case PARALLEL:
      parallel = side_effect;
      for(i = 0; i < XVECLEN (parallel, 0); i++){
        side_effect = XVECEXP (parallel, 0, i);
        if(GET_CODE (side_effect) == SET){
      /* FALL THROUGH */
    case SET:
          reg = SET_DEST (side_effect);
          if(REG_P (reg)){
            switch(REGNO (reg)){
              case LC0_REGNUM:
                return "lc0 is set in the loop.";
              case SA0_REGNUM:
                return "sa0 is set in the loop.";
              case LC1_REGNUM:
                return "lc1 is set in the loop.";
              case SA1_REGNUM:
                return "sa1 is set in the loop.";
              default:
                break;
            }
          }
          if(parallel){
            continue;
          }
          return NULL;
        }
        else if(GET_CODE (side_effect) == CLOBBER){
      /* FALL THROUGH */
    case CLOBBER:
          reg = XEXP (side_effect, 0);
          if(REG_P (reg)){
            switch(REGNO (reg)){
              case LC0_REGNUM:
                return "lc0 is clobbered in the loop.";
              case SA0_REGNUM:
                return "sa0 is clobbered in the loop.";
              case LC1_REGNUM:
                return "lc1 is clobbered in the loop.";
              case SA1_REGNUM:
                return "sa1 is clobbered in the loop.";
              default:
                break;
            }
          }
          if(parallel){
            continue;
          }
          return NULL;
        }
      }
      /* FALL THROUGH */
    default:
      return NULL;
  }
}


/* Implement TARGET_CANNOT_COPY_INSN_P hook.  */

static bool
hexagon_cannot_copy_insn_p (rtx insn)
{
  switch (recog_memoized (insn))
    {
    case CODE_FOR_loop0_v4:
    case CODE_FOR_loop0:
    case CODE_FOR_loop0_pic:
    case CODE_FOR_loop0_pic_v4:
    case CODE_FOR_loop1_v4:
    case CODE_FOR_loop1:
    case CODE_FOR_loop1_pic:
    case CODE_FOR_loop1_pic_v4:
    case CODE_FOR_endloop0:
    case CODE_FOR_endloop1:
      /* Don't allow hardware loop insns created during RTL expansion (i.e. by
	 inline movmem/setmem) to be copied, since that tends to lead to
	 incorrect code being generated.  */
      return true;
    }

  return false;
}



/*-------------------------------------------
Functions called from the machine description
-------------------------------------------*/



/* Helper function for hexagon_expand_prologue

   Allocate stack space by decrementing SP. */

static void
hexagon_allocate_stack(unsigned HOST_WIDE_INT size, int allocate_stack_insn){
  rtx offset, insn;

  if(size == 0){
    return;
  }

  /* If more than two add instructions would be required, then load the offset
     into r28. */
  if(size > 2 * -MIN_ADD_IMMED){
    offset = gen_rtx_REG (SImode, 28);

    if (!TARGET_V4_FEATURES)
      {
        rtx size_rtx = gen_int_mode (-size, SImode);
        /* Insns with RTX_FRAME_RELATED_P set can't be split, so emit
	   high/lo_sum insns explicitly.  */
	insn = emit_insn (gen_rtx_SET (VOIDmode, offset,
			    gen_rtx_HIGH (SImode, size_rtx)));
	RTX_FRAME_RELATED_P (insn) = 1;
	insn = emit_insn (gen_rtx_SET (VOIDmode, offset,
			    gen_rtx_LO_SUM (SImode, offset, size_rtx)));
      }
    else
      insn = emit_move_insn(offset, gen_int_mode(-size, Pmode));

    RTX_FRAME_RELATED_P (insn) = 1;
    size = 0;
  }
  else if(size > -MIN_ADD_IMMED){
    offset = gen_int_mode(MIN_ADD_IMMED, Pmode);
    size -= -MIN_ADD_IMMED;
  }
  else {
    offset = gen_int_mode(-size, Pmode);
    size = 0;
  }

  insn = emit_insn(GEN_FCN (allocate_stack_insn)(offset));
  RTX_FRAME_RELATED_P (insn) = 1;

  if(size > 0){
    offset = gen_int_mode(-size, Pmode);
    insn = emit_insn(gen_increment_stack_pointer(offset));
    RTX_FRAME_RELATED_P (insn) = 1;
  }
}




/* Emit instructions to set up a stack frame and save any registers that need
   saving for the current function. */

void
hexagon_expand_prologue(void)
{
  struct hexagon_frame_info *frame;
  rtx base_reg;
  HOST_WIDE_INT offset;
  unsigned int i;

  frame = hexagon_frame_info();

#ifdef LINUX
  /* Handle pushing of varargs registers before the allocframe if needed */
  if (frame->pretend_size &&
      frame->first_anon_arg < HEXAGON_NUM_ARG_REGS){
      int ap, stack_off, align_slop;
      int first_reg_offset = frame->first_anon_arg;
      rtx stack_adj;
      int regno;

      stack_off = -(frame->pretend_size) / UNITS_PER_WORD;

      /* Move named args that appeared on stack... memcpy from src to dst. */
      for (ap=0; ap<frame->named_stack_words; ap++, stack_off++) {
        rtx stack_src = gen_rtx_PLUS (Pmode, stack_pointer_rtx,
                                      GEN_INT (UNITS_PER_WORD * ap));
        rtx stack_dst = gen_rtx_PLUS (Pmode, stack_pointer_rtx,
                                      GEN_INT (UNITS_PER_WORD * stack_off));

        rtx arg_reg = gen_rtx_REG (word_mode, HEXAGON_NUM_ARG_REGS);
        rtx move_src = emit_move_insn (arg_reg,
                                        gen_rtx_MEM (word_mode, stack_src));
        rtx move_dst = emit_move_insn (gen_rtx_MEM (word_mode, stack_dst),
                                        arg_reg);
        RTX_FRAME_RELATED_P (move_dst) = 1;
      }

      align_slop = (HEXAGON_NUM_ARG_REGS - first_reg_offset) & 1;
      stack_off += align_slop;

      /* Iterate through the vararg regs to push on the stack */
      for (regno = first_reg_offset;
           regno < HEXAGON_NUM_ARG_REGS; regno++, stack_off++) {
        rtx stack_loc = gen_rtx_PLUS (Pmode, stack_pointer_rtx,
                                      GEN_INT (UNITS_PER_WORD * stack_off));
        rtx arg_reg = gen_rtx_REG (word_mode, FIRST_ARG_REGNUM + regno);
        rtx move_insn = emit_move_insn (gen_rtx_MEM (word_mode, stack_loc),
                                        arg_reg);
        RTX_FRAME_RELATED_P (move_insn) = 1;
      }

      /* Reset the stack pointer to sp - pretend_size. sp - pretend_size must be
         double-word aligned. Hence there may be some padding at the end of the
         pushed varargs registers */
      stack_adj = emit_insn(gen_addsi3(stack_pointer_rtx,
                                       stack_pointer_rtx,
                                       gen_int_mode(-frame->pretend_size, Pmode)));
      RTX_FRAME_RELATED_P (stack_adj) = 1;
  }
#endif

  /* Do we need to use the allocframe instruction? */
  if(frame->use_allocframe){
    rtx allocframe_size = gen_int_mode(frame->allocframe_size, SImode);
    rtx insn = emit_insn(GEN_FCN (frame->allocframe_insn)(allocframe_size));
    rtx pat;
    RTX_FRAME_RELATED_P (insn) = 1;
    pat = PATTERN (insn);
    if (GET_CODE (pat) == PARALLEL)
      {
        int i;
	for (i = 0; i < XVECLEN (pat, 0); i++)
	  RTX_FRAME_RELATED_P (XVECEXP (pat, 0, i)) = 1;
      }
  }

  /* Allocate any remaning stack space. */
  hexagon_allocate_stack(frame->sp_adjustment, frame->allocate_stack_insn);

  /* Save callee-save registers. */
  if(frame->prologue_function != CODE_FOR_nothing){
    rtx pat;
    rtx insn = emit_insn(GEN_FCN (frame->prologue_function)(NULL_RTX));
    RTX_FRAME_RELATED_P (insn) = 1;

    /* Ensure the each part of a multi-register save is frame-related.  */
    pat = PATTERN (insn);
    if (GET_CODE (pat) == PARALLEL)
      {
	int i;
	for (i = 0; i < XVECLEN (pat, 0); i++)
	  RTX_FRAME_RELATED_P (XVECEXP (pat, 0, i)) = 1;
      }
  }
  base_reg = frame->base_reg;
  offset = frame->offset;
  /* Account for any pairs that have already been saved. */
  offset -= 2 * UNITS_PER_WORD * frame->num_specially_saved_pairs;
  /* Save any callee-save registers that can be stored as pairs and have not
     already been saved in some other manner. */
  for(i = frame->num_specially_saved_pairs; i < frame->num_saved_pairs; i++){
    rtx insn, par;
    offset -= 2 * UNITS_PER_WORD;
    par = gen_rtx_PARALLEL (VOIDmode, rtvec_alloc (2));
    XVECEXP (par, 0, 0) = gen_rtx_SET (VOIDmode,
				       gen_rtx_MEM (SImode,
					 plus_constant (base_reg, offset)),
				       gen_rtx_REG (SImode,
					 frame->saved_pairs[i]));
    RTX_FRAME_RELATED_P (XVECEXP (par, 0, 0)) = 1;
    XVECEXP (par, 0, 1) = gen_rtx_SET (VOIDmode,
				       gen_rtx_MEM (SImode,
					 plus_constant (base_reg, offset + 4)),
				       gen_rtx_REG (SImode,
					 frame->saved_pairs[i] + 1));
    RTX_FRAME_RELATED_P (XVECEXP (par, 0, 1)) = 1;
    insn = emit_insn (par);
    RTX_FRAME_RELATED_P (insn) = 1;
  }
  /* Account for any singles that have already been saved. */
  offset -= UNITS_PER_WORD * frame->num_specially_saved_singles;
  /* Save any remaining callee-save registers. */
  for(i = frame->num_specially_saved_singles; i < frame->num_saved_singles; i++){
    rtx insn;
    offset -= UNITS_PER_WORD;
    insn = emit_move_insn(gen_rtx_MEM (SImode, plus_constant(base_reg, offset)),
                	  gen_rtx_REG (SImode, frame->saved_singles[i]));
    RTX_FRAME_RELATED_P (insn) = 1;
  }

  if (flag_pic && crtl->uses_pic_offset_table) {
    hexagon_load_pic_register ();
  }

  if (frame->tls_set)
    hexagon_load_tls_register ();

}



/* Return true if the current function's epilogue consists only of a return
   instruction. */

bool
hexagon_direct_return(void)
{
  struct hexagon_frame_info *frame = hexagon_frame_info();

  return reload_completed
         && frame->pretend_size == 0	  /* not for a varargs function  */
         && (frame->total_size == 0
	     || (TARGET_V4_FEATURES && frame->use_allocframe
		 && frame->reg_size == 0));
}




/* Helper function for hexagon_expand_epilogue

   Deallocate stack space by incrementing SP. */

static void
hexagon_deallocate_stack(unsigned HOST_WIDE_INT size){
  rtx r28;

  /* If more than two add instructions would be required, then reload the offset
     into r28. */
  if(size > 2 * MAX_ADD_IMMED){
    r28 = gen_rtx_REG (SImode, 28);
    emit_move_insn(r28, gen_int_mode(size, Pmode));
    emit_insn(gen_addsi3(stack_pointer_rtx, stack_pointer_rtx, r28));
  }
  else {
    while(size > MAX_ADD_IMMED){
      emit_insn(gen_addsi3(stack_pointer_rtx,
                           stack_pointer_rtx,
                           gen_int_mode(MAX_ADD_IMMED, Pmode)));
      size -= MAX_ADD_IMMED;
    }
    if(size != 0){
      emit_insn(gen_addsi3(stack_pointer_rtx,
                           stack_pointer_rtx,
                           gen_int_mode(size, Pmode)));
    }
  }
}




/* Emit instructions to tear down the stack frame and restore any registers that
   need restoring for the current function.  Also emit a return instruction iff
   EMIT_RETURN is true. */

void
hexagon_expand_epilogue(bool sibcall)
{
  bool emit_return;
  struct hexagon_frame_info *frame;
  rtx base_reg;
  HOST_WIDE_INT offset;
  int i;

  emit_return = !sibcall;

  frame = hexagon_frame_info();

  /* Restore callee-save registers. */
  base_reg = frame->base_reg;
  offset = frame->offset - (2 * frame->num_saved_pairs
                              + frame->num_saved_singles) * UNITS_PER_WORD;
  /* Restore any callee-save registers not stored as part of a pair. */
  for(i = (int) frame->num_saved_singles - 1; i >= 0; i--){
    emit_move_insn(gen_rtx_REG (SImode, frame->saved_singles[i]),
                   gen_rtx_MEM (SImode, plus_constant(base_reg, offset)));
    offset += UNITS_PER_WORD;
  }
  /* Restore any callee-save registers that can be loaded as pairs and will not
     be loaded by a common epilogue function. */
  for(i = (int) frame->num_saved_pairs - 1;
      i >= (int) (sibcall ? frame->num_sibcall_function_restored_pairs
                          : frame->num_function_restored_pairs);
      i--){
    emit_move_insn(gen_rtx_REG (DImode, frame->saved_pairs[i]),
                   gen_rtx_MEM (DImode, plus_constant(base_reg, offset)));
    offset += 2 * UNITS_PER_WORD;
  }

  /* Determine how best to deallocate the stack frame and restore LR and FP if
     necessary. */
  /* If LR and FP do not need to be restored, then simply deallocate the
     stack. */
  if(!frame->use_allocframe){
    hexagon_deallocate_stack(frame->sp_adjustment);
  }
  /* Otherwise, call a function if doing so will save code size. */
  else if(!sibcall && frame->epilogue_function != CODE_FOR_nothing){
    emit_jump_insn(GEN_FCN (frame->epilogue_function)(NULL_RTX));
    emit_return = false;
  }
  else if(sibcall && frame->sibcall_epilogue_function != CODE_FOR_nothing){
    emit_insn(GEN_FCN (frame->sibcall_epilogue_function)(NULL_RTX));
  }
  /* Otherwise, use separate deallocframe and return instructions. */
  else {
    emit_insn(gen_deallocframe());
  }

  /* If returning to an exception handler, adjust the stack as necessary. */
  if(crtl->calls_eh_return){
    emit_insn(gen_addsi3(stack_pointer_rtx,
                         stack_pointer_rtx, EH_RETURN_STACKADJ_RTX));
  }

#ifdef LINUX
  if (frame->pretend_size){
      rtx stack_adj = emit_insn(gen_addsi3(stack_pointer_rtx,
                                           stack_pointer_rtx,
                                           gen_int_mode(frame->pretend_size, Pmode)));
      RTX_FRAME_RELATED_P (stack_adj) = 1;
  }
#endif

  /* Emit a return jump if we still need to. */
  if(emit_return){
    emit_jump_insn(gen_return_jump());
  }
}

/* Extract the REG part of a paradoxical subreg for a comparison CMP, if it
   is safe to do so (i.e. the high-order bits contain valid information).  */

static rtx
extract_subreg_for_comparison (enum rtx_code cmp, rtx op)
{
  gcc_assert (GET_CODE (op) == SUBREG && SUBREG_PROMOTED_VAR_P (op));

  switch (cmp)
    {
    case EQ:
    case NE:
      return SUBREG_REG (op);

    case GT:
    case GE:
    case LT:
    case LE:
      if (!SUBREG_PROMOTED_UNSIGNED_P (op))
	return SUBREG_REG (op);
      break;

    case GTU:
    case GEU:
    case LTU:
    case LEU:
      if (SUBREG_PROMOTED_UNSIGNED_P (op))
	return SUBREG_REG (op);
      break;

    default:
      ;
    }

  return op;
}

/* Truncate integer VAL so that it is suitable as the RHS of a comparison,
   where the LHS is op0.  If we have an unsigned comparison, we must make
   sure the high bits of the immediate (above mode size of MODE) are zero.  */

static rtx
trunc_int_for_comparison (rtx op0, rtx val, enum machine_mode mode,
			  enum machine_mode comparison_mode)
{
  gcc_assert (GET_CODE (op0) == SUBREG && SUBREG_PROMOTED_VAR_P (op0));

  /* This condition is true if extract_subreg_for_comparison managed to
     extract a subreg.  */
  if (mode != comparison_mode)
    return convert_modes (comparison_mode, mode, val,
			  SUBREG_PROMOTED_UNSIGNED_P (op0));
  else
    return val;
}

rtx
hexagon_expand_compare (enum rtx_code code, rtx op0, rtx op1)
{
#define SET_CODES(CC, JC) compare_code = (CC); jump_code = (JC)
  enum rtx_code compare_code, jump_code;
  rtx p_reg;
  int offset = 0;
  bool swap_operands = false;
  enum machine_mode mode = GET_MODE (op0);

  if (mode == BImode)
    {
      gcc_assert (REG_P (op0)
		  && (code == NE || code == EQ) && op1 == const0_rtx);
      return gen_rtx_fmt_ee (code, VOIDmode, op0, op1);
    }

  /* A weird hack: subword comparisons (cmph, cmpb) are less packetisable than
     their full-word equivalents.  Attempt to use word-size compares instead
     when it is safe to do so.  */
  if ((mode == QImode || mode == HImode)
      && GET_CODE (op0) == SUBREG && GET_MODE (SUBREG_REG (op0)) == SImode
      && SUBREG_PROMOTED_VAR_P (op0))
    {
      if (GET_CODE (op1) == CONST_INT)
	{
	  rtx old_op0 = op0;
	  op0 = extract_subreg_for_comparison (code, op0);
	  op1 = trunc_int_for_comparison (old_op0, op1, mode, GET_MODE (op0));
	  mode = GET_MODE (op0);
	}
      else if (GET_CODE (op1) == SUBREG && GET_MODE (op1) == mode
	       && GET_MODE (SUBREG_REG (op1)) == SImode
	       && SUBREG_PROMOTED_VAR_P (op1))
	{
	  op0 = extract_subreg_for_comparison (code, op0);
	  op1 = extract_subreg_for_comparison (code, op1);
	  mode = GET_MODE (op0);
	}
    }

  if (mode == SFmode || mode == DFmode)
    {
      op0 = force_reg (mode, op0);
      op1 = force_reg (mode, op1);

      p_reg = gen_reg_rtx (BImode);

      /* Hexagon floating point comparison instructions don't check for
	 unordered operands, it must be done explicitly. */
      if (code == UNEQ || code == UNGE || code == UNGT || code == LTGT
          || code == UNLE || code == UNLT)
	{
	  rtx unordered = gen_reg_rtx (BImode);
          rtx comparison = gen_reg_rtx (BImode);
	  emit_insn (gen_rtx_SET (VOIDmode, unordered,
                                  gen_rtx_UNORDERED (BImode, op0, op1)));
          switch (code)
            {
              case UNEQ:
        	SET_CODES (EQ, NE); break;
              case UNGE:
        	SET_CODES (GE, NE); break;
              case UNGT:
        	SET_CODES (GT, NE); break;
              case LTGT:
        	SET_CODES (EQ, EQ); break;
              case UNLE:
        	SET_CODES (LE, NE); break;
              case UNLT:
        	SET_CODES (LT, NE); break;
              default:
        	gcc_unreachable ();
            }
          emit_insn (gen_rtx_SET (VOIDmode, comparison,
                                  gen_rtx_fmt_ee (compare_code, BImode,
                                                  op0, op1)));
          emit_insn (gen_rtx_SET (VOIDmode, p_reg,
                                  gen_rtx_IOR (BImode, unordered, comparison)));
          return gen_rtx_fmt_ee (jump_code, VOIDmode, p_reg, const0_rtx);
	}
      else
	{
          switch (code)
            {
              case NE:
        	SET_CODES (EQ, EQ); break;
              case ORDERED:
        	SET_CODES (UNORDERED, EQ); break;
              default:
        	SET_CODES (code, NE); break;
            }

          emit_insn (gen_rtx_SET (VOIDmode, p_reg,
                       gen_rtx_fmt_ee (compare_code, BImode, op0, op1)));
          return gen_rtx_fmt_ee (jump_code, VOIDmode, p_reg, const0_rtx);
	}
    }

  if (REG_P (op1) || GET_CODE (op1) == SUBREG)
    switch(code)
      {
      /* Equality compares */
      case EQ:
        SET_CODES (EQ, NE); break;
      case NE:
        SET_CODES (EQ, EQ); break;

      /* Signed compares */
      case LT:
        SET_CODES (GT, NE); swap_operands = true; break;
      case LE:
        SET_CODES (GT, EQ); break;
      case GT:
        SET_CODES (GT, NE); break;
      case GE:
        SET_CODES (GT, EQ); swap_operands = true; break;

      /* Unsigned compares */
      case LTU:
        SET_CODES (GTU, NE); swap_operands = true; break;
      case LEU:
        SET_CODES (GTU, EQ); break;
      case GTU:
        SET_CODES (GTU, NE); break;
      case GEU:
        SET_CODES (GTU, EQ); swap_operands = true; break;

      default:
        gcc_unreachable ();
      }
  else
    {
      if (mode == DImode)
        {
	  op0 = force_reg (DImode, op0);
	  op1 = force_reg (DImode, op1);
	}

      switch(code)
        {
	/* Equality compares */
	case EQ:
          SET_CODES (EQ, NE); break;
	case NE:
          SET_CODES (EQ, EQ); break;

	/* Signed compares */
	case LT:
          SET_CODES (GT, EQ); offset = -1; break;
	case LE:
          SET_CODES (GT, EQ); break;
	case GT:
          SET_CODES (GT, NE); break;
	case GE:
          SET_CODES (GT, NE); offset = -1; break;

	/* Unsigned compares */
	case LTU:
          SET_CODES (GTU, EQ); offset = -1; break;
	case LEU:
          SET_CODES (GTU, EQ); break;
	case GTU:
          SET_CODES (GTU, NE); break;
	case GEU:
          SET_CODES (GTU, NE); offset = -1; break;

	default:
          gcc_unreachable ();
	}

      op1 = plus_constant (op1, offset);

      if ((GET_CODE (op1) == CONST_INT) && (offset != 0))
	op1 = gen_int_mode (INTVAL (op1), mode);

      if (GET_CODE (op1) != CONST_INT
	  || !IN_RANGE (INTVAL (op1), compare_code == GTU ? 0 : -512, 511))
	op1 = force_reg (mode, op1);
    }

  if (swap_operands)
    {
      rtx tmp = op0;
      op0 = op1;
      op1 = tmp;
    }

  p_reg = gen_reg_rtx (BImode);

  /* Emit (set (reg:BI p_reg) (compare_code:BI op0 op1)).  */
  emit_insn (gen_rtx_SET (VOIDmode, p_reg,
                          gen_rtx_fmt_ee (compare_code, BImode, op0, op1)));

  /* Return (eq/ne (reg:BI p_reg) (const_int 0)).  */
  return gen_rtx_fmt_ee (jump_code, VOIDmode, p_reg, const0_rtx);
#undef SET_CODES
}

void
hexagon_split_subword_compare (enum machine_mode mode, rtx operands[],
			       rtx comparison)
{
  enum rtx_code comp = GET_CODE (comparison);
  rtx op1, op2, comp_insn;
  bool swap_operands = false;

  switch (comp)
    {
    case EQ:
    case GT:
    case GTU:
    case GE:
    case GEU:
      break;

    case LT:
      comp = GT;
      swap_operands = true;
      break;

    case LTU:
      comp = GTU;
      swap_operands = true;
      break;

    default:
      gcc_unreachable ();
    }

  op1 = gen_lowpart (mode, operands[1]);
  op2 = gen_lowpart (mode, operands[2]);

  if (swap_operands)
    comp_insn = gen_rtx_fmt_ee (comp, BImode, op2, op1);
  else
    comp_insn = gen_rtx_fmt_ee (comp, BImode, op1, op2);

  emit_insn (gen_rtx_SET (BImode, operands[0], comp_insn));
}


/* Creates a tree node for a block-move function named NAME. */

static tree
hexagon_special_case_memcpy_fn(const char *name)
{
  tree args, fn;

  fn = get_identifier(name);
  args = build_function_type_list(ptr_type_node, ptr_type_node,
                                  const_ptr_type_node, sizetype,
                                  NULL_TREE);

  fn = build_decl(UNKNOWN_LOCATION, FUNCTION_DECL, fn, args);
  DECL_EXTERNAL (fn) = 1;
  TREE_PUBLIC (fn) = 1;
  DECL_ARTIFICIAL (fn) = 1;
  TREE_NOTHROW (fn) = 1;
  DECL_VISIBILITY (fn) = VISIBILITY_DEFAULT;
  DECL_VISIBILITY_SPECIFIED (fn) = 1;
  make_decl_rtl(fn);
  assemble_external(fn);

  return fn;
}




/* Emits a call to block-move function *FN named NAME with parameters DST, SRC,
   and SIZE.  TAILCALL is true only if the call may be implemented as a tail
   call.  If *FN is NULL, then it is initialized to a tree node for a function
   named NAME. */

static void
hexagon_emit_special_case_memcpy_fn(
  tree *fn,
  const char *name,
  rtx dst,
  rtx src,
  rtx size
/* FIXME: tailcall requires target independent change */
#if 0
  ,bool tailcall
#endif
)
{
  rtx dst_addr, src_addr;
  tree call_expr, src_tree, dst_tree, size_tree;
  enum machine_mode size_mode;

  /* Emit code to copy the addresses of DST and SRC and SIZE into new
     pseudos.  We can then place those new pseudos into a VAR_DECL and
     use them later.  */

  dst_addr = copy_to_mode_reg(Pmode, XEXP (dst, 0));
  src_addr = copy_to_mode_reg(Pmode, XEXP (src, 0));

  dst_addr = convert_memory_address(ptr_mode, dst_addr);
  src_addr = convert_memory_address(ptr_mode, src_addr);

  dst_tree = make_tree(ptr_type_node, dst_addr);
  src_tree = make_tree(ptr_type_node, src_addr);

  size_mode = TYPE_MODE (sizetype);

  size = convert_to_mode(size_mode, size, 1);
  size = copy_to_mode_reg(size_mode, size);

  size_tree = make_tree(sizetype, size);

  if(!*fn){
    *fn = hexagon_special_case_memcpy_fn(name);
  }
  call_expr = build_call_expr (*fn, 3, dst_tree, src_tree, size_tree);
/* FIXME tailcall requires target independent change */
#if 0
  CALL_EXPR_TAILCALL (call_expr) = tailcall;
#endif
  expand_normal (call_expr);
}

/* Emits an inline implementation of a block move.
   dst_0 is a MEM containing the destination address.
   src_0 is a MEM containing the source address.
   nbytes is a CONST_INT containing the number of bytes to be copied.
   min_align is a CONST_INT containing the guaranteed minimum
   alignment of both the source and destination addresses.
   If VOLATILE_P is true then the final assembly corresponding to the
   code being emitted must not contain any packets containing more than
   one memory access. */

static void
hexagon_expand_movmem_inline(rtx dst_0, rtx src_0, rtx nbytes,
                             rtx min_align, bool volatile_p)
{
  rtx src_reg, dst_reg, src_mem, dst_mem, value_reg, count_reg;
  rtx label, loopcount_rtx;
  int count, loopcount, align, size, log2size;
  enum machine_mode mode;
  rtx (*gen_memcpy_kernel)(rtx, rtx, rtx, rtx);

  count = INTVAL (nbytes);
  align = INTVAL (min_align);

  gcc_assert(count >= 0);

  src_reg = copy_to_mode_reg(Pmode, XEXP (src_0, 0));
  dst_reg = copy_to_mode_reg(Pmode, XEXP (dst_0, 0));

  /* Copy as many pieces as possible of size equal to the mutual alignment of
     the source and destination. */
  if((align & 7) == 0){
    mode = DImode;
    size = 8;
    log2size = 3;
    gen_memcpy_kernel = gen_memcpy_kerneldi;
  }
  else if((align & 3) == 0){
    mode = SImode;
    size = 4;
    log2size = 2;
    gen_memcpy_kernel = gen_memcpy_kernelsi;
  }
  else if((align & 1) == 0){
    mode = HImode;
    size = 2;
    log2size = 1;
    gen_memcpy_kernel = gen_memcpy_kernelhi;
  }
  else {
    mode = QImode;
    size = 1;
    log2size = 0;
    gen_memcpy_kernel = gen_memcpy_kernelqi;
  }

  src_mem = change_address(src_0, mode, src_reg);
  dst_mem = change_address(dst_0, mode, dst_reg);

  /* Only form a loop if it would execute for at least two iterations. */
  loopcount = count >> log2size;
  if(loopcount > 2){
    rtx pred;

    /* If the source or destination might be volatile, then load and store
       serially inside the loop.  Otherwise, software pipeline the loop. */

    value_reg = gen_reg_rtx(mode);
    count_reg = gen_reg_rtx(SImode);
    loopcount_rtx = gen_int_mode(volatile_p ? loopcount : loopcount - 1,
                                 SImode);
    label = gen_label_rtx();

    if(!volatile_p){
      emit_move_insn(value_reg, src_mem);
      emit_insn(gen_addsi3(src_reg, src_reg, gen_int_mode(size, Pmode)));
    }

    emit_move_insn (count_reg, loopcount_rtx);

    emit_label(label);

    if(volatile_p){
      emit_move_insn(value_reg, src_mem);
      emit_move_insn(dst_mem, value_reg);
    }
    else {
      emit_insn(gen_memcpy_kernel(dst_mem, value_reg, value_reg, src_mem));
    }
    emit_insn(gen_addsi3(dst_reg, dst_reg, gen_int_mode(size, Pmode)));
    emit_insn(gen_addsi3(src_reg, src_reg, gen_int_mode(size, Pmode)));

    pred = gen_reg_rtx(BImode);
    emit_insn(gen_addsi3(count_reg, count_reg, constm1_rtx));
    emit_insn(gen_cmpsi_eq(pred, count_reg, const0_rtx));
    emit_jump_insn(gen_cond_jump(gen_rtx_EQ (VOIDmode, pred, const0_rtx),
                                 pred, label));

    if(!volatile_p){
      emit_move_insn(dst_mem, value_reg);
      emit_insn(gen_addsi3(dst_reg, dst_reg, gen_int_mode(size, Pmode)));
    }

  }
  else {
    for(; loopcount; loopcount--){

      value_reg = gen_reg_rtx(mode);

      emit_move_insn(value_reg, src_mem);
      emit_insn(gen_addsi3(src_reg, src_reg, gen_int_mode(size, Pmode)));
      emit_move_insn(dst_mem, value_reg);
      emit_insn(gen_addsi3(dst_reg, dst_reg, gen_int_mode(size, Pmode)));

    }
  }

  /* Copy any leftovers. */
  if((align & 7) == 0 && count & 4){

    value_reg = gen_reg_rtx(SImode);
    src_mem = change_address(src_0, SImode, src_reg);
    dst_mem = change_address(dst_0, SImode, dst_reg);

    emit_move_insn(value_reg, src_mem);
    emit_insn(gen_addsi3(src_reg, src_reg, gen_int_mode(4, Pmode)));
    emit_move_insn(dst_mem, value_reg);
    emit_insn(gen_addsi3(dst_reg, dst_reg, gen_int_mode(4, Pmode)));

  }
  if(count & 2){

    value_reg = gen_reg_rtx(HImode);
    src_mem = change_address(src_0, HImode, src_reg);
    dst_mem = change_address(dst_0, HImode, dst_reg);

    emit_move_insn(value_reg, src_mem);
    emit_insn(gen_addsi3(src_reg, src_reg, gen_int_mode(2, Pmode)));
    emit_move_insn(dst_mem, value_reg);
    emit_insn(gen_addsi3(dst_reg, dst_reg, gen_int_mode(2, Pmode)));

  }
  if(count & 1){

    value_reg = gen_reg_rtx(QImode);
    src_mem = change_address(src_0, QImode, src_reg);
    dst_mem = change_address(dst_0, QImode, dst_reg);

    emit_move_insn(value_reg, src_mem);
    emit_move_insn(dst_mem, value_reg);

  }
}



/* Tree nodes for specialized block-move functions */

static GTY(()) tree hexagon_memcpy_volatile;
static GTY(()) tree hexagon_memcpy_likely_aligned8_min32_mult8;

/* Emits RTL to perform a block move or returns false.
   dst is a MEM containing the destination address.
   src is a MEM containing the source address.
   nbytes is a CONST_INT containing the number of bytes to be copied.
   min_align is a CONST_INT containing the guaranteed minimum alignment
   of both the source and destination addresses. */

bool
hexagon_expand_movmem(rtx dst, rtx src, rtx nbytes, rtx min_align)
{
  HOST_WIDE_INT length, align;
  int cycles, leftovers;
  bool volatile_p, simple_copy_p;
  const int O3_inline_cycle_threshold = 10;
  const int O2_inline_cycle_threshold = 5;
  const int likely_aligned_cycle_threshold = 100;

  volatile_p = !TARGET_V4_FEATURES
               && (MEM_VOLATILE_P (dst)
                   || MEM_VOLATILE_P (src)
                   || !hexagon_dual_memory_accesses);

  align = MIN (INTVAL (min_align), BIGGEST_ALIGNMENT / BITS_PER_UNIT);
  if(GET_CODE (nbytes) == CONST_INT){
    length = INTVAL (nbytes);
    length &= 0xFFFFFFFFLL;
    simple_copy_p = (length <= align && exact_log2(length) != -1)
                    || length == 0;
  }
  else {
    length = -1;
    simple_copy_p = false;
  }
  gcc_assert(length >= -1);
  gcc_assert((align & (align - 1)) == 0);

  cycles = length / align + 1;
  for(leftovers = length & (align - 1); leftovers; leftovers >>= 1){
    cycles += leftovers & 1;
  }

  if(optimize
     && (simple_copy_p
         || (!optimize_size
             && (align & 3) == 0
             && length != -1
             && cycles <= (optimize >= 3 ? O3_inline_cycle_threshold
                                         : O2_inline_cycle_threshold)))){
    hexagon_expand_movmem_inline(dst, src, nbytes, min_align, volatile_p);
    return true;
  }

  if(optimize && !volatile_p
     && (align & 3) == 0 && length >= 32 && length % 8 == 0
     && cycles < likely_aligned_cycle_threshold){
    hexagon_emit_special_case_memcpy_fn(&hexagon_memcpy_likely_aligned8_min32_mult8,
                                      "__hexagon_memcpy_likely_aligned_min32bytes_mult8bytes",
                                      dst, src, nbytes);
    return true;
  }

  if(volatile_p){
    hexagon_emit_special_case_memcpy_fn(&hexagon_memcpy_volatile,
#ifdef LINUX
                                      "__hexagon_memcpy_volatile",
#else
                                      "memcpy_v",
#endif
                                      dst, src, nbytes);
    return true;
  }

  return false;
}

/* Block set instruction. The destination string is the first operand, given as a
   mem:BLK whose address is in mode Pmode. The number of bytes to set is the second operand,
   in mode m. The value to initialize the memory with is the third operand. Targets that only
   support the clearing of memory should reject any value that is not the constant 0.

   The fourth operand is the known alignment of the destination, in the form of a const_int rtx.
   Thus, if the compiler knows that the destination is word-aligned, it may provide the value 4 for this operand.

   Optional operands 5 and 6 specify expected alignment and size of block respectively. The expected
   alignment differs from alignment in operand 4 in a way that the blocks are not required to be aligned
   according to it in all cases. This expected alignment is also in bytes, just like operand 4. Expected
   size, when unknown, is set to (const_int -1).

   Implementation details-
   1. From the alignment, figure out all valid modes which can be used to set memory. For example-
      a. alignment == 4  -> valid modes (QI, HI, SI)
      b. aligment == 128 -> valid modes (QI, HI, SI, DI)

   2. Now loop thru all valid modes starting from the widest to the narrowest-
      a. calculate loop count = bytes-to-copy / mode-size
      b. if loop count == 0 -> not enough bytes to store
      c. if loop count == 1 -> do simple store
      d. if loop count > 1  -> do hardware loop
*/

bool
hexagon_expand_setmem(rtx operands[])
{
#define MEMSET_LIBCALL_THRESHOLD 64
  HOST_WIDE_INT align_intval, nbytes_intval, initval_intval;
  rtx destaddr, nbytes, initval, align, base_reg;
  int nbytes_left;

  /* Starting from the narrowest mode, figure out all valid modes */
  enum machine_mode valid_modes[MAX_MODE_INT-MIN_MODE_INT+1] = {VOIDmode};
  enum machine_mode curr_mode;
  int index = 0;

  destaddr = operands[0];
  nbytes = operands[1];
  initval = operands[2];
  align = operands[3];

  /* Sanity checks */
  gcc_assert(destaddr && nbytes && initval);

  /* nbytes should be constant */
  if (GET_CODE(nbytes) != CONST_INT)
    return false;

  /* align should be constant. We should probably assert but... */
  if (GET_CODE(align) != CONST_INT)
    return false;

  /* initval should be constant. */
  if (GET_CODE(initval) != CONST_INT)
    return false;

  align_intval = INTVAL(align);
  nbytes_intval = INTVAL(nbytes);
  initval_intval = INTVAL(initval);

  /* If nothing to set or large memory (memset might optimize it better), return */
  if (nbytes_intval <= 0 || nbytes_intval > MEMSET_LIBCALL_THRESHOLD)
    return false;

  /* If not clearing memory, return */
  if (initval_intval != 0)
    return false;

  /* Step 1. See comments above */
  for (curr_mode = GET_CLASS_NARROWEST_MODE(MODE_INT); curr_mode != VOIDmode;
       curr_mode = GET_MODE_WIDER_MODE(curr_mode))
    {
      /* Is mode supported */
      if (!TARGET_SCALAR_MODE_SUPPORTED_P (curr_mode))
        continue;

      /* Make sure alignment does not exceed align */
      if (GET_MODE_SIZE(curr_mode) > align_intval)
        break;

      valid_modes[index++] = curr_mode;
    }

  /* Index must be non-zero */
  gcc_assert(index);

  /* Get the base pointer */
  base_reg = copy_to_mode_reg(SImode, XEXP(destaddr, 0));
  nbytes_left = nbytes_intval;
  /* Step 2. Starting from the widest mode, generate hardware loop stores */
  for (index = index-1; index >= 0; index--)
    {
      int loop_count;
      rtx store_mem, label, zero_reg;
      curr_mode = valid_modes[index];
      loop_count = nbytes_left / GET_MODE_SIZE(curr_mode);
      /* Size is smaller than the alignment */
      if (loop_count == 0)
          continue;

      /* r = #0 */
      zero_reg = gen_reg_rtx(curr_mode);
      emit_move_insn(zero_reg, initval);

      store_mem = gen_rtx_MEM(curr_mode, base_reg);
      MEM_COPY_ATTRIBUTES(store_mem, destaddr);

      if (loop_count == 1)
        {
          /* Simple store.  */
          emit_move_insn(store_mem, zero_reg);

          /* Increment the base pointer.  */
          emit_insn(gen_addsi3(base_reg, base_reg,
                               gen_int_mode(GET_MODE_SIZE(curr_mode), SImode)));
        }
      else
        {
          /* Hardware loop */
          rtx count_reg, loopcount_rtx, pred_reg;

          label = gen_label_rtx();

          count_reg = gen_reg_rtx(SImode);
          loopcount_rtx = gen_int_mode(loop_count, SImode);
	  pred_reg = gen_reg_rtx (BImode);

	  emit_move_insn (count_reg, loopcount_rtx);

	  /* Doesn't explicitly create a hardware loop: we get slightly better
	     performance if we generate a normal loop, and leave hardware loop
	     generation to later passes.  */
	  emit_label (label);
	  emit_move_insn (store_mem, zero_reg);
	  emit_insn (gen_addsi3 (base_reg, base_reg,
				 gen_int_mode (GET_MODE_SIZE (curr_mode),
					       SImode)));
	  emit_insn (gen_addsi3 (count_reg, count_reg, constm1_rtx));
	  emit_move_insn (pred_reg, gen_rtx_GTU (BImode, count_reg,
						 const0_rtx));;
	  emit_jump_insn (gen_rtx_SET (VOIDmode, pc_rtx,
			    gen_rtx_IF_THEN_ELSE (VOIDmode,
			      gen_rtx_NE (VOIDmode, pred_reg, const0_rtx),
			      gen_rtx_LABEL_REF (Pmode, label), pc_rtx)));
        }

      /* Count leftover */
      nbytes_left = nbytes_left % GET_MODE_SIZE(curr_mode);
    }
  gcc_assert(nbytes_left == 0);
  return true;
#undef MEMSET_LIBCALL_THRESHOLD
}


/* Determine a static branch hint for INSN.  Return the hint in the form of a
   CONST_INT rtx.  Non-zero means "predict taken" and zero means "predict not
   taken". */

rtx
hexagon_branch_hint(rtx insn)
{
  rtx note = find_reg_note(insn, REG_BR_PROB, 0);
  return GEN_INT (note && INTVAL (XEXP (note, 0)) > REG_BR_PROB_BASE / 2);
}




void
hexagon_hardware_loop(void)
{
  cfun->machine->has_hardware_loops++;
}




bool
hexagon_valid_parallel_combine_insn_p(rtx insn)
{
  rtx pattern1;
  rtx pattern2;
  rtx insn1;
  rtx insn2;

  if(!NONJUMP_INSN_P (insn) || GET_CODE (PATTERN (insn)) != PARALLEL){
    return false;
  }

  pattern1 = XVECEXP (PATTERN (insn), 0, 0);
  pattern2 = XVECEXP (PATTERN (insn), 0, 1);

  /* Reject transfers. */
  if(GET_CODE (pattern1) == SET && GET_CODE (pattern2) == SET
     && (rtx_equal_p(SET_SRC (pattern1), SET_SRC (pattern2))
         || register_operand(SET_SRC (pattern1), GET_MODE (SET_SRC (pattern1)))
         || immediate_operand(SET_SRC (pattern1), GET_MODE (SET_SRC (pattern1)))
         || register_operand(SET_SRC (pattern2), GET_MODE (SET_SRC (pattern2)))
         || immediate_operand(SET_SRC (pattern2), GET_MODE (SET_SRC (pattern2))))){
    return false;
  }

  cfun->machine->combine_parallel_pattern1 = pattern1;
  cfun->machine->combine_parallel_pattern2 = pattern2;

  start_sequence();
  insn1 = emit_insn(pattern1);
  end_sequence();
  if(insn_invalid_p(insn1)){
    return false;
  }

  start_sequence();
  insn2 = emit_insn(pattern2);
  end_sequence();
  if(insn_invalid_p(insn2)){
    return false;
  }

  return    get_attr_compound(insn1) == COMPOUND_YES
         || get_attr_compound(insn2) == COMPOUND_YES;
}




bool
hexagon_must_not_swap_parallel_insn_p(rtx pattern1, rtx pattern2)
{
#if 0
  static FILE *fp = NULL;
#endif
  rtx insn1;
  rtx insn2;
  struct hexagon_insn_info insn_info;
  struct hexagon_reg_access *write;

#if 0
  if(fp == NULL){
    fp = fopen("Combines.c.0r.split", "a");
  }

  fputs("\nCombine parallels:\n", fp);
  print_rtl_single(fp, pattern1);
  print_rtl_single(fp, pattern2);
#endif

  start_sequence();
  insn2 = emit_insn(pattern2);
  end_sequence();

  memset(&insn_info, 0, sizeof(insn_info));
  insn_info.insn = insn2;
  insn_info.flags = hexagon_get_flags(insn2);
  for_each_rtx(&PATTERN (insn2), hexagon_record_reads, &insn_info);
  write = insn_info.reg_writes;

  start_sequence();
  insn1 = emit_insn(pattern1);
  end_sequence();

  memset(&insn_info, 0, sizeof(insn_info));
  insn_info.insn = insn1;
  insn_info.flags = hexagon_get_flags(insn1);
  for_each_rtx(&PATTERN (insn1), hexagon_record_reads, &insn_info);

  for(; write; write = write->next){
    if(hexagon_register_used(&insn_info, write->regno)){
      return true;
    }
  }
  return false;
}




/*--------------------------------------------
Functions for forming and manipulating packets
--------------------------------------------*/

void
hexagon_print_insn_info(FILE *file, struct hexagon_insn_info *insn_info)
{
  struct hexagon_reg_access *reg_access;
  struct hexagon_mem_access *mem_access;
  bool first;

  if(insn_info == NULL){
    fputs("!!!! NULL INSN_INFO !!!!\n", file);
    return;
  }

  fputs(";; condition: ", file);
  if(HEXAGON_GPR_CONDITION_P (insn_info)){
    fputs("gpr", file);
  }
  else if(HEXAGON_SENSE (insn_info) == HEXAGON_UNCONDITIONAL){
    fputs("unconditional", file);
  }
  else if(HEXAGON_SENSE (insn_info) == 0){
    fputs("never", file);
  }
  else {
    if(HEXAGON_SENSE (insn_info) == HEXAGON_IF_FALSE){
      fputc('!', file);
    }
    fputs(reg_names[P0_REGNUM + HEXAGON_PREDICATE (insn_info)], file);
    if(HEXAGON_NEW_PREDICATE_P (insn_info)){
      fputs(".new", file);
    }
  }
  fputc('\n', file);

  fputs(";; control: ", file);
  if(HEXAGON_CONTROL_P (insn_info)){
    if(HEXAGON_DIRECT_JUMP_P (insn_info)){
      fputs("jump", file);
    }
    if(HEXAGON_INDIRECT_JUMP_P (insn_info)){
      fputs("indirect jump", file);
    }
    if(HEXAGON_RETURN_P (insn_info)){
      fputs("return", file);
    }
    if(HEXAGON_DIRECT_CALL_P (insn_info)){
      fputs("call", file);
    }
    if(HEXAGON_INDIRECT_CALL_P (insn_info)){
      fputs("indirect call", file);
    }
    if(HEXAGON_ENDLOOP_P (insn_info)){
      fputs("endloop", file);
    }
  }
  else {
    fputs("none", file);
  }
  fputc('\n', file);

  fputs(";; flags: (", file);
  first = true;
  if(HEXAGON_EMULATION_CALL_P (insn_info)){
    fputs("emulation_call", file);
    first = false;
  }
  if(HEXAGON_VOLATILE_P (insn_info)){
    if(!first){
      fputs(", ", file);
    }
    fputs("volatile", file);
    first = false;
  }
  if(HEXAGON_MEM_P (insn_info)){
    if(!first){
      fputs(", ", file);
    }
    fputs("mem", file);
    first = false;
  }
  if(HEXAGON_NEW_GPR_P (insn_info)){
    if(!first){
      fputs(", ", file);
    }
    fputs("gpr.new", file);
  }
  if(HEXAGON_NEW_MEM_P (insn_info)){
    if(!first){
      fputs(", ", file);
    }
    fputs("mem.new", file);
  }
  fputs(")\n", file);

  fputs(";; register reads:", file);
  first = true;
  reg_access = insn_info->reg_reads;
  while(reg_access){
    if(!first){
      fputc(',', file);
    }
    fputc(' ', file);
    fputs(reg_names[reg_access->regno], file);
    first = false;
    reg_access = reg_access->next;
  }
  if(first){
    fputs(" none", file);
  }
  fputc('\n', file);

  fputs(";; register writes:", file);
  first = true;
  reg_access = insn_info->reg_writes;
  while(reg_access){
    if(!first){
      fputc(',', file);
    }
    fputc(' ', file);
    fputs(reg_names[reg_access->regno], file);
    first = false;
    reg_access = reg_access->next;
  }
  if(first){
    fputs(" none", file);
  }
  fputc('\n', file);

  fputs(";; loads:", file);
  first = true;
  mem_access = insn_info->loads;
  while(mem_access){
    if(first){
      fputc('\n', file);
    }
    print_rtl_single(file, mem_access->mem);
    first = false;
    mem_access = mem_access->next;
  }
  if(first){
    fputs(" none\n", file);
  }

  fputs(";; stores:", file);
  first = true;
  mem_access = insn_info->stores;
  while(mem_access){
    if(first){
      fputc('\n', file);
    }
    print_rtl_single(file, mem_access->mem);
    first = false;
    mem_access = mem_access->next;
  }
  if(first){
    fputs(" none\n", file);
  }

  fputs(";; insn:\n", file);
  print_rtl_single(file, insn_info->insn);
}




void
hexagon_debug_insn_info(struct hexagon_insn_info *insn_info)
{
  hexagon_print_insn_info(stderr, insn_info);
}




void
hexagon_print_packet(FILE *file, struct hexagon_packet_info *packet)
{
  int i;

  if(packet == NULL){
    fputs("!!!! NULL PACKET !!!!\n", file);
    return;
  }

  fputs(";; ==== BEGIN PACKET ====\n", file);
  for(i = 0; i < packet->num_insns && i < HEXAGON_MAX_INSNS_PER_PACKET; i++){
    hexagon_print_insn_info(file, packet->insns[i]);
  }
  fputs(";; ==== END PACKET ====\n", file);
}




void
hexagon_print_packets(FILE *file, struct hexagon_packet_info *packet)
{
  if(packet == NULL){
    hexagon_print_packet(file, packet);
    return;
  }

  do {
    hexagon_print_packet(file, packet);
    packet = packet->next;
  }while(packet);
}




void
hexagon_debug_packets(void)
{
  basic_block bb;
  FOR_EACH_BB(bb)
  {
    struct hexagon_packet_info *packet;
    packet = BB_HEAD_PACKET(bb);
    hexagon_print_packets(stderr, packet);
  }
}




void
hexagon_debug_packet(struct hexagon_packet_info *packet)
{
  hexagon_print_packet(stderr, packet);
}




void
hexagon_print_bb_packets(FILE *file, basic_block bb)
{
  struct hexagon_packet_info *packet;

  dump_bb_info(bb, true, false, TDF_DETAILS, ";; ", file);
  packet = BB_HEAD_PACKET (bb);
  if(packet){
    fputs("\n;; sentinel packet:\n", file);
    hexagon_print_packet(file, packet->prev);
  }
  fputc('\n', file);
  while(packet && packet != BB_END_PACKET (bb)){
    hexagon_print_packet(file, packet);
    packet = packet->next;
  }
  hexagon_print_packet(file, packet);
  dump_bb_info(bb, false, true, TDF_DETAILS, ";; ", file);
  fputs("\n\n", file);
}




void
hexagon_debug_bb_packets(basic_block bb)
{
  hexagon_print_bb_packets(stderr, bb);
}




void
hexagon_debug_all_bb_packets(void)
{
  basic_block bb;

  FOR_EACH_BB (bb){
   hexagon_debug_bb_packets(bb);
  }
}




static int
hexagon_get_flags(rtx insn)
{
  int flags = 0;
  rtx pattern;
  rtx test;
  rtx target;

  pattern = PATTERN (insn);
  if(GET_CODE (pattern) == PARALLEL){
    pattern = XVECEXP (pattern, 0, 0);
  }

  if(GET_CODE (pattern) == COND_EXEC){
    test = COND_EXEC_TEST (pattern);
  }
  else if(JUMP_P (insn) && GET_CODE (pattern) == SET
          && GET_CODE (SET_SRC (pattern)) == IF_THEN_ELSE){

    test = XEXP (SET_SRC (pattern), 0);

    if(C_REG_P (XEXP (test, 0))){
      flags |= HEXAGON_ENDLOOP;
      test = NULL_RTX;
    }
  }
  else {
    test = NULL_RTX;
  }

  if(test){
    if(P_REG_P (XEXP (test, 0))){
      flags |= REGNO (XEXP (test, 0)) - P0_REGNUM;
      flags |= GET_CODE (test) == NE ? HEXAGON_IF_TRUE : HEXAGON_IF_FALSE;
    }
    else {
      flags |= HEXAGON_GPR_CONDITION | HEXAGON_UNCONDITIONAL;
    }
  }
  else {
    flags |= HEXAGON_UNCONDITIONAL;
  }

  if ( INSN_CODE (insn) == CODE_FOR_compare_and_jump1 ||
       INSN_CODE (insn) == CODE_FOR_compare_and_jump2 )
  {
    flags |= HEXAGON_NEW_PREDICATE;
  }

  if(JUMP_P (insn) && !(flags & HEXAGON_ENDLOOP) ){
    if(GET_CODE (pattern) == RETURN){
      flags |= HEXAGON_RETURN;
    }
    else if (GET_CODE (pattern) == ASM_OPERANDS)
      {
        if (ASM_OPERANDS_LABEL_LENGTH (pattern) > 0)
          flags |= HEXAGON_DIRECT_JUMP;
        else
          flags |= HEXAGON_INDIRECT_JUMP;
      }
    else {
      gcc_assert(GET_CODE (pattern) == SET);
      target = SET_SRC (pattern);
      if(GET_CODE (target) == IF_THEN_ELSE){
        target = XEXP (target, 1);
      }

      if(GET_CODE (target) == RETURN){
        flags |= HEXAGON_RETURN;
      }
      else if(GET_CODE (target) == LABEL_REF){
        flags |= HEXAGON_DIRECT_JUMP;
      }
      else {
        flags |= HEXAGON_INDIRECT_JUMP;
      }
    }
  }
  else if(CALL_P (insn)){
    if(GET_CODE (pattern) == SET){
      pattern = SET_SRC (pattern);
    }

    gcc_assert(GET_CODE (pattern) == CALL && MEM_P (XEXP (pattern, 0)));
    if(REG_P (XEXP (XEXP (pattern, 0), 0))){
      flags |= HEXAGON_INDIRECT_CALL;
    }
    else {
      flags |= HEXAGON_DIRECT_CALL;
    }
  }
  /* Ignore instructions that do nothing */
  else if (DEBUG_INSN_P(insn) ||
	   (GET_CODE (PATTERN (insn)) == CLOBBER) ||
	   (GET_CODE (PATTERN (insn)) == USE) ||
	   (GET_CODE (PATTERN (insn)) == ADDR_VEC) ||
	   (GET_CODE (PATTERN (insn)) == ADDR_DIFF_VEC))
    {
      /* do nothing */
    }
  else if(get_attr_emulation_call(insn) == EMULATION_CALL_YES){
    flags |= HEXAGON_EMULATION_CALL;
  }

  return flags;
}




static struct hexagon_reg_access *
hexagon_add_reg_access(struct hexagon_reg_access *accesses, rtx reg, int flags)
{
  struct hexagon_reg_access *access;
  unsigned int regno;
  unsigned int next_hard_regno;

  /* Ignore duplicates. */
  for(access = accesses; access; access = access->next){
    if(rtx_equal_p(reg, access->reg)){
      return accesses;
    }
  }

  regno = REGNO (reg);
  next_hard_regno = regno + HARD_REGNO_NREGS (regno, GET_MODE (reg));
  for(; regno < next_hard_regno; regno++){
    access = ggc_alloc_cleared_hexagon_reg_access ();
    access->reg = reg;
    access->regno = regno;
    access->flags = flags;
    access->next = accesses;
    accesses = access;
  }
  return accesses;
}




static struct hexagon_mem_access *
hexagon_add_mem_access(struct hexagon_mem_access *accesses, rtx mem, int flags)
{
  struct hexagon_mem_access *access;

  /* Ignore duplicates. */
  for(access = accesses; access; access = access->next){
    if(rtx_equal_p(mem, access->mem)){
      return accesses;
    }
  }

  access = ggc_alloc_cleared_hexagon_mem_access ();
  access->mem = mem;
  access->flags = flags;
  access->next = accesses;
  return access;
}




static int
hexagon_record_writes(rtx *y, void *info)
{
  rtx x;
  struct hexagon_insn_info *insn_info;

  x = *y;
  insn_info = (struct hexagon_insn_info *) info;

  switch(GET_CODE (x)){
    case MEM:
      insn_info->stores = hexagon_add_mem_access(insn_info->stores,
                                               x,
                                               insn_info->flags);
      insn_info->flags |= HEXAGON_MEM;
      if(MEM_VOLATILE_P (x)){
        insn_info->flags |= HEXAGON_VOLATILE;
      }
      for_each_rtx(&XEXP (x, 0), hexagon_record_reads, insn_info);
      return -1;
    case REG:
      insn_info->reg_writes = hexagon_add_reg_access(insn_info->reg_writes,
                                                   x,
                                                   insn_info->flags);
      return 0;
    default:
      return 0;
  }
}




static int
hexagon_record_reads(rtx *y, void *info)
{
  rtx x;
  struct hexagon_insn_info *insn_info;
  int saved_flags;

  x = *y;
  insn_info = (struct hexagon_insn_info *) info;

  switch(GET_CODE (x)){
    case COND_EXEC:
      saved_flags = insn_info->flags;
      insn_info->flags = HEXAGON_UNCONDITIONAL;
      for_each_rtx(&COND_EXEC_TEST (x), hexagon_record_reads, insn_info);
      insn_info->flags = saved_flags;
      for_each_rtx(&COND_EXEC_CODE (x), hexagon_record_reads, insn_info);
      return -1;
    case SET:
      /* V2/V3/V4 insert instruction is unusual in the way that it reads and
         writes its destination, so we need to record reads here
         as a special case.
         (set (zero_extract (...) (...) (...)) (...)) */
      if(XEXP (x, 0) && GET_CODE (XEXP (x, 0)) == ZERO_EXTRACT){
          for_each_rtx(&XEXP (x, 0), hexagon_record_reads, insn_info);
      }
      /* fall through */
    case POST_MODIFY:
      if(GET_CODE (XEXP (x, 1)) != CALL){
        for_each_rtx(&XEXP (x, 0), hexagon_record_writes, insn_info);
      }
      for_each_rtx(&XEXP (x, 1), hexagon_record_reads, insn_info);
      return -1;
    case CALL:
      gcc_assert(MEM_P (XEXP (x, 0)));
      for_each_rtx(&XEXP (XEXP (x, 0), 0), hexagon_record_reads, insn_info);
      return -1;
    case CLOBBER:
      for_each_rtx(&XEXP (x, 0), hexagon_record_writes, insn_info);
      return -1;
    case POST_DEC:
    case POST_INC:
      for_each_rtx(&XEXP (x, 0), hexagon_record_writes, insn_info);
      return 0;
    case MEM:
      insn_info->loads = hexagon_add_mem_access(insn_info->loads,
                                              x,
                                              insn_info->flags);
      insn_info->flags |= HEXAGON_MEM;
      if(MEM_VOLATILE_P (x)){
        insn_info->flags |= HEXAGON_VOLATILE;
      }
      return 0;
    case REG:
      insn_info->reg_reads = hexagon_add_reg_access(insn_info->reg_reads,
                                                  x,
                                                  insn_info->flags);
      return 0;
    case RETURN:
      if(!HEXAGON_CALL_P (insn_info)){
        insn_info->reg_reads = hexagon_add_reg_access(insn_info->reg_reads,
                                                    gen_rtx_REG (word_mode,
                                                                 LINK_REGNUM),
                                                    insn_info->flags);
      }
      return 0;
    default:
      return 0;
  }
}




static struct hexagon_insn_info *
hexagon_get_insn_info(rtx insn)
{
  struct hexagon_insn_info *insn_info;

  insn_info = ggc_alloc_cleared_hexagon_insn_info ();

  insn_info->insn = insn;
  insn_info->flags = hexagon_get_flags(insn);

  /* Record accesses. */
  for_each_rtx(&PATTERN (insn), hexagon_record_reads, insn_info);

  /* Transfers of immediate values that cannot be encoded in a transfer
     immediate instruction are implemented as GP-relative loads. */
  if (DEBUG_INSN_P(insn) ||
      (GET_CODE (PATTERN (insn)) == CLOBBER) ||
      (GET_CODE (PATTERN (insn)) == USE) ||
      (GET_CODE (PATTERN (insn)) == ADDR_VEC) ||
      (GET_CODE (PATTERN (insn)) == ADDR_DIFF_VEC))
    {
      /* do nothing */
    }
  else if (get_attr_type(insn) == TYPE_LOAD)
    {
      insn_info->flags |= HEXAGON_MEM;
    }

  return insn_info;
}


/* Given register reg, return its corresponding pair register */
static inline unsigned int
get_pair_reg(unsigned int regno)
{
  return (regno & 0x1) ? regno - 1 : regno + 1;
}


/*
   Given a vector tranfer_source that maps register numbers to insn pointers:
   Iterate through the vector.
   If an element of the vector is a register copy with orig_reg as a source,
   replace that element with replacement
*/
static void
hexagon_replace_transfer_map(VEC(rtx, gc)* transfer_source, unsigned orig_reg,
			     rtx replacement)
{
  unsigned int iter;

  gcc_assert((replacement == NULL_RTX) || INSN_P(replacement));

  for (iter = 0; iter <  FIRST_PSEUDO_REGISTER; ++iter)
    {
      rtx element = VEC_index(rtx, transfer_source, iter);
      if (element != NULL_RTX)
	{
	  rtx element_source = SET_SRC (single_set (element));
	  if (REG_P (element_source) &&
	      (REGNO (element_source) == orig_reg))
	    {
	      VEC_replace(rtx, transfer_source, iter, replacement);
	    }
	}
    }
}


/* Predicate that returns true if insn is a register-to-register or
   a register-to-immediate copy */
static inline bool
hexagon_register_copy_word_p (rtx insn)
{
  rtx reg_copy;
  bool dest_reg, src_reg_imm;
  if (!insn || (!(reg_copy = single_set(insn))))
    {
      return false;
    }

  dest_reg = register_operand(SET_DEST (reg_copy), SImode);
  src_reg_imm = register_operand(SET_SRC (reg_copy), SImode) ||
    const_int_operand(SET_SRC (reg_copy), SImode);

  return (dest_reg && src_reg_imm);
}

/* Returns the pattern for immediate from an
   immediate-to-register copy */
static inline rtx
hexagon_register_copy_imm_p (rtx insn)
{
  rtx reg_copy;

  if (!insn || (!(reg_copy = single_set(insn))) ||
      !REG_P (SET_DEST (reg_copy)))
    {
      return NULL;
    }

  if (GET_CODE (SET_SRC (reg_copy)) == CONST_INT)
    return SET_SRC(reg_copy);

  if (GET_CODE (SET_SRC (single_set(insn))) == HIGH &&
      GET_CODE (XEXP (SET_SRC (single_set(insn)),0)) == CONST_INT)
    return XEXP (SET_SRC (single_set(insn)),0);

  if (GET_CODE (SET_SRC (single_set(insn))) == LO_SUM &&
      GET_CODE (XEXP (SET_SRC (single_set(insn)),1)) == CONST_INT)
    return XEXP (SET_SRC (single_set(insn)),1);

    return NULL;
}


/* Predicate that checks if REG is a lower component of a pair register */
#define HEXAGON_LOWER_PAIR(REG)         (!(REG & 0x1))


/*
   Pass that scans through all basic blocks and changes register-register
   and register-immediate transfers into combine instructions where possible.
   For instance, it combines:
   r0 = r3
   ...
   r1 = r4
   into r1:0 = combine(r4,r3)

   High-level overview of the pass:
   --------------------------------
   Iterate over all instructions I in the basic block.
   Maintain a map M from register number to insn pointer of the last write
   of that register
   For all register reads, R, clear map(R)
   Consider all transfer instructions of the form rx = y, where rx is a
   register and y is a register or an immediate
   Consult the map to verify if the corresponding pair register for rx
   was written to by a transfer. If so, replace the transfers with a combine
   instruction
   For all non-transfer instructions, iterate over the register writes
   and invalidate the corresponding entry in the map
*/
static void
hexagon_local_combine_pass(void)
{
  rtx insn;
  basic_block bb;
  rtx head_insn;
  rtx end_insn;

  /* transfer_source: vector that holds the sources used by a transfer
     instruction. The index is the physical register defined by the transfer.
     The vector element is the rtx of the source used by the transfer
     instruction.
     This vector depends on NULL_RTX being defined to 0
  */
  VEC(rtx, gc) *transfer_source = NULL;
  VEC_safe_grow_cleared(rtx, gc, transfer_source, FIRST_PSEUDO_REGISTER);

  FOR_EACH_BB(bb)
    {
      rtx combine_insn;
      head_insn = BB_HEAD (bb);
      end_insn = BB_END (bb);
      insn = head_insn;

      /* Reset the vector */
      VEC_truncate(rtx, transfer_source, 0);
      VEC_safe_grow_cleared(rtx, gc, transfer_source, FIRST_PSEUDO_REGISTER);

      FOR_BB_INSNS (bb, insn)
	{
	  struct hexagon_insn_info *insn_info;
	  struct hexagon_reg_access *read;
	  struct hexagon_reg_access *write;

	  /* Only process instructions */
	  if (!INSN_P(insn))
	    {
	      continue;
	    }
	  insn_info = hexagon_get_insn_info(insn);

	  /* Process all instructions -- transfers and non-transfers. If the
	     instruction is reading the register. Remove from map. This condition
	     must be checked before processing register copies. Else, the code will
	     incorrectly handle this situation:
	     r1 = r25
	     r0 = r1   ;; We must invalidate r1 before processing this instruction
                       ;; We cannot insert a r1:0=combine(r25,r1) here
	  */
	  for(read = insn_info->reg_reads; read; read = read->next)
	    {
	      VEC_replace(rtx, transfer_source, read->regno, NULL_RTX);
	    }


	  if (hexagon_register_copy_word_p (insn))
	    {
	      rtx source;
	      unsigned int dest_pair_regno;
	      rtx dest_pair_insn, source_pair;
	      unsigned int dest_regno = REGNO (SET_DEST(single_set (insn)));
	      gcc_assert(dest_regno < FIRST_PSEUDO_REGISTER);

	      /* Get the source for this transfer instruction */
	      source = SET_SRC (single_set (insn));
	      VEC_replace(rtx, transfer_source, dest_regno, insn);

	      /* A register has been defined. If that register is an element of
		 transfer_source, we can no longer consider that transfer to be valid.
		 Invalidate that element */
	      hexagon_replace_transfer_map (transfer_source, dest_regno, NULL_RTX);

	      /* Get the pair register for dest_regno */
	      dest_pair_regno = get_pair_reg(dest_regno);
	      if ((dest_pair_insn = VEC_index(rtx, transfer_source, dest_pair_regno)))
		{
		  enum machine_mode dummy_mode = VOIDmode;
		  source_pair = SET_SRC (single_set (dest_pair_insn));

		  /* Check if the sources for transfer instructions defining the pair are either:
		     1. Both registers, in which case we can use combine(reg, reg), or
		     2. Both immediates, in which case we can use combine(imm, imm)
		  */
		  /*
		        To consider: Can I make the int operand belongs to s8 code more generic?
		  */
		  if ((REG_P (source_pair) && REG_P (source)) ||
		      (CONST_INT_P (source_pair) && CONST_INT_P (source) &&
		       s8_const_int_operand(source, dummy_mode) &&
		       s8_const_int_operand(source_pair, dummy_mode)))
		    {
		      rtx dest_reg_rtx  = gen_rtx_REG(SImode, dest_regno);
		      rtx dest_pair_rtx = gen_rtx_REG(SImode, dest_pair_regno);

		      /* Check if the current instruction defines the lower or the higher pair
			 and create the appropriate combine instruction */
		      if (HEXAGON_LOWER_PAIR (dest_regno))
			{
			  combine_insn =
			    emit_insn_after( gen_combinesi (dest_pair_rtx,
							    source_pair,
							    dest_reg_rtx,
							    source),
					     insn);
			}
		      else
			{
			  combine_insn =
			    emit_insn_after( gen_combinesi (dest_reg_rtx,
							    source,
							    dest_pair_rtx,
							    source_pair),
					     insn);
			}

		      delete_insn(insn);
		      /* If this pass hasn't already deleted dest_pair_insn, then
			 delete it
			 This can happen if dest_pair_insn has been replaced with a combine,
			 and then the defined register is used in another replacement */
		      if (!INSN_DELETED_P (dest_pair_insn))
			{
			  delete_insn(dest_pair_insn);
			}
		      insn = combine_insn;
		    }
		}
	    } /* hexagon_is_register_copy */
	  else
	    {
	      /* For Call instructions we need to invalidate all caller-saved registers */
	      if (CALL_P(insn)) {
		unsigned int i;
		for (i = 0; i < VEC_length(rtx, transfer_source); ++i)
		  {
		    if (call_used_regs[i])
		      {
			VEC_replace(rtx, transfer_source, i, NULL_RTX);
			/* If any element of the vector contains a caller-save register
			   invalidate that entry */
			hexagon_replace_transfer_map (transfer_source, i, NULL_RTX);
		      }
		  }
	      }

	      /* An non-transfer instruction is writing to a register. Remove from map */
	      for(write = insn_info->reg_writes; write; write = write->next)
		{
		  unsigned int defined_reg = write->regno;

		  VEC_replace(rtx, transfer_source, defined_reg, NULL_RTX);
		  /* Register defined_reg is redefined. Iterate through vector and remove all
		     occurrences of register defined_reg as a source

		     This can be made more efficient by maintaining a map of regno to
		     a list of indices in transfer_source that contain regno as an
		     element */
		  hexagon_replace_transfer_map(transfer_source, defined_reg, NULL_RTX);
		}
	    }
	}
    }
}

/*
  Both rtx are set immediate, both have the same dst mode.
  Add asserts.
*/
static inline bool
imm_delta_ok_for_mode(rtx insn1, rtx insn2)
{
  HOST_WIDE_INT imm_1 = INTVAL (hexagon_register_copy_imm_p (insn1));
  HOST_WIDE_INT imm_2 = INTVAL (hexagon_register_copy_imm_p (insn2));
  HOST_WIDE_INT original_offset = imm_1 - imm_2;

  return CONST_OK_FOR_CONSTRAINT_P (original_offset, 'I', "Is16");
}

/*
   Pass that scans through all basic blocks and optimizes
   register = immediate; transfers where possible.
   For instance, it combines:
   r0 = #0080
   ...
   r1 = #0080
   or
   r1:0 = CONST64(#0xffff0000)
   ...
   r3:4 = CONST64(#0xffff0000)

   into

   r0 = #0080
   ...
   r1 = r0
   or
   r1:0 = CONST64(#0xffff0000)
   ...
   r3:4 = r1:0

   or with -G0

   r7.h = #HI(131072)
   r7.l = #LO(131072)
   r0.h = #HI(131072)
   r0.l = #LO(131072)

   into:

   r7.h = #HI(131072)
   r7.l = #LO(131072)
   r0 = r7

   and try to create base+offset style immediates:

   r0.h = #HI(131072)
   r0.l = #LO(131072)
   ...
   r0.h = #HI(131070)
   r0.l = #LO(131070)

   Change it to:

   r0.h = #HI(131072)
   r0.l = #LO(131072)
   ...
   r0 = add(r0,#-2);

   High-level overview of the pass:
   --------------------------------
   Iterate over all instructions I in the basic block.
   Consider all transfer instructions of the form rx = y, where rx is a
   register and y is an immediate.
   Maintain a map M from dest register number to insn pointer of the last
   immediate being written to that register.
   If the same immediate value is used as previously seen, and still "live",
   replace usage with reg copy.
   For all non-transfer instructions, iterate over the register writes
   and invalidate the corresponding entry in the map M.
*/

typedef union  {
    unsigned long long ulonglong;
    unsigned short ushort[4];
    unsigned int uint[2];
} converter_type;

typedef struct {
  rtx origin;
  HOST_WIDE_INT reg_const;
  unsigned char valid;
} const_table_type;

#define GPR_ONLY 32

static void
hexagon_local_combine_immediate_pass(void)
{
  rtx insn;
  basic_block bb;
  rtx immediate;
  /* immediate_single_set: array that holds the transfer
     instruction. The index is the physical register defined by the transfer.
     The vector element is the record of immediate and rtx of the
     instruction.
  */
  const_table_type immediate_single_set[GPR_ONLY];
  /* old_value is a way to keep around old value of the register
     in hope it is close enough to the new one to be set in it.
   */
  const_table_type old_value[GPR_ONLY];
  converter_type converter;
  unsigned int i;


  FOR_EACH_BB(bb)
    {
      insn = BB_HEAD (bb);

      /* Reset both vectors */
      for (i=0;i<GPR_ONLY;i++)
        {
          immediate_single_set[i].valid = 0;
          old_value[i].valid = 0;
        }

      FOR_BB_INSNS (bb, insn)
	{
	  /* Only process real instructions */
	  if (!INSN_P(insn))    continue;
	  if (DEBUG_INSN_P(insn))    continue;

	  /* See if this is an instruction of interest:
	     Something we know the result of at compile time.
	     - simple immediate transfer
	     - HI/LO transfer
	   */
	  if ((immediate = hexagon_register_copy_imm_p (insn)))
	    {
	      HOST_WIDE_INT current_imm = INTVAL (immediate);
	      unsigned int iter = 0;
	      unsigned int dest_regno = REGNO (SET_DEST(single_set (insn)));
	      bool code_modified = false;
	      converter.ulonglong = (unsigned long long)current_imm;

	      /* Only deal with GPRs for now */
	      if (dest_regno >= GPR_ONLY)   continue;

	      /* First deal with redundant CONST64 copies
		 and some other DI related redundancies
	       */
	      if (GET_MODE (SET_DEST (single_set (insn))) == DImode)
		{
		  /* If this is a small immediate, that would fit into
		     8bit, do not bother messing with it. It will unlikely
		     be profitable to replace it.
		     Nevertheless update the table with its value.
		   */
		  if (!CONST_OK_FOR_CONSTRAINT_P (current_imm, 'I', "Is8"))
		    {
		      for (iter = 0; iter <  GPR_ONLY; ++iter)
			{
			  if ((immediate_single_set[iter].valid == 0xf) &&
			      (immediate_single_set[get_pair_reg(iter)].valid == 0xf))
			    {

			      /* Two valid items are found in paired registers.
				 We can now do extensive tracking on this,
				 but for simplicity just handle identical
				 immediate case.
				 The other case would be of a similar constant
				 that we can somehow transform into what we need,
				 but opportunities are infrequent, and code to
				 handle them is bulky.
			       */
			      converter_type match_converter;

			      match_converter.uint[0] = immediate_single_set[iter].reg_const;
			      match_converter.uint[1] = immediate_single_set[get_pair_reg(iter)].reg_const;

			      /* For a register pair Rn+1:Rn, Rn stores the LSB,
				 Rn+1 sotres the MSB
				 LSB must go to uint[0]; MSB must go to uint[1]
			       */
			      if (iter < get_pair_reg(iter)
				  && (unsigned long long) current_imm == match_converter.ulonglong)
				{
				  /* Exact match */
				  gcc_assert(GET_CODE (SET_SRC (single_set (insn))) == CONST_INT);

				  if ((GET_MODE (SET_DEST (single_set (insn)))
					== GET_MODE (SET_DEST (single_set (immediate_single_set[iter].origin))))
				      && (iter != dest_regno)
				      && (iter != get_pair_reg(dest_regno)))
				    {
				      rtx pattern = copy_rtx (PATTERN (insn));
				      pattern = replace_rtx (pattern,
					  SET_SRC (single_set (insn)),
					  SET_DEST(single_set (immediate_single_set[iter].origin)));

				      pattern = emit_insn_after(pattern,insn);
				      delete_insn(insn);
				      code_modified = true;
				      break;
				    }
				}
			    }
			}
		    }
		  if(!code_modified)
		    {
		      immediate_single_set[dest_regno].valid = 0xf;
		      immediate_single_set[dest_regno].reg_const = converter.uint[0];
		      immediate_single_set[dest_regno].origin = insn;
		      immediate_single_set[get_pair_reg(dest_regno)].valid = 0xf;
		      immediate_single_set[get_pair_reg(dest_regno)].reg_const = converter.uint[1];
		      immediate_single_set[get_pair_reg(dest_regno)].origin = insn;

		      /* Also do not track split old values - they are
			 overriten now
		       */
		      old_value[dest_regno].valid = 0;
		      old_value[get_pair_reg(dest_regno)].valid = 0;
		    }
		}
	      /* Now that we are done with DI, do all other
		 integer data types except for predicates.
	       */
	      else
		{
		  /* The HI/LO pair in fact sets the same immediate
		     in two different instructions, so they need to be tracked
		     as a pair.
		     Normally LO always should come after HI, but if we
		     would encounter an assembly fragment that only
		     sets HI, or reverses the order, we do not want to
		     get confused here.
		   */
		  if(GET_CODE (SET_SRC (single_set(insn))) == HIGH)
		    {
		      if ((immediate_single_set[dest_regno].valid == 0x3) &&
			  (immediate_single_set[dest_regno].reg_const ==
			   converter.uint[0]))
			{
			  /* This is a case that should not happen
			     in compiler generated sequence:
			       r0.l = #LO(80954304)
			       r0.h = #HI(80954304)
			     But it is conceivable  in hand written assembly.
			     We are not handling this here, and to avoid accidents,
			     let's simply invalidate the entry.
			   */
			  immediate_single_set[dest_regno].valid = 0;
			  /* There was no code modified,
			     but this prevents further tracking of the
			     instruction.
			   */
			  code_modified = true;
			}
		      if (immediate_single_set[dest_regno].valid == 0xf)
			{
			  /* There is another full const in this reg.
			     Maybe it is a reuse of the reg to put another const in it.
			     Remember the old value. There is a chance we can reuse it.
			   */
			  old_value[dest_regno].valid = 0xf;
			  old_value[dest_regno].reg_const =
			      immediate_single_set[dest_regno].reg_const;
			  old_value[dest_regno].origin =
			      immediate_single_set[dest_regno].origin;
			}
		      if (!code_modified)
			{
			  immediate_single_set[dest_regno].valid = 0xc;
			  /* Remember the whole immediate,
			     but only 16 bit are actually valid.
			   */
			  immediate_single_set[dest_regno].reg_const = converter.uint[0];
			  immediate_single_set[dest_regno].origin = insn;
			}
                    }
		  else if (GET_CODE (SET_SRC (single_set(insn))) == LO_SUM)
		    {
		      if ((immediate_single_set[dest_regno].valid == 0xc) &&
			  (immediate_single_set[dest_regno].reg_const == converter.uint[0]))
			{
			  /* This means HI was already performed and the immediate matches
			     The set is complete. Look for matches.
			   */
			  for (iter = 0; iter <  GPR_ONLY; ++iter)
			    {
			      if (immediate_single_set[iter].valid == 0xf)
				{
				  /* Exact match */
				  if (immediate_single_set[iter].reg_const
				      == converter.uint[0])
				    {
				      rtx pattern = gen_movsi_real (
					  gen_rtx_REG (SImode, dest_regno),
					  gen_rtx_REG (SImode, iter));

				      pattern = emit_insn_after (pattern,insn);
				      delete_insn (insn);
				      delete_insn (immediate_single_set[dest_regno].origin);
				      code_modified = true;
				      immediate_single_set[dest_regno].valid = 0;

				      break;
				    }
				  /* Approximate match */
				  else if (imm_delta_ok_for_mode(insn,
				      immediate_single_set[iter].origin))
				    {
				      HOST_WIDE_INT original_offset
					= current_imm -
					  INTVAL (hexagon_register_copy_imm_p
						  (immediate_single_set[iter].origin));

				      rtx pattern
					= gen_addsi3_real (gen_rtx_REG(SImode, dest_regno),
							   gen_rtx_REG(SImode, iter),
							   gen_int_mode (original_offset, SImode));

				      pattern = emit_insn_after(pattern,insn);
				      delete_insn(insn);
				      delete_insn(immediate_single_set[dest_regno].origin);
				      code_modified = true;

				      /* Now I know exactly what is in this reg,
				         and it is NOT the same value
				         as might already exist there
				      */
				      immediate_single_set[dest_regno].valid = 0xf;
				      immediate_single_set[dest_regno].reg_const = current_imm;
				      immediate_single_set[dest_regno].origin = pattern;

				      break;
				    }
				}
			    }
			  /* Now check the same reg - maybe the old value in it
			     could be reused. This effectively creates
			     base+offset style immediates.
			   */
			  if (!code_modified &&
			      old_value[dest_regno].valid == 0xf)
			    {
			      if (imm_delta_ok_for_mode(insn,old_value[dest_regno].origin))
				{
				  /* Approximate match with previous
				     value in the same register
				   */
				  HOST_WIDE_INT original_offset = current_imm -
						  old_value[dest_regno].reg_const;
				  rtx pattern =
				    gen_addsi3_real (gen_rtx_REG(SImode, dest_regno),
						     gen_rtx_REG(SImode, dest_regno),
						     gen_int_mode (original_offset, SImode));

				  pattern = emit_insn_after(pattern,insn);
				  delete_insn(insn);
				  delete_insn(immediate_single_set[dest_regno].origin);
				  code_modified = true;

				  /* Now I know exactly what is in this reg,
				     and it is NOT the same value
				     as might have already exist there.
				   */
				  immediate_single_set[dest_regno].valid = 0xf;
				  immediate_single_set[dest_regno].reg_const = current_imm;
				  immediate_single_set[dest_regno].origin = pattern;

				  break;
				}
			    }
			}
		      if (!code_modified)
			{
			  /* New LO that was not modified.
			     Make available. Invalidate old value.
			   */
			  immediate_single_set[dest_regno].valid |= 0x3;
			  immediate_single_set[dest_regno].reg_const = converter.uint[0];
			  immediate_single_set[dest_regno].origin = insn;

			  old_value[dest_regno].valid = 0;
			}
		    }
		  else
		    {
		      /* Ordinary set. Most likely it is an immediate
			 transfer of a small value replacing which will only
			 create unnecesary dependency for packetizer, so
			 only update records, and do not try to modify.
		       */
		      immediate_single_set[dest_regno].valid = 0xf;
		      immediate_single_set[dest_regno].reg_const = converter.uint[0];
		      immediate_single_set[dest_regno].origin = insn;

		      old_value[dest_regno].valid = 0;
		    }
		}
	    } /* hexagon_register_copy_imm_p */
	  else
	    {
	      struct hexagon_insn_info *insn_info;
	      struct hexagon_reg_access *write;

	      /* For Call instructions we need to invalidate all caller-saved registers */
	      if (CALL_P(insn))
		{
		  for (i = 0; i < GPR_ONLY; ++i)
		    {
		      if (call_used_regs[i])
			{
			  immediate_single_set[i].valid = 0;
			  old_value[i].valid = 0;
			}
		    }
		}
	      insn_info = hexagon_get_insn_info(insn);
	      /* A non-transfer instruction is writing to a register.
		 Remove from map  */
	      for(write = insn_info->reg_writes; write; write = write->next)
		{
		  if(write->regno < GPR_ONLY)
		    {
		      immediate_single_set[write->regno].valid = 0;
		      old_value[write->regno].valid = 0;
		    }
		}
	    }
	}
    }
}

static struct hexagon_packet_info *
hexagon_start_new_packet(void)
{
  struct hexagon_packet_info *new_packet;

  new_packet = ggc_alloc_cleared_hexagon_packet_info ();

  if(hexagon_head_packet == NULL){
    hexagon_head_packet = new_packet;
  }
  else {
    gcc_assert(hexagon_tail_packet);
    hexagon_tail_packet->next = new_packet;
    new_packet->prev = hexagon_tail_packet;
  }
  hexagon_tail_packet = new_packet;

  return new_packet;
}




/* Add INSN_INFO to PACKET.  If ADD_TO_BEGINNING is true, the add INSN_INFO to
   the beginning of PACKET.  If ADD_TO_BEGINNING is false and INSN_INFO is not a
   control insn, then add INSN_INFO above any jump insns already in PACKET.  If
   ADD_TO_BEGINNING is false and INSN_INFO is a control insn, then add INSN_INFO
   above any unconditional jumps in PACKET, but below any conditional jumps in
   PACKET. */

static void
hexagon_add_insn_to_packet(
  struct hexagon_packet_info *packet,
  struct hexagon_insn_info *insn_info,
  bool add_to_beginning
)
{
  int i;

  gcc_assert(packet->num_insns < HEXAGON_MAX_INSNS_PER_PACKET);

  if(!packet->location){
    packet->location = insn_info->insn;
  }

  for(i = packet->num_insns;
      i > 0
      && ((HEXAGON_JUMP_P (packet->insns[i - 1])
           && (!HEXAGON_CONDITIONAL_P (packet->insns[i - 1])
               || !HEXAGON_CONTROL_P (insn_info)))
          || add_to_beginning);
      i--){
    packet->insns[i] = packet->insns[i - 1];
  }
  packet->insns[i] = insn_info;
  packet->num_insns++;
}




static void
hexagon_remove_insn_from_packet(
  struct hexagon_packet_info *packet,
  struct hexagon_insn_info *insn_info
)
{
  int i;

  /* Find the insn. */
  for(i = 0; i < packet->num_insns; i++){
    if(packet->insns[i] == insn_info){
      break;
    }
  }

  /* That insn wasn't in this packet. */
  gcc_assert(i < packet->num_insns);

  /* Remove the insn. */
  for(; i < packet->num_insns - 1; i++){
    packet->insns[i] = packet->insns[i + 1];
  }
  packet->insns[i] = NULL;
  packet->num_insns--;
}




static bool
hexagon_can_speculate_p(struct hexagon_insn_info *insn_info, basic_block bb)
{
  struct hexagon_reg_access *write;

  /* Don't speculate any control insns, stores, or insns that might trap. */
  if(HEXAGON_CONTROL_P (insn_info)
     || insn_info->stores
     || may_trap_or_fault_p(PATTERN (insn_info->insn))){
    return false;
  }

  for(write = insn_info->reg_writes; write; write = write->next){
    if(TEST_HARD_REG_BIT (BB_LIVE_OUT (bb), write->regno)){
      return false;
    }
  }

  return true;
}




static void
hexagon_add_live_out(struct hexagon_insn_info *insn_info, basic_block bb)
{
  struct hexagon_reg_access *write;

  for(write = insn_info->reg_writes; write; write = write->next){
    SET_HARD_REG_BIT (BB_LIVE_OUT (bb), write->regno);
  }
}




static bool
hexagon_predicable_p(struct hexagon_insn_info *insn_info)
{
  rtx cond_exec;

  /* Form a COND_EXEC wrapper around the pattern of the insn to test for
     predicability. */
  cond_exec = gen_rtx_COND_EXEC (VOIDmode,
                                 gen_rtx_NE (VOIDmode,
                                             gen_rtx_REG (BImode, P0_REGNUM),
                                             const0_rtx),
                                 PATTERN (insn_info->insn));
  start_sequence();
  if(JUMP_P (insn_info->insn)){
    emit_jump_insn(cond_exec);
  }
  else if(CALL_P (insn_info->insn)){
    emit_call_insn(cond_exec);
  }
  else {
    emit_insn(cond_exec);
  }
  cond_exec = get_insns();
  end_sequence();

  /* If the predicated version is valid, then the original insn is
     predicable. */
  return !insn_invalid_p(cond_exec);
}




static struct hexagon_insn_info *
hexagon_predicate_insn(
  struct hexagon_insn_info *insn_info,
  struct hexagon_insn_info *jump_insn_info,
  bool invert_condition
)
{
  struct hexagon_insn_info *new_insn_info;
  rtx insn;
  rtx pattern;
  rtx test;
  enum rtx_code test_code;

  pattern = PATTERN (jump_insn_info->insn);
  if(GET_CODE (pattern) == PARALLEL){
    pattern = XVECEXP (pattern, 0, 0);
  }
  test = XEXP (SET_SRC (pattern), 0);
  test_code = GET_CODE (test);
  gcc_assert(test_code == EQ || test_code == NE);
  if(invert_condition){
    test_code = test_code == EQ ? NE : EQ;
  }

  pattern = copy_rtx(PATTERN (insn_info->insn));
  pattern = gen_rtx_COND_EXEC (VOIDmode,
                               gen_rtx_fmt_ee(test_code,
                                              GET_MODE (test),
                                              XEXP (test, 0),
                                              XEXP (test, 1)),
                               pattern);

  start_sequence();
  if(JUMP_P (insn_info->insn)){
    emit_jump_insn(pattern);
  }
  else if(CALL_P (insn_info->insn)){
    emit_call_insn(pattern);
  }
  else {
    emit_insn(pattern);
  }
  insn = get_insns();
  end_sequence();

  new_insn_info = hexagon_get_insn_info(insn);
  new_insn_info->stack = insn_info;

  if(HEXAGON_NEW_PREDICATE_P (insn_info)){
    new_insn_info->flags |= HEXAGON_NEW_PREDICATE;
  }
  if(HEXAGON_NEW_GPR_P (insn_info)){
    new_insn_info->flags |= HEXAGON_NEW_GPR;
  }
  if(HEXAGON_NEW_MEM_P (insn_info)){
    new_insn_info->flags |= HEXAGON_NEW_MEM;
  }
  if(HEXAGON_MOVED_P (insn_info)){
    new_insn_info->flags |= HEXAGON_MOVED;
  }
  set_block_for_insn(new_insn_info->insn, BLOCK_FOR_INSN (insn_info->insn));

  return new_insn_info;
}


static bool
hexagon_can_be_new_value_p(
  struct hexagon_insn_info *producer,
  struct hexagon_insn_info *consumer,
  struct hexagon_dependence *dependence
)
{
  rtx pattern;
  rtx set;

  /* If the producer is conditional, then consumer must have the same
     condition. */
  if(HEXAGON_CONDITIONAL_P (producer)
     && (HEXAGON_CONDITION (consumer) != HEXAGON_CONDITION (producer)
         || (HEXAGON_NEW_PREDICATE_P (consumer)
             && !HEXAGON_NEW_PREDICATE_P (producer))))
  {
    return false;
  }

  /* The new value must not come from or be a register pair. */
  if(GET_MODE_SIZE (GET_MODE (dependence->set)) > UNITS_PER_WORD
     || GET_MODE_SIZE (GET_MODE (dependence->use)) > UNITS_PER_WORD){
    return false;
  }

  /* The producer may be conditional. */
  pattern = PATTERN (producer->insn);
  if(GET_CODE (pattern) == COND_EXEC){
    pattern = COND_EXEC_CODE (pattern);
  }

  /* The new value must not come from a post-update address. */
  if(GET_CODE (pattern) != SET){
    return false;
  }
  set = SET_DEST (pattern);
  if(GET_CODE (set) == ZERO_EXTRACT){
    set = XEXP (set, 0);
  }
  if(set != dependence->set){
    return false;
  }

  return true;
}


static bool
hexagon_can_be_new_value_store_p(
  struct hexagon_insn_info *producer,
  struct hexagon_insn_info *consumer,
  struct hexagon_dependence *dependence
)
{
  if(!TARGET_NEW_VALUE_STORES)
  {
    return false;
  }

  if (!hexagon_can_be_new_value_p(producer, consumer, dependence))
  {
    return false;
  }

  return true;
}

static bool
hexagon_can_be_new_value_jump_p(
  struct hexagon_insn_info *consumer,
  struct hexagon_dependence *dependence
)
{

  if(!(TARGET_V4_FEATURES && TARGET_NEW_VALUE_JUMP))
  {
    return false;
  }

  if (HEXAGON_DIRECT_JUMP_P(consumer) &&
      (INSN_CODE(consumer->insn) == CODE_FOR_new_value_jump1 ||
       INSN_CODE(consumer->insn) == CODE_FOR_new_value_jump2 ||
       INSN_CODE(consumer->insn) == CODE_FOR_new_value_jump_tstbit) &&
      hexagon_nvj_new_gpr_p(consumer, dependence))
  {
    return true;
  }

  return false;
}

static bool
hexagon_is_floating_point_p(rtx *x, void *data);

static bool
hexagon_gpr_dot_newable_p(
  struct hexagon_insn_info *producer,
  struct hexagon_insn_info *consumer,
  struct hexagon_dependence *dependence
)
{
  rtx insn;
  rtx pattern;

  /* Disallow an FP producer with a new value jump consumer.  */
  if(TARGET_V5_FEATURES && HEXAGON_DIRECT_JUMP_P(consumer))
  {
    pattern = PATTERN (producer->insn);
    if(for_each_rtx(&pattern, hexagon_is_floating_point_p, NULL)){
      return false;
    }
  }

  if (hexagon_can_be_new_value_jump_p( consumer, dependence))
  {
    return true;
  }

  if (!hexagon_can_be_new_value_store_p( producer, consumer, dependence))
  {
     return false;
  }

  /* Try wrapping all uses of the register in the consumer in a NEW_VALUE
     unspec. */
  pattern = copy_rtx(PATTERN (consumer->insn));
  pattern = replace_rtx(pattern,
                        dependence->use,
                        gen_rtx_UNSPEC (GET_MODE (dependence->use),
                                        gen_rtvec(1, dependence->use),
                                        UNSPEC_NEW_VALUE));

  start_sequence();
  if(JUMP_P (consumer->insn)){
    emit_jump_insn(pattern);
  }
  else if(CALL_P (consumer->insn)){
    emit_call_insn(pattern);
  }
  else {
    emit_insn(pattern);
  }
  insn = get_insns();
  end_sequence();

  /* If the resulting insn is valid, then the original insn is .new-able. */
  return !insn_invalid_p(insn);
}




  /* get compare portion of the nvj*/
  /* get compare - cmp.XX(Ns.new, YY) */
static rtx
hexagon_nvj_get_compare(rtx nvj_insn, int elem_num)
{
  rtx pattern;
  rtx condition;
  rtx compare;

  pattern = PATTERN(nvj_insn);

  /* get condition code - if(cmp.XX(Ns.new,YY)) */
  gcc_assert(PARALLEL==GET_CODE(pattern));
  condition = XVECEXP(pattern, 0, elem_num);

  if (GET_CODE(XEXP(condition,1)) == IF_THEN_ELSE)
  {
    /* get compare - cmp.XX(Ns.new, YY) */
    compare  = XEXP(XEXP(condition, 1),0);
  }
  else
  {
    compare  = XEXP(condition, 1);
  }

  return compare;
}

static rtx
hexagon_nvj_get_operand( rtx insn, int op_count)
{
  return XEXP(insn, op_count-1);
}


/* first get the first element and see it's
   EQ, if yes, swap it and get the second element
   insn, and swap that as well */
static rtx
hexagon_nvj_swap_operands( rtx insn )
{
  rtx compare;
  rtx operand1;
  rtx operand2;

  /*first element insn of nvj insn set*/
  compare = hexagon_nvj_get_compare(insn,0);

  if (!(GET_CODE(compare) == EQ ||
        GET_CODE(compare) == NE ))
    return NULL;

  operand1 = hexagon_nvj_get_operand(compare, 1);
  operand2 = hexagon_nvj_get_operand(compare, 2);

  if ( GET_CODE(operand2) == UNSPEC &&
       XINT(operand2,1) == UNSPEC_NEW_VALUE )
  {
    /*now swap the operands correctly*/
    XEXP(compare, 0)=operand2;
    XEXP(compare, 1)=operand1;
    /*first element of the nvj instruction is swapped*/

    /*now get the second pair, and swap them
      as well. .new property of the first
      and the second instruction will always
      be the same*/
    compare = hexagon_nvj_get_compare(insn,1);
    operand1 = hexagon_nvj_get_operand(compare, 1);
    operand2 = hexagon_nvj_get_operand(compare, 2);

    XEXP(compare, 0)=operand2;
    XEXP(compare, 1)=operand1;
    /*second element of the nvj instruction is swapped*/
  }
  return insn;
}




static struct hexagon_insn_info *
hexagon_dot_newify_gpr(
  struct hexagon_insn_info *insn_info,
  struct hexagon_dependence *dependence
)
{
  struct hexagon_insn_info *new_insn_info;
  rtx insn;
  rtx pattern;

  pattern = copy_rtx(PATTERN (insn_info->insn));
  pattern = replace_rtx(pattern,
                        dependence->use,
                        gen_rtx_UNSPEC (GET_MODE (dependence->use),
                                        gen_rtvec(1, dependence->use),
                                        UNSPEC_NEW_VALUE));

  start_sequence();
  if(JUMP_P (insn_info->insn)){
    emit_jump_insn(pattern);
  }
  else if(CALL_P (insn_info->insn)){
    emit_call_insn(pattern);
  }
  else {
    emit_insn(pattern);
  }
  insn = get_insns();
  end_sequence();

  /* Make assembler happy and swap the operands */
  if (TARGET_NEW_VALUE_JUMP &&
      (INSN_CODE(insn_info->insn) == CODE_FOR_new_value_jump1   ||
       INSN_CODE(insn_info->insn) == CODE_FOR_new_value_jump2   ||
       INSN_CODE(insn_info->insn) == CODE_FOR_compare_and_jump1 ||
       INSN_CODE(insn_info->insn) == CODE_FOR_compare_and_jump2)
     )
  {
    rtx original_insn = insn;
    if ((insn = hexagon_nvj_swap_operands(original_insn))==NULL)
      insn = original_insn;
  }

  new_insn_info = hexagon_get_insn_info(insn);
  new_insn_info->stack = insn_info;
  new_insn_info->flags |= HEXAGON_NEW_GPR;


  if (TARGET_NEW_VALUE_JUMP &&
      (INSN_CODE(insn_info->insn) == CODE_FOR_new_value_jump1   ||
       INSN_CODE(insn_info->insn) == CODE_FOR_new_value_jump2   ||
       INSN_CODE(insn_info->insn) == CODE_FOR_compare_and_jump1 ||
       INSN_CODE(insn_info->insn) == CODE_FOR_compare_and_jump2 ||
       INSN_CODE(insn_info->insn) == CODE_FOR_new_value_jump_tstbit   ||
       INSN_CODE(insn_info->insn) == CODE_FOR_compare_and_jump_tstbit ))
  {
    rtx note = find_reg_note (insn_info->insn, REG_BR_PROB, 0);
    if (note)
      add_reg_note (insn, REG_BR_PROB, note);
    JUMP_LABEL(insn) = JUMP_LABEL(insn_info->insn);
  }

  if(HEXAGON_NEW_PREDICATE_P (insn_info)){
    new_insn_info->flags |= HEXAGON_NEW_PREDICATE;
  }
  if(HEXAGON_NEW_MEM_P (insn_info)){
    new_insn_info->flags |= HEXAGON_NEW_MEM;
  }
  if(HEXAGON_MOVED_P (insn_info)){
    new_insn_info->flags |= HEXAGON_MOVED;
  }
  set_block_for_insn(new_insn_info->insn, BLOCK_FOR_INSN (insn_info->insn));

  return new_insn_info;
}




static bool
hexagon_mem_dot_newable_p(
  struct hexagon_insn_info *producer,
  struct hexagon_insn_info *consumer,
  struct hexagon_dependence *dependence ATTRIBUTE_UNUSED
)
{
  return TARGET_V5_FEATURES && TARGET_NEW_MEMORY_LOADS
         && !HEXAGON_EMULATION_CALL_P (producer)
         && !HEXAGON_EMULATION_CALL_P (consumer)
         && consumer->loads && !consumer->loads->next && !consumer->stores;
}




static int
hexagon_dot_newify_rtx(rtx *x, void *voidp_use)
{
  rtx use = (rtx) voidp_use;
  if(rtx_equal_p(*x, use)){
    *x = gen_rtx_UNSPEC (GET_MODE (use),
                         gen_rtvec(1, copy_rtx(use)),
                         UNSPEC_NEW_VALUE);
    return -1;
  }
  return 0;
}




static struct hexagon_insn_info *
hexagon_dot_newify_mem(
  struct hexagon_insn_info *insn_info,
  struct hexagon_dependence *dependence
)
{
  struct hexagon_insn_info *new_insn_info;
  rtx insn;
  rtx pattern;

  pattern = copy_rtx(PATTERN (insn_info->insn));
  for_each_rtx(&pattern, hexagon_dot_newify_rtx, dependence->use);

  start_sequence();
  if(JUMP_P (insn_info->insn)){
    emit_jump_insn(pattern);
  }
  else if(CALL_P (insn_info->insn)){
    emit_call_insn(pattern);
  }
  else {
    emit_insn(pattern);
  }
  insn = get_insns();
  end_sequence();

  new_insn_info = hexagon_get_insn_info(insn);
  new_insn_info->stack = insn_info;
  new_insn_info->flags |= HEXAGON_NEW_MEM;


  if(HEXAGON_NEW_PREDICATE_P (insn_info)){
    new_insn_info->flags |= HEXAGON_NEW_PREDICATE;
  }
  if(HEXAGON_NEW_GPR_P (insn_info)){
    new_insn_info->flags |= HEXAGON_NEW_GPR;
  }
  if(HEXAGON_MOVED_P (insn_info)){
    new_insn_info->flags |= HEXAGON_MOVED;
  }
  set_block_for_insn(new_insn_info->insn, BLOCK_FOR_INSN (insn_info->insn));

  return new_insn_info;
}




static int
hexagon_find_new_value(rtx *x, void *y)
{
  rtx *new_value = (rtx *) y;
  if(GET_CODE (*x) == UNSPEC && XINT (*x, 1) == UNSPEC_NEW_VALUE){
    *new_value = *x;
    return -1;
  }
  return 0;
}




static void
hexagon_dot_oldify_insn(struct hexagon_insn_info *insn_info)
{
  rtx insn;
  rtx pattern;
  rtx new_value;

  if (TARGET_NEW_VALUE_JUMP &&
      (INSN_CODE(insn_info->insn) == CODE_FOR_new_value_jump1 ||
       INSN_CODE(insn_info->insn) == CODE_FOR_new_value_jump2 ||
       INSN_CODE(insn_info->insn) == CODE_FOR_new_value_jump_tstbit )) {
    pattern = copy_rtx(PATTERN(insn_info->insn));

    /* new value jump patterns have two instructions */
    /* eg. if (cmp.XX(r0.new,#-1)) jump; p0 = cmp.XX(r0.new, #-1) */
    /* one is the actual new value jump - get the .new new operand from the first one*/
    for_each_rtx(&XVECEXP(pattern,0,0), hexagon_find_new_value, &new_value);
    XVECEXP(pattern,0,0) = replace_rtx(XVECEXP(pattern,0,0), new_value, XVECEXP (new_value, 0, 0));

    /* the other one is actually restricting the use of the predicate, in case we have to fall
       out to the original jump (non new value jump) - get the .new new operand from the second one*/
    for_each_rtx(&XVECEXP(pattern,0,1), hexagon_find_new_value, &new_value);
    XVECEXP(pattern,0,1) = replace_rtx(XVECEXP(pattern,0,1), new_value, XVECEXP (new_value, 0, 0));

  } else
  {
    pattern = copy_rtx(PATTERN (insn_info->insn));
    for_each_rtx(&pattern, hexagon_find_new_value, &new_value);
    pattern = replace_rtx(pattern, new_value, XVECEXP (new_value, 0, 0));
  }


  if(JUMP_P (insn_info->insn)){
    insn = emit_jump_insn_after(pattern, insn_info->insn);
    if (INSN_CODE(insn_info->insn) == CODE_FOR_new_value_jump1 ||
        INSN_CODE(insn_info->insn) == CODE_FOR_new_value_jump2 ||
        INSN_CODE(insn_info->insn) == CODE_FOR_new_value_jump_tstbit )
    {
      rtx note = find_reg_note (insn_info->insn, REG_BR_PROB, 0);
      if (note)
	add_reg_note (insn, REG_BR_PROB, note);
      JUMP_LABEL(insn) = JUMP_LABEL(insn_info->insn);
    }
  }
  else if(CALL_P (insn_info->insn)){
    insn = emit_call_insn_after(pattern, insn_info->insn);
  }
  else {
    insn = emit_insn_after(pattern, insn_info->insn);
  }
  set_block_for_insn(insn, BLOCK_FOR_INSN (insn_info->insn));
  delete_insn(insn_info->insn);
}


static bool
hexagon_is_floating_point_p(rtx *x, void *data ATTRIBUTE_UNUSED)
{
  if(FLOAT_MODE_P (GET_MODE (*x))){
      return true;
  }
  return false;
}


static bool
hexagon_predicate_dot_newable_p (struct hexagon_insn_info *producer ATTRIBUTE_UNUSED,
                                 struct hexagon_insn_info *insn_info)
{
  rtx pattern;
  rtx address;

  if (!TARGET_NEW_PREDICATES)
    return false;

  /* Insns must already be conditional on a predicate condition to be
     new-predicable. */
  if (!HEXAGON_CONDITIONAL_P (insn_info)
      || HEXAGON_GPR_CONDITION_P (insn_info))
    return false;

  /* Before V4, the only control insns that could be .new predicated were
     direct jumps. In V4, all control instructions can be .new predicated
     except calls and register-condition jumps. */
  if (HEXAGON_CONTROL_P (insn_info)
      && (HEXAGON_CALL_P (insn_info)
          || (!TARGET_V4_FEATURES && !HEXAGON_DIRECT_JUMP_P (insn_info))))
    return false;

  /* In V4, all non-control predicable instructions are .new predicable. */
  if (TARGET_V4_FEATURES)
    return true;

  pattern = PATTERN (insn_info->insn);
  if (GET_CODE (pattern) == COND_EXEC)
    {
      pattern = COND_EXEC_CODE (pattern);
      if (GET_CODE (pattern) == SET)
        {
          /* Stores are not .newable. */
          if (MEM_P (SET_DEST (pattern)))
            return false;

          /* loads */
          if (MEM_P (SET_SRC (pattern)))
            {
              address = XEXP (SET_SRC (pattern), 0);
              /* Only base + offset loads are .newable. */
              if(G_REG_P (address) || GET_CODE (address) == PLUS)
                return true;
              return false;
            }
        }
    }
  return true;
}




static struct hexagon_insn_info *
hexagon_dot_newify_predicate(struct hexagon_insn_info *insn_info)
{
  struct hexagon_insn_info *new_insn_info;
  new_insn_info = ggc_alloc_hexagon_insn_info ();
  memcpy(new_insn_info, insn_info, sizeof(struct hexagon_insn_info));
  new_insn_info->stack = insn_info;
  new_insn_info->flags |= HEXAGON_NEW_PREDICATE;
  return new_insn_info;
}




/* Return true if INSN_INFO appears to be part of the prologue. */

static bool
hexagon_prologue_insn_p(struct hexagon_insn_info *insn_info)
{
  struct hexagon_reg_access *write;
  struct hexagon_reg_access *read;
  struct hexagon_mem_access *store;
  rtx address;
  rtx reg;

  /* Assume any instruction that adjusts the frame pointer or stack pointer is
     part of the prologue. */
  for(write = insn_info->reg_writes; write; write = write->next){
    if(write->regno == HARD_FRAME_POINTER_REGNUM
       || write->regno == STACK_POINTER_REGNUM){
      return true;
    }
  }

  /* Assume any instruction that reads a callee-save register and stores to
     memory using FP or SP as a base is part of the prologue. */
  for(read = insn_info->reg_reads; read; read = read->next){
    if(!(call_used_regs[read->regno] || read->regno == LINK_REGNUM)){
      for(store = insn_info->stores; store; store = store->next){
        address = XEXP (store->mem, 0);
        reg = GET_CODE (address) == PLUS ? XEXP (address, 0) : address;
        if(REG_P (reg) && (REGNO (reg) == HARD_FRAME_POINTER_REGNUM
                           || REGNO (reg) == STACK_POINTER_REGNUM)){
          return true;
        }
      }
      break;
    }
  }

  return false;
}




static struct hexagon_dependence *
hexagon_add_dependence(
  struct hexagon_dependence *dependencies,
  enum hexagon_dependence_type type,
  rtx set,
  rtx use
)
{
  struct hexagon_dependence *new_dependence;
  new_dependence = ggc_alloc_hexagon_dependence ();
  new_dependence->type = type;
  new_dependence->set = set;
  new_dependence->use = use;
  new_dependence->next = dependencies;
  return new_dependence;
}




static struct hexagon_dependence *
hexagon_remove_dependence(
  struct hexagon_dependence *dependencies,
  struct hexagon_dependence *dependence
)
{
  struct hexagon_dependence *dep;

  gcc_assert(dependence);

  if(dependencies == dependence){
    return dependencies->next;
  }

  for(dep = dependencies; dep->next != dependence; dep = dep->next){
    /* The dependence to be removed must be in the list. */
    gcc_assert(dep->next);
  }
  dep->next = dep->next->next;

  return dependencies;
}




static struct hexagon_dependence *
hexagon_concat_dependencies(
  struct hexagon_dependence *a,
  struct hexagon_dependence *b
)
{
  struct hexagon_dependence *c;
  if(!a){
    return b;
  }
  for(c = a; c->next; c = c->next);
  c->next = b;
  return a;
}


static struct hexagon_dependence *
hexagon_control_dependencies(
  struct hexagon_insn_info *first,
  struct hexagon_insn_info *second
)
{
  struct hexagon_dependence *dependencies = NULL;

  if(HEXAGON_CONTROL_P (first)){
    /* Allow dual jumps by not adding a control dependence between two jumps
       that are allowed to be in the same packet. */
    if(!((TARGET_V3_FEATURES
          /* The first jump must be conditional. */
          && HEXAGON_CONDITIONAL_P (first)
          /* Indirect jumps are not allowed to be dual jumps. */
          && HEXAGON_DIRECT_JUMP_P (first) && HEXAGON_DIRECT_JUMP_P (second)
          /* In V3, dot-new jumps are not allowed to be dual jumps. */
          && !HEXAGON_NEW_PREDICATE_P (second) && !HEXAGON_NEW_PREDICATE_P (first)
          /* Register-condition jumps are not allowed to be dual jumps. */
          && !HEXAGON_GPR_CONDITION_P (second) && !HEXAGON_GPR_CONDITION_P (first))
         || (TARGET_V4_FEATURES
             /* The first jump must be conditional. */
             && HEXAGON_CONDITIONAL_P (first)
             /* Indirect jumps are not allowed to be dual jumps. */
             && (HEXAGON_DIRECT_JUMP_P (first) || HEXAGON_DIRECT_CALL_P (first))
             && (HEXAGON_DIRECT_JUMP_P (second) || HEXAGON_DIRECT_CALL_P (second))
             /* Register-condition jumps are not allowed to be dual jumps. */
             && !HEXAGON_GPR_CONDITION_P (first)
             && !HEXAGON_GPR_CONDITION_P (second)))){
      dependencies = hexagon_add_dependence(dependencies,
                                          HEXAGON_DEP_CONTROL,
                                          first->insn,
                                          second->insn);
    }
  }

  /* Both insns are actually calls. */
  if(HEXAGON_EMULATION_CALL_P (first) && HEXAGON_EMULATION_CALL_P (second)){
    dependencies = hexagon_add_dependence(dependencies,
                                        HEXAGON_DEP_CONTROL,
                                        first->insn,
                                        second->insn);
  }

  /* When generating exception handling information, do not put a call in the
     same packet as a prologue insn. */
  if(flag_exceptions && HEXAGON_CALL_P (second) && hexagon_prologue_insn_p(first)){
    dependencies = hexagon_add_dependence(dependencies,
                                        HEXAGON_DEP_CONTROL,
                                        first->insn,
                                        second->insn);
  }

  return dependencies;
}




static struct hexagon_dependence *
hexagon_true_dependencies(
  struct hexagon_insn_info *writer,
  struct hexagon_insn_info *reader
)
{
  struct hexagon_dependence *dependencies;
  struct hexagon_mem_access *store;
  struct hexagon_mem_access *load;
  struct hexagon_reg_access *write;
  struct hexagon_reg_access *read;

  dependencies = NULL;

  if(HEXAGON_EMULATION_CALL_P (writer)){
    /* For an emulation call followed by another insn, consider anti
       dependencies to be true dependencies since emulation calls do not read
       their inputs until after all results in the current packet have been
       written. */
    dependencies = hexagon_concat_dependencies(dependencies,
                                             hexagon_anti_dependencies(writer,
                                                                     reader));
  }
  else if(HEXAGON_EMULATION_CALL_P (reader)){
    /* For an emulation call following another insn, ignore true dependencies
       since emulation calls do not read their inputs until after all results in
       the current packet have been written. */
    return dependencies;
  }

  if (!TARGET_V4_FEATURES){
    /* If a memory access is volatile, then it should be the only memory access in
       a packet.  (not really a dependency, but oh well) */
    if((HEXAGON_VOLATILE_P (writer) || HEXAGON_VOLATILE_P (reader))
       && (HEXAGON_MEM_P (writer) && HEXAGON_MEM_P (reader))){
      dependencies = hexagon_add_dependence(dependencies,
                                          HEXAGON_DEP_VOLATILE,
                                          writer->insn,
                                          reader->insn);
    }
  }

  /* Check for possible true memory dependencies. */
  for(store = writer->stores; store; store = store->next){
    for(load = reader->loads; load; load = load->next){
      if((HEXAGON_CONFLICT_P (store, load)
          || (HEXAGON_NEW_PREDICATE_P (reader)
              && !HEXAGON_NEW_PREDICATE_P (writer)))
         && true_dependence(store->mem, VOIDmode, load->mem, rtx_varies_p)){
        dependencies = hexagon_add_dependence(dependencies,
                                            HEXAGON_DEP_MEMORY,
                                            store->mem,
                                            load->mem);
      }
    }
  }

  /* Check for true register dependencies. */
  for(write = writer->reg_writes; write; write = write->next){
    for(read = reader->reg_reads; read; read = read->next){
      if((HEXAGON_CONFLICT_P (write, read)
          || (HEXAGON_NEW_PREDICATE_P (reader)
              && !HEXAGON_NEW_PREDICATE_P (writer)))
         && write->regno == read->regno){
        dependencies = hexagon_add_dependence(dependencies,
                                            HEXAGON_DEP_REGISTER,
                                            write->reg,
                                            read->reg);
      }
    }
  }

  return dependencies;
}




static struct hexagon_dependence *
hexagon_output_dependencies(
  struct hexagon_insn_info *insn0,
  struct hexagon_insn_info *insn1
)
{
  struct hexagon_dependence *dependencies = NULL;
  struct hexagon_reg_access *write0;
  struct hexagon_reg_access *write1;

  if(HEXAGON_EMULATION_CALL_P (insn1)){
    /* For an emulation call following another insn, ignore output dependencies
       since emulation calls do not write their outputs until after all results
       in the current packet have been written.  The exception is LR, since it
       is written by the call insn itself. */
    for(write0 = insn0->reg_writes; write0; write0 = write0->next){
      if(write0->regno == LINK_REGNUM){
        write1 = insn1->reg_writes;
        while(write1 && write1->regno != LINK_REGNUM){
          write1 = write1->next;
        }
        gcc_assert(write1);
        dependencies = hexagon_add_dependence(dependencies,
                                            HEXAGON_DEP_REGISTER,
                                            write0->reg,
                                            write1->reg);
      }
    }
    return dependencies;
  }

  /* Check for register output dependencies. */
  for(write0 = insn0->reg_writes; write0; write0 = write0->next){
    for(write1 = insn1->reg_writes; write1; write1 = write1->next){
      if((HEXAGON_CONFLICT_P (write0, write1)
          || (HEXAGON_NEW_PREDICATE_P (insn1)
              && !HEXAGON_NEW_PREDICATE_P (insn0)))
         && write0->regno == write1->regno){
        dependencies = hexagon_add_dependence(dependencies,
                                            HEXAGON_DEP_REGISTER,
                                            write0->reg,
                                            write1->reg);
      }
    }
  }

  return dependencies;
}




static struct hexagon_dependence *
hexagon_anti_dependencies(
  struct hexagon_insn_info *earlier,
  struct hexagon_insn_info *later
)
{
  struct hexagon_dependence *dependencies = NULL;
  struct hexagon_mem_access *mem;
  struct hexagon_mem_access *store;
  struct hexagon_reg_access *read;
  struct hexagon_reg_access *write;

  /* Don't allow insns to move across control insns and vice versa. */
  if(HEXAGON_CONTROL_P (earlier) ^ HEXAGON_CONTROL_P (later)){
    dependencies = hexagon_add_dependence(dependencies,
                                        HEXAGON_DEP_CONTROL,
                                        earlier->insn,
                                        later->insn);
  }

  /* Check for possible memory anti dependencies. */
  for(store = later->stores; store; store = store->next){
    /* load followed by a store */
    for(mem = earlier->loads; mem; mem = mem->next){
      if(HEXAGON_CONFLICT_P (mem, store)
         && anti_dependence(mem->mem, store->mem)){
        dependencies = hexagon_add_dependence(dependencies,
                                            HEXAGON_DEP_MEMORY,
                                            mem->mem,
                                            store->mem);
      }
    }
    /* store followed by a store */
    for(mem = earlier->stores; mem; mem = mem->next){
      if(HEXAGON_CONFLICT_P (mem, store)
         && output_dependence(mem->mem, store->mem)){
        dependencies = hexagon_add_dependence(dependencies,
                                            HEXAGON_DEP_MEMORY,
                                            mem->mem,
                                            store->mem);
      }
    }
  }

  /* Check for register anti dependencies. */
  for(read = earlier->reg_reads; read; read = read->next){
    for(write = later->reg_writes; write; write = write->next){
      if(HEXAGON_CONFLICT_P (read, write) && read->regno == write->regno){
        dependencies = hexagon_add_dependence(dependencies,
                                            HEXAGON_DEP_REGISTER,
                                            read->reg,
                                            write->reg);
      }
    }
  }

  return dependencies;
}




static bool
hexagon_packet_insn_dependence_p(
  struct hexagon_packet_info *packet,
  struct hexagon_insn_info **insn_info,
  bool ignore_jumps
)
{
  struct hexagon_insn_info *original_insn_info = *insn_info;
  struct hexagon_dependence *dep;
  int i;

  /* Check each insn in the packet in reverse order for dependencies. */
  for(i = packet->num_insns - 1; i >= 0; i--){

    /* We don't have any way to eliminate output dependencies. */
    if(hexagon_output_dependencies(packet->insns[i], *insn_info)){
      *insn_info = original_insn_info;
      return true;
    }

    /* Disallow anti-dependencies with jumps since INSN_INFO would be inserted
       before any jumps if it were added to the packet. */
    if(HEXAGON_JUMP_P (packet->insns[i])){
      dependencies = hexagon_anti_dependencies(packet->insns[i], *insn_info);
      /* If IGNORE_JUMPS is true, ignore control dependencies from jumps. */
      if(ignore_jumps){
        for(dep = dependencies; dep; dep = dep->next){
          if(dep->type == HEXAGON_DEP_CONTROL){
            dependencies = hexagon_remove_dependence(dependencies, dep);
          }
        }
      }
      if(dependencies){
        return true;
      }
    }

    dependencies = hexagon_true_dependencies(packet->insns[i], *insn_info);

    /* If IGNORE_JUMPS is true, ignore control dependencies from jumps. */
    if(! (ignore_jumps && HEXAGON_JUMP_P (packet->insns[i]))){
    dependencies =
      hexagon_concat_dependencies(dependencies,
                                hexagon_control_dependencies(packet->insns[i],
                                                           *insn_info));
    }

    for(dep = dependencies; dep; dep = dep->next){

      /* See if the consumer insn can use the newly generated predicate in the
         same packet. */
      if(dep->type == HEXAGON_DEP_REGISTER
         && P_REG_P (dep->use)
         && hexagon_predicate_dot_newable_p(packet->insns[i], *insn_info)){

        if(!HEXAGON_NEW_PREDICATE_P (*insn_info)){
          *insn_info = hexagon_dot_newify_predicate(*insn_info);
          (*insn_info)->transformed_at_packet = packet;
        }
        dependencies = hexagon_remove_dependence(dependencies, dep);
      }

      /* See if the consumer insn can use the newly generated value in the same
         packet. */
      if(dep->type == HEXAGON_DEP_REGISTER
         && G_REG_P (dep->use)
         && hexagon_gpr_dot_newable_p(packet->insns[i], *insn_info, dep)){

        if(!HEXAGON_NEW_GPR_P (*insn_info)){
          *insn_info = hexagon_dot_newify_gpr(*insn_info, dep);
          (*insn_info)->transformed_at_packet = packet;
        }
        dependencies = hexagon_remove_dependence(dependencies, dep);
      }

      if(dep->type == HEXAGON_DEP_MEMORY
         && hexagon_mem_dot_newable_p(packet->insns[i], *insn_info, dep)){

        if(!HEXAGON_NEW_MEM_P (*insn_info)){
          *insn_info = hexagon_dot_newify_mem(*insn_info, dep);
          (*insn_info)->transformed_at_packet = packet;
        }
        dependencies = hexagon_remove_dependence(dependencies, dep);
      }

    }

    /* We have encountered a dependence that we are unable to eliminate. */
    if(dependencies){
      *insn_info = original_insn_info;
      return true;
    }
  }

  /* Transforming the insn might have introduced a control dependence. */
  if(*insn_info != original_insn_info && !ignore_jumps){
    for(i = packet->num_insns - 1; i >= 0; i--){
      if(hexagon_control_dependencies(packet->insns[i], *insn_info)){
        *insn_info = original_insn_info;
        return true;
      }
    }
  }

  return false;
}




static bool
hexagon_packet_insn_internal_dependence_p(
  struct hexagon_packet_info *packet,
  struct hexagon_insn_info *insn_info
)
{
  int num_insns;
  int i;

  /* If INSN_INFO is in PACKET, then only check for dependencies on insns before
     it. */
  for(num_insns = 0; num_insns < packet->num_insns; num_insns++){
    if(insn_info == packet->insns[num_insns]){
      break;
    }
  }

  for(i = num_insns - 1; i >= 0; i--){

    anti_dependencies = hexagon_anti_dependencies(packet->insns[i], insn_info);

    /* We can have truly dependent insns in the same packet becasue of .new. */
    true_dependencies = hexagon_true_dependencies(packet->insns[i], insn_info);
    control_dependencies = hexagon_control_dependencies(packet->insns[i],
                                                           insn_info);

    dependencies = hexagon_concat_dependencies(anti_dependencies,
                                             true_dependencies);

    /* Ignore control dependencies from jumps. */
    if(! HEXAGON_JUMP_P (packet->insns[i])){
      dependencies = hexagon_concat_dependencies(dependencies,
                                               control_dependencies);
    }

    if(dependencies){
      return true;
    }
  }

  return false;
}




static bool
hexagon_insn_fits_in_packet_p(
  struct hexagon_insn_info *insn,
  struct hexagon_packet_info *packet
)
{
  state_t pipeline_state;
  int i;

  pipeline_state = alloca(state_size());
  state_reset(pipeline_state);
  for(i = 0; i < packet->num_insns; i++){
    gcc_assert(state_transition(pipeline_state, packet->insns[i]->insn) < 0);
  }

  return state_transition(pipeline_state, insn->insn) < 0;
}




static bool
hexagon_can_add_insn_to_packet_p(
  struct hexagon_insn_info **insn,
  struct hexagon_packet_info *packet
)
{
  struct hexagon_insn_info *original_insn = *insn;

  if(hexagon_packet_insn_dependence_p(packet, insn, false)){
    gcc_assert(*insn == original_insn);
    return false;
  }
  if(!hexagon_insn_fits_in_packet_p(*insn, packet)){
    *insn = original_insn;
    return false;
  }
  return true;
}




static rtx
hexagon_bb_real_head_insn(basic_block bb)
{
  rtx insn;
  for(insn = BB_HEAD (bb);
      insn && (!INSN_P (insn)
               || DEBUG_INSN_P (insn)
               || GET_CODE (PATTERN (insn)) == USE
               || GET_CODE (PATTERN (insn)) == CLOBBER
               || GET_CODE (PATTERN (insn)) == ADDR_VEC
               || GET_CODE (PATTERN (insn)) == ADDR_DIFF_VEC);
      insn = NEXT_INSN (insn));
  if(!insn || BLOCK_FOR_INSN (insn) != bb){
    return NULL_RTX;
  }
  return insn;
}




static rtx
hexagon_bb_real_end_insn(basic_block bb)
{
  rtx insn;
  for(insn = BB_END (bb);
      insn && (!INSN_P (insn)
               || DEBUG_INSN_P (insn)
               || GET_CODE (PATTERN (insn)) == USE
               || GET_CODE (PATTERN (insn)) == CLOBBER
               || GET_CODE (PATTERN (insn)) == ADDR_VEC
               || GET_CODE (PATTERN (insn)) == ADDR_DIFF_VEC);
      insn = PREV_INSN (insn));
  if(!insn || BLOCK_FOR_INSN (insn) != bb){
    return NULL_RTX;
  }
  return insn;
}

static void
hexagon_create_bb_sentinel_packet(struct hexagon_packet_info *packet)
{
  struct hexagon_packet_info *sentinel_packet;
  struct hexagon_insn_info *sentinel_insn;

  sentinel_packet = ggc_alloc_cleared_hexagon_packet_info ();
  sentinel_insn = ggc_alloc_cleared_hexagon_insn_info ();

  sentinel_insn->insn = PREV_INSN (packet->insns[0]->insn);
  sentinel_packet->insns[0] = sentinel_insn;
  sentinel_packet->num_insns = 1;
  sentinel_packet->prev = packet->prev;
  sentinel_packet->next = packet;

  if(packet->prev){
    packet->prev->next = sentinel_packet;
  }
  else {
    hexagon_head_packet = sentinel_packet;
  }
  packet->prev = sentinel_packet;
}



static void
hexagon_finalize_transformations(
  struct hexagon_packet_info *first,
  struct hexagon_packet_info *last
)
{
  struct hexagon_packet_info *packet;
  struct hexagon_insn_info *new_insn_info;
  struct hexagon_insn_info *old_insn_info;
  rtx prev_insn;
  rtx insn;
  int i;

  for(packet = first; packet && packet->prev != last; packet = packet->next){
    prev_insn = packet->location;
    for(i = 0; i < packet->num_insns; i++){
      new_insn_info = packet->insns[i];

      while(new_insn_info->stack
            && new_insn_info->insn == new_insn_info->stack->insn){
        new_insn_info->stack = new_insn_info->stack->stack;
      }

      if(HEXAGON_MOVED_P (new_insn_info) || new_insn_info->stack){
        old_insn_info = new_insn_info;

        while(old_insn_info->stack){
          old_insn_info = old_insn_info->stack;
        }

        insn = new_insn_info->insn;
        if(JUMP_P (insn)){
          insn = emit_jump_insn_after(insn, prev_insn);
        }
        else if(CALL_P (insn)){
          insn = emit_call_insn_after(insn, prev_insn);
        }
        else {
          insn = emit_insn_after(insn, prev_insn);
        }
        new_insn_info->insn = insn;

        delete_insn(old_insn_info->insn);
      }

      new_insn_info->stack = NULL;
      new_insn_info->transformed_at_packet = NULL;

      prev_insn = new_insn_info->insn;
    }
  }
}




static void
hexagon_pack_insns(bool need_bb_info)
{
  basic_block bb = NULL;
  struct hexagon_packet_info *packet = NULL;
  struct hexagon_insn_info *insn_info;
  rtx insn;
  rtx bb_head_insn = NULL_RTX;
  rtx bb_end_insn  = NULL_RTX;
  bool start_packet = true;
  bool start_bb = false;
  bool end_bb = false;
  int fr, fr2;

  if(need_bb_info){
    bb = ENTRY_BLOCK_PTR->next_bb;
    do
      {
        if (bb)
          {
            bb_head_insn = hexagon_bb_real_head_insn(bb);
            bb_end_insn = hexagon_bb_real_end_insn(bb);
            if(!(bb_head_insn && bb_end_insn))
              bb = bb->next_bb;
          }
        else
          return;
      }
    while (!(bb_head_insn && bb_end_insn));
  }

  for(insn = get_insns(); insn; insn = NEXT_INSN (insn)){

    /* Ignore notes. */
    if (NOTE_P (insn)) continue;
    if (DEBUG_INSN_P (insn)) continue;

    /* Don't bundle insns across other non-insns, like labels. */
    if (!INSN_P (insn))
      {
	start_packet = true;
	continue;
      }

    /* Ignore insns that don't do anything. */
    switch(GET_CODE (PATTERN (insn)))
      {
      case USE:
      case CLOBBER:
      case ADDR_VEC:
      case ADDR_DIFF_VEC:
        continue;
      default:
        break;
      }

    if(need_bb_info){
      if(bb_head_insn == insn){
        start_bb = true;
        start_packet = true;
      }
      if(bb_end_insn == insn){
        end_bb = true;
      }
    }

    insn_info = hexagon_get_insn_info(insn);

    if(start_packet || !hexagon_can_add_insn_to_packet_p(&insn_info, packet)){
      if (dump_file && packet && packet->num_insns) {
	fr = INSN_UID(packet->insns[0]->insn);
	fr2 = INSN_UID(packet->insns[packet->num_insns-1]->insn);
	fprintf (dump_file,
		 ";; made pkt w/%d insns %d-%d.\n",
		 packet->num_insns, fr, fr2);
      }
      packet = hexagon_start_new_packet();
    }

    hexagon_add_insn_to_packet(packet, insn_info, false);

    start_packet = false;

    if(need_bb_info){
      if(start_bb){
        BB_HEAD_PACKET (bb) = packet;
        hexagon_create_bb_sentinel_packet(packet);
      }
      if(end_bb){
        BB_END_PACKET (bb) = packet;
        do {
          bb = bb->next_bb;
          if(!bb){
            bb_head_insn = NULL_RTX;
            bb_end_insn = NULL_RTX;
            break;
          }
          bb_head_insn = hexagon_bb_real_head_insn(bb);
          bb_end_insn = hexagon_bb_real_end_insn(bb);
        }while(!(bb_head_insn && bb_end_insn));
      }
      start_bb = false;
      end_bb = false;
    }

  }
  if (dump_file && packet && packet->num_insns) {
      fr = INSN_UID(packet->insns[0]->insn);
      fr2 = INSN_UID(packet->insns[packet->num_insns-1]->insn);
      fprintf (dump_file,
	       ";; made pkt last w/%d insns %d-%d.\n",
	       packet->num_insns, fr, fr2);
  }

  hexagon_finalize_transformations(hexagon_head_packet, hexagon_tail_packet);
}




static int
check_call_reorder_feasibility(struct hexagon_insn_info *insn_info){

  struct hexagon_reg_access *write;
  struct hexagon_reg_access *read;
  char default_call_used_regs[] = CALL_USED_REGISTERS;

  gcc_assert(insn_info);
  /* If this is a call, I should assume it to alias with any memory access,
              but I might try to reason about calee-save registers */
  if(HEXAGON_MEM_P (insn_info)) return true;
    for(write = insn_info->reg_writes; write; write = write->next){
      if(default_call_used_regs[write->regno] || (write->regno == LINK_REGNUM))
        return true;
    }
    for(read = insn_info->reg_reads; read; read = read->next){
      if(default_call_used_regs[read->regno] || (read->regno == LINK_REGNUM))
        return true;
    }
  return false;
}




/* This function is  used by tail merging
   Looks at the relationship of two instructions
   and tries to establish their independenc.
   Conservative.
   Shaped after hexagon_true_dependencies(from,to); */

int
hexagon_instructions_dependent (rtx from,rtx to){
 struct hexagon_insn_info *insn_info_from = NULL;
 struct hexagon_insn_info *insn_info_to   = NULL;
 struct hexagon_mem_access *store;
 struct hexagon_mem_access *load;
 struct hexagon_reg_access *write;
 struct hexagon_reg_access *read;


  /* I know it is INSN_P for both cases, and it should not be a jump,
          since we are not at the "bottom" of BB, but I will double check
          for generality */
  if(!INSN_P(from) ||
    (GET_CODE (PATTERN (from)) == USE) ||
    (GET_CODE (PATTERN (from)) == CLOBBER))
      return true;
  if(!INSN_P(to) ||
    (GET_CODE (PATTERN (to)) == USE) ||
    (GET_CODE (PATTERN (to)) == CLOBBER))
      return true;

  insn_info_from = hexagon_get_insn_info(from);
  insn_info_to   = hexagon_get_insn_info(to);

  if(HEXAGON_JUMP_P(insn_info_from) || HEXAGON_JUMP_P(insn_info_to))
    /* I really should not have jumps here... */
    return true;

  if(HEXAGON_EMULATION_CALL_P (insn_info_from) ||
     HEXAGON_EMULATION_CALL_P (insn_info_to))
    /* not handling these yet */
    return true;

  /* If this is a call, I should assume it to alias with any memory access,
               but I might try to reason about calee-save registers */
  if(HEXAGON_CALL_P (insn_info_from))
    return check_call_reorder_feasibility(insn_info_to);

  /* If this is a call, I should assume it to alias with any memory access,
               but I might try to reason about calee-save registers */
  if(HEXAGON_CALL_P(insn_info_to))
    return check_call_reorder_feasibility(insn_info_from);

  /* this will cover the case of memory output dependency   */
  if((HEXAGON_VOLATILE_P (insn_info_from) || HEXAGON_VOLATILE_P (insn_info_to))
  && (HEXAGON_MEM_P (insn_info_from) && HEXAGON_MEM_P (insn_info_to)))
    return true;

  /* Another form of output dependency. Check for writes to overlapping
     memory regions. */
  if (GET_CODE (PATTERN (to)) == SET
      && GET_CODE (PATTERN (from)) == SET
      && GET_CODE (XEXP (PATTERN (to), 0)) == MEM
      && GET_CODE (XEXP (PATTERN (from), 0)) == MEM
      && !nonoverlapping_memrefs_p (XEXP (PATTERN (to), 0),
                                    XEXP (PATTERN (from), 0),
                                    false))
      return true;

  /* Check for possible true memory dependencies. */
  for(store = insn_info_from->stores; (store && store->mem); store = store->next){
    for(load = insn_info_to->loads; (load && load->mem); load = load->next){
      if(true_dependence(store->mem, VOIDmode, load->mem, rtx_varies_p))
        return true;
    }
  }

  /* Check for possible anti memory dependencies. */
  for(store = insn_info_to->stores; (store && store->mem); store = store->next){
    for(load = insn_info_from->loads; (load && load->mem); load = load->next){
      if(true_dependence(store->mem, VOIDmode, load->mem, rtx_varies_p))
        return true;
    }
  }

  /* Check for true register dependencies. */
  for(write = insn_info_from->reg_writes; write; write = write->next){
    for(read = insn_info_to->reg_reads; read; read = read->next){
      if(write->regno == read->regno)
        return true;
    }
  }
  /* Check for anti register dependencies. */
  for(write = insn_info_to->reg_writes; write; write = write->next){
    for(read = insn_info_from->reg_reads; read; read = read->next){
      if(write->regno == read->regno)
        return true;
    }
  }
  /* We do not need to check for output dependency since this is a check inside BB, and DCE has been recently
     performed */

  return false;
}




static void
hexagon_move_insn(
  struct hexagon_insn_info *old_insn,
  struct hexagon_packet_info *from_packet,
  struct hexagon_insn_info *new_insn,
  struct hexagon_packet_info *to_packet,
  bool move_to_beginning
)
{
  basic_block bb;
  rtx adjacent_insn;

  gcc_assert(to_packet != from_packet);

  hexagon_remove_insn_from_packet(from_packet, old_insn);
  hexagon_add_insn_to_packet(to_packet, new_insn, move_to_beginning);

  if(old_insn->insn == new_insn->insn){
    if(PREV_INSN (old_insn->insn)){
      NEXT_INSN (PREV_INSN (old_insn->insn)) = NEXT_INSN (old_insn->insn);
    }
    if(NEXT_INSN (old_insn->insn)){
      PREV_INSN (NEXT_INSN (old_insn->insn)) = PREV_INSN (old_insn->insn);
    }
    bb = BLOCK_FOR_INSN (old_insn->insn);
    if(bb && BB_END (bb) == old_insn->insn){
      BB_END (bb) = PREV_INSN (old_insn->insn);
    }
  }
  else {
    delete_insn(old_insn->insn);
  }


  if(to_packet->num_insns > 1){
    if (move_to_beginning){
      adjacent_insn = to_packet->insns[1]->insn;
      if(PREV_INSN (adjacent_insn) != new_insn->insn){
        PREV_INSN(new_insn->insn) = PREV_INSN (adjacent_insn);
        NEXT_INSN (PREV_INSN (adjacent_insn)) = new_insn->insn;
        NEXT_INSN(new_insn->insn) = adjacent_insn;
        PREV_INSN (adjacent_insn) = new_insn->insn;
      }
    }
    else if(to_packet->insns[to_packet->num_insns - 1] == new_insn){
      adjacent_insn = to_packet->insns[to_packet->num_insns - 2]->insn;
      if(NEXT_INSN (adjacent_insn) != new_insn->insn){
        PREV_INSN (new_insn->insn) = adjacent_insn;
        NEXT_INSN (new_insn->insn) = NEXT_INSN (adjacent_insn);
        NEXT_INSN (adjacent_insn) = new_insn->insn;
        PREV_INSN (NEXT_INSN (new_insn->insn)) = new_insn->insn;
        bb = BLOCK_FOR_INSN(adjacent_insn);
        if(bb && BB_END (bb) == adjacent_insn){
          BB_END (bb) = new_insn->insn;
        }
      }
    }
    else {
      gcc_assert(to_packet->insns[to_packet->num_insns - 2] == new_insn);
      adjacent_insn = to_packet->insns[to_packet->num_insns - 1]->insn;
      NEXT_INSN (new_insn->insn) = adjacent_insn;
      PREV_INSN (new_insn->insn) = PREV_INSN (adjacent_insn);
      PREV_INSN (adjacent_insn) = new_insn->insn;
      NEXT_INSN (PREV_INSN (new_insn->insn)) = new_insn->insn;
    }
  }
  else {
    gcc_assert(to_packet->num_insns == 1);
    do {
      to_packet = to_packet->prev;
    }while(to_packet->num_insns == 0);
    adjacent_insn = to_packet->insns[to_packet->num_insns - 1]->insn;
    if(NEXT_INSN (adjacent_insn) != new_insn->insn){
      PREV_INSN (new_insn->insn) = adjacent_insn;
      NEXT_INSN (new_insn->insn) = NEXT_INSN (adjacent_insn);
      NEXT_INSN (adjacent_insn) = new_insn->insn;
      PREV_INSN (NEXT_INSN (new_insn->insn)) = new_insn->insn;
      bb = BLOCK_FOR_INSN(adjacent_insn);
      if(bb && BB_END (bb) == adjacent_insn){
        BB_END (bb) = new_insn->insn;
      }
    }
  }

 /*
  df_insn_rescan_all();
  set_block_for_insn(new_insn->insn, BLOCK_FOR_INSN (adjacent_insn));
*/
  df_insn_change_bb (new_insn->insn, BLOCK_FOR_INSN (adjacent_insn)); /* Support for data flow analysis */
}




static void
hexagon_sanity_check_cfg_packet_info(void)
{
  basic_block bb;
  struct hexagon_packet_info *packet;
  struct hexagon_insn_info *insn;
  int i;

  FOR_EACH_BB (bb){
    packet = BB_HEAD_PACKET (bb);
    while(packet && packet->prev != BB_END_PACKET (bb)){
      for(i = 0; i < packet->num_insns; i++){
        insn = packet->insns[i];
        gcc_assert(insn && insn->insn && BLOCK_FOR_INSN (insn->insn) == bb);
      }
      if(!packet->next){
        gcc_assert(packet == BB_END_PACKET (bb));
      }
      packet = packet->next;
    }
  }

  for(packet = hexagon_head_packet; packet; packet = packet->next){
    gcc_assert(packet->num_insns > 0
               && packet->insns[0]
               && packet->insns[0]->insn);
    bb = BLOCK_FOR_INSN (packet->insns[0]->insn);
    gcc_assert(BB_HEAD_PACKET (bb) && BB_END_PACKET (bb));
    for(i = 0; i < packet->num_insns; i++){
      gcc_assert(packet->insns[i]
                 && packet->insns[i]->insn
                 && BLOCK_FOR_INSN (packet->insns[i]->insn) == bb);
    }
    if(!packet->insns[0]->flags){
      if(packet->prev){
        gcc_assert(packet->prev->num_insns
                   && packet->prev->insns[0]
                   && packet->prev->insns[0]->insn
                   && BLOCK_FOR_INSN (packet->prev->insns[0]->insn)
                   && packet->prev == BB_END_PACKET (BLOCK_FOR_INSN (packet->prev->insns[0]->insn)));
      }
      gcc_assert(packet->next && packet->next == BB_HEAD_PACKET (bb));
    }
    if(packet == BB_HEAD_PACKET (bb)){
      gcc_assert(packet->prev
                 && packet->prev->num_insns == 1
                 && !packet->prev->insns[0]->reg_reads
                 && !packet->prev->insns[0]->reg_writes
                 && !packet->prev->insns[0]->loads
                 && !packet->prev->insns[0]->stores
                 && !packet->prev->insns[0]->flags);
    }
    if(packet== BB_END_PACKET (bb)){
      if(packet->next){
        gcc_assert(packet->next->num_insns == 1
                   && !packet->next->insns[0]->reg_reads
                   && !packet->next->insns[0]->reg_writes
                   && !packet->next->insns[0]->loads
                   && !packet->next->insns[0]->stores
                   && !packet->next->insns[0]->flags);
      }
    }
    if(!packet->next){
      gcc_assert(packet == BB_END_PACKET (bb) && packet == hexagon_tail_packet);
    }
  }
}




#define SORTED_EDGE_LIST_SIZE 1024

static void
hexagon_pull_up_insns(void)
{
  basic_block bb;
  basic_block successor_bb;
  edge e;
  edge_iterator ei;
  HARD_REG_SET prev_live_out;
  struct hexagon_packet_info *top_packet;
  struct hexagon_packet_info *forward_edge_packet;
  struct hexagon_packet_info *back_edge_packet;
  struct hexagon_packet_info *last_packet;
  struct hexagon_packet_info *packet;
  struct hexagon_packet_info *target_packet;
  struct hexagon_insn_info *insn;
  struct hexagon_insn_info *original_insn;
  struct hexagon_insn_info *edge_insn;
  bool start_at_the_next_packet;
  int i;
  int j;
  int num_edges = 0;
  /* This fixed way of book keeping will need to be updated */
  edge sorted_edge_list[SORTED_EDGE_LIST_SIZE];

  hexagon_sanity_check_cfg_packet_info();

#if 0
  verify_flow_info();
#endif /* 0 */

  FOR_EACH_BB (bb){
    if(!(BB_HEAD_PACKET (bb) && BB_END_PACKET (bb))
       || !JUMP_P (BB_END (bb))){
      continue;
    }
    /* Construct a sorted list of edges. */
    num_edges = 0;
    FOR_EACH_EDGE (e, ei, bb->succs){
      edge last_edge = e;
      /* Insert in sorted order. */
      for(i = 0; i < num_edges; i++){
        if(e->probability > sorted_edge_list[i]->probability){
          last_edge = sorted_edge_list[num_edges - 1];
          for(j = num_edges - 1; j > i; j--){
            sorted_edge_list[j] = sorted_edge_list[j - 1];
          }
          sorted_edge_list[i] = e;
          break;
        }
      }
      if(num_edges < SORTED_EDGE_LIST_SIZE){
        sorted_edge_list[num_edges] = last_edge;
        num_edges++;
      }
    }
    for(j = 0; j < num_edges; j++){
      e = sorted_edge_list[j];
      successor_bb = e->dest;
      if(e->flags & EDGE_COMPLEX
         || successor_bb->index == EXIT_BLOCK
         || !(BB_HEAD_PACKET (successor_bb) && BB_END_PACKET (successor_bb))
         || !single_pred_p(successor_bb)){
      /* || e->probability < 0.1 ) ??? Can set a threshold  to prevent low
         probability code from speculation, but need more investigation.
      */
        continue;
      }
      gcc_assert(single_pred(successor_bb) == bb);
      gcc_assert(BB_HEAD_PACKET (bb));
      gcc_assert(BB_END_PACKET (bb));
      gcc_assert(BB_HEAD_PACKET (successor_bb));
      gcc_assert(BB_END_PACKET (successor_bb));
      top_packet = BB_HEAD_PACKET (bb);
      forward_edge_packet = BB_END_PACKET (bb);
      back_edge_packet = BB_HEAD_PACKET (successor_bb);
      last_packet = BB_END_PACKET (successor_bb);
      gcc_assert(top_packet->num_insns > 0);
      gcc_assert(forward_edge_packet->num_insns > 0);
      gcc_assert(back_edge_packet->num_insns > 0);
      gcc_assert(last_packet->num_insns > 0);
      packet = back_edge_packet;
      while(packet && packet->prev != last_packet){
        for(i = 0; i < packet->num_insns; i++){
          COPY_HARD_REG_SET(prev_live_out, BB_LIVE_OUT (bb));
          original_insn = packet->insns[i];
          insn = original_insn;

          /* Don't try to reorder the final control flow insn if present. */
          if(control_flow_insn_p (insn->insn)){
            if(packet->num_insns > 1 || packet == back_edge_packet){
              break;
            }
            target_packet = packet->prev;
            while (target_packet->num_insns == 0 && target_packet != back_edge_packet){
              target_packet = target_packet->prev;
            }
            if(!hexagon_can_add_insn_to_packet_p(&insn, target_packet)){
              target_packet = target_packet->next;
              if(target_packet == packet){
                break;
              }
            }
            hexagon_remove_insn_from_packet(packet, original_insn);
            hexagon_add_insn_to_packet(target_packet, insn, false);
            break;
          }

          /* We can't pull up insns dependent on an insn in the same packet. */
          if(hexagon_packet_insn_internal_dependence_p(packet, insn)){
            continue;
          }

          /* pull up */
	  if (dump_file)
	    fprintf (dump_file, ";; pull up insn %d.\n",
		     INSN_UID(original_insn->insn));

          target_packet = packet;
          start_at_the_next_packet = false;
          while(target_packet != top_packet){
            if(target_packet == back_edge_packet){
              target_packet = forward_edge_packet;
              edge_insn = target_packet->insns[target_packet->num_insns - 1];
              gcc_assert(HEXAGON_JUMP_P (edge_insn));
              if(hexagon_can_speculate_p(insn, bb)){
                /* speculate */
              }
              else if(HEXAGON_CONDITIONAL_P (edge_insn)
                      && !HEXAGON_GPR_CONDITION_P (edge_insn)
                      && (e->flags & EDGE_FALLTHRU
                          || HEXAGON_DIRECT_JUMP_P (edge_insn))
                      && hexagon_predicable_p(insn)){
                /* predicate */
                insn = hexagon_predicate_insn(insn, edge_insn,
                                            e->flags & EDGE_FALLTHRU);
                insn->transformed_at_packet = target_packet;
              }
              else if(!HEXAGON_CONDITIONAL_P (edge_insn)
                      && HEXAGON_DIRECT_JUMP_P (edge_insn)){
                /* We can always move an insn across an uncondtional direct
                   jump. */
              }
              else {
                target_packet = back_edge_packet;
                break;
              }
              hexagon_add_live_out(insn, bb);
            }
            else {
              target_packet = target_packet->prev;
            }
            /* dependent? */
            if(hexagon_packet_insn_dependence_p(target_packet, &insn, true)){
              start_at_the_next_packet = true;
              break;
            }
            /* anti-dependent? */
            if(hexagon_packet_insn_internal_dependence_p(target_packet, insn)){
              break;
            }
          }

          /* trickle down */
          if(start_at_the_next_packet){
            goto try_the_next_packet;
          }
          while(target_packet != packet){
            if(hexagon_insn_fits_in_packet_p(insn, target_packet)){
              break;
            }

            try_the_next_packet:

            while(insn->transformed_at_packet == target_packet){
              insn = insn->stack;
            }
            if(target_packet == forward_edge_packet){
              target_packet = back_edge_packet;
              COPY_HARD_REG_SET( BB_LIVE_OUT(bb), prev_live_out);
            }
            else {
              target_packet = target_packet->next;
            }
          }

          if(target_packet != packet){
            hexagon_move_insn(original_insn, packet, insn, target_packet, false);
            i--;
          }
          insn->stack = NULL;
          insn->transformed_at_packet = NULL;
        }
        packet = packet->next;
      }

      /* remove empty packets */
      packet = back_edge_packet;
      while(packet != last_packet){
        while(packet->next && packet->next->num_insns == 0){
          if(packet->next == BB_END_PACKET (successor_bb)){
            BB_END_PACKET (successor_bb) = packet;
          }
          if(packet->next->next){
            packet->next->next->prev = packet;
          }
          packet->next = packet->next->next;
        }
        if(packet == BB_END_PACKET (successor_bb)){
          break;
        }
        packet = packet->next;
      }
    }
  }


  if(dump_file){
    fputs("\n\n\n\nall basic blocks:\n\n", dump_file);
    FOR_EACH_BB (bb){
      hexagon_print_bb_packets(dump_file, bb);
    }
    fputs("\n\n", dump_file);
    FOR_EACH_BB (bb){
      fputs("BB_HEAD:\n", dump_file);
      print_rtl_single(dump_file, BB_HEAD (bb));
      fputs("BB_END:\n", dump_file);
      print_rtl_single(dump_file, BB_END (bb));
    }
    fputs("\n\n", dump_file);
    print_rtl(dump_file, get_insns());
  }

#if 0
  verify_flow_info();
#endif /* 0 */
  /* CLEANUP_CROSSJUMP will not work any more here,
     but it is OK to try to clean up potential empty BBs
     if any were created by the PULL-UP phase
  */
  cleanup_cfg(CLEANUP_EXPENSIVE);

}




static void
hexagon_init_packing_info(bool need_bb_info)
{
  basic_block bb;
  struct hexagon_bb_aux_info *new_bb_aux;

  dfa_start();

  hexagon_head_packet = NULL;
  hexagon_tail_packet = NULL;

  if(need_bb_info){
    df_set_flags(DF_LR_RUN_DCE);
    df_note_add_problem();
    df_analyze();
    df_set_flags(DF_DEFER_INSN_RESCAN);

    hexagon_head_bb_aux = NULL;
    new_bb_aux = NULL;
    FOR_EACH_BB (bb){
      if(hexagon_head_bb_aux == NULL){
        hexagon_head_bb_aux = ggc_alloc_cleared_hexagon_bb_aux_info ();
        new_bb_aux = hexagon_head_bb_aux;
      }
      else {
        new_bb_aux->next = ggc_alloc_cleared_hexagon_bb_aux_info ();
        new_bb_aux = new_bb_aux->next;
      }
      REG_SET_TO_HARD_REG_SET (new_bb_aux->live_out, df_get_live_out(bb));
      bb->aux = new_bb_aux;
    }
  }
}




static void
hexagon_remove_new_values(void)
{
  struct hexagon_packet_info *packet;
  int i;

  for(packet = hexagon_head_packet; packet; packet = packet->next){
    for(i = 0; i < packet->num_insns; i++){
      if(   HEXAGON_NEW_GPR_P (packet->insns[i])
         || HEXAGON_NEW_MEM_P (packet->insns[i])){
        hexagon_dot_oldify_insn(packet->insns[i]);
      }
    }
  }
}




static void
hexagon_free_packing_info(bool free_bb_info)
{
  basic_block bb;

  if(free_bb_info){
    hexagon_remove_new_values();

    FOR_EACH_BB (bb){
      bb->aux = NULL;
    }
    hexagon_head_bb_aux = NULL;

    df_finish_pass(false);
  }

  hexagon_head_packet = NULL;
  hexagon_tail_packet = NULL;

  dfa_finish();
}


/****************************************/
/*** Functions for New Value Jump *******/
/****************************************/


static bool
hexagon_nvj_new_gpr_p(
  struct hexagon_insn_info *consumer,
  struct hexagon_dependence *dependence
)
{

  rtx compare;
  rtx operand1, operand2;

  /* get compare portion of the nvj,
    from the first 1st elemenet of insn*/
  /* get compare - cmp.XX(Ns.new, YY) */
  compare = hexagon_nvj_get_compare(consumer->insn, 0);

  /*get the operands, Ns.new and YY*/
  operand1 = hexagon_nvj_get_operand(compare, 1);
  operand2 = hexagon_nvj_get_operand(compare, 2);

  if ( GET_CODE(operand1) == UNSPEC &&
      XINT(operand1,1) == UNSPEC_NEW_VALUE &&
      XVECEXP(operand1,0,0) == dependence->use )
  {
    return true;
  }

  if ( GET_CODE(operand2) == UNSPEC &&
       XINT(operand2,1) == UNSPEC_NEW_VALUE &&
       XVECEXP(operand2,0,0) == dependence->use )
  {
    return true;
  }
  return false;
}




/*Given: an instruction,
 *       any two packets (from and to in downward flow)
 *Returns true if insn can be moved from from_packet to the to_packet
 */
static bool
hexagon_move_insn_across_packet_down(
  struct hexagon_packet_info *from,
  struct hexagon_packet_info *to,
  struct hexagon_insn_info  *insn)
{
  while( from != to)
  {
    if (!hexagon_move_insn_down_p(insn, from, from->next))
      return (false);
    from = from->next;
  }
  return (true);
}

static bool
hexagon_move_all_insn_across_packet_down(
  struct hexagon_packet_info *from,
  struct hexagon_packet_info *to)
{
  int i;

  if (from->num_insns == 1)
  {
    /*there is only only one instruction in the packet and it will be moved to the
    candidate packet and this packet will be removed */
    return true;
  }

  /* make sure you can move all the packets*/
  for(i = 0; i < from->num_insns; i++)
  {
    if (!hexagon_move_insn_across_packet_down(from, to, from->insns[i]))
    {
      return false;
    }
  }
  return true;
}

static bool
hexagon_nvj_move_possible(
  struct hexagon_packet_info *candidate_packet,
  struct hexagon_packet_info *prev_packet,
  struct hexagon_insn_info *jump_insn_info,
  struct hexagon_insn_info *comp_insn_info,
  rtx *oper,
  rtx *pred,
  rtx *op1,
  rtx *op2,
  rtx *b_lab,
  struct hexagon_packet_info **src_packet,
  struct hexagon_insn_info **src_insn,
  int *op_cnt
)
{
  rtx feeder_reg, feeder_reg1;
  rtx nvj_instruction, nvj_insn_rtx;
  enum rtx_code compare_code, jump_code;
  bool resource_avail = false;
  bool move_possible = false;
  struct hexagon_reg_access *read;
  *src_insn = NULL;
  *src_packet = NULL;


  for (read=comp_insn_info->reg_reads; read; read=read->next)
  {
    feeder_reg1 = XEXP(SET_SRC(PATTERN(comp_insn_info->insn)),*op_cnt);
    /*for tstbit */
    if (GET_CODE(feeder_reg1) == ZERO_EXTRACT )
    {
      feeder_reg = XEXP(feeder_reg1,*op_cnt);
    }
    else
      feeder_reg = feeder_reg1;


    *src_packet = hexagon_get_source_packet(prev_packet, feeder_reg, comp_insn_info, src_insn);
    if (!*src_packet || GET_MODE(XEXP(PATTERN((*src_insn)->insn),*op_cnt))!=SImode)
    {
      /*reject everything that not SI - pattern won't support it */
      /*src packet is not in the same bb or source is from a combine */
      /*move on to the next operand if its reg or bail out*/
      continue;
    }

    compare_code = GET_CODE(SET_SRC(PATTERN(comp_insn_info->insn)));
    jump_code    = GET_CODE(XEXP(SET_SRC(PATTERN(jump_insn_info->insn)),0));

    /* swap the compare code depeding on the condition of jump and compare*/
    /* tstbit comparison */
    if (GET_CODE(feeder_reg1) == ZERO_EXTRACT )
    {
      /* xor jump and compare code */
      if ( jump_code ^ compare_code)
        compare_code = NE;
      else
        compare_code = EQ;
    }
    else
      /*non tstbit comparison */
    {
      if (jump_code ==  EQ )
      {
        switch(compare_code)
        {
          case EQ:  compare_code = NE; break;
          case NE:  compare_code = EQ; break;
          case GT:  compare_code = LE; break;
          case LE:  compare_code = GT; break;
          case GTU: compare_code = LEU; break;
          case LEU: compare_code = GTU; break;
          default: break;
        }
      }
    }


    *oper  = gen_rtx_fmt_ee(compare_code, VOIDmode, NULL_RTX, NULL_RTX);
    *pred  = SET_DEST(PATTERN(comp_insn_info->insn));
    *op1   = XEXP(SET_SRC(PATTERN(comp_insn_info->insn)),0);
    *op2   = XEXP(SET_SRC(PATTERN(comp_insn_info->insn)),1);
    *b_lab = XEXP(SET_SRC(PATTERN(jump_insn_info->insn)),1);

    if ( (GET_CODE(feeder_reg1) == ZERO_EXTRACT) &&
         !zero_constant(XEXP(*op1, 2), SImode) )
      {
        return false;
      }

    start_sequence();

    if (GET_CODE(feeder_reg1) == ZERO_EXTRACT )
    {
      rtx tstbit_operands = *op1;
      *op1 = XEXP(tstbit_operands, 0);
      *op2 = XEXP(tstbit_operands, 2);
      nvj_instruction =  gen_new_value_jump_tstbit( *oper, *op1, *op2, *b_lab, *pred );
    }
    else
    {
      if (*op_cnt==0)
        nvj_instruction =  gen_new_value_jump1( *oper, *op1, *op2, *b_lab, *pred );
      else
        nvj_instruction =  gen_new_value_jump2( *oper, *op1, *op2, *b_lab, *pred );
    }

    nvj_insn_rtx = emit_jump_insn ( nvj_instruction );
    resource_avail =  hexagon_nvj_check_resources ( *src_insn, candidate_packet, comp_insn_info, jump_insn_info, nvj_insn_rtx);
    end_sequence();

    if (resource_avail)
    {
      hexagon_remove_insn_from_packet(candidate_packet, comp_insn_info);
      move_possible = hexagon_move_insn_across_packet_down(*src_packet, candidate_packet, *src_insn);
      /*Make a decision whether nvj should be created or not*/
      /*Don't create nvj, if the packet can not be removed*/
      if (move_possible && !optimize_size)
        move_possible &= hexagon_move_all_insn_across_packet_down(*src_packet, candidate_packet);
      hexagon_add_insn_to_packet(candidate_packet, comp_insn_info, false);
      if (move_possible)
      {
        return true;
      }
    }

    /* operand1 could not be .newifed, try operand2 - only for cmp.gt and cmp.gtu */
    if ((GET_CODE(feeder_reg1) == ZERO_EXTRACT) ||
         compare_code == NE || compare_code == EQ )
      /* dont' try for tstbit, and cmp.eq */
      break;
    else
      (*op_cnt)++;
  }

  return false;
}

static int
hexagon_get_writes_count(struct hexagon_insn_info *insn_info)
{
  int cnt=0;
  struct hexagon_reg_access *write;
  for (write=insn_info->reg_writes; write; write = write->next) cnt++;
  return (cnt);
}

static int
hexagon_get_reads_count(struct hexagon_insn_info *insn_info)
{
  int cnt=0;
  struct hexagon_reg_access *read;
  for (read=insn_info->reg_reads; read; read = read->next) cnt++;
  return (cnt);
}



/* Now that we have identified the feeder register,
*  iterate over the prev packets, and find it's definition
*  (src_insn) and return the packet which contains the def.
*/
static struct hexagon_packet_info *
hexagon_get_source_packet(
              struct hexagon_packet_info *prev_packet,
              rtx feeder_reg,
              struct hexagon_insn_info *d_insn,
              struct hexagon_insn_info **s_insn)
{
 int i;
 basic_block bb = BLOCK_FOR_INSN(d_insn->insn);
 *s_insn = NULL;

 while( prev_packet &&
     prev_packet->num_insns &&
     BLOCK_FOR_INSN(prev_packet->insns[0]->insn) == bb )
 {
  for( i = 0; i < prev_packet->num_insns; i++ )
  {
    if (!NOTE_P(prev_packet->insns[i]->insn) &&
        /*
        1. don't assume pattern to have xexp
        2. check and reject the following conditions like
            a. post_update
            b. register pair
        */
	XEXP(PATTERN(prev_packet->insns[i]->insn),0) == feeder_reg)
    {
      switch (INSN_CODE(prev_packet->insns[i]->insn))
      {
      case CODE_FOR_fixuns_truncsfsi2:
      case CODE_FOR_fixuns_truncdfsi2:
      case CODE_FOR_fix_truncsfsi2:
      case CODE_FOR_fix_truncdfsi2:
	return NULL;
      default:
	;
      }

      *s_insn = prev_packet->insns[i];
      return prev_packet;
    }
  }
  prev_packet=prev_packet->prev;
 }
 return (NULL);
}

/* NV/J instructions only support some comparisions. By switching
   between strict and non-strict comparisions, we can convert a
   comparision of the form:
      r0 UNSUPPORTED_CMP immediate
   into
      r0 SUPPORTED_CMP (immediate - 1)
   Return true if insn can be used NV/J insns. */
static bool
hexagon_nvj_maybe_convert_comparison (rtx insn)
{
  enum rtx_code code, new_code;
  rtx pattern = PATTERN (insn);
  rtx operand1, operand2;

  if (GET_CODE (pattern) != SET)
    return false;

  code = GET_CODE (XEXP (pattern, 1));
  operand1 = XEXP (SET_SRC (pattern), 0);
  operand2 = XEXP (SET_SRC (pattern), 1);

  /* These comparisions are supported by NV/J insns. */
  if (code == NE  || code == EQ
      || code == LE  || code == GT
      || code == LEU || code == GTU)
    return true;

  switch (code)
    {
    case GE:
      new_code = GT;
      break;
    case LT:
      new_code = LE;
      break;
    case GEU:
      new_code = GTU;
      break;
    case LTU:
      new_code = LEU;
      break;
    default:
      return false;
    }

  if (GET_CODE (operand2) == CONST_INT)
    {
      rtx new_imm = plus_constant (operand2, - 1);

      /* Check for overflow, then do the transformation. */
      if (INTVAL (new_imm) < INTVAL (operand2))
        {
          rtx new_src = gen_rtx_fmt_ee (new_code,
                                        GET_MODE (SET_SRC (pattern)),
                                        operand1, new_imm);
          return validate_change (insn, &SET_SRC (pattern), new_src, false);
        }
    }

  return false;
}

static bool
hexagon_nvj_compare_p(rtx insn)
{
  enum rtx_code code;
  enum machine_mode mode;
  rtx pattern;
  rtx const_int_operand  = NULL;
  rtx operand1;
  rtx operand2;

  if (GET_CODE(PATTERN(insn)) != SET)
  {
    return (false);
  }

  pattern = PATTERN(insn);

  code = GET_CODE(XEXP(pattern,1));
  mode = GET_MODE(XEXP(pattern,1));

  if (mode != BImode)
  {
    return (false);
  }

  /* new value jump supports only the following comparisons */
  if(!(code == NE  || code == EQ  ||
       code == LE  || code == GT  ||
       code == LEU || code == GTU))
  {
    return (false);
  }

  /*now that we know it is a compare instruction, get the operands */
  operand1 = XEXP(SET_SRC(pattern),0);
  operand2 = XEXP(SET_SRC(pattern),1);

  /* for tstbit, only #0 comparison is permitted*/
  if (GET_CODE(operand1) == ZERO_EXTRACT &&
      INTVAL(XEXP(operand1,2)) != 0 )
  {
    return (false);
  }


  /* Make sure compare uses either two registers or
     one register and an CONST_INT */
  if ( (GET_CODE(operand1) == REG) &&
       (!(GET_CODE(operand2) == CONST_INT || GET_CODE(operand2) == REG)))
  {
    return (false);
  }
  if ( (GET_CODE(operand2) == REG) &&
       (!(GET_CODE(operand1) == CONST_INT || GET_CODE(operand1) == REG)))
  {
    return (false);
  }

  /* reject if cmp.XX(r1,r1) case:peren_c++/Sec23/2_1/P23784.C*/
  if ((GET_CODE(operand1) == REG) &&
      (GET_CODE(operand2) == REG) &&
      (REGNO(operand1) == REGNO(operand2)))
  {
    return (false);
  }

  if (GET_CODE(operand1) == REG && GET_CODE(operand2) == CONST_INT)
    const_int_operand = operand2;
  if (GET_CODE(operand2) == REG && GET_CODE(operand1) == CONST_INT)
    const_int_operand = operand1;

  /* if one of operands of the compare is immediate,
   * only allowed values are -1 to U5 ie 32*/
  if (const_int_operand)
  {
    switch(code) {
      case EQ:
      case NE:
      case LE:
      case GT:
        if (INTVAL(const_int_operand) < -1 || INTVAL(const_int_operand) > 31)
          return (false);
        break;
      case GTU:
      case LEU:
        if ((INTVAL(const_int_operand) < 0)  || (INTVAL(const_int_operand) > 31))
          return (false);
        break;
      default:
        break;
    }
  }

  return (true);

}

/*
* Validate that predicate is used only for jump and nothing else
*/
static struct hexagon_insn_info *
hexagon_nvj_query_compare(struct hexagon_packet_info *packet, struct hexagon_insn_info *jump_p_insn_info)
{
  struct hexagon_insn_info *compare_insn = NULL;
  int i, auto_anding_chk = 0;
  /*unsigned int pred_no = (*jump_p_insn_info)->reg_reads->regno; */
  unsigned int pred_no = jump_p_insn_info->reg_reads->regno;
  for( i = 0; i < packet->num_insns; i++ )
  {
    struct hexagon_insn_info *pred_insn = packet->insns[i];
    /*while we are at it, get the compare instruction*/

    /*this avoids cases like { r3 = r2; r2 = r1 } generated by combinesi_v4*/
    /*assumption: there will NOT be parallel RTX's one of which will be use of a predicate*/
    if (GET_CODE(PATTERN(pred_insn->insn)) == PARALLEL)
      continue;

    if ( HEXAGON_NEW_PREDICATE_P(pred_insn) &&
        !HEXAGON_DIRECT_JUMP_P (pred_insn)  &&
        ( hexagon_predicate_use_DImode(pred_insn)      ||
          hexagon_register_defined(pred_insn, pred_no) ||
          hexagon_register_used(pred_insn, pred_no)))
    {
      /*there is another use/def of the predicate in this packet */
      return NULL;
    }

    hexagon_nvj_maybe_convert_comparison (pred_insn->insn);

    if ( hexagon_nvj_compare_p(pred_insn->insn) )
    {
      if ( hexagon_register_defined(pred_insn, pred_no) )
      {
        compare_insn = pred_insn;
        /*auto anding also multiple register definitions*/
        if (++auto_anding_chk > 1)
        {
          return NULL;
        }
      }
    }

  }
  return compare_insn;
}

/*
* Determine if the new value jump can be created (along with other instructions)
* in the packet.
*/
static bool
hexagon_nvj_check_resources (
     struct hexagon_insn_info *q_insn,
     struct hexagon_packet_info *nvj_packet,
     struct hexagon_insn_info *comp_insn,
     struct hexagon_insn_info *jump_insn,
     rtx nvj_insn_rtx )


{
  struct hexagon_packet_info *d_packet;
  struct hexagon_insn_info *src_insn;
  int i=0;

  d_packet = ggc_alloc_cleared_hexagon_packet_info ();

  /*first copy all the non-jump/non-compare instructions*/
  for (i=0; i < nvj_packet->num_insns; i++ )
  {
    if ( (nvj_packet->insns[i] != comp_insn) &&
         (nvj_packet->insns[i] != jump_insn))
    {
      hexagon_add_insn_to_packet(d_packet, nvj_packet->insns[i], false);
    }
  }

  /* see if nvj can be put in the packet */
  src_insn = hexagon_get_insn_info(nvj_insn_rtx);
  if (!hexagon_insn_fits_in_packet_p(src_insn, d_packet))
  {
    return false;
  }

  /*if nvj can be put, check to see if its source (.new)
    can be put in the same packet,
    and at the start of the packet - */
  hexagon_add_insn_to_packet(d_packet, src_insn, false);
  return (hexagon_insn_fits_in_packet_p(q_insn, d_packet));
}


/* Given an instruction, return
*    true if compare uses combine registers
*/
static bool
hexagon_predicate_use_DImode( struct hexagon_insn_info *insn_info)
{
  rtx pattern = PATTERN(insn_info->insn);
  rtx uses = XEXP(pattern,1);
  if ((GET_CODE (XEXP(uses,1)) == REG &&
       GET_MODE (XEXP(uses,1)) == DImode) ||
      (GET_CODE (XEXP(uses,0)) == REG &&
       GET_MODE (XEXP(uses,0)) == DImode)
     )
  {
    return (true);
  }
  return (false);
}

/* reject if the operands of the compare instruction is def' in the packet */
static bool
hexagon_used_operands( struct hexagon_packet_info *packet,
                     struct hexagon_insn_info *jump_insn_info,
                     struct hexagon_insn_info *comp_insn_info)
{
  int i, j, reads_cnt = hexagon_get_reads_count(comp_insn_info);
  for( i = 0; i < packet->num_insns; i++ )
  {
    if ( packet->insns[i] != jump_insn_info &&
         packet->insns[i] != comp_insn_info )
    {
      for (j=0; j<hexagon_get_writes_count(packet->insns[i]); j++)
      {
        unsigned int reg_no = comp_insn_info->reg_reads->regno;
        if (hexagon_register_defined(packet->insns[i], reg_no))
        {
          return (true);
        }

        if (reads_cnt > 1)
        {
          reg_no = (comp_insn_info)->reg_reads->next->regno;
          if (hexagon_register_defined(packet->insns[i], reg_no))
          {
            return (true);
          }
        }
      }
    }
  }
  return (false);
}

/*
* Given an instruction insn_info, return true
*    if it defines/loads the register reg_no
*/
static bool
hexagon_register_defined(struct hexagon_insn_info *insn_info, unsigned int reg_no)
{
  struct hexagon_reg_access *write = insn_info->reg_writes;
  while(write)
  {
    if (write->regno == reg_no)
    {
      return (true);
    }
    write = write->next;
  }
  return (false);
}

/*
* Given an instruction insn_info, return true
*     if it uses the register reg_no
*/
static bool
hexagon_register_used(struct hexagon_insn_info *insn_info, unsigned int reg_no)
{
  struct hexagon_reg_access *read = insn_info->reg_reads;
  while(read)
  {
    if (read->regno == reg_no)
    {
      return (true);
    }
    read = read->next;
  }
  return (false);
}


static bool
hexagon_in_bb_live_out( basic_block bb, struct hexagon_insn_info *q_insn)
{
  struct hexagon_reg_access *write;

  for(write = q_insn->reg_reads; write; write = write->next)
  {
    if(TEST_HARD_REG_BIT (BB_LIVE_OUT (bb), write->regno))
    {
      return (true);
    }
  }
  return (false);
}

/* Given a packet and array of two rtx
 * returns
 *    false if
 *       a. there are no jumps in the packet
 *     b. two dumps (dual jumps not allowed)
 *    true  if there is a single jump in the packet
 *    does not modify the array of rtx if there are(is) no jump
 *    modifies the first  element of rtx with the jmp if it's a jump (single jump packet)
 *    modifies the second element of rtx with the jmp if it's the second jump and can be
            converted into the compound new value jump (dual jump & copacket)
 */
static struct hexagon_insn_info *
hexagon_nvj_query_jump(
  struct hexagon_packet_info *packet,
  bool   *dot_new_predicate,
  basic_block bb)

{
  int i, nvj_count = 0;
  struct hexagon_insn_info *jump_insn;
  for( i = 0; i < packet->num_insns; i++ )
  {
    struct hexagon_insn_info *insn_info = packet->insns[i];
    if ( HEXAGON_DIRECT_JUMP_P(insn_info) &&
         HEXAGON_CONDITIONAL_P(insn_info) &&
         !hexagon_in_bb_live_out(bb, insn_info)
       )
    {

    nvj_count++;
    jump_insn=insn_info;

    /*if dot new predicate*/
    if (HEXAGON_NEW_PREDICATE_P(insn_info))
      *dot_new_predicate = true;
    else
      *dot_new_predicate = false; /*this is just for the breakpoint - don't need it */
    }
  }


  if (nvj_count == 1 ) return (jump_insn);

  /*just making sure, in case of dual jumps - it might have been set to true */
  *dot_new_predicate = false;
  return NULL;
}


static void
hexagon_new_value_jump(void)
{
 /*
 * 1. Scan the packets from the bottom of the basic looking for predicated
 *    jump, - it's either .new or otherwise.
 *
 * 2. a. If pX.new is found, we have a two packet scenario.
 *    a. i.   scan the predecessor of this packet,
 *       ii.  if the predecessor has the producer that feeds the compare (check
 *            both the operands of the compare), you've found the guy,
 *            collapse compare and jump instructions into one
 *            compound new value jump and move the producer/feeder to the compare
 *            instruction to the same packet.
 *       iii. if the predecessor does not have the producer that feeds the
 *            compare, repeat 2.a.ii until top of the basic block is reached.
 *
 * 2. b. If pX is found, we have a three packet scenario. This is little more complicated
 *       and less beneficial -
 *    a. i.  scan the predecessor and the precedessor of the predessor of this packet,
 *       ii. if the predecessor has compare I that feeds the jump, temporalily move
 *           the compare to the packet which contains jump insn ( see if the packet,
 *           which contained the compare could be removed) and continue traversing
 *           up - towards the top of the basic block, until you find the feeder to
 *           the compare. If the feeder is found, try moving the feeder to the
 *           packet which contains the jump. If can be moved, see if the packet which
 *           contained the feeder can be removed. If at least, one of the packets
 *           can be removed, continue otherwise give up.
 *
 * Note: In three packet scenario, I did not any scenario, where I could remove at least
 * one of the packets.
 */

  basic_block bb;

  FOR_EACH_BB(bb)
  {
    struct hexagon_packet_info  *packet, *nvj_packet;
    struct hexagon_insn_info    *compare_insn_info = NULL;
    struct hexagon_insn_info    *jump_p_insn_info = NULL;
    bool                      dot_new_predicate = false;
    rtx                       nvj_insn;
    rtx                       nvj_instruction;

    packet = BB_END_PACKET(bb);

    if (packet && packet->prev &&
           packet->prev->num_insns &&
           BLOCK_FOR_INSN(packet->prev->insns[0]->insn) == bb)
    {
      if ( !(jump_p_insn_info=hexagon_nvj_query_jump(packet, &dot_new_predicate, bb)))
      {
        continue;
      }
      else /* single jumps */
      {
        struct hexagon_insn_info *src_insn;
        struct hexagon_packet_info *src_packet;
        rtx comp_operator, p_reg, operand1, operand2, branch_label;
        int op_cnt = 0;
        struct hexagon_insn_info *new_insn;

        nvj_packet = packet;
        if (dot_new_predicate)  /* predicate is generated in the same packet, two packets */
        {
          if (!(compare_insn_info = hexagon_nvj_query_compare(nvj_packet, jump_p_insn_info)))
          {
          /*predicate is used, other than for jump insn*/
             continue;
          }

          if (hexagon_used_operands(nvj_packet, jump_p_insn_info, compare_insn_info))
          {
            /*one of operands of the compare is defined in the same packet*/
            continue;
          }


          /* get the feeder to the compare and search in the prev packets
           * of the bb
          op_cnt = hexagon_get_reads_count(compare_insn_info);
           */
          if (!hexagon_nvj_move_possible(nvj_packet, packet->prev, jump_p_insn_info,
                                       compare_insn_info, &comp_operator, &p_reg,
                                       &operand1, &operand2, &branch_label, &src_packet,
                                       &src_insn,  &op_cnt))
          {
            continue;
          }

          if ( (GET_CODE(XEXP(SET_SRC(PATTERN(compare_insn_info->insn)),0))
                == ZERO_EXTRACT) &&
              !zero_constant(operand2, SImode) )
          {
              continue;
          }

          hexagon_move_insn(src_insn, src_packet, src_insn, nvj_packet, true);

          /*now call the hexagon genroutine to get the actual instruction */
          /*if the compare is a tstbit, only first operand can be .new  */
          if (GET_CODE(XEXP(SET_SRC(PATTERN(compare_insn_info->insn)),0)) == ZERO_EXTRACT)
          {
            nvj_instruction = gen_new_value_jump_tstbit(comp_operator, operand1, operand2, branch_label, p_reg);
          }
          else
          {
            if (op_cnt==0)
              nvj_instruction = gen_new_value_jump1(comp_operator, operand1, operand2, branch_label, p_reg);
            else
              nvj_instruction = gen_new_value_jump2(comp_operator, operand1, operand2, branch_label, p_reg);

          }
          /*generate a nvj instruction after the jump; later - remove old jump insn */
          nvj_insn = emit_jump_insn_after(nvj_instruction, jump_p_insn_info->insn);

          /*add a jump label*/
          JUMP_LABEL(nvj_insn) = XEXP(branch_label,0);

	  /* preserve branch probability of original jump */
	  {
	    rtx note = find_reg_note (jump_p_insn_info->insn, REG_BR_PROB, 0);
	    if (note)
	      add_reg_note (nvj_insn, REG_BR_PROB, note);
	  }

           /*ensuring that code label can be accessed through label ref */
          XEXP(XEXP(XVECEXP(PATTERN(nvj_insn),0,0),1),1) = branch_label;

          if (!(find_reg_note(nvj_insn, REG_LABEL_OPERAND, branch_label)))
          {
            add_reg_note(nvj_insn, REG_LABEL_OPERAND, branch_label);
          }

          /* assign the block number for the instruction*/
          set_block_for_insn(nvj_insn, BLOCK_FOR_INSN(jump_p_insn_info->insn));

          hexagon_remove_insn_from_packet(nvj_packet, jump_p_insn_info);
          delete_insn(jump_p_insn_info->insn);

          hexagon_remove_insn_from_packet(nvj_packet, compare_insn_info);
          delete_insn(compare_insn_info->insn);

          new_insn = hexagon_get_insn_info(nvj_insn);
          new_insn->flags |= HEXAGON_NEW_GPR;
          hexagon_add_insn_to_packet(nvj_packet, new_insn, false);

          if (src_packet->num_insns == 0 )
          {
            /* remove empty packet */
            hexagon_remove_empty_packet(bb, src_packet);
          }
        }
        else  /* three packets */
        {
          /* In qbench, I did not fidn any three packet cases, where I would reduce
             the number of packets, so I am not implementing them.
             SPEC2000 or SPEC2006 may have opportunities - in spec analysis, if I find
             opportunities, I will implement them
          */
        }
      } /* jump found*/
    }   /* packet is not a candidate */
  }     /* iterated over all the basic blocks in proc*/

}       /* end of hexagon_new_value_jump */


static void
hexagon_remove_empty_packet(
              basic_block bb,
              struct hexagon_packet_info *packet)
{
  /* There is always a sentinal packet */
  if (BB_HEAD_PACKET(bb) == packet)
  {
    BB_HEAD_PACKET(bb) = packet->next;
  }

  /* There is always a sentinal packet */
  if (packet->prev != NULL)
  {
    packet->prev->next = packet->next;
  }
  if (packet->next != NULL)
  {
    packet->next->prev = packet->prev;
  }
  else
  {
    packet->prev = NULL;
  }
}



static void
hexagon_packet_optimizations(void)
{
  if(!TARGET_PACKETS){
    return;
  }

  shorten_branches(get_insns());

  hexagon_init_packing_info(true);
  hexagon_pack_insns(true);

  if (TARGET_V4_FEATURES && TARGET_NEW_VALUE_JUMP)
    hexagon_new_value_jump();

  if (TARGET_PULLUP)
    hexagon_pull_up_insns();

  hexagon_free_packing_info(true);
}

static void
hexagon_final_pack_insns(void)
{
  if(!TARGET_PACKETS){
    return;
  }

  hexagon_init_packing_info(false);
  hexagon_pack_insns(false);

  if(TARGET_V4_FEATURES && TARGET_DUPLEX_SCHEDULING)
    {
      hexagon_pack_duplex_insns();
    }
  if (TARGET_V4_FEATURES)
    {
      hexagon_gen_hintjr ();
    }
  shorten_branches(get_insns());
}

/*--------------------------------------
Functions for hintjr generation.
--------------------------------------*/
static bool
endloop_in_packet (struct hexagon_packet_info *packet)
{
  int i;
  for (i=0; i < packet->num_insns; i++) {
    if (HEXAGON_ENDLOOP_P(packet->insns[i]))
      return true;
  }
  return false;
}
static bool
slot2_occupied_p (struct hexagon_packet_info* packet, bool count_duplexes, rtx reg)
{
  state_t pipeline_state;
  rtx slot2_insn;
  pipeline_state = alloca(state_size());
  state_reset(pipeline_state);

  slot2_insn = make_insn_raw(gen_hintjr(reg));
  INSN_UID(slot2_insn) = get_max_uid() + 1;

  compute_pipeline_state (packet, count_duplexes, &pipeline_state);
  return (state_transition (pipeline_state, slot2_insn) >= 0);
}
static struct hexagon_packet_info*
find_indirect_jump_or_call (struct hexagon_packet_info *current_packet,
                            struct hexagon_insn_info **indirect_jmp_call)
{
  int i;
  (*indirect_jmp_call) = NULL;
  while (current_packet) {
    for (i = 0; i < current_packet->num_insns; ++i) {
      struct hexagon_insn_info *insn_info = current_packet->insns[i];
      if (HEXAGON_INDIRECT_JUMP_P (insn_info) || HEXAGON_INDIRECT_CALL_P (insn_info)) {
        *indirect_jmp_call = insn_info;
        return current_packet;
      }
    }
    current_packet = current_packet->next;
  }
  return NULL;
}

static void
find_packet_for_hintjr (struct hexagon_packet_info* start,
                        struct hexagon_packet_info *end,
                        struct hexagon_insn_info *indirect_jmp_call)
{
  int i;
  rtx reg = NULL_RTX;
  struct hexagon_packet_info *current_packet = end->prev;

  if (INSN_CODE (indirect_jmp_call->insn) == CODE_FOR_cond_jump_reg_indirect) {
    struct hexagon_reg_access *read;
    for(read = indirect_jmp_call->reg_reads; read; read = read->next) {
      if (register_operand (read->reg, SImode)) {
        reg = read->reg;
        break;
      }
    }
  }
  else {
    reg = indirect_jmp_call->reg_reads->reg;
    gcc_assert (register_operand (reg, SImode));
  }
  gcc_assert (reg);
  while (current_packet)
    {
      if (start && current_packet == start->prev)
        return;
      /* Check 1. If producer is in this packet, bail out. */
      /* Iterate over the instructions in the packet and check if there exists a true
         dependence between an instruction in the packet (writer) and the indirect_jmp_call
         in question. */
      for (i = 0; i < current_packet->num_insns; ++i) {
        struct hexagon_insn_info *insn_info = current_packet->insns[i];
        if (hexagon_true_dependencies (insn_info, indirect_jmp_call))
          return;
      }
      /* Check 2. check if the current packet has endloop in it. If so skip over this packet. */
      if (endloop_in_packet (current_packet)) {
        current_packet = current_packet->prev;
        continue;
      }

      /* Check 3. If slot 2 is occupied, move to the previous packet. */
      if (!slot2_occupied_p (current_packet, true, reg)) {
        /* Insert the hintjr */
        struct hexagon_insn_info *last_insn_info = current_packet->insns[current_packet->num_insns - 1];
        struct hexagon_insn_info *hintjr_insn_info;
        rtx hintjr_insn;
        rtx hintjr = gen_hintjr (reg);
        rtx last_insn = last_insn_info->insn;
        hintjr_insn = emit_insn_after (hintjr, last_insn);
        hintjr_insn_info = hexagon_get_insn_info (hintjr_insn);
        hexagon_add_insn_to_packet (current_packet, hintjr_insn_info, false);
        return;
      }
      current_packet = current_packet->prev;
    }
}

static void
hexagon_gen_hintjr (void)
{
  struct hexagon_packet_info *packet;
  struct hexagon_packet_info *targetpacket;
  struct hexagon_insn_info *indirect_jmp_call = NULL;

  packet = hexagon_head_packet;
  targetpacket = NULL;

  while ((targetpacket = find_indirect_jump_or_call (packet,
                                                     &indirect_jmp_call))) {
    /* targetpacket is the packet with the indirect jmp or call.
       indirect_jmp_call is the insn_info of the indirect jump or call.*/
    find_packet_for_hintjr (packet, targetpacket, indirect_jmp_call);
    packet = targetpacket->next;
    indirect_jmp_call = NULL;
  }
}

/*------------------------------
Functions for duplex scheduling
-------------------------------*/

/* The list of sub-instructions for duplex scheduling was obtained from
   the V4 Architecture Specification version 0.94 */

enum duplex_move_direction { UP, DOWN, BOTH };

/* Return true of regno can be used in a duplex insn.

   Note: This will identify instructions that are not sub-instructions
   as sub-instructions.  However, refining this function to be more
   accurate does not result in code size reductions in AMSS.  */
static bool
hexagon_duplex_register_p (int regno)
{
  return (regno == DUPLEX_PRED_REG
          || regno == SP_REGNUM
          || regno == FP_REGNUM
          || regno == LINK_REGNUM
          || (regno >= DUPLEX_REG_LOW_START
              && regno <= DUPLEX_REG_LOW_END)
          || (regno >= DUPLEX_REG_HIGH_START
              && regno <= DUPLEX_REG_HIGH_END));
}

/* Return true of insn is a duplex instruction.  */
static bool
hexagon_duplex_insn_p (struct hexagon_insn_info* insn)
{
  struct hexagon_reg_access *read;
  struct hexagon_reg_access *write;

  if (get_attr_duplex (insn->insn) != DUPLEX_YES)
      return false;

  for (read = insn->reg_reads; read; read = read->next)
    {
      if (!hexagon_duplex_register_p (read->regno))
        return false;
    }

  for (write = insn->reg_writes; write; write = write->next)
    {
      if (!hexagon_duplex_register_p (write->regno))
        return false;
    }

  return true;
}

/* Return true if insn can be move from succ_packet to pred_packet.  */
static bool
hexagon_move_insn_up_p (struct hexagon_insn_info* insn,
                        struct hexagon_packet_info* pred_packet,
                        struct hexagon_packet_info* succ_packet)
{
  int i;
  struct hexagon_dependence *dependencies = NULL;
  struct hexagon_dependence *dep = NULL;
  bool before = TRUE;

  /* Control flow instructions cannot be moved.  */
  if (HEXAGON_CONTROL_P (insn))
    return false;

  /* Check for dependencies in pred_packet.  */
  for (i = 0; i < pred_packet->num_insns; ++i)
    {
      struct hexagon_insn_info *pred_insn = pred_packet->insns[i];

      if (hexagon_control_dependencies(pred_insn, insn))
        return false;

      if (hexagon_true_dependencies(pred_insn, insn))
        return false;

      if (hexagon_output_dependencies(pred_insn, insn))
        return false;
    }

  /* Check for dependencies in succ_packet.  */
  for (i = 0; i < succ_packet->num_insns; ++i)
    {
      struct hexagon_insn_info *succ_insn = succ_packet->insns[i];

      if (succ_insn == insn)
	{
	  before = FALSE;
	  continue;
	}

      dependencies = hexagon_true_dependencies(succ_insn, insn);
      for (dep = dependencies; dep; dep = dep->next)
        {
          if (dep->type == HEXAGON_DEP_REGISTER)
            {
              /* Check for dependence due to p.new.  */
              if (HEXAGON_NEW_PREDICATE_P (insn)
                  && P_REGNO_P (REGNO (dep->use)))
                return false;
            }
        }

      if (hexagon_true_dependencies(insn, succ_insn))
        return false;

      /* Check for a memory output dependance with insns before INSN.  */
      if (before)
	{
	  struct hexagon_mem_access *store0, *store1;

	  /* Check for memory output dependencies. */
	  for(store0 = succ_insn->stores; store0; store0 = store0->next)
	    {
	      for(store1 = insn->stores; store1; store1 = store1->next)
		{
		  if(output_dependence(store0->mem, store1->mem))
		    return false;
		}
	    }
	}
    }

  return true;
}

/* Return true if insn can be moved from pred_packet to succ_packet.  */
static bool
hexagon_move_insn_down_p (struct hexagon_insn_info* insn,
                          struct hexagon_packet_info* pred_packet,
                          struct hexagon_packet_info* succ_packet)
{
  int i;
  struct hexagon_dependence *dependencies = NULL;
  struct hexagon_dependence *dep = NULL;

  /* Control flow insns in non-empty packets cannot be moved.  */
  if (HEXAGON_CONTROL_P (insn)
      && (succ_packet->num_insns > 0))
    return false;

  for (i = 0; i < pred_packet->num_insns; ++i)
    {
      struct hexagon_insn_info *pred_insn = pred_packet->insns[i];

      if (pred_insn == insn)
        continue;

      /* Check for control instructions in pred_packet.  */
      if (HEXAGON_JUMP_P (pred_insn)
          || HEXAGON_CALL_P (pred_insn)
          || HEXAGON_EMULATION_CALL_P (pred_insn))
          return false;

      /* Check for an anti-dependencies.  */
      if (hexagon_anti_dependencies (insn, pred_insn))
        return false;

      /* Check for dot-new consumers in pred_packet.  */
      dependencies = hexagon_true_dependencies (insn, pred_insn);
      for (dep = dependencies; dep; dep = dep->next)
        {
          if (dep->type == HEXAGON_DEP_REGISTER)
            {
              /* Check for dependence due to p.new.  */
              if (HEXAGON_NEW_PREDICATE_P (pred_insn)
                  && P_REGNO_P (REGNO (dep->use)))
                return false;

              /* Check for dependence due to r.new.  */
              if (HEXAGON_NEW_GPR_P (pred_insn))
                {
                  rtx pattern, new_value;
                  pattern = copy_rtx(PATTERN (pred_insn->insn));
                  for_each_rtx(&pattern, hexagon_find_new_value, &new_value);
                  /* new_value is wrapped in an unspec; hence look
                     at XVECEXP(new_value, 0, 0).  */
                  if (XVECEXP(new_value, 0, 0) == dep->use)
                      return false;
                }
            }
        }
    }

  /* If I consumes a dot-new value produced by an instruction in pred_packet
     (TODO: This can be relaxed by dot-oldifying I).  */
  if (HEXAGON_NEW_PREDICATE_P (insn))
    return false;

  /* Check for true and output dependencies.  */
  for (i = 0; i < succ_packet->num_insns; ++i)
    {
      struct hexagon_insn_info *succ_insn = succ_packet->insns[i];
      if (hexagon_true_dependencies (insn, succ_insn)
          || hexagon_output_dependencies (insn, succ_insn))
        return false;
    }

  return true;
}

/*
  Given a predecessor packet and a successor packet and a direction
  to move instructions, check if we can create a duplex by moving
  from one of these packets to another */
static bool
hexagon_pair_duplex_insn(struct hexagon_packet_info* pred_packet,
                       struct hexagon_packet_info* succ_packet,
                       enum duplex_move_direction dir)
{
  int i;

  if (dir == DOWN || dir == BOTH)
    {
      /*
         First, check if we can move a duplex instruction from
         pred_packet to succ_packet:
         If pred_packet contains a duplex instruction that fits in
         the next packet and the dependences allow us to move the instruction
         into the next packet, conduct the move.
      */
      for(i = 0; i < pred_packet->num_insns; ++i)
        {
          struct hexagon_insn_info *insn = pred_packet->insns[i];
          if (hexagon_duplex_insn_p(insn) &&
              hexagon_insn_fits_in_packet_p(insn, succ_packet) &&
              hexagon_move_insn_down_p(insn, pred_packet, succ_packet))
            {
              hexagon_move_insn(insn, pred_packet, insn, succ_packet, true);
              return (true);
            }
        }
    }

  if (dir == UP || dir == BOTH)
    {
      /*
         Then, check if we can move a duplex instruction from
         succ_packet to pred_packet:
         If succ_packet contains a duplex instruction that fits in
         the previous packet and the dependences allow us to move the instruction
         into the previous packet, conduct the move.
      */
      for(i = 0; i < succ_packet->num_insns; ++i)
        {
          struct hexagon_insn_info *insn = succ_packet->insns[i];
          if (hexagon_duplex_insn_p (insn) &&
              hexagon_insn_fits_in_packet_p(insn, pred_packet) &&
              hexagon_move_insn_up_p(insn, pred_packet, succ_packet))
            {
              hexagon_move_insn(insn, succ_packet, insn, pred_packet, false);
              return (true);
            }
        }
    }

  /*
     If none of these succeed, we should attempt a swap between
     a duplex instruction in pred_packet and a non-duplex in
     succ_packet or vice versa (duplex in succ_packet and
     non-duplex in pred_packet)
     It may make sense to memoize all instructions that can
     be moved from one packet to the other before we get
     started
  */
  return (false);
}

static int
hexagon_count_duplex_insns(struct hexagon_packet_info *packet)
{
  int i, count = 0;
  for(i = 0; i < packet->num_insns; i++)
    {
      struct hexagon_insn_info *insn = packet->insns[i];
      if (NOTE_P(insn->insn))
        {
          continue;
        }
      count += (hexagon_duplex_insn_p (insn));
    }
  return(count);
}

static void
compute_pipeline_state (struct hexagon_packet_info *packet, bool count_duplexes,
                             state_t* pipeline_state)
{
  int i;
  for(i = 0; i < packet->num_insns; i++)
    {
      struct hexagon_insn_info *insn = packet->insns[i];
      if (NOTE_P(insn->insn))
        {
          continue;
        }

      if (!count_duplexes && hexagon_duplex_insn_p(insn)) {
        continue;
      }

      gcc_assert(state_transition(*pipeline_state, insn->insn) < 0);
    }
}

/*
  slot0and1_occupied_p(): Given a packet, predicate that returns true if
  slot 0 and 1 are both occupied by instructions in that packet
*/
static bool
slot0and1_occupied_p(struct hexagon_packet_info *packet, bool count_duplexes)
{
  state_t pipeline_state;
  rtx slot0or1_insn;
  pipeline_state = alloca(state_size());
  state_reset(pipeline_state);

  slot0or1_insn = make_insn_raw(gen_deallocframe());
  INSN_UID(slot0or1_insn) = get_max_uid() + 1;

  compute_pipeline_state (packet, count_duplexes, &pipeline_state);
  return (state_transition(pipeline_state, slot0or1_insn) >= 0);
}


/*
  slot0or1_occupied_p(): Given a packet, predicate that returns true if
  either slot 0 or slot 1 is occupied by instructions in that packet
*/
static bool
slot0or1_occupied_p(struct hexagon_packet_info *packet, bool count_duplexes)
{
  state_t pipeline_state;
  rtx slot0or1_insn;
  pipeline_state = alloca(state_size());
  state_reset(pipeline_state);

  slot0or1_insn = make_insn_raw(gen_deallocframe());
  INSN_UID(slot0or1_insn) = get_max_uid() + 1;

  compute_pipeline_state (packet, count_duplexes, &pipeline_state);
  return ((state_transition(pipeline_state, slot0or1_insn) >= 0) ||
          (state_transition(pipeline_state, slot0or1_insn) >= 0));
}


/*
   hexagon_pack_duplex_insns(): Operates after instructions have been packetized.
   Look for duplex opportunities across packets. Move instructions across
   packets to create more duplexes. Currently moves instructions between
   adjacent packets.
*/
static void hexagon_pack_duplex_insns(void)
{
  struct hexagon_packet_info *packet;
  int i;

  packet = hexagon_head_packet;
  while(packet && packet->next)
        {
          struct hexagon_packet_info *this_packet = packet;
          struct hexagon_packet_info *next_packet = packet->next;
          int duplex_this_packet = hexagon_count_duplex_insns(packet);
          int duplex_next_packet = hexagon_count_duplex_insns(packet->next);
          basic_block this_bb, next_bb;
          bool split_packet, open_slot_this_packet, open_slot_next_packet;

          /* Check if neither packet has any duplex instructions */
          if ((this_packet->num_insns == 0) || (next_packet->num_insns == 0))
            {
              packet = packet->next;
              continue;
            }

          /* Only consider moving instructions within the same basic block */
          this_bb = BLOCK_FOR_INSN(this_packet->insns[0]->insn);
          next_bb = BLOCK_FOR_INSN(next_packet->insns[0]->insn);
          if (this_bb != next_bb)
            {
              packet = packet->next;
              continue;
            }

          /*
             Try splitting the packet if:
             The packet contains 4 pairable instructions or
             The packet contains more than 1 pairable instruction and resource
             restrictions prevent the pairing of those duplexes
          */
          split_packet = (duplex_this_packet == 4) ||
            ((duplex_this_packet >= 2) &&
             slot0or1_occupied_p(packet, false /* Don't count duplexes */));

          /* First, try to split the packet */
          if (split_packet)
            {
              /* Create a new packet and choose instructions to move to that
                 packet */
              struct hexagon_packet_info *new_packet =
                ggc_alloc_cleared_hexagon_packet_info ();
              struct hexagon_insn_info *movable_insns[2];
              int num_movable_insns = 0;

              for (i = 0; (i < this_packet->num_insns) &&
                     (num_movable_insns < 2); ++i)
                {
                  if (hexagon_duplex_insn_p (packet->insns[i]) &&
                      hexagon_move_insn_down_p(packet->insns[i], this_packet,
                                             new_packet))
                    {
                      movable_insns[num_movable_insns++] = packet->insns[i];
                    }
                }

              /* Can we move at least two instructions to a new packet */
              if (num_movable_insns >= 2)
                {
                  struct hexagon_insn_info *third_insn = movable_insns[0],
                    *fourth_insn = movable_insns[1];

                  /* Splice new packet in between this_packet and next_packet */
                  this_packet->next = new_packet;
                  new_packet->next = next_packet;
                  new_packet->prev = this_packet;
                  next_packet->prev = new_packet;

                  hexagon_move_insn(third_insn, this_packet, third_insn, new_packet, false);
                  hexagon_move_insn(fourth_insn, this_packet, fourth_insn, new_packet, false);
                  packet = new_packet;
                  continue;
                }
            }


          /* Then, try to move instructions between packets. Check for
             resource restrictions */
          open_slot_this_packet = !slot0and1_occupied_p (this_packet,
                                                              true);
          open_slot_next_packet = !slot0and1_occupied_p (next_packet,
                                                              true);

          /* We try moving instructions from one packet to the next
             if an issue slot is vacant in at least one of the packets */
          if (open_slot_this_packet || open_slot_next_packet)
            {

              /* Look for unpaired instructions in this and the next packet */
              bool unpaired_this_packet = (duplex_this_packet == 1) ||
                (duplex_this_packet > 2);
              bool unpaired_next_packet = (duplex_next_packet == 1) ||
                (duplex_next_packet > 2);

              /* Then check if there are unpaired duplex instructions in this
                 packet and the next packet */
              if (unpaired_this_packet && unpaired_next_packet)
                {
                  /*
                     If there are unpaired instructions in both packets,
                     try to move a duplex instruction between this_packet and
                     next_packet (in both directions) to pair the instructions.
                     If there are extra duplexes in one packet, try to move
                     from that packet to the other
                  */
                  enum duplex_move_direction dir = UP;
                  bool can_move = false;
                  if ((duplex_this_packet == 1) && (duplex_next_packet == 1))
                    {
                      if (open_slot_this_packet && open_slot_next_packet)
                        {
                          dir = BOTH;
                        }
                      else if (open_slot_this_packet)
                        {
                          dir = UP;
                        }
                      else if (open_slot_next_packet)
                        {
                          dir = DOWN;
                        }
                      can_move = true;
                    }
                  else if ((duplex_this_packet > 2) &&
                           (duplex_next_packet == 1) &&
                           open_slot_next_packet)
                    {
                      dir = DOWN;
                      can_move = true;
                    }
                  else if ((duplex_next_packet > 2) &&
                           (duplex_this_packet == 1) &&
                           open_slot_this_packet)
                    {
                      dir = UP;
                      can_move = true;
                    }
                  else if ((duplex_next_packet > 2) &&
                           (duplex_this_packet > 2))
                    {
                      /* Not implemented. We must create a new packet in between
                         this_packet and next_packet and attempt to move instructions
                         to the new packet */
                      can_move = false;
                    }

                  if (can_move)
                    {
                      hexagon_pair_duplex_insn(this_packet, next_packet, dir);
                    }
                }
            }

          if(!packet->next)
            {
              gcc_assert(packet == BB_END_PACKET (this_bb));
            }
          packet = packet->next;
        }

}


/*------------------------------
Functions for fast math
-------------------------------*/
void hexagon_fast_math_libfunc(rtx operand)
{
  gcc_assert (flag_unsafe_math_optimizations);
  if (GET_CODE (XEXP (operand, 0)) == SYMBOL_REF) {
    const char *name = XSTR (XEXP (operand, 0), 0);
    if (!strncmp (name, "sqrt", strlen(name))){
      rtx sqrt_func;
      if (flag_unsafe_math_optimizations)
        sqrt_func = gen_rtx_SYMBOL_REF (Pmode, "__hexagon_fast_sqrt_df");
      else
        gcc_unreachable ();
      XEXP (operand, 0) = sqrt_func;
    }
    /* gcc's builtin fabs is faster than fast_fabs on floats. So, we do not
       emit fast_fabs */
  }
}

/* Initialize the GCC target structure. */

/*---------------------------
Run-time Target Specification
---------------------------*/

#undef TARGET_DEFAULT_TARGET_FLAGS
#define TARGET_DEFAULT_TARGET_FLAGS \
  (MASK_LITERAL_POOL | MASK_LITERAL_POOL_ADDRESSES | MASK_HARDWARE_LOOPS \
   | MASK_NEW_PREDICATES | MASK_NEW_VALUE_STORES | MASK_NEW_MEMORY_LOADS \
   | MASK_BASE_PLUS_INDEX | MASK_MEMOPS | MASK_SECTION_SORTING \
   | MASK_SECTION_SORTING_CODE_SUPPORT | MASK_NEW_VALUE_JUMP \
   | MASK_DEEP_PHI_MATCH | MASK_DEALLOC_RETURN)

#undef TARGET_HANDLE_OPTION
#define TARGET_HANDLE_OPTION hexagon_handle_option

#undef TARGET_OPTION_OVERRIDE
#define TARGET_OPTION_OVERRIDE hexagon_override_options

#undef TARGET_OPTION_OPTIMIZATION_TABLE
#define TARGET_OPTION_OPTIMIZATION_TABLE hexagon_option_optimization_table

#undef TARGET_OPTION_DEFAULT_PARAMS
#define TARGET_OPTION_DEFAULT_PARAMS hexagon_option_default_params

/*------------
Storage Layout
------------*/

#undef TARGET_PROMOTE_FUNCTION_MODE
#define TARGET_PROMOTE_FUNCTION_MODE \
  default_promote_function_mode_always_promote

#undef TARGET_ALIGN_ANON_BITFIELD
#define TARGET_ALIGN_ANON_BITFIELD hexagon_align_anon_bitfield

/*bool TARGET_VECTOR_OPAQUE_P (tree type)*/ /* ??? Should this be true? */

#undef TARGET_VECTORIZE_PREFERRED_SIMD_MODE
#define TARGET_VECTORIZE_PREFERRED_SIMD_MODE \
  hexagon_vectorize_preferred_simd_mode

/*----------------------------------
Layout of Source Language Data Types
----------------------------------*/

#undef TARGET_DEFAULT_SHORT_ENUMS
#define TARGET_DEFAULT_SHORT_ENUMS hexagon_default_short_enums

/*--------------------------------
Basic Characteristics of Registers
--------------------------------*/

#undef TARGET_CONDITIONAL_REGISTER_USAGE
#define TARGET_CONDITIONAL_REGISTER_USAGE hexagon_conditional_register_usage


/*--------------
Register Classes
--------------*/

#undef TARGET_SECONDARY_RELOAD
#define TARGET_SECONDARY_RELOAD hexagon_secondary_reload


/*---------------------------------------
Eliminating Frame Pointer and Arg Pointer
---------------------------------------*/

#undef TARGET_FRAME_POINTER_REQUIRED
#define TARGET_FRAME_POINTER_REQUIRED hexagon_frame_pointer_required

#undef TARGET_CAN_ELIMINATE
#define TARGET_CAN_ELIMINATE hexagon_can_eliminate


/*-------------------------------------
Passing Function Arguments on the Stack
-------------------------------------*/

/* Calls leave the caller's frame alone. */
#undef TARGET_RETURN_POPS_ARGS
#define TARGET_RETURN_POPS_ARGS hook_int_tree_tree_int_0


/*----------------------------
Passing Arguments in Registers
----------------------------*/

#undef TARGET_MUST_PASS_IN_STACK
#define TARGET_MUST_PASS_IN_STACK hexagon_must_pass_in_stack

#undef TARGET_VECTOR_MODE_SUPPORTED_P
#define TARGET_VECTOR_MODE_SUPPORTED_P hexagon_vector_mode_supported_p

#undef TARGET_FUNCTION_ARG_BOUNDARY
#define TARGET_FUNCTION_ARG_BOUNDARY hexagon_function_arg_boundary


/*-------------------------------------
How Scalar Function Values Are Returned
-------------------------------------*/

#undef TARGET_FUNCTION_VALUE
#define TARGET_FUNCTION_VALUE hexagon_function_value


/*---------------------------
How Large Values Are Returned
---------------------------*/

#undef TARGET_RETURN_IN_MEMORY
#define TARGET_RETURN_IN_MEMORY hexagon_return_in_memory


/*---------------------
Function Entry and Exit
---------------------*/

#undef TARGET_ASM_FUNCTION_PROLOGUE
#define TARGET_ASM_FUNCTION_PROLOGUE hexagon_asm_function_prologue


/*-------------------
Permitting tail calls
-------------------*/

#undef  TARGET_FUNCTION_OK_FOR_SIBCALL
#define TARGET_FUNCTION_OK_FOR_SIBCALL hexagon_function_ok_for_sibcall


/*-----------------------------
Implementing the Varargs Macros
-----------------------------*/

#undef TARGET_STRICT_ARGUMENT_NAMING
#define TARGET_STRICT_ARGUMENT_NAMING hook_bool_CUMULATIVE_ARGS_true

/* For Linux, our ABI handles varargs differently */
#ifdef LINUX
#undef  TARGET_SETUP_INCOMING_VARARGS
#define TARGET_SETUP_INCOMING_VARARGS hexagon_setup_incoming_varargs

#undef TARGET_EXPAND_BUILTIN_VA_START
#define TARGET_EXPAND_BUILTIN_VA_START hexagon_va_start

#undef TARGET_GIMPLIFY_VA_ARG_EXPR
#define TARGET_GIMPLIFY_VA_ARG_EXPR hexagon_gimplify_va_arg_expr

#undef TARGET_BUILD_BUILTIN_VA_LIST 
#define TARGET_BUILD_BUILTIN_VA_LIST hexagon_build_builtin_va_list 
#endif

/*--------------------------------
Implicit Calls to Library Routines
--------------------------------*/

#if HEXAGON_DINKUMWARE || HEXAGON_NEWLIB
#undef TARGET_INIT_LIBFUNCS
#define TARGET_INIT_LIBFUNCS hexagon_init_libfuncs
#endif /* HEXAGON_DINKUMWARE || HEXAGON_NEWLIB */


/*--------------
Addressing Modes
--------------*/

#undef TARGET_VECTORIZE_BUILTIN_MASK_FOR_LOAD
#define TARGET_VECTORIZE_BUILTIN_MASK_FOR_LOAD \
  hexagon_vectorize_builtin_mask_for_load

#undef TARGET_LEGITIMATE_ADDRESS_P
#define TARGET_LEGITIMATE_ADDRESS_P hexagon_legitimate_address_p

#undef TARGET_LEGITIMIZE_ADDRESS
#define TARGET_LEGITIMIZE_ADDRESS hexagon_legitimize_address


/*-------------------------------------
Describing Relative Costs of Operations
-------------------------------------*/

#undef TARGET_RTX_COSTS
#define TARGET_RTX_COSTS hexagon_rtx_costs

#undef TARGET_ADDRESS_COST
#define TARGET_ADDRESS_COST hexagon_address_cost


/*---------------------------------
Adjusting the Instruction Scheduler
---------------------------------*/

#undef TARGET_SCHED_ISSUE_RATE
#define TARGET_SCHED_ISSUE_RATE hexagon_sched_issue_rate

#undef TARGET_SCHED_FIRST_CYCLE_MULTIPASS_DFA_LOOKAHEAD
#define TARGET_SCHED_FIRST_CYCLE_MULTIPASS_DFA_LOOKAHEAD \
  hexagon_sched_first_cycle_multipass_dfa_lookahead

#undef TARGET_SCHED_DEPENDENCIES_EVALUATION_HOOK
#define TARGET_SCHED_DEPENDENCIES_EVALUATION_HOOK \
  hexagon_sched_dependencies_eval

/*--------------------------------------------------
Dividing the Output into Sections (Texts, Data, ...)
--------------------------------------------------*/

#undef TARGET_ASM_SELECT_SECTION
#define TARGET_ASM_SELECT_SECTION hexagon_asm_select_section

#undef TARGET_ASM_SELECT_RTX_SECTION
#define TARGET_ASM_SELECT_RTX_SECTION hexagon_asm_select_rtx_section

#undef TARGET_IN_SMALL_DATA_P
#define TARGET_IN_SMALL_DATA_P hexagon_in_small_data_p

#undef TARGET_ASM_UNIQUE_SECTION
#define TARGET_ASM_UNIQUE_SECTION hexagon_asm_unique_section

#undef TARGET_ASM_NAMED_SECTION
#define TARGET_ASM_NAMED_SECTION hexagon_asm_named_section

#undef TARGET_HAVE_NAMED_SECTIONS
#define TARGET_HAVE_NAMED_SECTIONS true

/*------------
Output of Data
------------*/

#undef TARGET_ASM_ALIGNED_HI_OP
#define TARGET_ASM_ALIGNED_HI_OP "\t.hword\t"

#undef TARGET_ASM_ALIGNED_SI_OP
#define TARGET_ASM_ALIGNED_SI_OP "\t.word\t"


#undef TARGET_ASM_OUTPUT_ADDR_CONST_EXTRA
#define TARGET_ASM_OUTPUT_ADDR_CONST_EXTRA hexagon_output_addr_const_extra

/*----------------------
Miscellaneous Parameters
----------------------*/

#undef TARGET_MACHINE_DEPENDENT_REORG
#define TARGET_MACHINE_DEPENDENT_REORG hexagon_machine_dependent_reorg

#undef TARGET_INIT_BUILTINS
#define TARGET_INIT_BUILTINS  hexagon_init_builtins

#undef TARGET_EXPAND_BUILTIN
#define TARGET_EXPAND_BUILTIN hexagon_expand_builtin

/*tree TARGET_RESOLVE_OVERLOADED_BUILTIN (tree fndecl, tree arglist)*/
/*tree TARGET_FOLD_BUILTIN (tree fndecl, tree arglist, bool ignore)*/

#undef TARGET_INVALID_WITHIN_DOLOOP
#define TARGET_INVALID_WITHIN_DOLOOP hexagon_invalid_within_doloop

/* ??? Should this be GENERAL_REGS? */
/*int TARGET_BRANCH_TARGET_REGISTER_CLASS (void)*/

#undef TARGET_CANNOT_COPY_INSN_P
#define TARGET_CANNOT_COPY_INSN_P hexagon_cannot_copy_insn_p

#undef TARGET_CANNOT_FORCE_CONST_MEM
#define TARGET_CANNOT_FORCE_CONST_MEM hexagon_cannot_force_const_mem

#undef TARGET_ASM_EXTERNAL_LIBCALL
#define TARGET_ASM_EXTERNAL_LIBCALL hexagon_asm_external_libcall

/*------------------------------
Trampolines for Nested Functions
------------------------------*/

#undef TARGET_TRAMPOLINE_INIT
#define TARGET_TRAMPOLINE_INIT hexagon_initialize_trampoline

#undef TARGET_ASM_TRAMPOLINE_TEMPLATE
#define TARGET_ASM_TRAMPOLINE_TEMPLATE hexagon_trampoline_template

#undef TARGET_ASM_FILE_END
#define TARGET_ASM_FILE_END file_end_indicate_exec_stack

#undef TARGET_DELAY_SCHED2
#define TARGET_DELAY_SCHED2 true

/* Variable tracking should be run after all optimizations which
   change order of insns.  It also needs a valid CFG.  */
#undef TARGET_DELAY_VARTRACK
#define TARGET_DELAY_VARTRACK true

struct gcc_target targetm = TARGET_INITIALIZER;

/* Should be at the end of the file */
#include "gt-hexagon.h"
