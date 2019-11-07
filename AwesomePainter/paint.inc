.code


;Dialog of Text Input 
textDialog proc hWnd:DWORD, uMsg:DWORD, wParam:DWORD, lParam:DWORD
	local hParentWnd : DWORD
	local hParentDC : HDC
	local readLen: DWORD

	.if uMsg==WM_INITDIALOG
	.elseif uMsg==WM_COMMAND
		mov eax,wParam
		shr eax,16
		.if ax==BN_CLICKED
			mov eax,wParam
			.if ax==IDCANCEL
				invoke SendMessage,hWnd,WM_CLOSE,0,0
			.elseif ax==IDOK
				invoke GetDlgItemText, hWnd, IDC_TEXTBOX, offset inputText, inputTextMaxLen	
				mov readLen, eax
				invoke GetParent, hWnd
				mov  hParentWnd, eax
				invoke  GetDC, hParentWnd
				mov hParentDC, eax
				invoke TextOut, hParentDC, g_NewPointX, g_NewPointY, offset inputText, readLen
				invoke  ReleaseDC, hParentWnd, hParentDC
				invoke  EndDialog, hWnd, 0
			.endif
		.endif
	.elseif uMsg==WM_CLOSE
		invoke EndDialog,hWnd,0
	.else
		mov eax,FALSE
		ret
	.endif
	
	mov eax,TRUE
	ret
textDialog endp


;Dialog of Color Change
changeColor PROC hWnd:DWORD, uMsg:DWORD, wParam:DWORD, lParam:DWORD
	local color:CHOOSECOLOR
	local hDC : HDC

	.if uMsg==WM_INITDIALOG
		
	.elseif uMsg==WM_COMMAND
		mov eax,wParam
		shr eax,16
		.if ax==BN_CLICKED
			mov eax,wParam
			.if ax==IDCANCEL
				invoke SendMessage,hWnd,WM_CLOSE,0,0
			.elseif ax==IDOK		
				invoke  GetDC, hWnd
				mov     hDC, eax
				invoke  CreatePen, PS_SOLID, PenWidth, PenColor
				mov     hPen, eax
				
				.if BrushStyle == HS_SOLID
					invoke  CreateSolidBrush, PenColor
				.else
					invoke  CreateHatchBrush, BrushStyle, PenColor
				.endif 
				mov		hBrsh, eax

				invoke  wsprintfA, offset penClr, offset colorFmt, PenColor
				invoke	SendMessage, hStatusBar, SB_SETTEXT, 4, offset penClr

				.if DrawToolSelect == DRAW_PEN
					push 	hPen
					pop     PenLikeTool
					invoke  SendMessage, hStatusBar,SB_SETTEXT, 2,offset penString
				.elseif DrawToolSelect == DRAW_BRUSH
					push 	hBrsh
					pop     PenLikeTool
					invoke  SendMessage, hStatusBar,SB_SETTEXT, 2,offset brushString
				.endif
				invoke  SelectObject, hDC, PenLikeTool
				invoke  DeleteObject, eax
				invoke  ReleaseDC, hWnd, hDC
				invoke  EndDialog, hWnd, 0
			.elseif ax==IDC_COLORBOX
			    invoke RtlZeroMemory,addr color,sizeof color
				mov color.lStructSize,sizeof color
				push hWnd
				pop color.hwndOwner
				push hInstance
				pop color.hInstance
				push PenColor
				pop color.rgbResult
				mov color.lpCustColors,offset PenColor
				mov color.Flags,CC_ANYCOLOR or CC_RGBINIT
				invoke ChooseColor,addr color
				
				.if eax!=0
					push color.rgbResult
					pop PenColor
					invoke GetDlgItem,hWnd,IDC_COLORBOX
					invoke InvalidateRect,eax,0,TRUE
				.endif
			.endif
		.endif
	.elseif uMsg==WM_CTLCOLORSTATIC
		invoke GetDlgItem,hWnd,IDC_COLORBOX
		.if eax==lParam
			invoke CreateSolidBrush,PenColor			
			ret
		.endif
		mov eax,FALSE
		ret
	.elseif uMsg==WM_CLOSE
		invoke EndDialog,hWnd,0
	.else
		mov eax,FALSE
		ret
	.endif

	mov eax,TRUE
	ret
changeColor ENDP


