#ifndef EMONCMS_H
#define EMONCMS_H

#include "config.h"

// common function declarations
extern int initGatherData();
extern int gatherData (struct config *config, struct data *data);
static void printHelp(void);

// function declaration for huawei_emoncms
static int readModemData (struct config *config, struct data *data, int socket_fd);

// emonCMS specific function
extern int sendToEmonCMS (struct config *config, struct data *data, int socket_fd);

#endif
