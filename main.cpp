#include <GL/freeglut_std.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <iostream>
#include <cmath>
#include "body.cpp"

#define DELTA 1000

GLfloat angle, fAspect, largura, altura, xcamera, ycamera, zcamera;

Body bodies[10] = {
    // Sun
    Body(2000, 0, 4E+11, Vec3(1, 1, 0.4)),

    // Planets

    // Ash Twin
    Body(169, 5250, 1.6E+6, Vec3(0.85, 0.31, 0.11)),
    // Ember Twin
    Body(170, 4750, 1.6E+6, Vec3(0.89, 0.62, 0.29)),

    // Timber Heart
    Body(254, 8600, 3E+6, Vec3(0.22, 0.38, 0.27)),
    // Attlerock
    Body(80, 9500, 5E+7, Vec3(0.62, 0.49, 0.42)),

    // Brittle Hollow
    Body(272, 11700, 3E+6, Vec3(0.31, 0.26, 0.34)),
    // Hollow's Lantern
    Body(97, 12700, 9.1E+5, Vec3(1, 0.75, 0.13)),

    // Giant's Deep
    Body(500, 16460, 2.2E+7, Vec3(0.11, 0.42, 0.27)),

    // Dark Bramble
    Body(203.3, 20000, 3.25E+6, Vec3(0.3, 0.13, 0.12)),

    // Interloper
    Body(83, 19000, 5.5E+6, Vec3(0.13, 0.48, 0.62)),

    // Quantum Moon
    //Body(100, 0, 0, Vec3(1, 1, 1)),
};

void update(int _) {
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            if (i != j) {
                float acc_factor = bodies[i].mass * bodies[j].mass / pow((bodies[i].position - bodies[j].position).length(), 2);
                Vec3 acc = (bodies[j].position - bodies[i].position).unitary() * acc_factor;
                bodies[i].velocity = bodies[i].velocity + acc * DELTA * 0.001;
            }
        }

        bodies[i].position = bodies[i].position + bodies[i].velocity; // * DELTA * 0.001 * 0;
        
        Vec3 pos_delta = bodies[i].velocity * DELTA * 0.001 * 0;
    }
    
    glutPostRedisplay();
    glutTimerFunc(DELTA, update, 0);
}

void draw(void) {
	//glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Desenha objetos 3D (wire-frame)
	for (Body body : bodies) {
	    glColor3f(body.color.x, body.color.y, body.color.z);
		glPushMatrix();
			glTranslated(body.position.x, body.position.y, body.position.z);
			glutWireSphere(body.radius, 20, 20);
		glPopMatrix();
	}
	
	glutSwapBuffers();
}

void setup(void)
{
    // Slightly blue tone
	glClearColor(0.0f, 0.0f, 0.1f, 1.0f);

    glEnable(GL_DEPTH_TEST);

	angle = 45;
}

// Função usada para especificar o volume de visualização
void view_setup(void)
{
	// Especifica sistema de coordenadas de projeção
	glMatrixMode(GL_PROJECTION);
	// Inicializa sistema de coordenadas de projeção
	glLoadIdentity();
	// Especifica a projeção perspectiva
	gluPerspective(90, fAspect, 0.01, 1E+9);

	// Especifica sistema de coordenadas do modelo
	glMatrixMode(GL_MODELVIEW);
	// Inicializa sistema de coordenadas do modelo
	glLoadIdentity();

	// Especifica posição do observador e do alvo
	gluLookAt(0, 20000, 0,  // posição da câmera
              0, 0, 0,          // posição do alvo
              0, 0, 1);         // vetor UP da câmera
}

// Função callback chamada quando o tamanho da janela é alterado
void window_change_callback(GLint largura, GLint altura)
{
	// Para previnir uma divisão por zero
	if (altura == 0) altura = 1;

	// Especifica o tamanho da viewport
	glViewport(0, 0, largura, altura);

	// Calcula a correção de aspecto
	fAspect = (GLfloat)largura / (GLfloat)altura;

	view_setup();
}

// Função callback chamada para gerenciar teclas especiais
void TeclasEspeciais(int key, int x, int y)
{
	if (key == GLUT_KEY_UP) {
		// ycamera += 10;
	}
	if (key == GLUT_KEY_DOWN) {
		// ycamera -= 10;
	}
	/*
	if (key == GLUT_KEY_RIGHT) {

	}
	if (key == GLUT_KEY_LEFT) {

	}
	*/
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);   //aplica o zBuffer
    view_setup();
	// glutPostRedisplay();
}
// Função callback chamada para gerenciar teclado
void GerenciaTeclado(unsigned char key, int x, int y) {
	switch (key) {
		case ' ':
			ycamera = 50;
            xcamera = 50;
            zcamera = 100;
			break;
/*
			// movimentacao da camera em z
		case 'w':

        case 's':

*/
	}
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);   //aplica o zBuffer
    view_setup();
	glutPostRedisplay();
}


// Função callback chamada para gerenciar eventos do mouse
void mouse_callback(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON){

	}
	if (button == GLUT_RIGHT_BUTTON){

	}
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    view_setup();
	glutPostRedisplay();
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);  //GLUT_DOUBLE trabalha com dois buffers: um para renderização e outro para exibição

    glutInitWindowPosition(700,100);
    largura = 600;
    altura = 500;
	glutInitWindowSize(largura,altura);
	fAspect = (GLfloat)largura / (GLfloat)altura;
	angle = 90;
    glutCreateWindow("Sistema Solar");

    glutTimerFunc(DELTA, update, 0);
	glutDisplayFunc(draw);
	glutReshapeFunc(window_change_callback); // Função para ajustar o tamanho da tela
    //glutMouseFunc(GerenciaMouse);
    glutKeyboardFunc(GerenciaTeclado); // Define qual funcao gerencia o comportamento do teclado
    glutSpecialFunc(TeclasEspeciais); // Define qual funcao gerencia as teclas especiais

    // Start
    setup();
	glutMainLoop();

}
