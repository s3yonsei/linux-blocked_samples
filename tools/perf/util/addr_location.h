/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __PERF_ADDR_LOCATION
#define __PERF_ADDR_LOCATION 1

#include <linux/perf_event.h>
#include <linux/types.h>

struct thread;
struct maps;
struct map;
struct symbol;

struct addr_location {
	struct thread *thread;
	struct map    *map;
	struct symbol *sym;
	const char    *srcline;
	u64	      addr;
	char	      level;
	u8	      cpumode;
	/*
	 * Off-CPU subclass copied from struct perf_sample by callers that
	 * want hist accounting to keep off-CPU samples separate from
	 * on-CPU samples that share the same comm/dso/symbol/callchain.
	 * Zero on samples that have no off-CPU information.
	 */
	u8	      offcpu_subclass;
	u16	      filtered;
	s32	      cpu;
	s32	      socket;
	/* Same as machine.parallelism but within [1, nr_cpus]. */
	int	      parallelism;
	/* See he_stat.latency. */
	u64	      latency;
};

void addr_location__init(struct addr_location *al);
void addr_location__exit(struct addr_location *al);

void addr_location__copy(struct addr_location *dst, struct addr_location *src);

/*
 * Map a task-clock-plus off-CPU subclass to the level character used by
 * "perf report" (and friends). The convention follows ps(1)'s task-state
 * column, with IOWAIT promoted to its own letter:
 *
 *   PREEMPT         -> 'R'  (runnable, preempted while on CPU)
 *   IOWAIT          -> 'I'
 *   INTERRUPTIBLE   -> 'S'
 *   UNINTERRUPTIBLE -> 'D'
 *
 * Returns 0 when @subclass is zero, NONE, or unrecognized, so callers can
 * fall back to the cpumode-derived level ('k', '.', etc.).
 */
static inline char addr_location__offcpu_level(u8 subclass)
{
	switch (subclass) {
	case PERF_RECORD_MISC_OFFCPU_PREEMPT:
		return 'R';
	case PERF_RECORD_MISC_OFFCPU_IOWAIT:
		return 'I';
	case PERF_RECORD_MISC_OFFCPU_INTERRUPTIBLE:
		return 'S';
	case PERF_RECORD_MISC_OFFCPU_UNINTERRUPTIBLE:
		return 'D';
	default:
		return 0;
	}
}

#endif /* __PERF_ADDR_LOCATION */
