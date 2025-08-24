#include "board.h"

bool board_new(struct Board **board, SDL_Renderer *renderer) {
    *board = calloc(1, sizeof(struct Board));
    if (*board == NULL) {
        fprintf(stderr, "Error in Calloc of New Board.\n");
        return false;
    }
    struct Board *b = *board;

    b->renderer = renderer;
    b->columns = WINDOW_WIDTH / CELL_SIZE;
    b->rows = WINDOW_HEIGHT / CELL_SIZE;

    b->board = calloc(1, sizeof(bool)*(size_t)(b->rows*b->columns));
    if(b->board == NULL) {
        fprintf(stderr, "Error in Calloc of Board.\n");
        return false;
    }

    b->next_board = calloc(1, sizeof(bool)*(size_t)(b->rows*b->columns));
    if(b->next_board == NULL) {
        fprintf(stderr, "Error in Calloc of Next Board.\n");
        return false;
    }

    board_reset(b);

    return true;
}

void board_free(struct Board **board) {
    if (*board) {
        struct Board *b = *board;

        if (b->board) {
            free(b->board);
            b->board = NULL;
        }
        if (b->next_board) {
            free(b->next_board);
            b->next_board = NULL;
        }

        b->renderer = NULL;

        free(*board);
        *board = NULL;
        b = NULL;

        printf("Clean board.\n");
    }
}

void board_reset(struct Board *b) {
    for(int row = 0; row < b->rows; row++) {
        for(int col = 0; col < b->columns; col++) {
            b->board[row * b->columns + col] = (rand() % 2)? true : false;
        }
    }
}

void board_clear(struct Board *b) {
    for(int row = 0; row < b->rows; row++) {
        for(int col = 0; col < b->columns; col++) {
            b->board[row * b->columns + col] = false;
        }
    }
}

void board_edit(struct Board *b, int x, int y) {
    int col = x / CELL_SIZE;
    int row = y / CELL_SIZE;
    int index = row * b->columns + col;
    b->board[index] = !b->board[index];
}

void board_update(struct Board *b) {
    for (int row = 0; row < b->rows; row++) {
        for(int col = 0; col < b->columns; col++) {
            int index = row * b->columns + col;
            int count = 0;
            for (int r = row - 1; r <= row + 1; r++) {
                for (int c = col - 1; c <= col + 1; c++) {
                    if (r != row || c != col) {
                        int wrap_r = (r + b->rows) % b->rows;
                        int wrap_c = (c + b->columns) % b->columns;
                        count += b->board[wrap_r * b->columns + wrap_c];
                    }
                }
            }
            bool is_alive = b->board[index];
            if (count == 3) {
                b->next_board[index] = true;
            } else if (is_alive && count == 2) {
                b->next_board[index] = true;
            } else {
                b->next_board[index] = false;
            }
        }
    }

    bool *old_board = b->board;
    b->board = b->next_board;
    b->next_board = old_board;
}

void board_draw(const struct Board *b) {
    SDL_SetRenderDrawColor(b->renderer, CELL_COLOR);
    SDL_Rect rect = {0, 0, CELL_SIZE - CELL_PADDING, CELL_SIZE - CELL_PADDING};
    for (int row = 0; row < b->rows; row++) {
        rect.y = CELL_SIZE*row;
        for (int col = 0; col < b->columns; col++) {
            rect.x = CELL_SIZE*col;
            if (b->board[row * b->columns + col]) {
                SDL_RenderFillRect(b->renderer, &rect);
            }
        }
    }
}