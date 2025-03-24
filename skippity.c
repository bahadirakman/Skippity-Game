
// BAHADIR AKMAN 31.05.2024

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

void printTitle(void){
    printf(" ____  _  _____ ____  ____ ___ _______   __\n");
    printf("/ ___|| |/ /_ _|  _ \\|  _ \\_ _|_   _\\ \\ / /\n");
    printf("\\___ \\| ' / | || |_) | |_) | |  | |  \\ V /\n");
    printf(" ___) | . \\ | ||  __/|  __/| |  | |   | |\n");
    printf("|____/|_|\\_\\___|_|   |_|  |___| |_|   |_|\n");
    printf("\n");
}

void createBoard(char **board, int size)
{
    int i, j;                                   // loop variables
    int skipper_count = size * size - 4;        // middle 4 spaces are empty
    int each_skipper_count = skipper_count / 5; // skippers should appear equal number of times or very close
    int excess_skipper = skipper_count % 5;     // track excess skippers, some of them might be more in number, but just by 1
    
    // Create an array of skippers to shuffle afterwards
    char *skippers = (char*)malloc(skipper_count * sizeof(char));
    for(i = 0; i < skipper_count; i++)
    {
             if (i < each_skipper_count)        skippers[i] = 'A';
        else if (i < each_skipper_count * 2)    skippers[i] = 'B';
        else if (i < each_skipper_count * 3)    skippers[i] = 'C';
        else if (i < each_skipper_count * 4)    skippers[i] = 'D';
        else if (i < each_skipper_count * 5)    skippers[i] = 'E';
        // put excess skippers so that board is a square even though size is not multiply of 5.
        else
            if(excess_skipper > 0)
            {
                skippers[i] = 'A' + (i % 5);
                --excess_skipper;
            }
    }
    int rand_index;
    char temp;
    srand(time(NULL));
    // shuffle array
    for(i = 0; i < skipper_count- 1; i++)
    {
        rand_index = i + rand() / (RAND_MAX / (skipper_count - i) + 1);
        temp = skippers[i];
        skippers[i] = skippers[rand_index];
        skippers[rand_index] = temp;
    }
    
    // assign board elements
    int a = 0;
    int empty_square1 = size / 2 - 1;
    int empty_square2 = empty_square1 + 1;
    for(i = 0; i < size; i++)
    {
        for(j = 0; j < size; j++)
        {
            // middle 4 element of the board are empty
            if((i == empty_square1 || i == empty_square2) && (j == empty_square1 || j == empty_square2))
            {
                board[i][j] = ' ';
            }
            // fill the rest of the board
            else
            {
                board[i][j] = skippers[a];
                a++;
            }
        }
    }
    free(skippers);
}
// print the scores of players     {  each skipper count  }   -->   collected sets (total skipper count)
void printScores(int *one, int *two, char * name1, char * name2)
{
    int i, min1 = 100, min2 = 100;
    int sum1 = 0, sum2 = 0;
    printf("\n%s score\n", name1);
    printf("---------------\n");
    printf("{\t");
    for(i = 0; i < 5; i++)
    {
        printf("%d\t", one[i]);
        sum1 += one[i];
        // find the set count using mininum amount of a skipper
        if(min1 > one[i])
            min1 = one[i];
    }
    printf("}  -->  %d (%d)\n\n", min1, sum1);
    
    printf("%s score\n", name2);
    printf("---------------\n");
    printf("{\t");
    for(i = 0; i < 5; i++)
    {
        printf("%d\t", two[i]);
        sum2 += two[i];
        // find the set count using mininum amount of a skipper
        if(min2 > two[i])
            min2 = two[i];
    }
    printf("}  -->  %d (%d)\n", min2, sum2);
}
// print the board
void printBoard(char **board, int board_size)
{
    printf("\nCurrent Board: \n\n\t");
    int r, c;
    // display the column numbers at the top of the board
    for (c = 0; c < board_size; c++)
    {
        if(c >= 9)
            printf("  %d", c+1);  // two digit numbers
        else
            printf("  %d ", c+1); // one digit numbers
    }
    printf("\n\n\t");
    for (r = 0; r < board_size; r++)
    {
        for (c = 0; c < board_size; c++)
        {
            printf("+---");
        }
        printf("+\n");
        printf("%d\t", r+1); // display row numbers
        for (c = 0; c < board_size; c++)
        {
            printf("| %c ", board[r][c]);
        }
        printf("|\n\t");
    }
    for (c = 0; c < board_size; c++)
    {
        printf("+---");
    }
    printf("+\n\n");
}

