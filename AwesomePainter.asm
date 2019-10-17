TITLE Windows Painter Application
.386
.model flat, stdcall
option casemap : none

INCLUDE     windows.inc
INCLUDE     user32.inc
INCLUDE     kernel32.inc
INCLUDE     gdi32.inc
INCLUDELIB  user32.lib
INCLUDELIB  kernel32.lib
INCLUDELIB  gdi32.lib

NULL EQU 0
MAIN_WINDOW_STYLE = WS_VISIBLE + WS_DLGFRAME + WS_CAPTION + WS_BORDER + WS_SYSMENU \
+ WS_MAXIMIZEBOX + WS_MINIMIZEBOX + WS_THICKFRAME

; ==================== DATA ====================== =
.data
GreetTitle BYTE "Main Window Active", 0

CloseMsg   BYTE "Bye", 0

ErrorTitle  BYTE "Error", 0
WindowTitle  BYTE "ASM Painter", 0
WindowClass BYTE "ASMWin", 0; 窗口类，名字可以随便指定

hInstG DWORD ?

; =================== CODE ======================== =
.code
; 初始化->注册窗口->创建窗口->显示->事件循环
WinMain PROC hInst : HINSTANCE, hPrevInst : HINSTANCE, nShowCmd : DWORD
	local MainWin : WNDCLASSEX
	local hWnd : HWND
	local msg : MSG

	; 初始化
	.IF !hPrevInst
		mov MainWin.cbSize, SIZEOF WNDCLASSEX
		mov MainWin.style, CS_HREDRAW or CS_VREDRAW ; 此窗口风格使得窗口尺寸改变时将重绘
		mov MainWin.cbClsExtra, 0
		mov MainWin.cbWndExtra, 0
		mov MainWin.lpfnWndProc, OFFSET WndProc
		mov eax, hInst
		mov MainWin.hInstance, eax

		; 加载程序光标和图标等
		INVOKE LoadIcon, hInst, IDI_APPLICATION
		mov MainWin.hIcon, eax
		mov MainWin.hIconSm, eax
		INVOKE LoadCursor, 0, IDC_ARROW
		mov MainWin.hCursor, eax
		mov MainWin.hbrBackground, COLOR_WINDOW + 1
		mov MainWin.lpszClassName, OFFSET WindowClass

		; 加载菜单
		mov MainWin.lpszMenuName, 0 ; 窗口暂时还没有有菜单


		; 注册窗口类
		INVOKE RegisterClassEx, ADDR MainWin
		.IF eax == 0
			call ErrorHandler
			jmp Exit_Program
		.ENDIF
	.ENDIF

	; 创建程序窗口
	; 在这里会传递WM_CREATE信息
	INVOKE CreateWindowEx, 0, ADDR WindowClass,
	ADDR WindowTitle, MAIN_WINDOW_STYLE, ; MAIN_WINDOW_STYLE在GraphicWin.inc定义
	100, 100, 800, 600, ; 窗口默认长宽位置
	NULL, NULL, hInst, NULL
	mov hWnd, eax

	.IF eax == 0
		call ErrorHandler
		jmp  Exit_Program
	.ENDIF

	; 显示窗口，第一次显示需要随后调用UpdateWindow立即更新
	INVOKE ShowWindow, hWnd, nShowCmd
	INVOKE UpdateWindow, hWnd
Message_Loop:
	; 开始程序事件循环
	; Get next message from the queue.
	INVOKE GetMessage, ADDR msg, NULL, NULL, NULL

	; Quit if no more messages.
	.IF eax == 0
		jmp Exit_Program
	.ENDIF

	; DispatchMessage将消息传给WinProc
	INVOKE TranslateMessage, ADDR msg
	INVOKE DispatchMessage, ADDR msg
	jmp Message_Loop

Exit_Program :
	ret
WinMain ENDP

; ------------------------------------------------------
WndProc PROC uses ebx edi esi,
	hWnd:DWORD, localMsg : DWORD, wParam : DWORD, lParam : DWORD
	local hDC : HDC, hMemDC : HDC
	local ps : PAINTSTRUCT

	mov eax, localMsg

	.IF eax == WM_CREATE; create window
		 
	; 在WM_PAINT里用BeginPaint和EndPaint包裹绘制操作
	; 在WM_PAINT之外需要用GetDC方法
	.ELSEIF eax == WM_PAINT
		 INVOKE BeginPaint, hWnd, ADDR ps
		 mov hDC, eax
		 INVOKE TextOutA, hDC, 0, 0, OFFSET GreetTitle, 18
		 INVOKE EndPaint, hWnd, ADDR ps
	.ELSEIF eax == WM_LBUTTONDOWN; mouse butto
		 
	.ELSEIF eax == WM_KEYDOWN; key down
		 
	.ELSEIF eax == WM_CLOSE; close window
		 INVOKE MessageBox, hWnd, ADDR CloseMsg,
		 ADDR WindowTitle, MB_OK
		 INVOKE PostQuitMessage, 0
	.ELSE; other message
		 INVOKE DefWindowProc, hWnd, localMsg, wParam, lParam
	.ENDIF

	ret
WndProc ENDP

; ------------------------------------------------------
ErrorHandler PROC
; 当可能出现错误时，调用错误处理函数显示错误信息
.data
	pErrorMsg  DWORD ? ; ptr to error message
	messageID  DWORD ?
.code
	INVOKE GetLastError; Returns message ID in EAX
	mov messageID, eax

	; Get the corresponding message string.
	INVOKE FormatMessage, FORMAT_MESSAGE_ALLOCATE_BUFFER + \
	FORMAT_MESSAGE_FROM_SYSTEM, NULL, messageID, NULL,
	ADDR pErrorMsg, NULL, NULL

	; Display the error message.
	INVOKE MessageBox, NULL, pErrorMsg, ADDR ErrorTitle,
	MB_ICONERROR + MB_OK

	; Free the error message string.
	INVOKE LocalFree, pErrorMsg
	ret
ErrorHandler ENDP

_start PROC
	INVOKE  GetModuleHandle, NULL
	mov     hInstG, eax
	INVOKE  WinMain, hInstG, NULL, SW_SHOWDEFAULT
	INVOKE  ExitProcess, eax
_start ENDP

END _start
