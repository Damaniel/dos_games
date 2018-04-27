#include "d:\source\day_03\graph3.h"
#include "d:\source\day_04\graph4.h"
#include "d:\source\day_07\graph7k.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define HOR_SEGMENT   116
#define VER_SEGMENT    37

// Important data structures.... //////////////////////////////////////////////

int Game_Speed, Left_Flag, Right_Flag;
int Game_Over_Flag =0;
long score=0;
int score_left_pos;
int lines=0;
int Counter_Flag=0;
int height=0;
int rotation=0;           //clockwise
char name[12];
// statistics...

int singles=0,doubles=0,triples=0,tetrises=0;
int num_pieces[7];

char Game_Grid[16][10];

typedef struct Block {

    int shape;
    int Rotation;
    char X_Pos[4];
    char Y_Pos[4];
    char color;

} Brick;

Brick Falling_Block, Next_Block;

sprite Blocks;

///////////////////////////////////////////////////////////////////////////////

void Init_Sequence(void);
void Initialize_Game(void);
void Load_Background(int Speed);
void Write_Text(void);
void Initialize_Brick(void);
void Draw_Tetris_Block(int X, int Y, int color);
void Draw_Tetris_Full_Brick(void);
void Game_Loop(void);
void Ground_Check(void);
void Keyboard_Poller(void);
void Do_Line_Cleanup(int line);
void Die(void);
void Deal_With_Score(int amount);
void Rotate(void);
void Drop_The_Piece(int flag);
void Deal_With_Lines(void);
void Fade_Lights(void);
void Do_Stats(void);
void Check_Starting_Rotation(int rotation);
void Initialize_New_Brick(void);
void Draw_Next_Piece(void);
void Do_Options_Screen(void);
void Draw_Game_Speed_Box(int oldSpeed, int newSpeed);
void Draw_Height_Box(int oldHeight, int newHeight);
void Draw_Rotation_Box(void);
void Venetian_Blinds_File(char *file);
void Initialize_Height(void);
void High_Score_Stuff(void);
void Update_High_Score(int position);
void clear_kb(void);

///////////////////////////////////////////////////////////////////////////////

void Init_Sequence(void) {

    int count;
    char buffer[8];

    for(count=0;count<7;count++)
        num_pieces[count]=0;

    //Initial score
    score = 0;
    lines = 0;

    Load_Background(Game_Speed);
    Write_Text();
    Deal_With_Score(0);
    Deal_With_Lines();
    sprintf(buffer,"%d",Game_Speed);
    Blit_String(57,147,15,buffer,0);

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

    Game_Loop();

}

///////////////////////////////////////////////////////////////////////////////

void Initialize_Game(void) {

    int count, count2;
    for(count=0;count<10;count++) {
        for(count2=0;count2<16;count2++) {
            Game_Grid[count2][count]=0;      //Nothing there....
        }
    }

    Left_Flag=0;
    Right_Flag=0;
}

///////////////////////////////////////////////////////////////////////////////

void Load_Background(int Speed) {

    int count, count2, temp;

    pcx_picture theBackground;

    PCX_Init((pcx_picture_ptr)&theBackground);

    //Will be switched to a switch statement, as more pictures are designed
    switch(Speed) {
            case 0:
                PCX_Load("concept.pcx", (pcx_picture_ptr)&theBackground,1);
                break;
            case 1:
                PCX_Load("concept2.pcx", (pcx_picture_ptr)&theBackground,1);
                break;
            case 2:
                PCX_Load("concept3.pcx", (pcx_picture_ptr)&theBackground,1);
                break;
            case 3:
                PCX_Load("concept4.pcx", (pcx_picture_ptr)&theBackground,1);
                break;
            case 4:
                PCX_Load("concept5.pcx", (pcx_picture_ptr)&theBackground,1);
                break;
            case 5:
                PCX_Load("concept6.pcx", (pcx_picture_ptr)&theBackground,1);
                break;
            case 6:
                PCX_Load("concept7.pcx", (pcx_picture_ptr)&theBackground,1);
                break;
            default:
                PCX_Load("concept5.pcx", (pcx_picture_ptr)&theBackground,1);
    }

    PCX_Show_Buffer((pcx_picture_ptr)&theBackground);

    PCX_Delete((pcx_picture_ptr)&theBackground);

    for(count=0;count<16;count++) {
        for(count2=0;count2<10;count2++) {

            temp=Game_Grid[count][count2];
            if(temp<=0) {
                Draw_Tetris_Block(count2,count,8);
                Game_Grid[count][count2]=temp;
            }
            else {
                Draw_Tetris_Block(count2,count,temp);
                Game_Grid[count][count2]=temp;
            }
        }
    }

}

