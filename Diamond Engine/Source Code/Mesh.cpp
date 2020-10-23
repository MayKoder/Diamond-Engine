#include "Mesh.h"
#include "OpenGL.h"
#include "MeshArrays.h"

//TODO: A mesh should be loaded only once, if the mesh is loaded, just send the 
//id's to the new mesh
Mesh::Mesh() : indices_id(-1), vertices_id(-1), generalWireframe(nullptr)
{

}

Mesh::~Mesh()
{
	//Clear buffers (Should not happen on Mesh components)
	if (indices_id != -1)
		glDeleteBuffers(1, &indices_id);

	if (vertices_id != -1)
		glDeleteBuffers(1, &vertices_id);

	if (normalbuffer_id != -1)
		glDeleteBuffers(1, &normalbuffer_id);

	if (texCoords_id != -1)
		glDeleteBuffers(1, &texCoords_id);

	//Clear buffers
	if(indices != nullptr)
		delete[] indices;

	if (vertices != nullptr)
		delete[] vertices;

	if (normals != nullptr)
		delete[] normals;

	if (texCoords != nullptr)
		delete[] texCoords;

	//Restart to nullptr
	indices = nullptr;
	vertices = nullptr;
	normals = nullptr;
	texCoords = nullptr;

}

void Mesh::SetAsCube()
{
	//ERROR: This will crash on cleanup, you cant delete a static array element pointer
	//indices_count = sizeof(MA_Cube_Indices) / sizeof(int);
	//indices = (uint*)&MA_Cube_Indices[0];

	//vertices_count = (sizeof(MA_Cube_Vertices) / sizeof(float)) / 3;
	//vertices = (float*)&MA_Cube_Vertices[0];

	//normals_count = (sizeof(MA_Cube_Normals) / sizeof(float)) / 3;
	//normals = (float*)&MA_Cube_Normals[0];

	//texCoords_count = (sizeof(MA_Cube_TexCoords) / sizeof(float)) / 2;
	//texCoords = (float*)&MA_Cube_TexCoords[0];

	//GenBuffers();

	GenerateSphere(1, 30, 30);
	GenBuffers();

}

void Mesh::SetAsPyramid()
{


}

void Mesh::SetAsSphere()
{
	//createSphere(vertex, index, 1, 20, 20);
}

void Mesh::SetAsCylinder()
{
}

