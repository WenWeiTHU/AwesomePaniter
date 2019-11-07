.code

;Flip Horizontally
FilpHorizontal proc uses ebx edx eax, hWnd
	local hDC : HDC
	local m_rect : RECT

    invoke  GetDC, hWnd
	mov     hDC, eax
	invoke GetClientRect, hWnd, addr m_rect
	invoke DPtoLP, hDC, addr m_rect, 2
	mov ebx, m_rect.right
	mov eax, ebx
	shl eax, 1
	sub ebx, eax ; ebx = - width
	mov eax, m_rect.right  ; eax = width
	mov edx, m_rect.bottom
	invoke StretchBlt, hDC, eax, 0, ebx, edx, hDC, 0, 0, eax, edx, SRCCOPY

	invoke ReleaseDC,hWnd,hDC
    ret
FilpHorizontal endp

;Flip Vertically
FilpVertical proc uses ebx edx eax, hWnd
	local hDC : HDC
	local m_rect : RECT

    invoke  GetDC, hWnd
	mov     hDC, eax
	invoke GetClientRect, hWnd, addr m_rect
	invoke DPtoLP, hDC, addr m_rect, 2
	mov ebx, m_rect.bottom
	sub	ebx, StatusBarHeight
	mov eax, ebx
	shl eax, 1
	sub ebx, eax ; ebx = - height
	mov eax, m_rect.bottom  ; eax = height
	sub eax, StatusBarHeight
	mov edx, m_rect.right
	invoke StretchBlt, hDC, 0, eax, edx, ebx, hDC, 0, 0, edx, eax, SRCCOPY

	invoke ReleaseDC,hWnd,hDC
    ret
FilpVertical endp

;Rotate the Whole Canvas
RotateAntiClockWise proc, hdcDest: DWORD, centerXPos: DWORD, centerYPos: DWORD, angle: DWORD, hdcSrc: DWORD, xSrc: DWORD, ySrc: DWORD, srcWidth: DWORD, srcHeight: DWORD
	local pt0 : myPoint, pt1 : myPoint, pt2 : myPoint
	local defPt0 : myPoint, defPt1 : myPoint, defPt2 : myPoint
	local desPoints[3] : POINT
	local thetS : REAL8
	local thetC : REAL8
	local notPI : REAL8    ; pi/180
	local zerohalf : REAL8 ;0.5
	local tmp1 : WORD  ;180
	local tmp2 : WORD  ;2
	;st[0] = pi
	fldpi

	mov tmp1, 180
	;st[0] = pi/180
	fidiv tmp1
	;notPI = pi/180
	fstp notPI
	;st[0] = 1.0
	fld1
	mov tmp2, 2
	;st[0] = 0.5
	fidiv tmp2
	;zerohalf = 0.5
	fst zerohalf
	;st[0] = 0.5 * srcWidth
	fimul srcWidth
	;st[0] = - 0.5 * srcWidth
	fchs
	;pt[0].x = - 0.5 * srcWidth
	fstp pt0.x
	;st[0] = 0.5
	fld zerohalf
	fimul srcHeight
	fchs
	;pt[0].y = - 0.5 * srcHeight
	fstp pt0.y

	fld pt0.x
	fst pt2.x   ;pt[2].x = pt[0].x 
	fiadd srcWidth
	fstp pt1.x  ;pt[1].x = pt[0].x + srcWidth
	fld pt0.y
	fst pt1.y  ;pt[1].y = pt[0].y
	fiadd srcHeight
	fstp pt2.y ;pt[2].y = pt[0].y + srcHeight

	fild angle
	fmul notPI
	fsin
	fstp thetS  ;sin(angle)

	fild angle
	fmul notPI
	fcos
	fstp thetC  ;cos(angle)

    fld pt0.x
	fmul thetC
	fstp defPt0.x ; x*cos
	fld pt0.y
	fmul thetS
	fadd defPt0.x
	fstp defPt0.x ; x*cos + y*sin

	fld pt0.x
	fmul thetS
	fstp defPt0.y ; x*sin
	fld pt0.y
	fmul thetC
	fsub defPt0.y
	fstp defPt0.y ; y*cos - x*sin

	
    fld pt1.x
	fmul thetC
	fstp defPt1.x ; x*cos
	fld pt1.y
	fmul thetS
	fadd defPt1.x
	fstp defPt1.x ; x*cos + y*sin

	fld pt1.x
	fmul thetS
	fstp defPt1.y ; x*sin
	fld pt1.y
	fmul thetC
	fsub defPt1.y
	fstp defPt1.y ; y*cos - x*sin


    fld pt2.x
	fmul thetC
	fstp defPt2.x ; x*cos
	fld pt2.y
	fmul thetS
	fadd defPt2.x
	fstp defPt2.x ; x*cos + y*sin

	fld pt2.x
	fmul thetS
	fstp defPt2.y ; x*sin
	fld pt2.y
	fmul thetC
	fsub defPt2.y
	fstp defPt2.y ; y*cos - x*sin

	fld defPt0.x
	fiadd centerXPos
	fistp desPoints[0].x
	fld defPt0.y
	fiadd centerYPos
	fistp desPoints[0].y
	fld defPt1.x
	fiadd centerXPos
	;POINT 有 2 个 LONG，共8个字节
	fistp desPoints[8].x
	fld defPt1.y
	fiadd centerYPos
	fistp desPoints[8].y
	fld defPt2.x
	fiadd centerXPos
	fistp desPoints[16].x
	fld defPt2.y
	fiadd centerYPos
	fistp desPoints[16].y
	invoke PlgBlt, hdcDest, addr desPoints, hdcSrc, xSrc, ySrc, srcWidth, srcHeight, 0, 0, 0

	ret
