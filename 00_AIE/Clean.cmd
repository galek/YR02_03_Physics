@echo off
FOR /R %%G in (.) DO (
Pushd %%G
echo %%G
RD /S /Q ".\ipch"
RD /S /Q ".\debug"
RD /S /Q ".\release"
Popd )
del "*.sdf"
attrib -h "*.suo"
del "*.suo"
echo All done. should be clean for transferal :D
pause