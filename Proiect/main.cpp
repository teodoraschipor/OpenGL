#include "loadShaders.h"

#ifdef __linux__
#include <stdlib.h> // necesare pentru citirea shader-elor
#include <stdio.h>
#include <fstream>
#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/freeglut.h>
#include <chrono>

#include "glm/glm/glm.hpp"
#include "glm/glm/gtc/matrix_transform.hpp"
#include "glm/glm/gtx/transform.hpp"
#include "glm/glm/gtc/type_ptr.hpp"
#else
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <chrono>

#include "glm/glm/glm.hpp"
#include "glm/glm/gtc/matrix_transform.hpp"
#include "glm/glm/gtx/transform.hpp"
#include "glm/glm/gtc/type_ptr.hpp"
#endif

namespace Constants {
    const int WIDTH = 400;
    const int HEIGHT = 300;
}

/// POINT
class Point {
public:
    GLfloat x;
    GLfloat y;
    GLfloat z;
    GLfloat k;
public:
    Point();
    Point(GLfloat X, GLfloat Y);
    bool CheckInBoundaries();
    const Point& operator = (const Point& A);
};

float distanceBetweeNTwoPoints(Point& x1, Point& x2){
    return sqrt((x1.x-x2.x)*(x1.x-x2.x) + (x1.y-x2.y)*(x1.y-x2.y));
}
Point::Point() {
    x = 1.0f;
    y = 1.0f;
    z = 0.0f;
    k = 1.0f;
}

Point::Point(GLfloat X, GLfloat Y) {

    x = X;
    y = Y;
    z = 0.0f;
    k = 1.0f;
}

bool Point::CheckInBoundaries() {

    if (this->x <= Constants::WIDTH && this->x >= -Constants::WIDTH && this->y <= Constants::HEIGHT && this->y >= -Constants::HEIGHT)
        return true;
    return false;
}

const Point& Point::operator=(const Point& A) {
    x = A.x;
    y = A.y;
    z = A.z;
    k = A.k;
    return *this;
}

//END POINT

/// VERTICES
class Vertices {
public:
    GLfloat vertices[1000];
    int n;
    int m;
public:
    Vertices();
    void AddVertices(Point x1, Point x2, Point x3, Point x4);
};

Vertices::Vertices() {

    n = 0;
    m = 0;
}

void Vertices::AddVertices(Point x1, Point x2, Point x3, Point x4) {

    this->vertices[m++] = x1.x;
    this->vertices[m++] = x1.y;
    this->vertices[m++] = x1.z;
    this->vertices[m++] = x1.k;

    this->vertices[m++] = x2.x;
    this->vertices[m++] = x2.y;
    this->vertices[m++] = x2.z;
    this->vertices[m++] = x2.k;

    this->vertices[m++] = x3.x;
    this->vertices[m++] = x3.y;
    this->vertices[m++] = x3.z;
    this->vertices[m++] = x3.k;

    this->vertices[m++] = x4.x;
    this->vertices[m++] = x4.y;
    this->vertices[m++] = x4.z;
    this->vertices[m++] = x4.k;

    n = n + 4;
//    std::cout << " " << m << " ";
}

/// END VERTICES

/// GLOBALS


namespace Globals {
    Vertices VERTICES;
    GLuint
        VaoId,
        VboId,
        ColorBufferId,
        ProgramId,
        myMatrixLocation,
        matrScaleLocation,
        matrTranslLocation,
        matrRotlLocation,
        codColLocation;
    glm::mat4 myMatrix, resizeMatrix, matrTransl, matrScale, matrRot;

}

/// POLYGON
class PolygonGL {
public:
    Point x1, x2, x3, x4;
    Point x1r, x2r, x3r, x4r;
    int index;
    float tx, ty; // translation values
    float angle;
    float scaleFactor;
    int codCol;
    bool direction;
    bool deleted;
public:
    PolygonGL();
    PolygonGL(Point& X1, Point& X2, Point& X3, Point& X4);
    bool CheckIfCollision(PolygonGL& B);
    bool CheckInBoundaries();
    void Draw();
    void Rotate(float appendAngle);
    void Translate(float appendX, float appendY);
    void Scale(float appendFactor);
    void deleteP();
    int random();
    float Area();
    Point ApplyTransformationsToAPoint(Point &P);

};

