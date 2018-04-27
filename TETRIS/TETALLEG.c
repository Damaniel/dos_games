#include <stdlib.h>
#include "allegro.h"
#include "sb_lib.h"
#include <stdio.h>
#include <time.h>
#include <string.h>

#define HOR_SEGMENT   116
#define VER_SEGMENT    37

/////////////////////////////////////
// Thou must fixeth the bugs....   //
/////////////////////////////////////

//variables
int Game_Speed, Left_Flag, Right_Flag,red_flag,Next_Piece_Flag=1;
int Game_Over_Flag =0, color_rotate_flag=0,b_game_flag;
long score=0;
int score_left_pos;
int lines;
int Counter_Flag=0;
int height=0;
int rotation=0;           //clockwise
char name[12];
// statistics...

int DMA=1,setup_flag=0,frequency=0, SFX_vol=0, mod_vol=0, sound_enable=0,parse_flag=0;
int singles=0,doubles=0,triples=0,tetrises=0;
int num_pieces[8];
int mod_to_play=0;

char Game_Grid[16][10];

typedef struct Block {

    int shape;
    int Rotation;
    char X_Pos[4];
    char Y_Pos[4];
    char color;

} Brick;

Brick Falling_Block, Next_Block;

//Must put a bitmap type for the different blocks here.......

BITMAP *blocks[9];

sb_sample *line_sound, *neartop, *line_sound_conv, *neartop_conv;
sb_mod_file *gamemod, *tune1, *tune2, *tune3;

//OK....that's done, I believe....

//prototypes.....
void High_Score_Stuff(void);
void Update_High_Score(int position);
void Init_Sequence(void);
void Initialize_Game(void);
void Load_Background(int Speed);
void Write_Text(void);
void Initialize_Brick(void);
void Draw_Tetris_Block(int X, int Y, int color);
void Draw_Tetris_Full_Brick(void);
void Erase_Tetris_Block(int X, int Y);
void Erase_Tetris_Full_Brick(void);
void Game_Loop(void);
void Ground_Check(void);
void Keyboard_Poller(void);
void Do_Line_Cleanup(int line);
void Die(void);
void Deal_With_Score(int amount);
void Rotate(void);
void Drop_The_Piece(int flag);
void Deal_With_Lines(void);
void Do_Stats(void);
void Check_Starting_Rotation(int rotation);
void Initialize_New_Brick(void);
void Draw_Next_Piece(void);
void Do_Options_Screen(void);
void Draw_Game_Speed_Box(int oldSpeed, int newSpeed);
void Draw_Height_Box(int oldHeight, int newHeight);
void Draw_Rotation_Box(void);
void Initialize_Height(void);
void Draw_Color_Rotating_Box(int tx, int ty, int bx, int by);
void Draw_Black_Box(int tx, int ty, int bx, int by);
void Draw_Black_Line(int position);
void Draw_Color_Rotating_Line(int position);
void Choose_Music(int oldmod, int newmod);
void Parse_Sound_Cfg_File(void);
void Do_Setup(void);
void Set_Mod_Volume(int oldvol, int newvol);
void Draw_Game_B_Box(void);
void Do_Game_B_Rewards(void);
void Game_Over(void);


//Here beginnith the functions.....

/////////////////////////////////////////////////////////////////////////////

void Init_Sequence(void) {


    int count;
    char buffer[8];

    if(sound_enable) {
    if(mod_to_play==0)
            gamemod=sb_load_mod_file("tune1.mod");
    if(mod_to_play==1)
            gamemod=sb_load_mod_file("tune2.mod");
    if(mod_to_play==2)
            gamemod=sb_load_mod_file("tune3.mod");
    }

    for(count=0;count<8;count++)
        num_pieces[count]=0;

    //Initial score
    score = 0;

    if(b_game_flag==0)
    lines = 0;
    else
    lines = 25;


    Load_Background(Game_Speed);
    Write_Text();
    Deal_With_Score(0);
    Deal_With_Lines();
    sprintf(buffer,"%d",Game_Speed);
    textout(screen,font,buffer,57,147,15);

    Initialize_Height();

    //setup the next brick thingy....
        Initialize_Brick();

        Next_Block.shape=Falling_Block.shape;
        Next_Block.Rotation=Falling_Block.Rotation;
        Next_Block.color=Falling_Block.color;

        for(count=0;count<4;count++) {
            Next_Block.Y_Pos[count]=Falling_Block.Y_Pos[count];
            Next_Block.X_Pos[count]=Falling_Block.X_Pos[count];
        }

        Initialize_Brick();
        Check_Starting_Rotation(Falling_Block.Rotation);
        num_pieces[Falling_Block.shape]++;

    for(count=0;count<4;count++) {
        if(Falling_Block.Y_Pos[count]>=0)
            Game_Grid[Falling_Block.Y_Pos[count]][Falling_Block.X_Pos[count]]=-1;
    }

    Draw_Next_Piece();

    //Play the game

    if(sound_enable)
           sb_mod_play(gamemod);
    Game_Loop();

}

/////////////////////////////////////////////////////////////////////////////

void Load_Background(int Speed) {

    int count, count2, temp;

    BITMAP *theBackground;
    PALETTE bg_palette;

    //Will be switched to a switch statement, as more pictures are designed
    switch(Speed) {
            case 0:
                theBackground=load_pcx("concept.pcx", bg_palette);
                break;
            case 1:
                theBackground=load_pcx("concept2.pcx", bg_palette);
                break;
            case 2:
                theBackground=load_pcx("concept3.pcx", bg_palette);
                break;
            case 3:
                theBackground=load_pcx("concept4.pcx", bg_palette);
                break;
            case 4:
                theBackground=load_pcx("concept5.pcx", bg_palette);
                break;
            case 5:
                theBackground=load_pcx("concept6.pcx", bg_palette);
                break;
            case 6:
                theBackground=load_pcx("concept7.pcx", bg_palette);
                break;
            default:
                theBackground=load_pcx("concept6.pcx", bg_palette);
    }

    set_palette(bg_palette);
    blit(theBackground,screen,0,0,0,0,320,200);

    destroy_bitmap(theBackground);

    for(count=0;count<16;count++) {
        for(count2=0;count2<10;count2++) {

            temp=Game_Grid[count][count2];
            if(temp<=0) {
                Erase_Tetris_Block(count2,count);
                Game_Grid[count][count2]=temp;
            }
            else {
                Draw_Tetris_Block(count2,count,temp);
                Game_Grid[count][count2]=temp;
            }
        }
    }

}

/////////////////////////////////////////////////////////////////////////////

void Game_Loop(void) {

    int counter;
    RGB theColor,color1;
    int count,color_index;

    while(Game_Over_Flag==0) {

    if(sound_enable) {
       if(!sb_mod_active)
         sb_mod_play(gamemod);
    }

    //Step 1:  Draw the Tetris Block....
    Draw_Tetris_Full_Brick();

    //Step 2:  Set the rest counter
    counter=13-Game_Speed;

    //Step 3:  Decrement the counter, poll keyboard every so often.....
    while(counter!=0) {

///////
get_color(21,&color1);

        for(count=22;count<=35;count++) {

            get_color(count,&theColor);
            set_color(count-1,&theColor);
        }

        set_color(35,&color1);
///////

        Keyboard_Poller();
        rest(50);
        counter--;
        if(Counter_Flag==1) {
            counter=0;
            Counter_Flag=0;
        }

    }

    //Step 4: Perform ground check
        Ground_Check();


    }
}

/////////////////////////////////////////////////////////////////////////////

void Keyboard_Poller(void) {

    int theChar;
    int count,count2;

    if(!keypressed())
         return;
    else
         theChar=readkey();

    theChar= (theChar >> 8);
    if(theChar==KEY_LEFT) {
        Left_Flag=1;
        for(count=0;count<4;count++) {
            if((Falling_Block.X_Pos[count]==0) || (Game_Grid[Falling_Block.Y_Pos[count]][Falling_Block.X_Pos[count]-1]>0))
                Left_Flag=0;
        }
    }

    if(theChar==KEY_RIGHT) {
        Right_Flag=1;
        for(count=0;count<4;count++) {
            if((Falling_Block.X_Pos[count]==9) || (Game_Grid[Falling_Block.Y_Pos[count]][Falling_Block.X_Pos[count]+1]>0))
                Right_Flag=0;
        }
    }

    if(theChar==KEY_UP) { //Just let the piece fall a little
       Drop_The_Piece(1);
       Counter_Flag=1;
    }

    if(theChar==KEY_SPACE) {   //Let the piece go all the way
        Drop_The_Piece(0);
        Counter_Flag=1;
    }

    if(theChar==KEY_ESC) {
        Game_Over_Flag=1;
        Counter_Flag=1;
    }

    if(theChar==KEY_DOWN) {
        Rotate();
    }

    if(theChar==KEY_N) {
            if(Next_Piece_Flag==1)
                Next_Piece_Flag=0;
            else
                Next_Piece_Flag=1;
            Draw_Next_Piece();
    }

    if((Left_Flag==1) || (Right_Flag==1)) {

    for(count=0;count<4;count++) {
        if(Falling_Block.Y_Pos[count]>=0)
        Game_Grid[Falling_Block.Y_Pos[count]][Falling_Block.X_Pos[count]]=0;
    }

    Erase_Tetris_Full_Brick();

    if(Right_Flag==1) {
        for(count=0;count<4;count++)
           Falling_Block.X_Pos[count]++;
    }
    if(Left_Flag==1) {
        for(count=0;count<4;count++)
            Falling_Block.X_Pos[count]--;

    }

    for(count=0;count<4;count++)
         Game_Grid[Falling_Block.Y_Pos[count]][Falling_Block.X_Pos[count]]=-1;

    Draw_Tetris_Full_Brick();
    }

    Right_Flag=0;
    Left_Flag=0;
    clear_keybuf();
    theChar=0;
}