;Dialog of Pen Width Change
changeWidth PROC hWnd:DWORD, uMsg:DWORD, wParam:DWORD, lParam:DWORD
	local LPTSuccess: DWORD
	local hDC : HDC

	.if uMsg==WM_INITDIALOG
		invoke SetDlgItemInt,hWnd,IDC_WIDTHBOX, PenWidth, 0
	.elseif uMsg==WM_COMMAND
		mov eax,wParam
		shr eax,16
		.if ax==BN_CLICKED
			mov eax,wParam
			.if ax==IDCANCEL
				invoke SendMessage,hWnd,WM_CLOSE,0,0
			.elseif ax==IDOK	
				invoke GetDlgItemInt, hWnd, IDC_WIDTHBOX, addr LPTSuccess, TRUE
				.if LPTSuccess == TRUE
					mov PenWidth, eax
					invoke  GetDC, hWnd
					mov     hDC, eax

					invoke  CreatePen, PS_SOLID, PenWidth, 0ffffffH
					mov     hEraser, eax
					invoke  CreatePen, PS_SOLID, PenWidth, PenColor
					mov		hPen, eax

					.if DrawToolSelect == DRAW_PEN
						push hPen
						pop  PenLikeTool
						invoke  SendMessage, hStatusBar,SB_SETTEXT, 2,offset penString
					.elseif DrawToolSelect == DRAW_ERASER
						push hEraser
						pop  PenLikeTool
						invoke  SendMessage, hStatusBar,SB_SETTEXT, 2,offset eraserString
					.endif
					invoke  wsprintfA, offset penWth, offset widthFmt, PenWidth
					invoke  SendMessage, hStatusBar, SB_SETTEXT, 3, offset penWth
					invoke  SelectObject, hDC, PenLikeTool
					invoke  DeleteObject, eax
					invoke  ReleaseDC, hWnd, hDC
				.else
					invoke MessageBox, hWnd, offset invalidInput, offset invalidInputTitle, MB_OK
				.endif
				invoke EndDialog,hWnd,0
			.endif
		.endif
	.elseif uMsg==WM_CLOSE
		invoke EndDialog,hWnd,0
	.else
		mov eax,FALSE
		ret
	.endif

	mov eax,TRUE
	ret
changeWidth ENDP


;Dialog of Draw Arc
drawArc PROC hWnd:DWORD, uMsg:DWORD, wParam:DWORD, lParam:DWORD
	local LPTSuccess: DWORD

	.if uMsg==WM_INITDIALOG
		invoke SetDlgItemInt,hWnd,IDC_RADIUSBOX, ArcRadius, 0
		invoke SetDlgItemInt,hWnd,IDC_STARTABOX, ArcStartA, 0
		invoke SetDlgItemInt,hWnd,IDC_SWEEPABOX, ArcSweepA, 0
	.elseif uMsg==WM_COMMAND
		mov eax,wParam
		shr eax,16
		.if ax==BN_CLICKED
			mov eax,wParam
			.if ax==IDCANCEL
				invoke SendMessage,hWnd,WM_CLOSE,0,0
			.elseif ax==IDOK	
				invoke GetDlgItemInt, hWnd, IDC_RADIUSBOX, addr LPTSuccess, TRUE
				.if LPTSuccess == TRUE && eax > 0
					mov 	ArcRadius, eax
				.else
					invoke MessageBox, hWnd, offset invalidInput, offset invalidInputTitle, MB_OK
					push SHAPE_DRAW
					pop  ShapeSelect
					invoke  wsprintfA, offset shapeStr, offset shapeFmt, offset customStr
					invoke	SendMessage, hStatusBar, SB_SETTEXT, 6, offset shapeStr
				.endif
                invoke GetDlgItemInt, hWnd, IDC_STARTABOX, addr LPTSuccess, TRUE
				.if LPTSuccess == TRUE
					mov 	ArcStartA, eax
				.else
					invoke MessageBox, hWnd, offset invalidInput, offset invalidInputTitle, MB_OK
					push SHAPE_DRAW
					pop  ShapeSelect
					invoke  wsprintfA, offset shapeStr, offset shapeFmt, offset customStr
					invoke	SendMessage, hStatusBar, SB_SETTEXT, 6, offset shapeStr
				.endif
				invoke GetDlgItemInt, hWnd, IDC_SWEEPABOX, addr LPTSuccess, TRUE
				.if LPTSuccess == TRUE
					mov 	ArcSweepA, eax
				.else
					invoke MessageBox, hWnd, offset invalidInput, offset invalidInputTitle, MB_OK
					push SHAPE_DRAW
					pop  ShapeSelect
					invoke  wsprintfA, offset shapeStr, offset shapeFmt, offset customStr
					invoke	SendMessage, hStatusBar, SB_SETTEXT, 6, offset shapeStr
				.endif
				invoke EndDialog,hWnd,0
			.endif
		.endif
	.elseif uMsg==WM_CLOSE
		invoke EndDialog,hWnd,0
	.else
		mov eax,FALSE
		ret
	.endif

	mov eax,TRUE
	ret
