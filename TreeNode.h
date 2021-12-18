
#ifndef TREENODE_H
#define TREENODE_H


#include <bits/stdc++.h>

#include "CurveElement.h"

//TreeNode class
class TreeNode
{

public:
    
    CurveElement* Curve_Object;
    TreeNode* left; 
    TreeNode* right;


public:

    TreeNode();
    TreeNode* AddNode(TreeNode*, int, int);
    bool AddCurve(TreeNode* , CurveElement* );
    CurveElement* MeanCurveTraversal(TreeNode*);
    void DeleteNode(TreeNode*);
};




#endif /* TREENODE_H */