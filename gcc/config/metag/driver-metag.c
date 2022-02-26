/* Subroutines for the gcc driver.
   Copyright (C) 2008 Imagination Technologies Ltd

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3, or (at your option)
any later version.

GCC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING3.  If not see
<http://www.gnu.org/licenses/>.  */

#include <string.h>
#include <stdlib.h>
#include "libiberty.h"
#include "filenames.h"

const char *metag_reduce_options (int argc, const char **argv);
const char *metag_emb_asm_preprocessor (int argc, const char **argv);
const char *metag_emb_onlylast (int argc, const char **argv);
const char *metag_emb_change_suffix (int argc, const char **argv);

/* This function will reduce all -mmetac options to remove all but
   the last one with the %<S construct. It will do the same for
   hard-float taking in to account that soft-float is the inverse 
   and also dealing with simd-float */

enum fpu_state
{
  FPU_SOFT_FLOAT,
  FPU_HARD_FLOAT,
  FPU_HARD_FLOAT_DOUBLE,
  FPU_HARD_FLOAT_SINGLE
};

const char *
metag_reduce_options (int argc, const char **argv)
{
  /* Default FPU mode is soft float */
  enum fpu_state fpu_state = FPU_SOFT_FLOAT;

  /* Default META core is 2.1 (though this will always be overriden so it does not matter) */
  int metac_state = 21;

  /* SIMD FPU is not enabled by default */
  int simd_float = 0;

  /* 'i' is always useful */
  int i;

  /* 140 is the total length of all possible options that will be emitted with a null
     terminator at the end */
  char* buf = (char*)xmalloc(159);

  for (i = 0 ; i < argc ; i++)
    {
      if (strcmp (argv[i], "mhard-float=D") == 0)
        {
          fpu_state = FPU_HARD_FLOAT_DOUBLE;
          simd_float = 0;
        }
      else if (strcmp (argv[i], "mhard-float=S") == 0)
        {
          fpu_state = FPU_HARD_FLOAT_SINGLE;
          simd_float = 0;
        }
      else if (strcmp (argv[i], "mhard-float") == 0)
        {
          fpu_state = FPU_HARD_FLOAT;
          simd_float = 0;
        }
      else if (strcmp (argv[i], "msoft-float") == 0 || strcmp (argv[i], "mhard-float=none") == 0)
        {
          fpu_state = FPU_SOFT_FLOAT;
          simd_float = 0;
        }
      else if (strcmp (argv[i], "msimd-float") == 0)
        simd_float = 1;
      else if (strncmp (argv[i], "mmetac=", 7) == 0)
        if (strlen (argv[i]) == 10)
          {
            metac_state = (argv[i][7] - '0') * 10;
            metac_state += (argv[i][9] - '0');
          }
    }

  /* Point to the start of the buffer */
  i = 0;

  /* Strip various duplicated/overridden options */
  strncpy (&buf[i], "%<mhard-float=none ", 19);
  i += 19;

  if (simd_float == 0)
    {
      strncpy (&buf[i], "%<msimd-float ", 14);
      i += 14;
    }

  if (fpu_state != FPU_SOFT_FLOAT)
    {
      strncpy (&buf[i], "%<msoft-float ", 14);
      i += 14;
    }

  if (fpu_state != FPU_HARD_FLOAT)
    {
      strncpy (&buf[i], "%<mhard-float ", 14);
      i += 14;
    }

  if (fpu_state != FPU_HARD_FLOAT_DOUBLE)
    {
      strncpy (&buf[i], "%<mhard-float=D ", 16);
      i += 16;
    }

  if (fpu_state != FPU_HARD_FLOAT_SINGLE)
    {
      strncpy (&buf[i], "%<mhard-float=S ", 16);
      i += 16;
    }

  if (metac_state != 1)
    {
      strncpy (&buf[i], "%<mmetac=0.1 ", 13);
      i += 13;
    }

  if (metac_state != 10)
    {
      strncpy (&buf[i], "%<mmetac=1.0 ", 13);
      i += 13;
    }

  if (metac_state != 11)
    {
      strncpy (&buf[i], "%<mmetac=1.1 ", 13);
      i += 13;
    }

  if (metac_state != 12)
    {
      strncpy (&buf[i], "%<mmetac=1.2 ", 13);
      i += 13;
    }

  if (metac_state != 21)
    {
      strncpy (&buf[i], "%<mmetac=2.1 ", 13);
      i += 13;
    }

  buf[i] = 0;

  return buf;
}

/* This will be called by the spec parser in gcc.c when it sees
   a %:meta_preprocessor(args) construct.

   It returns a string containing new command line parameters to be
   passed to the assembler. This is for transforming -Dwhatever
   into --defwhatever. The spec will do the translation of -D
   to --def but this code removes all definitions that have
   assignments as the embedded assembler cannot cope with these

   ARGC and ARGV are set depending on the actual arguments given
   in the spec.  */
const char *
metag_emb_asm_preprocessor (int argc, const char **argv)
{
  char * cmd_args = NULL;
  char * current_pos = NULL;
  int args_size = 0;
  int i;
  
  for (i = 0 ; i < argc ; i++)
    {
      if (strchr (argv[i], '=') == NULL)
        args_size += strlen (argv[i]) + 1;
    }

  if (args_size == 0)
    return NULL;

  cmd_args = (char *)malloc (args_size);
  current_pos = cmd_args;

  for (i = 0 ; i < argc ; i++)
    {
      if (strchr (argv[i], '=') == NULL)
        {
          int length = strlen (argv[i]);

          strcpy (current_pos, argv[i]);
          *(current_pos+length) = ' ';
          current_pos += length + 1;
        }
    }
  *(current_pos-1) = 0;

  return cmd_args; 
}

const char *
metag_emb_onlylast (int argc, const char **argv)
{
  if (argc != 0)
    return argv[argc-1];
  else
    return NULL;
}

const char *
metag_emb_change_suffix (int argc, const char **argv)
{
  const char * old_filename = NULL;
  char *       new_filename = NULL;
  unsigned int old_length = 0;
  unsigned int new_length = 0;
  const char * suffix = NULL;
  int new_suffix_length = 0;
  int dot_pos = 0;
  int has_dot = 0;

  if (argc < 2)
    {
      fprintf (stderr, "Not enough arguments given to the meta_change_suffix function!\n");
      exit (1);
    }
  else
    {
      suffix = argv[0];
      /* If multiple -o switches are used on the command line the last one is used */
      old_filename = argv[argc - 1];
      old_length = strlen (old_filename);
      new_suffix_length = strlen (suffix);

      /* Find the location of the . in the filename */
      dot_pos = old_length;
      while (dot_pos-- && !IS_DIR_SEPARATOR (old_filename[dot_pos]))
        {
          if (old_filename[dot_pos] == '.')
            {
              has_dot = 1;
              break;
            }
        }
    
      /* Deal with the case where there is no dot in the filename */
      if (!has_dot)
        dot_pos = old_length;
    
      /* Compute the length of the string to hold the filename with the new suffix. */
      new_length = dot_pos + new_suffix_length + 1;

      /* Create a new string to hold the filename, and initialise 
         it with the old filename excluding the dot and suffix (if applicable) */
      new_filename = (char *)malloc (new_length + 1);
      strncpy (new_filename, old_filename, dot_pos + 1);

      /* Add the dot and new suffix to the filename */
      new_filename[dot_pos] = '.';
      strcpy (&new_filename[dot_pos + 1], suffix);

      return new_filename;
  }
}
