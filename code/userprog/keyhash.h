#ifndef KEYHASH_H
#define KEYHASH_H

/* !! "It is helpful for the hash function to be some computation
 *       based on the virtual address and the id of the process
 *       that owns the page."
 */

static const int TABLE_SIZE = 256;

template <typename K>
    struct KeyHash {
	unsigned long operator()(const K& key) const
	{
	    return reinterpret_cast<unsigned long>(key) % TABLE_SIZE;
	}

    };
#endif