drawArc ENDP

;Dialog of Polygon Edge Num
polygonNum PROC hWnd:DWORD, uMsg:DWORD, wParam:DWORD, lParam:DWORD
	local LPTSuccess: DWORD

	.if uMsg==WM_INITDIALOG
	.elseif uMsg==WM_COMMAND
		mov eax,wParam
		shr eax,16
		.if ax==BN_CLICKED
			mov eax,wParam
			.if ax==IDCANCEL
				invoke SendMessage,hWnd,WM_CLOSE,0,0
			.elseif ax==IDOK	
				invoke GetDlgItemInt, hWnd, IDC_POLYGONBOX, addr LPTSuccess, TRUE
				.if LPTSuccess == TRUE && eax >= 2 && eax <= 100
					mov 	EdgeNum, eax
					mov		EdgeNumLeft, eax
				.else
					invoke MessageBox, hWnd, offset invalidInput, offset invalidInputTitle, MB_OK
					push SHAPE_DRAW
					pop  ShapeSelect
					invoke  wsprintfA, offset shapeStr, offset shapeFmt, offset customStr
					invoke	SendMessage, hStatusBar, SB_SETTEXT, 6, offset shapeStr
				.endif
				invoke EndDialog,hWnd,0
			.endif
		.endif
	.elseif uMsg==WM_CLOSE
		invoke EndDialog,hWnd,0
	.else
		mov eax,FALSE
		ret
	.endif

	mov eax,TRUE
	ret
polygonNum ENDP


;Dialog of Polylin Point Num
polylineNum PROC hWnd:DWORD, uMsg:DWORD, wParam:DWORD, lParam:DWORD
	local LPTSuccess: DWORD

	.if uMsg==WM_INITDIALOG
	.elseif uMsg==WM_COMMAND
		mov eax,wParam
		shr eax,16
		.if ax==BN_CLICKED
			mov eax,wParam
			.if ax==IDCANCEL
				invoke SendMessage,hWnd,WM_CLOSE,0,0
			.elseif ax==IDOK	
				invoke GetDlgItemInt, hWnd, IDC_POLYLINEBOX, addr LPTSuccess, TRUE
				.if LPTSuccess == TRUE && eax >= 2 && eax <= 100
					mov 	EdgeNum, eax
					mov		EdgeNumLeft, eax
				.else
					invoke MessageBox, hWnd, offset invalidInput, offset invalidInputTitle, MB_OK
					push SHAPE_DRAW
					pop  ShapeSelect
					invoke  wsprintfA, offset shapeStr, offset shapeFmt, offset customStr
					invoke	SendMessage, hStatusBar, SB_SETTEXT, 6, offset shapeStr
				.endif
				invoke EndDialog,hWnd,0
			.endif
		.endif
	.elseif uMsg==WM_CLOSE
		invoke EndDialog,hWnd,0
	.else
		mov eax,FALSE
		ret
	.endif

	mov eax,TRUE
	ret
polylineNum ENDP