// ask user for what they want to do
int turnPrompt(char *name)
{
    int choice;
    printf("%s's turn\n\n", name);
    printf("What do you do? \n");
    printf("1) Play\n");
    printf("2) Undo last move\n");
    printf("3) End turn\n");
    printf("4) End game\n");
    scanf("%d", &choice);
    
    return choice;
}

// write the current game state to a file
void saveGameState(char **board, char **old_board, int board_size,
                   int *player1_score, int *player2_score, char *name1, char *name2, int turn,
                   int * old_player1_score, int *old_player2_score,
                   int game_playable, int undo_spent, int row, int col, int old_row, int old_col,
                   int game_finished, int terminate, int a_piece_played, int game_mode)
{
    int save;
    // pick a save slot
    if(game_mode == 1)
    {
        do{
            printf("Choose a save slot (0-4): ");
            scanf("%d", &save);
        }while(save > 4 || save < 0);
    }
    else
    {
        do{
            printf("Choose a save slot (5-9): ");
            scanf("%d", &save);
        }while(save > 9 || save < 5);
    }
    
    char save_file[20] = "saved_game";
    char save_number = '0' + save;
    strncat(save_file, &save_number, 1);
    strcat(save_file, ".txt");
    // create a file in write mode
    FILE *fptr = fopen(save_file, "w");
    if(fptr == NULL)
    {
        printf("File error!\n");
        return;
    }
    
    fprintf(fptr, "%d\n", board_size);          // save board size
    int i, j;
    for (i = 0; i < board_size; i++)            // save board
    {
        for (j = 0; j < board_size; j++)
        {
            if(board[i][j] == ' ')
            {
                fprintf(fptr, "%c ", 'X');
            }
            else
            {
                fprintf(fptr, "%c ", board[i][j]);
            }
        }
        fprintf(fptr, "\n");
    }
    for (i = 0; i < 5; i++)                     // save player 1 score
    {
        fprintf(fptr, "%d ", player1_score[i]);
    }
    fprintf(fptr, "\n");
    for (i = 0; i < 5; i++)                     // save player 2 score
    {
        fprintf(fptr, "%d ", player2_score[i]);
    }
    fprintf(fptr, "\n");
    fprintf(fptr, "%s\n", name1);               // save player1 name
    fprintf(fptr, "%s\n", name2);               // save player2 name
    fprintf(fptr, "%d\n", turn);                // save whose turn it is
    for (i = 0; i < board_size; i++)            // save old_board
    {
        for (j = 0; j < board_size; j++)
        {
            if(old_board[i][j] == ' ')
            {
                fprintf(fptr, "%c ", 'X');
            }
            else
            {
                fprintf(fptr, "%c ", old_board[i][j]);
            }
        }
        fprintf(fptr, "\n");
    }
    for (i = 0; i < 5; i++)                     // save old player 1 score
    {
        fprintf(fptr, "%d ", old_player1_score[i]);
    }
    fprintf(fptr, "\n");
    for (i = 0; i < 5; i++)                     // save old player 2 score
    {
        fprintf(fptr, "%d ", old_player2_score[i]);
    }
    fprintf(fptr, "\n");
    // save game_playable
    fprintf(fptr, "%d\n", game_playable);
    // save undo_spent
    fprintf(fptr, "%d\n", undo_spent);
    // save row
    fprintf(fptr, "%d\n", row);
    // save col
    fprintf(fptr, "%d\n", col);
    // save old_row
    fprintf(fptr, "%d\n", old_row);
    // save old_col
    fprintf(fptr, "%d\n", old_col);
    // save game_finished
    fprintf(fptr, "%d\n", game_finished);
    // save terminate
    fprintf(fptr, "%d\n", terminate);
    // save a_piece_played
    fprintf(fptr, "%d\n", a_piece_played);

    fclose(fptr);
}
// load the game from file that previously saved
void loadGame(FILE *fptr, char **board, char **old_board, int board_size,
              int *player1_score, int *player2_score, char *name1, char *name2, int *turn,
              int * old_player1_score, int *old_player2_score,
              int *game_playable, int *undo_spent, int *row, int *col, int *old_row, int *old_col,
              int *game_finished, int *terminate, int *a_piece_played)
{
    int i, j;
    
    // get board values
    for(i = 0; i < board_size; i++)
    {
        for(j = 0; j < board_size; j++)
        {
            fscanf(fptr, " %c", &board[i][j]);
        }
    }
    // get scores of players
    for(i = 0; i < 5; i++)
    {
        fscanf(fptr, "%d", &player1_score[i]);
    }
    for(i = 0; i < 5; i++)
    {
        fscanf(fptr, "%d", &player2_score[i]);
    }
    // get names of players
    fscanf(fptr, "%s", name1);
    fscanf(fptr, "%s", name2);
    // get turn
    fscanf(fptr, "%d", turn);
    // get old_board values
    for(i = 0; i < board_size; i++)
    {
        for(j = 0; j < board_size; j++)
        {
            fscanf(fptr, " %c", &old_board[i][j]);
        }
    }

    // get old scores of players
    for(i = 0; i < 5; i++)
    {
        fscanf(fptr, "%d", &old_player1_score[i]);
    }
    for(i = 0; i < 5; i++)
    {
        fscanf(fptr, "%d", &old_player2_score[i]);
    }
    // get game_playable
    fscanf(fptr, "%d", game_playable);
    // get undo_spent
    fscanf(fptr, "%d", undo_spent);
    // get row
    fscanf(fptr, "%d", row);
    // get col
    fscanf(fptr, "%d", col);
    // get old_row
    fscanf(fptr, "%d", old_row);
    // get old_col
    fscanf(fptr, "%d", old_col);
    // get game_finished
    fscanf(fptr, "%d", game_finished);
    // get terminate
    fscanf(fptr, "%d", terminate);
    // get a_piece_played
    fscanf(fptr, "%d", a_piece_played);
    
    // replace 'X's with spaces
    for(i = 0; i < board_size; i++)
    {
        for(j = 0; j < board_size; j++)
        {
            if(board[i][j] == 'X')
            {
                board[i][j] = ' ';
            }
            if(old_board[i][j] == 'X')
            {
                old_board[i][j] = ' ';
            }
        }
    }
}
// check if chosen skipper is empty or coordinate is empty
int playable(char **board, int board_size, int row, int col)
{
    int playable = 0;   // if it is zero it is not playable

    // given coordinate is empty
    if(board[row][col] == ' ')
    {
        playable = 2;
    }
    // given coordinate is a skipper
    else
    {
        // check upper side of skipper
        if(row - 2 >= 0)
        {
            if(board[row - 2][col] == ' ' && board[row - 1][col] != ' ')
            {
                playable = 1;
            }
        }
        // check lower side of skipper
        if(row + 2 < board_size)
        {
            if(board[row + 2][col] == ' ' && board[row + 1][col] != ' ')
            {
                playable = 1;
            }
        }
        // check left side of skipper
        if(col - 2 >= 0)
        {
            if(board[row][col - 2] == ' ' && board[row][col - 1] != ' ')
            {
                playable = 1;
            }
        }
        // check right side of skipper
        if(col + 2 < board_size)
        {
            if(board[row][col + 2] == ' ' && board[row][col + 1] != ' ')
            {
                playable = 1;
            }
        }
    }
    return playable;
}
// check the current state of the board, if no piece can be played, return 0
int gamePlayable(char ** board, int board_size)
{
    int isPlayable = 0;
    
    int i, j;
    for(i = 0; i < board_size && isPlayable != 1; i++)
    {
        for(j = 0; j < board_size && isPlayable != 1; j++)
        {
            isPlayable = playable(board, board_size, i, j);
        }
    }
    
    return isPlayable;
}
// copy board into old_board
void copyBoard(char ** old_board, char ** board, int board_size)
{
    int i, j;
    for(i = 0; i < board_size; i++)
    {
        for(j = 0; j < board_size; j++)
        {
            old_board[i][j] = board[i][j];
        }
    }
}
// copy old score of the player
void copyScore(int *old_player_score, int *player_score)
{
    int j;
    for(j = 0; j < 5; j++)
    {
        old_player_score[j] = player_score[j];
    }
}
// move the chosen skipper to a given destination by user
void playSkipper(char **board, int board_size, int*score, int *row, int *col)
{
    int direction;
    do{
        printf("Choose a direction to move your skipper [ left(1), right(2), up(3), down(4) ]: ");
        scanf("%d", &direction);
    }while(direction > 4 || direction < 1);
    
    int moveSuccess = 0;
    while(!moveSuccess)
    {
        if(direction == 1 && *col-2 >= 0 && board[*row][*col-2] == ' ' &&  board[*row][*col-1] != ' ')
        {
            // move the skipper
            board[*row][*col-2] = board[*row][*col];
            // remove the skipper from its old place
            board[*row][*col] = ' ';
            // keep the removed skippity
            char removed_skippity = board[*row][*col-1];
            // remove the skippity that is jumped over
            board[*row][*col-1] = ' ';
            // determine the letter of removed skippity
            int score_index = removed_skippity - 'A';
            // increase score
            score[score_index]++;
            // move is done
            moveSuccess = 1;
            // skipper is now on a different place on board, remember it
            *col = *col - 2;
        }
        else if(direction == 2 && *col+2 < board_size && board[*row][*col+2] == ' ' && board[*row][*col+1] != ' ')
        {
            // move the skipper
            board[*row][*col+2] = board[*row][*col];
            // remove the skipper from its old place
            board[*row][*col] = ' ';
            // keep the removed skippity
            char removed_skippity = board[*row][*col+1];
            // remove the skippity that is jumped over
            board[*row][*col+1] = ' ';
            // determine the letter of removed skippity
            int score_index = removed_skippity - 'A';
            // increase score
            score[score_index]++;
            // move is done
            moveSuccess = 1;
            // skipper is now on a different place on board, remember it
            *col = *col + 2;
        }
        else if(direction == 3 && *row-2 >= 0 && board[*row-2][*col] == ' ' && board[*row-1][*col] != ' ')
        {
            // move the skipper
            board[*row-2][*col] = board[*row][*col];
            // remove the skipper from its old place
            board[*row][*col] = ' ';
            // keep the removed skippity
            char removed_skippity = board[*row-1][*col];
            // remove the skippity that is jumped over
            board[*row-1][*col] = ' ';
            // determine the letter of removed skippity
            int score_index = removed_skippity - 'A';
            // increase score
            score[score_index]++;
            // move is done
            moveSuccess = 1;
            // skipper is now on a different place on board, remember it
            *row = *row - 2;
        }
        else if(direction == 4 && *row+2 < board_size && board[*row+2][*col] == ' ' && board[*row+1][*col] != ' ')
        {
            // move the skipper
            board[*row+2][*col] = board[*row][*col];
            // remove the skipper from its old place
            board[*row][*col] = ' ';
            // keep the removed skippity
            char removed_skippity = board[*row+1][*col];
            // remove the skippity that is jumped over
            board[*row+1][*col] = ' ';
            // determine the letter of removed skippity
            int score_index = removed_skippity - 'A';
            // increase score
            score[score_index]++;
            // move is done
            moveSuccess = 1;
            // skipper is now on a different place on board, remember it
            *row = *row + 2;
        }
        else
        {
            printf("You can't move your skipper there!\n\n");
            do{
                printf("Choose a direction to move your skipper [ left(1), right(2), up(3), down(4) ]: ");
                scanf("%d", &direction);
            }while(direction > 4 || direction < 1);
        }
    }
}
// ask user for a skipper to play and its destination coordinate
void makeMove(char **board, int board_size, int*score, int *row, int *col, int *old_row, int *old_col)
{
    // first time picking a skipper to play in this turn
    if(*row == -1 && *col == -1)
    {
        int x, y;
        do{
            printf("Choose a skipper to play (row column): ");
            scanf("%d%d", &x, &y);
        }while(x > board_size || x < 1 || y > board_size || y < 1);
        
        *row = x-1;
        *col = y-1;
        // check if chosen skipper can be played
        int return_playable = playable(board, board_size, *row, *col);
        if(return_playable == 1)
        {
            *old_row = *row;
            *old_col = *col;
            playSkipper(board, board_size, score, row, col);
        }
        else if(return_playable == 2)
        {
            printf("Given coordinate is empty. Pick another one!\n");
            *row = -1;
            *col = -1;
            makeMove(board, board_size, score, row, col, old_row, old_col);
            return;
        }
        else
        {
            printf("Skipper is not playable. Pick another one!\n");
            *row = -1;
            *col = -1;
            makeMove(board, board_size, score, row, col, old_row, old_col);
            return;
        }
    }
    // if a skipper is already played this turn
    else
    {
        printf("Your current skipper is [%d][%d]\n", *row+1, *col+1);
        *old_row = *row;
        *old_col = *col;
        playSkipper(board, board_size, score, row, col);
    }
}

