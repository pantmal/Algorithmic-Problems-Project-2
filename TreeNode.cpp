

#include <bits/stdc++.h>

#include "TreeNode.h"
#include "CurveElement.h"
#include "DiscreteFrechet.h"

//Constructor
TreeNode::TreeNode(){


    this->Curve_Object = NULL;
	this->left = this->right = NULL;

}

//Used to built the tree recursively
TreeNode* TreeNode::AddNode(TreeNode* current, int step, int N){

    //Allocating N nodes
    if (step < N){
	
	//allocate new node
        TreeNode* new_node = new TreeNode();
		current = new_node;

	    //then move in left and right subtrees 
		current->left = AddNode(current->left, 2 * step + 1, N);
		current->right = AddNode(current->right, 2 * step + 2, N);
	}

	return current;

}

//Sort of recursive method to add a curve in a leaf node
bool TreeNode::AddCurve(TreeNode* current, CurveElement* curve){

    //return if curve is already set
    if (current->Curve_Object != NULL){
        return false;
    }
    
    // return on null node
    if (!current) return false;
     
    // set the curve, if we're in leaf
    if (!current->left && !current->right)
    {
        
        current->Curve_Object = curve;
        
        bool set = true;
        return set;
    }
    
    //check in left subtree
    if (current->left){
        bool get = AddCurve(current->left,curve); 
        if (get){
            return true;
        }
    }
       
         
    //check in right subtree
    if (current->right){
        bool get = AddCurve(current->right,curve);       
        if (get){
            return true;
        }
    }
    
    return false;

}

//Post order traversal to calculate the mean curve
CurveElement* TreeNode::MeanCurveTraversal(TreeNode* current){

    //return curve, if we're on leaf
    if (!current->left && !current->right)
    {
      return current->Curve_Object;
    }

    //left subtree
    CurveElement* left_mean;
    if (current->left){
        left_mean = MeanCurveTraversal(current->left);
    }else{
        left_mean = NULL;
    }
    
    //right subtree
    CurveElement* right_mean;
    if (current->right){
        right_mean = MeanCurveTraversal(current->right);
    }else{
        right_mean = NULL;
    }

    //null for empty nodes
    if (left_mean == NULL && right_mean == NULL){
        return NULL;
    }

	//get mean curve
    CurveElement* mean_curve = MeanCurve(left_mean, right_mean);
    // if (mean_curve != NULL){
    //     mean_curve->displayVectorElementGrid();
    //     myLogFile << "size " << mean_curve->arrayElementTwoD.size() << std::endl;
    // }
    

    return mean_curve;

}

//Destructor
void TreeNode::DeleteNode(TreeNode* current)
{
    if (current == NULL) return;

    //delete both subtrees 
    DeleteNode(current->left);
    DeleteNode(current->right);
     
    //cout << "\n Deleting node: " << node->data;

    //Deleting curve object
    if(current->Curve_Object != NULL){
        // std::cout << " del curve" << std::endl;
        if (current->Curve_Object->mark_deletion){
          delete current->Curve_Object;  
        }
    } 

    //delete curr node
    delete current;
}
