#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <time.h>

#define LINE    "-----------------------\n"

#define BLOCK   219
#define AIR     '.'//32

#define HEIGHT  30
#define WIDTH   10

char * uiToStr(unsigned int);      //unsigned int to string
char * arrToStr(char **c3dArray, unsigned int uiWidth, unsigned int uiHeight);  //convert 3d char array to string
void insert(char **cGoal, char *cMessage, unsigned int uiPos);   //insert a string into string starting at certain position
void write(char **cGoal, char *cMessage);    //writes a string to a string
void clearScr(char ** cGoal);                //replaces all chars of a string with \b
void clearArr(char ***c3dArray, unsigned int uiWidth, unsigned int uiHeight);    //clears 3d array

enum eStatus {
    ePlaying, eWin, eLose, eTerminate, eRot0, eRot90, eRot180, eRot270
};

enum eAction {
    eStandby, eMoveLeft, eMoveRight, eTurnLeft, eTurnRight, eHop, eFall, eNewTetr
};

enum eTetrominos {
    TETR_I, TETR_O, TETR_J, TETR_L, TETR_S, TETR_Z, TETR_T
};

struct sDimension {
    unsigned int uiWidth;
    unsigned int uiHeight;
    unsigned int uiPosX;
    unsigned int uiPosY;
};

struct sBoard {
    char ** cField;
};

