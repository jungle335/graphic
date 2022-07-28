#include <windows.h>  
#include <stdlib.h> 
#include <stdio.h>
#include <math.h>
#include <iostream>
#include <GL/glew.h> 
#include <GL/freeglut.h> 

#include "loadShaders.h"

#include "glm/glm/glm.hpp"  
#include "glm/glm/gtc/matrix_transform.hpp"
#include "glm/glm/gtx/transform.hpp"
#include "glm/glm/gtc/type_ptr.hpp"

#include <algorithm>
#include <vector>
#include <tuple>
#include <random>
using namespace std;

GLuint ProgramId, matLoc, codLoc;
glm::mat4 mat, rRot, trT1;
float angles[7], xas[7], yas[7], da, dx, dy;
float rtx, timer_rs = 1000.0, xstjos, ystjos, xdrsus, ydrsus;
float PI = 3.141592, PI2 = 6.283184, radius = 0.1, x = -0.85, y = 0.5, leftt, rightt;
unsigned int cod, rcoliziune, reversed, nr_puncte_cerc = 14, rpoz, dr, VAO, VBO[4];

tuple <float, float, float, float> minMaxXY(vector <float> vf)
{
	float mini_x = 1000000, maxi_x, mini_y = 1000000, maxi_y;
	int i = 0, j = 1;
	while (i < vf.size() && j < vf.size())
	{
		mini_x = min(mini_x, vf[i]);
		maxi_x = max(maxi_x, vf[i]);
		mini_y = min(mini_y, vf[j]);
		maxi_y = max(maxi_y, vf[j]);
		i += 2;
		j += 2;
	}
	return make_tuple(mini_x, maxi_x, mini_y, maxi_y);
}

vector<float> normVf(vector <float> vf, float st, float dr)
{
	tuple <float, float, float, float> getMMXY = minMaxXY(vf);
	float mini_x = get<0>(getMMXY), maxi_x = get<1>(getMMXY), mini_y = get<2>(getMMXY), maxi_y = get<3>(getMMXY);

	vector <float> norm;
	for (int i = 0; i < vf.size(); i += 2) {
		float scale_x = (dr - st) * (vf[i] - mini_x) / (maxi_x - mini_x) + st;
		float scale_y = (dr - st) * (vf[i + 1] - mini_y) / (maxi_y - mini_y) + st;
		norm.push_back(scale_x);
		norm.push_back(scale_y);
	}
	return norm;
}

int hitRectangle(float xp, float yp, float xjos, float yjos, float xsus, float ysus)
{
	if (xp >= xjos && xp <= xsus && yp >= yjos && yp <= ysus)
		return true;
	return false;
}