PolygonGL::PolygonGL() {}
PolygonGL::PolygonGL(Point& X1, Point& X2, Point& X3, Point& X4) {
    x1 = X1;
    x2 = X2;
    x3 = X3;
    x4 = X4;
    direction = 0;
    index = Globals::VERTICES.n;
    Globals::VERTICES.AddVertices(x1, x2, x3, x4);
    tx = 0.0;
    ty = 0.0;
    angle = 0.0;
    scaleFactor = 1.0;
    deleted = 0;
    codCol = random();
}

bool PolygonGL::CheckIfCollision(PolygonGL& B) {

    if (this->deleted){
        return false;
    }

    Point l1 = this->x4;
    Point r1 = this->x2;
    Point l2 = B.x4;
    Point r2 = B.x2;

    if ((l1.x+tx)*scaleFactor >= r2.x+B.tx || l2.x+B.tx >= (r1.x+tx)*scaleFactor)
        return false;

    if ((l1.y+ty)*scaleFactor <= r2.y+B.ty || l2.y+B.ty <= (r1.y+ty)*scaleFactor)
        return false;

    return true;
}

bool PolygonGL::CheckInBoundaries() {

    Point x1 = this->x1;
    Point x2 = this->x2;
    Point x3 = this->x3;
    Point x4 = this->x4;

    x1.x += tx;
    x2.x += tx;
    x3.x += tx;
    x4.x += tx;

    x1.y += ty;
    x2.y += ty;
    x3.y += ty;
    x4.y += ty;

    if (x1.CheckInBoundaries() && x2.CheckInBoundaries() && x3.CheckInBoundaries() && x4.CheckInBoundaries())
        return true;
    return false;
}

void PolygonGL::Draw() {

    if (deleted){
        return;
    }
    glm::mat4 myMatrix, resizeMatrix, matrTransl, matrScale, matrRot;

    resizeMatrix = glm::scale(glm::mat4(1.0f), glm::vec3((1.f / Constants::WIDTH) * scaleFactor, (1.f / Constants::HEIGHT) * scaleFactor, 1.0));
    matrTransl = glm::translate(glm::mat4(1.0f), glm::vec3(tx, ty, 0.0));
    matrRot = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0, 0.0, 1.0));


    myMatrix = resizeMatrix * matrTransl * matrRot;
    Globals::myMatrixLocation = glGetUniformLocation(Globals::ProgramId, "myMatrix");
    glUniformMatrix4fv(Globals::myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);

    Globals::codColLocation = glGetUniformLocation(Globals::ProgramId, "codCol");
 
    glUniform1i(Globals::codColLocation, this->codCol);
    glDrawArrays(GL_POLYGON, index, 4);

    glFlush();
}


Point PolygonGL::ApplyTransformationsToAPoint(Point& P){
    glm::mat4 myMatrix, resizeMatrix, matrTransl, matrScale, matrRot;

//    resizeMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(scaleFactor,  scaleFactor, 1.0));
    matrTransl = glm::translate(glm::mat4(1.0f), glm::vec3(tx, ty, 0.0));
//    matrRot = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0, 0.0, 1.0));

    myMatrix =  matrTransl;

    glm::vec4 v(P.x, P.y, 0.0, 1.0);
    glm::vec4 result = v * myMatrix ;

    auto x = result[0];
    auto y = result[1];
    auto resultPoint = Point(result[0], result[1]);
    return resultPoint;
}

void PolygonGL::Translate(float appendX, float appendY) {
    this->tx += appendX;
    this->ty += appendY;
}

void PolygonGL::Rotate(float appendAngle) {
    angle += appendAngle;
}
void PolygonGL::Scale(float appendFactor) {
    scaleFactor += appendFactor;
}

int PolygonGL::random() {
    return rand() % 11;
}

float PolygonGL::Area() {
    auto a = sqrt((x1.x-x2.x)*(x1.x-x2.x) + (x1.y-x2.y)*(x1.y-x2.y));
    auto b = sqrt((x2.x-x3.x)*(x2.x-x3.x) + (x2.y-x3.y)*(x2.y-x3.y));
    return a*b*scaleFactor;
}

void PolygonGL::deleteP() {
    this->deleted = 1;
}
/// END POLYGON

/// BACKGROUND
class Background :public PolygonGL {
public:
    Background();
    Background(Point& X1, Point& X2, Point& X3, Point& X4);
    void Draw();
};

Background::Background() {}

