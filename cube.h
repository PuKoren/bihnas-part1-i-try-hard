#ifndef _CUBE_H
#define _CUBE_H

class Cube{
    private:

    public:
        Cube();
        ~Cube();
        void Update(Uint32);
        void Draw(SDL_Surface*);
        void CheckCollision(Cube&);
};

#endif