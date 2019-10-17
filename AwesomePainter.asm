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
WindowClass BYTE "ASMWin", 0; �����࣬���ֿ������ָ��

hInstG DWORD ?

; =================== CODE ======================== =
.code
; ��ʼ��->ע�ᴰ��->��������->��ʾ->�¼�ѭ��
WinMain PROC hInst : HINSTANCE, hPrevInst : HINSTANCE, nShowCmd : DWORD
	local MainWin : WNDCLASSEX
	local hWnd : HWND
	local msg : MSG

	; ��ʼ��
	.IF !hPrevInst
		mov MainWin.cbSize, SIZEOF WNDCLASSEX
		mov MainWin.style, CS_HREDRAW or CS_VREDRAW ; �˴��ڷ��ʹ�ô��ڳߴ�ı�ʱ���ػ�
		mov MainWin.cbClsExtra, 0
		mov MainWin.cbWndExtra, 0
		mov MainWin.lpfnWndProc, OFFSET WndProc
		mov eax, hInst
		mov MainWin.hInstance, eax

		; ���س������ͼ���
		INVOKE LoadIcon, hInst, IDI_APPLICATION
		mov MainWin.hIcon, eax
		mov MainWin.hIconSm, eax
		INVOKE LoadCursor, 0, IDC_ARROW
		mov MainWin.hCursor, eax
		mov MainWin.hbrBackground, COLOR_WINDOW + 1
		mov MainWin.lpszClassName, OFFSET WindowClass

		; ���ز˵�
		mov MainWin.lpszMenuName, 0 ; ���������в˵�


		; ע�ᴰ����
		INVOKE RegisterClassEx, ADDR MainWin
		.IF eax == 0
			call ErrorHandler
			jmp Exit_Program
		.ENDIF
	.ENDIF

	; �������򴰿�
	; ������ᴫ��WM_CREATE��Ϣ
	INVOKE CreateWindowEx, 0, ADDR WindowClass,
	ADDR WindowTitle, MAIN_WINDOW_STYLE, ; MAIN_WINDOW_STYLE��GraphicWin.inc����
	100, 100, 800, 600, ; ����Ĭ�ϳ���λ��
	NULL, NULL, hInst, NULL
	mov hWnd, eax

	.IF eax == 0
		call ErrorHandler
		jmp  Exit_Program
	.ENDIF

	; ��ʾ���ڣ���һ����ʾ��Ҫ������UpdateWindow��������
	INVOKE ShowWindow, hWnd, nShowCmd
	INVOKE UpdateWindow, hWnd
Message_Loop:
	; ��ʼ�����¼�ѭ��
	; Get next message from the queue.
	INVOKE GetMessage, ADDR msg, NULL, NULL, NULL

	; Quit if no more messages.
	.IF eax == 0
		jmp Exit_Program
	.ENDIF

	; DispatchMessage����Ϣ����WinProc
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
		 
	; ��WM_PAINT����BeginPaint��EndPaint�������Ʋ���
	; ��WM_PAINT֮����Ҫ��GetDC����
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
; �����ܳ��ִ���ʱ�����ô���������ʾ������Ϣ
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