#include "Ingame.h"

Ingame::Ingame(){
    this->gameFont = TTF_OpenFont("visitor2.ttf", SCREEN_WIDTH/20);
    this->textCombo = NULL;
    this->textLife = NULL;
    this->textScore = NULL;
    this->Init();
    std::srand(10);
}

Ingame::~Ingame(){
    if(this->textCombo != NULL){
        SDL_FreeSurface(this->textCombo);
        SDL_FreeSurface(this->textLife);
        SDL_FreeSurface(this->textScore);
    }
    TTF_CloseFont(this->gameFont);
}

void Ingame::Init(){
    this->SetScore(0);
    this->SetCombo(1);
    this->lifes = 4;
    this->RefreshTextSurface(&this->textLife, this->lifes);

	#if PSP
    this->playerCube.Width = 2;
    this->playerCube.Height = 2;
    this->cubeSpeed = 0.04f;
    this->gravitySpeed = 0.025f;
    #else
    this->playerCube.Width = 3;
    this->playerCube.Height = 3;
    this->cubeSpeed = 0.08f;
    this->gravitySpeed = 0.05f;
    #endif
    
    this->generationCurrentCooldown = 1000;
    this->generationCooldown = 1000;
    this->difficulty = 1;

    this->direction.X = 0.0f;
    this->direction.Y = 0.0f;

    this->gravity.X = -1.f;
    this->gravity.Y = 0.f;

    this->playerCube.Position.X = SCREEN_WIDTH / 10;
    this->playerCube.Position.Y = SCREEN_HEIGHT / 2;

    this->canPause = false;

    this->waveCount = 1;
    for(unsigned int i = 0; i < this->waves.size(); i++){
        this->DeleteEnemy(i);
        i--;
    }
}

void Ingame::Event(SDL_Event* ev, GAME_STATE* gs){
    if(ev->type == SDL_KEYDOWN){
        if(ev->key.keysym.sym == SDLK_DOWN){
            this->direction.Y += this->cubeSpeed;
        }else if(ev->key.keysym.sym == SDLK_UP){
            this->direction.Y -= this->cubeSpeed;
        }else if(ev->key.keysym.sym == SDLK_RIGHT){
            this->direction.X += this->cubeSpeed;
        }else if(ev->key.keysym.sym == SDLK_LEFT){
            this->direction.X -= this->cubeSpeed;
        }else if(ev->key.keysym.sym == SDLK_ESCAPE){
            *gs = PAUSE;
        }
    }else if(ev->type == SDL_KEYUP){
        if(ev->key.keysym.sym == SDLK_DOWN){
            this->direction.Y -= this->cubeSpeed;
        }else if(ev->key.keysym.sym == SDLK_UP){
            this->direction.Y += this->cubeSpeed;
        }else if(ev->key.keysym.sym == SDLK_RIGHT){
            this->direction.X -= this->cubeSpeed;
        }else if(ev->key.keysym.sym == SDLK_LEFT){
            this->direction.X += this->cubeSpeed;
        }
    }
}

#if PSP
void Ingame::Event(SceCtrlData* pad, SDL_Event* ev, GAME_STATE* gs){
    if(pad->Buttons != 0)
    {
        if(this->canPause && (pad->Buttons & PSP_CTRL_START || pad->Buttons & PSP_CTRL_TRIANGLE || pad->Buttons & PSP_CTRL_SELECT)){
            *gs = PAUSE;
            this->canPause = false;
        }
        
        if(pad->Buttons & PSP_CTRL_UP){
            this->direction.Y = -this->cubeSpeed;
        }else if(pad->Buttons & PSP_CTRL_DOWN){
            this->direction.Y = this->cubeSpeed;
        }else{
        	this->direction.Y = 0.f;
        }
        
        if(pad->Buttons & PSP_CTRL_RIGHT){
            this->direction.X = this->cubeSpeed;
        }else if(pad->Buttons & PSP_CTRL_LEFT){
            this->direction.X = -this->cubeSpeed;
        }else{
        	this->direction.X = 0.f;
        }
    }else{
        this->direction.Y = 0.0f;
        this->direction.X = 0.0f;
        this->canPause = true;
    }
}
#endif

void Ingame::Update(Uint32 gameTime, GAME_STATE* gs){
    this->playerCube.Position =  this->playerCube.Position + this->direction * (float)gameTime;

    this->generationCurrentCooldown += gameTime;
    if(this->generationCurrentCooldown >= this->generationCooldown){
        
        if(this->waveCount % 20 == 0){
            float direction = (this->GetRandom(2) == 2)?-1.0f:1.0f;
            if(this->GetRandom(2) == 2){
                this->gravity.Y = 1.f * direction;
                this->gravity.X = 0.f;
            }else{
                this->gravity.Y = 0.f;
                this->gravity.X = 1.f * direction;
            }
            this->difficulty -= 4;
        }

        this->GenerateWave();
        if(this->difficulty < 23){
            this->difficulty++;
        }

        this->generationCurrentCooldown -= this->generationCooldown;
        this->generationCooldown += this->generationCurrentCooldown;
    }

    for(unsigned int i = 0; i < this->waves.size(); i++){
        this->waves[i]->Position = this->waves[i]->Position + (this->gravity* this->gravitySpeed * (float)gameTime);
        if(this->IsOutOfBounds(this->waves[i])){
            this->DeleteEnemy(i);
            i--;
        }else{
            if(this->waves[i]->CollideWith(&this->playerCube)){
                if(this->waves[i]->IsColored(BLUE)){
                    this->RemoveLife(gs);
                }else if(this->waves[i]->IsColored(GREEN)){
                    this->SetCombo(this->combo+1);
                }else if(this->waves[i]->IsColored(YELLOW)){
                    this->AddLife();
                    this->AddScore(10000);
                }
                this->DeleteEnemy(i);
                i--;
            }
        }
    }

    this->AddScore(gameTime);
}

