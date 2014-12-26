#include <Windows.h>
#include <fstream>
using namespace std;
#include <glm.hpp>
#include <glew.h>
#include "meshobj.h"

//Structure for Vertices storage
struct VERTEX
{
    glm::vec3 position;
	glm::vec3 normal;
    glm::vec2 texcoord;
};

//Temporary Geometry Buffer for vertices and indices storage
struct GBuffer {
    VERTEX vertices[buffermax];
	glm::vec3 bPosition[buffermax];
	glm::vec3 bPositionDif[buffermax];
	glm::vec3 bNormal[buffermax];
	glm::vec2 bTexcoord[buffermax];
    unsigned int  indices[buffermax];
    int    verticesI;
    int    indicesI;
} buffer1, buffer2;

MeshFromObj::MeshFromObj(wchar_t * fname1, wchar_t * fname2)
{
	meshloaded=false;
	ZeroMemory(&buffer1,sizeof(buffer1));
	ZeroMemory(&buffer2,sizeof(buffer2));
	LoadMeshFromObj(fname1, buffer1); if (buffer1.verticesI==0) return;
	LoadMeshFromObj(fname2, buffer2); if (buffer2.verticesI==0) return;
	CreateVertexAndIndexBuffers(buffer1, buffer2);
	meshloaded=true;
}

MeshFromObj::~MeshFromObj()
{
    glDeleteBuffers(5, uiVBO);	
	glDeleteVertexArrays(1, uiVAO);
}

//Creation of the vertex and index buffers
bool MeshFromObj::CreateVertexAndIndexBuffers(GBuffer & buffer1, GBuffer & buffer2)
{
	//Copying the Difference Position Buffer
	for (int i=0; i<buffermax; i++)
	{
		buffer1.bPositionDif[i]=buffer2.bPosition[i]-buffer1.bPosition[i];
	}
	
	//Determaning the numbers of vertices and indices
   verticesCount=buffer1.verticesI;
   indicesCount=buffer1.indicesI;

   glGenVertexArrays(1, uiVAO); // Generalized buffer
   glGenBuffers(5, uiVBO);		// Four buffers for position1, positionDiff, normal, texcoord and one index buffer

   // Copying data of buffers to the generalized buffer
   glBindVertexArray(uiVAO[0]); 

   glBindBufferARB(GL_ARRAY_BUFFER, uiVBO[0]); 
   glBufferDataARB(GL_ARRAY_BUFFER, 3*verticesCount*sizeof(float), buffer1.bPosition, GL_STATIC_DRAW); 
   glEnableVertexAttribArrayARB(0); 
   glVertexAttribPointerARB(0, 3, GL_FLOAT, GL_FALSE, 0, 0); 

   glBindBufferARB(GL_ARRAY_BUFFER, uiVBO[1]); 
   glBufferDataARB(GL_ARRAY_BUFFER, 3*verticesCount*sizeof(float), buffer1.bPositionDif, GL_STATIC_READ); 
   glEnableVertexAttribArrayARB(1); 
   glVertexAttribPointerARB(1, 3, GL_FLOAT, GL_FALSE, 0, 0); 

   glBindBufferARB(GL_ARRAY_BUFFER, uiVBO[2]); 
   glBufferDataARB(GL_ARRAY_BUFFER, 3*verticesCount*sizeof(float), buffer1.bNormal, GL_STATIC_DRAW); 
   glEnableVertexAttribArrayARB(2); 
   glVertexAttribPointerARB(2, 3, GL_FLOAT, GL_FALSE, 0, 0); 

   glBindBufferARB(GL_ARRAY_BUFFER, uiVBO[3]); 
   glBufferDataARB(GL_ARRAY_BUFFER, 2*verticesCount*sizeof(float), buffer1.bTexcoord, GL_STATIC_DRAW); 
   glEnableVertexAttribArrayARB(3); 
   glVertexAttribPointerARB(3, 2, GL_FLOAT, GL_FALSE, 0, 0); 

   // Copying of index buffer
   glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER, uiVBO[4]); 
   glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER, indicesCount*sizeof(unsigned int), buffer1.indices, GL_STATIC_DRAW); 

   return true;

}