/////////////////////////////////////////////////////////////////////////////

void Ground_Check(void) {

    int count,count2,count3;
    int counter,lines_counter;

    for(count=0;count<4;count++) {      //start check of piece...
        if((Falling_Block.Y_Pos[count]==15) || (Game_Grid[(Falling_Block.Y_Pos[count])+1][Falling_Block.X_Pos[count]] > 0)) {



            //Do needed updates
            for(count2=0;count2<4;count2++)
                Game_Grid[Falling_Block.Y_Pos[count2]][Falling_Block.X_Pos[count2]] = Falling_Block.color;


            //Perform the line check....

            lines_counter=0;

            for(count2=0;count2<16;count2++) {
                counter=0;
                for(count3=0;count3<10;count3++) {
                    if(Game_Grid[count2][count3]>0)
                        counter++;

                }

                if(counter==10) {
                    Do_Line_Cleanup(count2);
                    lines_counter++;
                }
            }

            if(lines_counter==1) {
                singles++;
                Deal_With_Score((20*(Game_Speed+1)));
            }
            if(lines_counter==2) {
                doubles++;
                Deal_With_Score((50*(Game_Speed+1)));
            }
            if(lines_counter==3) {
                triples++;
                Deal_With_Score((150*(Game_Speed+1)));
            }
            if(lines_counter==4) {
                tetrises++;
                Deal_With_Score((400*(Game_Speed+1)));
            }

            //Do the death check

            for(count3=0;count3<4;count3++) {
                if(Falling_Block.Y_Pos[count3]==0) {
                    Die();
                }
                if((Falling_Block.Y_Pos[count3]<5) && (red_flag==0)) {
                if(sound_enable)
                      sb_mix_sample(neartop);
                red_flag=1;
                }
            }


            //Start a new piece

            Deal_With_Score(((rand()%6)+1)*(Game_Speed+1));

            Initialize_New_Brick();
            num_pieces[Falling_Block.shape]++;
            Draw_Next_Piece();
            //get outta here!
            return;

        }
    }

    Erase_Tetris_Full_Brick();

    for(count=0;count<4;count++) {
        Falling_Block.Y_Pos[count]++;
    }

    Left_Flag=0;
    Right_Flag=0;
}

/////////////////////////////////////////////////////////////////////////////

void Initialize_New_Brick(void) {

        Brick Temp_Block;
        int count;

        //Move the next brick content's into the temporary brick....
        Temp_Block.color=Next_Block.color;
        Temp_Block.shape=Next_Block.shape;
        Temp_Block.Rotation=Next_Block.Rotation;
        for(count=0;count<4;count++) {
            Temp_Block.X_Pos[count]=Next_Block.X_Pos[count];
            Temp_Block.Y_Pos[count]=Next_Block.Y_Pos[count];
        }

        //OK....now move the new data into the Falling_Block position
        Initialize_Brick();
        Check_Starting_Rotation(Falling_Block.Rotation);

        //Now...move this data into the Next_Block

        Next_Block.color=Falling_Block.color;
        Next_Block.shape=Falling_Block.shape;
        Next_Block.Rotation=Falling_Block.Rotation;
        for(count=0;count<4;count++) {
            Next_Block.X_Pos[count]=Falling_Block.X_Pos[count];
            Next_Block.Y_Pos[count]=Falling_Block.Y_Pos[count];
        }

        //Finally, move the temp data into the Falling_Block position

        Falling_Block.color=Temp_Block.color;
        Falling_Block.shape=Temp_Block.shape;
        Falling_Block.Rotation=Temp_Block.Rotation;
        for(count=0;count<4;count++) {
            Falling_Block.X_Pos[count]=Temp_Block.X_Pos[count];
            Falling_Block.Y_Pos[count]=Temp_Block.Y_Pos[count];
        }

        for(count=0;count<4;count++) {
        if(Falling_Block.Y_Pos[count]>0)
            Game_Grid[Falling_Block.Y_Pos[count]][Falling_Block.X_Pos[count]]=-1;
    }

}

/////////////////////////////////////////////////////////////////////////////

