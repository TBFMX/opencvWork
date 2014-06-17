#include "obj.h"
#include<string>
#include<fstream>
#include<vector>

cObj::cObj(std::string filename) {
	std::ifstream ifs(filename.c_str(), std::ifstream::in);
	std::string line, key;
	while (ifs.good() && !ifs.eof() && std::getline(ifs, line)) {
		key = "";
		std::stringstream stringstream(line);
		stringstream >> key >> std::ws;
		
		if (key == "v") { // vertex
			vertex v; float x;
			while (!stringstream.eof()) {
				stringstream >> x >> std::ws;
				v.v.push_back(x);
			}
			vertices.push_back(v);
		} else if (key == "vp") { // parameter
			vertex v; float x;
			while (!stringstream.eof()) {
				stringstream >> x >> std::ws;
				v.v.push_back(x);
			}
			parameters.push_back(v);
		} else if (key == "vt") { // texture coordinate
			vertex v; float x;
			while (!stringstream.eof()) {
				stringstream >> x >> std::ws;
				v.v.push_back(x);
			}
			texcoords.push_back(v);
		} else if (key == "vn") { // normal
			vertex v; float x;
			while (!stringstream.eof()) {
				stringstream >> x >> std::ws;
				v.v.push_back(x);
			}
			v.normalize();
			normals.push_back(v);
		} else if (key == "f") { // face
			face f; int v, t, n;
			while (!stringstream.eof()) {
				stringstream >> v >> std::ws;
				f.vertex.push_back(v-1);
				if (stringstream.peek() == '/') {
					stringstream.get();
					if (stringstream.peek() == '/') {
						stringstream.get();
						stringstream >> n >> std::ws;
						f.normal.push_back(n-1);
					} else {
						stringstream >> t >> std::ws;
						f.texture.push_back(t-1);
						if (stringstream.peek() == '/') {
							stringstream.get();
							stringstream >> n >> std::ws;
							f.normal.push_back(n-1);
						}
					}
				}
			}
			faces.push_back(f);
		} else {

		}
	}
	ifs.close();
	std::cout << "               Name: " << filename << std::endl;
	std::cout << "           Vertices: " << number_format(vertices.size()) << std::endl;
	std::cout << "         Parameters: " << number_format(parameters.size()) << std::endl;
	std::cout << "Texture Coordinates: " << number_format(texcoords.size()) << std::endl;
	std::cout << "            Normals: " << number_format(normals.size()) << std::endl;
	std::cout << "              Faces: " << number_format(faces.size()) << std::endl << std::endl;
	list = glGenLists(1);
	
	std::string outputFile = filename + "_h";
	writeAll(outputFile,vertices,normals,faces,texcoords);
	
	compileList();
	vertices.clear();
	normals.clear();
	faces.clear();
	texcoords.clear();
};

cObj::~cObj() {
	glDeleteLists(list, 1);
}

void cObj::writeAll(std::string outName, std::vector<vertex> vertices,
		std::vector<vertex> normals, std::vector<face> faces, std::vector<vertex> texcoords){
	char *outNameChar=&outName[0];
	std::ofstream outFile(outNameChar);
	
	outFile << "//               Name: " << outName << std::endl;
	outFile << "//           Vertices: " << number_format(vertices.size()) << std::endl;
	outFile << "//         Parameters: " << number_format(parameters.size()) << std::endl;
	outFile << "//Texture Coordinates: " << number_format(texcoords.size()) << std::endl;
	outFile << "//            Normals: " << number_format(normals.size()) << std::endl;
	outFile << "//              Faces: " << number_format(faces.size()) << std::endl << std::endl;
	std::vector<vertex>::iterator itVertex;
	// writing vertices
	outFile << "float vertices[]={" << std::endl;
	unsigned counter = 0;
	for(itVertex = vertices.begin(); itVertex < vertices.end(); ++itVertex){
		outFile << (*itVertex).v[0] << "," <<(*itVertex).v[1] << "," << (*itVertex).v[2] << "," << std::endl;
		++counter;
	}
	outFile << "};" << std::endl << std::endl;
	// writing normals
	outFile << "float normals[]={" << std::endl;
	counter = 0;
	for(itVertex = normals.begin(); itVertex < normals.end(); ++itVertex){
		outFile << (*itVertex).v[0] << "," <<(*itVertex).v[1] << "," << (*itVertex).v[2] << "," << std::endl;
		++counter;
	}
	outFile << "};" << std::endl << std::endl;
	// writing faces
	outFile << "int faces[]={" << std::endl;
	counter = 0;
	std::vector<face>::iterator itFaces;
	for(itFaces = faces.begin(); itFaces < faces.end(); ++itFaces){
		outFile << (*itFaces).vertex[0] << "," << (*itFaces).vertex[1] << "," << (*itFaces).vertex[2] << ","<< std::endl;
		++counter;
	}
	outFile << "};" << std::endl << std::endl;
	// writing textures
	outFile << "float texcoords[]={" << std::endl;
	counter = 0;
	for(itVertex = texcoords.begin(); itVertex < texcoords.end(); ++itVertex){
		outFile << (*itVertex).v[0] << "," <<(*itVertex).v[1] << "," << std::endl;
		++counter;
	}
	outFile << "};" << std::endl;
}

