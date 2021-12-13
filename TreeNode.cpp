

#include <bits/stdc++.h>

#include "TreeNode.h"
#include "CurveElement.h"
#include "DiscreteFrechet.h"

TreeNode::TreeNode(){


    this->Curve_Object = NULL;
	this->left = this->right = NULL;

}

TreeNode* TreeNode::AddNode(TreeNode* current, int step, int N){

    if (step < N){
		
        TreeNode* new_node = new TreeNode();
		current = new_node;

		current->left = AddNode(current->left, 2 * step + 1, N);
		current->right = AddNode(current->right, 2 * step + 2, N);
	}

	return current;

}

bool TreeNode::AddCurve(TreeNode* current, CurveElement* curve){


    if (current->Curve_Object != NULL){
        return false;
    }
    
    // if node is null, return
    if (!current) return false;
     
    // if node is leaf node, print its data   
    if (!current->left && !current->right)
    {
        
        current->Curve_Object = curve;
        //cout << root->curve << " ";
        
        bool set = true;
        return set;
    }
    
    // if left child exists, check for leaf
    // recursively
    if (current->left){
        bool get = AddCurve(current->left,curve); 
        if (get){
            return true;
        }
    }
       
         
    // if right child exists, check for leaf
    // recursively
    if (current->right){
        bool get = AddCurve(current->right,curve);       
        if (get){
            return true;
        }
    }
    
    return false;

}


CurveElement* TreeNode::MeanCurveTraversal(TreeNode* current){


    if (!current->left && !current->right)
    {
      return current->Curve_Object;
    }

    // first recur on left subtree
    CurveElement* left_mean;
    if (current->left){
        left_mean = MeanCurveTraversal(current->left);
    }else{
        left_mean = NULL;
    }
    
    //if (left_mean == -1){
    //    left_mean = 0;
    //}

    //CurveElement* right_mean = MeanCurveTraversal(current->right);
    
    CurveElement* right_mean;
    if (current->right){
        right_mean = MeanCurveTraversal(current->right);
    }else{
        right_mean = NULL;
    }

    //if (right_mean == -1){
    //    right_mean = 0;
    //}

    if (left_mean == NULL && right_mean == NULL){
        return NULL;
    }

    CurveElement* mean_curve = MeanCurve(left_mean, right_mean);
    if (mean_curve != NULL){
        mean_curve->displayVectorElementGrid();
        myLogFile << "size " << mean_curve->arrayElementTwoD.size() << std::endl;
    }
    
    // now deal with the node
    //cout << root->curve << " " << root->data << " end ";

    

    return mean_curve;


}