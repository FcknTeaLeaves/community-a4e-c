#pragma once
    #include <Windows.h>

    static WCHAR* files[] = {L"entry.lua",L"A-4E-C.lua",L"Cockpit/Scripts/EFM_Data_Bus.lua",L"Entry/Suspension.lua",L"EFM_BETA_1_BUILD_DO_NOT_DISTRIBUTE.lua"};

    static BYTE hashes[][32] = {
	{0xc8,0xc8,0x3e,0xf8,0x38,0xfe,0x0a,0x0d,0xa8,0x77,0xe7,0x21,0x9c,0x23,0x8b,0x21,0xbd,0x5d,0x58,0x37},
	{0xee,0x40,0xb6,0x8f,0xf8,0x23,0x4f,0x6b,0xec,0x48,0x2c,0x4e,0xab,0xca,0x9a,0xb5,0x8a,0x94,0x21,0x98},
	{0xe7,0xa9,0xda,0x88,0xed,0x00,0x8b,0x21,0x51,0x29,0xf6,0xd4,0xb3,0xad,0x64,0x67,0xe3,0xb5,0xf0,0x16},
	{0x01,0x6c,0xa1,0xcd,0x92,0x2c,0x41,0x4c,0xe4,0x45,0xe2,0x09,0x78,0xa5,0x5e,0x26,0x14,0xa6,0xcc,0x70},
	{0xc5,0x45,0xa1,0xec,0x92,0x4b,0x49,0x9a,0x55,0xa0,0x8e,0xbe,0xa2,0x0c,0x6c,0x35,0x2f,0xb9,0xf0,0xa6}
};
    