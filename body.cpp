#include <GL/freeglut_std.h>
#include <GL/glut.h>
#include "vec3.cpp"

class Body {
    private:
        float radius;
        float distance;
        float mass;
        Vec3 position;
        Vec3 speed;

    public:
        Body(float radius, float distance, float mass) {
            this->radius = radius;
            this->distance = distance;
            this->mass = mass;

            this->position = new Vec3(0, 0, distance);
        }

        void math_update(){

        }

        void draw() {
            glColor3f(0.0f, 1.0f, 0.0f);
            glPushMatrix();
            glTranslatef(distance, 0, 0);
            glutSolidSphere(radius, 20, 20);
            glPopMatrix();
        }
};
