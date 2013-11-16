#include "Enemy.h"

Enemy::Enemy(Vector2 _pos, Vector2 _size, SDL_Color _color): Rectangle(_pos, (int)_size.X, (int)_size.Y){
    this->color = _color;
}

void Enemy::Draw(SDL_Surface* viewport){
    Rectangle::Draw(viewport, this->color);
}

bool Enemy::IsColored(SDL_Color _color){
    if(this->color.r == _color.r && this->color.g == _color.g && this->color.b == _color.b){
        return true;
    }else{
        return false;
    }
}