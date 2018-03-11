//Bibliothèques necessaires
#include <stdio.h>
#include <stdlib.h>
#include <SDL\SDL.h>
#include <time.h>

///Configuration arene
#define ARENE_WIDTH 7
#define ARENE_HEIGHT 5

///Config joueur
#define LIFE 0
#define MANA 1
#define SCORE 2
#define P_HEIGHT 3
#define P_WIDTH 4

///Action
#define MOVE 0
#define ATTACK 1
#define SPECIAL 2

///Mouvement
#define DROITE 0
#define GAUCHE 1
#define HAUT 2
#define BAS 3

///SDL Sprites
#define ARENE_FILE "areneresize.bmp"
#define CHARACTERS {"Anim.bmp", "Anim2.bmp", "Anim3.bmp", "Anim4.bmp"}
#define SHEET_WIDTH 720
#define SHEET_HEIGHT 90

///SDL Windows
#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720
#define ARENE_WIDTH_SIZE 1150
#define ARENE_HEIGHT_SIZE 525
#define BORDER_HEIGHT 74
#define BORDER_WIDTH 67





///Fonction squelette interne

int **Init_arena(); ///Initaialisation du plateau
int **Init_player(int nb_joueur); ///Initialisation des joueurs
void Random_place(int **arena, int **players, int nb_joueur); ///Placement aléatoire des joueurs

void start_game(int **arena, int **players, int nb_joueur, SDL_Surface *screen, SDL_Surface **sprites, int coeff[2], SDL_Rect *pos_joueur, SDL_Rect *clip, SDL_Surface *background);/// Demarre la partie

///Fonctions d'action
void player_movement(int **arena, int **players, int mouvement, int joueur); ///Deplace le joueur
int presence(int **arena, int mouvement, int x, int y); ///Regarde s'il y a la présence d'un joueur ou d'une bordure
void attack(int **arena, int **players, int joueur); ///Attacque les adversaires dans une zone de
void special(int **players, int joueur, int nb_joueur, int **arena, int mana); ///Attaque spécial du joueur

///Fonctions utilitaires
void mana_update(int **players, int nb_joueur); /// Augmente la mana de 1 à chaque tour
int isDead(int **arena, int life, int joueur, int x, int y); /// Regarde si le joueur est mort
void winner(int **arena, int **players);///Affiche le gagnant
int gameover(int **arena);/// Condition d'arret du jeu

///Fonctions SDL
SDL_Surface **graph_init_sprite(int nb_joueur);
SDL_Rect *graph_init_pos(int nb_joueur, SDL_Surface **sprites, SDL_Surface *screen,int **players, int coeff[2]);
SDL_Rect *graph_init_clip();
void graph_mouvement(SDL_Surface *screen, SDL_Surface *background, SDL_Surface **sprites, int joueur, int mouvement, int coeff[2], int x, int y, SDL_Rect *pos_joueur, int nb_joueur, SDL_Rect *clip);
void graph_display_update(SDL_Surface *screen, SDL_Surface *background, SDL_Surface **sprites, SDL_Rect *pos_joueur, int nb_joueur, SDL_Rect *clip);
void graph_player_update(SDL_Surface *screen, SDL_Surface *background,  SDL_Surface **sprites, SDL_Rect *pos_joueur, int nb_joueur, SDL_Rect *clip, SDL_Rect *defaultclip, int joueur);


int main(int argc, char *argv[])
{
    srand(time(NULL));
    SDL_Surface *screen, **sprites, *background;
    SDL_Rect *clip;
    int nb_joueur;

    int coeff[2];
    coeff[0] = (int) ARENE_WIDTH_SIZE / ARENE_WIDTH;
    coeff[1] = (int) ARENE_HEIGHT_SIZE / ARENE_HEIGHT;

    if(SDL_Init( SDL_INIT_VIDEO ) == -1)
    {
        fprintf(stderr, "Error: %s\n", SDL_GetError());
        exit( EXIT_FAILURE );
    }

    screen = SDL_SetVideoMode(WINDOW_WIDTH, WINDOW_HEIGHT, 32, SDL_SWSURFACE);


    nb_joueur = 4;

    int **arena = Init_arena();
    int **players = Init_player(nb_joueur);


    Random_place(arena, players, nb_joueur);

    sprites = graph_init_sprite(nb_joueur);
    clip = graph_init_clip();
    background = SDL_DisplayFormat(SDL_LoadBMP(ARENE_FILE));

    SDL_Rect *pos_joueur = graph_init_pos(nb_joueur, sprites, screen, players, coeff);

    start_game(arena, players, nb_joueur, screen, sprites,coeff, pos_joueur, clip, background);

    winner(arena, players);

    SDL_free(screen);
    SDL_free(background);
    SDL_free(clip);
    SDL_Quit();
    return EXIT_SUCCESS;
}

