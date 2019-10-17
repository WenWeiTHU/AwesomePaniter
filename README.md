# AwesomePaniter配置说明：

右键项目名->属性 \n
链接器->常规->附加库，填到masm32库的路径 如C:\masm32\lib
链接器->输入->依赖项  kernel32.lib;user32.lib;gdi32.lib
链接器->系统->子系统 /SUBSYSTEM:WINDOWS
Microsoft Macro Assembler,填到masm32头文件们所在的路径 如C:\masm32\include

> 不需要设置入口点