int main()
{
    char * cMessage = 0;
    int iButton = 0;

    do
    {
        cMessage = 0;
        write(&cMessage, "Console Tetris by Raphael\n");
        write(&cMessage, LINE);
        write(&cMessage, "These Keys do the following:\n");
        write(&cMessage, "(1) New Game\n");
        write(&cMessage, "(2) Settings\n");
        write(&cMessage, "(0) Terminator\n");
        write(&cMessage, LINE);

        system("CLS");
        printf("%s", cMessage);

        iButton = _getch();

        switch(iButton)
        {
            case '1':
                //initialize
                srand((int)time(NULL)); //initialize random number generator
                clock_t cClock;         //initialize clock

                cMessage = 0;

                //initializing the Tetrominos
                char *** cTetromino;
                char cTetrominoSize[7] = {14,22,23,23,23,23,23};    //I,O,J,L,S,Z,T (Tens = Height, Ones = Width)

                cTetromino = (char***)malloc(sizeof(char**) * 7);
                for(int i = 0; i < 7; i++)
                {
                    cTetromino[i] = (char**)malloc(sizeof(char*) * (cTetrominoSize[i] / 10));
                    for(int j = 0; j < cTetrominoSize[i] / 10; j++)
                    {
                        cTetromino[i][j] = (char*)malloc(sizeof(char) * (cTetrominoSize[i] % 10));
                        for(int k = 0; k < cTetrominoSize[i] % 10; k++)
                        {
                            cTetromino[i][j][k] = AIR;
                        }
                    }
                }
                //I Tetromino
                cTetromino[TETR_I][0][0] = BLOCK;
                cTetromino[TETR_I][0][1] = BLOCK;
                cTetromino[TETR_I][0][2] = BLOCK;
                cTetromino[TETR_I][0][3] = BLOCK;
                //O Tetromino
                cTetromino[TETR_O][0][0] = BLOCK;
                cTetromino[TETR_O][0][1] = BLOCK;
                cTetromino[TETR_O][1][0] = BLOCK;
                cTetromino[TETR_O][1][1] = BLOCK;
                //J Tetromino
                cTetromino[TETR_J][0][0] = BLOCK;
                cTetromino[TETR_J][1][0] = BLOCK;
                cTetromino[TETR_J][1][1] = BLOCK;
                cTetromino[TETR_J][1][2] = BLOCK;
                //L Tetromino
                cTetromino[TETR_L][0][0] = BLOCK;
                cTetromino[TETR_L][1][0] = BLOCK;
                cTetromino[TETR_L][1][1] = BLOCK;
                cTetromino[TETR_L][1][2] = BLOCK;
                //S Tetromino
                cTetromino[TETR_S][0][1] = BLOCK;
                cTetromino[TETR_S][0][2] = BLOCK;
                cTetromino[TETR_S][1][0] = BLOCK;
                cTetromino[TETR_S][1][1] = BLOCK;
                //Z Tetromino
                cTetromino[TETR_Z][0][0] = BLOCK;
                cTetromino[TETR_Z][0][1] = BLOCK;
                cTetromino[TETR_Z][1][1] = BLOCK;
                cTetromino[TETR_Z][1][2] = BLOCK;
                //T Tetromino
                cTetromino[TETR_T][0][1] = BLOCK;
                cTetromino[TETR_T][1][0] = BLOCK;
                cTetromino[TETR_T][1][1] = BLOCK;
                cTetromino[TETR_T][1][2] = BLOCK;

                //initialize board
                struct sDimension sDimensions;
                sDimensions.uiHeight = HEIGHT;
                sDimensions.uiWidth = WIDTH * 2;
                sDimensions.uiPosX = 0;
                sDimensions.uiPosY = 0;
                struct sBoard sBackground, sForeground, sVisible;
                sBackground.cField = (char**)malloc(sizeof(char*) * sDimensions.uiHeight);  //already placed Tetrominos
                for(int i = 0; i < sDimensions.uiHeight; i++)
                {
                    sBackground.cField[i] = (char*)malloc(sizeof(char) * sDimensions.uiWidth);
                    for(int j = 0; j < sDimensions.uiWidth; j++)
                    {
                        sBackground.cField[i][j] = AIR;
                    }
                }
                sForeground.cField = (char**)malloc(sizeof(char*) * sDimensions.uiHeight);  //falling Tetromino
                for(int i = 0; i < sDimensions.uiHeight; i++)
                {
                    sForeground.cField[i] = (char*)malloc(sizeof(char) * sDimensions.uiWidth);
                    for(int j = 0; j < sDimensions.uiWidth; j++)
                    {
                        sForeground.cField[i][j] = AIR;
                    }
                }
                sVisible.cField = (char**)malloc(sizeof(char*) * sDimensions.uiHeight);     //Background and Foreground combined
                for(int i = 0; i < sDimensions.uiHeight; i++)
                {
                    sVisible.cField[i] = (char*)malloc(sizeof(char) * sDimensions.uiWidth);
                    for(int j = 0; j < sDimensions.uiWidth; j++)
                    {
                        sVisible.cField[i][j] = AIR;
                    }
                }

                //initialize other variables
                char cPrint = 1;        //print (1) or not (0)
                char cStatus = ePlaying;    //game status
                char cAction = eNewTetr;    //system action
                char cUserAction = eStandby;    //user action
                char cTetrCurr = rand() % 7;    //Random Tetromino
                char cRotation = eRot0;         //rotation 0°

                int iTime = 1000;       //1000 ms
                int iTimeOld = clock(); //starting time

                //game start
                do
                {
                    if(kbhit()) //get keyboard hit
                    {
                        iButton = _getch();
                        //get action
                        switch(iButton)
                        {
                            case 224:
                                iButton = _getch();
                                switch(iButton)
                                {
                                    case 75: //224.75 = arrow key left
                                    {
                                        cUserAction = eMoveLeft;
                                        break;
                                    }
                                    case 77: //224.77 = arrow key right
                                    {
                                        cUserAction = eMoveRight;
                                        break;
                                    }
                                    case 72: //224.72 = arrow key up
                                    {
                                        cUserAction = eTurnRight;
                                        break;
                                    }
                                    case 80: //224.80 = arrow key down
                                    {
                                        iTimeOld = clock();
                                        cUserAction = eHop;
                                        break;
                                    }
                                    default:
                                        break;
                                }
                                break;
                            default:
                                break;
                        }
                        //take action
                        switch(cUserAction)
                        {
                            case eMoveLeft:
                                if(sDimensions.uiPosX > 0)  //inbounds?
                                {
                                    sDimensions.uiPosX -= 2;
                                    cPrint = 1;
                                }
                                break;
                            case eMoveRight:
                                if(sDimensions.uiPosX < sDimensions.uiWidth - 2 * (cTetrominoSize[cTetrCurr] % 10)) //inbounds?
                                {
                                    sDimensions.uiPosX += 2;
                                    cPrint = 1;
                                }
                                break;
                            case eTurnLeft:
                                break;
                            case eTurnRight:
                                cRotation++;
                                if(cRotation > eRot270)
                                {
                                    cRotation = eRot0;
                                }
                                // rotate Tetromino
                                for(int i = 0; i < 7; i++)  // for each Tetromino
                                {
                                    char * cTetrominoCopy = (char*)malloc(sizeof(cTetromino));
                                    for(int y = 0; y < cTetrominoSize[i] / 10; y++)
                                    {
                                        for(int x = 0; x < cTetrominoSize[i] % 10; x++)
                                        {
                                            cTetrominoCopy = cTetromino[i][y][x];
                                        }
                                    }
                                }
                            case eHop:
                                if(sDimensions.uiPosY < sDimensions.uiHeight - cTetrominoSize[cTetrCurr] / 10)    //inbounds?
                                {
                                    iTimeOld = clock();
                                    sDimensions.uiPosY++;
                                    cPrint = 1;
                                }
                                else
                                {
                                    cPrint = 1;
                                    cAction = eNewTetr;
                                }
                                break;
                            default:
                                break;
                        }
                        cUserAction = eStandby;
                    }


                    //refreshing via timer
                    if(clock() - iTimeOld >= iTime)
                    {
                        iTimeOld = clock();
                        cAction = eHop;
                        cPrint = 1;
                    }
                    //printing
                    if(cPrint == 1)
                    {
                        if(cAction == eHop)      //hop Tetromino
                        {
                            if(sDimensions.uiPosY < sDimensions.uiHeight - cTetrominoSize[cTetrCurr] / 10)    //inbounds?
                            {
                                sDimensions.uiPosY++;
                                cAction = eStandby;
                            }
                            else
                            {
                                cAction = eNewTetr;
                            }
                        }
                        if(cAction == eNewTetr)
                        {
                            sDimensions.uiPosX = sDimensions.uiWidth / 2 - 4;
                            sDimensions.uiPosY = 0;
                            cTetrCurr = rand() % 7; //Random Tetromino
                        }
                        cAction = eStandby;



                        //recalculate foreground (falling Tetromino)
                        clearArr(&sForeground.cField, sDimensions.uiWidth, sDimensions.uiHeight);
                        for(int i = 0; i < cTetrominoSize[cTetrCurr] / 10; i++)
                        {
                            for(int j = 0; j < cTetrominoSize[cTetrCurr] % 10; j++)
                            {
                                sForeground.cField[i + sDimensions.uiPosY][j * 2 + sDimensions.uiPosX] = cTetromino[cTetrCurr][i][j];
                                sForeground.cField[i + sDimensions.uiPosY][j * 2 + sDimensions.uiPosX + 1] = cTetromino[cTetrCurr][i][j];
                            }
                        }


                        cMessage = arrToStr(sForeground.cField, sDimensions.uiWidth, sDimensions.uiHeight);

                        //recalculate background
                        //combine fore- and background

                        //print combined
                        cPrint = 0;
                        system("CLS");
                        printf("%s", cMessage);

                    }
                }
                while(iButton != 0 || cStatus != ePlaying);
                break;
            case '2':
                break;
            case '0':
                cMessage = 0;
                write(&cMessage, "Confirm Termination with '0'\n");
                printf("%s", cMessage);
                iButton = _getch();
                break;
        }

        //clearScr(&cMessage);
    }
    while(iButton != '0');
    return 0;
}