int **Init_arena()
{

    int **tab = (int**) malloc(ARENE_HEIGHT * sizeof(int*));

    int i, j;

    for(i = 0; i < ARENE_HEIGHT; i++)
    {
        tab[i] = (int*) malloc(ARENE_WIDTH * sizeof(int));
    }

    for(i = 0; i < ARENE_HEIGHT; i++)
    {
        for(j = 0; j < ARENE_WIDTH; j++)
        {
            tab[i][j] = 0;
        }
    }

    return tab;
}

int **Init_player(int nb_joueur)
{

    int i;

    int **tab_player = (int**) malloc(nb_joueur * sizeof(int*));

    for(i = 0; i < nb_joueur; i++)
    {
        tab_player[i] = (int*) malloc(5 * sizeof(int));
    }

    for(i = 0; i < nb_joueur; i++)
    {
        tab_player[i][LIFE] = 10;
        tab_player[i][MANA] = 5;
        tab_player[i][SCORE] = 0;
        tab_player[i][P_HEIGHT] = 0;
        tab_player[i][P_WIDTH] = 0;
    }

    return tab_player;
}

void Random_place(int **arena, int **players, int nb_joueur)
{

    int height, width, i;
    int pass;

    for(i = 0; i < nb_joueur; i++)
    {
        do{
            width = rand()%ARENE_WIDTH;
            height = rand()%ARENE_HEIGHT;
            pass = 0;

            if(arena[height][width] != 0)
            {
                pass = 1;
            }
            else
            {
                players[i][P_HEIGHT] = height;
                players[i][P_WIDTH] = width;
                arena[height][width] = i + 1;
            }
        }while(pass);
    }

}

void start_game(int **arena, int **players, int nb_joueur, SDL_Surface *screen, SDL_Surface **sprites, int coeff[2], SDL_Rect *pos_joueur, SDL_Rect *clip, SDL_Surface *background)
{
    int i;
    int action[2];

    do{
        for(i = 0; i < nb_joueur; i++)
        {
        if(players[i][LIFE])
        {
            mana_update(players, nb_joueur);
            action[0] = 0;
            printf("Action du joueur %d: %d\n",i +1,action[0]);

            switch(action[0]){

                    case MOVE:
                    action[1] = rand()%4;
                    if(!presence(arena, action[1], players[i][P_HEIGHT], players[i][P_WIDTH]))
                    {
                        continue;
                    }
                    else
                    {
                        player_movement(arena, players, action[1], i);
                        graph_mouvement(screen, background, sprites, i, action[1], coeff, players[i][P_HEIGHT], players[i][P_WIDTH], pos_joueur, nb_joueur, clip);
                    }
                    break;
                    case ATTACK:
                    attack(arena, players, i);
                    break;

                    case SPECIAL:
                    special(players, i, nb_joueur, arena,players[i][MANA]);
                    break;

                    default:
                    printf("error\n");
                }
            }
        }
    }while(1);//gameover(arena));


}

void player_movement(int **arena, int **players, int mouvement, int joueur)
{

    if(mouvement < 0 || mouvement > 3)
    {
        printf("error\n");
        return;
    }

    switch(mouvement)
    {
        case DROITE:
            arena[players[joueur][P_HEIGHT]][players[joueur][P_WIDTH]] = 0;
            players[joueur][P_WIDTH]++;
            arena[players[joueur][P_HEIGHT]][players[joueur][P_WIDTH]] = joueur + 1;
            break;
        case GAUCHE:
            arena[players[joueur][P_HEIGHT]][players[joueur][P_WIDTH]] = 0;
            players[joueur][P_WIDTH]--;
            arena[players[joueur][P_HEIGHT]][players[joueur][P_WIDTH]] = joueur + 1;
            break;
        case HAUT:
            arena[players[joueur][P_HEIGHT]][players[joueur][P_WIDTH]] = 0;
            players[joueur][P_HEIGHT]--;
            arena[players[joueur][P_HEIGHT]][players[joueur][P_WIDTH]] = joueur + 1;
            break;
        case BAS:
            arena[players[joueur][P_HEIGHT]][players[joueur][P_WIDTH]] = 0;
            players[joueur][P_HEIGHT]++;
            arena[players[joueur][P_HEIGHT]][players[joueur][P_WIDTH]] = joueur + 1;
            break;
    }
}

