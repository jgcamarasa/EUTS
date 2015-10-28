#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

int main()
{
	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile("cube.dae", aiProcess_MakeLeftHanded | aiProcess_FlipUVs);

	return 0;
}