int playableSpecific(char ** board, int board_size, int row, int col, char min_char)
{
    int playable_specific = -1; // which direction
    int i = row, j = col;
    
    if(board[i][j] == ' ')
    {
        playable_specific = -1;
    }
    // given coordinate is a skipper
    else
    {
        // check upper side of skipper
        if(i - 2 >= 0)
        {
            if(board[i - 2][j] == ' ' && board[i - 1][j] != ' ' && board[i - 1][j] == min_char)
            {
                playable_specific = 3;
            }
        }
        // check lower side of skipper
        if(i + 2 < board_size)
        {
            if(board[i + 2][j] == ' ' && board[i + 1][j] != ' ' && board[i + 1][j] == min_char)
            {
                playable_specific = 4;
            }
        }
        // check left side of skipper
        if(j - 2 >= 0)
        {
            if(board[i][j - 2] == ' ' && board[i][j - 1] != ' ' && board[i][j - 1] == min_char)
            {
                playable_specific = 1;
            }
        }
        // check right side of skipper
        if(j + 2 < board_size)
        {
            if(board[i][j + 2] == ' ' && board[i][j + 1] != ' ' && board[i][j + 1] == min_char)
            {
                playable_specific = 2;
            }
        }
    }
        
    return playable_specific;
}
void playSkipperAI(char **board, int board_size, int *score, int *row, int *col, int found)
{
    int direction = found;
    
    int moveSuccess = 0;
    while(!moveSuccess)
    {
        if(direction == 1)
        {
            // move the skipper
            board[*row][*col-2] = board[*row][*col];
            // remove the skipper from its old place
            board[*row][*col] = ' ';
            // keep the removed skippity
            char removed_skippity = board[*row][*col-1];
            // remove the skippity that is jumped over
            board[*row][*col-1] = ' ';
            // determine the letter of removed skippity
            int score_index = removed_skippity - 'A';
            // increase score
            score[score_index]++;
            // move is done
            moveSuccess = 1;
            // skipper is now on a different place on board, remember it
            *col = *col - 2;
        }
        else if(direction == 2)
        {
            // move the skipper
            board[*row][*col+2] = board[*row][*col];
            // remove the skipper from its old place
            board[*row][*col] = ' ';
            // keep the removed skippity
            char removed_skippity = board[*row][*col+1];
            // remove the skippity that is jumped over
            board[*row][*col+1] = ' ';
            // determine the letter of removed skippity
            int score_index = removed_skippity - 'A';
            // increase score
            score[score_index]++;
            // move is done
            moveSuccess = 1;
            // skipper is now on a different place on board, remember it
            *col = *col + 2;
        }
        else if(direction == 3)
        {
            // move the skipper
            board[*row-2][*col] = board[*row][*col];
            // remove the skipper from its old place
            board[*row][*col] = ' ';
            // keep the removed skippity
            char removed_skippity = board[*row-1][*col];
            // remove the skippity that is jumped over
            board[*row-1][*col] = ' ';
            // determine the letter of removed skippity
            int score_index = removed_skippity - 'A';
            // increase score
            score[score_index]++;
            // move is done
            moveSuccess = 1;
            // skipper is now on a different place on board, remember it
            *row = *row - 2;
        }
        else if(direction == 4)
        {
            // move the skipper
            board[*row+2][*col] = board[*row][*col];
            // remove the skipper from its old place
            board[*row][*col] = ' ';
            // keep the removed skippity
            char removed_skippity = board[*row+1][*col];
            // remove the skippity that is jumped over
            board[*row+1][*col] = ' ';
            // determine the letter of removed skippity
            int score_index = removed_skippity - 'A';
            // increase score
            score[score_index]++;
            // move is done
            moveSuccess = 1;
            // skipper is now on a different place on board, remember it
            *row = *row + 2;
        }
    }
}

