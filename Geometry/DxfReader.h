#include <Urho3D/Core/Context.h>
#include <Urho3D/Core/Object.h>
#include <Urho3D/Container/Vector.h>
#include <Urho3D/Container/Str.h>
#include <Urho3D/Core/Variant.h>
#include <Urho3D/IO/Deserializer.h>
#include <Urho3D/IO/File.h>
#include <Urho3D/IO/FileSystem.h>

using namespace Urho3D;

typedef Pair<int, Variant> LinePair;

URHO3D_API class DxfReader : public Object
{
	URHO3D_OBJECT(DxfReader, Object);

public:
	DxfReader(Context* context, String path);
	DxfReader(Context* context, File* source);
	~DxfReader() {};

	/**************************************************************************
	Dxf info comes in pairs of lines, eg:
	 ---- 0         <- code id
	 ---- HEADER    <- value for this code 

	 NOTE: a code can have more than one value, therefore the full pair must be considered.

	This is the most basic example, other codes have more complex values.
	Therefore, we define a method that reads in pairs of lines.
	***************************************************************************/
	LinePair GetNextLinePair();

	//main loop for parsing
	bool Parse();

	//individual parsers
	void SkipSection();
	void ParseHeader();
	void ParseEntities();
	void ParseBlocks();
	void ParseBlock();
	void ParseInsertion();
	void ParsePolyLine();
	void ParseLWPolyLine();
	void ParsePolyLineVertex(VariantMap& polyline);
	void ParsePoint();
	void Parse3DFace();

	//some helpers
	bool Is(LinePair pair, int code, String name);
	bool IsEnd(LinePair pair);
	bool IsType(LinePair pair, int code, VariantType type);

	//getters
	VariantVector GetBlocks() { return blocks_; };
	VariantVector GetInsertions() { return insertions_; };
	VariantVector GetMeshes() { return meshes_; };
	VariantVector GetPolylines() { return polylines_; };
	VariantVector GetPoints() { return points_; };

	void SetForceYUp(bool yUp) { forceYUp_ = yUp; }
	bool GetForceYUp() { return forceYUp_; }

protected:

	bool forceYUp_;

	//Blocks are logical chunks of a drawing (dxf) file.
	//Often, they just define base points for model space, paper space by specifying a base point, scale.
	//However, they CAN have entitites (i.e. polylines, points, etc) embedded in them. I have not seen this in any test files,
	//but it is allowed. We don't support it currently.
	VariantVector blocks_;

	//Insertions seem to be a short way to specify an instance of an entity with pos,rot, and scale.
	//I think they are appended to entities (or blocks) but I am not sure. I haven't seen one yet.
	VariantVector insertions_;

	//These are the things we want. 
	VariantVector meshes_;
	VariantVector polylines_;
	VariantVector points_;

};