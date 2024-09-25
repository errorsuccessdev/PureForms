#pragma once
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <stdbool.h>
#include <assert.h>
#include <commctrl.h>

#define NOT !
#define wchar wchar_t

#define WINDOW_CLASS_NAME L"PureForms"

typedef void (*EventHandler_OnClick) (void* this);

typedef struct structEventHandlers
{
    EventHandler_OnClick OnClick;
} EventHandlers;

typedef struct structForm
{
    HWND hWnd;
    int x;
    int y;
    int width;
    int height;
    wchar* title;
    EventHandlers* eventHandlers;
} Form;

typedef struct structButton
{
    HWND hWnd;
    int id;
    int x;
    int y;
    int width;
    int height;
    wchar* text;
    EventHandlers* eventHandlers;
} Button;

Form* createForm(
    int x, int y, int width, int height, wchar* title
);

Button* createButton(
    int x, int y, int width, int height, wchar* text
);

bool showForm(Form* form, int showCommand);
