/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.c
 * Author: TheSayian
 *
 * Created on December 3, 2017, 1:51 PM
 */


/*
 * 
 * This program was created by:
 *  Connor Fitzmaurice
 *  Alexander Flynn
 *  Katherine Mawhinney
 *  William Smith
 *  
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include "input_error.h"

char arrayBad[5] = {'#', '\0', '\n', 'S', 'F'};
char goodMapChar[8] = {'#', '\0', '\n', 'S', 'E', ' ', 'F', 'L'};

int horSize(FILE*);
int verSize(FILE*);
int col;
int row;
int findRowNumber(char startx, char maze[row][col]);
int findColumnNumber(char starty, char maze[row][col]);
void OpenInFile(FILE **ptr, char *FileName);
void CloseInFile(FILE *ptr);
void findExit(char maze[row][col], char start, char end, char other);
void buildMap(char house[row][col], FILE *InFile);
bool isBad(char maze[row][col], int cr, int cc);
bool isNear(char maze[row][col], int cr, int cc, char other);

int main(int argc, char** argv) {
    
    if( argc < 2 )
    {
        exit(INCORRECT_NUMBER_OF_COMMAND_LINE_ARGUMENTS);
    }
    
    row = 0;
    col = 0;

    FILE *InFile = NULL;
    OpenInFile(&InFile, argv[1]);

    //find size of map
    col = horSize(InFile);
    row = verSize(InFile);
    
    //Map will be a 2D char array named 'house'
    char house[row][col];
    
    buildMap(house,InFile);

    //Find a path from S to E avoiding F on the way
    findExit(house, 'S', 'E', 'F');
    
    //Clear map of S, E, and S's path
    int i, j;
    for( i = 0; i < row; i++ ){
        for( j = 0; j < col; j++ ){
            if( house[i][j] == '.' || house[i][j] == 'S' || house[i][j] == '*')
                house[i][j] = ' ';
        }
    }
    
    //Find a path from F to L
    findExit(house, 'F', 'L', 'S');

    CloseInFile(InFile);
    return (EXIT_SUCCESS);
}

// Given a 2D char array, find the row number for the given character 'x'
int findRowNumber(char x, char maze[row][col]) {
    int i = 0;
    int j = 0;
    int rowNum = -1;
    for (i = 0; i < row; i++) {
        for (j = 0; j < col; j++) {
            if (maze[i][j] == x) {
                if( rowNum != -1)       //the character has already appeared in the map
                {
                    exit(INPUT_FILE_NOT_VALID_MAP);
                }
                rowNum= i;
            }
        }
    }
    if( rowNum != -1)
    {
        return(rowNum);
    }
    exit(INPUT_FILE_NOT_VALID_MAP);
}

// Given a 2D char array, find the column number for the given character 'y'
int findColumnNumber(char y, char maze[row][col]) {
    int i = 0;
    int j = 0;
    int column = -1;
    for (i = 0; i < row; i++) {
        for (j = 0; j < col; j++) {
            if (maze[i][j] == y) {
                if( column != -1)       //the character has already appeared in the map
                {
                    exit(INPUT_FILE_NOT_VALID_MAP);
                }
                column = j;
            }
        }
    }
    if( column != -1)
    {
        return(column);
    }
    exit(INPUT_FILE_NOT_VALID_MAP);
}

//returns length of longest line in text file
int horSize(FILE* fp) {
    int max, temp, count;

    count = 0;
    max = 0;

    while (temp != EOF) {
        temp = fgetc(fp);

        if (temp == '\n') {
            if (max < count)
                max = count;

            count = 0;
        } else
            count++;
    }

    rewind(fp);

    return max;
}

// Returns number of lines in text file
int verSize(FILE* fp) {
    int temp, count;

    count = 1;

    while (temp != EOF) {
        temp = fgetc(fp);

        if (temp == '\n')
            count++;

    }

    rewind(fp);

    return count;
}

// Opens file
void OpenInFile(FILE **InPtr, char *argv) {
    *InPtr = fopen(argv, "r");
    if (*InPtr == NULL) {
        exit(INPUT_FILE_FAILED_TO_OPEN);
    }
}

// Closes file
void CloseInFile(FILE *InPtr) {
    if (fclose(InPtr) != 0) {
        exit(INPUT_FILE_FAILED_TO_CLOSE);
    }
}

// Print maze
void print_grid( char array[row][col],int row,int column){

    int i;
    int j;
    for (i=0;i<row;i++) {
        for (j=0;j<column;j++) {
            printf("%c", array[i][j]);
        }
        printf("\n");
    }
    
}

// This function checks to see if any of the surrounding characters of a certain point are 'bad', meaning you cant go there.
bool isBad(char maze[row][col], int cr, int cc) {
    int i = 0;
    for (i = 0; i < 5; i++) {
       // printf("\nMazeVal: %c, row: %d, col; %d", maze[cr][cc], cr, cc);
        if (maze[cr][cc] == arrayBad[i]) {
            return (true);
        }
    }
    return (false);
}

// Sets to true if on or adjacent to other robot, else false
bool isNear(char maze[row][col], int cr, int cc, char other){
    if(maze[cr][cc] == other){return (true);}
    if(maze[cr][cc + 1] == other && maze[cr][cc] != '#'){return (true);}
    if(maze[cr + 1][cc] == other && maze[cr][cc] != '#'){return (true);}
    if(maze[cr + 1][cc + 1] == other && maze[cc][col] != '#'){return (true);}
    if(maze[cr][cc - 1] == other && maze[cr][cc] != '#'){return (true);}
    if(maze[cr - 1][cc] == other && maze[cr][cc] != '#'){return (true);}
    if(maze[cr - 1][cc - 1] == other){return (true);}
    
    return (false);
}

// Starting at given point in 2D char array, moves until it finds 'end' char 
// while avoiding becoming adjacent to 'other' char
// Marks path with '...' and exit point with "*"
// Prints map before and after traversal to standard out
void findExit(char maze[row][col], char start, char end, char other) {

    //cr=current row, cc=current col, or=old row, oc = old col
    int c, r, cr = 0, cc = 0, or = 0, oc = 0, numRuns = 0; 
    int done = 0, nextDir = 0; 
    int dir = 0 /*, move = 0 */; //directions: north=0,east=1,south=2,west=3;
    
    //Find starting point
    cr = findRowNumber(start, maze);
    cc = findColumnNumber(start, maze);
    
    //initializes direction for movement
    if (maze[cr][cc + 1] == ' ') {
        dir = 0; //not dir=1!
    } else if (maze[cr + 1][cc] == ' ') {
        dir = 1;
    } else if (maze[cr][cc - 1] == ' ') {
        dir = 2;
    } else if (maze[cr - 1][cc] == ' ' ) {
        dir = 3;
    }
    
    //Display starting map
    if(start == 'S')
    {
        printf("Initial maze:\n");
        print_grid( maze, row, col );
    }
    else
    {
        printf("Maze after S has exited at E:\n");
        print_grid( maze, row, col );
    }
    
    //main solving loop
    while (!done) {
        
        or = cr;
        oc = cc;
        
        if(dir==0) { //North
              
            if (!isBad(maze,cr,cc+1) && !isNear(maze,cr,cc+1, other)) {
                nextDir = 1;
                cc++;
            } else if (!isBad(maze,cr-1,cc) && !isNear(maze,cr-1, cc , other)) {
                nextDir = 0;
                cr--;
            } else if (!isBad(maze,cr,cc-1 ) && !isNear(maze,cr,cc-1 , other)) {
                nextDir = 3;
                cc--;
            } else if (!isBad(maze,cr+1, cc ) && !isNear(maze,cr+1 ,cc , other)) {
                nextDir = 2;
                cr++;
            }
        }
        if (dir == 1) { //East
            if (!isBad(maze,cr+1, cc ) && !isNear(maze,cr+1, cc , other)) {
                nextDir = 2;
                cr++;
            } else if (!isBad(maze,cr ,cc + 1 ) && !isNear(maze,cr ,cc + 1 , other)) {
                nextDir = 1;
                cc++;
            } else if (!isBad(maze,cr-1, cc ) && !isNear(maze,cr-1, cc, other)) {
                nextDir = 0;
                cr--;
            } else if (!isBad(maze,cr ,cc - 1 ) && !isNear(maze,cr ,cc - 1 , other)) {
                nextDir = 3;
                cc--;
            }
        }
        if (dir == 2) { //South
            if (!isBad(maze,cr ,cc - 1 ) && !isNear(maze,cr ,cc - 1 , other)) {
                nextDir = 3;
                cc--;
            } else if (!isBad(maze,cr+1, cc ) && !isNear(maze,cr+1 ,cc , other)) {
                nextDir = 2;
                cr++;
            } else if (!isBad(maze,cr ,cc + 1 ) && !isNear(maze,cr, cc + 1 , other)) {
                nextDir = 1;
                cc++;
            } else if (!isBad(maze,cr-1, cc ) && !isNear(maze,cr-1 ,cc , other)) {
                nextDir = 0;
                cr--;
            }
        }
        if (dir == 3) { //West
            if (!isBad(maze,cr-1 ,cc ) && !isNear(maze,cr-1 ,cc , other)) {
                nextDir = 0;
                cr--;
            } else if (!isBad(maze,cr, cc - 1 ) && !isNear(maze,cr ,cc - 1 , other)) {
                nextDir = 3;
                cc--;
            } else if (!isBad(maze,cr+1, cc ) && !isNear(maze,cr+1, cc , other)) {
                nextDir = 2;
                cr++;
            } else if (!isBad(maze,cr, cc + 1 ) && !isNear(maze,cr ,cc + 1 , other)) {
                nextDir = 1;
                cc++;
            }
        }
        
        //When you get to the exit mark with asterick
        if (maze[cr][cc] == end){ 
            done = 1;
            maze[cr][cc] = '*';
        }
        else //This just makes sure the tail isn't created at the same time as the initial thing, which causes an infinite loop
        {   
            if( numRuns != 0 ){ 
                maze[or][oc] = '.'; 
                //maze[cr][cc] = '*';
            }

            numRuns++;
            if( numRuns > (row*row*col*col) )
            {
                print_grid( maze, row, col );
                printf("No solution");
                exit(NO_SOLUTION);
            }           
        }
     
        dir = nextDir; //dir value calculated above, is assigned
        int lastmovex = -1;
        int lastmovey = -1;
        if(lastmovex == cr && lastmovey == cc){
            exit(NO_SOLUTION);
        }
        lastmovex = cr;
        lastmovey = cc;
    }
    
    //Display solution
    if(start == 'S')
    {
        printf("The path from S to E:\n");
    }
    else
    {
        printf("The path from F to L:\n");
    }
    print_grid( maze, row, col );
}


// Takes an X x Y char array and input text file
// Populates array with chars from file to create map
void buildMap(char house[row][col], FILE *InFile){
    int temp = 0;
    int x = 0,y = 0, i = 0;
    
    for (x = 0; x < row; x++) {
        for (y = 0; y < col; y++) {
            house[x][y] = ' ';
        }
    }

    x = 0;
    y = 0;

    while( temp != EOF ){
        temp = fgetc( InFile );
        if(temp == EOF){break;}
                
        //brute force error check for invalid characters
        if( temp == goodMapChar[0] || temp == goodMapChar[2] || 
            temp == goodMapChar[3] || temp == goodMapChar[4] ||
            temp == goodMapChar[5] || temp == goodMapChar[6] || 
            temp == goodMapChar[7] )
        {}
        else
        {
            exit(INPUT_FILE_NOT_VALID_MAP);
        }

        //At newline increment row and reset column
        if( temp == '\n' )
        {
            y = 0;
            x++;
        }
        else
        {
            house[x][y] = (char)temp;
            y++;
        }
    }  
}
