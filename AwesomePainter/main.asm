.386
.model flat, stdcall
option casemap: none

include	header.inc
include utils.inc
include paint.inc
include transform.inc

.code
ProcWinMain proc uses ebx edi esi, hWnd, uMsg, wParam, lParam		  
	local 	hDC : HDC, hMemDC : HDC
	local 	ps : PAINTSTRUCT
	local 	m_rect : RECT
	local 	bmp:BITMAP
	local   tmpBrsh: DWORD
	local 	selecthbmp:HBITMAP
	local	selectColor: DWORD
	local   startangleF : REAL4
	local   sweepangleF : REAL4
	
	mov eax, uMsg
	.if eax == WM_CREATE
		invoke  CreateSolidBrush, 0
		mov     hBrsh, eax
		invoke  CreatePen, PenStyle, PenWidth, PenColor
		mov     hPen, eax
		invoke  CreatePen, PS_SOLID, 5, 0ffffffH
		mov     hEraser, eax
		push    hPen	; 初始先用画笔
		pop     PenLikeTool
		invoke LoadCursor, hInstance, IDC_BRUSH
		invoke SetCursor, eax
		mov LastCursor, IDC_BRUSH
        
	.elseif eax == WM_COMMAND
		invoke Enable, IDM_CUSTOM
		mov	eax, wParam
		.if ax == IDM_NEW
			.if ischanged==1
				mov		ischanged,0
				invoke MessageBox, hWinMain, offset changedWindow, offset changedWindowTitle, MB_YESNO
				.if eax == IDYES
					invoke 	Save, hWnd
					invoke 	Clear, hWnd
				.else
					invoke 	Clear, hWnd
				.endif
			.else
				mov		ischanged,0
				invoke 	Clear, hWnd
			.endif
		.elseif	ax == IDM_OPEN
			.if ischanged==1
				mov		ischanged,0
				invoke MessageBox, hWinMain, offset changedWindow, offset changedWindowTitle, MB_YESNO
				.if eax == IDYES
					invoke 	Save, hWnd
					invoke	Open, hWnd
				.else
					invoke	Open, hWnd
				.endif
			.else
				mov		ischanged,0
				invoke	Open, hWnd
			.endif
               invoke saveImage, hWnd
		.elseif ax == IDM_SAVE
			invoke Save, hWnd
			mov		ischanged,0
        .elseif ax == IDM_UNDO
			invoke Undo, hWnd
			mov ischanged,1
        .elseif ax == IDM_FLIPH
			invoke FilpHorizontal, hWnd
			invoke saveImage, hWnd
			mov ischanged,1
        .elseif ax == IDM_FLIPV
			invoke FilpVertical, hWnd
			invoke saveImage, hWnd
			mov ischanged,1
        .elseif ax == IDM_CLEAR
			invoke 	Clear, hWnd
			mov ischanged,1
        .elseif ax == IDM_CSIZE
			mov ischanged,1
			invoke DialogBoxParam,hInstance, IDD_SETSIZEDIALOG,hWnd,addr SetSize,0
        .elseif ax == IDM_ROTATION
			push hWnd
			pop hWndGlobal
            mov ischanged, 1
			invoke DialogBoxParam,hInstance,IDD_ROTATEDIALOG,hWnd,addr rotateCanvas,0		
        .elseif ax == IDM_PEN
			invoke LoadCursor, hInstance, IDC_BRUSH
			invoke SetCursor, eax
			mov LastCursor, IDC_BRUSH
			mov  isDraw, TRUE
			push DRAW_PEN
			pop  DrawToolSelect
			push hPen
			pop  PenLikeTool
			invoke SendMessage, hStatusBar,SB_SETTEXT, 2,offset penString
			invoke  wsprintfA, offset penClr, offset colorFmt, PenColor
			invoke	SendMessage, hStatusBar, SB_SETTEXT, 4, offset penClr
			invoke  wsprintfA, offset penWth, offset widthFmt, PenWidth
			invoke  SendMessage, hStatusBar, SB_SETTEXT, 3, offset penWth
        .elseif ax == IDM_BRUSH
			invoke LoadCursor, hInstance, IDC_BRUSH
			invoke SetCursor, eax
			mov LastCursor, IDC_BRUSH
			invoke Disable, IDM_CUSTOM
			mov  isDraw, TRUE
			push DRAW_BRUSH
			pop  DrawToolSelect
			push hBrsh
			pop  PenLikeTool
			invoke SendMessage, hStatusBar,SB_SETTEXT, 2,offset brushString
			invoke SendMessage, hStatusBar,SB_SETTEXT, 3,offset nullstring
			invoke  wsprintfA, offset penClr, offset colorFmt, PenColor
			invoke	SendMessage, hStatusBar, SB_SETTEXT, 4, offset penClr
			invoke  wsprintfA, offset shapeStr, offset shapeFmt, offset lineStr
			invoke	SendMessage, hStatusBar, SB_SETTEXT, 6, offset shapeStr
        .elseif ax == IDM_TEXT
			invoke LoadCursor, hInstance, IDC_TEXT
			invoke SetCursor, eax
			mov LastCursor, IDC_TEXT
			mov  isDraw, TRUE
			push DRAW_TEXT
			pop  DrawToolSelect
			invoke SendMessage, hStatusBar,SB_SETTEXT, 2,offset textString
        .elseif ax == IDM_ERASER
			invoke LoadCursor, hInstance, IDC_ERASER
			invoke SetCursor, eax
			mov LastCursor, IDC_ERASER
			mov  isDraw, TRUE
			push DRAW_ERASER
			pop  DrawToolSelect
			push hEraser
			pop  PenLikeTool
			invoke SendMessage, hStatusBar,SB_SETTEXT, 2, offset eraserString
			invoke  wsprintfA, offset penClr, offset colorFmt, PenColor
			invoke	SendMessage, hStatusBar, SB_SETTEXT, 4, offset penClr
			invoke  wsprintfA, offset penWth, offset widthFmt, PenWidth
			invoke  SendMessage, hStatusBar, SB_SETTEXT, 3, offset penWth
        .elseif ax == IDM_OILPAINT
			invoke LoadCursor, hInstance, IDC_BUCKET
			invoke SetCursor, eax
			mov LastCursor, IDC_BUCKET
			mov  isDraw, TRUE
			push DRAW_BUCKET
			pop  DrawToolSelect
			invoke SendMessage, hStatusBar,SB_SETTEXT, 2, offset bucketString
			invoke  wsprintfA, offset penClr, offset colorFmt, PenColor
			invoke	SendMessage, hStatusBar, SB_SETTEXT, 4, offset penClr
        .elseif ax == IDM_SELECTAREA
			invoke LoadCursor, NULL, IDC_CROSS
			invoke SetCursor, eax
			mov LastCursor, IDC_CROSS
			mov  isDraw, FALSE
			mov isSelect,TRUE
			mov isStraw, FALSE
			invoke  SendMessage, hStatusBar, SB_SETTEXT, 2, offset selString
        .elseif ax == IDM_SELECTCOLOR
			invoke LoadCursor, hInstance, IDC_STRAW
			invoke SetCursor, eax
			mov LastCursor, IDC_STRAW
			mov isDraw, FALSE
			mov isSelect,FALSE
			mov isStraw, TRUE
			invoke  SendMessage, hStatusBar, SB_SETTEXT, 2, offset strawString
        .elseif ax == IDM_PAINTCOLOR
			invoke DialogBoxParam,hInstance, IDD_COLORDIALOG,hWnd,addr changeColor,0
        .elseif ax == IDM_PENSIZE
			invoke DialogBoxParam,hInstance, IDD_WIDTHDIALOG,hWnd,addr changeWidth,0
        .elseif ax == IDM_PENTYPE
			invoke DialogBoxParam,hInstance, IDD_STYLEDIALOG,hWnd,addr changeStyle,0
        .elseif ax == IDM_BRUSHTYPE
			invoke Disable, IDM_CUSTOM
			invoke DialogBoxParam,hInstance, IDD_BRSTYLEDIALOG,hWnd,addr changeBRStyle,0
        .elseif ax == IDM_LINE
			push SHAPE_LINE
			pop  ShapeSelect
			invoke  wsprintfA, offset shapeStr, offset shapeFmt, offset lineStr
			invoke	SendMessage, hStatusBar, SB_SETTEXT, 6, offset shapeStr
        .elseif ax == IDM_ELLIPSE
			push SHAPE_ELLIPSE
			pop  ShapeSelect
			invoke  wsprintfA, offset shapeStr, offset shapeFmt, offset ellipseStr
			invoke	SendMessage, hStatusBar, SB_SETTEXT, 6, offset shapeStr
        .elseif ax == IDM_RECTANGLE
			push SHAPE_RECTANGLE
			pop  ShapeSelect
			invoke  wsprintfA, offset shapeStr, offset shapeFmt, offset rectangleStr
			invoke	SendMessage, hStatusBar, SB_SETTEXT, 6, offset shapeStr
		.elseif ax == IDM_POLYGON
			push SHAPE_POLYGON
			pop  ShapeSelect
			invoke  wsprintfA, offset shapeStr, offset shapeFmt, offset polygonStr
			invoke	SendMessage, hStatusBar, SB_SETTEXT, 6, offset shapeStr
			invoke DialogBoxParam,hInstance, IDD_POLYGONDIALOG,hWnd,addr polygonNum,0
		.elseif ax == IDM_POLYLINE
			push SHAPE_POLYLINE
			pop  ShapeSelect
			invoke  wsprintfA, offset shapeStr, offset shapeFmt, offset polylineStr
			invoke	SendMessage, hStatusBar, SB_SETTEXT, 6, offset shapeStr
			invoke DialogBoxParam,hInstance, IDD_POLYLINEDIALOG,hWnd,addr polylineNum,0
		.elseif ax == IDM_ARC
			push SHAPE_ARC
			pop  ShapeSelect
			invoke  wsprintfA, offset shapeStr, offset shapeFmt, offset arcStr
			invoke	SendMessage, hStatusBar, SB_SETTEXT, 6, offset shapeStr
			invoke DialogBoxParam,hInstance, IDD_ARCDIALOG,hWnd,addr drawArc,0
		.elseif ax == IDM_CUSTOM
			push  SHAPE_DRAW
			pop   ShapeSelect
			invoke  wsprintfA, offset shapeStr, offset shapeFmt, offset customStr
			invoke	SendMessage, hStatusBar, SB_SETTEXT, 6, offset shapeStr
		.elseif ax == IDM_VERSION
			invoke MessageBox,hWinMain,addr version,addr versionTitle,MB_OK
		.endif
	.elseif eax == WM_CLOSE
		.if ischanged==1
			mov		ischanged,0
			invoke MessageBox, hWinMain, offset changedWindow, offset changedWindowTitle, MB_YESNOCANCEL
			.if eax == IDYES
				invoke 	Save, hWnd
			.elseif eax == IDNO
				invoke 	DestroyWindow, hWinMain
			.endif
		.else
			invoke 	DestroyWindow, hWinMain
		.endif
	.elseif eax == WM_LBUTTONDOWN
		mov 	ischanged, 1
		mov 	edx, isButtonDown
		.if edx == FALSE
			mov 	isButtonDown, TRUE
		.endif
		; 记录旧点位置
		.if selectok == TRUE
			;异或模式下，画去上个选取框	
			invoke GetDC,hWnd
			mov hDC, eax
			invoke SetROP2, hDC, R2_NOTXORPEN
			invoke Polyline, hDC, addr aptRect, 5
			;清零
			mov ptTmp.x ,0
			mov ptTmp.y ,0
			;把选取框要删掉（通过异或模式划线）
			mov aptRect[0].x, 0
			mov aptRect[0].y, 0
			mov aptRect[8].x, 0
			mov aptRect[8].y, 0
			mov aptRect[16].x, 0
			mov aptRect[16].y, 0
			mov aptRect[24].x, 0
			mov aptRect[24].y, 0
			mov aptRect[32].x, 0
			mov aptRect[32].y, 0
			invoke ReleaseDC, hWnd, hDC

		.endif
		mov     edx, lParam
		movzx   ebx, dx
		shr		edx, 16
		mov     g_OldPointY, edx
		mov     g_OldPointX, ebx
	.elseif eax == WM_LBUTTONUP
		.if isButtonDown == TRUE
			mov 	isButtonDown, FALSE
		.endif
		.if isDraw == TRUE
			mov     edx, lParam
			movzx   ebx, dx
			shr     edx, 16
			mov		g_NewPointX, ebx
			mov		g_NewPointY, edx
			invoke  GetDC, hWnd
			mov     hDC, eax
			.if (DrawToolSelect == DRAW_BRUSH) || (DrawToolSelect == DRAW_PEN)
				mov eax, ShapeSelect
				.if eax == SHAPE_DRAW
					invoke  SelectObject, hDC, PenLikeTool
					invoke  MoveToEx, hDC, g_OldPointX, g_OldPointY, NULL
					invoke	LineTo, hDC, g_NewPointX, g_NewPointY
					push    g_NewPointX
					pop		g_OldPointX
					push    g_NewPointY
					pop		g_OldPointY
				.elseif eax == SHAPE_LINE
					invoke  SelectObject, hDC, PenLikeTool
					invoke  MoveToEx, hDC, g_OldPointX, g_OldPointY, NULL
					invoke	LineTo, hDC, g_NewPointX, g_NewPointY
				.elseif eax == SHAPE_RECTANGLE
					invoke  SelectObject, hDC, PenLikeTool
					invoke  Rectangle, hDC, g_OldPointX, g_OldPointY, g_NewPointX, g_NewPointY
				.elseif eax == SHAPE_ARC
					invoke  BeginPath, hDC
					invoke  MoveToEx, hDC, g_NewPointX, g_NewPointY, NULL
					invoke  SelectObject, hDC, PenLikeTool
					fld1
					fidivr ArcStartA
					fstp  startangleF
					fld1
					fidivr ArcSweepA
					fstp  sweepangleF
					invoke  AngleArc, hDC, g_NewPointX, g_NewPointY, ArcRadius, startangleF, sweepangleF
					invoke  LineTo, hDC, g_NewPointX, g_NewPointY

					invoke  EndPath, hDC 
    				invoke  StrokeAndFillPath, hDC
				.elseif eax == SHAPE_ELLIPSE
					invoke  SelectObject, hDC, PenLikeTool
					invoke  Ellipse, hDC, g_OldPointX, g_OldPointY, g_NewPointX, g_NewPointY
				.elseif eax == SHAPE_POLYGON
					invoke SetPixel, hDC, g_NewPointX, g_NewPointY, 0
					mov esi, EdgeNum
					sub	esi, EdgeNumLeft
					shl	esi, 3
					mov eax, g_NewPointX
					mov	PolyPoints[esi].x, eax
					mov	eax, g_NewPointY
					mov	PolyPoints[esi].y, eax
					mov ecx, EdgeNumLeft
					sub ecx, 1
					mov EdgeNumLeft, ecx
					.if EdgeNumLeft == 0
						invoke  SelectObject, hDC, PenLikeTool
						invoke  Polygon, hDC, offset PolyPoints, EdgeNum
						mov eax, 0
						mov EdgeNum, eax
						mov EdgeNumLeft, eax
						push SHAPE_DRAW
						pop  ShapeSelect
						invoke  wsprintfA, offset shapeStr, offset shapeFmt, offset customStr
						invoke	SendMessage, hStatusBar, SB_SETTEXT, 6, offset shapeStr
					.endif
				.elseif eax == SHAPE_POLYLINE
					invoke SetPixel, hDC, g_NewPointX, g_NewPointY, 0
					mov esi, EdgeNum
					sub	esi, EdgeNumLeft
					shl	esi, 3
					mov eax, g_NewPointX
					mov	PolyPoints[esi].x, eax
					mov	eax, g_NewPointY
					mov	PolyPoints[esi].y, eax
					mov ecx, EdgeNumLeft
					sub ecx, 1
					mov EdgeNumLeft, ecx
					.if EdgeNumLeft == 0
						invoke  SelectObject, hDC, PenLikeTool
						invoke  Polyline, hDC, offset PolyPoints, EdgeNum
						mov eax, 0
						mov EdgeNum, eax
						mov EdgeNumLeft, eax
						push SHAPE_DRAW
						pop  ShapeSelect
						invoke  wsprintfA, offset shapeStr, offset shapeFmt, offset customStr
						invoke	SendMessage, hStatusBar, SB_SETTEXT, 6, offset shapeStr
					.endif
				.endif
			.elseif DrawToolSelect == DRAW_ERASER
				invoke  SelectObject, hDC, PenLikeTool
				invoke  MoveToEx, hDC, g_OldPointX, g_OldPointY, NULL
				invoke	LineTo, hDC, g_NewPointX, g_NewPointY
				push    g_NewPointX
				pop		g_OldPointX
				push    g_NewPointY
				pop		g_OldPointY
			.elseif DrawToolSelect == DRAW_TEXT
				invoke DialogBoxParam,hInstance, IDD_TEXTDIALOG,hWnd,addr textDialog, 0
			.elseif DrawToolSelect == DRAW_BUCKET
				invoke CreateSolidBrush, PenColor
				mov tmpBrsh, eax
				invoke  SelectObject, hDC, tmpBrsh
				invoke  GetPixel, hDC, g_NewPointX, g_NewPointY
				mov	selectColor, eax
				invoke  ExtFloodFill, hDC, g_NewPointX, g_NewPointY, selectColor, FLOODFILLSURFACE
				invoke DeleteObject, tmpBrsh
			.endif
			invoke  ReleaseDC, hWnd, hDC
			invoke saveImage, hWnd
		; 非画图工具
		.elseif isSelect == TRUE
			; 选区按钮
			.if selectok == TRUE
				mov selectok,FALSE
				mov     edx, lParam
				movzx   ebx, dx
				shr		edx, 16
				mov     g_NewPointY, edx
				mov     g_NewPointX, ebx
				invoke  OpenClipboard,hWinMain
				invoke  GetClipboardData,CF_BITMAP
				mov		selecthbmp,eax
				invoke  GetDC,hWinMain
				mov     hDC, eax
				;invoke  CreateCompatibleDC,hDC
				;mov     selecttmphdc,eax
				invoke  GetObject,selecthbmp,SIZEOF BITMAP,addr bmp
				;invoke  SelectObject,selecttmphdc, selecthbmp
				;invoke  SelectObject,testtmphdc, selecthbmp
				;直到这里信息也都正确
				invoke  BitBlt, hDC, g_NewPointX, g_NewPointY, bmp.bmWidth, bmp.bmHeight, selecttmphdc, 0, 0, SRCCOPY; 打开后,以固定大小贴到hDC上
				invoke  DeleteDC, selecttmphdc
				invoke ReleaseDC, hWnd, hDC
				
				invoke saveImage, hWnd
				;invoke  DeleteDC, testtmphdc
				invoke	DeleteObject, selecthbmp
				invoke  CloseClipboard
				invoke  SendMessage, hStatusBar, SB_SETTEXT, 2, offset selString
			.else;剪切板没有问题
				mov     edx, lParam
				movzx   ebx, dx
				shr		edx, 16
				mov     g_NewPointY, edx
				mov     g_NewPointX, ebx
				mov edi,g_NewPointX
				sub edi,g_OldPointX
				mov selectwidth,edi
				mov edi,g_NewPointY
				sub edi,g_OldPointY
				mov selectheight,edi
				invoke GetDC,hWnd
				mov hDC, eax
				invoke CreateCompatibleDC,hDC

				;mov testtmphdc,eax
				mov selecttmphdc,eax
				invoke CreateCompatibleBitmap,hDC,selectwidth,selectheight
				mov selecthbmp,eax
				invoke SelectObject,selecttmphdc,selecthbmp
				invoke BitBlt,selecttmphdc,0,0,selectwidth,selectheight,hDC,g_OldPointX,g_OldPointY,SRCCOPY
				;invoke SelectObject,testtmphdc,selecthbmp
				;invoke BitBlt,testtmphdc,0,0,selectwidth,selectheight,hDC,g_OldPointX,g_OldPointY,SRCCOPY
				;剪切板操作没有必要应该，但是确实再剪切板上，有待解决
				invoke OpenClipboard,hWnd
				invoke EmptyClipboard
				invoke SetClipboardData,CF_BITMAP,selecthbmp
				invoke CloseClipboard

				invoke DeleteObject,selecthbmp
				invoke DeleteDC,hDC
				mov selectok,TRUE
				
				invoke  SendMessage, hStatusBar, SB_SETTEXT, 2, offset selOKString
			.endif
		.elseif isStraw == TRUE
			invoke GetDC, hWnd
			mov  hDC, eax
			invoke GetPixel, hDC, g_OldPointX, g_OldPointY
			mov  selectColor, eax
			push selectColor
			pop  PenColor
			invoke  CreatePen, PS_SOLID, PenWidth, PenColor
			mov     hPen, eax
			.if BrushStyle == HS_SOLID
				invoke  CreateSolidBrush, PenColor
			.else
				invoke  CreateHatchBrush, BrushStyle, PenColor
			.endif
			mov	hBrsh, eax
			invoke  wsprintfA, offset penClr, offset colorFmt, PenColor
			invoke	SendMessage, hStatusBar, SB_SETTEXT, 4, offset penClr
			invoke ReleaseDC, hWnd, hDC
		.endif
	.elseif eax == WM_PAINT
		; 得到显示区域的设备描述表句柄hDC
		invoke  BeginPaint, hWnd, addr ps
		mov     hDC, eax

		invoke  CreateCompatibleDC, hDC
		mov     hMemDC, eax

		; 将位图画在"屏幕内存映像"上
		invoke  SelectObject, hMemDC, hSaveBitmap
		invoke GetClientRect, hWnd, addr m_rect
		invoke DPtoLP, hDC, addr m_rect, 2
		mov ebx, m_rect.right
		mov edx, m_rect.bottom
		sub edx, StatusBarHeight

		; 将位图从hMemDC复制到hDC中
		invoke  BitBlt, hDC, 0, 0, ebx, edx, hMemDC, 0, 0, SRCCOPY
		; 删除hMemDC
		invoke  DeleteDC, hMemDC
		; EndPaint()和BeginPaint()配对使用，使无效区域有效
		invoke  EndPaint, hWnd, addr ps
	.elseif eax == WM_MOUSEMOVE
		mov     edx, lParam
		movzx   ebx, dx
		shr     edx, 16
		mov		g_NewPointX, ebx
		mov		g_NewPointY, edx
		invoke  wsprintfA,offset mousePos,offset posFmt,g_NewPointX,g_NewPointY
		invoke SendMessage, hStatusBar,SB_SETTEXT, 1,offset mousePos
		.if isButtonDown != FALSE && isDraw == TRUE
			.if (DrawToolSelect == DRAW_PEN && ShapeSelect == SHAPE_DRAW) || \
				(DrawToolSelect == DRAW_ERASER)
				; lParam的高16位为y坐标，低16位为x坐标
				; 调用GetDC(hWnd)得到窗口区域的设备描述表句柄
				invoke  GetDC, hWnd
				mov     hDC, eax
				invoke  SelectObject, hDC, PenLikeTool
				invoke  MoveToEx, hDC, g_OldPointX, g_OldPointY, NULL
				invoke	LineTo, hDC, g_NewPointX, g_NewPointY
				invoke  ReleaseDC, hWnd, hDC
				push    g_NewPointX
				pop		g_OldPointX
				push    g_NewPointY
				pop		g_OldPointY
			.endif
		.elseif isButtonDown == TRUE && isSelect == TRUE && selectok == FALSE
			invoke GetDC,hWnd
			mov hDC, eax
			invoke SetROP2, hDC, R2_NOTXORPEN
			;在异或模式下，可以把上次化的框画掉
			.if ptTmp.x != 0
				push g_OldPointX
				pop edx
				dec edx
				mov aptRect[0].x,edx
				mov aptRect[24].x, edx
				mov aptRect[32].x, edx

				push g_OldPointY
				pop edx
				dec edx
				mov aptRect[0].y,edx
				mov aptRect[8].y, edx
				mov aptRect[32].y, edx

				
				push ptTmp.x
				pop edx
				inc edx
				mov aptRect[8].x,edx
				mov aptRect[16].x, edx

				push ptTmp.y
				pop edx
				inc edx
				mov aptRect[16].y,edx
				mov aptRect[24].y, edx

				invoke Polyline, hDC, addr aptRect, 5
			.endif
		
			push g_OldPointX
				pop edx
				dec edx
				mov aptRect[0].x,edx
				mov aptRect[24].x, edx
				mov aptRect[32].x, edx

				push g_OldPointY
				pop edx
				dec edx
				mov aptRect[0].y,edx
				mov aptRect[8].y, edx
				mov aptRect[32].y, edx

				
				push g_NewPointX
				pop edx
				inc edx
				mov aptRect[8].x,edx
				mov aptRect[16].x, edx

				push g_NewPointY
				pop edx
				inc edx
				mov aptRect[16].y,edx
				mov aptRect[24].y, edx

			invoke Polyline, hDC, addr aptRect, 5
			push g_NewPointX
			pop ptTmp.x
			push g_NewPointY
			pop ptTmp.y


			invoke ReleaseDC, hWnd, hDC
		
		.endif
	.if LastCursor != IDC_CROSS
		invoke LoadCursor, hInstance, LastCursor
		invoke SetCursor, eax
	.ELSE
		invoke LoadCursor, NULL, LastCursor
		invoke SetCursor, eax
	.endif
	.elseif eax == WM_DRAWCLIPBOARD	
	.elseif eax == WM_DRAWCLIPBOARD
	.elseif eax == WM_SIZE
		invoke  MoveWindow,hStatusBar,0,0,0,0,TRUE
	.elseif eax == WM_DESTROY
		invoke 	PostQuitMessage, NULL
	.else
		invoke 	DefWindowProc, hWnd, uMsg, wParam, lParam	
		ret
	.endif		
	xor		eax, eax;清零,返回
	ret				