void makeAIMove(char **board, int board_size, int *score, int* actual_score)
{
    int i,j;
    static int row, col;
    static int a_piece_played = 0;
    int min_score_index = -1;
    int min_score = 200;
   
    static int recursion = 0; // no more than 5 recursion because there are 5 different skippers
    
    // determine the minimum right-most value in score array
    for(i = 0; i < 5; i++)
    {
        if(min_score >= score[i])
        {
            min_score = score[i];
            min_score_index = i;
        }
    }

    // determine the min char
    char min_char = 'A' + min_score_index;
    static int found = -1;
    if(found == -1 && a_piece_played == 0)
    {
        // find a skipper to play that can capture the minimum amount of skipper that AI has
        for(i = 0; i < board_size && found == -1; i++)
        {
            for(j = 0; j < board_size && found == -1; j++)
            {
                found = playableSpecific(board, board_size, i, j, min_char);
                if(found != -1)
                {
                    row = i;
                    col = j;
                }
            }
        }
    }
    else
    {
        found = playableSpecific(board, board_size, row, col, min_char);
    }
   
    // loop stops if AI finds a move that can capture min_char, row and col are the skipper that ai will move
    if(found == -1 && recursion < 5)
    {
        recursion++;
        score[min_score_index] = 100;
        makeAIMove(board, board_size, score, actual_score);
    }
    else if(found > 0 && found < 5 && recursion < 5)
    {
        // move the piece
        playSkipperAI(board, board_size, actual_score, &row, &col, found);
        for(i = 0; i < 5; i++){
            score[i] = actual_score[i];
        }
        printf("--------------------------------------------------------\n");
        printBoard(board, board_size);
        printf("--------------------------------------------------------\n");
        sleep(1);                                       // wait 2 seconds
        recursion = 0;
        a_piece_played = 1;
        makeAIMove(board, board_size, score, actual_score);
    }
    else
    {
        a_piece_played = 0;
        recursion = 0;
        found = -1;
        
    }
}