///////////////////////////////////////////////////////////////////////////////

void Write_Text(void) {

    Blit_String(248,49,0,"NEXT:",1);
    Blit_String(247,48,15,"NEXT:",1);

    Blit_String(216,11,0,"SCORE:",1);
    Blit_String(215,10,15,"SCORE:",1);

    Blit_String(265,129,0,"LINES:",1);
    Blit_String(264,128,15,"LINES:",1);

    Blit_String(39,130,0,"LEVEL:",1);
    Blit_String(38,129,15,"LEVEL:",1);
}

///////////////////////////////////////////////////////////////////////////////

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
            Delay(6);
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
    };      //end case statement

}

///////////////////////////////////////////////////////////////////////////////

void Draw_Tetris_Block(int X, int Y, int color) {

    if(Y>=0) {
    Blocks.x = HOR_SEGMENT + (X*8);
    Blocks.y = VER_SEGMENT + (Y*8);
    Blocks.curr_frame = (color-1);
    Behind_Sprite((sprite_ptr)&Blocks);
    Draw_Sprite((sprite_ptr)&Blocks);
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
    Blocks.x = HOR_SEGMENT + (X*8);
    Blocks.y = VER_SEGMENT + (Y*8);
    Erase_Sprite((sprite_ptr)&Blocks);
    Game_Grid[Y][X] = 0;                //Get rid of old.......
    }


}

///////////////////////////////////////////////////////////////////////////////

void Erase_Tetris_Full_Brick(void) {

   int count;

   for(count=0;count<4;count++)
       Erase_Tetris_Block(Falling_Block.X_Pos[count], Falling_Block.Y_Pos[count]);

}

///////////////////////////////////////////////////////////////////////////////