RotateAntiClockWise endp

;hWnd里的图放到全局位图里
saveImage proc, hWnd:DWORD
	local m_rect: RECT
	local hDcParent : HDC
	local tmphdc : HDC
	local tmphdcUndo : HDC
	local rect_width : DWORD
	local rect_height : DWORD
	;内存映像DC
	invoke  GetDC, hWnd
	mov     hDcParent, eax
	;获得长宽
	invoke GetClientRect, hWnd, addr m_rect
	invoke DPtoLP, hDcParent, addr m_rect, 2
	mov ebx, m_rect.right
	mov rect_width, ebx
	mov edx, m_rect.bottom
	sub edx, StatusBarHeight
	mov rect_height, edx

	invoke CreateCompatibleDC,hDcParent
	mov tmphdc,eax
	
	invoke DeleteObject, hUndoBitmap
     ;如果第一笔撤销，为了防止此时UndoBitmap为空，先涂白色
	invoke SelectObject, tmphdcUndo, hUndoBitmap
	invoke 	GetStockObject, WHITE_BRUSH
    invoke 	FillRect, tmphdcUndo, addr m_rect, eax
	;两个 0 表示和原图保持一致的长和宽
	invoke CopyImage, hSaveBitmap,IMAGE_BITMAP, 0,0,LR_COPYDELETEORG
	mov hUndoBitmap, eax

	
	;将位图保存在全局位图中
	invoke DeleteObject, hSaveBitmap
	invoke CreateCompatibleBitmap,hDcParent,rect_width,rect_height
	mov hSaveBitmap, eax
	invoke SelectObject,tmphdc,hSaveBitmap
	invoke BitBlt,tmphdc,0,0,rect_width,rect_height,hDcParent,0,0,SRCCOPY

	invoke DeleteDC, tmphdc
	invoke DeleteDC, tmphdcUndo
	invoke ReleaseDC,hWnd,hDcParent

    
	;允许撤销
	mov isUndoOnce, TRUE

    ret
saveImage endp

Undo proc, hWnd:DWORD
	local hDcParent : HDC
	local tmphdc: HDC
	local tmphdcUndo: HDC
	local rect_width : DWORD
	local rect_height : DWORD
	local m_rect: RECT

	.if isUndoOnce == TRUE
		invoke  GetDC, hWnd
		mov     hDcParent, eax
		invoke  CreateCompatibleDC, hDcParent
		mov     tmphdcUndo, eax

		; 将位图画在"屏幕内存映像"上
		invoke  SelectObject, tmphdcUndo, hUndoBitmap
		invoke GetClientRect, hWnd, addr m_rect
		mov ebx, m_rect.right
		mov rect_width, ebx
		mov edx, m_rect.bottom
		sub edx,StatusBarHeight
		mov rect_height, edx

		; 将位图从tmphdcUndo复制到hDC中
		invoke  BitBlt, hDcParent, 0, 0, rect_width, rect_height, tmphdcUndo, 0, 0, SRCCOPY
		;这里必须使用srcPaint，如果是srcCopy，颜色会丢失
	
	
		; 删除tmphdc
		invoke DeleteDC, tmphdc
		invoke DeleteDC, tmphdcUndo
		invoke ReleaseDC, hWnd, hDcParent
		invoke saveImage, hWnd
		;不能再撤销了
		mov isUndoOnce, FALSE

		

		;让hSaveBitmap 和 hUndoBitmap 一样

	.else
		invoke MessageBox,hWinMain,addr UndoOnce,addr UndoOnceTitle,MB_OK
	.endif
	ret
