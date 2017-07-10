setlocal

rem output dll location.
set src="D:\SrcFile\C\mqsdk456\mqsdk\Station_ObjectComposition\Win32\Debug\"

rem metasequoia station plugin location.
set dest="C:\Program Files (x86)\tetraface\Metasequoia4\Plugins\Station\"
copy %src%ObjectComposition.dll %dest%

rem need dll remote debug. metasequoia.exe location.
set dest="C:\Program Files (x86)\tetraface\Metasequoia4\"
copy %src%ObjectComposition.pdb %dest%