;Dialog of Pen Style Change
changeStyle PROC hWnd:DWORD, uMsg:DWORD, wParam:DWORD, lParam:DWORD
	local LPTCBBOX: DWORD
	local styleSelected : DWORD
	local hDC: HDC

	.if uMsg==WM_INITDIALOG
		invoke GetDlgItem,hWnd,IDC_STYLEBOX
		mov LPTCBBOX, eax
		invoke SendMessage, LPTCBBOX, CB_ADDSTRING, 0, offset PENSTYLE_SOLID
		invoke SendMessage, LPTCBBOX, CB_ADDSTRING, 0, offset PENSTYLE_DASH
		invoke SendMessage, LPTCBBOX, CB_ADDSTRING, 0, offset PENSTYLE_DOT
		invoke SendMessage, LPTCBBOX, CB_ADDSTRING, 0, offset PENSTYLE_DASHDOT
		invoke SendMessage, LPTCBBOX, CB_ADDSTRING, 0, offset PENSTYLE_DASHDOTDOT
		invoke SendMessage, LPTCBBOX, CB_ADDSTRING, 0, offset PENSTYLE_NULL
		invoke SendMessage, LPTCBBOX, CB_ADDSTRING, 0, offset PENSTYLE_INSIDEFRAME
	.elseif uMsg==WM_COMMAND
		mov eax,wParam
		shr eax,16
		.if ax==BN_CLICKED
			mov eax,wParam
			.if ax==IDCANCEL
				invoke SendMessage,hWnd,WM_CLOSE,0,0
			.elseif ax==IDOK
				invoke SendDlgItemMessage, hWnd, IDC_STYLEBOX, CB_GETCURSEL, 0, 0
				mov styleSelected, eax
				.if styleSelected == 0
					push PS_SOLID
					invoke wsprintfA, offset penlikeStyle, offset styleFmt, offset PENSTYLE_SOLID
				.elseif styleSelected == 1
					push PS_DASH
					invoke wsprintfA, offset penlikeStyle, offset styleFmt, offset PENSTYLE_DASH
				.elseif styleSelected == 2
					push PS_DOT
					invoke wsprintfA, offset penlikeStyle, offset styleFmt, offset PENSTYLE_DOT
				.elseif styleSelected == 3
					push PS_DASHDOT
					invoke wsprintfA, offset penlikeStyle, offset styleFmt, offset PENSTYLE_DASHDOT
				.elseif styleSelected == 4
					push PS_DASHDOTDOT
					invoke wsprintfA, offset penlikeStyle, offset styleFmt, offset PENSTYLE_DASHDOTDOT
				.elseif styleSelected == 5
					push PS_NULL
					invoke wsprintfA, offset penlikeStyle, offset styleFmt, offset PENSTYLE_NULL
				.elseif styleSelected == 6
					push PS_INSIDEFRAME
					invoke wsprintfA, offset penlikeStyle, offset styleFmt, offset PENSTYLE_INSIDEFRAME
				.else
					invoke  EndDialog,hWnd,0
				.endif
				pop  PenStyle
				invoke  SendMessage, hStatusBar, SB_SETTEXT, 5, offset penlikeStyle
				invoke SendMessage, hStatusBar,SB_SETTEXT, 3,offset nullstring
				invoke  GetDC, hWnd
				mov     hDC, eax
				.if PenStyle == PS_SOLID || PenStyle == PS_INSIDEFRAME || PenStyle == PS_NULL
					invoke 	CreatePen, PenStyle, PenWidth, PenColor
				.else
					invoke 	CreatePen, PenStyle, 1, PenColor
				.endif
				mov 	PenLikeTool, eax
				invoke  SendMessage, hStatusBar,SB_SETTEXT, 2,offset penString
				mov 	hPen, eax
				invoke  SelectObject, hDC, PenLikeTool
				invoke  DeleteObject, eax
				invoke  ReleaseDC, hWnd, hDC
				invoke  EndDialog,hWnd,0
			.endif
		.endif
	.elseif uMsg==WM_CLOSE
		invoke EndDialog,hWnd,0
	.else
		mov eax,FALSE
		ret
	.endif

	mov eax,TRUE
	ret
changeStyle ENDP