int presence(int **arena, int mouvement, int x, int y)
{
    switch(mouvement)
    {
        case GAUCHE:
            if(y-1 > -1 && arena[x][y-1] == 0){return 1;}
            else{return 0;}

        case DROITE:
            if(y+1 < 5 && arena[x][y+1] == 0){return 1;}
            else{return 0;}

        case HAUT:
            if(x-1 > -1 && arena[x-1][y] == 0){return 1;}
            else{return 0;}

        case BAS:
            if( x+1 < 5  && arena[x+1][y] == 0){return 1;}
            else{return 0;}
    }
    return 0;
}

void attack(int **arena, int **players, int joueur)
{

    int i, j;

    for(i = players[joueur][P_HEIGHT] - 1; i <= players[joueur][P_HEIGHT] + 1; i++)
    {
        for(j = players[joueur][P_WIDTH] - 1; j <= players[joueur][P_WIDTH] + 1; j++)
        {
            if(i > 4 || i < 0 || j > 4 || j < 0)
            {
                continue;
            }
            else if(arena[i][j] != 0 && arena[i][j] != joueur + 1)
            {
                players[arena[i][j]-1][LIFE]--;

                if(isDead(arena, players[arena[i][j]-1][LIFE], arena[i][j]-1, i, j))
                {
                    players[i][MANA] = 0;
                    players[joueur][SCORE]++;
                }

            }
        }
    }
}

void special(int **players, int joueur, int nb_joueur, int **arena, int mana)
{
    int i;

    if(mana < 8)
    {
        return;
    }

    printf("Attaque spécial de %d\n\n",joueur + 1);

    players[joueur][MANA] -= 8;

    for(i = 0; i < nb_joueur; i++)
    {
        if(i != joueur && players[i][LIFE] != 0)
        {
            players[i][LIFE]--;
            if(isDead(arena, players[i][LIFE], i, players[i][P_HEIGHT], players[i][P_WIDTH]))
                {
                    players[i][MANA] = 0;
                    players[joueur][SCORE]++;
                }
        }
        else
        {
            continue;
        }
    }
}

int isDead(int **arena, int life, int joueur, int x, int y)
{
    if(!life)
    {
        printf("Le joueur %d est mort\n", joueur + 1);
        arena[x][y] = 0;
        return 1;
    }

    return 0;
}

void mana_update(int **players, int nb_joueur)
{
    int i;

    for(i = 0; i < nb_joueur; i++)
    {
        if((players[i][MANA] == 10) || (players[i][LIFE] == 0))
        {
            continue;
        }
        else
        {
            players[i][MANA]++;
        }
    }
}

int gameover(int **arena)
{
    int i, j;
    int vivant = 0;

    for(i = 0; i < ARENE_HEIGHT; i++)
    {
        for(j = 0; j < ARENE_WIDTH; j++)
        {
            if(arena[i][j])
            {
             vivant++;
            }
        }
    }

    if(vivant == 1){
        return 0;
    }

    return 1;
}

void winner(int **arena, int **players)
{
    int i, j;

    for(i = 0; i < ARENE_HEIGHT; i++)
    {
        for(j = 0; j < ARENE_WIDTH; j++)
        {
            if(arena[i][j])
            {
                printf("Le joueur %d gagne avec %d de score", arena[i][j],players[arena[i][j] - 1][SCORE]);
            }
        }
    }
}

SDL_Surface **graph_init_sprite(int nb_joueur)
{
    const char* sprite_file[nb_joueur] = CHARACTERS;
    SDL_Surface **sprites = (SDL_Surface**) malloc(nb_joueur * sizeof(SDL_Surface*));
    int i;

    for(i = 0; i < nb_joueur; i++)
    {

        sprites[i] = SDL_DisplayFormat(SDL_LoadBMP(sprite_file[i]));

        SDL_SetColorKey(sprites[i], SDL_SRCCOLORKEY, SDL_MapRGB(sprites[i]->format, 255, 0, 255));
    }

    return sprites;
}

