// *********************************
// Подключаем библиотеки и структуры
// *********************************
#include <text_editor.h>

// *****************
// Функция отрисовки
// *****************
static void text_editor_app_draw_callback(Canvas* canvas, void* ctx) {
    furi_assert(ctx);
    TextEditorApp* app = ctx;

    // Очищаем канву
    canvas_clear(canvas);

    // Пишем на экране режим работы
    DrawMode mode = app->draw_mode;
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 4, 8, "Draw Mode");

    if(mode == INSTRUCTION) { // РЕЖИМ отображения инструкции
        canvas_set_font(canvas, FontSecondary);
        canvas_draw_str(canvas, 4, 20, "Instruction");
    } else if(mode == VIEWING) { // РЕЖИМ просмотра файла
        canvas_set_font(canvas, FontSecondary);
        canvas_draw_str(canvas, 4, 20, "Viewing");
    } else if(mode == EDITING) { // РЕЖИМ редактирования файла
        canvas_set_font(canvas, FontSecondary);
        canvas_draw_str(canvas, 4, 20, "Editing");
    } else { // РЕЖИМ неопознан
        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str(canvas, 4, 20, "Something wrong!");
    }
}

// ***********************************
// Функция обработки очереди сообщений
// ***********************************
static void text_editor_app_input_callback(InputEvent* input_event, void* ctx) {
    furi_assert(ctx);

    // Заполняем очередь сообщений
    FuriMessageQueue* event_queue = ctx;
    furi_message_queue_put(event_queue, input_event, FuriWaitForever);
}

// **************************
// Функция выделения ресурсов
// **************************
TextEditorApp* text_editor_app_alloc() {
    TextEditorApp* app = malloc(sizeof(TextEditorApp));

    app->view_port = view_port_alloc();
    app->event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));

    // Подключаем обработчики
    view_port_draw_callback_set(app->view_port, text_editor_app_draw_callback, app);
    view_port_input_callback_set(app->view_port, text_editor_app_input_callback, app->event_queue);

    app->gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(app->gui, app->view_port, GuiLayerFullscreen);

    return app;
}

// *****************************
// Функция освобождения ресурсов
// *****************************
void text_editor_app_free(TextEditorApp* app) {
    furi_assert(app);

    // Закрываем Вьюпорт
    view_port_enabled_set(app->view_port, false);
    gui_remove_view_port(app->gui, app->view_port);
    view_port_free(app->view_port);

    // Закрываем очередь сообщений
    furi_message_queue_free(app->event_queue);

    furi_record_close(RECORD_GUI);
}

// ******************
// Основная программа
// ******************
int32_t text_editor_app(void* p) {
    UNUSED(p);

    // Выделяем ресурсы
    TextEditorApp* app = text_editor_app_alloc();

    InputEvent event;

    while(1) {
        // Считываем очередь...
        if(furi_message_queue_get(app->event_queue, &event, 100) == FuriStatusOk) {
            DrawMode mode = app->draw_mode;

            // ...и обрабатываем нажатия кнопок
            if((event.type == InputTypeLong || (event.type == InputTypePress && mode == 0)) && event.key == InputKeyBack)
                break; // Если нажали Назад на первом экране или зажали - выход из программы
            else if(event.type == InputTypePress) {
                if(event.key == InputKeyBack) { // Если нажали Назад - возвращаемся в предыдущий режим
                    app->draw_mode = (mode - 1 + TOTAL_DRAW_MODES) % TOTAL_DRAW_MODES;
                } else if(event.key == InputKeyOk) { // Если нажали ОК - идем в следующий режим
                    app->draw_mode = (mode + 1 + TOTAL_DRAW_MODES) % TOTAL_DRAW_MODES;
                }
            }

            view_port_update(app->view_port);
        }
    }

    // Освобождаем ресурсы
    text_editor_app_free(app);

    return 0;
}