#include <stdio.h>
#include <string.h>
#include <switch.h>

#include "util.h"
#include "es.h"                 // for starting es services.
#include "ns.h"                 // for starting ns services.
#include "ncm.h"                // for cleaning up placeholders on startup.
#include "menu/main_menu.h"

//#define DEBUG
#define APP_DIR     "/switch/yati"
#define APP_PATH    "sdmc:/switch/yati/yati.nro"


void app_init()
{
    consoleInit(NULL);              // default basic console window
    if (R_FAILED(socketInitializeDefault())) // open socket for network stuff
        fatalThrow(0xE0E1);
    if (R_FAILED(fsInitialize()))   // file system
        fatalThrow(0xE0E3);
    if (R_FAILED(nsInitialize()))   // ns services
        fatalThrow(0xE0E4);
    if (R_FAILED(ncmInitialize())) // nintendo content manager stuff
        fatalThrow(0xE0E5);
    if (R_FAILED(plInitialize())) //
        fatalThrow(0xE0E6);
    if (R_FAILED(usbCommsInitialize()))
        fatalThrow(0xE0E7);
    if (R_FAILED(ns_start_services()))
        fatalThrow(0xE0E9);
    if (R_FAILED(es_start_service()))
        fatalThrow(0xE0EA);
    if (R_FAILED(splInitialize()))
        fatalThrow(0xE0EB);
    if (R_FAILED(splSslInitialize()))
        fatalThrow(0xE0EC);
    if (R_FAILED(splEsInitialize()))
        fatalThrow(0xE0ED);
    if (R_FAILED(splFsInitialize()))
        fatalThrow(0xE0EE);
    if (R_FAILED(splCryptoInitialize()))
        fatalThrow(0xE0EF);
    #ifdef DEBUG
    printf("check your cmd window for output!\n");
    nxlinkStdio(); // redirect stdout to nxlink console window
    #endif
}

void app_exit()
{
    consoleExit(NULL);
    socketExit();
    fsExit();
    nsExit();
    ncmExit();
    plExit();
    setExit();
    usbCommsExit();
    es_close_service();
    ns_close_services();
    splExit();
    splFsExit();
    splEsExit();
    splSslExit();
    splCryptoExit();
}

void setup_app_dir(const char *nro)
{
    if (strcmp(APP_PATH, nro) == 0)
        return;
    if (!check_if_dir_exists(APP_DIR))
        create_dir(APP_DIR);
    if (check_if_file_exists(APP_PATH))
        delete_file(APP_PATH);
    move_file(nro, APP_PATH);
}

int main(int argc, char *argv[])
{
    // init stuff
    app_init();
    setup_app_dir(argv[0]);
    change_dir(APP_DIR);

    // clear all placeholders on startup.
    ncm_delete_all_placeholders();

    // goto main menu
    menu_main();

    // clean up then exit.
    app_exit();
    return 0;
}