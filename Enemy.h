#include <SDL.h>
#include "Vector2.h"
#include "Rectangle.h"

class Enemy: public Rectangle{
    private:
        SDL_Color color;
    public:
        Enemy(Vector2, Vector2, SDL_Color);
        ~Enemy();
        void Draw(SDL_Surface*);
        bool IsColored(SDL_Color);
};