Undo endp

;旋转对话框
rotateCanvas PROC hWnd:DWORD, uMsg:DWORD, wParam:DWORD, lParam:DWORD
	local LPTSuccess : DWORD
	local angle : DWORD
	local hWndParent: DWORD
	local hDcParent : HDC
	local tmphdc : HDC
	local m_rect :RECT
	local hbmp:HBITMAP
	
	local rect_width : DWORD
	local rect_height : DWORD

	.if uMsg==WM_INITDIALOG
		;invoke SetDlgItemInt,hWnd,IDC_WIDTHBOX, PenWidth, 0
		push hWndGlobal
		pop hWndParent
	.elseif uMsg==WM_COMMAND
		mov eax,wParam
		shr eax,16
		.if ax==BN_CLICKED
			mov eax,wParam
			.if ax==IDCANCEL
				invoke SendMessage,hWnd,WM_CLOSE,0,0
			.elseif ax==IDOK	
				push hWndGlobal
				pop hWndParent
				invoke GetDlgItemInt, hWnd, IDC_ROTATEBOX, addr LPTSuccess, TRUE
				.if LPTSuccess == TRUE
					mov angle, eax
					
					invoke GetClientRect, hWndParent, addr m_rect
					invoke DPtoLP, hDcParent, addr m_rect, 2
					mov ebx, m_rect.right
					mov rect_width, ebx
					mov edx, m_rect.bottom
                    sub edx, StatusBarHeight
					mov rect_height, edx

					; 将图像复制一份上
					invoke  GetDC, hWndParent
					mov     hDcParent, eax
					invoke CreateCompatibleDC,hDcParent
					mov tmphdc,eax

					invoke CreateCompatibleBitmap,hDcParent,rect_width,rect_height ;创建Bitmap
					mov hbmp,eax
					invoke SelectObject,tmphdc,hbmp
					invoke BitBlt,tmphdc,0,0,rect_width,rect_height,hDcParent,0,0,SRCCOPY;这一行的hdc待定，取决于画图的位置
	
					;清空一下画面
					invoke 	GetStockObject, WHITE_BRUSH
    				invoke 	FillRect, hDcParent, addr m_rect, eax
					;获取中心点
					mov eax, rect_width
					shr eax, 1
					mov ecx, rect_height
					shr ecx, 1
					;绕中心旋转
					invoke RotateAntiClockWise, hDcParent, eax, ecx, angle, tmphdc, 0, 0, rect_width, rect_height
					;将位图保存在全局位图中
					;invoke DeleteObject, hSaveBitmap
					;invoke CreateCompatibleBitmap,hDcParent,rect_width,rect_height
					;mov hSaveBitmap, eax
					;invoke SelectObject,tmphdc,hSaveBitmap
					;invoke BitBlt,tmphdc,0,0,rect_width,rect_height,hDcParent,0,0,SRCCOPY
					invoke saveImage, hWndParent
					invoke DeleteObject, hbmp
					invoke DeleteDC, tmphdc
					invoke ReleaseDC,hWndParent,hDcParent
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
rotateCanvas ENDP

;Clear Canvas
Clear proc, hwnd : HWND
	local hDC: HDC
	local rc : RECT

	invoke 	GetDC, hwnd
	mov     hDC, eax
	invoke 	GetClientRect, hwnd, addr rc
	invoke 	GetStockObject, WHITE_BRUSH
     invoke 	FillRect, hDC, addr rc, eax
	invoke  ReleaseDC, hwnd, hDC
	ret
Clear endp