SDL_Rect *graph_init_clip()
{
    int i;
    SDL_Rect *clip = (SDL_Rect*) malloc(8 * sizeof(SDL_Rect));

    for(i = 0; i < 8; i++)
    {
        clip[ i ].x = i * (SHEET_WIDTH/8);
        clip[ i ].y = 0;
        clip[ i ].w = SHEET_WIDTH/8;
        clip[ i ].h = SHEET_HEIGHT;
    }

    return clip;
}

SDL_Rect *graph_init_pos(int nb_joueur, SDL_Surface **sprites, SDL_Surface *screen,int **players, int coeff[2])
{
    int i;
    SDL_Rect *pos_joueur = (SDL_Rect*) malloc(nb_joueur * sizeof(SDL_Rect));

    SDL_FillRect( screen, &screen->clip_rect, SDL_MapRGB( screen->format, 0xFF, 0xFF, 0xFF ) );

    for(i = 0; i < nb_joueur; i++)
    {
        pos_joueur[i].x = players[i][P_WIDTH] * coeff[0] + BORDER_WIDTH;
        pos_joueur[i].y = players[i][P_HEIGHT] * coeff[1] + BORDER_HEIGHT;
    }

    return pos_joueur;
}

void graph_display_update(SDL_Surface *screen, SDL_Surface *background, SDL_Surface **sprites, SDL_Rect *pos_joueur, int nb_joueur, SDL_Rect *clip)
{
    int i;

    SDL_BlitSurface(background, NULL, screen, NULL);

    for(i = 0; i < nb_joueur; i++)
    {
        SDL_BlitSurface(sprites[i], clip, screen, &pos_joueur[i]);
    }

    SDL_Flip(screen);
}

void graph_player_update(SDL_Surface *screen, SDL_Surface *background,SDL_Surface **sprites, SDL_Rect *pos_joueur, int nb_joueur, SDL_Rect *clip, SDL_Rect *defaultclip, int joueur)
{
    int i;


    SDL_BlitSurface( background, NULL, screen, NULL);

    for(i = 0; i < nb_joueur; i++)
    {
        if(i == joueur)
        {
            SDL_BlitSurface(sprites[i], clip, screen, &pos_joueur[i]);
        }
        else
        {
            SDL_BlitSurface(sprites[i], defaultclip, screen, &pos_joueur[i]);
        }
    }

    SDL_Flip(screen);
}

void graph_mouvement(SDL_Surface *screen, SDL_Surface *background, SDL_Surface **sprites, int joueur, int mouvement, int coeff[2], int y, int x, SDL_Rect *pos_joueur, int nb_joueur, SDL_Rect *clip)
{
    do
    {
        switch(mouvement)
        {
            case DROITE:
                pos_joueur[joueur].x++;
                graph_player_update(screen, background, sprites, pos_joueur, nb_joueur, &clip[2], &clip[2], joueur);
                graph_player_update(screen, background, sprites, pos_joueur, nb_joueur, &clip[3], &clip[2], joueur);
                break;

            case GAUCHE:
                pos_joueur[joueur].x--;
                graph_player_update(screen, background, sprites, pos_joueur, nb_joueur, &clip[6], &clip[2], joueur);
                graph_player_update(screen, background, sprites, pos_joueur, nb_joueur, &clip[7], &clip[2], joueur);
                break;

            case HAUT:
                pos_joueur[joueur].y--;
                graph_player_update(screen, background, sprites, pos_joueur, nb_joueur, &clip[1], &clip[2], joueur);
                graph_player_update(screen, background, sprites, pos_joueur, nb_joueur, &clip[0], &clip[2], joueur);
                break;

            case BAS:
                pos_joueur[joueur].y++;
                graph_player_update(screen, background, sprites, pos_joueur, nb_joueur, &clip[4], &clip[2], joueur);
                graph_player_update(screen, background, sprites, pos_joueur, nb_joueur, &clip[5], &clip[2], joueur);
                break;
        }
    }while(pos_joueur[joueur].x != (x*coeff[0] + BORDER_WIDTH) || pos_joueur[joueur].y != (y*coeff[1] + BORDER_HEIGHT) );

    graph_display_update(screen, background, sprites, pos_joueur, nb_joueur, &clip[2]);

}