void Rotate(void) {

    int count,temp,count4;
    int No_Rotate_Flag = 0;
    int Temp_X[4];
    int Temp_Y[4];

    //OK....first thing's first -- we must assign the old X and Y values to
    //some temp variables.....and get rid of the Game_Grid associations....

    for(count=0;count<4;count++) {
        Temp_X[count]=Falling_Block.X_Pos[count];
        Temp_Y[count]=Falling_Block.Y_Pos[count];

        if(Temp_Y>=0)
            Game_Grid[Temp_Y[count]][Temp_X[count]]=0;

    }

    //Now....perform the rotation, based on the current position of the piece,
    //and the type of piece it is....

   if(rotation==0) {
    Falling_Block.Rotation++;
    if(Falling_Block.Rotation>3)
        Falling_Block.Rotation=0;
   }
   else {
    Falling_Block.Rotation--;
    if(Falling_Block.Rotation<0)
        Falling_Block.Rotation=3;
   }

switch(rotation) {

  case 0:
    switch(Falling_Block.shape) {
                case 0:
                    switch(Falling_Block.Rotation) {
                        case 1:
                        case 3:
                                Falling_Block.X_Pos[0]++;
                                Falling_Block.X_Pos[2]--;
                                Falling_Block.X_Pos[3]-=2;
                                Falling_Block.Y_Pos[0]--;
                                Falling_Block.Y_Pos[2]++;
                                Falling_Block.Y_Pos[3]+=2;
                                break;
                        case 0:
                        case 2:
                                Falling_Block.X_Pos[0]--;
                                Falling_Block.X_Pos[2]++;
                                Falling_Block.X_Pos[3]+=2;
                                Falling_Block.Y_Pos[0]++;
                                Falling_Block.Y_Pos[2]--;
                                Falling_Block.Y_Pos[3]-=2;
                                break;
                    }
                        break;
                case 1:
                        break;          //Nothing needed....it's the square!
                case 2:
                   switch(Falling_Block.Rotation) {
                        case 1:
                                Falling_Block.X_Pos[0]++;
                                Falling_Block.X_Pos[2]--;
                                Falling_Block.Y_Pos[0]--;
                                Falling_Block.Y_Pos[2]++;
                                Falling_Block.Y_Pos[3]+=2;
                                break;
                        case 2:
                                Falling_Block.X_Pos[0]++;
                                Falling_Block.X_Pos[2]--;
                                Falling_Block.X_Pos[3]-=2;
                                Falling_Block.Y_Pos[0]++;
                                Falling_Block.Y_Pos[2]--;
                                break;
                        case 3:
                                Falling_Block.X_Pos[0]--;
                                Falling_Block.X_Pos[2]++;
                                Falling_Block.Y_Pos[0]++;
                                Falling_Block.Y_Pos[2]--;
                                Falling_Block.Y_Pos[3]-=2;
                                break;
                        case 0:
                                Falling_Block.X_Pos[0]--;
                                Falling_Block.X_Pos[2]++;
                                Falling_Block.X_Pos[3]+=2;
                                Falling_Block.Y_Pos[0]--;
                                Falling_Block.Y_Pos[2]++;
                                break;
                    }
                 break;
                 case 3:
                   switch(Falling_Block.Rotation) {
                        case 1:
                                Falling_Block.X_Pos[0]+=2;
                                Falling_Block.X_Pos[1]++;
                                Falling_Block.X_Pos[3]--;
                                Falling_Block.Y_Pos[1]--;
                                Falling_Block.Y_Pos[3]++;
                                break;
                        case 2:
                                Falling_Block.X_Pos[1]++;
                                Falling_Block.X_Pos[3]--;
                                Falling_Block.Y_Pos[0]+=2;
                                Falling_Block.Y_Pos[1]++;
                                Falling_Block.Y_Pos[3]--;
                                break;
                        case 3:
                                Falling_Block.X_Pos[0]-=2;
                                Falling_Block.X_Pos[1]--;
                                Falling_Block.X_Pos[3]++;
                                Falling_Block.Y_Pos[1]++;
                                Falling_Block.Y_Pos[3]--;
                                break;
                        case 0:
                                Falling_Block.X_Pos[1]--;
                                Falling_Block.X_Pos[3]++;
                                Falling_Block.Y_Pos[0]-=2;
                                Falling_Block.Y_Pos[1]--;
                                Falling_Block.Y_Pos[3]++;
                                break;
                    }
              break;
              case 4:
                   switch(Falling_Block.Rotation) {
                        case 1:
                                Falling_Block.X_Pos[0]++;
                                Falling_Block.X_Pos[2]++;
                                Falling_Block.X_Pos[3]--;
                                Falling_Block.Y_Pos[0]--;
                                Falling_Block.Y_Pos[2]++;
                                Falling_Block.Y_Pos[3]++;
                                break;
                        case 2:
                                Falling_Block.X_Pos[0]++;
                                Falling_Block.X_Pos[2]--;
                                Falling_Block.X_Pos[3]--;
                                Falling_Block.Y_Pos[0]++;
                                Falling_Block.Y_Pos[2]++;
                                Falling_Block.Y_Pos[3]--;
                                break;
                        case 3:
                                Falling_Block.X_Pos[0]--;
                                Falling_Block.X_Pos[2]--;
                                Falling_Block.X_Pos[3]++;
                                Falling_Block.Y_Pos[0]++;
                                Falling_Block.Y_Pos[2]--;
                                Falling_Block.Y_Pos[3]--;
                                break;
                        case 0:
                                Falling_Block.X_Pos[0]--;
                                Falling_Block.X_Pos[2]++;
                                Falling_Block.X_Pos[3]++;
                                Falling_Block.Y_Pos[0]--;
                                Falling_Block.Y_Pos[2]--;
                                Falling_Block.Y_Pos[3]++;
                                break;
                    }
                break;
             case 5:
                   switch(Falling_Block.Rotation) {
                        case 1:
                        case 3:
                                Falling_Block.X_Pos[0]++;
                                Falling_Block.X_Pos[2]++;
                                Falling_Block.Y_Pos[0]--;
                                Falling_Block.Y_Pos[2]++;
                                Falling_Block.Y_Pos[3]+=2;
                                break;
                        case 2:
                        case 0:
                                Falling_Block.X_Pos[0]--;
                                Falling_Block.X_Pos[2]--;
                                Falling_Block.Y_Pos[0]++;
                                Falling_Block.Y_Pos[2]--;
                                Falling_Block.Y_Pos[3]-=2;
                                break;
                    }
                break;
            case 6:
                   switch(Falling_Block.Rotation) {
                        case 1:
                        case 3:
                                Falling_Block.X_Pos[0]++;
                                Falling_Block.X_Pos[2]--;
                                Falling_Block.X_Pos[3]-=2;
                                Falling_Block.Y_Pos[0]--;
                                Falling_Block.Y_Pos[2]--;
                                break;
                        case 2:
                        case 0:
                                Falling_Block.X_Pos[0]--;
                                Falling_Block.X_Pos[2]++;
                                Falling_Block.X_Pos[3]+=2;
                                Falling_Block.Y_Pos[0]++;
                                Falling_Block.Y_Pos[2]++;
                                break;
                    }
                break;
            default:
                        break;
        }
        break;

 case 1:
            switch(Falling_Block.shape) {
                case 0:
                    switch(Falling_Block.Rotation) {
                        case 1:
                        case 3:
                                Falling_Block.X_Pos[0]++;
                                Falling_Block.X_Pos[2]--;
                                Falling_Block.X_Pos[3]-=2;
                                Falling_Block.Y_Pos[0]--;
                                Falling_Block.Y_Pos[2]++;
                                Falling_Block.Y_Pos[3]+=2;
                                break;
                        case 0:
                        case 2:
                                Falling_Block.X_Pos[0]--;
                                Falling_Block.X_Pos[2]++;
                                Falling_Block.X_Pos[3]+=2;
                                Falling_Block.Y_Pos[0]++;
                                Falling_Block.Y_Pos[2]--;
                                Falling_Block.Y_Pos[3]-=2;
                                break;
                    }
                        break;
                case 1:
                        break;          //Nothing needed....it's the square!
                case 2:
                   switch(Falling_Block.Rotation) {
                        //Figure out what the hell I'm doing here....
                        case 3:
                               Falling_Block.X_Pos[0]++;
                               Falling_Block.X_Pos[2]--;
                               Falling_Block.X_Pos[3]-=2;
                               Falling_Block.Y_Pos[0]++;
                               Falling_Block.Y_Pos[2]--;
                               break;
                        case 2:
                               Falling_Block.X_Pos[0]++;
                               Falling_Block.X_Pos[2]--;
                               Falling_Block.Y_Pos[0]--;
                               Falling_Block.Y_Pos[2]++;
                               Falling_Block.Y_Pos[3]+=2;
                               break;
                        case 1:
                               Falling_Block.X_Pos[0]--;
                               Falling_Block.X_Pos[2]++;
                               Falling_Block.X_Pos[3]+=2;
                               Falling_Block.Y_Pos[0]--;
                               Falling_Block.Y_Pos[2]++;
                               break;
                        case 0:
                               Falling_Block.X_Pos[0]--;
                               Falling_Block.X_Pos[2]++;
                               Falling_Block.Y_Pos[0]++;
                               Falling_Block.Y_Pos[2]--;
                               Falling_Block.Y_Pos[3]-=2;
                               break;

                    }
              break;
                 case 3:
                   switch(Falling_Block.Rotation) {
                         case 3:
                               Falling_Block.X_Pos[1]++;
                               Falling_Block.X_Pos[3]--;
                               Falling_Block.Y_Pos[0]+=2;
                               Falling_Block.Y_Pos[1]++;
                               Falling_Block.Y_Pos[3]--;
                               break;
                        case 2:
                               Falling_Block.X_Pos[0]+=2;
                               Falling_Block.X_Pos[1]++;
                               Falling_Block.X_Pos[3]--;
                               Falling_Block.Y_Pos[1]--;
                               Falling_Block.Y_Pos[3]++;
                               break;
                        case 1:
                               Falling_Block.X_Pos[1]--;
                               Falling_Block.X_Pos[3]++;
                               Falling_Block.Y_Pos[0]-=2;
                               Falling_Block.Y_Pos[1]--;
                               Falling_Block.Y_Pos[3]++;
                               break;
                        case 0:
                               Falling_Block.X_Pos[0]-=2;
                               Falling_Block.X_Pos[1]--;
                               Falling_Block.X_Pos[3]++;
                               Falling_Block.Y_Pos[1]++;
                               Falling_Block.Y_Pos[3]--;
                               break;
                    }
                 break;
             case 4:
                   switch(Falling_Block.Rotation) {
                        case 3:
                                Falling_Block.X_Pos[0]++;
                                Falling_Block.X_Pos[2]--;
                                Falling_Block.X_Pos[3]--;
                                Falling_Block.Y_Pos[0]++;
                                Falling_Block.Y_Pos[2]++;
                                Falling_Block.Y_Pos[3]--;
                                break;
                        case 2:
                                Falling_Block.X_Pos[0]++;
                                Falling_Block.X_Pos[2]++;
                                Falling_Block.X_Pos[3]--;
                                Falling_Block.Y_Pos[0]--;
                                Falling_Block.Y_Pos[2]++;
                                Falling_Block.Y_Pos[3]++;
                                break;
                        case 1:
                                Falling_Block.X_Pos[0]--;
                                Falling_Block.X_Pos[2]++;
                                Falling_Block.X_Pos[3]++;
                                Falling_Block.Y_Pos[0]--;
                                Falling_Block.Y_Pos[2]--;
                                Falling_Block.Y_Pos[3]++;
                                break;
                        case 0:
                                Falling_Block.X_Pos[0]--;
                                Falling_Block.X_Pos[2]--;
                                Falling_Block.X_Pos[3]++;
                                Falling_Block.Y_Pos[0]++;
                                Falling_Block.Y_Pos[2]--;
                                Falling_Block.Y_Pos[3]--;
                                break;
                    }
                break;
             case 5:
                   switch(Falling_Block.Rotation) {
                        case 1:
                        case 3:
                                Falling_Block.X_Pos[0]++;
                                Falling_Block.X_Pos[2]++;
                                Falling_Block.Y_Pos[0]--;
                                Falling_Block.Y_Pos[2]++;
                                Falling_Block.Y_Pos[3]+=2;
                                break;
                        case 2:
                        case 0:
                                Falling_Block.X_Pos[0]--;
                                Falling_Block.X_Pos[2]--;
                                Falling_Block.Y_Pos[0]++;
                                Falling_Block.Y_Pos[2]--;
                                Falling_Block.Y_Pos[3]-=2;
                                break;
                    }
                break;
            case 6:
                   switch(Falling_Block.Rotation) {
                        case 1:
                        case 3:
                                Falling_Block.X_Pos[0]++;
                                Falling_Block.X_Pos[2]--;
                                Falling_Block.X_Pos[3]-=2;
                                Falling_Block.Y_Pos[0]--;
                                Falling_Block.Y_Pos[2]--;
                                break;
                        case 2:
                        case 0:
                                Falling_Block.X_Pos[0]--;
                                Falling_Block.X_Pos[2]++;
                                Falling_Block.X_Pos[3]+=2;
                                Falling_Block.Y_Pos[0]++;
                                Falling_Block.Y_Pos[2]++;
                                break;
                    }
                break;
            default:
                        break;
        }
        break;
}



        //Now, perform da collision detection....

        for(count=0;count<4;count++) {
            if(Falling_Block.X_Pos[count]>9)
                No_Rotate_Flag=1;
            if(Falling_Block.X_Pos[count]<0)
                No_Rotate_Flag=1;
            if(Game_Grid[Falling_Block.Y_Pos[count]][Falling_Block.X_Pos[count]]>0)
                No_Rotate_Flag=1;
            //if(Falling_Block.Y_Pos[count]<0)
            //    No_Rotate_Flag=1;
            if(Falling_Block.Y_Pos[count]>15)
                No_Rotate_Flag=1;
        }

        if(No_Rotate_Flag==0) {

        for(count=0;count<4;count++) {
            if(Falling_Block.Y_Pos[count]>0)
                Game_Grid[Falling_Block.Y_Pos[count]][Falling_Block.X_Pos[count]]=-1;
            Erase_Tetris_Block(Temp_X[count],Temp_Y[count]);

        }

            Draw_Tetris_Full_Brick();
        }

        if(No_Rotate_Flag==1) {
            for(count=0;count<4;count++) {
                Falling_Block.X_Pos[count]=Temp_X[count];
                Falling_Block.Y_Pos[count]=Temp_Y[count];
                if(Temp_Y[count]>=0)
                    Game_Grid[Temp_Y[count]][Temp_X[count]]=-1;


            }

            if(rotation==0) {
              Falling_Block.Rotation--;
                if(Falling_Block.Rotation<0)
                    Falling_Block.Rotation=3;
            }
            if(rotation==1) {
              Falling_Block.Rotation++;
                if(Falling_Block.Rotation>3)
                    Falling_Block.Rotation=0;
            }

        }

        No_Rotate_Flag=0;

}

