#ifndef _RECTANGLE_H
#define _RECTANGLE_H

#include <SDL.h>
#include "Vector2.h"

class Rectangle{
	public:
		Rectangle();
		Rectangle(Rectangle *);
		~Rectangle();
        Rectangle(Vector2, int, int);
		Rectangle(float, float, int, int);
		Vector2 Position;
		int Width;
		int Height;
		bool CollideWith(Rectangle *);
        void Draw(SDL_Surface*, SDL_Color);
		void Draw(SDL_Surface*, int, int, int);
};
#endif