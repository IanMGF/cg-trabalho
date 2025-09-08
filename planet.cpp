#include <GL/freeglut_std.h>
#include <GL/glut.h>
#include <vector>

class Planet {
    private:
        float radius;
        float distance;
        float mass;
        
    public:
        Planet(float radius, float distance, float mass) {
            this->radius = radius;
            this->distance = distance;
            this->mass = mass;
        }
    
        void draw() {
            glColor3f(0.0f, 1.0f, 0.0f);
            glPushMatrix();
            glTranslatef(distance, 0, 0);
            glutSolidSphere(radius, 20, 20);
            glPopMatrix();
        }
};