/////////////////////////////////////////////////////////////////////////////

void Do_Stats(void) {

    BITMAP *stats_pic;
    PALETTE stats_palette;
    int total=0,count;
    char buffer[16];
    int theChar=0;

    stats_pic=load_pcx("stats.pcx",stats_palette);
    set_palette(stats_palette);
    blit(stats_pic,screen,0,0,0,0,320,200);

    sprintf(buffer,"%d",num_pieces[0]);
    textout(screen,font,buffer,31,64,15);
    sprintf(buffer,"%d",num_pieces[1]);
    textout(screen,font,buffer,69,64,15);
    sprintf(buffer,"%d",num_pieces[2]);
    textout(screen,font,buffer,108,64,15);
    sprintf(buffer,"%d",num_pieces[3]);
    textout(screen,font,buffer,150,64,15);
    sprintf(buffer,"%d",num_pieces[4]);
    textout(screen,font,buffer,197,64,15);
    sprintf(buffer,"%d",num_pieces[5]);
    textout(screen,font,buffer,237,64,15);
    sprintf(buffer,"%d",num_pieces[6]);
    textout(screen,font,buffer,282,64,15);

    for(count=0;count<7;count++)
        total+=num_pieces[count];

    sprintf(buffer,"%d",total);
    textout(screen,font,buffer,83,30,15);

    sprintf(buffer,"%d",lines);
    textout(screen,font,buffer,89,89,15);
    sprintf(buffer,"%d",singles);
    textout(screen,font,buffer,116,112,15);
    sprintf(buffer,"%d",doubles);
    textout(screen,font,buffer,268,111,15);
    sprintf(buffer,"%d",triples);
    textout(screen,font,buffer,116,154,15);
    sprintf(buffer,"%d",tetrises);
    textout(screen,font,buffer,268,152,15);

    clear_keybuf();
    theChar=0;
    while((theChar >> 8) !=KEY_ENTER) {

         if(keypressed())
            theChar=readkey();
    }

    fade_out(2);

}

/////////////////////////////////////////////////////////////////////////////

void Do_Line_Cleanup(int line) {

       int count,count3,count4,temp,test_flag=0;
       char buffer[16];
       RGB theColor,color1;
        //Clear the line in question


       if(sound_enable)
            sb_mix_sample(line_sound);

        for(count4=0;count4<4;count4++) {

            for(count=0;count<10;count++) {
                temp=Game_Grid[line][count];
                Erase_Tetris_Block(count,line);
                Game_Grid[line][count]=temp;
            }

            for(count3=1;count3<=2;count3++) {
                ///////
                    get_color(21,&color1);

                        for(count=22;count<=35;count++) {

                        get_color(count,&theColor);
                        set_color(count-1,&theColor);
                        }

                        set_color(35,&color1);
                ///////
            rest(50);

            }

            for(count=0;count<10;count++) {
                temp=Game_Grid[line][count];
                Draw_Tetris_Block(count, line, temp);
                Game_Grid[line][count]=temp;

            }

         for(count3=1;count3<=2;count3++) {
                ///////
                    get_color(21,&color1);

                        for(count=22;count<=35;count++) {

                        get_color(count,&theColor);
                        set_color(count-1,&theColor);
                   }

                        set_color(35,&color1);
                ///////
            rest(50);

            }
        }
        for(count=0;count<10;count++) {
            Erase_Tetris_Block(count,line);
            Game_Grid[line][count]=0;
        }

        //Now, move everything down as much as necessary

        for(count=line-1;count>=0;count--) {

            for(count4=0;count4<10;count4++) {

                    if(Game_Grid[count][count4]>0) {
                    Erase_Tetris_Block(count4,count+1);
                    Draw_Tetris_Block(count4, count+1, Game_Grid[count][count4]);
                    }
                    if(Game_Grid[count][count4]==0)
                    Erase_Tetris_Block(count4, count+1);

                    Game_Grid[count+1][count4]=Game_Grid[count][count4];



            }

        }


       for(count=0;count<10;count++)
                    Game_Grid[0][count]=0;


       if(b_game_flag==0)
       lines++;
       else
       lines--;

       Deal_With_Lines();

       //Better redraw the whole screen to make sure that everything's peachy..

       for(count=0;count<16;count++) {
            for(count4=0;count4<10;count4++) {

                temp=Game_Grid[count][count4];

                if(Game_Grid[count][count4]>0)
                    Draw_Tetris_Block(count4,count,Game_Grid[count][count4]);
                else
                    Erase_Tetris_Block(count4,count);

                 Game_Grid[count][count4]=temp;

            }

       }

       for(count=0;count<6;count++) {
           for(count3=0;count3<10;count3++) {
              if(Game_Grid[count][count3]!=0)
                 test_flag=1;
           }
       }

       if(test_flag==0)
            red_flag=0;


       if((lines % 10==0) && (lines/10 > Game_Speed) && (!b_game_flag)) {

            if(Game_Speed<9) {
                Game_Speed++;
                Load_Background(Game_Speed);
                Write_Text();
                sprintf(buffer,"%d",Game_Speed);
                text_mode(0);
                textout(screen,font,buffer,57,147,15);
                text_mode(-1);
            }

            Deal_With_Score(0);
            Deal_With_Lines();
            Draw_Next_Piece();
       }

       if((b_game_flag) && (lines==0))
            Do_Game_B_Rewards();

}

/////////////////////////////////////////////////////////////////////////////

void Drop_The_Piece(int flag) {

    int count,count2;
    int Finish_Flag=0;

    for(count=0;count<4;count++)  {
        Erase_Tetris_Block(Falling_Block.X_Pos[count],Falling_Block.Y_Pos[count]);
    if(Falling_Block.Y_Pos[count]>=0)
        Game_Grid[Falling_Block.Y_Pos[count]][Falling_Block.X_Pos[count]]=0;
    }

    while(Finish_Flag==0) {

        for(count=0;count<4;count++) {
            if((Game_Grid[Falling_Block.Y_Pos[count]+1][Falling_Block.X_Pos[count]]>0) || (Falling_Block.Y_Pos[count]==15))
                Finish_Flag=1;
          }

            if(Finish_Flag==0) {
               for(count=0;count<4;count++) {
                    Falling_Block.Y_Pos[count]++;
                }
            }

       if(flag==1)
            Finish_Flag=1;
    }

    //for(count=0;count<4;count++) {
    //    if((Game_Grid[Falling_Block.Y_Pos[count]+1][Falling_Block.X_Pos[count]]!=0) || (Falling_Block.Y_Pos[count]>15))
    //        Finish_Flag=1;
    //}

    //if(Finish_Flag==0) {
    //    for(count=0;count<4;count++) {
    //        Falling_Block.Y_Pos[count]++;
    //    }
    //}

if(flag==0) {
    for(count=0;count<4;count++) {

        Draw_Tetris_Block(Falling_Block.X_Pos[count],Falling_Block.Y_Pos[count],Falling_Block.color);
        if(Falling_Block.Y_Pos[count]>=0)
            Game_Grid[Falling_Block.Y_Pos[count]][Falling_Block.X_Pos[count]]=Falling_Block.color;
    }
}

if(flag==1) {
    for(count=0;count<4;count++) {

        Draw_Tetris_Block(Falling_Block.X_Pos[count],Falling_Block.Y_Pos[count],Falling_Block.color);
        if(Falling_Block.Y_Pos[count]>=0)
            Game_Grid[Falling_Block.Y_Pos[count]][Falling_Block.X_Pos[count]]=-1;
    }


    for(count=0;count<4;count++) {

        if(Game_Grid[Falling_Block.Y_Pos[count]+1][Falling_Block.X_Pos[count]]>0) {
            for(count2=0;count2<4;count2++)
                Game_Grid[Falling_Block.Y_Pos[count]][Falling_Block.X_Pos[count]]=Falling_Block.color;
        }
    }
}

}

/////////////////////////////////////////////////////////////////////////////

void Draw_Next_Piece(void) {

    unsigned int count;

    //First, let's clear that little space....
    for(count=62;count<93;count++) {
        hline(screen,252,count,285,0);
    }

    if(Next_Piece_Flag) {
    //Done....now let's draw the new piece!
    for(count=0;count<4;count++) {
        blit(blocks[Next_Block.color-1],screen,0,0,
            HOR_SEGMENT + ((14+Next_Block.X_Pos[count])*8),
            VER_SEGMENT + ((4+Next_Block.Y_Pos[count])*8),8,8);
        }
    }
}

/////////////////////////////////////////////////////////////////////////////

void Write_Text(void) {

    text_mode(-1);
    textout(screen,font,"NEXT:",248,49,0);
    textout(screen,font,"NEXT:",247,48,15);

    textout(screen,font,"SCORE:",216,11,0);
    textout(screen,font,"SCORE:",215,10,15);

    textout(screen,font,"LINES:",265,129,0);
    textout(screen,font,"LINES:",264,128,15);

    textout(screen,font,"LEVEL:",39,130,0);
    textout(screen,font,"LEVEL:",38,129,15);
}

/////////////////////////////////////////////////////////////////////////////

void Deal_With_Score(int amount) {

        char buffer[16];
        int count,count2;

        score+=amount;

        if(score<10)
            score_left_pos=292;
        if(score>=10)
            score_left_pos=283;
        if(score>=100)
            score_left_pos=274;
        if(score>=1000)
            score_left_pos=265;
        if(score>=10000)
            score_left_pos=256;
        if(score>=1000000)
            score_left_pos=247;


        for(count2=25;count2<=37;count2++)
           hline(screen,219,count2,304,0);

        sprintf(buffer,"%ld",score);
        text_mode(0);
        textout(screen,font,buffer,score_left_pos,28,15);
        text_mode(-1);

}

