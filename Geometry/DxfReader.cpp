#include "DxfReader.h"
#include <Urho3D/Core/StringUtils.h>
#include <Urho3D/IO/Log.h>

namespace
{
	int currLineNumber = 0;
	LinePair nextPair;
	File* source_;
}


DxfReader::DxfReader(Context* context, String path) : Object(context),
	forceYUp_(true)
{
	FileSystem* fs = new FileSystem(GetContext());
	bool res = fs->FileExists(path);

	//make sure that this file exists
	assert(res);

	//create the file
	source_ = new File(GetContext(), path, FILE_READ);

	//double check
	assert(source_);

	//create the log
	GetContext()->RegisterSubsystem(new Log(GetContext()));

}

DxfReader::DxfReader(Context* context, File* source) : Object(context),
	forceYUp_(true)
{
	bool res = source->GetMode() == FILE_READ ? true : false;

	//create the file
	source_ = source;

	//make sure that this file exists
	assert(res);

	//double check
	assert(source_);

	//create the log
	GetContext()->RegisterSubsystem(new Log(GetContext()));

}

LinePair DxfReader::GetNextLinePair()
{
	//LinePair nextPair;
	
	//initialize with error code:
	nextPair.first_ = -100; //use this as error since DXF has some negative codes...
	nextPair.second_ = Variant();

	//read the code
	if (!source_->IsEof())
	{
		nextPair.first_ = ToInt(source_->ReadLine().Trimmed());
	}

	//read the value
	if (!source_->IsEof())
	{
		nextPair.second_ = source_->ReadLine().Trimmed();
	}

	currLineNumber += 2;

	return nextPair;
}

bool DxfReader::Is(LinePair pair, int code, String name)
{
	bool res = false;

	if (pair.first_ == code && pair.second_ == name)
	{
		res = true;
	}

	return res;
}

bool DxfReader::IsType(LinePair pair, int code, VariantType type)
{
	bool res = false;

	if (pair.first_ == code && pair.second_.GetType() == type)
	{
		res = true;
	}

	return res;
}

bool DxfReader::IsEnd(LinePair pair)
{
	bool res = false;

	//check for actual file end
	if (pair.first_ == -100 || source_->IsEof())
	{
		res = true;
	}

	//check end of file return code
	if (pair.first_ == 0 && pair.second_ == "EOF")
	{
		res = true;
	}

	return res;
}

bool DxfReader::Parse()
{
	while (!source_->IsEof())
	{
		nextPair = GetNextLinePair();

		//debug
		//URHO3D_LOGINFO("Line Pair: " + String(nextPair.first_) + " : " + nextPair.second_.GetString());

		// blocks table - these 'build blocks' are later (in ENTITIES)
		// referenced an included via INSERT statements.
		if (Is(nextPair, 2, "BLOCKS")) {
			ParseBlocks();
			continue;
		}

		// primary entity table
		if (Is(nextPair, 2, "ENTITIES")) {
			ParseEntities();
			continue;
		}

		// skip unneeded sections entirely to avoid any problems with them
		// alltogether.
		else if (Is(nextPair, 2, "CLASSES") || Is(nextPair, 2, "TABLES")) {
			SkipSection();
			continue;
		}

		else if (Is(nextPair, 2, "HEADER")) {
			ParseHeader();
			continue;
		}

		// comments
		else if (nextPair.first_ == 999) {
		URHO3D_LOGINFO("DXF comment");
		}

		// don't read past the official EOF sign
		else if (IsEnd(nextPair)) {
			URHO3D_LOGINFO("---END OF DXF FILE---");
			break;
		}

	}

	return true;
}

void DxfReader::SkipSection()
{
	URHO3D_LOGINFO("Skipping section...");

	nextPair = GetNextLinePair();

	while (!IsEnd(nextPair) && !Is(nextPair, 0, "ENDSEC"))
	{
		nextPair = GetNextLinePair();
	}
}

void DxfReader::ParseHeader()
{
	URHO3D_LOGINFO("Parsing header...");

	SkipSection();
}

