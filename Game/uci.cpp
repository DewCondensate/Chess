#include <cstring>

#include "uci.hpp"

const char * engineOptions[] = {
	"nooption1",
	"nooption2"
};

Command readCommand(char * input) {
    Command output = UNKNOWN;

    if(input == NULL) { return output; }
    if(strcmp(input, "uci") == 0) { output = UCI; return output; }
    if(strcmp(input, "debug") == 0) { output = DEBUG; return output; }
    if(strcmp(input, "isready") == 0) { output = ISREADY; return output; }
    if(strcmp(input, "setoption") == 0) { output = SETOPTION; return output; }
    if(strcmp(input, "register") == 0) { output = REGISTER; return output; }
    if(strcmp(input, "ucinewgame") == 0) { output = UCINEWGAME; return output; }
    if(strcmp(input, "position") == 0) { output = POSITION; return output; }
    if(strcmp(input, "go") == 0) { output = GO; return output; }
    if(strcmp(input, "stop") == 0) { output = STOP; return output; }
    if(strcmp(input, "ponderhit") == 0) { output = PONDERHIT; return output; }
    if(strcmp(input, "quit") == 0) { output = QUIT; return output; }

    return output;
}

EngineState parseInput(char userInput[2000], Game * game) {
	char * token = strtok(userInput, " ");
    Command command = readCommand(token);

    switch(command) {
        case UCI:
            return parseCommandUci(game);
        case DEBUG:
            return parseCommandDebug(game);
        case ISREADY:
            return parseCommandIsready(game);
        case SETOPTION:
            return parseCommandSetoption(game);
        case REGISTER:
            return parseCommandRegister(game);
        case UCINEWGAME:
            return parseCommandUcinewgame(game);
        case POSITION:
            return parseCommandPosition(game);
        case GO:
            // return parseCommandGo(game);
        case STOP:
            // return parseCommandStop(game);
        case PONDERHIT:
            // return parseCommandPonderhit(game);
        case QUIT:
            // return parseCommandQuit(game);
        case UNKNOWN:
            return CONTINUE;
    }
}

EngineState parseCommandUci(Game * game) {
    sendID();
    sendOptions();
    
    int argumentCount = 1;
    while(strtok(NULL, " ") != NULL) {
        printf("info string %d extra argument(s) supplied...\n", argumentCount++);
    }

    return CONTINUE;
}

EngineState parseCommandDebug(Game * game) {
    char * token = strtok(NULL, " ");

    if(token == NULL) {
        printf("info string not enough arguments supplied\n");
        return CONTINUE;
    }

    if(strcmp(token, "no") == 0) {
        // Does nothing for now
        printf("info string debug set to no\n");
    }

    if(strcmp(token, "yes") == 0) {
        // Does nothing for now
        printf("info string debug set to yes\n");
    }

    int argumentCount = 1;
    token = strtok(NULL, " ");
    while(token != NULL) {
        printf("info string %d extra argument(s) supplied...\n", argumentCount++);
        token = strtok(NULL, " ");
    }

    return CONTINUE;
}

EngineState parseCommandIsready(Game * game) {
    readyok();

    int argumentCount = 1;

    char * token = strtok(NULL, " ");
    while(token != NULL) {
        printf("info string %d extra argument(s) supplied...\n", argumentCount++);
        token = strtok(NULL, " ");
    }

    return CONTINUE;
}

EngineState parseCommandSetoption(Game * game) {
    char * token = strtok(NULL, " ");
    
    if(token == NULL) {
        printf("info string setoption 'name' not provided\n");
        return CONTINUE;
    }

    if(strcmp(token, "name") != 0) {
        printf("info string setoption expected 'name'\n");
        return CONTINUE;
    }

    token = strtok(NULL, " ");

    if(token == NULL) {
        printf("info string setoption name parameter not provided\n");
        return CONTINUE;
    }

    EngineOption option = ENGINE_OPTION_END;

    toLowerCase(token);

    for(int i = 0; i < ENGINE_OPTION_END; i++) {
        if(strcmp(engineOptions[i], token) == 0) {
            option = (EngineOption) i;
        }
    }

    if(option == ENGINE_OPTION_END) {
        printf("info string setoption unknown option name '%s'\n", token);
        return CONTINUE;
    }

    token = strtok(NULL, " ");

    if(token == NULL) {
        printf("info string setoption no value provided\n");
        return CONTINUE;
    }

    if(strcmp(token, "value") != 0) {
        printf("info string setoption expected 'value'\n");
        return CONTINUE;
    }

    token = strtok(NULL, " ");

    if(token == NULL) {
        printf("info string setoption no value parameter provided\n");
        return CONTINUE;
    }

    printf("info string setoption '%s' to '%s'\n", engineOptions[option], token);

    return CONTINUE;
}