void CreateVBO(void)
{
	vector<float> temp{
		  -500.0, -100.0,
			 0.0, -100.0,
			 0.0, 100.0,
		   -500.0, 100.0,
			200.0, -100.0,
			700.0, -100.0,
			700.0, 100.0,
			200.0, 100.0,
			900.0, -100.0,
			1400.0, -100.0,
			1400.0, 100.0,
			900.0, 100.0,
			1600.0, -100.0,
			2100.0, -100.0,
			2100.0, 100.0,
			1600.0, 100.0,
			2300.0, -100.0,
			2800.0, -100.0,
			2800.0, 100.0,
			2300.0, 100.0,
			3000.0, -100.0,
			3500.0, -100.0,
			3500.0, 100.0,
			3000.0, 100.0,
			3700.0, -100.0,
			4200.0, -100.0,
			4200.0, 100.0,
			3700.0, 100.0,
	};
	vector <float> Vertices, VertCircle, Colors;
	Vertices = normVf(temp, -0.65, 0.7);
	xstjos = Vertices[0]; ystjos = Vertices[1];
	xdrsus = Vertices[4]; ydrsus = Vertices[5];
	
	float coord_tr[] = { -1.0, -1.0, -0.5, 0.5, 1.0, 1.0, 1.0, -1.0, -1.0,  -1.0, -1.0, 1.0 };
	for (int i = 0; i < 6; i++)
	{
		Vertices.push_back(coord_tr[i]);
		Vertices.push_back(coord_tr[i + 6]);
	}

	float pas = PI2 / nr_puncte_cerc;
	for (float i = 0.0; i <= PI2; i += pas)
	{
		VertCircle.push_back(x + radius * cos(i));
		VertCircle.push_back(y + radius * sin(i));
	}

	vector < tuple<float, float, float> > Color10 = {
		make_tuple(0.545, 0.0, 0.0),
		make_tuple(0.282, 0.239, 0.545),
		make_tuple(0.0, 0.392, 0.0),
		make_tuple(0.627, 0.322, 0.176),
		make_tuple(0.184, 0.310, 0.31),
		make_tuple(0.275, 0.51, 0.706),
		make_tuple(0.333, 0.42, 0.184),
		make_tuple(0.647, 0.165, 0.165)
	};

	shuffle(Color10.begin(), Color10.end(), default_random_engine(random_device()()));
	for (auto x : Color10)
	{
		for (int j = 0; j < 4; j++)
		{
			Colors.push_back(get<0>(x));
			Colors.push_back(get<1>(x));
			Colors.push_back(get<2>(x));
		}
	}

	glGenBuffers(3, VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, Vertices.size() * sizeof(Vertices), &Vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, VertCircle.size() * sizeof(VertCircle), &VertCircle[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
	glBufferData(GL_ARRAY_BUFFER, Colors.size() * sizeof(Colors), &Colors[0], GL_STATIC_DRAW);

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(2);
}

void DestroyVBO(void)
{
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &VBO[2]);
	glDeleteBuffers(1, &VBO[1]);
	glDeleteBuffers(1, &VBO[0]);

	glBindVertexArray(0);
	glDeleteVertexArrays(1, &VAO);
}

void CreateShaders(void)
{
	ProgramId = LoadShaders("dominoShader.vert", "dominoShader.frag");
	glUseProgram(ProgramId);
}

void DestroyShaders(void)
{
	glDeleteProgram(ProgramId);
}

void Initialize(void)
{
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	CreateVBO();
	CreateShaders();
}

void processKeys(int key, int, int)
{
	if (reversed && x + rtx < -1.0)
		rtx += 0.3;
	switch (key) {
		case GLUT_KEY_LEFT:
		{
			rtx -= 0.1;
			if (hitRectangle(x + rtx, y, xstjos, ystjos, xdrsus, ydrsus))
				rcoliziune = 1;
			else rcoliziune = 0;
			break;
		}
		case GLUT_KEY_RIGHT: {
			rtx += 0.1;
			if (hitRectangle(x + rtx, y, xstjos, ystjos, xdrsus, ydrsus))
				rcoliziune = 1;
			else rcoliziune = 0;
			break;
		}
	}
}

void rtimer(int)
{
	if (rpoz > 24)
		return;
	if (rcoliziune)
		rpoz += 4;
	glutTimerFunc(timer_rs, rtimer, 0);
}

void moveDomino(int button, int state, int x, int y) 
{
	switch (button) {
		case GLUT_LEFT_BUTTON: {
			if (state == GLUT_DOWN)
			{
				if (dr > 6)
					dr = -1;
				if (dr == 0)
				{
					da = 0.4; dx = 0.35; dy = 0.1;
				}
				else
				{
					da -= 0.02; dx -= 0.06; dy -= 0.06;
				}
				if (dr != -1)
				{
					dr++;
					angles[6 - dr + 1] = angles[0] - da;
					xas[6 - dr + 1] = xas[0] - dx;
					yas[6 - dr + 1] = yas[0] - dy;
				}
			}
			break;
		}
	}
}

void RenderFunction(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	if (!reversed)
	{
		glBindVertexBuffer(0, VBO[0], 0, 2 * sizeof(float));
		cod = 1;
		codLoc = glGetUniformLocation(ProgramId, "cod");
		glUniform1i(codLoc, cod);
		glDrawArrays(GL_TRIANGLES, 28, 3);
		cod = 2;
		codLoc = glGetUniformLocation(ProgramId, "cod");
		glUniform1i(codLoc, cod);
		for (int i = 0; i < 28; i += 4)
		{
			trT1 = glm::translate(glm::mat4(1.0f), glm::vec3(xas[(24 - i) / 4], yas[(24 - i) / 4], 0.0));
			rRot = glm::rotate(glm::mat4(1.0f), -angles[(24 - i) / 4], glm::vec3(0.0, 0.0, 1.0));
			mat = rRot * trT1 * glm::translate(glm::mat4(1.0f), glm::vec3(0.15, -0.6, 0.0));
			matLoc = glGetUniformLocation(ProgramId, "mat");
			glUniformMatrix4fv(matLoc, 1, GL_FALSE, &mat[0][0]);
			glDrawArrays(GL_QUADS, 24 - i, 4);
		}
	}
	else
	{
		glBindVertexBuffer(0, VBO[1], 0, 2 * sizeof(float));
		cod = 2;
		codLoc = glGetUniformLocation(ProgramId, "cod");
		glUniform1i(codLoc, cod);
		mat = glm::translate(glm::mat4(1.0f), glm::vec3(rtx, 0.0, 0.0));
		matLoc = glGetUniformLocation(ProgramId, "mat");
		glUniformMatrix4fv(matLoc, 1, GL_FALSE, &mat[0][0]);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 14);

		glBindVertexBuffer(0, VBO[0], 0, 2 * sizeof(float));
		cod = 1;
		codLoc = glGetUniformLocation(ProgramId, "cod");
		glUniform1i(codLoc, cod);
		for (int i = 0; i < 28; i += 4)
			glDrawArrays(GL_QUADS, i, 4);
	
		if (rcoliziune == 1)
		{
			glClear(GL_COLOR_BUFFER_BIT);
			glBindVertexBuffer(0, VBO[0], 0, 2 * sizeof(float));
			glDrawArrays(GL_TRIANGLES, 31, 3);
			float angle = 0.4, xax = 0.0, yax = 0.0;
			cod = 2;
			codLoc = glGetUniformLocation(ProgramId, "cod");
			glUniform1i(codLoc, cod);
			for (int i = 0; i < rpoz; i += 4)
			{
				trT1 = glm::translate(glm::mat4(1.0f), glm::vec3(xax, yax * 3, 0.0));
				rRot = glm::rotate(glm::mat4(1.0f), -angle, glm::vec3(0.0, 0.0, 1.0));
				mat = rRot * trT1 * glm::translate(glm::mat4(1.0f), glm::vec3(0.35, -0.85, 0.0));
				matLoc = glGetUniformLocation(ProgramId, "mat");
				glUniformMatrix4fv(matLoc, 1, GL_FALSE, &mat[0][0]);
				glDrawArrays(GL_QUADS, i, 4);
				angle -= 0.01; xax -= 0.06; yax += 0.03;
			}
		}
	}
	glFlush();
}