///////////////////////////////////////////////////////////////////////////////

void Deal_With_Lines(void) {

     char buffer[16];
     int line_left_pos,count;

     if(lines<10)
            line_left_pos=290;
        if(lines>=10)
            line_left_pos=281;
        if(lines>=100)
            line_left_pos=272;
        if(lines>=1000)
            line_left_pos=263;

        for(count=145;count<=153;count++)
        hline(screen,271,count,300,0);

        sprintf(buffer,"%d",lines);
        text_mode(0);
        textout(screen,font,buffer,line_left_pos,145,15);
        text_mode(-1);
}

/////////////////////////////////////////////////////////////////////////////

void Initialize_Height(void) {

    int count,count2;
    int Draw_Height;
    int temp;

    Draw_Height=height*2;

    //now, let's fill the tetris screen with some crap...

    for(count=16-Draw_Height;count<16;count++) {
        for(count2=0;count2<10;count2++) {

            if(rand()%2==1) {
                temp=(rand()%7)+1;
                Draw_Tetris_Block(count2,count,temp);
                Game_Grid[count][count2]=temp;
            }
        }
    }

}

/////////////////////////////////////////////////////////////////////////////

void Do_Options_Screen(void) {

    BITMAP *option_screen;
    sb_sample *rotate_sound,*gong, *rotate_sound_conv,*gong_conv;

    int count,count2,counter_flag=0;
    RGB theColor,color1;
    PALETTE option_palette;
    int theChar;

    option_screen=load_pcx("option.pcx",option_palette);
    set_palette(option_palette);
    blit(option_screen,screen,0,0,0,0,320,200);
    destroy_bitmap(option_screen);

    if(sound_enable) {
    rotate_sound_conv=sb_load_sample("selectro.wav",_SB_WAV);
    rotate_sound=sb_convert_frequency(rotate_sound_conv,11025,sb_sample_frequency);
    sb_free_sample(rotate_sound_conv);

    gong_conv=sb_load_sample("select.wav",_SB_WAV);
    gong=sb_convert_frequency(gong_conv,11025,sb_sample_frequency);
    sb_free_sample(gong_conv);
    }

    text_mode(-1);
    textout(screen,font,"0  1  2  3  4  5  6  7  8  9",80,59,15);
    textout(screen,font,"0  1  2  3  4",80,85,15);
    textout(screen,font,"CLOCKWISE   COUNTERCLOCKWISE",70,111,15);
    textout(screen,font,"A   B",78,137,15);
    textout(screen,font,"A   B   C",50,163,15);
    textout(screen,font,"1 2 3 4 5 6",212,137,15);
    text_mode(0);
    theChar=0;
    Game_Speed=0;
    clear_keybuf();


   if(sound_enable) {
   tune1=sb_load_mod_file("tune1.mod");
   tune2=sb_load_mod_file("tune2.mod");
   tune3=sb_load_mod_file("tune3.mod");
   }

   Draw_Color_Rotating_Line(0);

   if(sound_enable)
      sb_mod_play(tune1);

    while((theChar >> 8)!=KEY_ENTER) {

       if(keypressed())
          theChar=readkey();
       else
          theChar=0;

        rest(100);

         ///////
           get_color(104,&color1);

           for(count=105;count<=255;count++) {

           get_color(count,&theColor);
           set_color(count-1,&theColor);
           }

           set_color(255,&color1);
        ///////

        get_color(18,&color1);

        for(count=19;count<=21;count++) {

            get_color(count,&theColor);
            set_color(count-1,&theColor);
        }

        set_color(21,&color1);

        switch(theChar >> 8) {

                case KEY_UP:

                            Draw_Black_Line(counter_flag);
                            if(counter_flag>0)
                                 counter_flag--;
                            else
                                 counter_flag=5;
                            Draw_Color_Rotating_Line(counter_flag);
                            if(sound_enable)
                               sb_mix_sample(rotate_sound);
                            break;
                case KEY_DOWN:
                            Draw_Black_Line(counter_flag);
                            if(counter_flag<5)
                                 counter_flag++;
                            else
                                counter_flag=0;
                            Draw_Color_Rotating_Line(counter_flag);
                            if(sound_enable)
                                 sb_mix_sample(rotate_sound);
                            break;
                case KEY_LEFT:
                              switch(counter_flag) {
                                       case 0:
                                          Draw_Game_Speed_Box(Game_Speed,Game_Speed-1);
                                          if(sound_enable)
                                              sb_mix_sample(rotate_sound);
                                          break;
                                       case 1:
                                          Draw_Height_Box(height,height-1);
                                          if(sound_enable)
                                             sb_mix_sample(rotate_sound);
                                          break;
                                       case 2:
                                          Draw_Rotation_Box();
                                          if(sound_enable)
                                             sb_mix_sample(rotate_sound);
                                          break;
                                       case 3:
                                          Draw_Game_B_Box();
                                          if(sound_enable)
                                             sb_mix_sample(rotate_sound);
                                          break;
                                       case 4:
                                          Choose_Music(mod_to_play,mod_to_play-1);
                                          if(sound_enable)
                                              sb_mix_sample(rotate_sound);
                                          break;
                                       case 5:
                                          Set_Mod_Volume(mod_vol,mod_vol-1);
                                          if(sound_enable)
                                             sb_mix_sample(rotate_sound);
                                          break;
                                       case 6:
                                          break;
                                       }
                               break;
                case KEY_RIGHT:
                               switch(counter_flag) {
                                       case 0:
                                          Draw_Game_Speed_Box(Game_Speed,Game_Speed+1);
                                          if(sound_enable)
                                               sb_mix_sample(rotate_sound);
                                          break;
                                       case 1:
                                          Draw_Height_Box(height,height+1);
                                          if(sound_enable)
                                             sb_mix_sample(rotate_sound);
                                          break;
                                       case 2:
                                          Draw_Rotation_Box();
                                          if(sound_enable)
                                              sb_mix_sample(rotate_sound);
                                          break;
                                       case 3:
                                          Draw_Game_B_Box();
                                          if(sound_enable)
                                              sb_mix_sample(rotate_sound);
                                          break;
                                       case 4:
                                          Choose_Music(mod_to_play,mod_to_play+1);
                                          if(sound_enable)
                                             sb_mix_sample(rotate_sound);
                                          break;
                                       case 5:
                                          Set_Mod_Volume(mod_vol,mod_vol+1);
                                          if(sound_enable)
                                             sb_mix_sample(rotate_sound);
                                          break;
                                       case 6:
                                          break;
                                       }
                               break;
                default:
                            break;
        }

    }

    if(sound_enable) {
    sb_free_sample(rotate_sound);
    sb_mod_pause();
    sb_free_mod_file(tune1);
    sb_free_mod_file(tune2);
    sb_free_mod_file(tune3);

    sb_mix_sample(gong);


    for(count2=0;count2<22;count2++) {
             ///////
           get_color(104,&color1);

           for(count=105;count<=255;count++) {

           get_color(count,&theColor);
           set_color(count-1,&theColor);
           }

           set_color(255,&color1);
        ///////
    rest(100);
    }


         sb_free_sample(gong);
    }
}

////////////////////////////////////////////////////////////////////////////

void Draw_Game_B_Box(void) {

      if(b_game_flag==0) {
           Draw_Black_Box(74,133,90,149);
           Draw_Color_Rotating_Box(106,133,122,149);
           b_game_flag=1;
      }
      else
      {
           Draw_Black_Box(106,133,122,149);
           Draw_Color_Rotating_Box(74,133,90,149);
           b_game_flag=0;
      }
}

////////////////////////////////////////////////////////////////////////////
void Draw_Black_Line(int position) {

      switch(position) {
                       case 0:
                              hline(screen,20,70,71,0);
                              break;
                       case 1:
                              hline(screen,20,95,45,0);
                              break;
                       case 2:
                              hline(screen,20,121,51,0);
                              break;
                       case 3:
                              hline(screen,20,148,67,0);
                              break;
                       case 4:
                              hline(screen,20,175,38,0);
                              break;
                       case 5:
                              hline(screen,140,147,201,0);
                              break;
                       case 6:
                              hline(screen,140,174,248,0);
                              break;
                       }
}

////////////////////////////////////////////////////////////////////////////

void Draw_Color_Rotating_Line(int position) {

      switch(position) {
                       case 0:
                              hline(screen,20,70,71,18);
                              break;
                       case 1:
                              hline(screen,20,95,45,18);
                              break;
                       case 2:
                              hline(screen,20,121,51,18);
                              break;
                       case 3:
                              hline(screen,20,148,67,18);
                              break;
                       case 4:
                              hline(screen,20,175,38,18);
                              break;
                       case 5:
                              hline(screen,140,147,201,18);
                              break;
                       case 6:
                              hline(screen,141,174,248,18);
                              break;
                       }
}

////////////////////////////////////////////////////////////////////////////
void Draw_Color_Rotating_Box(int tx, int ty, int bx, int by) {

   int count,current_color=18;

   for(count=tx;count<=bx;count++) {
        putpixel(screen,count,ty,current_color);
        if(current_color<21)
           current_color++;
        else
           current_color=18;
   }

   for(count=ty+1;count<by;count++) {
        putpixel(screen,bx,count,current_color);
        if(current_color<21)
           current_color++;
        else
           current_color=18;
   }

   for(count=bx;count>=tx;count--) {
        putpixel(screen,count,by,current_color);
        if(current_color<21)
           current_color++;
        else
           current_color=18;
   }

   for(count=by-1;count>ty;count--) {
        putpixel(screen,tx,count,current_color);
        if(current_color<21)
           current_color++;
        else
           current_color=18;
   }
}