Background::Background(Point& X1, Point& X2, Point& X3, Point& X4) {
    x1 = X1;
    x2 = X2;
    x3 = X3;
    x4 = X4;
    index = 0;
    Globals::VERTICES.AddVertices(x1, x2, x3, x4);
    tx = 0.0;
    ty = 0.0;
    angle = 0.0;
    scaleFactor = 1.0;
    codCol = 11;
}

void Background::Draw() {
  
    Globals::resizeMatrix = glm::scale(glm::mat4(1.0f), glm::vec3((1.f / Constants::WIDTH) * scaleFactor, (1.f / Constants::HEIGHT) * scaleFactor, 1.0));
    Globals::matrTransl = glm::translate(glm::mat4(1.0f), glm::vec3(tx, ty, 0.0));
    Globals::matrRot = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0, 0.0, 1.0));


    Globals::myMatrix = Globals::resizeMatrix * Globals::matrTransl * Globals::matrRot;

    Globals::myMatrixLocation = glGetUniformLocation(Globals::ProgramId, "myMatrix");
    glUniformMatrix4fv(Globals::myMatrixLocation, 1, GL_FALSE, &Globals::myMatrix[0][0]);

    Globals::codColLocation = glGetUniformLocation(Globals::ProgramId, "codCol");
    glUniform1i(Globals::codColLocation, this->codCol);
    glDrawArrays(GL_POLYGON, index, 4);
    glFlush();
}
/// END BACKGROUND 

using namespace std;

//////////////////////////////////////
/// BEGIN MAIN

int codCol;
int gameState=0;
float PI = 3.141592, angle = 0;
float tx = 0; float ty = 0;

PolygonGL polygonList[100];
int polygonListN;
Background bck, redBck, greenBck;
PolygonGL Player;
void processNormalKeys(unsigned char key, int x, int y)
{

    switch (key) {
    case 'q':
        Player.Rotate(0.2);
        break;
    case 'e':
        Player.Rotate(-0.2);
        break;
    case 'w':
        Player.Translate(0, 2);
        break;
    case 'a':
        Player.Translate(-2, 0);
        break;
    case 's':
        Player.Translate(0, -2);
        break;
    case 'd':
        Player.Translate(2, 0);
        break;
    case 'p':
        Player.Scale(0.000000000000001);
        break;

    }
    if (key == 27)
        exit(0);
}
void processSpecialKeys(int key, int xx, int yy) {

    switch (key) {
    case GLUT_KEY_LEFT:
        Player.Translate(-10, 0);
        break;
    case GLUT_KEY_RIGHT:
        Player.Translate(10, 0);
        break;
    case GLUT_KEY_UP:
        Player.Translate(0, 10);
        break;
    case GLUT_KEY_DOWN:
        Player.Translate(0, -10);
        break;
    }
}



