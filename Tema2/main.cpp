// IN RENDER FUNCTION SE RULEAZA DOAR EXERCITIUL DORIT!!!

#include <windows.h>  // biblioteci care urmeaza sa fie incluse
#include <stdlib.h> // necesare pentru citirea shader-elor
#include <stdio.h>
#include <math.h>
#include <iostream>
#include <GL/glew.h> // glew apare inainte de freeglut
#include <GL/freeglut.h> // nu trebuie uitat freeglut.h

#include "loadShaders.h"

#include "glm/glm/glm.hpp"  
#include "glm/glm/gtc/matrix_transform.hpp"
#include "glm/glm/gtx/transform.hpp"
#include "glm/glm/gtc/type_ptr.hpp"

using namespace std;

//////////////////////////////////////

GLuint
VaoId,
VboId,
ColorBufferId,
ProgramId,
myMatrixLocation,
matrScaleLocation,
matrTranslLocation,
matrRotlLocation,
codColLocation,
rectangleLocation;

glm::mat4 myMatrix, resizeMatrix, matrTransl, matrScale, matrRot, scaleMat;


int codCol;
float angle = 1.5;
float tx = 0; float ty = 0;
int width = 1000, height = 1000;

void displayMatrix()
{
	for (int ii = 0; ii < 4; ii++)
	{
		for (int jj = 0; jj < 4; jj++)
			cout << myMatrix[ii][jj] << "  ";
		cout << endl;
	};
	cout << "\n";

};

void CreateVBO(void)
{
	// varfurile 
	GLfloat Vertices[] = {
		 
		   // Fundal:
		   -990.0f, -990.0f, 0.0f, 1.0f,
		   990.0f,  -990.0f, 0.0f, 1.0f,
		   990.0f, 990.0f, 0.0f, 1.0f,
		   -990.0f, 990.0f, 0.0f, 1.0f,

		   // Dreptunghiul D de dimensiuni 800 x 1000:
		   1000.0f,  -800.0f, 0.0f, 1.0f,
		   -1000.0f, -800.0f, 0.0f, 1.0f,
		   -1000.0f,  800.0f, 0.0f, 1.0f,
		   1000.0f,  800.0f, 0.0f, 1.0f,

		   // Poligonul convex P1:
		   -400.0f,  -400.0f, 0.0f, 1.0f,
		   -550.0f, -400.0f, 0.0f, 1.0f,
		   -550.0f,  0.0f, 0.0f, 1.0f,
		   -400.0f,  0.0f, 0.0f, 1.0f,

		   // Poligonul concav P2:
		   400.0f,  400.0f, 0.0f, 1.0f,
		   550.0f,  0.0f, 0.0f, 1.0f,
		   400.0f,  0.0f, 0.0f, 1.0f,
		   550.0f, 400.0f, 0.0f, 1.0f,

		   // CENTRUL DINTRE P1 SI P2 ESTE CHIAR ORIGINEA (0.0f, 0.0f, 0.0f, 0.0f)
	};



	// Culorile varfurilor din colturi:
	GLfloat Colors[] = {
	  1.0f, 0.0f, 1.0f, 1.0f,
	  1.0f, 0.0f, 0.0f, 1.0f,
	  0.0f, 0.0f, 1.0f, 1.0f,
	  1.0f, 0.0f, 1.0f, 1.0f,
	};


	// se creeaza un buffer nou
	glGenBuffers(1, &VboId);
	// este setat ca buffer curent
	glBindBuffer(GL_ARRAY_BUFFER, VboId);
	// punctele sunt "copiate" in bufferul curent
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

	// se creeaza / se leaga un VAO (Vertex Array Object) - util cand se utilizeaza mai multe VBO
	glGenVertexArrays(1, &VaoId);
	glBindVertexArray(VaoId);
	// se activeaza lucrul cu atribute; atributul 0 = pozitie
	glEnableVertexAttribArray(0);
	// 
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);

	// un nou buffer, pentru culoare
	glGenBuffers(1, &ColorBufferId);
	glBindBuffer(GL_ARRAY_BUFFER, ColorBufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Colors), Colors, GL_STATIC_DRAW);
	// atributul 1 =  culoare
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);


}
void DestroyVBO(void)
{


	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &ColorBufferId);
	glDeleteBuffers(1, &VboId);

	glBindVertexArray(0);
	glDeleteVertexArrays(1, &VaoId);


}

void CreateShaders(void)
{
	ProgramId = LoadShaders("04_03_Shader.vert", "04_03_Shader.frag");
	glUseProgram(ProgramId);
}
void DestroyShaders(void)
{
	glDeleteProgram(ProgramId);
}

void Initialize(void)
{
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f); // culoarea de fond a ecranului
	CreateVBO();
	CreateShaders();
}