void Game_Loop(void) {

    int counter;
    RGB_color theColor,color1;
    int count;

    while(Game_Over_Flag==0) {

    //Step 1:  Draw the Tetris Block....
    Draw_Tetris_Full_Brick();

    //Step 2:  Set the delay counter
    counter=13-Game_Speed;

    //Step 3:  Decrement the counter, poll keyboard every so often.....
    while(counter!=0) {

///////
Get_Palette_Register(21,(RGB_color_ptr)&color1);

        for(count=22;count<=35;count++) {

            Get_Palette_Register(count,(RGB_color_ptr)&theColor);
            Set_Palette_Register(count-1,(RGB_color_ptr)&theColor);
        }

        Set_Palette_Register(35,(RGB_color_ptr)&color1);
///////

        Keyboard_Poller();
        Delay(1);
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

///////////////////////////////////////////////////////////////////////////////

void Keyboard_Poller(void) {

    char theChar;
    int count,count2;

    theChar=Get_Scan_Code();

    if(theChar==0) {
        return;
    }

    if(theChar==SCAN_LEFT) {
        Left_Flag=1;
        for(count=0;count<4;count++) {
            if((Falling_Block.X_Pos[count]==0) || (Game_Grid[Falling_Block.Y_Pos[count]][Falling_Block.X_Pos[count]-1]>0))
                Left_Flag=0;
        }
    }

    if(theChar==SCAN_RIGHT) {
        Right_Flag=1;
        for(count=0;count<4;count++) {
            if((Falling_Block.X_Pos[count]==9) || (Game_Grid[Falling_Block.Y_Pos[count]][Falling_Block.X_Pos[count]+1]>0))
                Right_Flag=0;
        }
    }

    if(theChar==SCAN_UP) { //Just let the piece fall a little
       Drop_The_Piece(1);
       Counter_Flag=1;
    }

    if(theChar==SCAN_SPACE) {   //Let the piece go all the way
        Drop_The_Piece(0);
        Counter_Flag=1;
    }

    if(theChar==SCAN_ESC) {
        Game_Over_Flag=1;
        Counter_Flag=1;
    }

    if(theChar==SCAN_DOWN)
        Rotate();

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
}

///////////////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////////

void Do_Line_Cleanup(int line) {

       int count,count2,count4,temp;
       char buffer[16];
       RGB_color theColor,color1;

        //Clear the line in question


        for(count4=0;count4<4;count4++) {

            for(count=0;count<10;count++) {
                temp=Game_Grid[line][count];
                Draw_Tetris_Block(count,line, 8);
                Game_Grid[line][count]=temp;
            }

            for(count2=1;count2<=2;count2++) {
                ///////
                    Get_Palette_Register(21,(RGB_color_ptr)&color1);

                        for(count=22;count<=35;count++) {

                        Get_Palette_Register(count,(RGB_color_ptr)&theColor);
                        Set_Palette_Register(count-1,(RGB_color_ptr)&theColor);
                        }

                        Set_Palette_Register(35,(RGB_color_ptr)&color1);
                ///////
            Delay(1);

            }

            for(count=0;count<10;count++) {
                temp=Game_Grid[line][count];
                Draw_Tetris_Block(count, line, temp);
                Game_Grid[line][count]=temp;

            }

         for(count2=1;count2<=2;count2++) {
                ///////
                    Get_Palette_Register(21,(RGB_color_ptr)&color1);

                        for(count=22;count<=35;count++) {

                        Get_Palette_Register(count,(RGB_color_ptr)&theColor);
                        Set_Palette_Register(count-1,(RGB_color_ptr)&theColor);
                   }

                        Set_Palette_Register(35,(RGB_color_ptr)&color1);
                ///////
            Delay(1);

            }
        }
        for(count=0;count<10;count++) {
            Draw_Tetris_Block(count,line,8);
            Game_Grid[line][count]=0;
        }

        //Now, move everything down as much as necessary

        for(count=line-1;count>=0;count--) {

            for(count4=0;count4<10;count4++) {

                    if(Game_Grid[count][count4]>0) {
                    Draw_Tetris_Block(count4,count+1,8);
                    Draw_Tetris_Block(count4, count+1, Game_Grid[count][count4]);
                    }
                    if(Game_Grid[count][count4]==0)
                    Draw_Tetris_Block(count4, count+1, 8);

                    Game_Grid[count+1][count4]=Game_Grid[count][count4];



            }

        }


       for(count=0;count<10;count++)
                    Game_Grid[0][count]=0;


       lines++;

       Deal_With_Lines();

       //Better redraw the whole screen to make sure that everything's peachy..

       for(count=0;count<16;count++) {
            for(count4=0;count4<10;count4++) {

                temp=Game_Grid[count][count4];

                if(Game_Grid[count][count4]>0)
                    Draw_Tetris_Block(count4,count,Game_Grid[count][count4]);
                else
                    Draw_Tetris_Block(count4,count,8);

                 Game_Grid[count][count4]=temp;

            }

       }

       if((lines % 10==0) && (lines/10 > Game_Speed)) {

            if(Game_Speed<9) {
                Game_Speed++;
                Load_Background(Game_Speed);
                Write_Text();
                sprintf(buffer,"%d",Game_Speed);
                Blit_String(57,147,15,buffer,0);
            }

            Deal_With_Score(0);
            Deal_With_Lines();
            Draw_Next_Piece();
       }

}

///////////////////////////////////////////////////////////////////////////////

void Die(void) {

    int count,count2;
    char theChar=0;

    for(count=0;count<16;count++) {
        for(count2=0;count2<10;count2++) {
            Draw_Tetris_Block(count2,count,9);
        }
        Delay(1);
    }

    for(count=66;count<81;count++)
        H_Line(119,193,count,0);

    Blit_String(121,70,15,"GAME OVER",0);

    while((theChar=Get_Scan_Code())!=SCAN_ENTER) {}

    Do_Stats();
    High_Score_Stuff();

    Blit_String(49,173,0,"                          ",0);
    Blit_String(62,174,15,"Press ENTER to continue.",0);
    while((theChar=Get_Scan_Code())!=SCAN_ENTER) {}

    //Clear the center part of the screen....
    for(count=70;count<=130;count+=3) {
        H_Line(48,268,count+1,0);
        H_Line(48,268,count,0);
        H_Line(48,268,count+2,0);
    }

    H_Line(47,269,69,1);
    H_Line(47,269,131,1);
    H_Line(46,270,68,2);
    H_Line(46,270,132,2);
    H_Line(45,271,67,1);
    H_Line(45,271,133,1);
    V_Line(69,131,47,1);
    V_Line(69,131,269,1);
    V_Line(68,132,46,2);
    V_Line(68,132,270,2);
    V_Line(67,133,45,1);
    V_Line(67,133,271,1);

    Blit_String(62,75,15,"Are ya gonna play again?",0);
    Blit_String(62,100,15,"   Y - Yeah, I will!",0);
    Blit_String(62,109,15,"      N - No way!",0);


    while(1) {

    theChar=Get_Scan_Code();

        if(theChar==SCAN_Y) {
                Fade_Lights();
                height=0;
                Do_Options_Screen();
                Fade_Lights();
                Init_Sequence();
                break;
        }

        if((theChar==SCAN_N)) {

            Venetian_Blinds_File("credits.pcx");
            theChar=0;
            while(theChar==0)
                theChar=Get_Scan_Code();

            Fade_Lights();
            Set_Video_Mode(TEXT_MODE);
            exit(0);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////

void Deal_With_Score(int amount) {

        char buffer[16];

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

        sprintf(buffer,"%ld",score);
        Blit_String(score_left_pos,28,15,buffer,0);

}

///////////////////////////////////////////////////////////////////////////////

void Deal_With_Lines(void) {

     char buffer[16];
     int line_left_pos;

     if(lines<10)
            line_left_pos=290;
        if(lines>=10)
            line_left_pos=281;
        if(lines>=100)
            line_left_pos=272;
        if(lines>=1000)
            line_left_pos=263;

        sprintf(buffer,"%d",lines);
        Blit_String(line_left_pos,145,15,buffer,0);

}

///////////////////////////////////////////////////////////////////////////////

void Rotate(void) {

    int count;
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
                    };
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
                    };
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
                    };
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
                    };
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
                    };
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
                    };
                break;
            default:
                        break;
        };
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
                    };
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

                    };
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
                    };
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
                    };
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
                    };
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
                    };
                break;
            default:
                        break;
        };
        break;
};



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
            Draw_Tetris_Block(Temp_X[count],Temp_Y[count],8);
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

