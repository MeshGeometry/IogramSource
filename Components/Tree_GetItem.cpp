
//
// Copyright (c) 2016 - 2017 Mesh Consultants Inc.
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//


#include "Tree_GetItem.h"

#include <assert.h>

using namespace Urho3D;

String Tree_GetItem::iconTexture = "Textures/Icons/Tree_GetItem.png";

Tree_GetItem::Tree_GetItem(Context* context) :
IoComponentBase(context, 3, 1)
{
    SetName("GetTreeItem");
    SetFullName("Get Tree Item");
    SetDescription("Get Tree Item by path and index");
    SetGroup(IoComponentGroup::TREE);
    SetSubgroup("Operations");
    
    inputSlots_[0]->SetName("IoDataTree");
    inputSlots_[0]->SetVariableName("T");
    inputSlots_[0]->SetDescription("IoDataTree storing lookup item");
    inputSlots_[0]->SetVariantType(VariantType::VAR_NONE);
    inputSlots_[0]->SetDataAccess(DataAccess::TREE);
    
    inputSlots_[1]->SetName("Branch (int)");
    inputSlots_[1]->SetVariableName("B");
    inputSlots_[1]->SetDescription("Path in branch in tree");
    inputSlots_[1]->SetVariantType(VariantType::VAR_INT);
    inputSlots_[1]->SetDataAccess(DataAccess::ITEM);
    inputSlots_[1]->SetDefaultValue(Urho3D::Variant(0));
    inputSlots_[1]->DefaultSet();
    
    inputSlots_[2]->SetName("Optional Index");
    inputSlots_[2]->SetVariableName("I");
    inputSlots_[2]->SetDescription("Index of item in list");
    inputSlots_[2]->SetVariantType(VariantType::VAR_INT);
    inputSlots_[2]->SetDataAccess(DataAccess::ITEM);
    inputSlots_[2]->SetDefaultValue(Urho3D::Variant(-1));
    inputSlots_[2]->DefaultSet();

    outputSlots_[0]->SetName("Item");
    outputSlots_[0]->SetVariableName("I");
    outputSlots_[0]->SetDescription("Item from tree");
    outputSlots_[0]->SetVariantType(VariantType::VAR_NONE);
    outputSlots_[0]->SetDataAccess(DataAccess::TREE);
}

// Always attempts to solve the node anew, based on current input values, regardless of value of solvedFlag_
// Returns:
//   1 if the node is successfully solved
//   0 otherwise
int Tree_GetItem::LocalSolve()
{
    if (inputSlots_[0]->HasNoData()) {
        solvedFlag_ = 0;
        return 0;
    }

    IoDataTree output_tree(GetContext());
    
    IoDataTree tree_to_query = inputSlots_[0]->GetIoDataTree();
    IoDataTree branch_tree = inputSlots_[1]->GetIoDataTree();
    IoDataTree index_tree = inputSlots_[2]->GetIoDataTree();
    

    Variant result;
    Vector<int> null_path;
    null_path.Push(0);
    
    // Figure out how many branches we are going to query.
    int number_of_branches = branch_tree.GetNumItemsAtBranch(null_path, ITEM);
    for (int i = 0; i < number_of_branches; ++i){
        // get branch IDs of interest
        Variant cur_input_branch;
        branch_tree.GetItem(cur_input_branch, null_path, i);
      
        // type checking branch IDs
        VariantType entry_type = cur_input_branch.GetType();
        if (entry_type != VAR_INT){
            URHO3D_LOGWARNING("branch ID must be an int");
            return solvedFlag_ = 0;
        }
        // get the branch ID, and convert to path format
        int branch = cur_input_branch.GetInt();
        Vector<int> branch_path;
        branch_path.Push(branch);
        
        // set up output branch path
        Vector<int> output_path;
        output_path.Push(i);
        
        // get item ID if applicable
        Variant index_var;
        index_tree.GetItem(index_var, null_path, 0);
        
        // type checking index ID
        VariantType item_type = index_var.GetType();
        if (item_type != VAR_INT){
            URHO3D_LOGWARNING("item ID must be an int");
            return solvedFlag_ = 0;
        }
        
        // get the item ID, note this defaults to -1 to get whole branch
        int index = index_var.GetInt();
        // if a specific ID is being queried, look it up
        if (index != -1){
            tree_to_query.GetItem(result, branch_path, index);
            output_tree.Add(output_path, result);
        }
        
        // otherwise, loop through all the items on the branch
        else{
            // we are getting the whole list
            int number_entries_on_branch = tree_to_query.GetNumItemsAtBranch(null_path, ITEM);
            
            for (int i = 0; i < number_entries_on_branch; ++i){
                tree_to_query.GetItem(result, branch_path, i);
                output_tree.Add(output_path, result);
            }
        }
    }
    
    outputSlots_[0]->SetIoDataTree(output_tree);
    
    return solvedFlag_ = 1; // the only way that solvedFlag_ ever becomes 1
}