void Exercitiul_i(void) 
{
	codColLocation = glGetUniformLocation(ProgramId, "codCol");

	rectangleLocation = glGetUniformLocation(ProgramId, "rectangle");
	glUniform1i(rectangleLocation, 0);
	codCol = 2;
	glUniform1i(codColLocation, codCol);
	glDrawArrays(GL_POLYGON, 4, 4);

	codCol = 3;
	glUniform1i(codColLocation, codCol);

	glDrawArrays(GL_POLYGON, 8, 4);

	codCol = 1;
	glUniform1i(codColLocation, codCol);
	glDrawArrays(GL_POLYGON, 12, 4);
	glutSwapBuffers();
	glFlush();
}

void Exercitiul_ii(void)
{
	glDrawArrays(GL_POLYGON, 0, 4);

	codColLocation = glGetUniformLocation(ProgramId, "codCol");

	rectangleLocation = glGetUniformLocation(ProgramId, "rectangle");

	glUniform1i(rectangleLocation, 0);
	codCol = 2;
	glUniform1i(codColLocation, codCol);
	glDrawArrays(GL_POLYGON, 4, 4);

	codCol = 3;
	glUniform1i(codColLocation, codCol);

	glDrawArrays(GL_POLYGON, 8, 4);

	codCol = 1;
	glUniform1i(codColLocation, codCol);
	glDrawArrays(GL_POLYGON, 12, 4);
	glutSwapBuffers();
	glFlush();


}

void Exercitiul_iii(void)
{
	glDrawArrays(GL_POLYGON, 0, 4);
	// Matricea pentru elementele care isi schimba pozitia
	myMatrix = matrRot * resizeMatrix;
	//displayMatrix ();

	// matricele de scalare si de translatie
	myMatrixLocation = glGetUniformLocation(ProgramId, "myMatrix");
	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);

	codColLocation = glGetUniformLocation(ProgramId, "codCol");

	rectangleLocation = glGetUniformLocation(ProgramId, "rectangle");

	glUniform1i(rectangleLocation, 0);
	codCol = 2;
	glUniform1i(codColLocation, codCol);
	glDrawArrays(GL_POLYGON, 4, 4);

	codCol = 3;
	glUniform1i(codColLocation, codCol);

	glDrawArrays(GL_POLYGON, 8, 4);

	codCol = 1;
	glUniform1i(codColLocation, codCol);
	glDrawArrays(GL_POLYGON, 12, 4);
	glutSwapBuffers();
	glFlush();
}

void Exercitiul_iv(void)
{
	glDrawArrays(GL_POLYGON, 0, 4);
	// Matricea pentru elementele care isi schimba pozitia
	myMatrix = matrRot * resizeMatrix;
	//displayMatrix ();

	// matricele de scalare si de translatie
	myMatrixLocation = glGetUniformLocation(ProgramId, "myMatrix");
	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);

	codColLocation = glGetUniformLocation(ProgramId, "codCol");

	rectangleLocation = glGetUniformLocation(ProgramId, "rectangle");

	// NU E COMPLET SHADER-UL
	codCol = 3;
	glUniform1i(codColLocation, codCol);
	glUniform1i(rectangleLocation, 1);
	glDrawArrays(GL_POLYGON, 4, 4);

	/*
	glUniform1i(rectangleLocation, 0);
	codCol = 2;
	glUniform1i(codColLocation, codCol);
	glDrawArrays(GL_POLYGON, 4, 4);
	*/
	codCol = 2;
	glUniform1i(codColLocation, codCol);

	glDrawArrays(GL_POLYGON, 8, 4);

	codCol = 1;
	glUniform1i(codColLocation, codCol);
	glDrawArrays(GL_POLYGON, 12, 4);
	glutSwapBuffers();
	glFlush();

}

void RenderFunction(void)
{

	resizeMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.f / width, 1.f / height, 1.0));
	matrTransl = glm::translate(glm::mat4(1.0f), glm::vec3(tx, ty, 0.0));
	matrRot = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0, 0.0, 1.0));

	glClear(GL_COLOR_BUFFER_BIT);
	myMatrix = resizeMatrix;


	// matricea de redimensionare (pentru elementele "fixe")
	myMatrixLocation = glGetUniformLocation(ProgramId, "myMatrix");// -- returneaza locatia unei variabile uniforme
	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);
	// desenare puncte din colturi si axe
	codCol = 0;
	glUniform1i(codColLocation, codCol);


	// ------EXERCITIUL (i)-------
	Exercitiul_i();

	// ------EXERCITIUL (ii)------ 
	// Exercitiul_ii();

	
	// ------EXERCITIUL (iii)------
	// Exercitiul_iii();
	
	// ------EXERCITIUL (iv)------
	// Exercitiul_iv();

	// -------EXERCITIUL (v)-------
	// ROTATIA AM REALIZAT-O INMULTIND MATRICELE IN PROGRAMUL PRINCIPAL
	// SCALAREA AM REALIZAT-O INMULTIND MATRICELE IN SHADER
}
void Cleanup(void)
{
	DestroyShaders();
	DestroyVBO();
}

int main(int argc, char* argv[])
{

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Compunerea transformarilor. Utilizarea tastaturii");
	glewInit();
	Initialize();
	glutDisplayFunc(RenderFunction);
	glutIdleFunc(RenderFunction);
	glutCloseFunc(Cleanup);
	glutMainLoop();

}
