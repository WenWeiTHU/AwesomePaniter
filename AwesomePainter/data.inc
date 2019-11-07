;Global data

.data

;---------------Handles---------------
hInstance 	  	DWORD 		?
hWinMain  	  	DWORD 		?
hMainMenu		DWORD		?
hStatusBar		DWORD		?
hFile			DWORD 		?


;---------------Rotation---------------
hWndGlobal		HWND		?
hSaveBitmap		HBITMAP		?
myPoint struct
	x REAL8 0.0
	y REAL8 0.0
myPoint ends

;--------------Undo------------------
hUndoDC 	HDC 		NULL
hUndoBitmap         HBITMAP        ?
isUndoOnce          BOOL      FALSE

;---------------Select---------------
selectwidth 	DWORD 		0
selectheight 	DWORD 		0
selecttmphdc 	HDC 		NULL
dcWidth			DWORD		784
dcHeight		DWORD		540
isSelect		BOOL		FALSE
isStraw			BOOL 		FALSE
selectok		BOOL		FALSE
aptRect        POINT   5 DUP (<0, 0>)
ptTmp          POINT     <0,0>

;---------------Painting---------------
;isDraw = TRUE:	 Pen,Brush,Eraser,Text,Oilpainter
;isDraw = FALSE: Straw,Selector
isDraw			BOOL		TRUE

;Drawing Tools
;Current Using Tool: Five options below
DrawToolSelect	DWORD		0
DRAW_PEN		equ			0
DRAW_BRUSH		equ 		1
DRAW_ERASER		equ 		2
DRAW_TEXT		equ 		3
DRAW_BUCKET		equ 		4

;Current Select Pen Tool: Three options below
PenLikeTool		DWORD		?
hPen     		DWORD		?
hBrsh      		DWORD 		?
hEraser			DWORD		?

;For hPen
PenColor 		DWORD		0 	; init with black
PenWidth		DWORD       5   ; init with 5
PenStyle		DWORD 		PS_SOLID ; init with solid

;hPen Style String
PENSTYLE_SOLID			BYTE		'Solid', 0
PENSTYLE_DASH			BYTE		'Dash(PW=1)', 0
PENSTYLE_DOT			BYTE		'Dot(PW=1)', 0
PENSTYLE_DASHDOT		BYTE		'DashDot(PW=1)', 0
PENSTYLE_DASHDOTDOT		BYTE		'DashDotDotT(PW=1)', 0
PENSTYLE_NULL			BYTE		'Null', 0
PENSTYLE_INSIDEFRAME	BYTE		'InsideFrame', 0

; For Brush
BrushStyle				DWORD 		HS_SOLID ; init with solid

;hBrsh Style String
BRSTYLE_BDIAGONAL		BYTE		'BDiagonal', 0
BRSTYLE_CROSS			BYTE		'Cross', 0
BRSTYLE_DIAGCROSS		BYTE		'DiagCross', 0
BRSTYLE_FDIAGONAL		BYTE		'FDiagonal', 0
BRSTYLE_HORIZONTAL		BYTE		'Horizontal', 0
BRSTYLE_VERTICAL		BYTE		'Vertical', 0
BRSTYLE_SOLID			BYTE		'Solid', 0

;For Mouse Postion
g_OldPointX		DWORD       ?
g_OldPointY		DWORD		?
g_NewPointX		DWORD       ?
g_NewPointY		DWORD		?
EdgeNum			DWORD		0
EdgeNumLeft		DWORD		0
ArcRadius		DWORD		0
ArcStartA		DWORD		0
ArcSweepA		DWORD		0
PolyPoints		POINT		100 DUP (<0, 0>)
isButtonDown	DWORD		FALSE

;Shapes: Options below
SHAPE_DRAW		equ			0; using mouse to draw free

SHAPE_LINE		equ			1
SHAPE_RECTANGLE equ			2
SHAPE_ELLIPSE	equ			3
SHAPE_POLYGON	equ			4
SHAPE_POLYLINE	equ			5
SHAPE_ARC		equ			6
ShapeSelect		DWORD		0; three options above