int main(void)
{
    printTitle();
    // ask the user for new game or load game
    printf("Choose a game mode\n\n");
    int game_mode;
    do{
        printf("1) Player versus Player\n");
        printf("2) Player versus AI\n");
        scanf("%d", &game_mode);
    }while(game_mode > 2 || game_mode < 0);
    
    int new_game;
    do{
        printf("1) New Game\n");
        printf("2) Load Game\n");
        scanf("%d", &new_game);
    }while(new_game > 2 || new_game < 0);
    
    int board_size = 0;             // board size
    char **board = NULL;            // game board
    char **old_board = NULL;        // for undo
    char name1[20];                 // name of player 1
    char name2[20];                 // name of player 2
    int player1_score[5] = {0};     // skipper counts of player 1
    int player2_score[5] = {0};     // skipper counts of player 2
    int turn = 1;                   // turn = 1 if player1's turn
                                    // turn = -1 if player2's turn
    int old_player1_score[5] = {0}; // create old scores to hold previous scores
    int old_player2_score[5] = {0};
    int game_playable = 1;          // keep track if there are moves to play
    int undo_spent = 0;             // keep track of whether a player spent his undo
    int row = -1, col = -1;         // keep track of the skipper the player played
    int old_row = -2, old_col = -2; // keep track of the previous place of players' skipper
    int game_finished = 0;          // if there are no playable skipper, game is finished
    int terminate = 0;              // end program if it is 1
    int a_piece_played = 0;         // check if a player made any move that turn
                                    // can't skip turn if it is 0
    int choice;                     // players' decision on what to do
    int i, j;                       // loop variables
    // create a fresh game
    if(new_game == 1)
    {
        // get the board size
        do{
            printf("Enter the size of the board (8-20): ");
            scanf("%d", &board_size);
        }while(board_size > 20 || board_size < 8);
        
        // create board
        board = (char**)malloc(board_size * sizeof(char*));
        for (i = 0; i < board_size; i++)
        {
            board[i] = (char*)malloc(board_size * sizeof(char));
        }
        // create old board to hold previous move
        old_board = (char**)malloc(board_size * sizeof(char*));
        for (i = 0; i < board_size; i++)
        {
            old_board[i] = (char*)malloc(board_size * sizeof(char));
        }
        old_board[0][0] = 'Z'; // old board may not be initialized, check if first element is 'X' when necessary
        
        // get players' names
        printf("Player 1 name: ");
        scanf("%s", name1);
        if(game_mode == 1)
        {
            printf("Player 2 name: ");
            scanf("%s", name2);
        }
        else
        {
            strcpy(name2, "AI");
        }
        createBoard(board, board_size); // create a board with random skippers
    }
    // load game from file
    else
    {
        // open a file in read mode, there are 10 slots to load from
        int load;
        if(game_mode == 1)
        {
            do{
                printf("Choose a load slot (0-4): ");
                scanf("%d", &load);
            }while(load > 4 || load < 0);
        }
        else
        {
            do{
                printf("Choose a load slot (5-9): ");
                scanf("%d", &load);
            }while(load > 9 || load < 5);
        }
        
        char load_file[20] = "saved_game";
        char load_number = '0' + load;
        strncat(load_file, &load_number, 1);
        strcat(load_file, ".txt");
        
        // open the chosen file
        FILE *fptr = fopen(load_file, "r");
        if(fptr == NULL)
        {
            printf("File error!\n");
            exit(0);
        }
        // check if file is empty
        int first_char = fgetc(fptr);
        if(first_char == EOF)
        {
            printf("File is empty! Terminating...\n");
            exit(0);
        }
        fseek(fptr, 0, SEEK_SET); // move cursor to the beginning of the file
        
        // get board size from first line
        fscanf(fptr, "%d", &board_size);
        // create board
        board = (char**)malloc(board_size * sizeof(char*));
        for (i = 0; i < board_size; i++)
        {
            board[i] = (char*)malloc(board_size * sizeof(char));
        }
        old_board = (char**)malloc(board_size * sizeof(char*));
        for (i = 0; i < board_size; i++)
        {
            old_board[i] = (char*)malloc(board_size * sizeof(char));
        }
        // load the saved game's every state
        loadGame(fptr, board, old_board, board_size,
                 player1_score, player2_score, name1, name2, &turn,
                 old_player1_score, old_player2_score,
                 &game_playable, &undo_spent, &row, &col, &old_row, &old_col,
                 &game_finished, &terminate, &a_piece_played);
        fclose(fptr);
    }
    
    printScores(player1_score, player2_score, name1, name2);        // display scores on top
    printBoard(board, board_size);                                  // display board each turn

    // while player doesn't end the game
    while(!terminate)
    {
        // player 1 turn
        if(turn == 1)
        {
            do{
                choice = turnPrompt(name1);     // display prompt
            }while(choice < 1 || choice > 4);
            
            // act according to players choice
            
            // player makes their move
            if(choice == 1)
            {
                // player plays a skipper for the first time in this turn
                if(row == -1 && col == -1)
                {
                    copyBoard(old_board, board, board_size);                    // keep previous board to undo if player asks for it
                    copyScore(old_player1_score, player1_score);                // keep prevous score of player
                    makeMove(board, board_size, player1_score, &row, &col, &old_row, &old_col);     // ask user to make his move
                    a_piece_played = 1;
                    printScores(player1_score, player2_score, name1, name2);
                    printBoard(board, board_size);
                }
                // player already played a skipper, now they have to play the same skipper
                else
                {
                    int return_playable = playable(board, board_size, row, col);
                    if(return_playable == 1)
                    {
                        old_row = row;  // store previous move
                        old_col = col;
                        copyBoard(old_board, board, board_size);                    // keep previous board to undo if player asks for it
                        copyScore(old_player1_score, player1_score);                // keep prevous score of player
                        makeMove(board, board_size, player1_score, &row, &col, &old_row, &old_col);     // ask user to make his move
                        printScores(player1_score, player2_score, name1, name2);
                        printBoard(board, board_size);
                    }
                    else
                    {
                        printf("Your skipper is not playable. You can't make another move!\n\n");
                    }
                }
            }
            // load the "old_board" (undo move)
            else if(choice == 2)
            {
                if(a_piece_played == 0)
                {
                    printf("You didn't play a piece this turn, you can't undo!\n\n");
                }
                else if(undo_spent == 1)
                {
                    printf("You already spent your undo!\n\n");
                }
                else
                {
                    // load previous board
                    copyBoard(board, old_board, board_size);
                    copyScore(player1_score, old_player1_score);
                    row = old_row;
                    col = old_col;
                    printf("\nPrevious board and scores are loaded!\n\n");
                    
                    printScores(player1_score, player2_score, name1, name2);
                    printBoard(board, board_size);
                    undo_spent = 1;
                }
            }
            // end the turn
            else if(choice == 3)
            {
                if(a_piece_played == 0)
                {
                    printf("You didn't play a piece this turn, please make your move!\n\n");
                }
                else
                {
                    turn = -1;
                    // refresh variables
                    a_piece_played = 0;
                    undo_spent = 0;
                    row = -1;
                    col = -1;
                    old_row = -2;
                    old_col = -2;
                }
            }
            // end game and save
            else if(choice == 4)
            {
                // if there is not any previous board (first turn), fill it with 'Z's
                if(old_board[0][0] == 'Z')
                {
                    for(i = 0; i < board_size; i++)
                    {
                        for(j = 0; j < board_size; j++)
                        {
                            old_board[i][j] = 'Z';
                        }
                    }
                }
                // save game state
                saveGameState(board, old_board, board_size,
                              player1_score, player2_score, name1, name2, turn,
                              old_player1_score, old_player2_score,
                              game_playable, undo_spent, row, col, old_row, old_col,
                              game_finished, terminate, a_piece_played, game_mode);
                terminate = 1;  // terminate game
                printf("Current game state is saved. Terminating...\n");
            }
        }
        // player 2 or AI turn
        else
        {
            // player 2 turn
            if(game_mode == 1)
            {
                do{
                    choice = turnPrompt(name2);     // display prompt
                }while(choice < 1 || choice > 4);
                
                // act according to players choice
                
                // player makes their move
                if(choice == 1)
                {
                    // player plays a skipper for the first time in this turn
                    if(row == -1 && col == -1)
                    {
                        copyBoard(old_board, board, board_size);                    // keep previous board to undo if player asks for it
                        copyScore(old_player2_score, player2_score);                // keep prevous score of player
                        makeMove(board, board_size, player2_score, &row, &col, &old_row, &old_col);     // ask user to make his move
                        a_piece_played = 1;
                        printScores(player1_score, player2_score, name1, name2);
                        printBoard(board, board_size);
                    }
                    // player already played a skipper, now they have to play the same skipper
                    else
                    {
                        int return_playable = playable(board, board_size, row, col);
                        if(return_playable == 1)
                        {
                            old_row = row;
                            old_col = col;
                            copyBoard(old_board, board, board_size);                    // keep previous board to undo if player asks for it
                            copyScore(old_player2_score, player2_score);                // keep prevous score of player
                            makeMove(board, board_size, player2_score, &row, &col, &old_row, &old_col);     // ask user to make his move
                            printScores(player1_score, player2_score, name1, name2);
                            printBoard(board, board_size);
                        }
                        else
                        {
                            printf("Your skipper is not playable. You can't make another move!\n\n");
                        }
                    }
                }
                // load the "old_board" (undo move)
                else if(choice == 2)
                {
                    if(a_piece_played == 0)
                    {
                        printf("You didn't play a piece this turn, you can't undo!\n\n");
                    }
                    else if(undo_spent == 1)
                    {
                        printf("You already spent your undo!\n\n");
                    }
                    else
                    {
                        // load previous board
                        copyBoard(board, old_board, board_size);
                        copyScore(player2_score, old_player2_score);
                        row = old_row;
                        col = old_col;
                        printf("\nPrevious board and scores are loaded!\n\n");
                        
                        printScores(player1_score, player2_score, name1, name2);
                        printBoard(board, board_size);
                        undo_spent = 1;
                    }
                }
                // end the turn
                else if(choice == 3)
                {
                    if(a_piece_played == 0)
                    {
                        printf("You didn't play a piece this turn, please make your move!\n\n");
                    }
                    else
                    {
                        turn = 1;
                        a_piece_played = 0;
                        undo_spent = 0;
                        row = -1;
                        col = -1;
                        old_row = -2;
                        old_col = -2;
                    }
                }
                // end game and save
                else if(choice == 4)
                {
                    if(old_board[0][0] == 'Z')
                    {
                        for(i = 0; i < board_size; i++)
                        {
                            for(j = 0; j < board_size; j++)
                            {
                                old_board[i][j] = 'Z';
                            }
                        }
                    }
                    // save game state
                    saveGameState(board, old_board, board_size,
                                  player1_score, player2_score, name1, name2, turn,
                                  old_player1_score, old_player2_score,
                                  game_playable, undo_spent, row, col, old_row, old_col,
                                  game_finished, terminate, a_piece_played, game_mode);
                    terminate = 1;  // terminate game
                    printf("Current game state is saved. Terminating...\n");
                }
            }
            // AI turn
            else
            {
                int temp_scores[5];
                for(i = 0; i < 5; i++)
                {
                    temp_scores[i] = player2_score[i];
                }
                printf("AI's turn...\n\n");
                sleep(1);
                makeAIMove(board, board_size, temp_scores, player2_score);   // AI makes their move
                turn = 1;                                       // end of AI's turn
                printScores(player1_score, player2_score, name1, name2);
                printBoard(board, board_size);
            }
        }
        
        // check each piece, if there is at least 1 piece playable, continue game
        game_playable = gamePlayable(board, board_size);
        if(game_playable != 1)
        {
            printf("\nGAME IS OVER!\n\n");
            terminate = 1;
            game_finished = 1;
        }
    }

    if(game_finished)
    {
        // there are no moves to play, game is finished, calculate and print scores, determine the winner
        int player1_total_skippers = 0;
        int player2_total_skippers = 0;
        int player1_total_set = 100;
        int player2_total_set = 100;
        int winner;
        for(i = 0; i < 5; i++)
        {
            player1_total_skippers += player1_score[i];
            player2_total_skippers += player2_score[i];
            if(player1_total_set > player1_score[i])
            {
                player1_total_set = player1_score[i];
            }
            if(player2_total_set > player2_score[i])
            {
                player2_total_set = player2_score[i];
            }
        }
        if(player1_total_set > player2_total_set)
        {
            winner = 1;
        }
        else if(player2_total_set > player1_total_set)
        {
            winner = -1;
        }
        else
        {
            if(player1_total_skippers > player2_total_skippers)
            {
                winner = 1;
            }
            else if(player2_total_skippers > player1_total_skippers)
            {
                winner = -1;
            }
            else
            {
                winner = 0;
            }
        }
        
        printf("%s TOTAL SET      -> %d\n", name1, player1_total_set);
        printf("%s TOTAL SKIPPERS -> %d\n\n", name1, player1_total_skippers);
        
        printf("%s TOTAL SET      -> %d\n", name2, player2_total_set);
        printf("%s TOTAL SKIPPERS -> %d\n\n", name2, player2_total_skippers);
        
        if(winner == 1)
        {
            printf("%s HAS WON\n\n", name1);
        }
        else if(winner == -1)
        {
            printf("%s HAS WON\n\n", name2);
        }
        else
        {
            printf("IT'S A TIE\n\n");
        }
    }
    
    for(i = 0; i < board_size; i++)
    {
        free(board[i]);
        free(old_board[i]);
    }
    free(board);
    free(old_board);
    return 0;
}