void insert(char **cGoal, char *cMessage, unsigned int uiPos)
{
    unsigned int i = 0;
    unsigned int k = 0;
    unsigned int j = 0;
    if(*cGoal != 0) //search for end of goal array
    {
        while((*cGoal)[i] != '\0')
        {
            i++;
        }
    }
    if(cMessage != 0) //search for end of string array
    {
        while(cMessage[k] != '\0')
        {
            k++;
        }
    }
    *cGoal = (char*)realloc(*cGoal, sizeof(char) * (k + i + 1));
    j = i + k;
    while(j >= k)   //move goal back
    {
        (*cGoal)[j] = (*cGoal)[j - k];
        j--;
    }
    j = 0;
    while(j < k)    //insert string
    {
        (*cGoal)[j] = cMessage[j];
        j++;
    }
    // ( terminator already added )
}

void write(char **cGoal, char *cMessage)
{
    unsigned int i = 0;
    unsigned int k = 0;
    if(*cGoal != 0) //search for end of array
    {
        while((*cGoal)[i] != '\0')
        {
            i++;
        }
    }
    while(cMessage[k] != '\0')   //write to the array
    {
        *cGoal = (char*)realloc(*cGoal, sizeof(char) * (i + 1));
        (*cGoal)[i] = cMessage[k];
        k++;
        i++;
    }
    *cGoal = (char*)realloc(*cGoal, sizeof(char) * (i + 1));
    (*cGoal)[i] = '\0';
}

char * uiToStr(unsigned int uiNumber)
{
    int i10 = 1;
    char * cMessage = 0;
    unsigned int uiLenString = 0;
    while(uiNumber / i10 > 9)
    {
        i10 *= 10;
    }
    while(i10 > 0)
    {
        cMessage = (char*)realloc(cMessage, sizeof(char) * (uiLenString + 1));
        cMessage[uiLenString] = uiNumber / i10 + 48;
        uiLenString++;
        uiNumber -= (uiNumber / i10) * i10;
        i10 /= 10;
    }
    cMessage = (char*)realloc(cMessage, sizeof(char) * (uiLenString + 1));
    cMessage[uiLenString] = '\0';
    return cMessage;
}

char * arrToStr(char **c3dArray, unsigned int uiWidth, unsigned int uiHeight)
{
    char * cFinal = 0;
    cFinal = (char*)malloc(sizeof(char) * (uiWidth + 1) * uiHeight + 1);

    for(int h = 0; h < uiHeight; h++)
    {
        for(int w = 0; w < uiWidth; w++)
        {
            cFinal[h * (uiWidth + 1) + w] = c3dArray[h][w];
        }
        cFinal[h * (uiWidth + 1) + uiWidth] = '\n';
    }
    cFinal[uiHeight * (uiWidth + 1)] = '\0';
    return cFinal;
}

void clearScr(char **cGoal)
{
    unsigned int i = 0;
    unsigned int k = 0;
    if(*cGoal != 0) //search for end of array
    {
        while((*cGoal)[i] != '\0')
        {
            i++;
        }
    }
    *cGoal = 0;
    for(k = 0; k < i; k++)
    {
        write(&(*cGoal), "\b");
    }
}

void clearArr(char ***c3dArray, unsigned int uiWidth, unsigned int uiHeight)
{
    for(int i = 0; i < uiHeight; i++)
    {
        for(int j = 0; j < uiWidth; j++)
        {
            (*c3dArray)[i][j] = AIR;
        }
    }
}









