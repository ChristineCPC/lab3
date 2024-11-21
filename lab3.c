#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include "lab3.h"

extern int** sudoku_board;
int* worker_validation;

int** read_board_from_file(char* filename){
    FILE *fp = NULL;
    int** board = NULL;

    // replace this comment with your code
    fp = fopen(filename, "r");

    //allocate board
    board = (int**)malloc(sizeof(int*)*ROW_SIZE);
    for(int row = 0; row < ROW_SIZE; row++)
    {
        board[row] = (int*)malloc(sizeof(int)*COL_SIZE);
    }

    //reads numbers from file and place them in each row
    for(int row = 0; row < ROW_SIZE; row++)
    {
        for(int col = 0; col < COL_SIZE; col++)
        {
            fscanf(fp, "%d,", &(board[row][col]));
        }
    }

     
    return board;
}

void *check_rows(void* param)
{
    param_struct *data = (param_struct*)param;
    //int num[9] = {1,2,3,4,5,6,7,8,9};
    //int buffer[9];
    //int count;
    int seen[9] = {0};
    
    worker_validation[data->id] = 1;
    
    for(int i = data->starting_col; i < data->ending_col; i++)
    {   
        
        int num = sudoku_board[data->starting_row][i];
        //printf("current number: %d\n", num);
        //printf("seen[%d] = %d\n",num-1,seen[num-1]);
        if(seen[num-1])
        {
            worker_validation[data->id] = 0;
            break;
        }
        seen[num-1] = 1;
        
    }
    //free(data);
    return NULL;
}

void *check_cols(void* param)
{
    param_struct *data = (param_struct*)param;
    //int num[9] = {1,2,3,4,5,6,7,8,9};
    //int buffer[9];
    //int count;
    int seen[9] = {0};
    
    worker_validation[data->id] = 1;
    
    for(int i = data->starting_row; i < data->ending_row; i++)
    {   
        
        int num = sudoku_board[i][data->starting_col];
        //printf("current number: %d\n", num);
        
        if(seen[num-1])
        {
            worker_validation[data->id] = 0;
            break;
        }
        seen[num-1] = 1;
        
    }
    //free(data);
    return NULL;
}

void *check_subgrids(void *param)
{
    param_struct *data = (param_struct*)param;
    //int num[9] = {1,2,3,4,5,6,7,8,9};
    //int buffer[9] = {0,0,0,0,0,0,0,0,0};
    //int count = 0;
    int seen[9] = {0};
    
    worker_validation[data->id] = 1;

    
    for(int i = data->starting_row; i < data->ending_row; i++)
    {   
        for(int j = data->starting_col; j < data->ending_col; j++)
        {
            int num = sudoku_board[i][j];
            //printf("current number: %d\n", num);

            if(seen[num-1])
            {
                worker_validation[data->id] = 0;
                break;
            }
            seen[num-1] = 1;
        }
        if(worker_validation[data->id] == 0)
        {
            break;
        }
    }
    
    //free(data);
    return NULL;
}


int is_board_valid(){
    pthread_t* tid;  /* the thread identifiers */
    pthread_attr_t attr;
    param_struct* parameter;
    
    // replace this comment with your code
    tid = (pthread_t*)malloc(sizeof(pthread_t)*NUM_OF_THREADS); //27 threads
    parameter = (param_struct*)malloc(sizeof(param_struct)*NUM_OF_THREADS);
    worker_validation = (int*)malloc(sizeof(int) * NUM_OF_THREADS);

    int count = 0;
    
    //rows
    for(int i = 0; i < ROW_SIZE; i++)
    {
        parameter[count].id = count;
        parameter[count].starting_col = 0;
        parameter[count].starting_row = i;
        parameter[count].ending_col = COL_SIZE-1;
        parameter[count].ending_row = i;

        pthread_create(&(tid[count]), NULL, check_rows, &(parameter[count]));
        count++;
    }

    //collumns
    for(int i = 0; i < COL_SIZE; i++)
    {
        parameter[count].id = count;
        parameter[count].starting_col = i;
        parameter[count].starting_row = 0;
        parameter[count].ending_col = i;
        parameter[count].ending_row = ROW_SIZE-1;

        pthread_create(&(tid[count]), NULL, check_cols, &(parameter[count]));
        count++;
    }

    //subgrids
    for(int i = 0; i < NUM_OF_SUBGRIDS; i++)
    {
        parameter[count].id = count;
        parameter[count].starting_col = (i%3)*3;
        parameter[count].starting_row = (i/3)*3;
        parameter[count].ending_col = (i%3)*3+3;
        parameter[count].ending_row = (i/3)*3+3;

        pthread_create(&(tid[count]), NULL, check_subgrids, &(parameter[count]));
        count++;
    }

    for(int i = 0; i < NUM_OF_THREADS; i++)
    {
        pthread_join(tid[i], NULL);
    }

    int valid = 1;
    for(int i = 0; i < NUM_OF_THREADS; i++)
    {
        if (worker_validation[i] == 0)
        {
            valid = 0;
            break;
        }
    }
    return valid;
} 
