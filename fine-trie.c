/* A simple, (reverse) trie.  Only for use with 1 thread. */

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include "trie.h"

struct trie_node {
  struct trie_node *next;  /* parent list */
  unsigned int strlen; /* Length of the key */
  int32_t ip4_address; /* 4 octets */
  struct trie_node *children; /* Sorted list of children */
  char key[64]; /* Up to 64 chars */
  pthread_mutex_t lock;
  pthread_cond_t c;
  int waiting;
};

int _insert (const char *string, size_t strlen, int32_t ip4_address,
  struct trie_node *node, struct trie_node *parent, struct trie_node *left);
  static struct trie_node * root = NULL;
  static pthread_mutex_t root_lock;

  struct trie_node * new_leaf (const char *string, size_t strlen, int32_t ip4_address) {
    struct trie_node *new_node = malloc(sizeof(struct trie_node));
    if (!new_node) {
      printf ("WARNING: Node memory allocation failed.  Results may be bogus.\n");
      return NULL;
    }

    assert(strlen < 64);
    assert(strlen > 0);
    new_node->next = NULL;
    new_node->strlen = strlen;
    strncpy(new_node->key, string, strlen);
    new_node->key[strlen] = '\0';
    new_node->ip4_address = ip4_address;
    new_node->children = NULL;
    int rv = pthread_mutex_init(&(new_node->lock), NULL);
    assert(rv == 0);
    rv = pthread_cond_init(&(new_node->c), NULL);
    assert(rv == 0);
    new_node->waiting = 0;
    return new_node;
  }

  int compare_keys (const char *string1, int len1, const char *string2, int len2, int *pKeylen) {
    int keylen, offset1, offset2;
    keylen = len1 < len2 ? len1 : len2;
    offset1 = len1 - keylen;
    offset2 = len2 - keylen;
    assert (keylen > 0);
    if (pKeylen)
    *pKeylen = keylen;
    return strncmp(&string1[offset1], &string2[offset2], keylen);
  }

  void init(int numthreads) {
    //Initialization function, set root node to null, initialize the lock we will use for root.
    root = NULL;
    int rv = pthread_mutex_init(&root_lock, NULL);
    assert(rv == 0);
  }

  /* Recursive helper function.
  * Returns a pointer to the node if found.
  * Returns ip address of the node... -1 if not found
  * Stores an optional pointer to the
  * parent, or what should be the parent if not found.
  *
  * assumes node is locked
  * node lock will be released before returning
  *
  * node should never be null
  */
  int32_t _search (struct trie_node *node, const char *string, size_t strlen) {
    //This function is always entered by one of the _search functions, this is the HEAVY LIFTER of the searchers.
    int keylen, cmp;
    int rc;
    // First things first, check if we are NULL
    // NULL node means something screwed up somewhere.
    assert(node != NULL);
    if (node == NULL) return -1;
    //Strlen must be below 64 characters, this is just a standard of the application
    assert(node->strlen < 64);
    //Similar to the strlen being below 64 it must be greater than 0
    assert(strlen >0);
    //Check for substring, we start by comparing the sizes.
    cmp = compare_keys(node->key, node->strlen, string, strlen, &keylen);
    if (cmp == 0)
    {
      // Yes, either quit, or recur on the children
      // If this key is longer than our search string, the key isn't here
      if (node->strlen > keylen)
      {
        //Unlock the lock we previously acquired in a controller search function
        int rc = pthread_mutex_unlock(&(node->lock));
        assert(rc == 0);
        //return fail code
        return -1;
      }
      else if (strlen > keylen)
      {
        if(node->children != NULL)
        {
          // first lock children, then unlock parent to prevent possible
          // hand over hand
          //acquire childs lock, assert gaurantees we got it or terminates
          rc = pthread_mutex_lock(&(node->children->lock));
          assert(rc == 0);
          //unlock the current nodes lock, this is us moving forward on the search
          rc = pthread_mutex_unlock(&(node->lock));
          assert(rc == 0);
          //Recur into the child
          return _search(node->children, string, strlen - keylen);
        }
        else
        {
          //if there are no children release the lock, return fail code
          rc = pthread_mutex_unlock(&(node->lock));
          assert(rc == 0);
          return -1;
        }
      }
      else
      {
        //strlen should be the same length as keylen, assert it, return the IP address of the node
        assert (strlen == keylen);
        rc = pthread_mutex_unlock(&(node->lock));
        assert(rc == 0);
        return node->ip4_address;
      }
    }
    else if (cmp < 0)
    {
      //cmp returns 0 then we are looking next to us for the node
      if(node->next != NULL)
      {
        // No, look right (the node's key is "less" than the search key)
        // hand over hand
        //assign the next node
        struct trie_node *next = node->next;
        //get its lock, release the lock of current node
        rc = pthread_mutex_lock(&(next->lock));
        assert(rc == 0);
        rc = pthread_mutex_unlock(&(node->lock));
        assert(rc == 0);
        //recur the search through the next node
        return _search(next, string, strlen);
      }
      else
      {
        //not found release lock, return fail.
        rc = pthread_mutex_unlock(&(node->lock));
        assert(rc == 0);
        return -1;
      }
    }
    else
    {
      // Quit early
      rc = pthread_mutex_unlock(&(node->lock));
      assert(rc == 0);
      return -1;
    }

  }

  /* Recursive helper function.
  * Returns a pointer to the node if found.
  * Returns ip address of the node... -1 if not found
  * Stores an optional pointer to the
  * parent, or what should be the parent if not found.
  *
  * assumes node is locked
  * node lock will be released before returning
  *
  * node should never be null
  */
  int32_t _search_and_squat(struct trie_node *node, const char *string, size_t strlen, int32_t ip4_address) {

    int keylen, cmp;
    int rc;
    // First things first, check if we are NULL
    // this should never happen
    assert(node != NULL);
    if (node == NULL) return -1;
    assert(node->strlen < 64);
    assert(strlen >0);
    // See if this key is a substring of the string passed in
    cmp = compare_keys(node->key, node->strlen, string, strlen, &keylen);
    if (cmp == 0)
    {
      // Yes, either quit, or recur on the children
      // If this key is longer than our search string, the key isn't here
      if (node->strlen > keylen)
      {
        rc = pthread_mutex_unlock(&(node->lock));
        assert(rc == 0);

        return -1;
      }
      else if (strlen > keylen)
      {
        if(node->children != NULL)
        {
          // first lock children, then unlock parent to prevent possible
          // situation where the thread has no locks and children is deleted
          rc = pthread_mutex_lock(&(node->children->lock));
          assert(rc == 0);
          rc = pthread_mutex_unlock(&(node->lock));
          assert(rc == 0);
          // Recur on children list
          return _search(node->children, string, strlen - keylen);
        }
        else
        {
          rc = pthread_mutex_unlock(&(node->lock));
          assert(rc == 0);
          return -1;
        }
      }
      else
      {
        //here is where squatting block happens, loop through until rv = 1 due to having a max # of squats
        int rv = 0;
        node->waiting++;
        while(!rv)
        {
          if(node->ip4_address == 0)
          {
            node->ip4_address = ip4_address;
            node->waiting--;
            rv = 1;
          }
          else
          {
            int waitSuccess = pthread_cond_wait(&(node->c), &(node->lock));
            assert(waitSuccess == 0);
          }
        }
        assert (strlen == keylen);
        rc = pthread_mutex_unlock(&(node->lock));
        assert(rc == 0);
        return 1;
      }
    }
    else if (cmp < 0)
    {
      if(node->next != NULL)
      {
        // No, look right (the node's key is "less" than the search key)
        // first lock next, then unlock parent to prevent possible
        // situation where the thread has no locks and next is deleted
        //assign next node, get its lock, return lock of current node
        //recur through the next
        struct trie_node *next = node->next;
        rc = pthread_mutex_lock(&(next->lock));
        assert(rc == 0);
        rc = pthread_mutex_unlock(&(node->lock));
        assert(rc == 0);
        return _search(next, string, strlen);
      }
      else
      {
        rc = pthread_mutex_unlock(&(node->lock));
        assert(rc == 0);
        return -1;
      }
    }
    else
    {
      // Quit early
      rc = pthread_mutex_unlock(&(node->lock));
      assert(rc == 0);
      return -1;
    }
  }

  int search  (const char *string, size_t strlen, int32_t *ip4_address) {
    //Search Controller
    //main thread communicates directly with this and this pushes us where to go.
    int32_t found;
    int rc;
    // Skip strings of length 0
    if (strlen == 0)
    return 0;
    struct trie_node *node = root;

    if(node == NULL)
    return 0;
    rc = pthread_mutex_lock(&(node->lock));
    assert(rc == 0);

    found = _search(node, string, strlen);

    if (found >= 0 && ip4_address)
    *ip4_address = found;

    return (found >= 0);
  }


  int search_and_squat  (const char *string, size_t strlen, int32_t ip4_address) {
    int32_t found;
    int rc;
    // Skip strings of length 0
    if (strlen == 0)
    return 0;

    // in case the node was removed between trying to insert and squatting
    rc = pthread_mutex_lock(&(root_lock));
    assert(rc == 0);
    if(root != NULL)
    {
      found = _insert(string, strlen, ip4_address, root, NULL, NULL);
    }
    else
    {
      root = new_leaf (string, strlen, ip4_address);
      found = 1;
    }
    rc = pthread_mutex_unlock(&(root_lock));
    assert(rc == 0);

    while(!found)
    {
      rc = pthread_mutex_lock(&(root->lock));
      assert(rc == 0);


      found = _search_and_squat(root, string, strlen, ip4_address);

      // in case the node was removed between trying to insert and squatting
      if(!found)
      {
        found = insert(string, strlen, ip4_address);
      }
    }



    return (found >= 0);
  }


  /* Recursive helper function */
  int _insert (const char *string, size_t strlen, int32_t ip4_address,
    struct trie_node *node, struct trie_node *parent, struct trie_node *left)
    {
      int rc;

      // assert that this thread has a lock on both parent or left
      // whichever is not null

      // First things first, check if we are NULL
      assert (node != NULL);


      // get a lock on node
      rc = pthread_mutex_lock(&(node->lock));
      assert(rc == 0);


      int cmp, keylen;


      assert (node->strlen < 64);
      assert (node->strlen > 0);
      assert (strlen > 0);

      // Take the minimum of the two lengths
      cmp = compare_keys (node->key, node->strlen, string, strlen, &keylen);
      if (cmp == 0) {
        // Yes, either quit, or recur on the children

        // If this key is longer than our search string, we need to insert
        // "above" this node
        if (node->strlen > keylen)
        {
          struct trie_node *new_node;

          assert(keylen == strlen);
          assert((!parent) || parent->children == node);

          new_node = new_leaf (string, strlen, ip4_address);

          rc = pthread_mutex_lock(&(new_node->lock));
          assert(rc == 0);


          node->strlen -= keylen;
          new_node->children = node;

          assert ((!parent) || (!left));

          // should already have a lock on the parent or left node
          if (parent)
          {
            parent->children = new_node;
          }
          else if (left) {
            left->next = new_node;
          }
          else if ((!parent) || (!left)) {
            root = new_node;

          }

          rc = pthread_mutex_unlock(&(new_node->lock));
          assert(rc == 0);


          rc = pthread_mutex_unlock(&(node->lock));
          assert(rc == 0);


          return 1;
        }
        else if (strlen > keylen)
        {
          if (node->children == NULL)
          {
            struct trie_node *new_node = new_leaf (string, strlen - keylen, ip4_address);

            rc = pthread_mutex_lock(&(new_node->lock));
            assert(rc == 0);

            node->children = new_node;

            rc = pthread_mutex_unlock(&(new_node->lock));
            assert(rc == 0);

            rc = pthread_mutex_unlock(&(node->lock));
            assert(rc == 0);

            return 1;
          }
          else
          {
            // Recur on children list, store "parent" (loosely defined)
            int rv = _insert(string, strlen - keylen, ip4_address, node->children, node, NULL);

            rc = pthread_mutex_unlock(&(node->lock));
            assert(rc == 0);

            return rv;
          }
        }
        else
        {
          assert (strlen == keylen);
          if (node->ip4_address == 0)
          {
            node->ip4_address = ip4_address;

            // unlock node
            rc = pthread_mutex_unlock(&(node->lock));
            assert(rc == 0);

            return 1;
          }
          else
          {
            // unlock node
            rc = pthread_mutex_unlock(&(node->lock));
            assert(rc == 0);

            return 0;
          }
        }
      } else
      {
        /* Is there any common substring? */
        int i, cmp2, keylen2, overlap = 0;
        for (i = 1; i < keylen; i++) {
          cmp2 = compare_keys (&node->key[i], node->strlen - i,
            &string[i], strlen - i, &keylen2);
            assert (keylen2 > 0);
            if (cmp2 == 0)
            {
              overlap = 1;
              break;
            }
          }

          if (overlap) {
            // Insert a common parent, recur
            struct trie_node *new_node = new_leaf (&string[i], strlen - i, 0);

            // lock new node
            rc = pthread_mutex_lock(&(new_node->lock));
            assert(rc == 0);


            int diff = node->strlen - i;
            assert ((node->strlen - diff) > 0);
            node->strlen -= diff;
            new_node->children = node;
            assert ((!parent) || (!left));

            // TODO get root lock
            if (node == root)
            {
              new_node->next = node->next;
              node->next = NULL;
              root = new_node;
            }
            else if (parent)
            {
              assert(parent->children == node);
              new_node->next = NULL;
              parent->children = new_node;
            }
            else if (left)
            {
              new_node->next = node->next;
              node->next = NULL;
              left->next = new_node;
            }
            else if ((!parent) && (!left)) {
              // TODO get root lock
              root = new_node;
            }

            rc = pthread_mutex_unlock(&(node->lock));
            assert(rc == 0);



            int rv = _insert(string, i, ip4_address, node, new_node, NULL);

            rc = pthread_mutex_unlock(&(new_node->lock));
            assert(rc == 0);


            return rv;

          }
          else if (cmp < 0)
          {
            if (node->next == NULL) {
              // Insert here
              struct trie_node *new_node = new_leaf (string, strlen, ip4_address);

              rc = pthread_mutex_lock(&(new_node->lock));
              assert(rc == 0);


              node->next = new_node;

              rc = pthread_mutex_unlock(&(new_node->lock));
              assert(rc == 0);


              // unlock node
              rc = pthread_mutex_unlock(&(node->lock));
              assert(rc == 0);

              return 1;
            }
            else
            {
              // No, recur right (the node's key is "greater" than  the search key)
              // by convention node should be locked and node->next will be locked inside the method
              int rv = _insert(string, strlen, ip4_address, node->next, NULL, node);
              rc = pthread_mutex_unlock(&(node->lock));
              assert(rc == 0);
              return rv;
            }
          }
          else
          {
            // Insert here
            struct trie_node *new_node = new_leaf (string, strlen, ip4_address);
            rc = pthread_mutex_lock(&(new_node->lock));
            assert(rc == 0);
            new_node->next = node;
            // TODO get root lock
            if (node == root)
            {
              root = new_node;
            }
            else if (parent && parent->children == node)
            {
              parent->children = new_node;
            }

            rc = pthread_mutex_unlock(&(new_node->lock));
            assert(rc == 0);

          }

          rc = pthread_mutex_unlock(&(node->lock));
          assert(rc == 0);
          return 1;
        }
      }

      int insert (const char *string, size_t strlen, int32_t ip4_address) {
        // Skip strings of length 0
        int rc;
        if (strlen == 0)
        return 0;
        int rv;
        // get root lock
        rc = pthread_mutex_lock(&(root_lock));
        assert(rc == 0);
        if (root == NULL) {
          root = new_leaf (string, strlen, ip4_address);
          rv =  1;
          rc = pthread_mutex_unlock(&(root_lock));
          assert(rc == 0);
        }
        else
        {
          rv = _insert (string, strlen, ip4_address, root, NULL, NULL);
          rc = pthread_mutex_unlock(&(root_lock));
          assert(rc == 0);
          if (!rv && allow_squatting)
          {
            search_and_squat(string, strlen, ip4_address);
          }
        }
        return rv;
      }

      /* Recursive helper function.
      * Returns a pointer to the node if found.
      * Stores an optional pointer to the
      * parent, or what should be the parent if not found.
      *
      * If it returns a node, there will be a lock on that node
      */
      struct trie_node *
      _delete (struct trie_node *node, const char *string,
        size_t strlen) {
          int keylen, cmp;
          int rc;
          // First things first, check if we are NULL

          if (node == NULL) return NULL;


          // lock the node here,
          rc = pthread_mutex_lock(&(node->lock));
          assert(rc == 0);


          assert(node->strlen < 64);

          // See if this key is a substring of the string passed in
          cmp = compare_keys (node->key, node->strlen, string, strlen, &keylen);
          if (cmp == 0) {
            // Yes, either quit, or recur on the children

            // If this key is longer than our search string, the key isn't here
            if (node->strlen > keylen)
            {
              // unlock node
              rc = pthread_mutex_unlock(&(node->lock));
              assert(rc == 0);

              return NULL;
            }
            else if (strlen > keylen)
            {
              // found wont be unlocked by this method
              struct trie_node *found =  _delete(node->children, string, strlen - keylen);

              if (found)
              {
                if(found->waiting > 0 && allow_squatting)
                {
                  pthread_cond_broadcast(&(found->c));
                  rc = pthread_mutex_unlock(&(found->lock));
                  assert(rc == 0);

                }
                else
                {
                  /* If the node doesn't have children, delete it.
                  * Otherwise, keep it around to find the kids */
                  if (found->children == NULL && found->ip4_address == 0)
                  {
                    // shouldnt need to lock found->next because as long as parent is locked,
                    // nothing bad should be able to happen to it (check what insert does)
                    assert(node->children == found);
                    node->children = found->next;
                    // unlock found
                    rc = pthread_mutex_unlock(&(found->lock));
                    assert(rc == 0);


                    free(found);

                  }
                  else
                  {
                    rc = pthread_mutex_unlock(&(found->lock));
                    assert(rc == 0);

                  }
                }

                // unlock found

                // TODO get root lock
                /* Delete the root node if we empty the tree */
                if (node == root && node->children == NULL && node->ip4_address == 0)
                {
                  if(node->waiting > 0 && allow_squatting)
                  {
                    pthread_cond_broadcast(&(node->c));
                    rc = pthread_mutex_unlock(&(node->lock));
                    assert(rc == 0);

                  }
                  else
                  {
                    root = node->next;

                    rc = pthread_mutex_unlock(&(node->lock));
                    assert(rc == 0);


                    free(node);
                  }
                }

                // dont unlock node, we are still doing operations on it
                return node; /* Recursively delete needless interior nodes */
              }
              else
              {
                rc = pthread_mutex_unlock(&(node->lock));
                assert(rc == 0);

                return NULL;
              }
            }
            else
            {
              assert (strlen == keylen);

              /* We found it! Clear the ip4 address and return. */
              if (node->ip4_address)
              {
                node->ip4_address = 0;

                //TODO lock root
                /* Delete the root node if we empty the tree */
                if (node == root && node->children == NULL && node->ip4_address == 0)
                {

                  root = node->next;


                  rc = pthread_mutex_unlock(&(node->lock));
                  assert(rc == 0);


                  // dont free if waiting > 0

                  free(node);
                  return (struct trie_node *) 0x100100; /* XXX: Don't use this pointer for anything except
                  * comparison with NULL, since the memory is freed.
                  * Return a "poison" pointer that will probably
                  * segfault if used.
                  */
                }

                // dont return node, still doing stuff
                return node;
              }
              else
              {
                /* Just an interior node with no value */
                rc = pthread_mutex_unlock(&(node->lock));
                assert(rc == 0);

                return NULL;
              }
            }
          }
          else if (cmp < 0) {
            // No, look right (the node's key is "less" than  the search key)

            // found should be locked
            struct trie_node *found = _delete(node->next, string, strlen);

            if (found)
            {
              if(found->waiting > 0 && allow_squatting)
              {
                pthread_cond_broadcast(&(found->c));
                rc = pthread_mutex_unlock(&(found->lock));
                assert(rc == 0);

              }
              else
              {
                /* If the node doesn't have children, delete it.
                * Otherwise, keep it around to find the kids */
                if (found->children == NULL && found->ip4_address == 0)
                {
                  assert(node->next == found);
                  node->next = found->next;
                  rc = pthread_mutex_unlock(&(found->lock));
                  assert(rc == 0);


                  free(found);
                }
                else
                {
                  rc = pthread_mutex_unlock(&(found->lock));
                  assert(rc == 0);

                }
              }

              return node; /* Recursively delete needless interior nodes */
            }

            rc = pthread_mutex_unlock(&(node->lock));
            assert(rc == 0);

            return NULL;
          }
          else
          {
            // Quit early
            rc = pthread_mutex_unlock(&(node->lock));
            assert(rc == 0);

            return NULL;
          }

        }

        int delete  (const char *string, size_t strlen) {
          // Skip strings of length 0
          if (strlen == 0)
          return 0;
          int rc;

          rc = pthread_mutex_lock(&root_lock);
          assert(rc == 0);
          struct trie_node *parent = _delete(root, string, strlen);

          rc = pthread_mutex_unlock(&root_lock);
          assert(rc == 0);
          // release root lock

          int rv =(NULL != parent);

          if(parent != NULL && parent != ((struct trie_node *) 0x100100))
          {
            rc = pthread_mutex_unlock(&(parent->lock));
            assert(rc == 0);

          }


          return rv;
        }


        void _print (struct trie_node *node, int printLevel, int nodePos) {
          //  printf ("Node at %p.  Key %.*s, IP %d.  Next %p, Children %p\n",
          //  node, node->strlen, node->key, node->ip4_address, node->next, node->children);
          printf("\nvvvvvvvv - The %d node on Level %d - vvvvvvvvvv\n", nodePos, printLevel);
          printf("|Node at: %p\n", node);
          printf("|Key: %.*s\n", node->strlen, node->key);
          printf("|IP: %d\n", node->ip4_address);
          printf("|Next: %p\n", node->next);
          printf("|Child: %p\n", node->children);
          printf("^^^^^^^^ - The %d node on Level %d - ^^^^^^^^^^\n", nodePos, printLevel);
          if (node->children) {
            printf("\nvvvvvvvvvvvvvv\n");
            printf("!!!Entering next level of trie!!!\n");
            printf("vvvvvvvvvvvvvv\n");
            // printf("Now on Level %d at Position %d\n", printLevel, nodePos);
            _print(node->children, printLevel+1, 0);
          }
          if (node->next) {
            printf("\n>>>>>>>>>>>>>>\n");
            printf("!!!Moving to next node of level!!!\n");
            printf(">>>>>>>>>>>>>>\n");
            //printf("Node on Level %d at Position %d\n", printLevel, nodePos);
            _print(node->next, printLevel, nodePos+1);
          }
        }


        void print() {
          // TODO get root lock
          printf ("Root is at %p\n", root);
          struct trie_node *node = root;
          // relese root lock

          if(node)
          {
            _print(node,0 ,0);
          }
        }