////////////////////////////////////////////////////////////////////////////

void Draw_Black_Box(int tx, int ty, int bx, int by) {

   hline(screen,tx,ty,bx,0);
   hline(screen,tx,by,bx,0);
   vline(screen,tx,ty,by,0);
   vline(screen,bx,ty,by,0);

}

////////////////////////////////////////////////////////////////////////////
void Initialize_Brick(void) {

    int count;

    //Give the block a shape...
    Falling_Block.shape = rand()%7;

    //...and standard rotation.
    Falling_Block.Rotation = 0;

    Check_Starting_Rotation(Falling_Block.Rotation);

    //check for piece overlap...a sure sign of death...

    for(count=0;count<4;count++) {
        if(Game_Grid[Falling_Block.Y_Pos[count]][Falling_Block.X_Pos[count]]>0) {
            Draw_Tetris_Full_Brick();
            num_pieces[Falling_Block.shape]++;
            rest(300);
            Die();
        }
    }


}       //Ya know, I think my work here is done.......

///////////////////////////////////////////////////////////////////////////////

void Check_Starting_Rotation(int rotation) {

        //Figure out the starting positions and color for each shape.
    switch(Falling_Block.shape) {
                    case 0:         //Straight line
                            Falling_Block.X_Pos[0]=3;
                            Falling_Block.Y_Pos[0]=0;
                            Falling_Block.X_Pos[1]=4;
                            Falling_Block.Y_Pos[1]=0;
                            Falling_Block.X_Pos[2]=5;
                            Falling_Block.Y_Pos[2]=0;
                            Falling_Block.X_Pos[3]=6;
                            Falling_Block.Y_Pos[3]=0;
                            Falling_Block.color=1;
                            break;
                    case 1:         //square
                            Falling_Block.X_Pos[0]=4;
                            Falling_Block.Y_Pos[0]=0;
                            Falling_Block.X_Pos[1]=5;
                            Falling_Block.Y_Pos[1]=0;
                            Falling_Block.X_Pos[2]=4;
                            Falling_Block.Y_Pos[2]=1;
                            Falling_Block.X_Pos[3]=5;
                            Falling_Block.Y_Pos[3]=1;
                            Falling_Block.color=2;
                            break;
                    case 2:         //L shaped piece
                            Falling_Block.X_Pos[0]=4;
                            Falling_Block.Y_Pos[0]=1;
                            Falling_Block.X_Pos[1]=5;
                            Falling_Block.Y_Pos[1]=1;
                            Falling_Block.X_Pos[2]=6;
                            Falling_Block.Y_Pos[2]=1;
                            Falling_Block.X_Pos[3]=6;
                            Falling_Block.Y_Pos[3]=0;
                            Falling_Block.color=3;
                            break;
                    case 3:         //Backwards L shaped piece
                            Falling_Block.X_Pos[0]=3;
                            Falling_Block.Y_Pos[0]=0;
                            Falling_Block.X_Pos[1]=3;
                            Falling_Block.Y_Pos[1]=1;
                            Falling_Block.X_Pos[2]=4;
                            Falling_Block.Y_Pos[2]=1;
                            Falling_Block.X_Pos[3]=5;
                            Falling_Block.Y_Pos[3]=1;
                            Falling_Block.color=4;
                            break;
                    case 4:         //T shaped piece
                            Falling_Block.X_Pos[0]=3;
                            Falling_Block.Y_Pos[0]=1;
                            Falling_Block.X_Pos[1]=4;
                            Falling_Block.Y_Pos[1]=1;
                            Falling_Block.X_Pos[2]=4;
                            Falling_Block.Y_Pos[2]=0;
                            Falling_Block.X_Pos[3]=5;
                            Falling_Block.Y_Pos[3]=1;
                            Falling_Block.color=5;
                            break;
                    case 5:        //squiggly piece 1
                            Falling_Block.X_Pos[0]=3;
                            Falling_Block.Y_Pos[0]=1;
                            Falling_Block.X_Pos[1]=4;
                            Falling_Block.Y_Pos[1]=1;
                            Falling_Block.X_Pos[2]=4;
                            Falling_Block.Y_Pos[2]=0;
                            Falling_Block.X_Pos[3]=5;
                            Falling_Block.Y_Pos[3]=0;
                            Falling_Block.color=6;
                            break;
                    case 6:        //squiggly piece 2
                            Falling_Block.X_Pos[0]=3;
                            Falling_Block.Y_Pos[0]=0;
                            Falling_Block.X_Pos[1]=4;
                            Falling_Block.Y_Pos[1]=0;
                            Falling_Block.X_Pos[2]=4;
                            Falling_Block.Y_Pos[2]=1;
                            Falling_Block.X_Pos[3]=5;
                            Falling_Block.Y_Pos[3]=1;
                            Falling_Block.color=7;
                            break;
    }      //end case statement

}

////////////////////////////////////////////////////////////////////////////


void Draw_Tetris_Block(int X, int Y, int color) {

    if(Y>=0) {
    blit(blocks[color-1],screen,0,0,HOR_SEGMENT+(X*8),VER_SEGMENT+(Y*8),8,8);
    Game_Grid[Y][X]=  -1;               //Falling block
    }

}

///////////////////////////////////////////////////////////////////////////////

void Draw_Tetris_Full_Brick(void) {

    int count;

    for(count=0;count<4;count++) {

        Draw_Tetris_Block(Falling_Block.X_Pos[count], Falling_Block.Y_Pos[count],
        Falling_Block.color);

    }
}

///////////////////////////////////////////////////////////////////////////////

void Erase_Tetris_Block(int X, int Y) {

    if(Y>=0) {
    blit(blocks[7],screen,0,0,HOR_SEGMENT+(X*8),VER_SEGMENT+(Y*8),8,8);
    Game_Grid[Y][X] = 0;                //Get rid of old.......
    }


}

///////////////////////////////////////////////////////////////////////////////

void Erase_Tetris_Full_Brick(void) {

   int count;

   for(count=0;count<4;count++)
       Erase_Tetris_Block(Falling_Block.X_Pos[count], Falling_Block.Y_Pos[count]);

}

////////////////////////////////////////////////////////////////////////////

void Die(void) {

    int count,count2;
    sb_sample *die_sound,*die_sound_conv;

    if(sound_enable) {
    die_sound_conv=sb_load_sample("gameover.wav",_SB_WAV);
    die_sound=sb_convert_frequency(die_sound_conv,11025,sb_sample_frequency);
    sb_free_sample(die_sound_conv);
    sb_mod_pause();
    sb_mix_sample(die_sound);

    }

    for(count=0;count<16;count++) {
        for(count2=0;count2<10;count2++) {
            Draw_Tetris_Block(count2,count,9);
        }
        rest(50);
    }

    for(count=66;count<81;count++)
        hline(screen,119,count,193,0);

    textout(screen,font,"GAME OVER",121,70,15);

    clear_keybuf();
    while(!keypressed()) {}

    //sb_free_sample(die_sound);
    Game_Over();
}

/////////////////////////////////////////////////////////////////////////////

void Game_Over(void) {

    int theChar=0,count,count2;
    BITMAP *finish_pcx;
    PALETTE finish;
    sb_sample *die_sound,*die_sound_conv,*ugh,*ugh_conv,*yay,*yay_conv;


    if(sound_enable) {

    ugh_conv=sb_load_sample("no.wav",_SB_WAV);
    ugh=sb_convert_frequency(ugh_conv,11025,sb_sample_frequency);
    sb_free_sample(ugh_conv);

    yay_conv=sb_load_sample("applause.wav",_SB_WAV);
    yay=sb_convert_frequency(yay_conv,11025,sb_sample_frequency);
    sb_free_sample(yay_conv);

    sb_mod_pause();

    }

    fade_out(2);
    Do_Stats();
    High_Score_Stuff();

    textout(screen,font,"                          ",49,173,0);
    textout(screen,font,"Press ENTER to continue.",62,174,15);
    theChar=0;
    clear_keybuf();
    while(!keypressed()) {}

    //Clear the center part of the screen....
    for(count=70;count<=130;count+=3) {
        hline(screen,48,count+1,268,0);
        hline(screen,48,count,268,0);
        hline(screen,48,count+2,268,0);
    }

    hline(screen,47,69,269,1);
    hline(screen,47,131,269,1);
    hline(screen,46,68,270,2);
    hline(screen,46,132,270,2);
    hline(screen,45,67,271,1);
    hline(screen,45,133,271,1);
    vline(screen,47,69,131,1);
    vline(screen,269,69,131,1);
    vline(screen,46,68,132,2);
    vline(screen,270,68,132,2);
    vline(screen,45,67,133,1);
    vline(screen,271,67,133,1);

    textout(screen,font,"Are ya gonna play again?",62,75,15);
    textout(screen,font,"   Y - Yeah, I will!",62,100,15);
    textout(screen,font,"      N - No way!",62,109,15);

    //sb_free_sample(die_sound);

    clear_keybuf();
    while(1) {

    if(keypressed())
       theChar=readkey();

       theChar=(theChar >> 8);

        if(theChar==KEY_Y) {
                if(sound_enable) {
                     sb_mix_sample(yay);
                     rest(4000);
                }
                fade_out(2);
                if(sound_enable) {
                    sb_free_sample(yay);
                    sb_free_sample(ugh);
                }
                height=0;
                lines=0;
                singles=0;
                doubles=0;
                triples=0;
                tetrises=0;
                red_flag=0;
                mod_vol=3;
                mod_to_play=0;
                b_game_flag=0;
                Do_Options_Screen();
                fade_out(2);
                Init_Sequence();
                break;
        }

        if((theChar==KEY_N)) {

         if(sound_enable) {
             sb_mix_sample(ugh);
             rest(1500);
         }
         fade_out(2);
         finish_pcx=load_pcx("credits.pcx",finish);
         set_palette(finish);
         blit(finish_pcx,screen,0,0,0,0,320,200);
            clear_keybuf();
            while(!keypressed()) {}

            fade_out(2);
            if(sound_enable) {
            sb_free_sample(yay);
            sb_free_sample(ugh);
            }

            set_gfx_mode(GFX_TEXT,80,25,0,0);
            exit(0);
        }
    }

}

