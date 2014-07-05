#pragma once

#include "Core.h"

typedef uint WindowFlags;

// See SDL_video.h
const WindowFlags WINDOW_FULLSCREEN = 0x00000001;         /**< fullscreen window */
const WindowFlags WINDOW_OPENGL = 0x00000002;             /**< window usable with OpenGL context */
const WindowFlags WINDOW_SHOWN = 0x00000004;              /**< window is visible */
const WindowFlags WINDOW_HIDDEN = 0x00000008;             /**< window is not visible */
const WindowFlags WINDOW_BORDERLESS = 0x00000010;         /**< no window decoration */
const WindowFlags WINDOW_RESIZABLE = 0x00000020;          /**< window can be resized */
const WindowFlags WINDOW_MINIMIZED = 0x00000040;          /**< window is minimized */
const WindowFlags WINDOW_MAXIMIZED = 0x00000080;          /**< window is maximized */
const WindowFlags WINDOW_INPUT_GRABBED = 0x00000100;      /**< window has grabbed input focus */
const WindowFlags WINDOW_INPUT_FOCUS = 0x00000200;        /**< window has input focus */
const WindowFlags WINDOW_MOUSE_FOCUS = 0x00000400;        /**< window has mouse focus */
const WindowFlags WINDOW_FULLSCREEN_DESKTOP = (WINDOW_FULLSCREEN | 0x00001000);
const WindowFlags WINDOW_FOREIGN = 0x00000800;            /**< window not created by SDL */
const WindowFlags WINDOW_ALLOW_HIGHDPI = 0x00002000;      /**< window should be created in high-DPI mode if supported */