void CreateVBO(void)
{
    Point a, b, c, d;
    a = Point(-400.0f, -300.0f);
    b = Point(400.0f, -300.0f);
    c = Point(400.0f, 300.0f);
    d = Point(-400.0f, 300.0f);
    bck = Background(a, b, c, d);

    a = Point(-400.0f, 15.0f);
    b = Point(-312.0f, 15.0f);
    c = Point(-312.0f, 73.0f);
    d = Point(-400.0f, 73.0f);
    PolygonGL Pol = PolygonGL(a, b, c, d);
    polygonList[polygonListN++] = Pol;

    a = Point(-400.0f, -20.0f);
    b = Point(-380.0f, -20.0f);
    c = Point(-380.0f, 0.0f);
    d = Point(-400.0f, 0.0f);
    PolygonGL Pol1 = PolygonGL(a, b, c, d);
    polygonList[polygonListN++] = Pol1;

    a = Point(-400.0f, 1.0f);
    b = Point(-370.0f, 1.0f);
    c = Point(-370.0f, 15.0f);
    d = Point(-400.0f, 15.0f);
    PolygonGL Pol2 = PolygonGL(a, b, c, d);
    polygonList[polygonListN++] = Pol2;

    a = Point(-400.0f, -5.0f);
    b = Point(-385.0f, -5.0f);
    c = Point(-385.0f, 5.0f);
    d = Point(-400.0f, 5.0f);
    PolygonGL Pol3 = PolygonGL(a, b, c, d);
    polygonList[polygonListN++] = Pol3;

    a = Point(-400.0f, -30.0f);
    b = Point(-385.0f, -30.0f);
    c = Point(-385.0f, -20.0f);
    d = Point(-400.0f, -20.0f);
    PolygonGL Pol4 = PolygonGL(a, b, c, d);
    polygonList[polygonListN++] = Pol4;

    a = Point(-400.0f, -40.0f);
    b = Point(-385.0f, -40.0f);
    c = Point(-385.0f, -30.0f);
    d = Point(-400.0f, -30.0f);
    PolygonGL Pol5 = PolygonGL(a, b, c, d);
    polygonList[polygonListN++] = Pol5;

    a = Point(-400.0f, -60.0f);
    b = Point(-375.0f, -60.0f);
    c = Point(-375.0f, -40.0f);
    d = Point(-400.0f, -40.0f);
    PolygonGL Pol6 = PolygonGL(a, b, c, d);
    polygonList[polygonListN++] = Pol6;

    a = Point(-400.0f, -75.0f);
    b = Point(-383.0f, -75.0f);
    c = Point(-383.0f, -60.0f);
    d = Point(-400.0f, -60.0f);
    PolygonGL Pol7 = PolygonGL(a, b, c, d);
    polygonList[polygonListN++] = Pol7;

    a = Point(-400.0f, -95.0f);
    b = Point(-365.0f, -95.0f);
    c = Point(-365.0f, -75.0f);
    d = Point(-400.0f, -75.0f);
    PolygonGL Pol8 = PolygonGL(a, b, c, d);
    polygonList[polygonListN++] = Pol8;

    a = Point(-400.0f, -125.0f);
    b = Point(-355.0f, -125.0f);
    c = Point(-355.0f, -95.0f);
    d = Point(-400.0f, -95.0f);
    PolygonGL Pol9 = PolygonGL(a, b, c, d);
    polygonList[polygonListN++] = Pol9;

    a = Point(-400.0f, -150.0f);
    b = Point(-360.0f, -150.0f);
    c = Point(-360.0f, -125.0f);
    d = Point(-400.0f, -125.0f);
    PolygonGL Pol10 = PolygonGL(a, b, c, d);
    polygonList[polygonListN++] = Pol10; 
 
    a = Point(-400.0f, 80.0f);
    b = Point(-378.0f, 80.0f);
    c = Point(-378.0f, 95.0f);
    d = Point(-400.0f, 95.0f);
    PolygonGL Pol11 = PolygonGL(a, b, c, d);
    polygonList[polygonListN++] = Pol11;

    a = Point(-400.0f, 95.0f);
    b = Point(-388.0f, 95.0f);
    c = Point(-388.0f, 105.0f);
    d = Point(-400.0f, 105.0f);
    PolygonGL Pol21 = PolygonGL(a, b, c, d);
    polygonList[polygonListN++] = Pol21;

    a = Point(-400.0f, 105.0f);
    b = Point(-373.0f, 105.0f);
    c = Point(-373.0f, 120.0f);
    d = Point(-400.0f, 120.0f);
    PolygonGL Pol12 = PolygonGL(a, b, c, d);
    polygonList[polygonListN++] = Pol12;

    a = Point(-400.0f, 114.0f);
    b = Point(-386.0f, 114.0f);
    c = Point(-386.0f, 125.0f);
    d = Point(-400.0f, 125.0f);
    PolygonGL Pol13 = PolygonGL(a, b, c, d);
    polygonList[polygonListN++] = Pol13;

    a = Point(-400.0f, 130.0f);
    b = Point(-350.0f, 130.0f);
    c = Point(-350.0f, 160.0f);
    d = Point(-400.0f, 160.0f);
    PolygonGL Pol14 = PolygonGL(a, b, c, d);
    polygonList[polygonListN++] = Pol14;

    a = Point(-400.0f, 190.0f);
    b = Point(-378.0f, 190.0f);
    c = Point(-378.0f, 165.0f);
    d = Point(-400.0f, 165.0f);
    PolygonGL Pol22 = PolygonGL(a, b, c, d);
    polygonList[polygonListN++] = Pol22;

    a = Point(-400.0f, 194.0f);
    b = Point(-385.0f, 194.0f);
    c = Point(-385.0f, 204.0f);
    d = Point(-400.0f, 204.0f);
    PolygonGL Pol15 = PolygonGL(a, b, c, d);
    polygonList[polygonListN++] = Pol15;


    a = Point(-400.0f, -185.0f);
    b = Point(-360.0f, -185.0f);
    c = Point(-360.0f, -150.0f);
    d = Point(-400.0f, -150.0f);
    PolygonGL Pol16 = PolygonGL(a, b, c, d);
    polygonList[polygonListN++] = Pol16;

    a = Point(-400.0f, 210.0f);
    b = Point(-360.0f, 210.0f);
    c = Point(-360.0f, 240.0f);
    d = Point(-400.0f, 240.0f);
    PolygonGL Pol17 = PolygonGL(a, b, c, d);
    polygonList[polygonListN++] = Pol17;

    a = Point(-400.0f, 245.0f);
    b = Point(-375.0f, 245.0f);
    c = Point(-375.0f, 260.0f);
    d = Point(-400.0f, 260.0f);
    PolygonGL Pol23 = PolygonGL(a, b, c, d);
    polygonList[polygonListN++] = Pol23;

    a = Point(-400.0f, 260.0f);
    b = Point(-380.0f, 260.0f);
    c = Point(-380.0f, 285.0f);
    d = Point(-400.0f, 285.0f);
    PolygonGL Pol24 = PolygonGL(a, b, c, d);
    polygonList[polygonListN++] = Pol24;

    a = Point(-400.0f, 235.0f);
    b = Point(-325.0f, 235.0f);
    c = Point(-325.0f, 300.0f);
    d = Point(-400.0f, 300.0f);
    PolygonGL Pol18 = PolygonGL(a, b, c, d);
    polygonList[polygonListN++] = Pol18;

    a = Point(-400.0f, 258.0f);
    b = Point(-365.0f, 258.0f);
    c = Point(-365.0f, 275.0f);
    d = Point(-400.0f, 275.0f);
    PolygonGL Pol19 = PolygonGL(a, b, c, d);
    polygonList[polygonListN++] = Pol19;

    a = Point(-400.0f, 10.0f);
    b = Point(-365.0f, 10.0f);
    c = Point(-365.0f, 20.0f);
    d = Point(-400.0f, 20.0f);
    PolygonGL Pol25 = PolygonGL(a, b, c, d);
    polygonList[polygonListN++] = Pol25;


    a = Point(-400.0f, 30.0f);
    b = Point(-365.0f, 30.0f);
    c = Point(-365.0f, 45.0f);
    d = Point(-400.0f, 45.0f);
    PolygonGL Pol26 = PolygonGL(a, b, c, d);
    polygonList[polygonListN++] = Pol26;

    a = Point(-400.0f, 135.0f);
    b = Point(-365.0f, 135.0f);
    c = Point(-365.0f, 155.0f);
    d = Point(-400.0f, 155.0f);
    PolygonGL Pol28 = PolygonGL(a, b, c, d);
    polygonList[polygonListN++] = Pol28;

    a = Point(-400.0f, 60.0f);
    b = Point(-355.0f, 60.0f);
    c = Point(-355.0f, 80.0f);
    d = Point(-400.0f, 80.0f);
    PolygonGL Pol27 = PolygonGL(a, b, c, d);
    polygonList[polygonListN++] = Pol27;

    a = Point(-400.0f, 275.0f);
    b = Point(-375.0f, 275.0f);
    c = Point(-375.0f, 300.0f);
    d = Point(-400.0f, 300.0f);
    PolygonGL Pol29 = PolygonGL(a, b, c, d);
    polygonList[polygonListN++] = Pol29;

    a = Point(-400.0f, 285.0f);
    b = Point(-380.0f, 285.0f);
    c = Point(-380.0f, 300.0f);
    d = Point(-400.0f, 300.0f);
    PolygonGL Pol30 = PolygonGL(a, b, c, d);
    polygonList[polygonListN++] = Pol30;

    a = Point(-400.0f, -210.0f);
    b = Point(-310.0f, -210.0f);
    c = Point(-310.0f, -180.f);
    d = Point(-400.0f, -180.0f);
    PolygonGL Pol20 = PolygonGL(a, b, c, d);
    polygonList[polygonListN++] = Pol20;

    a = Point(-17.0f, 0.0f);
    b = Point(0.0f,  0.0f);
    c = Point(0.0f, 10.0f);
    d = Point(-17.0f, 10.0f);
    Player = PolygonGL(a, b, c, d);

    a = Point(-400.0f, -300.0f);
    b = Point(400.0f, -300.0f);
    c = Point(400.0f, 300.0f);
    d = Point(-400.0f, 300.0f);
    redBck = Background(a, b, c, d);
    redBck.codCol = 13;

    a = Point(-400.0f, -300.0f);
    b = Point(400.0f, -300.0f);
    c = Point(400.0f, 300.0f);
    d = Point(-400.0f, 300.0f);
    greenBck = Background(a, b, c, d);
    greenBck.codCol = 12;
    // culorile varfurilor din colturi

    // se creeaza un buffer nou
    glGenBuffers(1, &Globals::VboId);
    // este setat ca buffer curent
    glBindBuffer(GL_ARRAY_BUFFER, Globals::VboId);
    // punctele sunt "copiate" in bufferul curent
    glBufferData(GL_ARRAY_BUFFER, sizeof(Globals::VERTICES.vertices), Globals::VERTICES.vertices, GL_STATIC_DRAW);

    // se creeaza / se leaga un VAO (Vertex Array Object) - util cand se utilizeaza mai multe VBO
    glGenVertexArrays(1, &Globals::VaoId);
    glBindVertexArray(Globals::VaoId);
    // se activeaza lucrul cu atribute; atributul 0 = pozitie
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);


}
void DestroyVBO(void)
{


    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &Globals::ColorBufferId);
    glDeleteBuffers(1, &Globals::VboId);

    glBindVertexArray(0);
    glDeleteVertexArrays(1, &Globals::VaoId);


}

