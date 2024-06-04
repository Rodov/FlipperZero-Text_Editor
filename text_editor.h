#pragma once
// Используем стандартные библиотеки
#include <furi.h>
#include <gui/gui.h>

// Структура программы
typedef struct TextEditorApp {
    Gui* gui;
    ViewPort* view_port;
    FuriMessageQueue* event_queue;
} TextEditorApp;