////////////////////////////////////////////////////////////////////////////

void Draw_Game_Speed_Box(int oldSpeed, int newSpeed) {

    int offset_x=75+(oldSpeed*24);
    int offset_y=54;
    int count;

    if(newSpeed<0)
      newSpeed=9;

    if(newSpeed>9)
      newSpeed=0;

    //first, erase the old box....

    Draw_Black_Box(offset_x,offset_y,offset_x+16,offset_y+16);

    //Now, let's TRY to draw the new box....

   offset_x=75+(newSpeed*24);

   Draw_Color_Rotating_Box(offset_x,offset_y,offset_x+16,offset_y+16);

   Game_Speed=newSpeed;
}

////////////////////////////////////////////////////////////////////////////

void Draw_Height_Box(int oldHeight, int newHeight) {


        int offset_x_old;
        int offset_x_new;
        int offset_y=80;


        if(newHeight<0)
           newHeight=4;
        if(newHeight>4)
           newHeight=0;

        offset_x_old=75+(oldHeight*24);
        offset_x_new=75+(newHeight*24);

        //First step is to erase the old box
        Draw_Black_Box(offset_x_old,offset_y,offset_x_old+16,offset_y+16);

        //Good.....now let's draw the new box...
        Draw_Color_Rotating_Box(offset_x_new,offset_y,offset_x_new+16,offset_y+16);

        height=newHeight;

}

////////////////////////////////////////////////////////////////////////////

void Draw_Rotation_Box(void) {

    int y_offset=106;

    //First thing's first....erase the old box...

    if(rotation==0)
       Draw_Black_Box(62,y_offset,150,y_offset+16);
    if(rotation==1)
       Draw_Black_Box(158,y_offset,302,y_offset+16);

    //Now, draw the new box...and assign the new rotation value....

    if(rotation==0) {
        rotation=1;
        Draw_Color_Rotating_Box(158,y_offset,302,y_offset+16);
    }
    else {
        rotation=0;
        Draw_Color_Rotating_Box(62,y_offset,150,y_offset+16);
    }

}

////////////////////////////////////////////////////////////////////////////

void Choose_Music(int oldmod, int newmod) {

   int x_offset=46;
   int y_offset=159;

   if(newmod<0)
     newmod=2;
   if(newmod>2)
     newmod=0;

   x_offset+=(oldmod*32);

   Draw_Black_Box(x_offset,y_offset,x_offset+16,y_offset+16);

   x_offset=46+(newmod*32);

   Draw_Color_Rotating_Box(x_offset,y_offset,x_offset+16,y_offset+16);

   mod_to_play=newmod;

   if(sound_enable) {
   sb_mod_pause();
   if(mod_to_play==0)
      sb_mod_play(tune1);
   if(mod_to_play==1)
      sb_mod_play(tune2);
   if(mod_to_play==2)
      sb_mod_play(tune3);
   }

}
////////////////////////////////////////////////////////////////////////////
void High_Score_Stuff(void) {

    int count,count2,count3;
    char Names[11][11], theChar,flag,flag2;
    long scores[10];
    int t_lines[10];
    PALETTE high_score_palette;
    BITMAP *High_Score_Screen;
    FILE *fp;
    char buffer[40];

    High_Score_Screen=load_pcx("highsc.pcx",high_score_palette);
    set_palette(high_score_palette);
    blit(High_Score_Screen,screen,0,0,0,0,320,200);

    destroy_bitmap(High_Score_Screen);

    if((fp=fopen("concept9.con","r"))==NULL) {
        set_gfx_mode(GFX_TEXT,80,25,0,0);
        printf("No high score info exists.....press any key");
        while(!keypressed()) {}
        set_gfx_mode(GFX_VGA,320,200,0,0);
        return;
    }

    for(count=0;count<9;count++) {

        fscanf(fp, "%s %ld %d", &Names[count],&scores[count],&t_lines[count]);

        sprintf(buffer,"%d. %s %d", count+1, Names[count], t_lines[count]);
        textout(screen,font,buffer,67,50+(10*count),15);
        sprintf(buffer,"%ld",scores[count]);
        textout(screen,font,buffer,219,50+(10*count),15);
    }

        count=9;
        fscanf(fp, "%s %ld %d", &Names[count],&scores[count],&t_lines[count]);
        sprintf(buffer,"%d. %s %d", count+1, Names[count], t_lines[count]);
        textout(screen,font,buffer,59,50+(10*count),15);
        sprintf(buffer,"%ld",scores[count]);
        textout(screen,font,buffer,219,50+(10*count),15);

        fclose(fp);

   //Now, check to see if a new high score has been attained.......
    for(count=0;count<=9;count++) {
        if(score>=scores[count]) {
    text_mode(0);
    for(count2=8;count2>=count;count2--) {
      sprintf(buffer,"%d. %s %d", count2+2, Names[count2], t_lines[count2]);
        if((count2+1)==9)
            textout(screen,font,buffer,59,50+(10*(count2+1)),15);
        else
            textout(screen,font,buffer,67,50+(10*(count2+1)),15);
      sprintf(buffer,"%ld",scores[count2]);
      textout(screen,font,buffer,219,50+(10*(count2+1)),15);
      }

          text_mode(-1);
          Update_High_Score(count);


            //Now, update the data so the new and old stuff jive....

            for(count2=9;count2>count;count2--) {
                for(count3=0;count3<11;count3++)
                Names[count2][count3]=Names[count2-1][count3];
                t_lines[count2]=t_lines[count2-1];
                scores[count2]=scores[count2-1];
            }

            for(count2=0;count2<11;count2++)
                Names[count][count2]=name[count2];
            t_lines[count]=lines;
            scores[count]=score;

            count=10;
        }

    }

    //Write the data back to the file...

    if((fp=fopen("concept9.con","w"))==NULL) {
        set_gfx_mode(GFX_TEXT,80,25,0,0);
        printf("High score info cannot be written.....press any key");
        while(!keypressed()) {}
        set_gfx_mode(GFX_VGA,320,200,0,0);
        return;
    }

     for(count=0;count<=9;count++) {

        fprintf(fp, "%c%c%c%c%c%c%c%c%c%c\n%ld\n%d\n", Names[count][0],Names[count][1],Names[count][2],Names[count][3],Names[count][4],Names[count][5],Names[count][6],Names[count][7],Names[count][8],Names[count][9],scores[count],t_lines[count]);

    }

    fclose(fp);

 }

/////////////////////////////////////////////////////////////////////////////

void Update_High_Score(int position) {

    FILE *fp;
    char buffer[27];
    int theChar=0;
    int counter,count;
    sb_sample *applause,*applause_conv;

    text_mode(0);

    if(sound_enable) {
    applause_conv=sb_load_sample("applause.wav",_SB_WAV);
    applause=sb_convert_frequency(applause_conv,11025,sb_sample_frequency);
    sb_free_sample(applause_conv);
    sb_mix_sample(applause);
    }
    //Find the position where the score is, highlight that position,
    //and clear it!!!

    if(position==9) {
        textout(screen,font,"10.                       ",59,50+(10*position),1);
    }
    else {
        sprintf(buffer,"%d.                       ",position+1);
        textout(screen,font,buffer,67,50+(10*position),1);
    }


    //First, inform the user they have a new high score

    textout(screen,font,"You have a new high score!!!",49,174,15);
    rest(2000);

    textout(screen,font,"You have a new high score!!!",49,174,0);
    textout(screen,font,"Name: ",49,174,15);

    counter=0;
    theChar=0;

    while((theChar >> 8)!=KEY_ENTER) {        //the carriage return (ENTER)

        if(!keypressed())
               theChar=0;

        if(keypressed())
               theChar=readkey();

        if(((theChar >> 8)==KEY_BACKSPACE) && (counter>0)) {     //backspace
            counter--;
            textout(screen,font," ",110+(8*counter),173,0);
            name[counter]=' ';
        }

        if(((theChar & 0xff)>=33) && ((theChar & 0xff)<=122) && (counter<8)) {  //it's a key...draw it
            sprintf(buffer,"%c",theChar);
            textout(screen,font,buffer,(110+(8*counter)),173,15);
            name[counter]=theChar;
            counter++;
            }

        //otherwise, do nada....
   }


        //now, let's terminate the string proper....

        for(count=counter;count<10;count++)
            name[count]='.';
            name[11]='\0';

        //now, blit the new string to the screen to show off the score...

        sprintf(buffer,"%s %d", name, lines);
        textout(screen,font,buffer,91,50+(10*position),1);
        sprintf(buffer,"%ld",score);
        textout(screen,font,buffer,219,50+(10*position),1);


        rest(3000);

        if(sound_enable)
        sb_free_sample(applause);

}

/////////////////////////////////////////////////////////////////////////////

int Check_For_Keypress(void) {

   rest(10);
   textout(screen, font, "1997 -- Seamonkey (aka Shaun Brandt)",16,181,250);
   text_mode(-1);
   textout(screen, font, "1997 -- Seamonkey (aka Shaun Brandt)",15,180,1);
   text_mode(0);


   if(!keypressed())
      return 0;
   else
      return 1;
}

/////////////////////////////////////////////////////////////////////////////