void CreateShaders(void)
{
    Globals::ProgramId = LoadShaders("shader.vert", "shader.frag");
    glUseProgram(Globals::ProgramId);
}
void DestroyShaders(void)
{
    glDeleteProgram(Globals::ProgramId);
}

void Initialize(void)
{
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f); // culoarea de fond a ecranului
    CreateVBO();
    CreateShaders();
}

auto previousTime = std::chrono::steady_clock::now();
int points = 0;
void RenderFunction(void)
{

    auto currentTime = std::chrono::steady_clock::now();
    auto elapsed = previousTime - currentTime;
    float delta_time = elapsed.count();
    glClear(GL_COLOR_BUFFER_BIT);
    bck.Draw();
    for (auto pol : polygonList) {

        pol.Draw();
    }
    Player.Draw();

    if (gameState == 1) {
        redBck.Draw();
    }
    //ecran verde
    else if (gameState == 2) {
        greenBck.Draw();
    }
    //ecran rosu

    // Animate fish
    for (int i=0; i<polygonListN;i++){
        auto area = polygonList[i].Area();
        auto speedFactor = abs(0.00001*delta_time*1/area);
        if (!polygonList[i].CheckInBoundaries()){
            polygonList[i].direction = !polygonList[i].direction;
        }
        int translateDirection = 1;
        if (polygonList[i].direction == 1){
            translateDirection = -1;
        }
        polygonList[i].Translate(translateDirection*speedFactor, 0);
    }

    // Check Colisions between fish and player
    for (int i=0; i<polygonListN; i++){
        bool colision = Player.CheckIfCollision(polygonList[i]);
        if (colision){
            if(polygonList[i].deleted){
                continue;
            }
            auto area = polygonList[i].Area();

            auto playerL = distanceBetweeNTwoPoints(Player.x1, Player.x2);
            auto playerH = distanceBetweeNTwoPoints(Player.x2, Player.x3);
            playerL *= Player.scaleFactor;
            playerH *= Player.scaleFactor;

            auto fishL = distanceBetweeNTwoPoints(polygonList[i].x1, polygonList[i].x2);
            auto fishH = distanceBetweeNTwoPoints(polygonList[i].x2, polygonList[i].x3);

            if (playerL<fishL || playerH<fishH){
                gameState = 1;
            }

            polygonList[i].deleteP();
            auto scaleFactor = 0.001*area;
            Player.Scale(scaleFactor);
        }
    }
    glutSwapBuffers();
    glFlush();
    bool gameFinish = 1;
    for (int i=0; i<polygonListN; i++){
        if(!polygonList[i].deleted){
            gameFinish = 0;
        }
    }
    if (gameFinish){
        gameState = 2;
    }
    previousTime = currentTime;


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
    glutInitWindowSize(1800, 1600);
    glutCreateWindow("Proiect Geometrie 2D Teo + Mihai");
    glewInit();
    Initialize();
    glutDisplayFunc(RenderFunction);
    glutIdleFunc(RenderFunction);
    glutKeyboardFunc(processNormalKeys);
    glutSpecialFunc(processSpecialKeys);
    glutCloseFunc(Cleanup);
    glutMainLoop();


}
