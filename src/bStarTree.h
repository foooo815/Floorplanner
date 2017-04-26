/****************************************************************************
  FileName  [ bStarTree.h ]
  Synopsis  [ Define the B*-tree data structure. ]
  Author    [ Fu-Yu Chuang ]
  Date      [ 2017.4.25 ]
****************************************************************************/
#ifndef BSTARTREE_H
#define BSTARTREE_H

#include <vector>
#include <string>
#include "module.h"
using namespace std;

// Doubly-linked list node
class LNode
{
    friend class BStarTree;

private:
    // constructor and destructor
    LNode(LNode* prev = NULL, LNode* next = NULL) :
        _prev(prev), _next(next) { }
    ~LNode()    { }

    void setPos(size_t x, size_t y) {
        _x = x; _y = y;
    }

    void insertPrev(LNode* node) {

    }

    void insertNext(LNode* node) {
        LNode* n = _next;
        _next = node;
        node->_next = n;
    }

    void deletePrev() {

    }

    void deleteNext() {
        _next = _next->_next;
    }

    LNode*      _prev;      // previous doubly-linked list node
    LNode*      _next;      // next doubly-linked list node
    size_t      _x;         // coordinate x
    size_t      _y;         // coordinate y
};


// B*-tree node
class TNode
{
    friend class BStarTree;

private:
    // constructor and destructor
    TNode(size_t id, bool orient = false, TNode* p = NULL, TNode* l = NULL, TNode* r = NULL) :
        _id(id), _orient(orient), _parent(p), _left(l), _right(r) { }
    ~TNode()    { }

    // basic access methods
    size_t getId()      { return _id; }
    bool   getOrient()  { return _orient; }

    // modify methods
    void rotate()           { _orient = !_orient; }
    void setId(size_t id)   { _id = id; }

    size_t      _id;        // id of the block storing in this node
    bool        _orient;    // record the orientation of the block (0: origin, 1: rotated)
    TNode*      _parent;    // parent of the node
    TNode*      _left;      // left child of the node
    TNode*      _right;     // right child of the node
};


class BStarTree
{
public:
    // constructor and destructor
    BStarTree(vector<Block*> blockList);
    BStarTree(const BStarTree& tree);
    BStarTree& operator = (const BStarTree& tree);
    ~BStarTree();

    // perturbing the B*-tree
    vector<BStarTree> perturb();

    // packing
    void pack();

private:
    TNode*          _root;          // root of the B*-tree
    LNode*          _contour;       // contour list for packing
    vector<TNode*>  _nodeList;      // list of nodes in the tree
    vector<LNode*>  _contourList;   // list of contours in the tree

    // private member functions
    void copyTree(TNode** nodePtr, const TNode* cNode, TNode* prev);
    void clear();

    // manipulating the B*-tree to get the "neighborhood structures"
    void rotate(vector<BStarTree>& trees);
    void swap(vector<BStarTree>& trees);
    void delAndInsert(vector<BStarTree>& trees);

    // manipulating the nodes
    void swapNodes(int id1, int id2);
    void rotateNode(int id);
    void deleteNode(int id);
    void insertNode(int id1, int id2, bool p_right, bool n_right);
};


#endif  // BSTARTREE_H
