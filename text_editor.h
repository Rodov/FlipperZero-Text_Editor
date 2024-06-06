#pragma once

// Используем стандартные библиотеки
#include <stdio.h>
#include <furi.h>
#include <gui/gui.h>
#include <gui/elements.h>
#include <input/input.h>
#include <notification/notification_messages.h>

typedef enum {
    INSTRUCTION,
    VIEWING,
    EDITING,
    TOTAL_DRAW_MODES = 3,
} DrawMode;

// Структура программы
typedef struct TextEditorApp {
    Gui* gui;
    ViewPort* view_port;
    FuriMessageQueue* event_queue;

    DrawMode draw_mode;

} TextEditorApp;