void Mesh::GenBuffers()
{


	// vertices_count = vector3's // size of the array (elements) = vertices_count * 3 // size of the array in bytes = sizeof(float) * vertices_count * 3
	if (vertices_count != 0)
	{
		glGenBuffers(1, (GLuint*)&(vertices_id));
		glBindBuffer(GL_ARRAY_BUFFER, vertices_id);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices_count * 3, &vertices[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		//glVertexPointer(3, GL_FLOAT, 0, NULL);
	}

	if (indices_count != 0)
	{
		// indices_count = elements // size of the array (elements) = indices_count // size of the array in bytes = sizeof(uint) * indices_count
		glGenBuffers(1, (GLuint*)&(indices_id));
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_id);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * indices_count, &indices[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	if (normals_count != 0)
	{
		//Normals buffer
		glGenBuffers(1, (GLuint*)&(normalbuffer_id));
		glBindBuffer(GL_ARRAY_BUFFER, normalbuffer_id);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * normals_count * 3, &normals[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		//glEnableVertexAttribArray(2);
		//glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	}

	if (texCoords_count != 0)
	{
		//Normals buffer
		glGenBuffers(1, (GLuint*)&texCoords_id);
		glBindBuffer(GL_ARRAY_BUFFER, texCoords_id);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * texCoords_count * 2, texCoords, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		//glEnableVertexAttribArray(2);
		//glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	}



	//WARNING: Maybe delete buffers here? destroys normals debug
	//delete[] indices;
	//delete[] vertices;
	//delete[] normals;
	//delete[] texCoords;

	//indices = nullptr;
	//vertices = nullptr;
	//normals = nullptr;
	//texCoords = nullptr;
}

void Mesh::RenderMesh()
{
	//ASK: glDrawElementsInstanced()?
	glBindTexture(GL_TEXTURE_2D, textureID);

	//Vertices --------------------------------------------
	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, vertices_id);
	glVertexPointer(3, GL_FLOAT, 0, NULL);
	//--------------------------------------------


	//TexCoords --------------------------------------------
	if (texCoords_count != 0)
	{
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glBindBuffer(GL_ARRAY_BUFFER, texCoords_id);
		glTexCoordPointer(2, GL_FLOAT, 0, NULL);
		//glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
		//glEnableVertexAttribArray(1);
	}
	//--------------------------------------------


	//Normals --------------------------------------------
	if (normals_count != 0)
	{
		glEnableClientState(GL_NORMAL_ARRAY);
		glBindBuffer(GL_ARRAY_BUFFER, normalbuffer_id);
		glNormalPointer(GL_FLOAT, 0, NULL);
		//glEnableVertexAttribArray(2);
		//glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	}
	//--------------------------------------------

	//TODO: Make a buffer for the colors and try this
	//glBindBuffer(GL_ARRAY_BUFFER, m_colorBuffer);
	//glVertexAttribPointer((GLint)1, 3, GL_FLOAT, GL_TRUE, 0, (void*)0);

	//Indices --------------------------------------------
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_id);
	//--------------------------------------------

	//Drawing --------------------------------------------
	glDrawElements(GL_TRIANGLES, indices_count, GL_UNSIGNED_INT, NULL);
	//--------------------------------------------

	////Wireframe temporal --------------------------------------------
	if(generalWireframe != nullptr && *generalWireframe == false)
	{
		glColor3f(0, 1, 0);
		glScalef(1.0001f, 1.0001f, 1.0001f);
		glLineWidth(2.5f);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDrawElements(GL_TRIANGLES, indices_count, GL_UNSIGNED_INT, NULL);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glScalef(1.f, 1.f, 1.f);
		glLineWidth(1.f);
		glColor3f(1, 1, 1);
	}
	////--------------------------------------------

	//Drawing cleanup --------------------------------------------
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDisableClientState(GL_VERTEX_ARRAY);

	if (normals_count != 0)
		glDisableClientState(GL_NORMAL_ARRAY);
	if (texCoords_count != 0)
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	//--------------------------------------------

	glPointSize(3.0f);
	glColor3f(1, 0, 0);
	glBegin(GL_POINTS);
	for (unsigned int i = 0; i < vertices_count * 3; i += 3)
	{
		glVertex3f(vertices[i], vertices[i + 1], vertices[i + 2]);
	}
	glEnd();
	glColor3f(0, 1, 0);
	glPointSize(1.0f);

	//Vertex normals
	glColor3f(0, 1, 0);
	glBegin(GL_LINES);
	float normalLenght = 0.05f;
	for (unsigned int i = 0; i < normals_count * 3; i += 3)
	{
		glVertex3f(vertices[i], vertices[i + 1], vertices[i + 2]);
		glVertex3f(vertices[i] + normals[i] * normalLenght, vertices[i + 1] + normals[i + 1] * normalLenght, vertices[i + 2] + normals[i + 2] * normalLenght);
	}
	glEnd();
	glColor3f(1, 1, 1);

	//Face normals
	glColor3f(1, 0, 0);
	//glBegin(GL_LINES);
	//for (int i = 0; i < indices_count; i += 3)
	//{
	//	//MaykMath::FindCentroid(&vertices[indices[i]], &vertices[indices[i + 1]], &vertices[indices[i + 2]], &middle.x);
	//	vec3 A = GetVectorFromIndex(&vertices[indices[i] * 3]);

	//	vec3 B = GetVectorFromIndex(&vertices[indices[i + 1] * 3]);

	//	vec3 C = GetVectorFromIndex(&vertices[indices[i + 2] * 3]);

	//	vec3 middle((A.x + B.x + C.x) / 3.f, (A.y + B.y + C.y) / 3.f, (A.z + B.z + C.z) / 3.f);

	//	vec3 crossVec = cross((B - A), (C - A));
	//	vec3 normalDirection = normalize(crossVec);

	//	//LOG("%f, %f, %f", middle.x, middle.y, middle.z);
	//	glVertex3f(middle.x, middle.y, middle.z);
	//	glVertex3f(middle.x + normalDirection.x * normalLenght, middle.y + normalDirection.y * normalLenght, middle.z + normalDirection.z * normalLenght);
	//}
	//glEnd();
	//glPointSize(1.f);
	glColor3f(1, 1, 1);


}

vec3 Mesh::GetVectorFromIndex(float* startValue)
{
	float x = *startValue;
	++startValue;
	float y = *startValue;
	++startValue;
	float z = *startValue;

	return vec3(x, y, z);
}

void Mesh::GenerateSphere(float radius, float sectorCount, float stackCount)
{
	// clear memory of prev arrays
	std::vector<float> _vertices;
	std::vector<float> _normals;
	std::vector<float> _texCoords;

	int selectorTracker = 0;

	vertices_count = normals_count = ((sectorCount + 1) * (stackCount + 1)); //Numero de elements, cal treure el *3?
	texCoords_count = ((sectorCount + 1) * (stackCount + 1)); //Numero de elements, cal treure el *2?
	indices_count = (((stackCount - 2) * (sectorCount * 2)) * 3) + ((2 * sectorCount) * 3);


	indices = new uint[indices_count];
	vertices = new float[vertices_count * 3];
	normals = new float[normals_count * 3];
	texCoords = new float[texCoords_count * 2];


	float x, y, z, xy;                              // vertex position
	float nx, ny, nz, lengthInv = 1.0f / radius;    // vertex normal
	float s, t;                                     // vertex texCoord

	float sectorStep = 2 * M_PI / sectorCount;
	float stackStep = M_PI / stackCount;
	float sectorAngle, stackAngle;

	for (int i = 0; i <= stackCount; ++i)
	{
		stackAngle = M_PI / 2.f - i * stackStep;        // starting from pi/2 to -pi/2
		xy = radius * cosf(stackAngle);             // r * cos(u)
		z = radius * sinf(stackAngle);              // r * sin(u)

		// add (sectorCount+1) vertices per stack
		// the first and last vertices have same position and normal, but different tex coords
		for (int j = 0; j <= sectorCount; ++j)
		{
			sectorAngle = j * sectorStep;           // starting from 0 to 2pi

			// vertex position (x, y, z)
			x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
			y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)
			_vertices.push_back(x);
			_vertices.push_back(y);
			_vertices.push_back(z);

			//vertices[selectorTracker] = x;
			//selectorTracker++;
			//vertices[selectorTracker] = y;
			//selectorTracker++;
			//vertices[selectorTracker] = z;
			//selectorTracker++;


			// normalized vertex normal (nx, ny, nz)
			nx = x * lengthInv;
			ny = y * lengthInv;
			nz = z * lengthInv;
			_normals.push_back(nx);
			_normals.push_back(ny);
			_normals.push_back(nz);

			// vertex tex coord (s, t) range between [0, 1]
			s = (float)j / sectorCount;
			t = (float)i / stackCount;
			_texCoords.push_back(s);
			_texCoords.push_back(t);
		}
	}

	// generate CCW index list of sphere triangles
	std::vector<uint> _indices;
	int k1, k2;
	for (int i = 0; i < stackCount; ++i)
	{
		k1 = i * (sectorCount + 1);     // beginning of current stack
		k2 = k1 + sectorCount + 1;      // beginning of next stack

		for (int j = 0; j < sectorCount; ++j, ++k1, ++k2)
		{
			// 2 triangles per sector excluding first and last stacks
			// k1 => k2 => k1+1
			if (i != 0)
			{
				_indices.push_back(k1);
				_indices.push_back(k2);
				_indices.push_back(k1 + 1);
			}

			// k1+1 => k2 => k2+1
			if (i != (stackCount - 1))
			{
				_indices.push_back(k1 + 1);
				_indices.push_back(k2);
				_indices.push_back(k2 + 1);
			}
		}
	}

	//for (int i = 0; i < vertices_count * 3; i++)
	//{
	//	vertices[i] = _vertices[i];
	//	normals[i] = _normals[i];
	//}
	//for (int i = 0; i < texCoords_count * 2; i++)
	//{
	//	texCoords[i] = _texCoords[i];
	//}


	memcpy(vertices, &_vertices[0], sizeof(float) * vertices_count * 3);
	memcpy(normals, &_normals[0], sizeof(float) * normals_count * 3);
	memcpy(texCoords, &_texCoords[0], sizeof(float) * texCoords_count * 2);
	memcpy(indices, &_indices[0], sizeof(uint) * indices_count);


}
