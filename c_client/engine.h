#pragma once

void freeGames(struct stContext *pCtx);
void freeGameSet(struct stContext *pCtx);

bool updateGamesInfo(struct stContext *pCtx, bool bSendRequest);
void printGamesInfo(struct stContext *pCtx);
bool startGamesMenu(struct stContext *pCtx);
bool updatePlayers(struct stContext *pCtx);
void printPlayers(struct stContext *pCtx);
void startGame(struct stContext *pCtx);
void updatePlayersEx(struct stContext *pCtx);
void printPlayersEx(struct stContext *pCtx);
void receivePersonalMessages(struct stContext *pCtx);
void receiveMultiCastMessages(struct stContext *pCtx);
void printMaze(struct stContext *pCtx);
void play(struct stContext *pCtx);
