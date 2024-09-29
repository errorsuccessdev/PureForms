#pragma once
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <stdbool.h>
#include <assert.h>
#include <commctrl.h>
#include <windowsx.h>

////////////////////////////////////////////////////////////////////////////
// Constants and defines
////////////////////////////////////////////////////////////////////////////

#define NOT !
typedef wchar_t wchar;
typedef unsigned char u8;

////////////////////////////////////////////////////////////////////////////
// Macros
////////////////////////////////////////////////////////////////////////////

/// Retreives a ointer to the control "base class" from a control
#define getControl(_a) &((_a)->control)

////////////////////////////////////////////////////////////////////////////
// Form "class"
////////////////////////////////////////////////////////////////////////////

typedef struct structForm Form;

/// @brief Form event handler prototype
typedef void (*FormEventHandler) (Form* this, void* eventData);

/// @brief Form event handlers
typedef struct structFormEventHandlers
{
    FormEventHandler OnClick;
    FormEventHandler OnClose;
} FormEventHandlers;

/// @brief Form event types
typedef enum enumFormEvent 
{ 
    FormEvent_OnClick,
    FormEvent_OnClose 
} FormEvent;

/// @brief The form struct
typedef struct structForm
{
    HWND hWnd;
    int x;
    int y;
    int width;
    int height;
    wchar* title;
    FormEventHandlers eventHandlers;
} Form;

/// @brief Creates a new form
/// @param x The starting X position of the form
/// @param y The starting y position of the form
/// @param width The width
/// @param height The height
/// @param title The form's title
/// @return A pointer to the newly created Form
Form* createForm(
    int x, int y, int width, int height, wchar* title
);

/// @brief Shows the form on the screen and runs the main message loop
/// @param form The form to be shown
/// @param showCommand The show command from winmain
void showForm(
    Form* form, int showCommand
);

/// @brief Register an event handler for a form
/// @param form The form to register the event handler to
/// @param event The event type to register for
/// @param eventHandler The event handler to call on that event
void addFormEventHandler(
    Form* form, FormEvent event, FormEventHandler eventHandler
);

/// @brief centers a window on the desktop
/// @param hWnd a handle to the window to center
void centerForm(Form* form);

////////////////////////////////////////////////////////////////////////////
// Control "class"
////////////////////////////////////////////////////////////////////////////

typedef struct structControl Control;

/// @brief Control event handler function prototype
typedef void (*ControlEventHandler) (Control* this, void* eventData);

/// @brief Control event handlers
typedef struct structControlEventHandlers
{
    ControlEventHandler OnClick;
    ControlEventHandler OnHover;
} ControlEventHandlers;

/// @brief Control event types
typedef enum enumControlEvent
{
    ControlEvent_OnClick,
    ControlEvent_OnHover
} ControlEvent;

/// @brief A tooltip window with tool info
typedef struct structTooltip
{
    HWND hWnd;
    TTTOOLINFOW toolInfo;
} Tooltip;

/// @brief The "parent" control struct for all controls
typedef struct structControl
{
    HWND hWnd;
    int x;
    int y;
    int width;
    int height;
    ControlEventHandlers eventHandlers;
    Tooltip tooltip;
} Control;

/// @brief Register an event handler for a control
/// @param control The control to register the event handler to
/// @param event The event type to register for
/// @param eventHandler The event handler to call on that event
void addControlEventHandler(
    Control* control, 
    ControlEvent event,
    ControlEventHandler eventHandler
);

/// @brief Creates a tooltip for the specified control
/// @param control The control to associate with the tooltip
/// @param text The tooltip text
void createTooltip(
    Control* control, wchar* text
);

////////////////////////////////////////////////////////////////////////////
// Button "class"
////////////////////////////////////////////////////////////////////////////

/// @brief The button struct, which "inherits" control
typedef struct structButton
{
    Control control;
    int id;
    wchar* text;
} Button;

/// @brief Creates a new button on the *TODO* specified form
/// @param x The X position of the button
/// @param y The Y position of the button
/// @param width The width
/// @param height The height
/// @param text The button's text
/// @param isDefault Whether or not the button should be set as default
/// @return A pointer to the created button
Button* createButton(
    int x, int y, int width, int height, 
    wchar* text, bool isDefault
);

////////////////////////////////////////////////////////////////////////////
// Bitmap "class"
////////////////////////////////////////////////////////////////////////////

/// @brief The Bitmap "class"
typedef struct structBitmap
{
    Control control;
    HBITMAP hBitmap;
    wchar* filename;
    u8 red;
    u8 green;
    u8 blue;
} Bitmap;

/// @brief Create a new bitmap to be shown at the specified position
/// @param x The X position
/// @param y The Y position
/// @param width The width
/// @param height The height
/// @param filepath The file path to load the bitmap form
/// @return A pointer to the created bitmap
Bitmap* createBitmapFromFile(
    int x, int y, int width, int height, wchar* filepath
);

/// @brief Create a new bitmap to be shown at the specified position
/// @param x The X position
/// @param y The Y position
/// @param width The width
/// @param height The height
/// @param red The desired red value
/// @param green The desired green value
/// @param blue The desired blue value
/// @return A pointer to the created bitmap
Bitmap* createBitmapFromRGB(
    int x, int y, int width, int height, 
    u8 red, u8 green, u8 blue
);

/// @brief Sets the bitmap file used to show a bitmap
/// @param newFilePath The file path of the new bitmap file
/// @param bitmap The bitmap to load the file into
/// @return None
void setBitmapFile(
    Bitmap* bitmap, wchar* newFilePath
);

/// @brief Sets the RGB color on the bitmap
/// @param bitmap The bitmap to set the color on
/// @param red The new red value
/// @param green The new green value
/// @param blue The new blue value
/// @return None
void setBitmapRGB(
    Bitmap* bitmap, u8 red, u8 green, u8 blue
);

////////////////////////////////////////////////////////////////////////////
// EventData structs
////////////////////////////////////////////////////////////////////////////

/// @brief Event data for click event
typedef struct structEventDataOnClick
{
    wchar* text; /// The text of the clicked control
} EventData_OnClick;

/// @brief Event data for close event
typedef struct structEventDataOnClose
{
    bool shouldClose; /// Whether or not PureForms should close the form
} EventData_OnClose;