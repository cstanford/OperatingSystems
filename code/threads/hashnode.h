#ifndef HASHNODE_H
#define HASHNODE_H

template <typename K, typename V>

class HashNode {

    public:
	HashNode(const K &key, const V &value) :
	    key(key), value(value), next(NULL){
	    }
    
	K getKey() const {
	    return key;
	}

	V getValue() const {
	    return value;
	}

	void setValue(V value) {
	    HashNode::value = value;
	}

	HashNode *getNext() const {
	    return next;
	}

	void setNext(HashNode *next) {
	    HashNode::next = next;
	}
    
	void setBit(bool bit){
	    HashNode:: dirty = bit;
	}

    
    private:

	K key;
	V value;
	HashNode *next;
	bool dirty;

};

#endif
