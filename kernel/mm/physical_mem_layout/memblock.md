memblock -- a simple memory management module

Before the whole system is up and buddy system be functional, we need another
module to manage the physical memory -- memblock.

This is a simple memory management module.

# History

From the search engine, the memblock is introduced by Yinghai in 2010. You
could take a look at this mail [thread][1] if you are interested.

By diving into the source code, the first memblock patch is 

[95f72d1ed41a66f1c1c29c24d479de81a0bea36f][2]

Well, it originally named "lmb", this patch just changs the name.

I don't know more details, if you see some interesting thing, please let me
know~

# The generic algorithm behind it

memblock maintains two regions: memblock.memory and memblock.reserved.

All available physical memory will be added to the memblock.memory region.
And each allocated memory will be added to the memblock.reserved region.

Attention: allocated memory will not be removed from memblock.memory region.

Let me borrow a chart and change it a little. The original chart is from
[here][3], which is one of my favorite series.

    +---------------------------+   +---------------------------+
    |         memblock          |   |       Array of the        |
    |  _______________________  |   |      memblock_region      |
    | |        memory         | |   |                           |
    | |      memblock_type    |-|-->| [start1, end1)            |
    | |_______________________| |   |                           |
    |                           |   | [start2, end2)            |
    |                           |   |                           |
    |                           |   | [start3, end3)            |
    |                           |   |                           |
    |                           |   +---------------------------+
    |                           |                                
    |  _______________________  |   +---------------------------+
    | |       reserved        | |   |       Array of the        |
    | |      memblock_type    |-|-->|      memblock_region      |
    | |_______________________| |   |                           |
    |                           |   |  [start1, end1)           |
    |                           |   |                           |
    |                           |   |  [start2, end2)           |
    |                           |   |                           |
    +---------------------------+   +---------------------------+

For example, current status of the memblock is:
1. There are three memory regions in the system, [start1/2/3, end1/2/3)
2. And two of them are allocated, [start1/2, end1/2)

In case you want to release [start2, end2), no changes to memblock.memory,
just remove [start2, end2) from memblock.reserved.

Ok, hope this may help you to get a whole picture about memblock :-)

# Where is the original memory information for the memblock.memory?

As we learnt previously, e820 retrieves physical memory information from BIOS,
so memblock information is set from e820 map. 

On x86 arch, this is done by function memblock_x86_fill().

```c
void __init memblock_x86_fill(void)
{
	int i;
	u64 end;

	/*
	 * EFI may have more than 128 entries
	 * We are safe to enable resizing, beause memblock_x86_fill()
	 * is rather later for x86
	 */
	memblock_allow_resize();

	for (i = 0; i < e820.nr_map; i++) {
		struct e820entry *ei = &e820.map[i];

		end = ei->addr + ei->size;
		if (end != (resource_size_t)end)
			continue;

		if (ei->type != E820_RAM && ei->type != E820_RESERVED_KERN)
			continue;

		memblock_add(ei->addr, ei->size);
	}

	/* throw away partial pages */
	memblock_trim_memory(PAGE_SIZE);

	memblock_dump_all();
}
```

For each type E820_RAM and E820_RESERVED_KERN region in e820 map, it is added
to memblock.memory. Ok, you are connecting the e820 and memblock now~

# Some important API you may want to know

After we getting some knowledge of the memblock, it is time to see how to use
it.

## add/remove memblock.memory range

First one is how to fill memblock with hardware information. They are
memblock_add() and memblock_remove(). And both of them make sure the
memblock.memory is sorted.

They just touch memblock.memory.

## memory allocation and deallocation in memblock

Secondly, the API is memory allocation and deallocation.

They are memblock_alloc() and memblock_free(), which is similar to any other
memory alloction facility.

They just touch memblock.reserved.

# Dive into some of the algorithms

## memblock_add_range()

This function is used to add a range to a memblock_type, memblock.memory or
memblock.reserved. And this guarantees the range added will be sorted.

The idea is simple, add and merge. Generally, there are four cases:

* total covered
* no overlap
* partial overlapped -- beginning
* partial overlapped -- end

Let's try to explain this a little.

(base,   end) represents the new range be added.
[rbase, rend] represents the range already in memblock_type.

### total covered

```
           (base,     end)
      [rbase,             rend]
```

In this case, nothing would be done, except checking the range type and flags.

### no overlap

```
    (base,     end)
                      [rbase,             rend]
```

This case is also simple, it just add the new range.

### partial overlapped -- beginning

```
   (base,     end)
        [rbase,             rend]

   After insert the lower part.

               ||
               ||
               vv

             (end)
        [rbase,             rend]
```

In this case, the lower part (base, rbase-1) will be added. After that, base
is assigned with end. Then fall back to a special case of total covered.

### partial overlapped -- end

```
                        (base,          end)
        [rbase,             rend]

   After truncate the lower part.

               ||
               ||
               vv

                                (base', end)
        [rbase,             rend]
```

In this case, the lower part is "ignored". After that, base is assigned with
rend. Then fall back to the no overlap case.

## memblock_isolate_range()

This function is used in several places, like memblock_remove_range() and
memblock_set_node(). The purpose of this function is used to split memblock
regions.

```
                         (base,                         end)

      [rbase1,                   rend1]    [rbase2,                  rend2]

           After split

               ||
               ||
               vv

                         (base,                         end)

      [rbase1,    rend1'][rbase1',rend1]   [rbase2,   rend2'][rbas2', rend2]


                          start_rgn         end_rgn
```

The function will pass two parameters, base & end. And it use these two
parameter to split the memblock region and return the index in the region of
beginning and end.

[1]: https://lkml.org/lkml/2010/7/13/114
[2]: https://git.kernel.org/cgit/linux/kernel/git/torvalds/linux.git/commit/mm/memblock.c?id=95f72d1ed41a66f1c1c29c24d479de81a0bea36f
[3]: https://0xax.gitbooks.io/linux-insides/content/MM/linux-mm-1.html
