#include <iostream>
#include "mesh.h"

// By: Aramina Lee

// calculate determinant of 3x3 matrix
// v0, v1, v2 are column vectors
float calcDet3x3(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2)
{
	return
		v0[0] * (v1[1] * v2[2] - v2[1] * v1[2]) +
		v1[0] * (v2[1] * v0[2] - v0[1] * v2[2]) +
		v2[0] * (v0[1] * v1[2] - v1[1] * v0[2]);
}

/*
 * Mesh constructor
 *
 * @param glm::vec3 point - vector3 point
 * @param const char* meshFile - mesh file name or NULL if no file
 * @param ofColor diffuse - color for diffuse reflection in Lambertian shading
 */
Mesh::Mesh(glm::vec3 p, const char* meshFile, ofColor diffuse)
{
	position = p;
	diffuseColor = diffuse;
	if( meshFile == NULL )
		loadMesh();
	else
		loadFile(meshFile);
	calcNormal();
}

// load a simple pyramid mesh when there is no mesh file
void Mesh::loadMesh()
{
	// Create vertices
	verts.push_back(position + glm::vec3(-1, 0, 1));
	verts.push_back(position + glm::vec3(1, 0, 1));
	verts.push_back(position + glm::vec3(1, 0, -1));
	verts.push_back(position + glm::vec3(-1, 0, -1));
	verts.push_back(position + glm::vec3(0, 3, 0));

	// Create index triangles
	// base
	tInd.push_back(glm::ivec3(2, 1, 0));
	tInd.push_back(glm::ivec3(3, 2, 0));
	// sides
	tInd.push_back(glm::ivec3(0, 1, 4));
	tInd.push_back(glm::ivec3(1, 2, 4));
	tInd.push_back(glm::ivec3(2, 3, 4));
	tInd.push_back(glm::ivec3(3, 0, 4));
}

/*
 * Load mesh from Wavefront .obj file
 *
 * @param const char* fname- mesh file name
 */
void Mesh::loadFile(const char* fname)
{
	ofFile file;

	file.open(ofToDataPath(fname), ofFile::ReadWrite, false);
	ofBuffer buff = file.readToBuffer();

	for (auto line : buff.getLines())
	{
		istringstream lineSS(line);
		string lineType;
		lineSS >> lineType;

		if (lineType == "v")	// if vertex
		{
			float x, y, z, w = 1;
			lineSS >> x >> y >> z >> w;
			verts.push_back(position + glm::vec3(x, y, z)); // ignoring w for now
		}
		else if (lineType == "vt")	// else if texture
		{
			// ignore for now
			// float u, v, w;
			// lineSS >> u >> v >> w;
		}
		else if (lineType == "vn")	// else if normal
		{
			// float i, j, k;
			// lineSS >> i >> j >> k;
		}
		else if (lineType == "f")	// else if face
		{
			vector <int> vInd;	// vertex indices
			vector <int> vtInd;	// texture indices
			vector <int> vnInd;	// normal indices

			string refStr;
			while (lineSS >> refStr)
			{
				istringstream ref(refStr);	// turn string into an input string stream
				string vStr, vtStr, vnStr;
				getline(ref, vStr , '/');
				getline(ref, vtStr, '/');
				getline(ref, vnStr, '/');
				int v  = atoi(vStr .c_str()) - 1;	// read vertex index, convert from 1 base indexing to 0
				int vt = atoi(vtStr.c_str()) - 1;	// read texture index, convert from 1 base indexing to 0
				int vn = atoi(vnStr.c_str()) - 1;	// read normal index, convert from 1 base indexing to 0

				vInd .push_back(v);				// store vertex index
				vtInd.push_back(vt);			// store texture index
				vnInd.push_back(vn);			// store normal index
			}
			// only keep first three vertices for now
			// TODO: for polygons larger than triangles, triangulate
			// check at least 3 vertices
			if (vInd.size() < 3)
			{
				cout << "error face has " << vInd.size() << " < 3 vertices" << endl;
				continue;
			}
			tInd.push_back(glm::ivec3(vInd[0], vInd[1], vInd[2]));
		}
		else if (lineType == "l")	// else if line
		{
			// ignore for now
		}
		else if (lineType == "" || lineType == "#")	// else if empty line or comment
		{
			// ignore
		}
		else cout << "unknown line type: " << lineType << endl;
	}
}