void Cleanup(void)
{
	DestroyShaders();
	DestroyVBO();
}

void setTimeHandler(int option) {
	switch (option)
	{
		case 1:
			timer_rs = 500.0;
			break;
		case 2:
			timer_rs = 1000.0;
			break;
		case 3:
			timer_rs = 2000.0;
			break;
		case 4:
			timer_rs = 3000.0;
			break;
	}
}

void setBackgroundHandler(int option)
{
	switch (option)
	{
		case 1:
			glClearColor(1.0, 1.0, 1.0, 0.0);
			break;
		case 2:
			glClearColor(0.596, 0.984, 0.596, 0.0);
			break;
		case 3:
			glClearColor(0.0, 1.0, 1.0, 0.0);
			break;
	}
}

void menu(int num) {
	switch (num)
	{
		case 1:
			exit(0);
			break;
	}
}


void reservedHandler(int option)
{
	switch (option)
	{
		case 1: {
			reversed = 1;
			glutCreateWindow("Domino 2D");
			glewInit();
			Initialize();
			glutDisplayFunc(RenderFunction);
			glutIdleFunc(RenderFunction);
			glutSpecialFunc(processKeys);
			glutTimerFunc(1000, rtimer, 0);
			int setTime = glutCreateMenu(setTimeHandler);
			glutAddMenuEntry("Fast", 1);
			glutAddMenuEntry("1s", 2);
			glutAddMenuEntry("2s", 3);
			glutAddMenuEntry("3s", 4);
			int setBackground = glutCreateMenu(setBackgroundHandler);
			glutAddMenuEntry("Default", 1);
			glutAddMenuEntry("Pale Green", 2);
			glutAddMenuEntry("Cyan", 3);
			int reversed = glutCreateMenu(reservedHandler);
			glutAddMenuEntry("Reverse", 1);
			glutCreateMenu(menu);
			glutAddSubMenu("Set Time Animation", setTime);
			glutAddSubMenu("Set Background", setBackground);
			glutAddMenuEntry("Exit", 1);
			glutAttachMenu(GLUT_RIGHT_BUTTON);
			glutMainLoop();
			break;
		}
	}
}

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(900, 550);
	glutCreateWindow("Domino 2D");
	glewInit();
	Initialize();
	glutDisplayFunc(RenderFunction);
	glutMouseFunc(moveDomino);
	glutIdleFunc(RenderFunction);
	glutSpecialFunc(processKeys);
	int reversed = glutCreateMenu(reservedHandler);
	glutAddMenuEntry("Reverse", 1);
	glutCreateMenu(menu);
	glutAddSubMenu("Reverse Scene", reversed);
	glutAddMenuEntry("Exit", 1);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
	glutCloseFunc(Cleanup);
	glutMainLoop();
}