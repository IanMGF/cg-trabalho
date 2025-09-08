#include <GL/freeglut_std.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <iostream>
GLfloat angle, fAspect, largura, altura, xcamera, ycamera, zcamera;

void update(int _) {

}

void draw(void) {
	//glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Desenha objetos 3D (wire-frame)
    glColor3f(1.0f, 0.5f, 0.0f);
    glPushMatrix();
        glTranslated(0, 0, 0);
        glutWireSphere(30.0, 20, 20);
    glPopMatrix();
    
    glColor3f(1.0, 0.1, 0.1);
    glPushMatrix();
        glTranslated(50, 0, 0);
        glutWireSphere(20.0, 20, 20);
    glPopMatrix();
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
void EspecificaParametrosVisualizacao(void)
{
	// Especifica sistema de coordenadas de projeção
	glMatrixMode(GL_PROJECTION);
	// Inicializa sistema de coordenadas de projeção
	glLoadIdentity();
	// Especifica a projeção perspectiva
	gluPerspective(angle, fAspect, 0.01, 1000);

	// Especifica sistema de coordenadas do modelo
	glMatrixMode(GL_MODELVIEW);
	// Inicializa sistema de coordenadas do modelo
	glLoadIdentity();

	// Especifica posição do observador e do alvo
	gluLookAt(50, ycamera, 300,  // posição da câmera
              0, 0, 0,          // posição do alvo
              0, 1, 0);         // vetor UP da câmera
}

// Função callback chamada quando o tamanho da janela é alterado
void AlteraTamanhoJanela(GLint largura, GLint altura)
{
	// Para previnir uma divisão por zero
	if (altura == 0) altura = 1;

	// Especifica o tamanho da viewport
	glViewport(0, 0, largura, altura);

	// Calcula a correção de aspecto
	fAspect = (GLfloat)largura / (GLfloat)altura;

	EspecificaParametrosVisualizacao();
}

// Função callback chamada para gerenciar teclas especiais
void TeclasEspeciais(int key, int x, int y)
{
	if (key == GLUT_KEY_UP) {
		ycamera += 10;
	}
	if (key == GLUT_KEY_DOWN) {
		ycamera -= 10;
	}
	/*
	if (key == GLUT_KEY_RIGHT) {

	}
	if (key == GLUT_KEY_LEFT) {

	}
	*/
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);   //aplica o zBuffer
    EspecificaParametrosVisualizacao();
	glutPostRedisplay();
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
    EspecificaParametrosVisualizacao();
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
    EspecificaParametrosVisualizacao();
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
    ycamera = 50;
	angle = 45;
    glutCreateWindow("Sistema Solar");

    glutTimerFunc(100, update, 0);
	glutDisplayFunc(draw);
	glutReshapeFunc(AlteraTamanhoJanela); // Função para ajustar o tamanho da tela
    //glutMouseFunc(GerenciaMouse);
    glutKeyboardFunc(GerenciaTeclado); // Define qual funcao gerencia o comportamento do teclado
    glutSpecialFunc(TeclasEspeciais); // Define qual funcao gerencia as teclas especiais

    // Start
    setup();
	glutMainLoop();

}
