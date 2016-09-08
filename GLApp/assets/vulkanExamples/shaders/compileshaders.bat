for /r %%i in (*.frag *.vert *.geom *.comp *.tesc *.tess) do (
	glslangValidator.exe -V %%i -o %%i.spv
)

pause