void DxfReader::ParseEntities()
{
	URHO3D_LOGINFO("Parsing entities...");

	nextPair = GetNextLinePair();

	//create a block and push it to list
	VariantMap block;
	block["000_TYPE"] = "BLOCK";
	block["Name"] = "$GENERIC_BLOCK_NAME";
	block["Insertions"] = VariantVector();
	block["Lines"] = VariantVector();
	blocks_.Push(block);

	//proceed
	while (!IsEnd(nextPair) && !Is(nextPair, 0, "ENDSEC")) {

		if (Is(nextPair, 0, "POLYLINE")) {
			ParsePolyLine();
			continue;
		}

		else if (Is(nextPair, 0, "INSERT")) {
			ParseInsertion();
			continue;
		}

		else if (Is(nextPair, 0, "POINT")) {
			ParsePoint();
			continue;
		}

		//parse these types
		else if (Is(nextPair, 0, "3DFACE") || Is(nextPair, 0, "LINE") || Is(nextPair, 0, "3DLINE")) {
			//http://sourceforge.net/tracker/index.php?func=detail&aid=2970566&group_id=226462&atid=1067632
			Parse3DFace();
			continue;
		}

		//recurse
		nextPair = GetNextLinePair();
	}
}

void DxfReader::ParseBlocks()
{
	URHO3D_LOGINFO("Parsing blocks...");

	nextPair = GetNextLinePair();

	//call individual block parsing loop
	while (!IsEnd(nextPair) && !Is(nextPair, 0, "ENDSEC")) {
		if (Is(nextPair, 0, "BLOCK")) {
			ParseBlock();
		}
		else {
			nextPair = GetNextLinePair();
		}
	}
}

void DxfReader::ParseBlock()
{
	URHO3D_LOGINFO("Parsing single block...");

	nextPair = GetNextLinePair();

	//we store all block info in variant map
	VariantMap block;
	//debug
	block["000_TYPE"] = "BLOCK";
	block["Insertions"] = VariantVector();
	block["Lines"] = VariantVector();
	blocks_.Push(block);

	//get reference to last block
	VariantMap* currBlock = blocks_.Back().GetVariantMapPtr();

	while (!IsEnd(nextPair) && !Is(nextPair, 0, "ENDBLK") && !Is(nextPair, 0, "ENDSEC")) {
		
		//get the info
		switch (nextPair.first_) {
		case 2:
			(*currBlock)["Name"] = nextPair.second_.GetString();
			break;
		case 10:
			(*currBlock)["Base_X"] = ToFloat(nextPair.second_.GetString());
			break;
		case 20:
			(*currBlock)["Base_Y"] = ToFloat(nextPair.second_.GetString());
			break;
		case 30:
			(*currBlock)["Base_Z"] = ToFloat(nextPair.second_.GetString());
			break;
		}

		//continue with parsing rest of content
		if (Is(nextPair, 0, "POLYLINE")) {
			ParsePolyLine();
			continue;
		}

		else if (Is(nextPair, 0, "POINT")) {
			ParsePoint();
			continue;
		}

		//skipping this case
		if (Is(nextPair, 0, "INSERT")) {
			URHO3D_LOGERROR("DXF: INSERT within a BLOCK not currently supported; skipping");
			while (!IsEnd(nextPair) && !Is(nextPair, 0, "ENDBLK"))
			{
				nextPair = GetNextLinePair();
			}
			break;
		}

		//parse these types
		else if (Is(nextPair, 0, "3DFACE") || Is(nextPair, 0, "LINE") || Is(nextPair, 0, "3DLINE")) {
			//http://sourceforge.net/tracker/index.php?func=detail&aid=2970566&group_id=226462&atid=1067632
			Parse3DFace();
			continue;
		}
	
		//recurse
		nextPair = GetNextLinePair();
	}
}

