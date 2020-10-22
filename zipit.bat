echo off
set build=%1
set fileName=Build_%build%.zip
echo Creating zip file %fileName% ...
zip -r -v %fileName% ^
BehavePlus6_Solution.sln ^
*.cpp ^
*.h ^
BehavePlus6.xml ^
License.rtf ^
wix_bp6_product_%build%.wxs ^
wix_bp6_directory.wxs ^
wix_bp6_feature.wxs ^
wix_bp6_component_executable.wxs ^
wix_bp6_component_datafolder_%build%.wxs ^
wix_bp6_component_docfolder_%build%.wxs ^
wix_bp6_component_imagefolder.wxs ^
wix_bp6_ui.wxs ^
wix_bp6_dialog_installdir.wxs ^
wix_bp6_dialog_licenseagreement.wxs ^
wix_bp6_dialog_prerelease.wxs ^
wix_bp6_dialog_welcome.wxs ^
Algorithms/* ^
tmp/moc/* ^
DocFolder/en_US/Html/* ^
DocFolder/GeoFolder/* ^
DefaultDataFolder/* ^
DLL/* ^
ImageFolder/* ^
Wix/*
echo Created zip file %fileName%