bool Ingame::IsOutOfBounds(Enemy* e){
    if(this->gravity.X < 0 && e->Position.X + e->Width < 0){
        return true;
    }else if(this->gravity.X > 0 && e->Position.X > SCREEN_WIDTH){
        return true;
    }
    if(this->gravity.Y > 0 && e->Position.Y > SCREEN_HEIGHT){
        return true;
    }else if(this->gravity.Y < 0 && e->Position.Y + e->Height < 0){
        return true;
    }

    return false;
}

void Ingame::AddScore(int score){
    this->score += score * this->combo;
    this->RefreshTextSurface(&this->textScore, this->score);
}

void Ingame::SetScore(int score){
    this->score = score;
    this->RefreshTextSurface(&this->textScore, this->score);
}

void Ingame::SetCombo(int combo){
    this->combo = combo;
    this->RefreshTextSurface(&this->textCombo, this->combo-1);
}

void Ingame::AddLife(){
    this->lifes++;
    this->RefreshTextSurface(&this->textLife, this->lifes);
}

void Ingame::RemoveLife(GAME_STATE* gs){
    this->lifes--;
    this->SetCombo(1);
    if(this->lifes < 0){
        *gs = GAMEOVER;
        this->RefreshTextSurface(&this->textLife, 0);
    }else{
        this->RefreshTextSurface(&this->textLife, this->lifes);
    }
}

void Ingame::RefreshTextSurface(SDL_Surface** surf, int value){
    if(*surf != NULL){
        SDL_FreeSurface(*surf);
    }
    *surf = this->GetIntTextSurface(value);
}

SDL_Surface* Ingame::GetIntTextSurface(int number){
    std::ostringstream s;
    s << number;
    return TTF_RenderUTF8_Solid(this->gameFont, s.str().c_str(), WHITE);
}

void Ingame::DeleteEnemy(int index){
    delete (Rectangle*)this->waves[index];
    this->waves.erase(this->waves.begin()+index);
}

void Ingame::GenerateWave(){
    for(int i = 0; i < this->difficulty * 3; i++){
        SDL_Color cubeColor = BLUE;
        if(this->GetRandom(50) == 1){
            cubeColor = GREEN;
        }
        if(this->GetRandom(500) == 1){
            cubeColor = YELLOW;
        }
        #if PSP
        this->waves.push_back(new Enemy(Vector2(this->GetRandom(SCREEN_WIDTH) + (SCREEN_WIDTH * -(int)this->gravity.X), this->GetRandom(SCREEN_HEIGHT) + (SCREEN_HEIGHT * -(int)this->gravity.Y)), Vector2(this->GetRandom(this->difficulty)/2, this->GetRandom(this->difficulty)/2), cubeColor));
        #else
        this->waves.push_back(new Enemy(Vector2(this->GetRandom(SCREEN_WIDTH) + (SCREEN_WIDTH * -(int)this->gravity.X), this->GetRandom(SCREEN_HEIGHT) + (SCREEN_HEIGHT * -(int)this->gravity.Y)), Vector2(this->GetRandom(this->difficulty), this->GetRandom(this->difficulty)), cubeColor));
        #endif
    }
    this->waveCount++;
}

int Ingame::GetRandom(int limit){
    return rand()%limit+1;
}

void Ingame::Draw(SDL_Surface* viewport){
    for(unsigned int i = 0; i < this->waves.size(); i ++){
        this->waves[i]->Draw(viewport);
    }
    this->playerCube.Draw(viewport, RED);
    this->DrawUi(viewport);
}

void Ingame::DrawUi(SDL_Surface* viewport){
    SDL_Rect destR;
    destR.x = 5;
    destR.y = 0;
    destR.w = this->textLife->w;
    destR.h = this->textLife->h;
    SDL_BlitSurface(this->textLife, NULL, viewport, &destR);

    destR.x = SCREEN_WIDTH/2;
    destR.y = 0;
    destR.w = this->textScore->w;
    destR.h = this->textScore->h;
    SDL_BlitSurface(this->textScore, NULL, viewport, &destR);

    destR.x = SCREEN_WIDTH - 30;
    destR.y = 0;
    destR.w = this->textCombo->w;
    destR.h = this->textCombo->h;
    SDL_BlitSurface(this->textCombo, NULL, viewport, &destR);
}
