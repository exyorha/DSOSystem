#ifndef DSO__DSO_KEYS__H
#define DSO__DSO_KEYS__H

#include <stdint.h>

enum DSOKeys : uint32_t {
	EncProgDec,
	EncProgInc,
	KeySaveRecall,
	KeyMeasure,
	KeyAcquire,
	KeyRunStop,
	KeyUtility,
	KeyCursor,
	KeyDisplay,
	KeyHardcopy,
	KeyAutoset,
	KeyF1,
	EncCH1PosDec,
	EncCH1PosInc,
	EncCH2PosDec,
	EncCH2PosInc,
	EncHorPosDec,
	EncHorPosInc,
	EncTrigLvlDec,
	EncTrigLvlInc,
	KeyMath,
	KeyCH1Menu,
	KeyCH2Menu,
	KeyHorMenu,
	KeyTrigMenu,
	EncCH1VDivDec,
	EncCH1VDivInc,
	EncCH2VDivDec,
	EncCH2VDivInc,
	EncSecDivDec,
	EncSecDivInc,
	KeyLevel50Percent,
	KeyForceTrig,
	KeyTrigView,
	KeyF2,
	KeyF3,
	KeyF4,
	KeyF5
};

#endif
