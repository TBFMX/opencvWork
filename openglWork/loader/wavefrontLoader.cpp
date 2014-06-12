#include "wavefrontLoader.h"

WFObject::WFObject()
{

}

WFObject::~WFObject()
{

}

int WFObject::load(char *filename)
{
	fstream objFile;
	objFile.open(filename);

	if(objFile.is_open())
	{
		char line[255];

		// Parse object file line by line
		while(objFile.good())
		{
			objFile.getline(line, 255);
			parseLine(line);
		}

		objFile.close();
	}
	else
	{
		cout << "Could not open WFObject file '" << filename << "'\n";
		return false;
	}

	return true;
}

void WFObject::parseLine(char *line)
{
	if(!strlen(line))
	{
		return;
	}

	char *lineType;
	lineType = strtok(strdup(line), " ");

	// Decide what to do
	if(!strcmp(lineType, "v"))		// Vertex
	{
		parseVertex(line);
	}
	else if(!strcmp(lineType, "vn"))	// Normal
	{
		parseNormal(line);
	}
	else if(!strcmp(lineType, "f"))	// Face
	{
		parseFace(line);
	}

	return;
}

// Draw object
void WFObject::draw()
{
	glBegin(GL_TRIANGLES);

	for(int f = 0; f < faces.size(); f++)
	{
		glNormal3f(normals[faces[f].vn1 - 1].x, normals[faces[f].vn1 - 1].y, normals[faces[f].vn1 - 1].z);
		glVertex3f(vertices[faces[f].v1 - 1].x, vertices[faces[f].v1 - 1].y, vertices[faces[f].v1 - 1].z);

		glNormal3f(normals[faces[f].vn2 - 1].x, normals[faces[f].vn2 - 1].y, normals[faces[f].vn2 - 1].z);
		glVertex3f(vertices[faces[f].v2 - 1].x, vertices[faces[f].v2 - 1].y, vertices[faces[f].v2 - 1].z);

		glNormal3f(normals[faces[f].vn3 - 1].x, normals[faces[f].vn3 - 1].y, normals[faces[f].vn3 - 1].z);
		glVertex3f(vertices[faces[f].v3 - 1].x, vertices[faces[f].v3 - 1].y, vertices[faces[f].v3 - 1].z);
	}

	glEnd();
}

// Parse a "v" vertex line of the file into our vertices array
void WFObject::parseVertex(char *line)
{
	vertices.push_back(Vector());		// Add a new element to the vertices array

	sscanf(line, "v %f %f %f", &vertices.back().x, &vertices.back().y, &vertices.back().z);

	return;
}

// Parse a "vn" normal line of the file into the normals array
void WFObject::parseNormal(char *line)
{
	normals.push_back(Vector());

	sscanf(line, "vn %f %f %f", &normals.back().x, &normals.back().y, &normals.back().z);

	return;
}

// Parse a "f" face line into the faces array. This gets complex due to there being different line formats.
void WFObject::parseFace(char *line)
{
	int fill = 0;

	faces.push_back(Face());

	// Read face line. If texture indicies aren't present, don't read them.
	if(sscanf(line, "f %d//%d %d//%d %d//%d", &faces.back().v1,
											  &faces.back().vn1,
											  &faces.back().v2,
											  &faces.back().vn2,
											  &faces.back().v3,
											  &faces.back().vn3) <= 1)
	{
		sscanf(line, "f %d/%d/%d %d/%d/%d %d/%d/%d", &faces.back().v1,
													 &fill,
													 &faces.back().vn1,
													 &faces.back().v2,
													 &fill,
													 &faces.back().vn2,
													 &faces.back().v3,
													 &fill,
													 &faces.back().vn3);
	}

	return;
}
