# RadialController
RadialController(Surface Dial) sample code for win32 and UWP

The C++ sample code that uses the Radial Controller.
This project support both UWP and Desktop.
It is in the form of DLL + exe.
This project does not use .NET. Full native code.

For win32, the DLL uses WRL. Refer to my blog posting.
https://megayuchi.wordpress.com/2016/11/16/using-surface-dial-in-desktop-application-with-using-c/

There are some videos of my game project using this code.
https://youtu.be/bWENbUh4G7Q

For UWP, the DLL uses C++/WinRT(https://moderncpp.com/) and the Client uses C++/CX.

---

[Requirements]
-
1. Windows 10 build 14393 or later

2. Visual Studio 2015 update 3 or later

3. For Desktop version, If you encounter link errors in ddraw.lib, install the Windows Driver Kit.

---

[Build for Desktop App]
-
1. Build RadialContrlloer/Desktop/RadialDevice project. Then dll files are created in the RadialContrlloer/Desktop/DLL folder.
2. Build the RadialContrlloer/Desktop/Client project. Then Client_xxx_xxxx.exe files are crated in the RadialContrlloer/Desktop/App folder
3. When running in VS Debugger, set the working folder as '$(OutDir)' in Project Settings.

--

[Build for UWP App]
-
1. Build RadialContrlloer/UWP/RadialDevice project. Then dll files are created in the RadialContrlloer/UWP/DLL folder.
2. Build the RadialContrlloer/UWP/Client project. 

--
