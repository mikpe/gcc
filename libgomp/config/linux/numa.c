/* Copyright (C) 2026 Free Software Foundation, Inc.
   Contributed by Tobias Burnus <tburnus@baylibre.com>

   This file is part of the GNU Offloading and Multi Processing Library
   (libgomp).

   Libgomp is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   Libgomp is distributed in the hope that it will be useful, but WITHOUT ANY
   WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
   FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
   more details.

   Under Section 7 of GPL version 3, you are granted additional
   permissions described in the GCC Runtime Library Exception, version
   3.1, as published by the Free Software Foundation.

   You should have received a copy of the GNU General Public License and
   a copy of the GCC Runtime Library Exception along with this program;
   see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
   <http://www.gnu.org/licenses/>.  */

/* Linux specific version of internally used NUMA information routines.  */

#ifndef _GNU_SOURCE
#define _GNU_SOURCE 1
#endif
#include "libgomp.h"
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/syscall.h>


int
gomp_get_current_numa_node ()
{
  int node;
  syscall (SYS_getcpu, NULL /* cpu */, &node, NULL /* no longer used */);
  return node;
}

int
gomp_get_numa_distance (int node1, int node2)
{
  /* num_numa_nodes: 0 = uninit, -1 = being initialized, -2 = error,
		     > 0 = the number of numa nodes.
     numa_distances: a 2D array of #nodes * #nodes with the distances.
     remap: map the numa number to the index in numa_distances;
	    as numa numbers might not be contiguous.
     num_remap: size of the remap array.  */
  static int num_numa_nodes = 0;
  static int *numa_distances = NULL;
  static unsigned num_remap = 0;
  static unsigned *remap = NULL;

retry:
  int num = __atomic_load_n (&num_numa_nodes, MEMMODEL_RELAXED);
  if (node1 < 0 || node2 < 0 || num <= -2)
    return -1;
  if (num > 0)
    {
      if (node1 > num_remap || node2 > num_remap)
	return -1;
      node1 = remap[node1];
      node2 = remap[node2];
      if (node1 >= num || node2 >= num)
	return -1;
      return numa_distances[node1 * num_numa_nodes + node2];
    }

  /* Obtain distance data - or busy wait until another thread did so.  */
  int val = 0;
  if (!__atomic_compare_exchange_n (&num_numa_nodes, &val, -1, false,
				    MEMMODEL_ACQUIRE, MEMMODEL_ACQUIRE))
    {
      while (-1 == __atomic_load_n (&num_numa_nodes, MEMMODEL_ACQUIRE));
      __asm volatile ("" : : : "memory");
      goto retry;
    }

  /* Obtain numa nodes.  */

  num = 0;
  char *lineptr = NULL;
  size_t nline = 0;
  FILE *f = fopen ("/sys/devices/system/node/online", "r");
  if (f == NULL)
    goto fail;
  if (getline (&lineptr, &nline, f) <= 0)
    {
      fclose (f);
      goto fail;
    }
  fclose (f);

  /* Create mapping array between the node number and contiguous
     n-th online node count. */
  unsigned constexpr UNSET_REMAP = (unsigned) -1;
  num_remap = 8;
  remap = malloc (sizeof (*remap) * num_remap);
  if (!remap)
    goto fail;
  for (int i = 0; i < num_remap; i++)
    remap[i] = UNSET_REMAP;
  char *q = lineptr;
  while (*q && *q != '\n')
    {
      unsigned nfirst, nlast;
      char *end;

      errno = 0;
      nfirst = strtoul (q, &end, 10);
      if (errno || end == q)
	goto fail;
      q = end;
      nlast = nfirst;
      if (*q == '-')
	{
	  errno = 0;
	  nlast = strtoul (q + 1, &end, 10);
	  if (errno || end == q + 1 || nlast < nfirst)
	    goto fail;
	  q = end;
	}
      if (num_remap <= nlast)
	{
	  int n = nlast > 2 * num_remap ? nlast + 1 : num_remap * 2;
	  remap = realloc (remap, sizeof (*remap) * n);
	  if (!remap || n <= nlast)
	    goto fail;
	  for (int i = num_remap; i < n; i++)
	    remap[i] = UNSET_REMAP;
	  num_remap = n;
	}
      for (; nfirst <= nlast; nfirst++)
	remap[nfirst] = num++;
      if (*q == ',')
	++q;
    }
  free (lineptr);
  lineptr = NULL;

  /* Fill distance data.  */
  numa_distances = (int *) gomp_malloc (sizeof (*numa_distances) * num * num);

  for (int i = 0; i < num_remap; i++)
    if (remap[i] == UNSET_REMAP)
      continue;
    else
      {
	constexpr int len = sizeof ("/sys/devices/system/node/node12345/"
				    "distance");
	char filename[len];
	int node = remap[i];
	if (node < 0)
	  goto fail;
	if (len < snprintf (filename, sizeof (filename),
			    "/sys/devices/system/node/node%d/distance", i))
	  goto fail;
	FILE *in = fopen (filename, "r");
	if (!in)
	  goto fail;
	for (int j = 0; j < num; j++)
	  {
	    /* Distance -1 might happen on virtualized systems or when the
	       kernel reports "No NUMA configuration found".  The code assumes
	       that only distance values show up for online nodes.  */
	    int distance;
	    if (fscanf (in, "%d", &distance) != 1)
	      {
		fclose (in);
		goto fail;
	      }
	    numa_distances[node * num + j] = distance;
	  }
	fclose (in);
      }
  __asm volatile ("" : : : "memory");
  __atomic_store_n (&num_numa_nodes, num, MEMMODEL_RELAXED);
  goto retry;

fail:
  free (lineptr);
  free (remap);
  free (numa_distances);
  __atomic_store_n (&num_numa_nodes, -2, MEMMODEL_RELAXED);
  return -1;
}
