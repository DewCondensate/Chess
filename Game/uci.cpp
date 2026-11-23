#include <cstring>

#include "uci.hpp"

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
            // return parseCommandIsready(game);
        case SETOPTION:
            // return parseCommandSetoption(game);
        case REGISTER:
            // return parseCommandRegister(game);
        case UCINEWGAME:
            // return parseCommandUcinewgame(game);
        case POSITION:
            // return parseCommandPosition(game);
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
    int argumentCount = 1;
    while(strtok(NULL, " ") != NULL) {
        printf("info string %d extra argument(s) supplied...\n", argumentCount++);
    }

    sendID();
    sendOptions();

    return CONTINUE;
}

EngineState parseCommandDebug(Game * game) {
    int argumentCount = 1;
    char * token = strtok(NULL, " ");

    if(strcmp(token, "no")) {
        // Does nothing for now
        return CONTINUE;
    }

    if(strcmp(token, "yes")) {
        // Does nothing for now
        return CONTINUE;
    }

    token = strtok(NULL, " ");
    while(token != NULL) {
        printf("info string %d extra argument(s) supplied...\n", argumentCount++);
        token = strtok(NULL, " ");
    }

    return CONTINUE;
}

EngineState parseCommandIsready(Game * game);

EngineState parseCommandSetoption(Game * game);

EngineState parseCommandRegister(Game * game);

EngineState parseCommandUcinewgame(Game * game);

EngineState parseCommandPosition(Game * game);

EngineState parseCommandGo(Game * game);

EngineState parseCommandStop(Game * game);

EngineState parseCommandPonderhit(Game * game);

EngineState parseCommandQuit(Game * game);

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