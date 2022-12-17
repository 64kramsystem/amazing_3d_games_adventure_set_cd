// Header file for Windows implementation of the ACK-3D engine

// Uncomment next line to use normal windows colors
//#define _USE_SYSTEM_PALETTE 1


short InitACKEngine(void);
void AckPaint(HWND hWnd,HDC hdc); // Actual paint routine for 3D Window
void ProcessKeys(UINT iMessage,WPARAM wParam);
void ProcessMouse(HWND hWnd,short nType,short xPos,short yPos);
void AckDoSize(HWND hWnd,LPARAM lParam);
void CreateAckPalette(void);

