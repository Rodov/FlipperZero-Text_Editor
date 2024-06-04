#pragma once
// Используем стандартные библиотеки
#include <furi.h>
#include <gui/gui.h>

// Виды событий
typedef enum {
    EventTypeTick,
    EventTypeInput,
} EventType;

// Структура очереди событий
typedef struct {
    EventType type;
    InputEvent input;
} TextEditorEvent;

// Структура программы
typedef struct TextEditorApp {
    Gui* gui;
    ViewPort* view_port;
    FuriMessageQueue* event_queue;
} TextEditorApp;
