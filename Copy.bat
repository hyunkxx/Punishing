// // 명령어			옵션			원본파일이 있는 위치					사본 파일을 저장할 위치
	
xcopy			/y			.\Engine\Bin\Engine.dll			.\Client\Bin\

xcopy			/y			.\Engine\Bin\Engine.lib			.\Reference\Librarys\
xcopy			/y/s		.\Engine\Public\*.*				.\Reference\Headers\