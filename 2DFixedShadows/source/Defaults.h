#pragma once

namespace Defaults {
	// Window width - applies only if in windowed mode.
	const unsigned int setting_window_width = 640;

	// Window height - applies only if in windowed mode.
	const unsigned int setting_window_height = 640;

	// Vertical retrace - limits framerate to display refresh rate, greatly reduces vertical tearing.
	const bool setting_vertical_retrace = true;

	// Fullscreen - draws window over entire screen, stretching resolution.
	const bool setting_fullscreen_mode = false;

	// Fullscreen width - applies only if in fullscreen mode.
	const unsigned int setting_fullscreen_width = 640;

	// Fullscreen height - applies only if in fullscreen mode.
	const unsigned int setting_fullscreen_height = 480;

	// Native-fullscreen force - Overrides settings by enabling fullscreen and changing the fullscreen resolution to match the display.
	const bool setting_native_fullscreen_force = true;
}
