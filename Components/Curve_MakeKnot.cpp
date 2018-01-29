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


#include "Curve_MakeKnot.h"

#include <assert.h>

#include "Polyline.h"
#include "Geomlib_PolylineIntersection.h"

using namespace Urho3D;

String Curve_MakeKnot::iconTexture = "";

namespace {
	bool PolylineKnot(VariantMap poly_in, Variant & mesh_out, float disp)
	{
		// 1) find all the intersections in order of traversal of the polyline

		// 2) add these intersection points in as vertices of the polyline
        
        // (assume this comes in as input from Curve_SelfIntersection
        float orig_D = disp;
        
        VariantVector verts = Polyline_GetVertexList(poly_in);
        VariantVector new_verts;
        new_verts.Push(verts[0]);
        for (int i = 1; i < verts.Size(); ++i){
            Vector3 prev_copy, new_copy;
            VariantVector::ConstIterator it = new_verts.Find(verts[i]);
            if (it == new_verts.End())
                new_verts.Push(verts[i]);
            else{
                new_copy = verts[i].GetVector3();
                prev_copy = it->GetVector3();
                // now modify
                new_copy.y_ += disp;
                prev_copy.y_ -= disp;
                disp *= -1; // switch this for next intersection
                // now modify the previous copy
                int prev_id = it - new_verts.Begin();
                new_verts[prev_id] = Variant(prev_copy);
                // push the new copy
                new_verts.Push(Variant(new_copy));
                
            }
        }

		// do the first under separately.
		// now ease in Y values: overs to unders

		int first_over = 0;
		int first_under = new_verts.Size();
		for (int i = 0; i < new_verts.Size(); ++i) {
			Vector3 cand_over = new_verts[i].GetVector3();
			if (Abs(new_verts[i].GetVector3().y_ + orig_D) < 0.01) {
				first_under = i;
				// number of verts between over and under
				int n_to_ease = first_under - first_over;
				// delta
				float delta = orig_D / n_to_ease;
				int multiplier = 1;
				for (int j = first_over + 1; j < first_under; ++j) {
					Vector3 new_copy = verts[j].GetVector3();
					// now modify
					new_copy.y_ = orig_D - multiplier*delta;
					new_verts[j] = Variant(new_copy);
					++multiplier;
				}
				break;
			}
		}

		// now ease in Y values: unders to overs
		for (int i = 0; i < new_verts.Size(); ++i) {
			int first_under = 0;
			int first_over = new_verts.Size();
			Vector3 cand_under = new_verts[i].GetVector3();
			if (Abs(new_verts[i].GetVector3().y_ + orig_D) < 0.01) {
				first_under = i;
				for (int j = i; j < new_verts.Size(); ++j) {
					// find the next under
					Vector3 cand_over = new_verts[j].GetVector3();
					if (Abs(new_verts[j].GetVector3().y_ - orig_D) < 0.01) {
						first_over = j;
                        break;
						}
					}
                // number of verts between over and under
                int n_to_ease = first_over - first_under;
                // delta
                float delta = 2 * orig_D / n_to_ease;
                int multiplier = 1;
                for (int k = first_under + 1; k < first_over; ++k) {
                    Vector3 new_copy = verts[k].GetVector3();
                    // now modify
                    new_copy.y_ = -1 * orig_D + multiplier*delta;
                    new_verts[k] = Variant(new_copy);
                    ++multiplier;
				}

			}
		}

        
        // now ease in Y values: overs to unders
        for (int i = 0; i < new_verts.Size(); ++i){
            int first_over = 0;
            int first_under = new_verts.Size();
			Vector3 cand_over = new_verts[i].GetVector3();
            if (Abs(new_verts[i].GetVector3().y_ - orig_D) < 0.01){
                first_over = i;
                for (int j = i; j < new_verts.Size(); ++j){
                    // find the next under
					Vector3 cand_under = new_verts[j].GetVector3();
					if (Abs(new_verts[j].GetVector3().y_ + orig_D) < 0.01) {
						first_under = j;
                        break;
						}
					}
                int n_to_ease = first_under - first_over;
                // delta
                float delta = 2 * orig_D / n_to_ease;
                int multiplier = 1;
                for (int k = first_over + 1; k < first_under; ++k) {
                    Vector3 new_copy = verts[k].GetVector3();
                    // now modify
                    new_copy.y_ = orig_D - multiplier*delta;
                    new_verts[k] = Variant(new_copy);
                    ++multiplier;
                }
            }

        }
        

        mesh_out = Polyline_Make(new_verts);
        

		// 3) compute new Y (vertical) values (assuming X-Z plane) for these points following an alternating pattern

		// 4) ease in the Y values for all the other polyline vertices

		// 5) create the tubes

		return true;
	}
}

Curve_MakeKnot::Curve_MakeKnot(Context* context) : IoComponentBase(context, 2, 1)
{
	SetName("DrawAKnot");
	SetFullName("Draw a Knot");
	SetDescription("Converts a polyline into a knot");
	SetGroup(IoComponentGroup::CURVE);
	SetSubgroup("Operators");

	inputSlots_[0]->SetName("Polyline");
	inputSlots_[0]->SetVariableName("P");
	inputSlots_[0]->SetDescription("Polyline to knotify");
	inputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
	inputSlots_[0]->SetDataAccess(DataAccess::ITEM);
    
    inputSlots_[1]->SetName("Displacement");
    inputSlots_[1]->SetVariableName("D");
    inputSlots_[1]->SetDescription("Thickness of knot");
    inputSlots_[1]->SetVariantType(VariantType::VAR_FLOAT);
    inputSlots_[1]->SetDataAccess(DataAccess::ITEM);
    inputSlots_[1]->SetDefaultValue(1.0f);
    inputSlots_[1]->DefaultSet();

	outputSlots_[0]->SetName("Knot");
	outputSlots_[0]->SetVariableName("M");
	outputSlots_[0]->SetDescription("knot polyline");
	outputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);
}

void Curve_MakeKnot::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
)
{
	assert(inSolveInstance.Size() == inputSlots_.Size());
	assert(outSolveInstance.Size() == outputSlots_.Size());

	///////////////////
	// VERIFY & EXTRACT

	// Verify input slot 0
	if (inSolveInstance[0].GetType() != VariantType::VAR_VARIANTMAP) {
		URHO3D_LOGWARNING("P must be a polyline.");
		outSolveInstance[0] = Variant();
		return;
	}
	VariantMap polyline_in = inSolveInstance[0].GetVariantMap();
	if (!Polyline_Verify(polyline_in))
	{
		URHO3D_LOGWARNING("P must be a polyline.");
		outSolveInstance[0] = Variant();
		return;
	}
    
    float disp = inSolveInstance[1].GetFloat();

	///////////////////
	// COMPONENT'S WORK

	Variant mesh;
	bool success = PolylineKnot(polyline_in, mesh, disp);
	if (!success) {
		URHO3D_LOGWARNING("PolylineKnot operation failed.");
		outSolveInstance[0] = Variant();
		return;
	}

	/////////////////
	// ASSIGN OUTPUTS

	outSolveInstance[0] = mesh;
}