;Text String
inputText			BYTE		500 DUP(0)
inputTextMaxLen		DWORD		500


;---------------File---------------
File			BYTE 		MAX_PATH	dup(?)
FileTitle		BYTE 		MAX_PATH	dup(?)
bmpExtend		BYTE        ".bmp",0			; support bmp format
nStrLen     	DWORD 		?					; filename length
ischanged		DWORD		0
MaxWidth		DWORD		1900				; max picture width
MaxHeight		DWORD		750					; max picture height
setwidth        DWORD		800
setheight 		DWORD		600

;--------------Information and Dialog Strings---------------
ClassName   		BYTE 		'MyPainter', 0
StatusBarName   	BYTE 		'MyStatusBar', 0
WindowName 			BYTE 		'AwesomePainter', 0 
closeWindow			BYTE		'Are you sure to exit?', 0
closeWindowTitle	BYTE		'Exit', 0
invalidInput		BYTE		'Invalid input!', 0
invalidInputTitle	BYTE		'Invalid input', 0
Filter				BYTE		'image files(*.bmp)', 0, '*.bmp', 0
					BYTE		'all files(*.*)', 0, '*.*', 0, 0
DefType				BYTE		'bmp', 0
CannotOpenFile		BYTE		'Can not open file.', 0
statusText			BYTE		'AwesomePainter', 0
ErrorTitle  		BYTE 		'Error',0
changedWindow		BYTE		'Your file is changed, do you want to save?',0
changedWindowTitle	BYTE		'Save',0
UndoOnceTitle       BYTE      'Undo',0
UndoOnce            BYTE      'You can only undo one step',0


;--------------Version Information---------------
versionTitle		BYTE		'Version Info',0
version				BYTE		'     Drawing Board Based on Win32 ASM', 0dh, 0ah, 0dh,\
				  				'     Aauthor: LiuYong, LiuWei, MaYuQing', 0dh, 0ah,\
				  				'     Date: 2019,11,1', 0dh, 0ah,\
				  				'     Compiling Environment: MASM32', 0dh, 0ah,0


;---------------StatusBar---------------
;StatusBar_Tool String
penString   	BYTE 		'Pen', 0
brushString   	BYTE 		'Brush', 0
eraserString   	BYTE 		'Eraser', 0
selString   	BYTE 		'Select', 0
selOKString   	BYTE 		'Paste', 0
textString   	BYTE 		'Text', 0
bucketString   	BYTE 		'Bucket', 0
strawString   	BYTE 		'Straw', 0

;StatusBar_Shape String
lineStr			BYTE		'Line', 0
rectangleStr	BYTE		'Rectangle', 0
ellipseStr		BYTE		'Ellipse', 0
polygonStr		BYTE		'Polygon', 0
polylineStr		BYTE		'Polyline', 0
arcStr			BYTE		'Pie', 0
customStr		BYTE		'Custom', 0

;StatusBar_Mouse_Position String
mousePos  			BYTE		30 DUP(0)
posFmt       		BYTE    	'Pixel (%d, %d)',0    ;

;StatusBar_Pen_Width String
penWth				BYTE		30 DUP(0)	
widthFmt 			BYTE		'PW=%d', 0 

;StatusBar_Pen_Color String
penClr				BYTE		30 DUP(0)	
colorFmt 			BYTE		'Color=%x', 0

;StatusBar_Pen_Style String
penlikeStyle		BYTE		30 DUP(0)
styleFmt 			BYTE		'Style=%s', 0

;StatusBar_Shape String
shapeStr			BYTE		30 DUP(0)
shapeFmt 			BYTE		'Shape=%s', 0

;StatusBar_NULL String
nullstring			BYTE		'NULL', 0
StatusWidth  		DWORD  		105,200,250,300,400,500,630,750

;StatusBar Height
StatusBarHeight		DWORD		28

;---------------Error Handler---------------
pErrorMsg  DWORD ?		; ptr to error message
messageID  DWORD ?

;---------------Cursor Image ----------------
LastCursor    DWORD  NULL