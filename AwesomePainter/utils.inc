.code

;Set Menu Item Selectable
Enable proc ID: DWORD
	invoke  EnableMenuItem,hMainMenu,ID,MF_ENABLED
	ret
Enable endp

Disable proc ID: DWORD
	invoke EnableMenuItem,hMainMenu,ID,MF_GRAYED
	ret
Disable endp

;Return Last Error in Dialog 
ErrorHandler PROC
	INVOKE GetLastError	; Returns message ID in EAX
	mov messageID,eax

	; Get the corresponding message string.
	INVOKE FormatMessage, FORMAT_MESSAGE_ALLOCATE_BUFFER + \
	  FORMAT_MESSAGE_FROM_SYSTEM,NULL,messageID,NULL,
	  ADDR pErrorMsg,NULL,NULL

	; Display the error message.
	INVOKE MessageBox,NULL, pErrorMsg, ADDR ErrorTitle,
	MB_ICONERROR+MB_OK

	; Free the error message string.
	INVOKE LocalFree, pErrorMsg
	ret
ErrorHandler ENDP

;Open File
Open proc uses ebx edx,hWnd:HWND
	local 	@stOfn: OPENFILENAME
	local	tmpdc:HDC
	local	hdc:HDC
	local	hbmp:HBITMAP
	local	bmp:BITMAP
	local	currentClientRC:RECT
	local	currentWindowRC:RECT
	local	clientWidth:SDWORD
	local	clientHeight:SDWORD
	local	extraWidth:SDWORD
	local	extraHeight:SDWORD
	local	newWidth:SDWORD
	local	newHeight:SDWORD

	invoke RtlZeroMemory, addr @stOfn, sizeof @stOfn
	push hWinMain
	pop	@stOfn.hwndOwner
	mov	@stOfn.lStructSize, sizeof OPENFILENAME
	mov	@stOfn.lpstrFilter, offset Filter
	mov	@stOfn.lpstrFile, offset File
	mov	@stOfn.nMaxFile, MAX_PATH
	mov	@stOfn.Flags, OFN_FILEMUSTEXIST or OFN_PATHMUSTEXIST
	mov	@stOfn.lpstrDefExt, offset DefType 
				
	invoke GetOpenFileName, addr @stOfn
	invoke  GetDC, hWnd
	mov     hdc, eax
	.if	eax
		invoke  CreateCompatibleDC, hdc
		mov     tmpdc, eax
		invoke	LoadImage, NULL, addr File, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE 
		mov		hbmp,eax
		invoke  SelectObject, tmpdc, hbmp
		invoke  GetObject,hbmp,SIZEOF BITMAP,addr bmp

		invoke	GetClientRect,hWnd,addr currentClientRC
		invoke	GetWindowRect,hWnd,addr currentWindowRC

		mov ebx,currentClientRC.right
		sub ebx,currentClientRC.left
		mov clientWidth,ebx
		mov ebx,currentClientRC.bottom
		sub ebx,currentClientRC.top
		mov clientHeight,ebx

		mov ebx,clientWidth
		.if bmp.bmWidth>1900
			mov ebx,1900
			sub ebx,clientWidth
			mov extraWidth,ebx
		.elseif bmp.bmWidth < 625
			mov ebx,625
			sub ebx,clientWidth
			mov extraWidth,ebx
		.else
			mov ebx,bmp.bmWidth
			sub ebx,clientWidth
			mov extraWidth,ebx
		.endif

		mov ebx,clientHeight
		.if bmp.bmHeight > 750
			mov ebx,750
			sub ebx,clientHeight
			mov extraHeight,ebx
		.elseif bmp.bmHeight < 100
			mov ebx,100
			sub ebx,clientHeight
			mov extraHeight,ebx
		.else
			mov ebx,bmp.bmHeight
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
		invoke Clear, hWnd
		invoke SetWindowPos,hWnd,HWND_NOTOPMOST,0,0,newWidth,newHeight,SWP_SHOWWINDOW
		invoke Clear, hWnd

		invoke  BitBlt, hdc, 0, 0, bmp.bmWidth, bmp.bmHeight, tmpdc, 0, 0, SRCCOPY; 打开后,以固定大小贴到hDC上
		invoke  DeleteDC, tmpdc
		invoke	DeleteObject, hbmp
		mov		ischanged,0
	.endif
	ret				
Open endp	


