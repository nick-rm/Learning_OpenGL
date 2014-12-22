#include <glm.hpp>

// Max of possible vertices an indices
const int buffermax=16384;
//Geometry Buffer structure
struct GBuffer;

//Class for Meshes loading and stortage
class MeshFromObj
{
	public:
	MeshFromObj(char * fname1, char * fname2);
	void Draw();
	~MeshFromObj();

	protected:
	unsigned int uiVBO[5];  // Four buffers for position1, positionDiff, normal, texcoord and one index buffer
	unsigned int uiVAO[1];  // Generalized buffer
	int						    verticesCount;
	int						    indicesCount;
	bool						meshloaded;
	void LoadMeshFromObj(char * fname, GBuffer & buffer);
	bool CreateVertexAndIndexBuffers(GBuffer & buffer1, GBuffer & buffer2);
};