void MeshFromObj::Draw()
{
    if(meshloaded)
	{
	glBindVertexArray(uiVAO[0]);
	glDrawElements(GL_TRIANGLES,indicesCount,GL_UNSIGNED_INT,0);
	}
}

//Adding of the vertex
int AddVertex(VERTEX vertex, GBuffer & buffer)
{
	int res=-1;
	//Searching the existing vertex
	for (int i=0; i<buffer.verticesI; i++)
		if (memcmp(&buffer.vertices[i],&vertex,sizeof(VERTEX))==0) res=i;
	//Adding the vertex
	if (res<0) {
		buffer.vertices[buffer.verticesI++]=vertex; res=buffer.verticesI-1; }
	return res;
}

//Adding of the index
void AddIndex(int index, GBuffer & buffer)
{
    buffer.indices[buffer.indicesI++]=index;
}

//Loading Geometry data from .obj file
void MeshFromObj::LoadMeshFromObj(wchar_t * fname, GBuffer & buffer)
{
    buffer.verticesI=0;
    buffer.indicesI=0;

    //Temporary storage for data
    glm::vec3 * Positions=(glm::vec3*)malloc(buffermax*sizeof(glm::vec3));
    glm::vec2 * TexCoords=(glm::vec2*)malloc(buffermax*sizeof(glm::vec2));
    glm::vec3 * Normals=(glm::vec3*)malloc(buffermax*sizeof(glm::vec3));

    //Arrays' indices
    int PositionsI=0;
    int TexCoordsI=0;
    int NormalsI=0;

    //Data input from file
    WCHAR strCommand[256] = {0};
    wifstream InFile( fname );

    if( !InFile ) return;

    for(; ; )
    {
        InFile >> strCommand;
        if( !InFile )
            break;

        if( 0 == wcscmp( strCommand, L"#" ) )
        {
            //Comment
        }
        else if( 0 == wcscmp( strCommand, L"v" ) )
        {
            //Coordinates
            float x, y, z;
            InFile >> x >> y >> z;
            Positions[PositionsI++]=glm::vec3(x,y,z)*0.05f;
        }
        else if( 0 == wcscmp( strCommand, L"vt" ) )
        {
            //Texture coordinates
            float u, v;
            InFile >> u >> v;
			TexCoords[TexCoordsI++]=glm::vec2(u,v);
        }
        else if( 0 == wcscmp( strCommand, L"vn" ) )
        {
            //Normals
            float x, y, z;
            InFile >> x >> y >> z;
            Normals[NormalsI++]=glm::vec3(x,y,z);
        }
        else if( 0 == wcscmp( strCommand, L"f" ) )
        {
            //Faces
            UINT iPosition, iTexCoord, iNormal;
            VERTEX vertex;

            for( UINT iFace = 0; iFace < 3; iFace++ )
            {
                ZeroMemory( &vertex, sizeof( VERTEX ) );

                InFile >> iPosition;
                vertex.position = Positions[ iPosition - 1 ];

                if( '/' == InFile.peek() )
                {
                    InFile.ignore();

                    if( '/' != InFile.peek() )
                    {
                        //Texture coordinates
                        InFile >> iTexCoord;
                        vertex.texcoord = TexCoords[ iTexCoord - 1 ];
                    }

                    if( '/' == InFile.peek() )
                    {
                        InFile.ignore();

                        //Normals
                        InFile >> iNormal;
                        vertex.normal = Normals[ iNormal - 1 ];
                    }
                }

                //Adding of vertex and index
                int index=AddVertex( vertex, buffer );
                AddIndex( index, buffer );

            }
		}
    }
    InFile.close();

    //Cleaning up of temporary arrays
    free(Positions);
    free(TexCoords);
    free(Normals);

	//Copying of buffers
	for (int i=0; i<buffermax; i++)
	{
		buffer.bPosition[i]=buffer.vertices[i].position;
		buffer.bNormal[i]=buffer.vertices[i].normal;
		buffer.bTexcoord[i]=buffer.vertices[i].texcoord;
	}
}