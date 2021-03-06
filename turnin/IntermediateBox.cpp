#include "IntermediateBox.h"
#include <algorithm>    // std::max
#include "Window.h"

IntermediateBox::IntermediateBox()
{
	init();
}


IntermediateBox::~IntermediateBox()
{

}

void IntermediateBox::update(glm::mat4 trans)
{
	matrixTrans = trans;
	processPoints();
}

void IntermediateBox::draw(glm::mat4 trans, GLint shader)
{
	if (draws) {
		GLint shaderProg = 3;
		glUseProgram(shaderProg);
		// Calculate the combination of the model and view (camera inverse) matrices
		glm::mat4 modelview = Window::V * trans;
		// We need to calcullate this because modern OpenGL does not keep track of any matrix other than the viewport (D)
		// Consequently, we need to forward the projection, view, and model matrices to the shader programs
		// Get the location of the uniform variables "projection" and "modelview"
		uProjection = glGetUniformLocation(shaderProg, "projection");
		uModelview = glGetUniformLocation(shaderProg, "modelview");
		glUniform1i(glGetUniformLocation(shaderProg, "light"), 1);
		// Now send these values to the shader program
		glUniformMatrix4fv(uProjection, 1, GL_FALSE, &Window::P[0][0]);
		glUniformMatrix4fv(uModelview, 1, GL_FALSE, &modelview[0][0]);

		glUniform3f(glGetUniformLocation(shaderProg, "viewPos"), 0, 0, 20.0f);
		// Now draw the cube. We simply need to bind the VAO associated with it.
		glBindVertexArray(VAO);
		// Tell OpenGL to draw with triangles, using 36 indices, the type of the indices, and the offset to start from
		glLineWidth(4);
		glDrawArrays(GL_LINE_STRIP, 0, 12 * 3);
		glBindVertexArray(0);
	}
}

void IntermediateBox::init()
{

	toWorld = glm::mat4(1.0f);

	// Create array object and buffers. Remember to delete your buffers when the object is destroyed!
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	// Bind the Vertex Array Object (VAO) first, then bind the associated buffers to it.
	// Consider the VAO as a container for all your buffers.
	glBindVertexArray(VAO);

	// Now bind a VBO to it as a GL_ARRAY_BUFFER. The GL_ARRAY_BUFFER is an array containing relevant data to what
	// you want to draw, such as vertices, normals, colors, etc.
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// glBufferData populates the most recently bound buffer with data starting at the 3rd argument and ending after
	// the 2nd argument number of indices. How does OpenGL know how long an index spans? Go to glVertexAttribPointer.
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	// Enable the usage of layout location 0 (check the vertex shader to see what this is)
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0,// This first parameter x should be the same as the number passed into the line "layout (location = x)" in the vertex shader. In this case, it's 0. Valid values are 0 to GL_MAX_UNIFORM_LOCATIONS.
		3, // This second line tells us how any components there are per vertex. In this case, it's 3 (we have an x, y, and z component)
		GL_FLOAT, // What type these components are
		GL_FALSE, // GL_TRUE means the values should be normalized. GL_FALSE means they shouldn't
		3 * sizeof(GLfloat), // Offset between consecutive indices. Since each of our vertices have 3 floats, they should have the size of 3 floats in between
		(GLvoid*)0); // Offset of the first vertex's component. In our case it's 0 since we don't pad the vertices array with anything.

					 // We've sent the vertex data over to OpenGL, but there's still something missing.
					 // In what order should it draw those vertices? That's why we'll need a GL_ELEMENT_ARRAY_BUFFER for this.
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Unbind the currently bound buffer so that we don't accidentally make unwanted changes to it.
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// Unbind the VAO now so we don't accidentally tamper with it.
	// NOTE: You must NEVER unbind the element array buffer associated with a VAO!
	glBindVertexArray(0);
}

float IntermediateBox::getHeight()
{
	return height;
}

float IntermediateBox::getWidth()
{
	return width;
}

float IntermediateBox::getLength()
{
	return length;
}

void IntermediateBox::processPoints()
{
	float maxX = -1000000;
	float maxY = -1000000;
	float maxZ = -1000000;
	float minX = 1000000;
	float minY = 1000000;
	float minZ = 1000000;

	glm::mat4 neutered = matrixTrans;
	for (int i = 0; i < 8; i++) {
		glm::vec3 temp = glm::vec3(vertices[i][0], vertices[i][1], vertices[i][2]);
		glm::vec4 mult = glm::vec4(temp, 1.0f);
		mult = neutered * mult;
		if (mult.x > maxX) {
			maxX = mult.x;
		}
		else if (mult.x < minX) {
			minX = mult.x;
		}
		if (mult.y > maxY) {
			maxY = mult.y;
		}
		else if (mult.y < minY) {
			minY = mult.y;
		}
		if (mult.z > maxZ) {
			maxZ = mult.z;
		}
		else if (mult.y < minZ) {
			minZ = mult.z;
		}
		width = maxX - minX;
		height = maxY - minY;
		length = maxZ - minZ;
	}
}

void IntermediateBox::setDraws()
{
	draws = !draws;
}
