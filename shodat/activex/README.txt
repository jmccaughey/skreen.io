afxsimple is the ActiveX counterpart to npsimple
------------------------------------------------

http://git.webvm.net/?p=afxsimple (Apache 2 license)
http://git.webvm.net/?p=npsimple (mpl-tri-license)

ActiveX is the mechanism you would employ to extend the Internet Explorer
platform. NPAPI is the interface you would use to extend browsers such as
Opera, Webkit and Gecko.

At Aplix we support both extension interfaces to allow our product WebVM to
work on as many Web platforms as possible.

http://wiki.webvm.net

Install on Win32
----------------

1. Install Cygwin with git and "cd c:"
2. Execute `git clone git://git.webvm.net/afxsimple`
3. Build afxsimple.sln with Visual Studio 2005 with the 'Win32' target
4. Execute afxsimple.reg to import registry settings
5. Append System variable PATH with ;C:\afxsimple
6. Open C:\afxsimple\afxsimple.html in Internet Explorer

Caveats
-------

Internet Explorer 8 seems to run the control in a seperate process making
debugging a bit of a pain like Opera's plugin wrapper. Use IE7 instead.

You need a full version of Visual Studio (not Express) to build for ARM/Windows
Mobile devices.
