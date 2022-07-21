#include "XOPStandardHeaders.r"

resource 'vers' (1) {						// XOP version info.
	0x01, 0x20, release, 0x00, 0,			// version bytes and country integer.
	"1.20",
	"1.20, Copyright 1993-2018 WaveMetrics, Inc., all rights reserved."
};

resource 'vers' (2) {						// Igor version info.
	0x08, 0x000, release, 0x00, 0,			// Version bytes and country integer.
	"8.00",
	"(for Igor 8.00 or later)"
};

// Custom error messages
resource 'STR#' (1100) {
	{
		/* [1] */
		"OpticalFlow requires Igor Pro 8.00 or later.",
	}
};

// No menu item

resource 'XOPI' (1100) {
	XOP_VERSION,							// XOP protocol version.
	DEV_SYS_CODE,							// Code for development system used to make XOP
	XOP_FEATURE_FLAGS,						// Tells Igor about XOP features
	XOPI_RESERVED,							// Reserved - must be zero.
	XOP_TOOLKIT_VERSION,					// XOP Toolkit version.
};

resource 'XOPC' (1100) {
	{
		"OpticalFlowFarneback",								// Name of operation.
		waveOP+XOPOp+compilableOp,			// Operation's category.
	}
};