;Dialog of Brush Style Change
changeBRStyle PROC hWnd:DWORD, uMsg:DWORD, wParam:DWORD, lParam:DWORD
	local LPTCBBOX: DWORD
	local styleSelected : DWORD
	local hDC: HDC

	.if uMsg==WM_INITDIALOG
		invoke GetDlgItem,hWnd,IDC_BRSTYLEBOX
		mov LPTCBBOX, eax
		invoke SendMessage, LPTCBBOX, CB_ADDSTRING, 0, offset BRSTYLE_BDIAGONAL
		invoke SendMessage, LPTCBBOX, CB_ADDSTRING, 0, offset BRSTYLE_CROSS
		invoke SendMessage, LPTCBBOX, CB_ADDSTRING, 0, offset BRSTYLE_DIAGCROSS
		invoke SendMessage, LPTCBBOX, CB_ADDSTRING, 0, offset BRSTYLE_FDIAGONAL
		invoke SendMessage, LPTCBBOX, CB_ADDSTRING, 0, offset BRSTYLE_HORIZONTAL
		invoke SendMessage, LPTCBBOX, CB_ADDSTRING, 0, offset BRSTYLE_VERTICAL
		invoke SendMessage, LPTCBBOX, CB_ADDSTRING, 0, offset BRSTYLE_SOLID
	.elseif uMsg==WM_COMMAND
		mov eax,wParam
		shr eax,16
		.if ax==BN_CLICKED
			mov eax,wParam
			.if ax==IDCANCEL
				invoke SendMessage,hWnd,WM_CLOSE,0,0
			.elseif ax==IDOK
				invoke SendDlgItemMessage, hWnd, IDC_BRSTYLEBOX, CB_GETCURSEL, 0, 0
				mov styleSelected, eax
				.if styleSelected == 0
					push HS_BDIAGONAL
					invoke wsprintfA, offset penlikeStyle, offset styleFmt, offset BRSTYLE_BDIAGONAL
				.elseif styleSelected == 1
					push HS_CROSS
					invoke wsprintfA, offset penlikeStyle, offset styleFmt, offset BRSTYLE_CROSS
				.elseif styleSelected == 2
					push HS_DIAGCROSS
					invoke wsprintfA, offset penlikeStyle, offset styleFmt, offset BRSTYLE_DIAGCROSS
				.elseif styleSelected == 3
					push HS_FDIAGONAL
					invoke wsprintfA, offset penlikeStyle, offset styleFmt, offset BRSTYLE_FDIAGONAL
				.elseif styleSelected == 4
					push HS_HORIZONTAL
					invoke wsprintfA, offset penlikeStyle, offset styleFmt, offset BRSTYLE_HORIZONTAL
				.elseif styleSelected == 5
					push HS_VERTICAL
					invoke wsprintfA, offset penlikeStyle, offset styleFmt, offset BRSTYLE_VERTICAL
				.elseif styleSelected == 6
					invoke  GetDC, hWnd
					mov     hDC, eax
					invoke 	CreateSolidBrush, PenColor
					mov 	PenLikeTool, eax
					invoke  SendMessage, hStatusBar,SB_SETTEXT, 2,offset brushString
					mov 	hBrsh, eax
					invoke  SelectObject, hDC, PenLikeTool
					invoke  DeleteObject, eax
					invoke  ReleaseDC, hWnd, hDC
					invoke wsprintfA, offset penlikeStyle, offset styleFmt, offset BRSTYLE_VERTICAL
					invoke  SendMessage, hStatusBar, SB_SETTEXT, 5, offset penlikeStyle
					invoke  EndDialog,hWnd,0
				.else
					invoke  EndDialog,hWnd,0
				.endif
				pop  BrushStyle
				invoke  SendMessage, hStatusBar, SB_SETTEXT, 5, offset penlikeStyle
				invoke  GetDC, hWnd
				mov     hDC, eax
				invoke 	CreateHatchBrush, BrushStyle, PenColor
				mov 	PenLikeTool, eax
				invoke SendMessage, hStatusBar,SB_SETTEXT, 2,offset brushString
				invoke SendMessage, hStatusBar,SB_SETTEXT, 3,offset nullstring
				push DRAW_BRUSH
				pop  DrawToolSelect
				invoke  wsprintfA, offset shapeStr, offset shapeFmt, offset lineStr
				invoke	SendMessage, hStatusBar, SB_SETTEXT, 6, offset shapeStr
				mov 	hBrsh, eax
				invoke  SelectObject, hDC, PenLikeTool
				invoke  DeleteObject, eax
				invoke  ReleaseDC, hWnd, hDC
				invoke  EndDialog,hWnd,0
			.endif
		.endif
	.elseif uMsg==WM_CLOSE
		invoke EndDialog,hWnd,0
	.else
		mov eax,FALSE
		ret
	.endif

	mov eax,TRUE
	ret
changeBRStyle ENDP