EngineState parseCommandRegister(Game * game) {
    printf("info string registration is not required\n");
    return CONTINUE;
}

EngineState parseCommandUcinewgame(Game * game) {
    int argumentCount = 1;

    char * token = strtok(NULL, " ");
    while(token != NULL) {
        printf("info string %d extra argument(s) supplied...\n", argumentCount++);
        token = strtok(NULL, " ");
    }

    printf("info string ucinewgame received\n");
    return CONTINUE;
}

EngineState parseCommandPosition(Game * game) {
    char * token = strtok(NULL, " ");
    
    if(token == NULL) {
        printf("info string position no position provided\n");
        return CONTINUE;
    }

    if(strcmp(token, "fen") == 0) {
        token = strtok(NULL, "\n");

        if(token == NULL) {
            printf("info string no fen provided\n");
            return CONTINUE;
        }

        // NOTE: THIS IS NOT ERROR SAFE!!
        game->loadFromFEN(token);

        int argumentCount = 1;
        
        token = strtok(NULL, " ");
        while(token != NULL) {
            printf("info string %d extra argument(s) supplied...\n", argumentCount++);
            token = strtok(NULL, " ");
        }

        printf("info string fen loaded!\n");
        return CONTINUE;
    }

    if(strcmp(token, "startpos") == 0) {
        token = strtok(NULL, " ");
        game->loadFromFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

        while(token != NULL) {
            Move move = parseMove(game, token);
            if(move.to == 0 && move.from == 0 && move.flag == NONE) {
                printf("info string invalid move %s\n", token);
                return CONTINUE;
            }
            game->doMove(move);
            token = strtok(NULL, " ");
        }
        printf("info string fen loaded!\n");

        return CONTINUE;
    }

    printf("info string position unknown argument '%s'\n", token);
    return CONTINUE;
}

EngineState parseCommandGo(Game * game);

EngineState parseCommandStop(Game * game);

EngineState parseCommandPonderhit(Game * game);

EngineState parseCommandQuit(Game * game);

void toLowerCase(char * input) {
    while(*input != '\0') {
        *input += ('a' - 'A') * (*input >= 'A' && *input <= 'Z');
        input++;
    }
}

void sendID() {
    printf("name DewChess\n");
    printf("author dew\n");
}

void sendOptions() {
    // Do nothing for now
}

void uciok() {
    printf("uciok\n");
}

void readyok() {
    printf("readyok\n");
}

Move parseMove(Game * game, char * input) {
    Move * output = 0;
	Move moveStart[MAXMOVESPOSSIBLE];
    Move * moves = moveStart;
	game->getLegalMoves(&moves);

    char col = input[0];
    int row = input[1] - '0';
    char col2 = input[2];
    int row2 = input[3] - '0';
    row -= 1;
    row = 7 - row;
    col = col >= 'a' ? col - 'a' + 'A' : col;
    col -= 'A';
    int from = row * 8 + col;

    row2 -= 1;
    row2 = 7 - row2;
    col2 = col2 >= 'a' ? col2 - 'a' + 'A' : col2;
    col2 -= 'A';
    int to = row2 * 8 + col2;

    Flag promotion = NONE;
    if(input[4] != '\0') {
        char promotionChar = input[4] >= 'a' ? input[4] - ('a' - 'A') : input[4];

        switch(promotionChar) {
            case 'K':
                promotion = PROMOTEKNIGHT;
                break;
            case 'B':
                promotion = PROMOTEBISHOP;
                break;  
            case 'R':
                promotion = PROMOTEROOK;
                break;
            case 'Q':
                promotion = PROMOTEQUEEN;
                break;
            default:
                return Move{0, 0, NONE};
        }
    }

    for(Move * i = moveStart; i < moves; i++) {
        if(i->from == from && i->to == to) {
            if(promotion != NONE) {
                if(i->flag == promotion) {
                    output = i;
                    break;
                }
            } else {
                output = i;
                break;
            }
        }
    }

	return (output == 0) ? Move{0, 0, NONE} : *output;
}