void DxfReader::ParseInsertion()
{
	URHO3D_LOGINFO("Parsing insertion...");

	nextPair = GetNextLinePair();

	//insertions are par to of the block structure
	VariantMap* currBlock = blocks_.Back().GetVariantMapPtr();

	//we store all insertion info in variant map
	VariantMap insertion;
	insertion["000_TYPE"] = "INSERTION";

	while (!IsEnd(nextPair) && !Is(nextPair, 0, "ENDBLK")) {

		//get the info
		switch (nextPair.first_) {
		case 2:
			insertion["Name"] = nextPair.second_.GetString();
			break;
			//translation
		case 10:
			insertion["Position_X"] = ToFloat(nextPair.second_.GetString());
			break;
		case 20:
			insertion["Position_Y"] = ToFloat(nextPair.second_.GetString());
			break;
		case 30:
			insertion["Position_Z"] = ToFloat(nextPair.second_.GetString());
			break;
			// scaling
		case 41:
			insertion["Scale_X"] = ToFloat(nextPair.second_.GetString());
			break;
		case 42:
			insertion["Scale_Y"] = ToFloat(nextPair.second_.GetString());
			break;
		case 43:
			insertion["Scale_Z"] = ToFloat(nextPair.second_.GetString());
			break;
			// rotation angle
		case 50:
			insertion["Angle"] = ToFloat(nextPair.second_.GetString());
			break;
		}

		//recurse
		nextPair = GetNextLinePair();
	}

	//done with parsing the insertion. Push to stack
	insertions_.Push(insertion);
}

void DxfReader::ParseLWPolyLine()
{
	URHO3D_LOGINFO("Parseing lwpolyline...");

	nextPair = GetNextLinePair();

	//get reference to last block
	VariantMap* currBlock = blocks_.Back().GetVariantMapPtr();

	//store lwpolyline data in variantmap
	VariantMap lwpolyline;
	lwpolyline["000_TYPE"] = "LWPOLYLINE";
	lwpolyline["Vertices"] = VariantVector();
	lwpolyline["Faces"] = VariantVector();

	while (!IsEnd(nextPair) && !Is(nextPair, 0, "ENDSEC")) {

		// vertex part omitted for now

		switch (nextPair.first_) {
		// Common Group Codes for Entities
		case 8:
			lwpolyline["Layer"] = nextPair.second_.GetString();
			break;
		// LWPolyLine codes
		case 100:
			lwpolyline["SubclassMarker"] = nextPair.second_.GetString();
			break;
		case 90:
			lwpolyline["NumVertices"] = ToUInt(nextPair.second_.GetString());
			break;
		case 70:
			lwpolyline["PolylineFlag"] = ToUInt(nextPair.second_.GetString());
			break;
		case 43:
			lwpolyline["ConstantWidth"] = ToUInt(nextPair.second_.GetString());
			break;
		case 38:
			lwpolyline["Elevantion"] = ToFloat(nextPair.second_.GetString());
			break;
		case 39:
			lwpolyline["Thickness"] = ToFloat(nextPair.second_.GetString());
			break;
		case 10:
			// Vertex coordinates (in OCS), multiple entries; ...
			break;
		case 20:
			// ?
			break;
		case 91:
			// Vertex identifier
			break;
		case 40:
			lwpolyline["StartingWidth"] = ToFloat(nextPair.second_.GetString());
			break;
		case 41:
			// Bulge (multiple entries; ...
			break;
		case 210:
			lwpolyline["ExtrusionDirection_X"] = ToFloat(nextPair.second_.GetString());
			break;
		case 220:
			lwpolyline["ExtrusionDirection_Y"] = ToFloat(nextPair.second_.GetString());
			break;
		case 230:
			lwpolyline["ExtrusionDirection_Z"] = ToFloat(nextPair.second_.GetString());
			break;
		default:
			break;
		}

		nextPair = GetNextLinePair();
	}
}

void DxfReader::ParsePolyLine()
{
	URHO3D_LOGINFO("Parsing polyline...");

	nextPair = GetNextLinePair();

	//get reference to last block
	VariantMap* currBlock = blocks_.Back().GetVariantMapPtr();

	//write type
	//(*currBlock)["000_TYPE"] = "Polyline";

	//store polyline line data in variantmap
	VariantMap polyline;
	polyline["000_TYPE"] = "POLYLINE";
	polyline["Vertices"] = VariantVector();
	polyline["Faces"] = VariantVector();

	while (!IsEnd(nextPair) && !Is(nextPair, 0, "ENDSEC")) {


		if (Is(nextPair, 0, "VERTEX")) {

			ParsePolyLineVertex(polyline);

			//not exactly sure what to do here...
			if (Is(nextPair, 0, "SEQEND")) {
				break;
			}


			continue;
		}

		switch (nextPair.first_)
		{
			// flags --- important that we know whether it is a
			// polyface mesh or 'just' a line.
		case 70:
			
			polyline["Flags"] = ToUInt(nextPair.second_.GetString());
			break;

			// optional number of vertices
		case 71:
			polyline["NumVerticesHint"] = ToUInt(nextPair.second_.GetString());
			break;

			// optional number of faces
		case 72:
			polyline["NumFacesHint"] = ToUInt(nextPair.second_.GetString());
			break;

			// 8 specifies the layer on which this line is placed on
		case 8:
			polyline["Layer"] = nextPair.second_.GetString();
			break;
		}

		//recurse
		nextPair = GetNextLinePair();
	}

	//if polyline has indices, then it is a mesh. Otherwise it is just a polyline
	bool hasIndices = polyline["Faces"].GetVariantVector().Size() > 3 ? true : false;
	if (hasIndices) {
		meshes_.Push(polyline);
	}
	else {
		polylines_.Push(polyline);
	}
}

