#ifndef KEYHASH_H
#define KEYHASH_H
/* !! "It is helpful for the hash function to be some computation
 *       based on the virtual address and the id of the process
 *       that owns the page."
 */

static const int TABLE_SIZE = 32;

struct IPTHash{
    int pid;
    int vpn;

    bool operator==(const IPTHash& rhs) const {
        return (this->pid == rhs.pid && this->vpn == rhs.vpn);
    }
    bool operator!=(const IPTHash& rhs) const {
        return (*this == rhs);
    }
};
template <typename K>
    struct KeyHash {
	unsigned long operator()(const K& key) const
	{
	    return reinterpret_cast<unsigned long>(key) % TABLE_SIZE;
	}

    };

struct PIDHash {
    unsigned long operator()(const IPTHash& key) const
    {
        return (key.pid*key.vpn*37) % TABLE_SIZE;
    }
};
#endif
