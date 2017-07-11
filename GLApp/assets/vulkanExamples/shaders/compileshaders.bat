for /r %%i in (*.frag *.vert *.geom *.comp *.tesc *.tess *.tese) do (
	glslangValidator.exe -V %%i -o %%i.spv
)

pause