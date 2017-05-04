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


#include "Graphics_VertexColors.h"

#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/Drawable.h>
#include <Urho3D/Graphics/Geometry.h>
#include <Urho3D/Graphics/VertexBuffer.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Graphics/Model.h>

using namespace Urho3D;

String Graphics_VertexColors::iconTexture = "Textures/Icons/Graphics_VertexColors.png";

namespace
{
	
}

Graphics_VertexColors::Graphics_VertexColors(Urho3D::Context* context) : IoComponentBase(context, 0, 0)
{
	SetName("CreateMaterial");
	SetFullName("Create Material");
	SetDescription("Create a material from parameters");

	AddInputSlot(
		"Drawable",
		"D",
		"Pointer to drawable to edit vertex colors.",
		VAR_PTR,
		ITEM
	);

	AddInputSlot(
		"Colors",
		"C",
		"Vertex colors.",
		VAR_COLOR,
		LIST,
		Color::CYAN
	);

	AddInputSlot(
		"Force",
		"FV",
		"Force vertex coloring.",
		VAR_BOOL,
		DataAccess::ITEM,
		false
	);

	AddInputSlot(
		"Copy counts",
		"C",
		"A list of numbers to copy the color. Useful for per face coloring.",
		VAR_INT,
		DataAccess::LIST,
		3
	);

	AddOutputSlot(
		"Reference",
		"Ref",
		"Reference to drawable",
		VAR_PTR,
		ITEM
	);
}


void Graphics_VertexColors::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
)
{
	//get the data in number of different ways	
	Model* mdl = new Model(GetContext());
	StaticModel* sm = (StaticModel*)inSolveInstance[0].GetPtr();

	if (sm)
	{
		mdl = sm->GetModel();
	}

	if (!mdl)
	{
		mdl = (Model*)inSolveInstance[0].GetPtr();
	}
	
	if (!mdl)
	{
		SetAllOutputsNull(outSolveInstance);
		return;
	}

	//get the other data
	bool force = inSolveInstance[2].GetBool();
	VariantVector copyCounts = inSolveInstance[3].GetVariantVector();
	int numCopyCounts = copyCounts.Size();

	//really need at least one color in the array
	if (copyCounts.Empty())
	{
		SetAllOutputsNull(outSolveInstance);
		return;
	}

	//proceed with coloring
	VariantVector colors = inSolveInstance[1].GetVariantVector();
	int numColors = colors.Size();
	int numGeos = mdl->GetNumGeometries();
	for (int i = 0; i < numGeos; i++)
	{
		int numLod = mdl->GetNumGeometryLodLevels(i);
		for (int j = 0; j < numLod; j++)
		{
			Geometry* g = mdl->GetGeometry(i, j);

			if (!g)
			{
				continue;
			}

			VertexBuffer* vb = g->GetVertexBuffer(0);

			if (g && vb)
			{
				unsigned char* vertexData = (unsigned char*)vb->Lock(0, vb->GetVertexCount());
				unsigned int offset = 24;
				if (vertexData)
				{
					unsigned vertexSize = vb->GetVertexSize();
					unsigned numVertices = vb->GetVertexCount();
					int copyIndex = 0;
					int currCopyNum = 0;
					int currCopyCount = copyCounts[0].GetInt();
					currCopyCount = Clamp(currCopyCount, 1, M_MAX_INT);
					int colorIndex = 0;
					for (unsigned k = 0; k < numVertices; ++k)
					{					
						unsigned int& dest = *reinterpret_cast<unsigned int*>(vertexData + k * vertexSize + 2 * sizeof(Vector3));
						dest = colors[colorIndex % numColors].GetColor().ToUInt();
						
						currCopyNum = (currCopyNum + 1) % currCopyCount;
						if (currCopyNum == 0)
						{
							colorIndex++;
							copyIndex++;
							currCopyCount = copyCounts[copyIndex % numCopyCounts].GetInt();
							currCopyCount = Clamp(currCopyCount, 1, M_MAX_INT);
						}
					}

					vb->Unlock();
				}
			}
		}
	}
	

	outSolveInstance[0] = mdl;
}