void cObj::compileList() {
	glNewList(list, GL_COMPILE);
	for (unsigned i = 0; i < faces.size(); i++) {
		if (faces[i].vertex.size() == 3) { // triangle
			if (faces[i].normal.size() == 3) { // with normals
				glBegin(GL_TRIANGLES);
				glNormal3f(normals[faces[i].normal[0]].v[0], normals[faces[i].normal[0]].v[1], normals[faces[i].normal[0]].v[2]);
				glVertex3f(vertices[faces[i].vertex[0]].v[0], vertices[faces[i].vertex[0]].v[1], vertices[faces[i].vertex[0]].v[2]);
				glNormal3f(normals[faces[i].normal[1]].v[0], normals[faces[i].normal[1]].v[1], normals[faces[i].normal[1]].v[2]);
				glVertex3f(vertices[faces[i].vertex[1]].v[0], vertices[faces[i].vertex[1]].v[1], vertices[faces[i].vertex[1]].v[2]);
				glNormal3f(normals[faces[i].normal[2]].v[0], normals[faces[i].normal[2]].v[1], normals[faces[i].normal[2]].v[2]);
				glVertex3f(vertices[faces[i].vertex[2]].v[0], vertices[faces[i].vertex[2]].v[1], vertices[faces[i].vertex[2]].v[2]);
				glEnd();
			} else { // without normals -- evaluate normal on quad
				vertex v = (vertices[faces[i].vertex[1]] - vertices[faces[i].vertex[0]]).cross(vertices[faces[i].vertex[2]] - vertices[faces[i].vertex[0]]);
				v.normalize();
				glBegin(GL_TRIANGLES);
				glNormal3f(v.v[0], v.v[1], v.v[2]);
				glVertex3f(vertices[faces[i].vertex[0]].v[0], vertices[faces[i].vertex[0]].v[1], vertices[faces[i].vertex[0]].v[2]);
				glVertex3f(vertices[faces[i].vertex[1]].v[0], vertices[faces[i].vertex[1]].v[1], vertices[faces[i].vertex[1]].v[2]);
				glVertex3f(vertices[faces[i].vertex[2]].v[0], vertices[faces[i].vertex[2]].v[1], vertices[faces[i].vertex[2]].v[2]);
				glEnd();
			}
		} else if (faces[i].vertex.size() == 4) { // quad
			if (faces[i].normal.size() == 4) { // with normals
				glBegin(GL_QUADS);
				glNormal3f(normals[faces[i].normal[0]].v[0], normals[faces[i].normal[0]].v[1], normals[faces[i].normal[0]].v[2]);
				glVertex3f(vertices[faces[i].vertex[0]].v[0], vertices[faces[i].vertex[0]].v[1], vertices[faces[i].vertex[0]].v[2]);
				glNormal3f(normals[faces[i].normal[1]].v[0], normals[faces[i].normal[1]].v[1], normals[faces[i].normal[1]].v[2]);
				glVertex3f(vertices[faces[i].vertex[1]].v[0], vertices[faces[i].vertex[1]].v[1], vertices[faces[i].vertex[1]].v[2]);
				glNormal3f(normals[faces[i].normal[2]].v[0], normals[faces[i].normal[2]].v[1], normals[faces[i].normal[2]].v[2]);
				glVertex3f(vertices[faces[i].vertex[2]].v[0], vertices[faces[i].vertex[2]].v[1], vertices[faces[i].vertex[2]].v[2]);
				glNormal3f(normals[faces[i].normal[3]].v[0], normals[faces[i].normal[3]].v[1], normals[faces[i].normal[3]].v[2]);
				glVertex3f(vertices[faces[i].vertex[3]].v[0], vertices[faces[i].vertex[3]].v[1], vertices[faces[i].vertex[3]].v[2]);
				glEnd();
			} else { // without normals -- evaluate normal on quad
				vertex v = (vertices[faces[i].vertex[1]] - vertices[faces[i].vertex[0]]).cross(vertices[faces[i].vertex[2]] - vertices[faces[i].vertex[0]]);
				v.normalize();
				glBegin(GL_QUADS);
				glNormal3f(v.v[0], v.v[1], v.v[2]);
				glVertex3f(vertices[faces[i].vertex[0]].v[0], vertices[faces[i].vertex[0]].v[1], vertices[faces[i].vertex[0]].v[2]);
				glVertex3f(vertices[faces[i].vertex[1]].v[0], vertices[faces[i].vertex[1]].v[1], vertices[faces[i].vertex[1]].v[2]);
				glVertex3f(vertices[faces[i].vertex[2]].v[0], vertices[faces[i].vertex[2]].v[1], vertices[faces[i].vertex[2]].v[2]);
				glVertex3f(vertices[faces[i].vertex[3]].v[0], vertices[faces[i].vertex[3]].v[1], vertices[faces[i].vertex[3]].v[2]);
				glEnd();
			}
		}
	}
	glEndList();
}

void cObj::render() {
	glCallList(list);
}
