#pragma once
#include <vector>
#include <iostream>
#include <string>
#include <fstream>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef UNTITLED_LOADSHADERS_H
#define UNTITLED_LOADSHADERS_H
GLuint LoadShaders(const char* vertex_file_path, const char* fragment_file_path);
#endif //UNTITLED_LOADSHADERS_H
