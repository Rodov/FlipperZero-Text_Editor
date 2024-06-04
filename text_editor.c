#include <stdio.h>
#include <furi.h>
#include <gui/gui.h>
#include <gui/elements.h>
#include <input/input.h>
#include <notification/notification_messages.h>

#include <text_editor.h>

// Функция отрисовки
static void text_editor_app_draw_callback(Canvas* canvas, void* ctx) {
    UNUSED(ctx);
    canvas_clear(canvas);

    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 4, 8, "Hello world!");

    canvas_set_font(canvas, FontSecondary);
    elements_multiline_text_aligned(canvas, 127, 15, AlignRight, AlignTop, "Some long long long long \n aligned multiline text");
}

// Функция обработки очереди сообщений
static void text_editor_app_input_callback(InputEvent* input_event, void* ctx) {
    furi_assert(ctx);

    FuriMessageQueue* event_queue = ctx;
    furi_message_queue_put(event_queue, input_event, FuriWaitForever);
}

// Функция выделения ресурсов
TextEditorApp* text_editor_app_alloc(){
    TextEditorApp* app = malloc(sizeof(TextEditorApp));

    app->view_port = view_port_alloc();
    app->event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));

    view_port_draw_callback_set(app->view_port, text_editor_app_draw_callback, NULL);
    view_port_input_callback_set(app->view_port, text_editor_app_input_callback, app->event_queue);


    app->gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(app->gui, app->view_port, GuiLayerFullscreen);

    return app;
}

// Функция освобождения ресурсов
void text_editor_app_free(TextEditorApp* app) {
    furi_assert(app);

    view_port_enabled_set(app->view_port, false);
    gui_remove_view_port(app->gui, app->view_port);
    view_port_free(app->view_port);

    furi_message_queue_free(app->event_queue);

    furi_record_close(RECORD_GUI);
}

// Основная программа
int32_t text_editor_app(void* p) {
    UNUSED(p);

    // Выделяем ресурсы
    TextEditorApp* app = text_editor_app_alloc();

    InputEvent event;

    while (1) {
    // Считываем очередь и обрабатываем нажатия кнопок
    if (furi_message_queue_get(app->event_queue, &event, 100) == FuriStatusOk) {
        if (event.type == InputTypePress) {
            if (event.key == InputKeyBack)
                break;
            }
        }
    }

    // Освобождаем ресурсы
    text_editor_app_free(app);

     return 0;
}