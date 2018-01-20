# Multithread lock demonstration over a DNS lookup simulator
This is a demonstration of different multithreaded data protection techniques using a DNS Host Name Lookup simulation, with a trie as the data structure.

This source code demonstrates:
  - Sequential lookups with one thread sequential-trie.c
  - Multithreaded lookups in the following ways:
    - Mutex, one lock for the entire trie, mutex-trie.c
    - Coarse grained (allows many concurrent readers, only one writer), rw-trie.c
    - Fine grained locking (every node in the trie has it's own lock), fine-trie.c

