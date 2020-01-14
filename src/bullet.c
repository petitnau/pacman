#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <malloc.h>

#include "entity.h"
#include "utils.h"
#include "interface.h"
#include "bullet.h"

typedef struct
{
    int bullet_pos;
    Bullet* bullet;
    BulletList* list;
} BulletThreadPar;

pthread_mutex_t bullet_mutex = PTHREAD_MUTEX_INITIALIZER;

void manage_b_info_in(int, int, BulletList*);

/**
 * Funzione principale processo proiettile
 * 
 * @param bullet_info pipe che gestisce le info in arrivo da control
 * @param bullet_pos pipe in uscita che invia posizioni a control
 */
int bullet_main(int bullet_info, int bullet_pos)
{
    BulletList list;
    list.head = NULL;
    list.tail = NULL;
    list.count = 0;
    
    while(1)
    {
        manage_b_info_in(bullet_info, bullet_pos, &list);
    }
}

/**
 * Thread di un singolo sparo dove gestisco il movimento e se collide con un muro
 * 
 * @param parameters parametri passati al thread
 */
void* bullet_thread(void* parameters)
{
    int movepause;
    _Bool stop = false;

    BulletThreadPar* b_par = (BulletThreadPar*) parameters;

    beep();
    while(!stop)
    {
        pthread_mutex_lock(&bullet_mutex);
        switch(b_par->bullet->dir)
        {
            case UP:
                b_par->bullet->p.y--;
                break;
            case DOWN:
                b_par->bullet->p.y++;
                break;
            case RIGHT:
                b_par->bullet->p.x++;
                break;
            case LEFT:
                b_par->bullet->p.x--;
                break;
        }
        map_loop(&b_par->bullet->p);
        //Controllo se vi è un muro o meno
        if(!is_empty_space(MAP_PACMAN[b_par->bullet->p.y][b_par->bullet->p.x]))
            b_par->bullet->dead = true;
            
        write(b_par->bullet_pos, b_par->bullet, sizeof(*(b_par->bullet)));
        
        movepause = 3e4;
        if(b_par->bullet->dir == UP || b_par->bullet->dir == DOWN) //gestisce la velocità di pacman
            movepause*=2;  
        
        stop = b_par->bullet->dead;

        pthread_mutex_unlock(&bullet_mutex);

        usleep(movepause);  
    }
    //Comunica la morte del proiettile a control e lo rimuove dalla lista
    pthread_mutex_lock(&bullet_mutex);
    write(b_par->bullet_pos, b_par->bullet, sizeof(*(b_par->bullet)));
    b_list_remove(b_par->list, b_list_search(*(b_par->list), *(b_par->bullet)));
    pthread_mutex_unlock(&bullet_mutex);
}

void manage_b_info_in(int bullet_info, int bullet_pos, BulletList* list)
{
    BulletInfo info;
    Bullet bullet;
    BulletNode* aux;

    while(read(bullet_info, &info, sizeof(info)) != -1)
    {
        if(info.create_bullet) //Crea un nuovo proiettile aggiungendo alla lista e creando un thread
        {
            BulletThreadPar* bullet_par = malloc(sizeof(BulletThreadPar));
            bullet.p = info.p;
            bullet.dir = info.dir;
            bullet.enemy = info.enemy;
            bullet.dead = false;
            bullet_par->list = list;
            bullet_par->bullet_pos = bullet_pos;

            pthread_mutex_lock(&bullet_mutex);
            bullet_par->bullet = &b_list_push(list, bullet)->bullet;
            pthread_mutex_unlock(&bullet_mutex);

            pthread_create(&bullet_par->bullet->id, NULL, &bullet_thread, bullet_par);
        }
        if(info.destroy_bullet) //Se bisogna distruggere il proiettile viene rimosso dalla lista
        {
            pthread_mutex_lock(&bullet_mutex);
            bullet.id = info.destroy_id;
            aux = b_list_search(*list, bullet);
            if(aux != NULL)
                aux->bullet.dead = true;
            pthread_mutex_unlock(&bullet_mutex);
        }
    }
}
