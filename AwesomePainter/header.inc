;including
include		  	windows.inc
include		  	gdi32.inc
include 	  	user32.inc
include 	  	kernel32.inc
include 		comdlg32.inc
include 		comctl32.inc
include			msvcrt.inc
include			definition.inc
include 		data.inc
includelib 	  	gdi32.lib
includelib	  	user32.lib
includelib    	kernel32.lib
includelib		comdlg32.lib
includelib		comctl32.lib
includelib		msvcrt.lib

;Function PROTOTYPES

ErrorHandler        PROTO
Enable              PROTO, ID: DWORD
Disable             PROTO, ID: DWORD
Clear               PROTO, hWnd : HWND
Open                PROTO, hWnd:HWND
Save                PROTO, hWnd:HWND
FilpHorizontal      PROTO, hWnd:HWND
FilpVertical        PROTO, hWnd:HWND
RotateAntiClockWise PROTO, hdcDest: DWORD, centerXPos: DWORD, centerYPos: DWORD, angle: DWORD, hdcSrc: DWORD, xSrc: DWORD, ySrc: DWORD, srcWidth: DWORD, srcHeight: DWORD
saveImage           PROTO, hWnd:DWORD
Undo                PROTO, hWnd:DWORD
rotateCanvas        PROTO, hWnd:DWORD, uMsg:DWORD, wParam:DWORD, lParam:DWORD
textDialog          PROTO, hWnd:DWORD, uMsg:DWORD, wParam:DWORD, lParam:DWORD
changeColor         PROTO, hWnd:DWORD, uMsg:DWORD, wParam:DWORD, lParam:DWORD
changeWidth         PROTO, hWnd:DWORD, uMsg:DWORD, wParam:DWORD, lParam:DWORD
changeStyle         PROTO, hWnd:DWORD, uMsg:DWORD, wParam:DWORD, lParam:DWORD
changeBRStyle       PROTO, hWnd:DWORD, uMsg:DWORD, wParam:DWORD, lParam:DWORD