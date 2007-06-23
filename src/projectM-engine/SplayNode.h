/**
 * projectM -- Milkdrop-esque visualisation SDK
 * Copyright (C)2003-2007 projectM Team
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * See 'LICENSE.txt' included within this release
 *
 */
/**
 * $Id$
 *
 * Node of a splay tree
 *
 * $Log$
 */

#ifndef _SPLAYNODE_H
#define _SPLAYNODE_H

#include "projectM.h"

#include "compare.h"

typedef void Object;
template <class Data = Object>
class SplayNode {
public:
    int type;
    SplayTree *tree;
    SplayNode *left, *right;
    Data *data;
    void *key;

    SplayNode();
    SplayNode( int type, void *key, void *data, SplayTree *tree );
    ~SplayNode();
  };

/** Create a new default splaynode */
template <class Data>
SplayNode<Data>::SplayNode() {
    this->data = NULL;
    this->type = -1;
    this->key = NULL;
    this->tree = NULL;
  }

/* Create a new splay node type */
template <class Data>
SplayNode<Data>::SplayNode(int type, void * key, Data * data, SplayTree *tree) {

	/* Creates the new splay node struct */
	this->data = data;
	this->type = type;
	this->key = key;
    this->tree = tree;
  }

/* Recursively free all the splaynodes */
template <class Data>
SplayNode<Data>::~SplayNode() {

    if ( tree == NULL ) {
        if ( key != NULL || data != NULL ) {
            /** This shouldn't happen */
            printf( "~SplayNode: tree is NULL with non-NULL key/data!\n" );
            printf( "\tleft: %X\tright: %X\tdata: %X\tkey: %X\n",
                    left, right, data, key );
          }
        return;
      }

  /* Ok if this happens, a recursive base case */
  /* Free left node */
    if ( left != NULL ) {
        delete left;
      }
  
  /* Free right node */
    if ( right != NULL ) {
        delete right;
      }
  
    /* Free this node's key */
//    printf( "~SplayNode: %X\t%X\n", key, tree->free_key );
    tree->free_key(key);
  
  /* Note that the data pointers are not freed here.
     Should be freed with a splay traversal function */
  }

#endif /** !_SPLAYTREE_H */