void Parse_Sound_Cfg_File(void) {

   FILE *fp;

   //attempt to open the file for reading.

   if((fp=fopen("sound.cfg","r"))==NULL) {
       printf("Could not open sound config file.\n\n");
       sound_enable=0;
       rest(2000);
       return;
   }

      //read in the settings
      fscanf(fp,"%d %d %d %d %d %d %d", &sound_enable,&frequency,&DMA, &mod_vol,
        &SFX_vol, &setup_flag,&parse_flag);

      fclose(fp);

      if(parse_flag!=999) {
         printf("Error in sound.cfg file.  Game will be silent.\n\n");
         sound_enable=0;
         rest(2000);
         return;
      }

      //Everything is good.   Now let's prepare to set these values.
           printf("sound.cfg parsed.\n\n");
           if(sound_enable==0) {
               printf("Sound is disabled.\n\n");
           }
           else {
                if(setup_flag==1) {
                printf("Initializing Sound Blaster with DMA %d, at a sample rate of %dHz.\n",DMA,frequency);
                printf("Mod volume is %d.  SFX volume is %d.\n",mod_vol,SFX_vol);
                }
           }

   rest(2000);

}

/////////////////////////////////////////////////////////////////////////////

void Set_Mod_Volume(int oldvol, int newvol) {

    int x_offset_old;
    int x_offset_new;
    int y_offset=134;

    if(newvol<1)
       newvol=6;
    if(newvol>6)
       newvol=1;

       x_offset_old=208+(16*(oldvol-1));
       x_offset_new=208+(16*(newvol-1));
    //First, erase the old box
    Draw_Black_Box(x_offset_old,y_offset,x_offset_old+13,y_offset+13);

    //Now, draw the new box
    Draw_Color_Rotating_Box(x_offset_new,y_offset,x_offset_new+13,y_offset+13);

    sb_mod_volume=(12-(newvol));
    mod_vol=newvol;

}

void Do_Setup(void) {

   int theChar;
   FILE *fp;

   printf("Sound Setup Utility\n\n");
   printf("This utility will set up your sound card to play music and sound EFX.\n");
   printf("The answers to the following questions will be saved to a file called\n");
   printf("SOUND.CFG, so these questions will not be asked every time you play.\n\n");
   printf("Do you wish to enable sound? (If you don't have a Soundblaster, or if\n");
   printf("you don't want sound, say (N)o, otherwise, press any other key for (Y)es...)\n\n");

   while(!keypressed()) {}
           theChar=readkey();

   theChar = theChar >> 8;

   if(theChar==KEY_N)  {
       sound_enable=0;
       printf("Sound has been disabled.\n\n");
   }
   else {
        sound_enable=1;
        printf("Sound has been enabled.\n\n");
   }

   if(sound_enable==1) {

   clear_keybuf();
     printf("Now, choose a music sample playback rate.\n");
     printf("This number tells the computer what quality to play back\n");
     printf("sounds and music.  The faster your computer is, the higher\n");
     printf("number you should select.\n\n");
     printf("Available choices:\n");
     printf("   1.  11025 Hz -  For 386 class machines\n");
     printf("   2.  22050 Hz -  For 486/486DX class machines\n");
     printf("   3.  44100 Hz -  For 486DX2/DX4/Pentium class machines\n\n");

   while(!keypressed()) {}
        theChar = readkey();

   theChar = theChar >> 8;

   switch(theChar) {
                   case KEY_1:
                        frequency=11025;
                        break;
                   case KEY_2:
                        frequency=22050;
                        break;
                   case KEY_3:
                        frequency=44100;
                        break;
                   default:
                        frequency=22050;
                        printf("Machine has defaulted to 22050 Hz.\n\n");
                        break;
                   }



   if((fp=fopen("SOUND.CFG","w"))==NULL) {
      printf("An error has occured while attempting to write to SOUND.CFG\n");
      printf("Program terminating.....check disk for errors...\n");
      exit(1);
   }

   fprintf(fp,"%d\n%d\n%d\n%d\n%d\n%d\n%d",sound_enable,11025,1,3,32,1,999);
   DMA=1;
   mod_vol=3;
   SFX_vol=32;
   frequency=11025;

   fclose(fp);

   printf("Your machine has now been set up.  A quick note....the program\n");
   printf("has set the sound card's DMA to a default of 1.  This is the case\n");
   printf("for the great majority of computers.  However, if this is not the\n");
   printf("case for your machine, press ESC to quit the program now, and edit\n");
   printf("the SOUND.CFG file, changing the third number to the DMA of your\n");
   printf("sound card.  This setup utility will not run again.\n\n");
   printf("Press any key to start game.\n");

   clear_keybuf();
   while(!keypressed()) {}
  }
}

/////////////////////////////////////////////////////////////////////////////

void Do_Game_B_Rewards(void) {

      BITMAP *reward;
      PALETTE reward_palette;

      //put some flashy stuff here....

      //now, wait a sec
      rest(1500);

      //and fade away...
      fade_out(2);


            reward=load_pcx("con6813.pcx",reward_palette);
            set_palette(reward_palette);
            blit(reward,screen,0,0,0,0,320,200);

            rest(2000);

            clear_keybuf();

            while(!keypressed()) {}

            fade_out(2);

            destroy_bitmap(reward);
            Game_Over();

}

/////////////////////////////////////////////////////////////////////////////
void main(int argc, char *argv[]) {

   //Here begins the port of my little Tetris game to a 32-bit, protected mode
   //environment....yay.....let's hope it works!
   PALLETE pcx_palette, block_palette;
   BITMAP  *title_pcx, *company_pcx,*high_score_pcx;
   sb_status theStatus;

   int card, w, h;
   int count, theKey,break_flag=0;


   allegro_init();
   install_keyboard();
   install_timer();

   printf("\nYet Another Tetris Clone -- Copyright 1997 Seamonkey\n");
   printf("----------------------------------------------------\n\n");

   Parse_Sound_Cfg_File();
   if(setup_flag==0)
       Do_Setup();

   if(sound_enable) {
      if((theStatus=sb_install_driver(frequency))!=SB_SUCCESS) {
           printf("Sound initialization failed.\n");
           printf("Cause:  %s\n\n", sb_driver_error);
           printf("Game will be silent.\n\n");
           sound_enable=0;
      }
   }

   if(sound_enable) {
   sb_change_dma_channel(DMA);
   sb_mod_volume=(12-(mod_vol));
   }

   clear_keybuf();

   srand(time(NULL));
   card=GFX_VGA;
   w=320;
   h=200;

   //set the graphics mode
   set_gfx_mode(card, w, h, 0, 0);



   company_pcx=load_pcx("company.pcx",pcx_palette);

   //set the screen palette to the pcx file's palette
   set_palette(pcx_palette);

   //show the pcx file on the screen....
   blit(company_pcx,screen,0,0,0,0,320,200);
   textout(screen, font, "1997 -- Seamonkey (aka Shaun Brandt)",16,171,250);
   text_mode(-1);
   textout(screen, font, "1997 -- Seamonkey (aka Shaun Brandt)",15,170,228);
   text_mode(0);
   textout(screen, font, "Created with Allegro 2.2 for DJGPP",23,180,228);



   //wait for the almighty keypress.....
   while(!keypressed()) {}

   //vsync();
   fade_out(2);
   destroy_bitmap(company_pcx);

   //now....let's repeat for the title screen....
   title_pcx=load_pcx("title.pcx",pcx_palette);
   set_palette(pcx_palette);
   blit(title_pcx,screen,0,0,0,0,320,200);
   clear_keybuf();
   theKey=0;
   //start the infinite loop_thingy....
   while(1) {

      for(count=0;count<144;count++) {
         if(keypressed())
              theKey=readkey();
         if((theKey >> 8)==KEY_ENTER)
              break_flag=1;

         if(break_flag==1)
             break;

         rest((int)(1000/18.2));
      }

     if(break_flag==1)
            break;

     fade_out(2);

     High_Score_Stuff();

      for(count=0;count<144;count++) {
         if(keypressed())
              theKey=readkey();
         if((theKey >> 8)==KEY_ENTER)
              break_flag=1;

         if(break_flag==1)
             break;

         rest((int)(1000/18.2));
      }

     if(break_flag==1)
            break;

     fade_out(2);

     set_palette(pcx_palette);
     blit(title_pcx,screen,0,0,0,0,320,200);

   }

   fade_out(2);
   Do_Options_Screen();
   fade_out(2);

   //initing of the sprites...
   blocks[0]=load_pcx("block0.pcx",block_palette);
   blocks[1]=load_pcx("block1.pcx",block_palette);
   blocks[2]=load_pcx("block2.pcx",block_palette);
   blocks[3]=load_pcx("block3.pcx",block_palette);
   blocks[4]=load_pcx("block4.pcx",block_palette);
   blocks[5]=load_pcx("block5.pcx",block_palette);
   blocks[6]=load_pcx("block6.pcx",block_palette);
   blocks[7]=load_pcx("block7.pcx",block_palette);
   blocks[8]=load_pcx("block8.pcx",block_palette);

   destroy_bitmap(title_pcx);

   if(sound_enable) {
   line_sound_conv=sb_load_sample("linecler.wav",_SB_WAV);
   line_sound=sb_convert_frequency(line_sound_conv,11025,sb_sample_frequency);
   sb_free_sample(line_sound_conv);
   neartop_conv=sb_load_sample("neartop.wav",_SB_WAV);
   neartop=sb_convert_frequency(neartop_conv,11025,sb_sample_frequency);
   sb_free_sample(neartop_conv);
   }

   Init_Sequence();

   set_gfx_mode(GFX_TEXT,80,25,0,0);
   allegro_exit();

   if(sound_enable)
        sb_uninstall_driver();

   exit(0);
}