void DxfReader::ParsePoint()
{
	URHO3D_LOGINFO("Parsing point...");

	nextPair = GetNextLinePair();

	//get reference to last block
	VariantMap* currBlock = blocks_.Back().GetVariantMapPtr();

	//write type
	//(*currBlock)["000_TYPE"] = "Polyline";

	//store polyline line data in variantmap
	VariantMap point;
	point["000_TYPE"] = "POINT";
	point["Position"] = Vector3();
	point["Layer"] = "Default";

	Vector3 v;

	while (!IsEnd(nextPair)) {

		if (nextPair.first_ == 0) { // SEQEND or another VERTEX
			break;
		}

		switch (nextPair.first_)
		{
		case 8:
			point["Layer"] = nextPair.second_.ToString();
			break;

		case 70:
			break;

			// VERTEX COORDINATES
		case 10:
			v.x_ = ToFloat(nextPair.second_.GetString());
			break;

		case 20:
			v.y_ = ToFloat(nextPair.second_.GetString());
			break;

		case 30:
			v.z_ = ToFloat(nextPair.second_.GetString());
			break;

			// POLYFACE vertex indices
		case 71:
		case 72:
		case 73:
		case 74:
			break;

			// color
		case 62:
			URHO3D_LOGERROR("Ignoring vertex color...");
			break;
		};

		//recurse
		nextPair = GetNextLinePair();
	}

	if (forceYUp_) {
		v = Vector3(v.x_, v.z_, v.y_);
	}


	point["Position"] = v;

	//push to list
	points_.Push(point);
}

void DxfReader::ParsePolyLineVertex(VariantMap& polyline)
{
	URHO3D_LOGINFO("Parsing polyline vertex...");

	nextPair = GetNextLinePair();

	unsigned int flags = 0;
	VariantVector indices;
	Vector3 v;

	while (!IsEnd(nextPair)) {

		if (nextPair.first_ == 0) { // SEQEND or another VERTEX
			break;
		}

		switch (nextPair.first_)
		{
		case 8:
			// layer to which the vertex belongs to - assume that
			// this is always the layer the top-level polyline
			// entity resides on as well.
			break;

		case 70:
			flags = ToUInt(nextPair.second_.GetString());
			break;

			// VERTEX COORDINATES
		case 10: 
			v.x_ = ToFloat(nextPair.second_.GetString());
			break;

		case 20: 
			v.y_ = ToFloat(nextPair.second_.GetString());
			break;

		case 30: 
			v.z_ = ToFloat(nextPair.second_.GetString());
			break;

			// POLYFACE vertex indices
		case 71:
		case 72:
		case 73:
		case 74:
			if (indices.Size() == 4) {
				URHO3D_LOGERROR("DXF: more than 4 indices per face not supported; ignoring");
				break;
			}
			indices.Push(ToUInt(nextPair.second_.GetString()) - 1);
			break;

			// color
		case 62:
			URHO3D_LOGERROR("Ignoring vertex color...");
			break;
		};

		//recurse
		nextPair = GetNextLinePair();
	}

	//add to vertex list
	if (indices.Size() == 0) {
		VariantVector* verts = polyline["Vertices"].GetVariantVectorPtr();
		assert(verts);

		if (forceYUp_) {
			v = Vector3(v.x_, v.z_, v.y_);
		}

		verts->Push(v);
	}

	//add to face list
	VariantVector* faces = polyline["Faces"].GetVariantVectorPtr();
	assert(faces);

	//adjust to triangles if necessary
	//this is kind of hacky...
	if (indices.Size() == 4) {
		if (indices[3].GetInt() == indices[2].GetInt()) {
			indices.Erase(3, 1);

			int a = indices[0].GetInt();
			int b = indices[1].GetInt();
			int c = indices[2].GetInt();

			indices[2] = b;
			indices[1] = c;
		}
		else {
			int a = indices[0].GetInt();
			int b = indices[1].GetInt();
			int c = indices[2].GetInt();
			int d = indices[3].GetInt();
			
			indices.Resize(6);
			indices[0] = a;
			indices[1] = d;
			indices[2] = b;

			indices[3] = d;
			indices[4] = c;
			indices[5] = b;
		}
	}
	else if(indices.Size() == 3)
	{
		int a = indices[0].GetInt();
		int b = indices[1].GetInt();
		int c = indices[2].GetInt();

		indices[2] = b;
		indices[1] = c;
	}

	//push
	if (indices.Size() == 3 || indices.Size() == 6) {
		faces->Push(indices);
	}

}