;Save File
Save proc uses ebx edx,hWnd:HWND
	local 	@stOfn: OPENFILENAME
	local	filenameLen:DWORD
	local	hdc:HDC
	local	tmphdc:HDC
	local	bmp:BITMAP
	local	hbmp:HBITMAP
	local	bmpfile:BITMAPFILEHEADER
	local	bmpinfo:BITMAPINFOHEADER
	local	dwBmBitsSize:DWORD
	local   hdib:HANDLE
	local	lpbi:PTR BYTE
    local	hfile:HANDLE  
	local	writelen:DWORD
	local   currentClientRC:RECT

	invoke RtlZeroMemory, addr @stOfn, sizeof @stOfn
	push hWinMain
	pop	@stOfn.hwndOwner
	mov	@stOfn.lStructSize, sizeof OPENFILENAME
	mov	@stOfn.lpstrFilter, offset Filter
	mov	@stOfn.lpstrFile, offset File
	mov	@stOfn.nMaxFile, MAX_PATH
	mov	@stOfn.Flags, OFN_FILEMUSTEXIST or OFN_PATHMUSTEXIST
	mov	@stOfn.lpstrDefExt, offset DefType 
	invoke GetSaveFileName, addr @stOfn
	.if eax
	;检查是否有.bmp后缀，若没有加上.bmp后缀
		invoke crt_strlen, offset File
		mov filenameLen, eax
		mov ebx, offset File
		add ebx, filenameLen
		sub  ebx, 4
		invoke crt_strcmp, ebx, offset bmpExtend
		.if eax != 0
			add ebx, 4
			invoke crt_strcpy, ebx, offset bmpExtend
		.endif
		;获得图片大小
		invoke	GetClientRect,hWnd,addr currentClientRC

		mov ebx,currentClientRC.right
		sub ebx,currentClientRC.left
		mov dcWidth,ebx
		mov ebx,currentClientRC.bottom
		sub ebx,currentClientRC.top
		sub ebx, StatusBarHeight
		mov dcHeight,ebx

		;通过重绘的方法获得HBITMAP
		invoke GetDC,hWnd
		mov hdc, eax
		invoke CreateCompatibleDC,hdc
		mov tmphdc,eax
		invoke CreateCompatibleBitmap,hdc,dcWidth,dcHeight ;创建Bitmap
		mov hbmp,eax
		invoke SelectObject,tmphdc,hbmp
		invoke BitBlt,tmphdc,0,0,dcWidth,dcHeight,hdc,0,0,SRCCOPY;这一行的hdc待定，取决于画图的位置

		;从图片句柄获得图片信息
		invoke GetObject,hbmp,SIZEOF BITMAP,addr bmp
		mov bmpinfo.biSize,sizeof BITMAPINFOHEADER
		push bmp.bmWidth
		pop bmpinfo.biWidth
		push bmp.bmHeight
		pop bmpinfo.biHeight
		mov bmpinfo.biPlanes,1
		mov bmpinfo.biBitCount,32
		mov bmpinfo.biCompression,BI_RGB
		mov bmpinfo.biSizeImage,0
		mov bmpinfo.biXPelsPerMeter,0
		mov bmpinfo.biYPelsPerMeter,0
		mov bmpinfo.biClrUsed,0
		mov bmpinfo.biClrImportant,0

		;C源码dwBmBitsSize = ((Bitmap.bmWidth*wBitCount+31)/32)*4*Bitmap.bmHeight;
		movzx eax,bmpinfo.biBitCount
		mul bmp.bmWidth
		add eax,31
		shr eax,3
		mul bmp.bmHeight
		mov dwBmBitsSize,eax

		;为位图分配内存
		mov ebx,dwBmBitsSize
		add ebx,sizeof BITMAPINFOHEADER
		invoke GlobalAlloc,GHND,dwBmBitsSize
		mov hdib,eax
		invoke GlobalLock,hdib
		mov lpbi,eax
		;GetDIBits(hDC, hBMP, 0, (WORD) pbih->biHeight, lpBits, pbi, DIB_RGB_COLORS)
		invoke GetDIBits,hdc,hbmp,0,bmpinfo.biHeight,lpbi,addr bmpinfo,DIB_RGB_COLORS
		;创建文件
		invoke CreateFile,addr File,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL
		mov hfile,eax
		mov bmpfile.bfType,4d42h
		mov edx,sizeof BITMAPFILEHEADER
		add edx,bmpinfo.biSize
		mov bmpfile.bfOffBits,edx
		add ebx,sizeof BITMAPFILEHEADER
		mov bmpfile.bfSize,ebx
		;写文件
		invoke WriteFile,hfile,addr bmpfile,sizeof BITMAPFILEHEADER,addr writelen,NULL
		invoke WriteFile,hfile,addr bmpinfo,sizeof BITMAPINFOHEADER,addr writelen,NULL
		invoke WriteFile,hfile,lpbi,dwBmBitsSize,addr writelen,NULL

		invoke GlobalUnlock,hdib
		invoke GlobalFree,hdib
		invoke CloseHandle,hfile

		invoke DeleteDC,tmphdc
		invoke DeleteObject,hbmp
		invoke ReleaseDC,hWnd,hdc
		ret
	.endif
	ret
Save endp