;
SetSize PROC uses ebx edx,hwnd:DWORD, uMsg:DWORD, wParam:DWORD, lParam:DWORD
	local	LPTSuccess: DWORD
	local	tmphdc:HDC
	local	hdc:HDC
	local	hbmp:HBITMAP
	local	currentClientRC:RECT
	local	currentWindowRC:RECT
	local	clientWidth:SDWORD
	local	clientHeight:SDWORD
	local	extraWidth:SDWORD
	local	extraHeight:SDWORD
	local	newWidth:SDWORD
	local	newHeight:SDWORD

	.if uMsg==WM_INITDIALOG
		;获得图片大小
		invoke	GetClientRect,hWinMain,addr currentClientRC
		mov ebx,currentClientRC.right
		sub ebx,currentClientRC.left
		mov setwidth,ebx
		mov ebx,currentClientRC.bottom
		sub ebx,currentClientRC.top
        sub ebx, StatusBarHeight
		mov setheight,ebx
		invoke SetDlgItemInt,hwnd,IDC_SETWIDTH, setwidth, 0
		invoke SetDlgItemInt,hwnd,IDC_SETHEIGHT, setheight, 0
	.elseif uMsg==WM_COMMAND
		mov eax,wParam
		shr eax,16
		.if ax==BN_CLICKED
			mov eax,wParam
			.if ax==IDCANCEL
				invoke SendMessage,hwnd,WM_CLOSE,0,0
			.elseif ax==IDOK
				;得到输入的宽度
				invoke GetDlgItemInt, hwnd, IDC_SETWIDTH, addr LPTSuccess, TRUE
				.if LPTSuccess == FALSE
					invoke MessageBox, hwnd, offset invalidInput, offset invalidInputTitle, MB_OK
					invoke EndDialog,hwnd,0
					mov eax,TRUE
					ret
				.endif
				mov setwidth,eax
				;得到输入的长度
				invoke GetDlgItemInt,hwnd,IDC_SETHEIGHT, addr LPTSuccess,TRUE
				.if LPTSuccess == FALSE
					invoke MessageBox, hwnd, offset invalidInput, offset invalidInputTitle, MB_OK
					invoke EndDialog,hwnd,0
					mov eax,TRUE
					ret
				.endif
				mov setheight,eax

				;获得图片大小
				invoke	GetClientRect,hWinMain,addr currentClientRC

				mov ebx,currentClientRC.right
				sub ebx,currentClientRC.left
				mov clientWidth,ebx
				mov ebx,currentClientRC.bottom
				sub ebx,currentClientRC.top
                sub ebx, StatusBarHeight
				mov clientHeight,ebx

				;保存当前位图
				invoke GetDC,hWinMain
				mov hdc, eax
				invoke CreateCompatibleDC,hdc
				mov tmphdc,eax
				invoke CreateCompatibleBitmap,hdc,clientWidth,clientHeight ;创建Bitmap
				mov hbmp,eax
				invoke SelectObject,tmphdc,hbmp
				invoke BitBlt,tmphdc,0,0,clientWidth,clientHeight,hdc,0,0,SRCCOPY

				invoke	GetWindowRect,hWinMain,addr currentWindowRC

				mov ebx,clientWidth
				.if setwidth>1900
					mov ebx,1900
					sub ebx,clientWidth
					mov extraWidth,ebx
				.elseif setwidth < 625
					mov ebx,625
					sub ebx,clientWidth
					mov extraWidth,ebx
				.else
					mov ebx,setwidth
					sub ebx,clientWidth
					mov extraWidth,ebx
				.endif

				mov ebx,clientHeight
				.if setheight > 750
					mov ebx,750
					sub ebx,clientHeight
					mov extraHeight,ebx
				.elseif setheight < 100
					mov ebx,100
					sub ebx,clientHeight
					mov extraHeight,ebx
				.else
					mov ebx,setheight
					sub ebx,clientHeight
					mov extraHeight,ebx
				.endif

				mov ebx,currentWindowRC.right
				sub ebx,currentWindowRC.left
				add ebx,extraWidth
				mov newWidth,ebx
				mov ebx,currentWindowRC.bottom
				sub ebx,currentWindowRC.top
				add ebx,extraHeight
				mov newHeight,ebx

				invoke Clear,hWinMain
				invoke SetWindowPos,hWinMain,HWND_NOTOPMOST,0,0,newWidth,newHeight,SWP_SHOWWINDOW
				invoke Clear,hWinMain

				invoke  BitBlt, hdc, 0, 0, clientWidth, clientHeight, tmphdc, 0, 0, SRCCOPY; 打开后,以固定大小贴到hDC上
				invoke  DeleteDC, tmphdc
				invoke	DeleteObject, hbmp

				invoke EndDialog,hwnd,0
			.endif
		.endif
	.elseif uMsg==WM_CLOSE
		invoke EndDialog,hwnd,0
	.else
		mov eax,FALSE
		ret
	.endif

	mov eax,TRUE
	ret
SetSize endp