void DxfReader::Parse3DFace()
{
	URHO3D_LOGINFO("Parsing 3D face...");

	nextPair = GetNextLinePair();

	//get reference to last block
	VariantMap* currBlock = blocks_.Back().GetVariantMapPtr();

	//store polyline line data in variantmap
	VariantMap polyline;
	polyline["000_TYPE"] = "3DFACE";
	polyline["Vertices"] = VariantVector();
	polyline["Faces"] = VariantVector();

	//some data
	Vector3 vip[4];
	Color clr = Color::BLACK;
	bool b[4] = { false,false,false,false };

	while (!IsEnd(nextPair)) {

		// next entity with a groupcode == 0 is probably already the next vertex or polymesh entity
		if (nextPair.first_ == 0) {
			break;
		}
		switch (nextPair.first_)
		{

			// 8 specifies the layer
		case 8:
			polyline["Layer"] = nextPair.second_.GetString();
			break;

			// x position of the first corner
		case 10: 
			vip[0].x_ = ToFloat(nextPair.second_.GetString());
			b[2] = true;
			break;

			// y position of the first corner
		case 20: 
			vip[0].y_ = ToFloat(nextPair.second_.GetString());
			b[2] = true;
			break;

			// z position of the first corner
		case 30: 
			vip[0].z_ = ToFloat(nextPair.second_.GetString());
			b[2] = true;
			break;

			// x position of the second corner
		case 11: 
			vip[1].x_ = ToFloat(nextPair.second_.GetString());
			b[3] = true;
			break;

			// y position of the second corner
		case 21: 
			vip[1].y_ = ToFloat(nextPair.second_.GetString());
			b[3] = true;
			break;

			// z position of the second corner
		case 31: 
			vip[1].z_ = ToFloat(nextPair.second_.GetString());
			b[3] = true;
			break;

			// x position of the third corner
		case 12:
			vip[2].x_ = ToFloat(nextPair.second_.GetString());
			b[0] = true;
			break;

			// y position of the third corner
		case 22: 
			vip[2].y_ = ToFloat(nextPair.second_.GetString());
			b[0] = true;
			break;

			// z position of the third corner
		case 32: 
			vip[2].z_ = ToFloat(nextPair.second_.GetString());
			b[0] = true;
			break;

			// x position of the fourth corner
		case 13: 
			vip[3].x_ = ToFloat(nextPair.second_.GetString());
			b[1] = true;
			break;

			// y position of the fourth corner
		case 23: 
			vip[3].y_ = ToFloat(nextPair.second_.GetString());
			b[1] = true;
			break;

			// z position of the fourth corner
		case 33: 
			vip[3].z_ = ToFloat(nextPair.second_.GetString());
			b[1] = true;
			break;

			// color
		case 62:
			break;
		};

		//recurse
		nextPair = GetNextLinePair();
	}

	//fill the data
	VariantVector pVerts;
	pVerts.Resize(4);

	for (int i = 0; i < 4; i++)
	{
		if (forceYUp_) {
			pVerts[i] = Vector3(vip[i].x_, vip[i].z_, vip[i].y_);
		}
		else {
			pVerts[i] = vip[i];
		}

	}

	polyline["Vertices"] = pVerts;

	//push to list
	polylines_.Push(polyline);
}