///////////////////////////////////////////////////////////////////////////////

void Drop_The_Piece(int flag) {

    int count,count2;
    int Finish_Flag=0;

    for(count=0;count<4;count++)  {
        Draw_Tetris_Block(Falling_Block.X_Pos[count],Falling_Block.Y_Pos[count],8);
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
    };

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
///////////////////////////////////////////////////////////////////////////////

void Fade_Lights(void)
{
// this functions fades the lights by slowly decreasing the color values
// in all color registers

int pal_reg,index;
RGB_color color;

for (index=0; index<30; index++)
    {

    for (pal_reg=1; pal_reg<255; pal_reg++)
        {
        // get the color to fade

        Get_Palette_Register(pal_reg,(RGB_color_ptr)&color);

        if (color.red   > 5) color.red-=3;
        else
           color.red = 0;

        if (color.green > 5) color.green-=3;
        else
           color.green = 0;
        if (color.blue  > 5) color.blue-=3;
        else
           color.blue = 0;

        // set the color to a diminished intensity

        Set_Palette_Register(pal_reg,(RGB_color_ptr)&color);

        } // end for pal_reg

    // wait a bit

    Delay(1);

    } // end fade for

} // end Fade_Lights

///////////////////////////////////////////////////////////////////////////////

void Do_Stats(void) {

    pcx_picture stats_pic;
    int total=0,count;
    char buffer[16],theChar=0;

    Venetian_Blinds_File("stats.pcx");

    sprintf(buffer,"%d",num_pieces[0]);
    Blit_String(31,64,15,buffer,1);
    sprintf(buffer,"%d",num_pieces[1]);
    Blit_String(69,64,15,buffer,1);
    sprintf(buffer,"%d",num_pieces[2]);
    Blit_String(108,64,15,buffer,1);
    sprintf(buffer,"%d",num_pieces[3]);
    Blit_String(150,64,15,buffer,1);
    sprintf(buffer,"%d",num_pieces[4]);
    Blit_String(197,64,15,buffer,1);
    sprintf(buffer,"%d",num_pieces[5]);
    Blit_String(237,64,15,buffer,1);
    sprintf(buffer,"%d",num_pieces[6]);
    Blit_String(282,64,15,buffer,1);

    for(count=0;count<7;count++)
        total+=num_pieces[count];

    sprintf(buffer,"%d",total);
    Blit_String(83,30,15,buffer,1);

    sprintf(buffer,"%d",lines);
    Blit_String(89,89,15,buffer,1);
    sprintf(buffer,"%d",singles);
    Blit_String(116,112,15,buffer,1);
    sprintf(buffer,"%d",doubles);
    Blit_String(268,111,15,buffer,1);
    sprintf(buffer,"%d",triples);
    Blit_String(116,154,15,buffer,1);
    sprintf(buffer,"%d",tetrises);
    Blit_String(268,152,15,buffer,1);

    theChar=0;
    while(theChar!=SCAN_ENTER) {
            theChar=Get_Scan_Code();
    }

    Fade_Lights();

}

///////////////////////////////////////////////////////////////////////////////

void Draw_Next_Piece(void) {

    unsigned int count;

    //First, let's clear that little space....
    for(count=62;count<93;count++) {
        H_Line(252,285,count,0);
    }

    //Done....now let's draw the new piece!
    for(count=0;count<4;count++) {
        Blocks.x = HOR_SEGMENT + ((14+Next_Block.X_Pos[count])*8);
        Blocks.y = VER_SEGMENT + ((4+Next_Block.Y_Pos[count])*8);
        Blocks.curr_frame = (Next_Block.color-1);
        Behind_Sprite((sprite_ptr)&Blocks);
        Draw_Sprite((sprite_ptr)&Blocks);
    }
}

///////////////////////////////////////////////////////////////////////////////

void Do_Options_Screen(void) {

    pcx_picture option_screen;
    int count;
    RGB_color theColor,color1;
    char theChar;

    PCX_Init((pcx_picture_ptr)&option_screen);
    PCX_Load("options.pcx",(pcx_picture_ptr)&option_screen,1);
    PCX_Show_Buffer((pcx_picture_ptr)&option_screen);
    PCX_Delete((pcx_picture_ptr)&option_screen);

    theChar=0;
    Game_Speed=0;

    while(theChar!=SCAN_ENTER) {

        theChar=Get_Scan_Code();

        Delay(2);

         ///////
           Get_Palette_Register(104,(RGB_color_ptr)&color1);

           for(count=105;count<=255;count++) {

           Get_Palette_Register(count,(RGB_color_ptr)&theColor);
           Set_Palette_Register(count-1,(RGB_color_ptr)&theColor);
           }

           Set_Palette_Register(255,(RGB_color_ptr)&color1);
        ///////

        Get_Palette_Register(18,(RGB_color_ptr)&color1);

        for(count=19;count<=21;count++) {

            Get_Palette_Register(count,(RGB_color_ptr)&theColor);
            Set_Palette_Register(count-1,(RGB_color_ptr)&theColor);
        }

        Set_Palette_Register(21,(RGB_color_ptr)&color1);

        switch(theChar) {

                case SCAN_0:
                            Draw_Game_Speed_Box(Game_Speed,0);
                            break;
                case SCAN_1:
                            Draw_Game_Speed_Box(Game_Speed,1);
                            break;
                case SCAN_2:
                            Draw_Game_Speed_Box(Game_Speed,2);
                            break;
                case SCAN_3:
                            Draw_Game_Speed_Box(Game_Speed,3);
                            break;
                case SCAN_4:
                            Draw_Game_Speed_Box(Game_Speed,4);
                            break;
                case SCAN_5:
                            Draw_Game_Speed_Box(Game_Speed,5);
                            break;
                case SCAN_6:
                            Draw_Game_Speed_Box(Game_Speed,6);
                            break;
                case SCAN_7:
                            Draw_Game_Speed_Box(Game_Speed,7);
                            break;
                case SCAN_8:
                            Draw_Game_Speed_Box(Game_Speed,8);
                            break;
                case SCAN_9:
                            Draw_Game_Speed_Box(Game_Speed,9);
                            break;
                case SCAN_LEFT:
                               if(height>0)
                                    Draw_Height_Box(height,height-1);
                            break;
                case SCAN_RIGHT:
                               if(height<5)
                                    Draw_Height_Box(height,height+1);
                            break;

                case SCAN_R:
                            Draw_Rotation_Box();
                default:
                            break;
        };
    };

}

///////////////////////////////////////////////////////////////////////////////

void Draw_Game_Speed_Box(int oldSpeed, int newSpeed) {

    int offset_x=56;
    int offset_y=56;
    int count;

    //first, erase the old box....

    H_Line(offset_x+(21*oldSpeed),offset_x+(21*oldSpeed)+19,56,0);
    H_Line(offset_x+(21*oldSpeed),offset_x+(21*oldSpeed)+19,75,0);
    V_Line(56,75,offset_x+(21*oldSpeed),0);
    V_Line(56,75,offset_x+(21*oldSpeed)+19,0);

    //Now, let's TRY to draw the new box....

   for(count=0;count<20;count+=4) {
        Plot_Pixel_Fast(offset_x+(21*newSpeed)+count,56,18);
        Plot_Pixel_Fast(offset_x+(21*newSpeed)+count+1,56,19);
        Plot_Pixel_Fast(offset_x+(21*newSpeed)+count+2,56,20);
        Plot_Pixel_Fast(offset_x+(21*newSpeed)+count+3,56,21);
        Plot_Pixel_Fast(offset_x+(21*newSpeed)+count,75,19);
        Plot_Pixel_Fast(offset_x+(21*newSpeed)+count+1,75,20);
        Plot_Pixel_Fast(offset_x+(21*newSpeed)+count+2,75,21);
        Plot_Pixel_Fast(offset_x+(21*newSpeed)+count+3,75,18);
   }

   for(count=57;count<70;count+=4) {

        Plot_Pixel_Fast(56+(21*newSpeed),count,21);
        Plot_Pixel_Fast(56+(21*newSpeed),count+1,20);
        Plot_Pixel_Fast(56+(21*newSpeed),count+2,19);
        Plot_Pixel_Fast(56+(21*newSpeed),count+3,18);
        Plot_Pixel_Fast(75+(21*newSpeed),count,18);
        Plot_Pixel_Fast(75+(21*newSpeed),count+1,19);
        Plot_Pixel_Fast(75+(21*newSpeed),count+2,20);
        Plot_Pixel_Fast(75+(21*newSpeed),count+3,21);
   }

   Plot_Pixel_Fast(75+(21*newSpeed),73,18);
   Plot_Pixel_Fast(75+(21*newSpeed),74,19);
   Plot_Pixel_Fast(56+(21*newSpeed),73,21);
   Plot_Pixel_Fast(56+(21*newSpeed),74,20);

   Game_Speed=newSpeed;
}

///////////////////////////////////////////////////////////////////////////////

void Draw_Height_Box(int oldHeight, int newHeight) {


        int offset_x_old;
        int offset_x_new;
        int count, count2, count3;

        offset_x_old=65+(38*oldHeight);
        offset_x_new=65+(38*newHeight);

        //First step is to erase the old box
        for(count=offset_x_old-3;count<=offset_x_old+3;count++) {
            V_Line(107,113,count,0);
        }

        //Done...now let's restore the slider bar...

        H_Line(65,255,110,15);

        if((oldHeight==0) || (oldHeight==5))
            V_Line(105,115,offset_x_old,15);
        else
            V_Line(108,112,offset_x_old,15);

        //Good.....now let's draw the new box...

        Plot_Pixel_Fast(offset_x_new,110,18);
        H_Line(offset_x_new-1,offset_x_new+1,109,19);
        H_Line(offset_x_new-1,offset_x_new+1,111,19);
        Plot_Pixel_Fast(offset_x_new-1,110,19);
        Plot_Pixel_Fast(offset_x_new+1,110,19);
        H_Line(offset_x_new-2,offset_x_new+2,108,20);
        H_Line(offset_x_new-2,offset_x_new+2,112,20);
        V_Line(109,111,offset_x_new-2,20);
        V_Line(109,111,offset_x_new+2,20);
        H_Line(offset_x_new-3,offset_x_new+3,107,21);
        H_Line(offset_x_new-3,offset_x_new+3,113,21);
        V_Line(108,112,offset_x_new-3,21);
        V_Line(108,112,offset_x_new+3,21);

        height=newHeight;



}

///////////////////////////////////////////////////////////////////////////////

void Draw_Rotation_Box(void) {

    int y_offset;

    //First thing's first....erase the old box...

    if(rotation==0)
        y_offset=142;
    if(rotation==1)
        y_offset=165;

    H_Line(213,311,y_offset,0);
    H_Line(213,311,y_offset+13,0);
    V_Line(y_offset,y_offset+13,213,0);
    V_Line(y_offset,y_offset+13,311,0);

    //Now, draw the new box...and assign the new rotation value....

    if(rotation==0) {
        rotation=1;
        y_offset=165;
    }
    else {
        rotation=0;
        y_offset=142;
    }

    H_Line(213,311,y_offset,18);
    H_Line(213,311,y_offset+13,20);
    V_Line(y_offset,y_offset+13,213,21);
    V_Line(y_offset,y_offset+13,311,19);
}

///////////////////////////////////////////////////////////////////////////////

void Venetian_Blinds_File(char *file) {

    pcx_picture pic;

    int count,count2,count3;


    for(count=31;count>=0;count-=2) {

        for(count2=0;count2<10;count2++) {

            for(count3=0;count3<200;count3++) {
                    Plot_Pixel_Fast((count2*32)+count,count3,0);
                    Plot_Pixel_Fast((count2*32)+count-1,count3,0);

            }
        }

    Delay(1);
    }

    PCX_Init((pcx_picture_ptr)&pic);
    PCX_Load(file,(pcx_picture_ptr)&pic,1);

    for(count=0;count<32;count+=2) {

        for(count2=0;count2<10;count2++) {

            for(count3=0;count3<200;count3++) {
                    Plot_Pixel_Fast((count2*32)+count,count3,pic.buffer[(count3*320)+(count2*32)+count]);
                    Plot_Pixel_Fast((count2*32)+count+1,count3,pic.buffer[(count3*320)+(count2*32)+count+1]);
            }
        }

    Delay(1);
    }

    PCX_Delete((pcx_picture_ptr)&pic);

}

///////////////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////////

void High_Score_Stuff(void) {

    int count,count2,count3;
    char Names[11][11], theChar,flag,flag2;
    long scores[10];
    int t_lines[10];
    pcx_picture High_Score_Screen;
    FILE *fp;
    char buffer[40];

    PCX_Init((pcx_picture_ptr)&High_Score_Screen);
    PCX_Load("highsc.pcx",(pcx_picture_ptr)&High_Score_Screen,1);
    PCX_Show_Buffer((pcx_picture_ptr)&High_Score_Screen);

    PCX_Delete((pcx_picture_ptr)&High_Score_Screen);

    //get the high score info from the file
    if((fp=fopen("concept9.con","r"))==NULL) {
        Set_Video_Mode(TEXT_MODE);
        printf("No high score info exists.....press any key");
        while(!kbhit()) {}
        Set_Video_Mode(VGA256);
        return;
    }

    for(count=0;count<9;count++) {

        fscanf(fp, "%s %ld %d", &Names[count],&scores[count],&t_lines[count]);

        sprintf(buffer,"%d. %s %d", count+1, Names[count], t_lines[count]);
        Blit_String(67,50+(10*count),15,buffer,1);
        sprintf(buffer,"%ld",scores[count]);
        Blit_String(219,50+(10*count),15,buffer,1);
    }

        count=9;
        fscanf(fp, "%s %ld %d", &Names[count],&scores[count],&t_lines[count]);
        sprintf(buffer,"%d. %s %d", count+1, Names[count], t_lines[count]);
        Blit_String(59,50+(10*count),15,buffer,1);
        sprintf(buffer,"%ld",scores[count]);
        Blit_String(219,50+(10*count),15,buffer,1);

    fclose(fp);

    //Now, check to see if a new high score has been attained.......
    for(count=0;count<=9;count++) {
        if(score>=scores[count]) {

    for(count2=8;count2>=count;count2--) {
      sprintf(buffer,"%d. %s %d", count2+2, Names[count2], t_lines[count2]);
        if((count2+1)==9)
            Blit_String(59,50+(10*(count2+1)),15,buffer,0);
        else
            Blit_String(67,50+(10*(count2+1)),15,buffer,0);
      sprintf(buffer,"%ld",scores[count2]);
      Blit_String(219,50+(10*(count2+1)),15,buffer,0);

    }


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
        Set_Video_Mode(TEXT_MODE);
        printf("High score info cannot be written.....press any key");
        while(!kbhit()) {}
        Set_Video_Mode(VGA256);
        return;
    }

     for(count=0;count<=9;count++) {

        fprintf(fp, "%c%c%c%c%c%c%c%c%c%c\n%ld\n%d\n", Names[count][0],Names[count][1],Names[count][2],Names[count][3],Names[count][4],Names[count][5],Names[count][6],Names[count][7],Names[count][8],Names[count][9],scores[count],t_lines[count]);

    }

    fclose(fp);

    //while((theChar=Get_Scan_Code())!=SCAN_ESC) {}
}


///////////////////////////////////////////////////////////////////////////////

void Update_High_Score(int position) {

    FILE *fp;
    char buffer[27];
    char theChar;
    int counter,count;

    //Find the position where the score is, highlight that position,
    //and clear it!!!

    if(position==9) {
        Blit_String(59,50+(10*position),1,"10.                       ",0);
    }
    else {
        sprintf(buffer,"%d.                       ",position+1);
        Blit_String(67,50+(10*position),1,buffer,0);
    }


    //First, inform the user they have a new high score

    Blit_String(49,174,15,"You have a new high score!!!",1);
    Delay(18);

    Blit_String(49,174,0,"You have a new high score!!!",1);
    Blit_String(49,174,15,"Name: ",1);

    //Start the counter at 0
    counter=0;
    theChar=0;

    //perform the parsing until the enter button is pressed....

    while(theChar!=13) {        //the carriage return (ENTER)

        theChar= Get_Ascii_Key();

        if((theChar==8) && (counter>0)) {        //backspace
            counter--;
            Blit_String(110+(8*counter),173,0," ",0);
            name[counter]=' ';
        }

        if((theChar>=33) && (theChar<=122) && (counter<8)) {  //it's a key...draw it
            Blit_Char(110+(8*counter),173,theChar,15,1);
            name[counter]=theChar;
            counter++;
            }

        //otherwise, do nada....
   };


        //now, let's terminate the string proper....

        for(count=counter;count<10;count++)
            name[count]='.';
            name[11]='\0';

        //now, blit the new string to the screen to show off the score...
        sprintf(buffer,"%s %d", name, lines);
        Blit_String(91,50+(10*position),1,buffer,0);
        sprintf(buffer,"%ld",score);
        Blit_String(219,50+(10*position),1,buffer,0);


        Delay(36);

}

//////////////////////////////////////////////////////////////////////////////

void clear_kb(void) {

    char junk[80];

    gets(junk);
}

///////////////////////////////////////////////////////////////////////////////
void main(void) {

    int count,break_flag=0;
    pcx_picture block_pic;
    pcx_picture title_screens;
    long counter;
    char buffer[16];
    char theChar;
    char junk[80];

    Set_Video_Mode(VGA256);

    srand(time(NULL));

    //To be moved to the Title Screen function (when I make it...)
    PCX_Init((pcx_picture_ptr)&title_screens);
    PCX_Load("company.pcx",(pcx_picture_ptr)&title_screens,1);
    PCX_Show_Buffer((pcx_picture_ptr)&title_screens);
    Blit_String(16,181,250,"1997 -- Seamonkey (aka Shaun Brandt)",1);
    Blit_String(15,180,228,"1997 -- Seamonkey (aka Shaun Brandt)",1);
    PCX_Delete((pcx_picture_ptr)&title_screens);

    while(!kbhit()) {}

    Fade_Lights();

    for(counter=0;counter<64000;counter++)
        video_buffer[counter]=254;

    theChar=0;
    clear_kb();

//start a big loop thingy where you get to see the high scores and stuff...
    PCX_Init((pcx_picture_ptr)&title_screens);
    PCX_Load("title.pcx",(pcx_picture_ptr)&title_screens,1);
    PCX_Show_Buffer((pcx_picture_ptr)&title_screens);
    PCX_Delete((pcx_picture_ptr)&title_screens);

while(1) {

     for(count=0;count<144;count++) {
        theChar=Get_Scan_Code();
        if(theChar==SCAN_ENTER)
            break_flag=1;

        if(break_flag==1)
            break;

        Delay(1);
     }

    if(break_flag==1)
        break;

    Fade_Lights();

    for(counter=0;counter<64000;counter++)
        video_buffer[counter]=0;

    High_Score_Stuff();

    for(count=0;count<144;count++) {
        theChar=Get_Scan_Code();
        if(theChar==SCAN_ENTER)
            break_flag=1;

        if(break_flag==1)
            break;

        Delay(1);
    }

    if(break_flag==1)
        break;

    Venetian_Blinds_File("title.pcx");

}

    Fade_Lights();


    Do_Options_Screen();

    Fade_Lights();

    for(counter=0;counter<64000;counter++)
        video_buffer[counter]=0;


    //To be moved to an init section
    sprite_width =8;
    sprite_height =8;

    PCX_Init((pcx_picture_ptr)&block_pic);
    PCX_Load("blocks.pcx",(pcx_picture_ptr)&block_pic,1);

    Sprite_Init((sprite_ptr)&Blocks,0,0,0,0,0,0);
    for(count=0;count<=8;count++) {
        PCX_Grab_Bitmap((pcx_picture_ptr)&block_pic,(sprite_ptr)&Blocks,
            count, count, 0);

    }

    PCX_Delete((pcx_picture_ptr)&block_pic);


    Init_Sequence();

    Set_Video_Mode(TEXT_MODE);


}