ProcWinMain 	endp	

WinMain proc
	local 	@stWndClass: WNDCLASSEX
	local 	@stMsg: MSG
	local 	@hAccelerator: DWORD
				
	;注册窗口类
	invoke 	RtlZeroMemory, addr @stWndClass, sizeof @stWndClass 
	invoke 	GetModuleHandle, NULL	
	mov		hInstance, eax
									
	push hInstance
	pop	@stWndClass.hInstance	
	mov @stWndClass.cbSize, sizeof WNDCLASSEX				
	mov @stWndClass.style,  CS_SAVEBITS			
	mov @stWndClass.lpfnWndProc, offset ProcWinMain		
	mov @stWndClass.hbrBackground, COLOR_WINDOW + 1			
	mov @stWndClass.lpszClassName, offset ClassName		

	invoke 	RegisterClassEx, addr @stWndClass

	;创建窗口
	invoke 	LoadMenu, hInstance, IDM_MAIN
	mov		hMainMenu, eax
	invoke 	LoadAccelerators, hInstance, IDA_MAIN
	mov 	@hAccelerator, eax
	invoke  CreateWindowEx, 0, offset ClassName, offset WindowName, \
			WS_CLIPCHILDREN OR WS_OVERLAPPEDWINDOW and not WS_MAXIMIZEBOX and not WS_THICKFRAME, 0, 0, 800, 600, \
			NULL, hMainMenu, hInstance, NULL
	mov 	hWinMain, eax
	
	invoke  CreateStatusWindow, WS_CHILD OR WS_VISIBLE  ,offset statusText, hWinMain, NULL
	mov     hStatusBar, eax
	invoke  SendMessage,hStatusBar,SB_SETPARTS, 8,offset StatusWidth
	invoke  SendMessage, hStatusBar,SB_SETTEXT, 2,offset penString
	invoke  wsprintfA, offset penWth, offset widthFmt, PenWidth
	invoke  SendMessage, hStatusBar, SB_SETTEXT, 3, offset penWth
	invoke  wsprintfA, offset penClr, offset colorFmt, PenColor
	invoke	SendMessage, hStatusBar, SB_SETTEXT, 4, offset penClr
	invoke  wsprintfA, offset penlikeStyle, offset styleFmt, offset PENSTYLE_SOLID
	invoke  SendMessage, hStatusBar, SB_SETTEXT, 5, offset penlikeStyle
	invoke  wsprintfA, offset shapeStr, offset shapeFmt, offset customStr
	invoke  SendMessage, hStatusBar, SB_SETTEXT, 6, offset shapeStr


	;显示窗口
	invoke 	ShowWindow, hWinMain, SW_SHOWNORMAL			
	;更新窗口
	invoke 	UpdateWindow, hWinMain		
	;消息循环
	.while 	TRUE
		invoke 	GetMessage, addr @stMsg, NULL, 0, 0			
		.break 	.if eax == 0
					
		invoke 	TranslateAccelerator, hWinMain, @hAccelerator, addr @stMsg
		.if		eax == 0
			invoke 	TranslateMessage, addr @stMsg				
			invoke 	DispatchMessage, addr @stMsg	
						
		.endif
	.endw
							
	ret				
WinMain		endp

start:
	invoke 	InitCommonControls
	call 	WinMain
	invoke 	ExitProcess, NULL				
end start