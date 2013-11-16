#ifndef _INGAME_H
#define _INGAME_H
#if PSP
#include <pspctrl.h>
#endif
#include <SDL.h>
#include <SDL_ttf.h>
#include <sstream>
#include <random>
#include <time.h>
#include <vector>
#include "Enemy.h"
#include "Vector2.h"
#include "Rectangle.h"
#include "config.h"

class Ingame{
    private:
        int lifes;
        int score;
        int combo;
        void AddScore(int);
        void SetScore(int);
        void SetCombo(int);
        void AddLife();
        void RemoveLife(GAME_STATE*);

        int difficulty;
        int waveCount;
        int generationCooldown;
        int generationCurrentCooldown;
        float cubeSpeed;
        Rectangle playerCube;
        Vector2 direction;
        Vector2 gravity;
        float gravitySpeed;
        std::vector<Enemy*> waves;
        bool IsOutOfBounds(Enemy* e);
        void DeleteEnemy(int);
        void GenerateWave();
        int GetRandom(int);
        
        void RefreshTextSurface(SDL_Surface** surf, int value);
        SDL_Surface* GetIntTextSurface(int number);
        SDL_Surface* textLife;
        SDL_Surface* textScore;
        SDL_Surface* textCombo;
        TTF_Font* gameFont;
        void DrawUi(SDL_Surface*);
        bool canPause;

    public:
        Ingame();
        ~Ingame();
        void Update(Uint32, GAME_STATE*);
        void Event(SDL_Event*, GAME_STATE*);
        #if PSP
        void Event(SceCtrlData*, SDL_Event*, GAME_STATE*);
        #endif
        void Draw(SDL_Surface*);
        void Init();
};

#endif
