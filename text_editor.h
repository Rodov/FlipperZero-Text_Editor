#pragma once

#include <furi.h>
#include <gui/gui.h>

typedef enum {
    EventTypeTick,
    EventTypeInput,
} EventType;

typedef struct {
    EventType type;
    InputEvent input;
} TextEditorEvent;

struct TextEditorApp {
    Gui* gui;
    ViewPort* view_port;
};

typedef struct TextEditorApp TextEditorApp;