/*
 * Print statistics of mesh
 */
void Mesh::printStats()
{
	cout << "Number of vertices: " << verts.size() << endl;
	cout << "Number of faces: " << tInd.size() << endl;
	cout << "Mesh size: " << ((verts.size() * sizeof(glm::vec3) + tInd.size() * sizeof(glm::ivec3)) / 1024) << " KB" << endl;
}

// calculate centroids and normals for every triangle in the mesh
// centroids are stored in the class vector tCentroid
// normals are stored in the class vector tNormal
// the mesh must be loaded ahead of time in class vectors verts and tInd
void Mesh::calcNormal()
{
	for (auto tri : tInd)
	{
		glm::vec3 v0 = verts[tri[0]];
		glm::vec3 v1 = verts[tri[1]];
		glm::vec3 v2 = verts[tri[2]];
		// calculate centroid of triangle
		tCentroid.push_back(((float)1.0 / 3) * (v0 + v1 + v2));

		glm::vec3 e1 = v1 - v0;
		glm::vec3 e2 = v2 - v1;
		// calculate normal direction
		tNormal.push_back(glm::cross(e1, e2));
	}
}

// draw every triangle in the mesh using ofDrawTriangle
void Mesh::draw()
{
	// draw each triangle
	for (int i = 0; i < tInd.size(); i++)
		ofDrawTriangle(verts[tInd[i][0]], verts[tInd[i][1]], verts[tInd[i][2]]);
}

/*
 * Intersect Ray with Mesh
 *
 * @param const Ray& ray - given ray
 * @param glm::vec3& point - vector3 point
 * @param glm::vec3& - normal Intersection
 * @return bool - true if ray intersects mesh, false if no intersection
 */
bool Mesh::intersect(const Ray& ray, glm::vec3& point, glm::vec3& normal)
{
	// parameters for finding the intersection of the ray with closest surface of the pyramid
	float tBest = numeric_limits<float>::max();		// best (min) t-value
	float betaBest = 0;	// beta for best ray triangle intersection
	float gammaBest = 0; // gamma for best ray triangle intersection
	size_t iTriBest = 0; // index of tInd for best triangle

	// for each of the triangles in the mesh
	for (size_t i = 0; i < tInd.size(); i++)
	{
		// draw the triangle
		glm::vec3 v0 = verts[tInd[i][0]];
		glm::vec3 v1 = verts[tInd[i][1]];
		glm::vec3 v2 = verts[tInd[i][2]];

		// determine if the ray intersects the triangle
		glm::vec3 c0 = v0 - v1;
		glm::vec3 c1 = v0 - v2;
		glm::vec3 c2 = ray.d;
		glm::vec3 c3 = v0 - ray.p;

		// use Cramer's Rule to solve for the intersection
		float dt = calcDet3x3(c0, c1, c2);		// determinant
		if (dt == 0)
			continue; // no solution to intersection so skip this triangle
		float dx = calcDet3x3(c3, c1, c2);
		float dy = calcDet3x3(c0, c3, c2);
		float dz = calcDet3x3(c0, c1, c3);

		float beta = dx / dt;
		float gamma = dy / dt;
		float t = dz / dt;

		// Check for intersection inside triangle
		if (beta < 0 || gamma < 0 || beta + gamma > 1)
			continue;	// intersection outside triangle so skip

		// if the intersection is closer than the previous best, then save it
		if (t < tBest)
		{
			tBest = t;
			betaBest = beta;
			gammaBest = gamma;
			iTriBest = i;
		}
	}

	// if there is an intersection between ray and mesh
	if (tBest < numeric_limits<float>::max())
	{
		// calculate position of intersection
		glm::vec3 v0 = verts[tInd[iTriBest][0]];
		glm::vec3 v1 = verts[tInd[iTriBest][1]];
		glm::vec3 v2 = verts[tInd[iTriBest][2]];
		point = v0 + betaBest * (v1 - v0) + gammaBest * (v2 - v0);
		normal = tNormal[iTriBest];
		return true;
	}

	return false;	// no intersection found
}

