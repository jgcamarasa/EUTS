#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags
#include <stdio.h>

/* TODO 
- Interleave the data. For each vertex:
	- vertPos
	- vertUV
	- vertColor
- Commandline arguments

*/
// Format:
// 'E','U','T','S'
// 'M','E','S','H'
// numVerts		: unsigned int 
// vertPos		: float[3][numVerts]
// vertUV		: float[2][numVerts]
// vertColor	: float[4][numVerts]
// numFaces		: unsigned int
// indices		: unsigned int[3][numFaces]


char EUTS_ID[4] = { 'E', 'U', 'T', 'S' };
char MESH_ID[4] = { 'M', 'E', 'S', 'H' };

void vector3D2Array(float* arr, aiVector3D vec)
{
	arr[0] = vec.x;
	arr[1] = vec.y;
	arr[2] = vec.z;
}

void color4D2Array(float* arr, aiColor4D col)
{
	arr[0] = col.r;
	arr[1] = col.g;
	arr[2] = col.b;
	arr[2] = col.a;
}

int main()
{
	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile("cube.dae", aiProcess_MakeLeftHanded | aiProcess_FlipUVs);

	FILE* outputFile;
	fopen_s(&outputFile, "output.mesh", "wb");
	
	fwrite(EUTS_ID, sizeof(char), sizeof(EUTS_ID), outputFile);
	fwrite(MESH_ID, sizeof(char), sizeof(MESH_ID), outputFile);

	aiMesh *mesh = scene->mMeshes[0];

	// num vertex
	unsigned int numVerts = mesh->mNumVertices;
	fwrite(&numVerts, sizeof(unsigned int), 1, outputFile);

	// vertex positions (x, y, z)
	for (unsigned int i = 0; i < numVerts; ++i)
	{
		float vertex[3];
		vector3D2Array(vertex, mesh->mVertices[i]);
		fwrite(vertex, sizeof(vertex), 1, outputFile);
	}

	// texture coordinates (u, v)
	for (unsigned int i = 0; i < numVerts; ++i)
	{
		float coord[3];
		vector3D2Array(coord, mesh->mTextureCoords[0][i]);
		fwrite(coord, sizeof(float)*2, 1, outputFile);
	}

	// vertex color (r, g, b, a)
	for (unsigned int i = 0; i < numVerts; ++i)
	{
		float color[4];
		if (mesh->HasVertexColors(0))
		{
			color4D2Array(color, mesh->mColors[0][i]);
		}
		else
		{
			color[0] = 1.0f;
			color[1] = 1.0f;
			color[2] = 1.0f;
			color[3] = 1.0f;
		}
		
		fwrite(color, sizeof(color), 1, outputFile);
	}

	// Num faces
	unsigned int numFaces = mesh->mNumFaces;
	fwrite(&numFaces, sizeof(unsigned int), 1, outputFile);
	for (unsigned int i = 0; i < numFaces; ++i)
	{
		aiFace face = mesh->mFaces[i];
		assert(face.mNumIndices == 3);
		for (unsigned int j = 0; j < face.mNumIndices; j++)
		{
			fwrite(&(face.mIndices[j]), sizeof(unsigned int), 1, outputFile);
		}
	}
	

	fclose(outputFile);

	return 0;
}
