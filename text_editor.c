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
    DrawMode mode = app->draw_mode;

    // Очищаем холст
    canvas_clear(canvas);

    if(mode == START) { // РЕЖИМ - Стартовый экран
        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str(canvas, 8, 8, "Text Viewer");
        elements_button_center(canvas, "File");
        elements_button_left(canvas, "About");
        elements_button_right(canvas, "Usage");

    } else if(mode == VIEWING) { // РЕЖИМ - Просмотр файла
        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str(canvas, 8, 8, "Text Viewer");
        canvas_set_font(canvas, FontSecondary);
        canvas_draw_str(canvas, 4, 20, "Mode: Viewing");

    } else if(mode == EDITING) { // РЕЖИМ - Редактирование файла
        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str(canvas, 8, 8, "Text Viewer");
        canvas_set_font(canvas, FontSecondary);
        canvas_draw_str(canvas, 4, 20, "Mode: Editing");

    } else if(mode == ABOUT) { // РЕЖИМ - О программе
        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str(canvas, 8, 8, "Text Viewer");
        canvas_set_font(canvas, FontSecondary);
        elements_multiline_text_aligned(canvas, 0, 14, AlignLeft, AlignTop, "Author: Fortunate \nVersion: 1.0 \nBuild: 06.06.2024");
    
    } else if(mode == INSTRUCTION) { // РЕЖИМ - Инструкция по использованию
        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str(canvas, 8, 8, "Instruction");
        canvas_set_font(canvas, FontSecondary);
        elements_multiline_text_aligned(canvas, 0, 14, AlignLeft, AlignTop, "1.  Choose file \n2. View and choose edit point \n3. Edit & save");

    } else { // Обработка ошибок, РЕЖИМ неопознан
        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str(canvas, 43, 20, "ERROR!");
        canvas_set_font(canvas, FontSecondary);
        canvas_draw_str(canvas, 23, 35, "Unrecognized mode");
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

    // Закрываем порт просмотра
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

            // ...и обрабатываем нажатия кнопок:
            // Если зажали Назад или нажали Назад на первом экране - выход из программы
            if((event.type == InputTypeLong || (event.type == InputTypePress && mode == 0)) && event.key == InputKeyBack) 
                break; 

            if(event.type == InputTypePress) {

                if(event.key == InputKeyBack && (mode == 3 || mode == 4)) { 
                    app->draw_mode = (mode = 0);
                    
                // Если нажали Назад - возвращаемся в предыдущий режим
                } else if(event.key == InputKeyBack && mode > 0) { 
                    app->draw_mode = (mode - 1);
                    
                // Если нажали ОК - идем в следующий режим
                } else if(event.key == InputKeyOk && mode < 2) { 
                    app->draw_mode = (mode + 1);

                // Если нажали Left - О программе
                } else if(event.key == InputKeyLeft && mode == 0) { 
                    app->draw_mode = (mode=3);

                // Если нажали Right - Инструкция, как пользоваться
                } else if(event.key == InputKeyRight && mode == 0) { 
                    app->draw_mode = (mode=4);

                // Если нажали Up - для отладочных целей (впоследствии удалить!!!)
                } else if(event.key == InputKeyUp && mode == 0) { 
                    app->draw_mode = (mode=7);
                }
            }

            // Обновляем экран
            view_port_update(app->view_port);
        }
    }

    // Освобождаем ресурсы
    text_editor_app_free(app);

    return 0;
}