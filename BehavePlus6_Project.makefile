# Compiler flags...
CPP_COMPILER = g++
C_COMPILER = gcc

# Include paths...
Debug_Include_Path=-I"../../../../Qt/3.3.8/include" 
Release_Include_Path=-I"../../../../Qt/3.3.8/include" 

# Library paths...
Debug_Library_Path=-L"../../../../Qt/3.3.8/gcclib" 
Release_Library_Path=-L"../../../../Qt/3.3.8/gcclib" 

# Additional libraries...
Debug_Libraries=-Wl,--start-group -lqt-mt338 -lqtmain  -Wl,--end-group
Release_Libraries=-Wl,--start-group -lqt-mt338 -lqtmain  -Wl,--end-group

# Preprocessor definitions...
Debug_Preprocessor_Definitions=-D GCC_BUILD -D _DEBUG -D _WINDOWS -D QT_DLL -D QT_THREAD_SUPPORT -D QT_NO_DEBUG -D UNICODE 
Release_Preprocessor_Definitions=-D GCC_BUILD -D NDEBUG -D QT_DLL -D QT_NO_DEBUG -D QT_THREAD_SUPPORT -D UNICODE 

# Implictly linked object files...
Debug_Implicitly_Linked_Objects=
Release_Implicitly_Linked_Objects=

# Compiler flags...
Debug_Compiler_Flags=-w -O0 -g 
Release_Compiler_Flags=

# Builds all configurations for this project...
.PHONY: build_all_configurations
build_all_configurations: Debug Release 

# Builds the Debug configuration...
.PHONY: Debug
Debug: create_folders gccDebug/aboutdialog.o gccDebug/appdialog.o gccDebug/appearancedialog.o gccDebug/appfilesystem.o gccDebug/appmessage.o gccDebug/appproperty.o gccDebug/appsiunits.o gccDebug/apptranslator.o gccDebug/appwindow.o gccDebug/attachdialog.o gccDebug/BehavePlus6.o gccDebug/Bp6ChaparralFuel.o gccDebug/Bp6ChaparralFuelChamise.o gccDebug/Bp6ChaparralFuelMixedBrush.o gccDebug/Bp6CrownFire.o gccDebug/Bp6FuelModel10.o gccDebug/Bp6SurfaceFire.o gccDebug/bpcomposecontaindiagram.o gccDebug/bpcomposedoc.o gccDebug/bpcomposefiredirdiagram.o gccDebug/bpcomposefireshapediagram.o gccDebug/bpcomposegraphs.o gccDebug/bpcomposehaulingchart.o gccDebug/bpcomposelogo.o gccDebug/bpcomposepage.o gccDebug/bpcomposetable1.o gccDebug/bpcomposetable2.o gccDebug/bpcomposetable3.o gccDebug/bpcomposeworksheet.o gccDebug/bpdocentry.o gccDebug/bpdocument.o gccDebug/bpfile.o gccDebug/calendardocument.o gccDebug/cdtlib.o gccDebug/composer.o gccDebug/conflictdialog.o gccDebug/contain.o gccDebug/Contain6.o gccDebug/ContainForce6.o gccDebug/ContainResource6.o gccDebug/ContainSim6.o gccDebug/datetime.o gccDebug/docdevicesize.o gccDebug/docpagesize.o gccDebug/docscrollview.o gccDebug/doctabs.o gccDebug/document.o gccDebug/fdfmcdialog.o gccDebug/fileselector.o gccDebug/filesystem.o gccDebug/fuelexportdialog.o gccDebug/fuelinitdialog.o gccDebug/fuelmodel.o gccDebug/fuelmodeldialog.o gccDebug/fuelwizards.o gccDebug/globalposition.o gccDebug/globalsite.o gccDebug/graph.o gccDebug/graphaxle.o gccDebug/graphbar.o gccDebug/graphlimitsdialog.o gccDebug/graphline.o gccDebug/graphmarker.o gccDebug/guidedialog.o gccDebug/helpbrowser.o gccDebug/horizontaldistancedialog.o gccDebug/humiditydialog.o gccDebug/module.o gccDebug/modulesdialog.o gccDebug/moisscenario.o gccDebug/newext.o gccDebug/pagemenudialog.o gccDebug/parser.o gccDebug/platform-windows.o gccDebug/printer.o gccDebug/property.o gccDebug/propertydialog.o gccDebug/randfuel.o gccDebug/randthread.o gccDebug/realspinbox.o gccDebug/requestdialog.o gccDebug/rundialog.o gccDebug/rxvar.o gccDebug/SemFireCrownFirebrandProcessor.o gccDebug/siunits.o gccDebug/slopetooldialog.o gccDebug/standardwizards.o gccDebug/stdafx.o gccDebug/sundialog.o gccDebug/sunview.o gccDebug/textview.o gccDebug/textviewdocument.o gccDebug/toc.o gccDebug/unitsconverterdialog.o gccDebug/unitseditdialog.o gccDebug/varcheckbox.o gccDebug/wizarddialog.o gccDebug/xeqapp.o gccDebug/xeqappparser.o gccDebug/xeqcalc.o gccDebug/xeqcalcmask.o gccDebug/xeqcalcreconfig.o gccDebug/xeqcalcV6Crown.o gccDebug/xeqcalcV6Main.o gccDebug/xeqfile.o gccDebug/xeqtree.o gccDebug/xeqtreeparser.o gccDebug/xeqtreeprint.o gccDebug/xeqvar.o gccDebug/xeqvaritem.o gccDebug/xfblib.o gccDebug/xmlparser.o gccDebug/tmp/moc/moc_aboutdialog.o gccDebug/tmp/moc/moc_appdialog.o gccDebug/tmp/moc/moc_appearancedialog.o gccDebug/tmp/moc/moc_appmessage.o gccDebug/tmp/moc/moc_appwindow.o gccDebug/tmp/moc/moc_attachdialog.o gccDebug/tmp/moc/moc_bpdocument.o gccDebug/tmp/moc/moc_calendardocument.o gccDebug/tmp/moc/moc_conflictdialog.o gccDebug/tmp/moc/moc_document.o gccDebug/tmp/moc/moc_fdfmcdialog.o gccDebug/tmp/moc/moc_fileselector.o gccDebug/tmp/moc/moc_fuelexportdialog.o gccDebug/tmp/moc/moc_fuelinitdialog.o gccDebug/tmp/moc/moc_fuelmodeldialog.o gccDebug/tmp/moc/moc_fuelwizards.o gccDebug/tmp/moc/moc_graphlimitsdialog.o gccDebug/tmp/moc/moc_guidedialog.o gccDebug/tmp/moc/moc_helpbrowser.o gccDebug/tmp/moc/moc_horizontaldistancedialog.o gccDebug/tmp/moc/moc_humiditydialog.o gccDebug/tmp/moc/moc_modulesdialog.o gccDebug/tmp/moc/moc_pagemenudialog.o gccDebug/tmp/moc/moc_propertydialog.o gccDebug/tmp/moc/moc_realspinbox.o gccDebug/tmp/moc/moc_rundialog.o gccDebug/tmp/moc/moc_slopetooldialog.o gccDebug/tmp/moc/moc_standardwizards.o gccDebug/tmp/moc/moc_sundialog.o gccDebug/tmp/moc/moc_textview.o gccDebug/tmp/moc/moc_textviewdocument.o gccDebug/tmp/moc/moc_unitsconverterdialog.o gccDebug/tmp/moc/moc_unitseditdialog.o gccDebug/tmp/moc/moc_varcheckbox.o gccDebug/tmp/moc/moc_wizarddialog.o gccDebug/Algorithms/Fofem6BarkThickness.o gccDebug/Algorithms/Fofem6Mortality.o gccDebug/Algorithms/Fofem6Species.o gccDebug/Algorithms/Fofem6SpeciesData.o 
	g++ gccDebug/aboutdialog.o gccDebug/appdialog.o gccDebug/appearancedialog.o gccDebug/appfilesystem.o gccDebug/appmessage.o gccDebug/appproperty.o gccDebug/appsiunits.o gccDebug/apptranslator.o gccDebug/appwindow.o gccDebug/attachdialog.o gccDebug/BehavePlus6.o gccDebug/Bp6ChaparralFuel.o gccDebug/Bp6ChaparralFuelChamise.o gccDebug/Bp6ChaparralFuelMixedBrush.o gccDebug/Bp6CrownFire.o gccDebug/Bp6FuelModel10.o gccDebug/Bp6SurfaceFire.o gccDebug/bpcomposecontaindiagram.o gccDebug/bpcomposedoc.o gccDebug/bpcomposefiredirdiagram.o gccDebug/bpcomposefireshapediagram.o gccDebug/bpcomposegraphs.o gccDebug/bpcomposehaulingchart.o gccDebug/bpcomposelogo.o gccDebug/bpcomposepage.o gccDebug/bpcomposetable1.o gccDebug/bpcomposetable2.o gccDebug/bpcomposetable3.o gccDebug/bpcomposeworksheet.o gccDebug/bpdocentry.o gccDebug/bpdocument.o gccDebug/bpfile.o gccDebug/calendardocument.o gccDebug/cdtlib.o gccDebug/composer.o gccDebug/conflictdialog.o gccDebug/contain.o gccDebug/Contain6.o gccDebug/ContainForce6.o gccDebug/ContainResource6.o gccDebug/ContainSim6.o gccDebug/datetime.o gccDebug/docdevicesize.o gccDebug/docpagesize.o gccDebug/docscrollview.o gccDebug/doctabs.o gccDebug/document.o gccDebug/fdfmcdialog.o gccDebug/fileselector.o gccDebug/filesystem.o gccDebug/fuelexportdialog.o gccDebug/fuelinitdialog.o gccDebug/fuelmodel.o gccDebug/fuelmodeldialog.o gccDebug/fuelwizards.o gccDebug/globalposition.o gccDebug/globalsite.o gccDebug/graph.o gccDebug/graphaxle.o gccDebug/graphbar.o gccDebug/graphlimitsdialog.o gccDebug/graphline.o gccDebug/graphmarker.o gccDebug/guidedialog.o gccDebug/helpbrowser.o gccDebug/horizontaldistancedialog.o gccDebug/humiditydialog.o gccDebug/module.o gccDebug/modulesdialog.o gccDebug/moisscenario.o gccDebug/newext.o gccDebug/pagemenudialog.o gccDebug/parser.o gccDebug/platform-windows.o gccDebug/printer.o gccDebug/property.o gccDebug/propertydialog.o gccDebug/randfuel.o gccDebug/randthread.o gccDebug/realspinbox.o gccDebug/requestdialog.o gccDebug/rundialog.o gccDebug/rxvar.o gccDebug/SemFireCrownFirebrandProcessor.o gccDebug/siunits.o gccDebug/slopetooldialog.o gccDebug/standardwizards.o gccDebug/stdafx.o gccDebug/sundialog.o gccDebug/sunview.o gccDebug/textview.o gccDebug/textviewdocument.o gccDebug/toc.o gccDebug/unitsconverterdialog.o gccDebug/unitseditdialog.o gccDebug/varcheckbox.o gccDebug/wizarddialog.o gccDebug/xeqapp.o gccDebug/xeqappparser.o gccDebug/xeqcalc.o gccDebug/xeqcalcmask.o gccDebug/xeqcalcreconfig.o gccDebug/xeqcalcV6Crown.o gccDebug/xeqcalcV6Main.o gccDebug/xeqfile.o gccDebug/xeqtree.o gccDebug/xeqtreeparser.o gccDebug/xeqtreeprint.o gccDebug/xeqvar.o gccDebug/xeqvaritem.o gccDebug/xfblib.o gccDebug/xmlparser.o gccDebug/tmp/moc/moc_aboutdialog.o gccDebug/tmp/moc/moc_appdialog.o gccDebug/tmp/moc/moc_appearancedialog.o gccDebug/tmp/moc/moc_appmessage.o gccDebug/tmp/moc/moc_appwindow.o gccDebug/tmp/moc/moc_attachdialog.o gccDebug/tmp/moc/moc_bpdocument.o gccDebug/tmp/moc/moc_calendardocument.o gccDebug/tmp/moc/moc_conflictdialog.o gccDebug/tmp/moc/moc_document.o gccDebug/tmp/moc/moc_fdfmcdialog.o gccDebug/tmp/moc/moc_fileselector.o gccDebug/tmp/moc/moc_fuelexportdialog.o gccDebug/tmp/moc/moc_fuelinitdialog.o gccDebug/tmp/moc/moc_fuelmodeldialog.o gccDebug/tmp/moc/moc_fuelwizards.o gccDebug/tmp/moc/moc_graphlimitsdialog.o gccDebug/tmp/moc/moc_guidedialog.o gccDebug/tmp/moc/moc_helpbrowser.o gccDebug/tmp/moc/moc_horizontaldistancedialog.o gccDebug/tmp/moc/moc_humiditydialog.o gccDebug/tmp/moc/moc_modulesdialog.o gccDebug/tmp/moc/moc_pagemenudialog.o gccDebug/tmp/moc/moc_propertydialog.o gccDebug/tmp/moc/moc_realspinbox.o gccDebug/tmp/moc/moc_rundialog.o gccDebug/tmp/moc/moc_slopetooldialog.o gccDebug/tmp/moc/moc_standardwizards.o gccDebug/tmp/moc/moc_sundialog.o gccDebug/tmp/moc/moc_textview.o gccDebug/tmp/moc/moc_textviewdocument.o gccDebug/tmp/moc/moc_unitsconverterdialog.o gccDebug/tmp/moc/moc_unitseditdialog.o gccDebug/tmp/moc/moc_varcheckbox.o gccDebug/tmp/moc/moc_wizarddialog.o gccDebug/Algorithms/Fofem6BarkThickness.o gccDebug/Algorithms/Fofem6Mortality.o gccDebug/Algorithms/Fofem6Species.o gccDebug/Algorithms/Fofem6SpeciesData.o  $(Debug_Library_Path) $(Debug_Libraries) -Wl,-rpath,./ -o gccDebug/BehavePlus6_Project.exe

# Compiles file aboutdialog.cpp for the Debug configuration...
-include gccDebug/aboutdialog.d
gccDebug/aboutdialog.o: aboutdialog.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c aboutdialog.cpp $(Debug_Include_Path) -o gccDebug/aboutdialog.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM aboutdialog.cpp $(Debug_Include_Path) > gccDebug/aboutdialog.d

# Compiles file appdialog.cpp for the Debug configuration...
-include gccDebug/appdialog.d
gccDebug/appdialog.o: appdialog.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c appdialog.cpp $(Debug_Include_Path) -o gccDebug/appdialog.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM appdialog.cpp $(Debug_Include_Path) > gccDebug/appdialog.d

# Compiles file appearancedialog.cpp for the Debug configuration...
-include gccDebug/appearancedialog.d
gccDebug/appearancedialog.o: appearancedialog.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c appearancedialog.cpp $(Debug_Include_Path) -o gccDebug/appearancedialog.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM appearancedialog.cpp $(Debug_Include_Path) > gccDebug/appearancedialog.d

# Compiles file appfilesystem.cpp for the Debug configuration...
-include gccDebug/appfilesystem.d
gccDebug/appfilesystem.o: appfilesystem.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c appfilesystem.cpp $(Debug_Include_Path) -o gccDebug/appfilesystem.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM appfilesystem.cpp $(Debug_Include_Path) > gccDebug/appfilesystem.d

# Compiles file appmessage.cpp for the Debug configuration...
-include gccDebug/appmessage.d
gccDebug/appmessage.o: appmessage.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c appmessage.cpp $(Debug_Include_Path) -o gccDebug/appmessage.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM appmessage.cpp $(Debug_Include_Path) > gccDebug/appmessage.d

# Compiles file appproperty.cpp for the Debug configuration...
-include gccDebug/appproperty.d
gccDebug/appproperty.o: appproperty.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c appproperty.cpp $(Debug_Include_Path) -o gccDebug/appproperty.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM appproperty.cpp $(Debug_Include_Path) > gccDebug/appproperty.d

# Compiles file appsiunits.cpp for the Debug configuration...
-include gccDebug/appsiunits.d
gccDebug/appsiunits.o: appsiunits.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c appsiunits.cpp $(Debug_Include_Path) -o gccDebug/appsiunits.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM appsiunits.cpp $(Debug_Include_Path) > gccDebug/appsiunits.d

# Compiles file apptranslator.cpp for the Debug configuration...
-include gccDebug/apptranslator.d
gccDebug/apptranslator.o: apptranslator.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c apptranslator.cpp $(Debug_Include_Path) -o gccDebug/apptranslator.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM apptranslator.cpp $(Debug_Include_Path) > gccDebug/apptranslator.d

# Compiles file appwindow.cpp for the Debug configuration...
-include gccDebug/appwindow.d
gccDebug/appwindow.o: appwindow.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c appwindow.cpp $(Debug_Include_Path) -o gccDebug/appwindow.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM appwindow.cpp $(Debug_Include_Path) > gccDebug/appwindow.d

# Compiles file attachdialog.cpp for the Debug configuration...
-include gccDebug/attachdialog.d
gccDebug/attachdialog.o: attachdialog.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c attachdialog.cpp $(Debug_Include_Path) -o gccDebug/attachdialog.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM attachdialog.cpp $(Debug_Include_Path) > gccDebug/attachdialog.d

# Compiles file BehavePlus6.cpp for the Debug configuration...
-include gccDebug/BehavePlus6.d
gccDebug/BehavePlus6.o: BehavePlus6.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c BehavePlus6.cpp $(Debug_Include_Path) -o gccDebug/BehavePlus6.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM BehavePlus6.cpp $(Debug_Include_Path) > gccDebug/BehavePlus6.d

# Compiles file Bp6ChaparralFuel.cpp for the Debug configuration...
-include gccDebug/Bp6ChaparralFuel.d
gccDebug/Bp6ChaparralFuel.o: Bp6ChaparralFuel.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c Bp6ChaparralFuel.cpp $(Debug_Include_Path) -o gccDebug/Bp6ChaparralFuel.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM Bp6ChaparralFuel.cpp $(Debug_Include_Path) > gccDebug/Bp6ChaparralFuel.d

# Compiles file Bp6ChaparralFuelChamise.cpp for the Debug configuration...
-include gccDebug/Bp6ChaparralFuelChamise.d
gccDebug/Bp6ChaparralFuelChamise.o: Bp6ChaparralFuelChamise.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c Bp6ChaparralFuelChamise.cpp $(Debug_Include_Path) -o gccDebug/Bp6ChaparralFuelChamise.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM Bp6ChaparralFuelChamise.cpp $(Debug_Include_Path) > gccDebug/Bp6ChaparralFuelChamise.d

# Compiles file Bp6ChaparralFuelMixedBrush.cpp for the Debug configuration...
-include gccDebug/Bp6ChaparralFuelMixedBrush.d
gccDebug/Bp6ChaparralFuelMixedBrush.o: Bp6ChaparralFuelMixedBrush.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c Bp6ChaparralFuelMixedBrush.cpp $(Debug_Include_Path) -o gccDebug/Bp6ChaparralFuelMixedBrush.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM Bp6ChaparralFuelMixedBrush.cpp $(Debug_Include_Path) > gccDebug/Bp6ChaparralFuelMixedBrush.d

# Compiles file Bp6CrownFire.cpp for the Debug configuration...
-include gccDebug/Bp6CrownFire.d
gccDebug/Bp6CrownFire.o: Bp6CrownFire.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c Bp6CrownFire.cpp $(Debug_Include_Path) -o gccDebug/Bp6CrownFire.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM Bp6CrownFire.cpp $(Debug_Include_Path) > gccDebug/Bp6CrownFire.d

# Compiles file Bp6FuelModel10.cpp for the Debug configuration...
-include gccDebug/Bp6FuelModel10.d
gccDebug/Bp6FuelModel10.o: Bp6FuelModel10.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c Bp6FuelModel10.cpp $(Debug_Include_Path) -o gccDebug/Bp6FuelModel10.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM Bp6FuelModel10.cpp $(Debug_Include_Path) > gccDebug/Bp6FuelModel10.d

# Compiles file Bp6SurfaceFire.cpp for the Debug configuration...
-include gccDebug/Bp6SurfaceFire.d
gccDebug/Bp6SurfaceFire.o: Bp6SurfaceFire.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c Bp6SurfaceFire.cpp $(Debug_Include_Path) -o gccDebug/Bp6SurfaceFire.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM Bp6SurfaceFire.cpp $(Debug_Include_Path) > gccDebug/Bp6SurfaceFire.d

# Compiles file bpcomposecontaindiagram.cpp for the Debug configuration...
-include gccDebug/bpcomposecontaindiagram.d
gccDebug/bpcomposecontaindiagram.o: bpcomposecontaindiagram.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c bpcomposecontaindiagram.cpp $(Debug_Include_Path) -o gccDebug/bpcomposecontaindiagram.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM bpcomposecontaindiagram.cpp $(Debug_Include_Path) > gccDebug/bpcomposecontaindiagram.d

# Compiles file bpcomposedoc.cpp for the Debug configuration...
-include gccDebug/bpcomposedoc.d
gccDebug/bpcomposedoc.o: bpcomposedoc.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c bpcomposedoc.cpp $(Debug_Include_Path) -o gccDebug/bpcomposedoc.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM bpcomposedoc.cpp $(Debug_Include_Path) > gccDebug/bpcomposedoc.d

# Compiles file bpcomposefiredirdiagram.cpp for the Debug configuration...
-include gccDebug/bpcomposefiredirdiagram.d
gccDebug/bpcomposefiredirdiagram.o: bpcomposefiredirdiagram.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c bpcomposefiredirdiagram.cpp $(Debug_Include_Path) -o gccDebug/bpcomposefiredirdiagram.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM bpcomposefiredirdiagram.cpp $(Debug_Include_Path) > gccDebug/bpcomposefiredirdiagram.d

# Compiles file bpcomposefireshapediagram.cpp for the Debug configuration...
-include gccDebug/bpcomposefireshapediagram.d
gccDebug/bpcomposefireshapediagram.o: bpcomposefireshapediagram.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c bpcomposefireshapediagram.cpp $(Debug_Include_Path) -o gccDebug/bpcomposefireshapediagram.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM bpcomposefireshapediagram.cpp $(Debug_Include_Path) > gccDebug/bpcomposefireshapediagram.d

# Compiles file bpcomposegraphs.cpp for the Debug configuration...
-include gccDebug/bpcomposegraphs.d
gccDebug/bpcomposegraphs.o: bpcomposegraphs.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c bpcomposegraphs.cpp $(Debug_Include_Path) -o gccDebug/bpcomposegraphs.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM bpcomposegraphs.cpp $(Debug_Include_Path) > gccDebug/bpcomposegraphs.d

# Compiles file bpcomposehaulingchart.cpp for the Debug configuration...
-include gccDebug/bpcomposehaulingchart.d
gccDebug/bpcomposehaulingchart.o: bpcomposehaulingchart.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c bpcomposehaulingchart.cpp $(Debug_Include_Path) -o gccDebug/bpcomposehaulingchart.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM bpcomposehaulingchart.cpp $(Debug_Include_Path) > gccDebug/bpcomposehaulingchart.d

# Compiles file bpcomposelogo.cpp for the Debug configuration...
-include gccDebug/bpcomposelogo.d
gccDebug/bpcomposelogo.o: bpcomposelogo.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c bpcomposelogo.cpp $(Debug_Include_Path) -o gccDebug/bpcomposelogo.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM bpcomposelogo.cpp $(Debug_Include_Path) > gccDebug/bpcomposelogo.d

# Compiles file bpcomposepage.cpp for the Debug configuration...
-include gccDebug/bpcomposepage.d
gccDebug/bpcomposepage.o: bpcomposepage.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c bpcomposepage.cpp $(Debug_Include_Path) -o gccDebug/bpcomposepage.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM bpcomposepage.cpp $(Debug_Include_Path) > gccDebug/bpcomposepage.d

# Compiles file bpcomposetable1.cpp for the Debug configuration...
-include gccDebug/bpcomposetable1.d
gccDebug/bpcomposetable1.o: bpcomposetable1.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c bpcomposetable1.cpp $(Debug_Include_Path) -o gccDebug/bpcomposetable1.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM bpcomposetable1.cpp $(Debug_Include_Path) > gccDebug/bpcomposetable1.d

# Compiles file bpcomposetable2.cpp for the Debug configuration...
-include gccDebug/bpcomposetable2.d
gccDebug/bpcomposetable2.o: bpcomposetable2.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c bpcomposetable2.cpp $(Debug_Include_Path) -o gccDebug/bpcomposetable2.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM bpcomposetable2.cpp $(Debug_Include_Path) > gccDebug/bpcomposetable2.d

# Compiles file bpcomposetable3.cpp for the Debug configuration...
-include gccDebug/bpcomposetable3.d
gccDebug/bpcomposetable3.o: bpcomposetable3.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c bpcomposetable3.cpp $(Debug_Include_Path) -o gccDebug/bpcomposetable3.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM bpcomposetable3.cpp $(Debug_Include_Path) > gccDebug/bpcomposetable3.d

# Compiles file bpcomposeworksheet.cpp for the Debug configuration...
-include gccDebug/bpcomposeworksheet.d
gccDebug/bpcomposeworksheet.o: bpcomposeworksheet.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c bpcomposeworksheet.cpp $(Debug_Include_Path) -o gccDebug/bpcomposeworksheet.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM bpcomposeworksheet.cpp $(Debug_Include_Path) > gccDebug/bpcomposeworksheet.d

# Compiles file bpdocentry.cpp for the Debug configuration...
-include gccDebug/bpdocentry.d
gccDebug/bpdocentry.o: bpdocentry.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c bpdocentry.cpp $(Debug_Include_Path) -o gccDebug/bpdocentry.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM bpdocentry.cpp $(Debug_Include_Path) > gccDebug/bpdocentry.d

# Compiles file bpdocument.cpp for the Debug configuration...
-include gccDebug/bpdocument.d
gccDebug/bpdocument.o: bpdocument.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c bpdocument.cpp $(Debug_Include_Path) -o gccDebug/bpdocument.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM bpdocument.cpp $(Debug_Include_Path) > gccDebug/bpdocument.d

# Compiles file bpfile.cpp for the Debug configuration...
-include gccDebug/bpfile.d
gccDebug/bpfile.o: bpfile.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c bpfile.cpp $(Debug_Include_Path) -o gccDebug/bpfile.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM bpfile.cpp $(Debug_Include_Path) > gccDebug/bpfile.d

# Compiles file calendardocument.cpp for the Debug configuration...
-include gccDebug/calendardocument.d
gccDebug/calendardocument.o: calendardocument.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c calendardocument.cpp $(Debug_Include_Path) -o gccDebug/calendardocument.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM calendardocument.cpp $(Debug_Include_Path) > gccDebug/calendardocument.d

# Compiles file cdtlib.c for the Debug configuration...
-include gccDebug/cdtlib.d
gccDebug/cdtlib.o: cdtlib.c
	$(C_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c cdtlib.c $(Debug_Include_Path) -o gccDebug/cdtlib.o
	$(C_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM cdtlib.c $(Debug_Include_Path) > gccDebug/cdtlib.d

# Compiles file composer.cpp for the Debug configuration...
-include gccDebug/composer.d
gccDebug/composer.o: composer.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c composer.cpp $(Debug_Include_Path) -o gccDebug/composer.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM composer.cpp $(Debug_Include_Path) > gccDebug/composer.d

# Compiles file conflictdialog.cpp for the Debug configuration...
-include gccDebug/conflictdialog.d
gccDebug/conflictdialog.o: conflictdialog.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c conflictdialog.cpp $(Debug_Include_Path) -o gccDebug/conflictdialog.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM conflictdialog.cpp $(Debug_Include_Path) > gccDebug/conflictdialog.d

# Compiles file contain.cpp for the Debug configuration...
-include gccDebug/contain.d
gccDebug/contain.o: contain.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c contain.cpp $(Debug_Include_Path) -o gccDebug/contain.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM contain.cpp $(Debug_Include_Path) > gccDebug/contain.d

# Compiles file Contain6.cpp for the Debug configuration...
-include gccDebug/Contain6.d
gccDebug/Contain6.o: Contain6.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c Contain6.cpp $(Debug_Include_Path) -o gccDebug/Contain6.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM Contain6.cpp $(Debug_Include_Path) > gccDebug/Contain6.d

# Compiles file ContainForce6.cpp for the Debug configuration...
-include gccDebug/ContainForce6.d
gccDebug/ContainForce6.o: ContainForce6.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c ContainForce6.cpp $(Debug_Include_Path) -o gccDebug/ContainForce6.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM ContainForce6.cpp $(Debug_Include_Path) > gccDebug/ContainForce6.d

# Compiles file ContainResource6.cpp for the Debug configuration...
-include gccDebug/ContainResource6.d
gccDebug/ContainResource6.o: ContainResource6.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c ContainResource6.cpp $(Debug_Include_Path) -o gccDebug/ContainResource6.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM ContainResource6.cpp $(Debug_Include_Path) > gccDebug/ContainResource6.d

# Compiles file ContainSim6.cpp for the Debug configuration...
-include gccDebug/ContainSim6.d
gccDebug/ContainSim6.o: ContainSim6.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c ContainSim6.cpp $(Debug_Include_Path) -o gccDebug/ContainSim6.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM ContainSim6.cpp $(Debug_Include_Path) > gccDebug/ContainSim6.d

# Compiles file datetime.cpp for the Debug configuration...
-include gccDebug/datetime.d
gccDebug/datetime.o: datetime.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c datetime.cpp $(Debug_Include_Path) -o gccDebug/datetime.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM datetime.cpp $(Debug_Include_Path) > gccDebug/datetime.d

# Compiles file docdevicesize.cpp for the Debug configuration...
-include gccDebug/docdevicesize.d
gccDebug/docdevicesize.o: docdevicesize.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c docdevicesize.cpp $(Debug_Include_Path) -o gccDebug/docdevicesize.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM docdevicesize.cpp $(Debug_Include_Path) > gccDebug/docdevicesize.d

# Compiles file docpagesize.cpp for the Debug configuration...
-include gccDebug/docpagesize.d
gccDebug/docpagesize.o: docpagesize.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c docpagesize.cpp $(Debug_Include_Path) -o gccDebug/docpagesize.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM docpagesize.cpp $(Debug_Include_Path) > gccDebug/docpagesize.d

# Compiles file docscrollview.cpp for the Debug configuration...
-include gccDebug/docscrollview.d
gccDebug/docscrollview.o: docscrollview.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c docscrollview.cpp $(Debug_Include_Path) -o gccDebug/docscrollview.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM docscrollview.cpp $(Debug_Include_Path) > gccDebug/docscrollview.d

# Compiles file doctabs.cpp for the Debug configuration...
-include gccDebug/doctabs.d
gccDebug/doctabs.o: doctabs.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c doctabs.cpp $(Debug_Include_Path) -o gccDebug/doctabs.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM doctabs.cpp $(Debug_Include_Path) > gccDebug/doctabs.d

# Compiles file document.cpp for the Debug configuration...
-include gccDebug/document.d
gccDebug/document.o: document.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c document.cpp $(Debug_Include_Path) -o gccDebug/document.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM document.cpp $(Debug_Include_Path) > gccDebug/document.d

# Compiles file fdfmcdialog.cpp for the Debug configuration...
-include gccDebug/fdfmcdialog.d
gccDebug/fdfmcdialog.o: fdfmcdialog.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c fdfmcdialog.cpp $(Debug_Include_Path) -o gccDebug/fdfmcdialog.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM fdfmcdialog.cpp $(Debug_Include_Path) > gccDebug/fdfmcdialog.d

# Compiles file fileselector.cpp for the Debug configuration...
-include gccDebug/fileselector.d
gccDebug/fileselector.o: fileselector.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c fileselector.cpp $(Debug_Include_Path) -o gccDebug/fileselector.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM fileselector.cpp $(Debug_Include_Path) > gccDebug/fileselector.d

# Compiles file filesystem.cpp for the Debug configuration...
-include gccDebug/filesystem.d
gccDebug/filesystem.o: filesystem.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c filesystem.cpp $(Debug_Include_Path) -o gccDebug/filesystem.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM filesystem.cpp $(Debug_Include_Path) > gccDebug/filesystem.d

# Compiles file fuelexportdialog.cpp for the Debug configuration...
-include gccDebug/fuelexportdialog.d
gccDebug/fuelexportdialog.o: fuelexportdialog.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c fuelexportdialog.cpp $(Debug_Include_Path) -o gccDebug/fuelexportdialog.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM fuelexportdialog.cpp $(Debug_Include_Path) > gccDebug/fuelexportdialog.d

# Compiles file fuelinitdialog.cpp for the Debug configuration...
-include gccDebug/fuelinitdialog.d
gccDebug/fuelinitdialog.o: fuelinitdialog.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c fuelinitdialog.cpp $(Debug_Include_Path) -o gccDebug/fuelinitdialog.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM fuelinitdialog.cpp $(Debug_Include_Path) > gccDebug/fuelinitdialog.d

# Compiles file fuelmodel.cpp for the Debug configuration...
-include gccDebug/fuelmodel.d
gccDebug/fuelmodel.o: fuelmodel.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c fuelmodel.cpp $(Debug_Include_Path) -o gccDebug/fuelmodel.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM fuelmodel.cpp $(Debug_Include_Path) > gccDebug/fuelmodel.d

# Compiles file fuelmodeldialog.cpp for the Debug configuration...
-include gccDebug/fuelmodeldialog.d
gccDebug/fuelmodeldialog.o: fuelmodeldialog.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c fuelmodeldialog.cpp $(Debug_Include_Path) -o gccDebug/fuelmodeldialog.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM fuelmodeldialog.cpp $(Debug_Include_Path) > gccDebug/fuelmodeldialog.d

# Compiles file fuelwizards.cpp for the Debug configuration...
-include gccDebug/fuelwizards.d
gccDebug/fuelwizards.o: fuelwizards.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c fuelwizards.cpp $(Debug_Include_Path) -o gccDebug/fuelwizards.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM fuelwizards.cpp $(Debug_Include_Path) > gccDebug/fuelwizards.d

# Compiles file globalposition.cpp for the Debug configuration...
-include gccDebug/globalposition.d
gccDebug/globalposition.o: globalposition.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c globalposition.cpp $(Debug_Include_Path) -o gccDebug/globalposition.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM globalposition.cpp $(Debug_Include_Path) > gccDebug/globalposition.d

# Compiles file globalsite.cpp for the Debug configuration...
-include gccDebug/globalsite.d
gccDebug/globalsite.o: globalsite.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c globalsite.cpp $(Debug_Include_Path) -o gccDebug/globalsite.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM globalsite.cpp $(Debug_Include_Path) > gccDebug/globalsite.d

# Compiles file graph.cpp for the Debug configuration...
-include gccDebug/graph.d
gccDebug/graph.o: graph.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c graph.cpp $(Debug_Include_Path) -o gccDebug/graph.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM graph.cpp $(Debug_Include_Path) > gccDebug/graph.d

# Compiles file graphaxle.cpp for the Debug configuration...
-include gccDebug/graphaxle.d
gccDebug/graphaxle.o: graphaxle.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c graphaxle.cpp $(Debug_Include_Path) -o gccDebug/graphaxle.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM graphaxle.cpp $(Debug_Include_Path) > gccDebug/graphaxle.d

# Compiles file graphbar.cpp for the Debug configuration...
-include gccDebug/graphbar.d
gccDebug/graphbar.o: graphbar.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c graphbar.cpp $(Debug_Include_Path) -o gccDebug/graphbar.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM graphbar.cpp $(Debug_Include_Path) > gccDebug/graphbar.d

# Compiles file graphlimitsdialog.cpp for the Debug configuration...
-include gccDebug/graphlimitsdialog.d
gccDebug/graphlimitsdialog.o: graphlimitsdialog.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c graphlimitsdialog.cpp $(Debug_Include_Path) -o gccDebug/graphlimitsdialog.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM graphlimitsdialog.cpp $(Debug_Include_Path) > gccDebug/graphlimitsdialog.d

# Compiles file graphline.cpp for the Debug configuration...
-include gccDebug/graphline.d
gccDebug/graphline.o: graphline.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c graphline.cpp $(Debug_Include_Path) -o gccDebug/graphline.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM graphline.cpp $(Debug_Include_Path) > gccDebug/graphline.d

# Compiles file graphmarker.cpp for the Debug configuration...
-include gccDebug/graphmarker.d
gccDebug/graphmarker.o: graphmarker.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c graphmarker.cpp $(Debug_Include_Path) -o gccDebug/graphmarker.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM graphmarker.cpp $(Debug_Include_Path) > gccDebug/graphmarker.d

# Compiles file guidedialog.cpp for the Debug configuration...
-include gccDebug/guidedialog.d
gccDebug/guidedialog.o: guidedialog.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c guidedialog.cpp $(Debug_Include_Path) -o gccDebug/guidedialog.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM guidedialog.cpp $(Debug_Include_Path) > gccDebug/guidedialog.d

# Compiles file helpbrowser.cpp for the Debug configuration...
-include gccDebug/helpbrowser.d
gccDebug/helpbrowser.o: helpbrowser.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c helpbrowser.cpp $(Debug_Include_Path) -o gccDebug/helpbrowser.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM helpbrowser.cpp $(Debug_Include_Path) > gccDebug/helpbrowser.d

# Compiles file horizontaldistancedialog.cpp for the Debug configuration...
-include gccDebug/horizontaldistancedialog.d
gccDebug/horizontaldistancedialog.o: horizontaldistancedialog.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c horizontaldistancedialog.cpp $(Debug_Include_Path) -o gccDebug/horizontaldistancedialog.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM horizontaldistancedialog.cpp $(Debug_Include_Path) > gccDebug/horizontaldistancedialog.d

# Compiles file humiditydialog.cpp for the Debug configuration...
-include gccDebug/humiditydialog.d
gccDebug/humiditydialog.o: humiditydialog.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c humiditydialog.cpp $(Debug_Include_Path) -o gccDebug/humiditydialog.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM humiditydialog.cpp $(Debug_Include_Path) > gccDebug/humiditydialog.d

# Compiles file module.cpp for the Debug configuration...
-include gccDebug/module.d
gccDebug/module.o: module.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c module.cpp $(Debug_Include_Path) -o gccDebug/module.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM module.cpp $(Debug_Include_Path) > gccDebug/module.d

# Compiles file modulesdialog.cpp for the Debug configuration...
-include gccDebug/modulesdialog.d
gccDebug/modulesdialog.o: modulesdialog.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c modulesdialog.cpp $(Debug_Include_Path) -o gccDebug/modulesdialog.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM modulesdialog.cpp $(Debug_Include_Path) > gccDebug/modulesdialog.d

# Compiles file moisscenario.cpp for the Debug configuration...
-include gccDebug/moisscenario.d
gccDebug/moisscenario.o: moisscenario.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c moisscenario.cpp $(Debug_Include_Path) -o gccDebug/moisscenario.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM moisscenario.cpp $(Debug_Include_Path) > gccDebug/moisscenario.d

# Compiles file newext.cpp for the Debug configuration...
-include gccDebug/newext.d
gccDebug/newext.o: newext.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c newext.cpp $(Debug_Include_Path) -o gccDebug/newext.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM newext.cpp $(Debug_Include_Path) > gccDebug/newext.d

# Compiles file pagemenudialog.cpp for the Debug configuration...
-include gccDebug/pagemenudialog.d
gccDebug/pagemenudialog.o: pagemenudialog.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c pagemenudialog.cpp $(Debug_Include_Path) -o gccDebug/pagemenudialog.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM pagemenudialog.cpp $(Debug_Include_Path) > gccDebug/pagemenudialog.d

# Compiles file parser.cpp for the Debug configuration...
-include gccDebug/parser.d
gccDebug/parser.o: parser.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c parser.cpp $(Debug_Include_Path) -o gccDebug/parser.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM parser.cpp $(Debug_Include_Path) > gccDebug/parser.d

# Compiles file platform-windows.cpp for the Debug configuration...
-include gccDebug/platform-windows.d
gccDebug/platform-windows.o: platform-windows.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c platform-windows.cpp $(Debug_Include_Path) -o gccDebug/platform-windows.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM platform-windows.cpp $(Debug_Include_Path) > gccDebug/platform-windows.d

# Compiles file printer.cpp for the Debug configuration...
-include gccDebug/printer.d
gccDebug/printer.o: printer.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c printer.cpp $(Debug_Include_Path) -o gccDebug/printer.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM printer.cpp $(Debug_Include_Path) > gccDebug/printer.d

# Compiles file property.cpp for the Debug configuration...
-include gccDebug/property.d
gccDebug/property.o: property.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c property.cpp $(Debug_Include_Path) -o gccDebug/property.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM property.cpp $(Debug_Include_Path) > gccDebug/property.d

# Compiles file propertydialog.cpp for the Debug configuration...
-include gccDebug/propertydialog.d
gccDebug/propertydialog.o: propertydialog.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c propertydialog.cpp $(Debug_Include_Path) -o gccDebug/propertydialog.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM propertydialog.cpp $(Debug_Include_Path) > gccDebug/propertydialog.d

# Compiles file randfuel.cpp for the Debug configuration...
-include gccDebug/randfuel.d
gccDebug/randfuel.o: randfuel.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c randfuel.cpp $(Debug_Include_Path) -o gccDebug/randfuel.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM randfuel.cpp $(Debug_Include_Path) > gccDebug/randfuel.d

# Compiles file randthread.cpp for the Debug configuration...
-include gccDebug/randthread.d
gccDebug/randthread.o: randthread.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c randthread.cpp $(Debug_Include_Path) -o gccDebug/randthread.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM randthread.cpp $(Debug_Include_Path) > gccDebug/randthread.d

# Compiles file realspinbox.cpp for the Debug configuration...
-include gccDebug/realspinbox.d
gccDebug/realspinbox.o: realspinbox.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c realspinbox.cpp $(Debug_Include_Path) -o gccDebug/realspinbox.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM realspinbox.cpp $(Debug_Include_Path) > gccDebug/realspinbox.d

# Compiles file requestdialog.cpp for the Debug configuration...
-include gccDebug/requestdialog.d
gccDebug/requestdialog.o: requestdialog.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c requestdialog.cpp $(Debug_Include_Path) -o gccDebug/requestdialog.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM requestdialog.cpp $(Debug_Include_Path) > gccDebug/requestdialog.d

# Compiles file rundialog.cpp for the Debug configuration...
-include gccDebug/rundialog.d
gccDebug/rundialog.o: rundialog.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c rundialog.cpp $(Debug_Include_Path) -o gccDebug/rundialog.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM rundialog.cpp $(Debug_Include_Path) > gccDebug/rundialog.d

# Compiles file rxvar.cpp for the Debug configuration...
-include gccDebug/rxvar.d
gccDebug/rxvar.o: rxvar.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c rxvar.cpp $(Debug_Include_Path) -o gccDebug/rxvar.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM rxvar.cpp $(Debug_Include_Path) > gccDebug/rxvar.d

# Compiles file SemFireCrownFirebrandProcessor.cpp for the Debug configuration...
-include gccDebug/SemFireCrownFirebrandProcessor.d
gccDebug/SemFireCrownFirebrandProcessor.o: SemFireCrownFirebrandProcessor.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c SemFireCrownFirebrandProcessor.cpp $(Debug_Include_Path) -o gccDebug/SemFireCrownFirebrandProcessor.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM SemFireCrownFirebrandProcessor.cpp $(Debug_Include_Path) > gccDebug/SemFireCrownFirebrandProcessor.d

# Compiles file siunits.cpp for the Debug configuration...
-include gccDebug/siunits.d
gccDebug/siunits.o: siunits.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c siunits.cpp $(Debug_Include_Path) -o gccDebug/siunits.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM siunits.cpp $(Debug_Include_Path) > gccDebug/siunits.d

# Compiles file slopetooldialog.cpp for the Debug configuration...
-include gccDebug/slopetooldialog.d
gccDebug/slopetooldialog.o: slopetooldialog.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c slopetooldialog.cpp $(Debug_Include_Path) -o gccDebug/slopetooldialog.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM slopetooldialog.cpp $(Debug_Include_Path) > gccDebug/slopetooldialog.d

# Compiles file standardwizards.cpp for the Debug configuration...
-include gccDebug/standardwizards.d
gccDebug/standardwizards.o: standardwizards.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c standardwizards.cpp $(Debug_Include_Path) -o gccDebug/standardwizards.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM standardwizards.cpp $(Debug_Include_Path) > gccDebug/standardwizards.d

# Compiles file stdafx.cpp for the Debug configuration...
-include gccDebug/stdafx.d
gccDebug/stdafx.o: stdafx.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c stdafx.cpp $(Debug_Include_Path) -o gccDebug/stdafx.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM stdafx.cpp $(Debug_Include_Path) > gccDebug/stdafx.d

# Compiles file sundialog.cpp for the Debug configuration...
-include gccDebug/sundialog.d
gccDebug/sundialog.o: sundialog.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c sundialog.cpp $(Debug_Include_Path) -o gccDebug/sundialog.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM sundialog.cpp $(Debug_Include_Path) > gccDebug/sundialog.d

# Compiles file sunview.cpp for the Debug configuration...
-include gccDebug/sunview.d
gccDebug/sunview.o: sunview.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c sunview.cpp $(Debug_Include_Path) -o gccDebug/sunview.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM sunview.cpp $(Debug_Include_Path) > gccDebug/sunview.d

# Compiles file textview.cpp for the Debug configuration...
-include gccDebug/textview.d
gccDebug/textview.o: textview.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c textview.cpp $(Debug_Include_Path) -o gccDebug/textview.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM textview.cpp $(Debug_Include_Path) > gccDebug/textview.d

# Compiles file textviewdocument.cpp for the Debug configuration...
-include gccDebug/textviewdocument.d
gccDebug/textviewdocument.o: textviewdocument.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c textviewdocument.cpp $(Debug_Include_Path) -o gccDebug/textviewdocument.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM textviewdocument.cpp $(Debug_Include_Path) > gccDebug/textviewdocument.d

# Compiles file toc.cpp for the Debug configuration...
-include gccDebug/toc.d
gccDebug/toc.o: toc.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c toc.cpp $(Debug_Include_Path) -o gccDebug/toc.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM toc.cpp $(Debug_Include_Path) > gccDebug/toc.d

# Compiles file unitsconverterdialog.cpp for the Debug configuration...
-include gccDebug/unitsconverterdialog.d
gccDebug/unitsconverterdialog.o: unitsconverterdialog.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c unitsconverterdialog.cpp $(Debug_Include_Path) -o gccDebug/unitsconverterdialog.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM unitsconverterdialog.cpp $(Debug_Include_Path) > gccDebug/unitsconverterdialog.d

# Compiles file unitseditdialog.cpp for the Debug configuration...
-include gccDebug/unitseditdialog.d
gccDebug/unitseditdialog.o: unitseditdialog.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c unitseditdialog.cpp $(Debug_Include_Path) -o gccDebug/unitseditdialog.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM unitseditdialog.cpp $(Debug_Include_Path) > gccDebug/unitseditdialog.d

# Compiles file varcheckbox.cpp for the Debug configuration...
-include gccDebug/varcheckbox.d
gccDebug/varcheckbox.o: varcheckbox.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c varcheckbox.cpp $(Debug_Include_Path) -o gccDebug/varcheckbox.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM varcheckbox.cpp $(Debug_Include_Path) > gccDebug/varcheckbox.d

# Compiles file wizarddialog.cpp for the Debug configuration...
-include gccDebug/wizarddialog.d
gccDebug/wizarddialog.o: wizarddialog.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c wizarddialog.cpp $(Debug_Include_Path) -o gccDebug/wizarddialog.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM wizarddialog.cpp $(Debug_Include_Path) > gccDebug/wizarddialog.d

# Compiles file xeqapp.cpp for the Debug configuration...
-include gccDebug/xeqapp.d
gccDebug/xeqapp.o: xeqapp.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c xeqapp.cpp $(Debug_Include_Path) -o gccDebug/xeqapp.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM xeqapp.cpp $(Debug_Include_Path) > gccDebug/xeqapp.d

# Compiles file xeqappparser.cpp for the Debug configuration...
-include gccDebug/xeqappparser.d
gccDebug/xeqappparser.o: xeqappparser.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c xeqappparser.cpp $(Debug_Include_Path) -o gccDebug/xeqappparser.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM xeqappparser.cpp $(Debug_Include_Path) > gccDebug/xeqappparser.d

# Compiles file xeqcalc.cpp for the Debug configuration...
-include gccDebug/xeqcalc.d
gccDebug/xeqcalc.o: xeqcalc.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c xeqcalc.cpp $(Debug_Include_Path) -o gccDebug/xeqcalc.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM xeqcalc.cpp $(Debug_Include_Path) > gccDebug/xeqcalc.d

# Compiles file xeqcalcmask.cpp for the Debug configuration...
-include gccDebug/xeqcalcmask.d
gccDebug/xeqcalcmask.o: xeqcalcmask.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c xeqcalcmask.cpp $(Debug_Include_Path) -o gccDebug/xeqcalcmask.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM xeqcalcmask.cpp $(Debug_Include_Path) > gccDebug/xeqcalcmask.d

# Compiles file xeqcalcreconfig.cpp for the Debug configuration...
-include gccDebug/xeqcalcreconfig.d
gccDebug/xeqcalcreconfig.o: xeqcalcreconfig.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c xeqcalcreconfig.cpp $(Debug_Include_Path) -o gccDebug/xeqcalcreconfig.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM xeqcalcreconfig.cpp $(Debug_Include_Path) > gccDebug/xeqcalcreconfig.d

# Compiles file xeqcalcV6Crown.cpp for the Debug configuration...
-include gccDebug/xeqcalcV6Crown.d
gccDebug/xeqcalcV6Crown.o: xeqcalcV6Crown.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c xeqcalcV6Crown.cpp $(Debug_Include_Path) -o gccDebug/xeqcalcV6Crown.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM xeqcalcV6Crown.cpp $(Debug_Include_Path) > gccDebug/xeqcalcV6Crown.d

# Compiles file xeqcalcV6Main.cpp for the Debug configuration...
-include gccDebug/xeqcalcV6Main.d
gccDebug/xeqcalcV6Main.o: xeqcalcV6Main.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c xeqcalcV6Main.cpp $(Debug_Include_Path) -o gccDebug/xeqcalcV6Main.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM xeqcalcV6Main.cpp $(Debug_Include_Path) > gccDebug/xeqcalcV6Main.d

# Compiles file xeqfile.cpp for the Debug configuration...
-include gccDebug/xeqfile.d
gccDebug/xeqfile.o: xeqfile.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c xeqfile.cpp $(Debug_Include_Path) -o gccDebug/xeqfile.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM xeqfile.cpp $(Debug_Include_Path) > gccDebug/xeqfile.d

# Compiles file xeqtree.cpp for the Debug configuration...
-include gccDebug/xeqtree.d
gccDebug/xeqtree.o: xeqtree.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c xeqtree.cpp $(Debug_Include_Path) -o gccDebug/xeqtree.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM xeqtree.cpp $(Debug_Include_Path) > gccDebug/xeqtree.d

# Compiles file xeqtreeparser.cpp for the Debug configuration...
-include gccDebug/xeqtreeparser.d
gccDebug/xeqtreeparser.o: xeqtreeparser.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c xeqtreeparser.cpp $(Debug_Include_Path) -o gccDebug/xeqtreeparser.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM xeqtreeparser.cpp $(Debug_Include_Path) > gccDebug/xeqtreeparser.d

# Compiles file xeqtreeprint.cpp for the Debug configuration...
-include gccDebug/xeqtreeprint.d
gccDebug/xeqtreeprint.o: xeqtreeprint.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c xeqtreeprint.cpp $(Debug_Include_Path) -o gccDebug/xeqtreeprint.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM xeqtreeprint.cpp $(Debug_Include_Path) > gccDebug/xeqtreeprint.d

# Compiles file xeqvar.cpp for the Debug configuration...
-include gccDebug/xeqvar.d
gccDebug/xeqvar.o: xeqvar.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c xeqvar.cpp $(Debug_Include_Path) -o gccDebug/xeqvar.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM xeqvar.cpp $(Debug_Include_Path) > gccDebug/xeqvar.d

# Compiles file xeqvaritem.cpp for the Debug configuration...
-include gccDebug/xeqvaritem.d
gccDebug/xeqvaritem.o: xeqvaritem.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c xeqvaritem.cpp $(Debug_Include_Path) -o gccDebug/xeqvaritem.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM xeqvaritem.cpp $(Debug_Include_Path) > gccDebug/xeqvaritem.d

# Compiles file xfblib.cpp for the Debug configuration...
-include gccDebug/xfblib.d
gccDebug/xfblib.o: xfblib.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c xfblib.cpp $(Debug_Include_Path) -o gccDebug/xfblib.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM xfblib.cpp $(Debug_Include_Path) > gccDebug/xfblib.d

# Compiles file xmlparser.cpp for the Debug configuration...
-include gccDebug/xmlparser.d
gccDebug/xmlparser.o: xmlparser.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c xmlparser.cpp $(Debug_Include_Path) -o gccDebug/xmlparser.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM xmlparser.cpp $(Debug_Include_Path) > gccDebug/xmlparser.d

# Compiles file tmp/moc/moc_aboutdialog.cpp for the Debug configuration...
-include gccDebug/tmp/moc/moc_aboutdialog.d
gccDebug/tmp/moc/moc_aboutdialog.o: tmp/moc/moc_aboutdialog.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c tmp/moc/moc_aboutdialog.cpp $(Debug_Include_Path) -o gccDebug/tmp/moc/moc_aboutdialog.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM tmp/moc/moc_aboutdialog.cpp $(Debug_Include_Path) > gccDebug/tmp/moc/moc_aboutdialog.d

# Compiles file tmp/moc/moc_appdialog.cpp for the Debug configuration...
-include gccDebug/tmp/moc/moc_appdialog.d
gccDebug/tmp/moc/moc_appdialog.o: tmp/moc/moc_appdialog.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c tmp/moc/moc_appdialog.cpp $(Debug_Include_Path) -o gccDebug/tmp/moc/moc_appdialog.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM tmp/moc/moc_appdialog.cpp $(Debug_Include_Path) > gccDebug/tmp/moc/moc_appdialog.d

# Compiles file tmp/moc/moc_appearancedialog.cpp for the Debug configuration...
-include gccDebug/tmp/moc/moc_appearancedialog.d
gccDebug/tmp/moc/moc_appearancedialog.o: tmp/moc/moc_appearancedialog.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c tmp/moc/moc_appearancedialog.cpp $(Debug_Include_Path) -o gccDebug/tmp/moc/moc_appearancedialog.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM tmp/moc/moc_appearancedialog.cpp $(Debug_Include_Path) > gccDebug/tmp/moc/moc_appearancedialog.d

# Compiles file tmp/moc/moc_appmessage.cpp for the Debug configuration...
-include gccDebug/tmp/moc/moc_appmessage.d
gccDebug/tmp/moc/moc_appmessage.o: tmp/moc/moc_appmessage.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c tmp/moc/moc_appmessage.cpp $(Debug_Include_Path) -o gccDebug/tmp/moc/moc_appmessage.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM tmp/moc/moc_appmessage.cpp $(Debug_Include_Path) > gccDebug/tmp/moc/moc_appmessage.d

# Compiles file tmp/moc/moc_appwindow.cpp for the Debug configuration...
-include gccDebug/tmp/moc/moc_appwindow.d
gccDebug/tmp/moc/moc_appwindow.o: tmp/moc/moc_appwindow.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c tmp/moc/moc_appwindow.cpp $(Debug_Include_Path) -o gccDebug/tmp/moc/moc_appwindow.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM tmp/moc/moc_appwindow.cpp $(Debug_Include_Path) > gccDebug/tmp/moc/moc_appwindow.d

# Compiles file tmp/moc/moc_attachdialog.cpp for the Debug configuration...
-include gccDebug/tmp/moc/moc_attachdialog.d
gccDebug/tmp/moc/moc_attachdialog.o: tmp/moc/moc_attachdialog.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c tmp/moc/moc_attachdialog.cpp $(Debug_Include_Path) -o gccDebug/tmp/moc/moc_attachdialog.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM tmp/moc/moc_attachdialog.cpp $(Debug_Include_Path) > gccDebug/tmp/moc/moc_attachdialog.d

# Compiles file tmp/moc/moc_bpdocument.cpp for the Debug configuration...
-include gccDebug/tmp/moc/moc_bpdocument.d
gccDebug/tmp/moc/moc_bpdocument.o: tmp/moc/moc_bpdocument.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c tmp/moc/moc_bpdocument.cpp $(Debug_Include_Path) -o gccDebug/tmp/moc/moc_bpdocument.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM tmp/moc/moc_bpdocument.cpp $(Debug_Include_Path) > gccDebug/tmp/moc/moc_bpdocument.d

# Compiles file tmp/moc/moc_calendardocument.cpp for the Debug configuration...
-include gccDebug/tmp/moc/moc_calendardocument.d
gccDebug/tmp/moc/moc_calendardocument.o: tmp/moc/moc_calendardocument.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c tmp/moc/moc_calendardocument.cpp $(Debug_Include_Path) -o gccDebug/tmp/moc/moc_calendardocument.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM tmp/moc/moc_calendardocument.cpp $(Debug_Include_Path) > gccDebug/tmp/moc/moc_calendardocument.d

# Compiles file tmp/moc/moc_conflictdialog.cpp for the Debug configuration...
-include gccDebug/tmp/moc/moc_conflictdialog.d
gccDebug/tmp/moc/moc_conflictdialog.o: tmp/moc/moc_conflictdialog.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c tmp/moc/moc_conflictdialog.cpp $(Debug_Include_Path) -o gccDebug/tmp/moc/moc_conflictdialog.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM tmp/moc/moc_conflictdialog.cpp $(Debug_Include_Path) > gccDebug/tmp/moc/moc_conflictdialog.d

# Compiles file tmp/moc/moc_document.cpp for the Debug configuration...
-include gccDebug/tmp/moc/moc_document.d
gccDebug/tmp/moc/moc_document.o: tmp/moc/moc_document.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c tmp/moc/moc_document.cpp $(Debug_Include_Path) -o gccDebug/tmp/moc/moc_document.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM tmp/moc/moc_document.cpp $(Debug_Include_Path) > gccDebug/tmp/moc/moc_document.d

# Compiles file tmp/moc/moc_fdfmcdialog.cpp for the Debug configuration...
-include gccDebug/tmp/moc/moc_fdfmcdialog.d
gccDebug/tmp/moc/moc_fdfmcdialog.o: tmp/moc/moc_fdfmcdialog.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c tmp/moc/moc_fdfmcdialog.cpp $(Debug_Include_Path) -o gccDebug/tmp/moc/moc_fdfmcdialog.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM tmp/moc/moc_fdfmcdialog.cpp $(Debug_Include_Path) > gccDebug/tmp/moc/moc_fdfmcdialog.d

# Compiles file tmp/moc/moc_fileselector.cpp for the Debug configuration...
-include gccDebug/tmp/moc/moc_fileselector.d
gccDebug/tmp/moc/moc_fileselector.o: tmp/moc/moc_fileselector.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c tmp/moc/moc_fileselector.cpp $(Debug_Include_Path) -o gccDebug/tmp/moc/moc_fileselector.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM tmp/moc/moc_fileselector.cpp $(Debug_Include_Path) > gccDebug/tmp/moc/moc_fileselector.d

# Compiles file tmp/moc/moc_fuelexportdialog.cpp for the Debug configuration...
-include gccDebug/tmp/moc/moc_fuelexportdialog.d
gccDebug/tmp/moc/moc_fuelexportdialog.o: tmp/moc/moc_fuelexportdialog.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c tmp/moc/moc_fuelexportdialog.cpp $(Debug_Include_Path) -o gccDebug/tmp/moc/moc_fuelexportdialog.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM tmp/moc/moc_fuelexportdialog.cpp $(Debug_Include_Path) > gccDebug/tmp/moc/moc_fuelexportdialog.d

# Compiles file tmp/moc/moc_fuelinitdialog.cpp for the Debug configuration...
-include gccDebug/tmp/moc/moc_fuelinitdialog.d
gccDebug/tmp/moc/moc_fuelinitdialog.o: tmp/moc/moc_fuelinitdialog.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c tmp/moc/moc_fuelinitdialog.cpp $(Debug_Include_Path) -o gccDebug/tmp/moc/moc_fuelinitdialog.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM tmp/moc/moc_fuelinitdialog.cpp $(Debug_Include_Path) > gccDebug/tmp/moc/moc_fuelinitdialog.d

# Compiles file tmp/moc/moc_fuelmodeldialog.cpp for the Debug configuration...
-include gccDebug/tmp/moc/moc_fuelmodeldialog.d
gccDebug/tmp/moc/moc_fuelmodeldialog.o: tmp/moc/moc_fuelmodeldialog.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c tmp/moc/moc_fuelmodeldialog.cpp $(Debug_Include_Path) -o gccDebug/tmp/moc/moc_fuelmodeldialog.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM tmp/moc/moc_fuelmodeldialog.cpp $(Debug_Include_Path) > gccDebug/tmp/moc/moc_fuelmodeldialog.d

# Compiles file tmp/moc/moc_fuelwizards.cpp for the Debug configuration...
-include gccDebug/tmp/moc/moc_fuelwizards.d
gccDebug/tmp/moc/moc_fuelwizards.o: tmp/moc/moc_fuelwizards.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c tmp/moc/moc_fuelwizards.cpp $(Debug_Include_Path) -o gccDebug/tmp/moc/moc_fuelwizards.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM tmp/moc/moc_fuelwizards.cpp $(Debug_Include_Path) > gccDebug/tmp/moc/moc_fuelwizards.d

# Compiles file tmp/moc/moc_graphlimitsdialog.cpp for the Debug configuration...
-include gccDebug/tmp/moc/moc_graphlimitsdialog.d
gccDebug/tmp/moc/moc_graphlimitsdialog.o: tmp/moc/moc_graphlimitsdialog.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c tmp/moc/moc_graphlimitsdialog.cpp $(Debug_Include_Path) -o gccDebug/tmp/moc/moc_graphlimitsdialog.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM tmp/moc/moc_graphlimitsdialog.cpp $(Debug_Include_Path) > gccDebug/tmp/moc/moc_graphlimitsdialog.d

# Compiles file tmp/moc/moc_guidedialog.cpp for the Debug configuration...
-include gccDebug/tmp/moc/moc_guidedialog.d
gccDebug/tmp/moc/moc_guidedialog.o: tmp/moc/moc_guidedialog.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c tmp/moc/moc_guidedialog.cpp $(Debug_Include_Path) -o gccDebug/tmp/moc/moc_guidedialog.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM tmp/moc/moc_guidedialog.cpp $(Debug_Include_Path) > gccDebug/tmp/moc/moc_guidedialog.d

# Compiles file tmp/moc/moc_helpbrowser.cpp for the Debug configuration...
-include gccDebug/tmp/moc/moc_helpbrowser.d
gccDebug/tmp/moc/moc_helpbrowser.o: tmp/moc/moc_helpbrowser.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c tmp/moc/moc_helpbrowser.cpp $(Debug_Include_Path) -o gccDebug/tmp/moc/moc_helpbrowser.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM tmp/moc/moc_helpbrowser.cpp $(Debug_Include_Path) > gccDebug/tmp/moc/moc_helpbrowser.d

# Compiles file tmp/moc/moc_horizontaldistancedialog.cpp for the Debug configuration...
-include gccDebug/tmp/moc/moc_horizontaldistancedialog.d
gccDebug/tmp/moc/moc_horizontaldistancedialog.o: tmp/moc/moc_horizontaldistancedialog.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c tmp/moc/moc_horizontaldistancedialog.cpp $(Debug_Include_Path) -o gccDebug/tmp/moc/moc_horizontaldistancedialog.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM tmp/moc/moc_horizontaldistancedialog.cpp $(Debug_Include_Path) > gccDebug/tmp/moc/moc_horizontaldistancedialog.d

# Compiles file tmp/moc/moc_humiditydialog.cpp for the Debug configuration...
-include gccDebug/tmp/moc/moc_humiditydialog.d
gccDebug/tmp/moc/moc_humiditydialog.o: tmp/moc/moc_humiditydialog.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c tmp/moc/moc_humiditydialog.cpp $(Debug_Include_Path) -o gccDebug/tmp/moc/moc_humiditydialog.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM tmp/moc/moc_humiditydialog.cpp $(Debug_Include_Path) > gccDebug/tmp/moc/moc_humiditydialog.d

# Compiles file tmp/moc/moc_modulesdialog.cpp for the Debug configuration...
-include gccDebug/tmp/moc/moc_modulesdialog.d
gccDebug/tmp/moc/moc_modulesdialog.o: tmp/moc/moc_modulesdialog.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c tmp/moc/moc_modulesdialog.cpp $(Debug_Include_Path) -o gccDebug/tmp/moc/moc_modulesdialog.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM tmp/moc/moc_modulesdialog.cpp $(Debug_Include_Path) > gccDebug/tmp/moc/moc_modulesdialog.d

# Compiles file tmp/moc/moc_pagemenudialog.cpp for the Debug configuration...
-include gccDebug/tmp/moc/moc_pagemenudialog.d
gccDebug/tmp/moc/moc_pagemenudialog.o: tmp/moc/moc_pagemenudialog.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c tmp/moc/moc_pagemenudialog.cpp $(Debug_Include_Path) -o gccDebug/tmp/moc/moc_pagemenudialog.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM tmp/moc/moc_pagemenudialog.cpp $(Debug_Include_Path) > gccDebug/tmp/moc/moc_pagemenudialog.d

# Compiles file tmp/moc/moc_propertydialog.cpp for the Debug configuration...
-include gccDebug/tmp/moc/moc_propertydialog.d
gccDebug/tmp/moc/moc_propertydialog.o: tmp/moc/moc_propertydialog.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c tmp/moc/moc_propertydialog.cpp $(Debug_Include_Path) -o gccDebug/tmp/moc/moc_propertydialog.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM tmp/moc/moc_propertydialog.cpp $(Debug_Include_Path) > gccDebug/tmp/moc/moc_propertydialog.d

# Compiles file tmp/moc/moc_realspinbox.cpp for the Debug configuration...
-include gccDebug/tmp/moc/moc_realspinbox.d
gccDebug/tmp/moc/moc_realspinbox.o: tmp/moc/moc_realspinbox.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c tmp/moc/moc_realspinbox.cpp $(Debug_Include_Path) -o gccDebug/tmp/moc/moc_realspinbox.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM tmp/moc/moc_realspinbox.cpp $(Debug_Include_Path) > gccDebug/tmp/moc/moc_realspinbox.d

# Compiles file tmp/moc/moc_rundialog.cpp for the Debug configuration...
-include gccDebug/tmp/moc/moc_rundialog.d
gccDebug/tmp/moc/moc_rundialog.o: tmp/moc/moc_rundialog.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c tmp/moc/moc_rundialog.cpp $(Debug_Include_Path) -o gccDebug/tmp/moc/moc_rundialog.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM tmp/moc/moc_rundialog.cpp $(Debug_Include_Path) > gccDebug/tmp/moc/moc_rundialog.d

# Compiles file tmp/moc/moc_slopetooldialog.cpp for the Debug configuration...
-include gccDebug/tmp/moc/moc_slopetooldialog.d
gccDebug/tmp/moc/moc_slopetooldialog.o: tmp/moc/moc_slopetooldialog.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c tmp/moc/moc_slopetooldialog.cpp $(Debug_Include_Path) -o gccDebug/tmp/moc/moc_slopetooldialog.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM tmp/moc/moc_slopetooldialog.cpp $(Debug_Include_Path) > gccDebug/tmp/moc/moc_slopetooldialog.d

# Compiles file tmp/moc/moc_standardwizards.cpp for the Debug configuration...
-include gccDebug/tmp/moc/moc_standardwizards.d
gccDebug/tmp/moc/moc_standardwizards.o: tmp/moc/moc_standardwizards.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c tmp/moc/moc_standardwizards.cpp $(Debug_Include_Path) -o gccDebug/tmp/moc/moc_standardwizards.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM tmp/moc/moc_standardwizards.cpp $(Debug_Include_Path) > gccDebug/tmp/moc/moc_standardwizards.d

# Compiles file tmp/moc/moc_sundialog.cpp for the Debug configuration...
-include gccDebug/tmp/moc/moc_sundialog.d
gccDebug/tmp/moc/moc_sundialog.o: tmp/moc/moc_sundialog.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c tmp/moc/moc_sundialog.cpp $(Debug_Include_Path) -o gccDebug/tmp/moc/moc_sundialog.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM tmp/moc/moc_sundialog.cpp $(Debug_Include_Path) > gccDebug/tmp/moc/moc_sundialog.d

# Compiles file tmp/moc/moc_textview.cpp for the Debug configuration...
-include gccDebug/tmp/moc/moc_textview.d
gccDebug/tmp/moc/moc_textview.o: tmp/moc/moc_textview.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c tmp/moc/moc_textview.cpp $(Debug_Include_Path) -o gccDebug/tmp/moc/moc_textview.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM tmp/moc/moc_textview.cpp $(Debug_Include_Path) > gccDebug/tmp/moc/moc_textview.d

# Compiles file tmp/moc/moc_textviewdocument.cpp for the Debug configuration...
-include gccDebug/tmp/moc/moc_textviewdocument.d
gccDebug/tmp/moc/moc_textviewdocument.o: tmp/moc/moc_textviewdocument.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c tmp/moc/moc_textviewdocument.cpp $(Debug_Include_Path) -o gccDebug/tmp/moc/moc_textviewdocument.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM tmp/moc/moc_textviewdocument.cpp $(Debug_Include_Path) > gccDebug/tmp/moc/moc_textviewdocument.d

# Compiles file tmp/moc/moc_unitsconverterdialog.cpp for the Debug configuration...
-include gccDebug/tmp/moc/moc_unitsconverterdialog.d
gccDebug/tmp/moc/moc_unitsconverterdialog.o: tmp/moc/moc_unitsconverterdialog.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c tmp/moc/moc_unitsconverterdialog.cpp $(Debug_Include_Path) -o gccDebug/tmp/moc/moc_unitsconverterdialog.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM tmp/moc/moc_unitsconverterdialog.cpp $(Debug_Include_Path) > gccDebug/tmp/moc/moc_unitsconverterdialog.d

# Compiles file tmp/moc/moc_unitseditdialog.cpp for the Debug configuration...
-include gccDebug/tmp/moc/moc_unitseditdialog.d
gccDebug/tmp/moc/moc_unitseditdialog.o: tmp/moc/moc_unitseditdialog.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c tmp/moc/moc_unitseditdialog.cpp $(Debug_Include_Path) -o gccDebug/tmp/moc/moc_unitseditdialog.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM tmp/moc/moc_unitseditdialog.cpp $(Debug_Include_Path) > gccDebug/tmp/moc/moc_unitseditdialog.d

# Compiles file tmp/moc/moc_varcheckbox.cpp for the Debug configuration...
-include gccDebug/tmp/moc/moc_varcheckbox.d
gccDebug/tmp/moc/moc_varcheckbox.o: tmp/moc/moc_varcheckbox.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c tmp/moc/moc_varcheckbox.cpp $(Debug_Include_Path) -o gccDebug/tmp/moc/moc_varcheckbox.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM tmp/moc/moc_varcheckbox.cpp $(Debug_Include_Path) > gccDebug/tmp/moc/moc_varcheckbox.d

# Compiles file tmp/moc/moc_wizarddialog.cpp for the Debug configuration...
-include gccDebug/tmp/moc/moc_wizarddialog.d
gccDebug/tmp/moc/moc_wizarddialog.o: tmp/moc/moc_wizarddialog.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c tmp/moc/moc_wizarddialog.cpp $(Debug_Include_Path) -o gccDebug/tmp/moc/moc_wizarddialog.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM tmp/moc/moc_wizarddialog.cpp $(Debug_Include_Path) > gccDebug/tmp/moc/moc_wizarddialog.d

# Compiles file Algorithms/Fofem6BarkThickness.cpp for the Debug configuration...
-include gccDebug/Algorithms/Fofem6BarkThickness.d
gccDebug/Algorithms/Fofem6BarkThickness.o: Algorithms/Fofem6BarkThickness.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c Algorithms/Fofem6BarkThickness.cpp $(Debug_Include_Path) -o gccDebug/Algorithms/Fofem6BarkThickness.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM Algorithms/Fofem6BarkThickness.cpp $(Debug_Include_Path) > gccDebug/Algorithms/Fofem6BarkThickness.d

# Compiles file Algorithms/Fofem6Mortality.cpp for the Debug configuration...
-include gccDebug/Algorithms/Fofem6Mortality.d
gccDebug/Algorithms/Fofem6Mortality.o: Algorithms/Fofem6Mortality.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c Algorithms/Fofem6Mortality.cpp $(Debug_Include_Path) -o gccDebug/Algorithms/Fofem6Mortality.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM Algorithms/Fofem6Mortality.cpp $(Debug_Include_Path) > gccDebug/Algorithms/Fofem6Mortality.d

# Compiles file Algorithms/Fofem6Species.cpp for the Debug configuration...
-include gccDebug/Algorithms/Fofem6Species.d
gccDebug/Algorithms/Fofem6Species.o: Algorithms/Fofem6Species.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c Algorithms/Fofem6Species.cpp $(Debug_Include_Path) -o gccDebug/Algorithms/Fofem6Species.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM Algorithms/Fofem6Species.cpp $(Debug_Include_Path) > gccDebug/Algorithms/Fofem6Species.d

# Compiles file Algorithms/Fofem6SpeciesData.cpp for the Debug configuration...
-include gccDebug/Algorithms/Fofem6SpeciesData.d
gccDebug/Algorithms/Fofem6SpeciesData.o: Algorithms/Fofem6SpeciesData.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c Algorithms/Fofem6SpeciesData.cpp $(Debug_Include_Path) -o gccDebug/Algorithms/Fofem6SpeciesData.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM Algorithms/Fofem6SpeciesData.cpp $(Debug_Include_Path) > gccDebug/Algorithms/Fofem6SpeciesData.d

# Builds the Release configuration...
.PHONY: Release
Release: create_folders gccRelease/aboutdialog.o gccRelease/appdialog.o gccRelease/appearancedialog.o gccRelease/appfilesystem.o gccRelease/appmessage.o gccRelease/appproperty.o gccRelease/appsiunits.o gccRelease/apptranslator.o gccRelease/appwindow.o gccRelease/attachdialog.o gccRelease/BehavePlus6.o gccRelease/Bp6ChaparralFuel.o gccRelease/Bp6ChaparralFuelChamise.o gccRelease/Bp6ChaparralFuelMixedBrush.o gccRelease/Bp6CrownFire.o gccRelease/Bp6FuelModel10.o gccRelease/Bp6SurfaceFire.o gccRelease/bpcomposecontaindiagram.o gccRelease/bpcomposedoc.o gccRelease/bpcomposefiredirdiagram.o gccRelease/bpcomposefireshapediagram.o gccRelease/bpcomposegraphs.o gccRelease/bpcomposehaulingchart.o gccRelease/bpcomposelogo.o gccRelease/bpcomposepage.o gccRelease/bpcomposetable1.o gccRelease/bpcomposetable2.o gccRelease/bpcomposetable3.o gccRelease/bpcomposeworksheet.o gccRelease/bpdocentry.o gccRelease/bpdocument.o gccRelease/bpfile.o gccRelease/calendardocument.o gccRelease/cdtlib.o gccRelease/composer.o gccRelease/conflictdialog.o gccRelease/contain.o gccRelease/Contain6.o gccRelease/ContainForce6.o gccRelease/ContainResource6.o gccRelease/ContainSim6.o gccRelease/datetime.o gccRelease/docdevicesize.o gccRelease/docpagesize.o gccRelease/docscrollview.o gccRelease/doctabs.o gccRelease/document.o gccRelease/fdfmcdialog.o gccRelease/fileselector.o gccRelease/filesystem.o gccRelease/fuelexportdialog.o gccRelease/fuelinitdialog.o gccRelease/fuelmodel.o gccRelease/fuelmodeldialog.o gccRelease/fuelwizards.o gccRelease/globalposition.o gccRelease/globalsite.o gccRelease/graph.o gccRelease/graphaxle.o gccRelease/graphbar.o gccRelease/graphlimitsdialog.o gccRelease/graphline.o gccRelease/graphmarker.o gccRelease/guidedialog.o gccRelease/helpbrowser.o gccRelease/horizontaldistancedialog.o gccRelease/humiditydialog.o gccRelease/module.o gccRelease/modulesdialog.o gccRelease/moisscenario.o gccRelease/newext.o gccRelease/pagemenudialog.o gccRelease/parser.o gccRelease/platform-windows.o gccRelease/printer.o gccRelease/property.o gccRelease/propertydialog.o gccRelease/randfuel.o gccRelease/randthread.o gccRelease/realspinbox.o gccRelease/requestdialog.o gccRelease/rundialog.o gccRelease/rxvar.o gccRelease/SemFireCrownFirebrandProcessor.o gccRelease/siunits.o gccRelease/slopetooldialog.o gccRelease/standardwizards.o gccRelease/stdafx.o gccRelease/sundialog.o gccRelease/sunview.o gccRelease/textview.o gccRelease/textviewdocument.o gccRelease/toc.o gccRelease/unitsconverterdialog.o gccRelease/unitseditdialog.o gccRelease/varcheckbox.o gccRelease/wizarddialog.o gccRelease/xeqapp.o gccRelease/xeqappparser.o gccRelease/xeqcalc.o gccRelease/xeqcalcmask.o gccRelease/xeqcalcreconfig.o gccRelease/xeqcalcV6Crown.o gccRelease/xeqcalcV6Main.o gccRelease/xeqfile.o gccRelease/xeqtree.o gccRelease/xeqtreeparser.o gccRelease/xeqtreeprint.o gccRelease/xeqvar.o gccRelease/xeqvaritem.o gccRelease/xfblib.o gccRelease/xmlparser.o gccRelease/tmp/moc/moc_aboutdialog.o gccRelease/tmp/moc/moc_appdialog.o gccRelease/tmp/moc/moc_appearancedialog.o gccRelease/tmp/moc/moc_appmessage.o gccRelease/tmp/moc/moc_appwindow.o gccRelease/tmp/moc/moc_attachdialog.o gccRelease/tmp/moc/moc_bpdocument.o gccRelease/tmp/moc/moc_calendardocument.o gccRelease/tmp/moc/moc_conflictdialog.o gccRelease/tmp/moc/moc_document.o gccRelease/tmp/moc/moc_fdfmcdialog.o gccRelease/tmp/moc/moc_fileselector.o gccRelease/tmp/moc/moc_fuelexportdialog.o gccRelease/tmp/moc/moc_fuelinitdialog.o gccRelease/tmp/moc/moc_fuelmodeldialog.o gccRelease/tmp/moc/moc_fuelwizards.o gccRelease/tmp/moc/moc_graphlimitsdialog.o gccRelease/tmp/moc/moc_guidedialog.o gccRelease/tmp/moc/moc_helpbrowser.o gccRelease/tmp/moc/moc_horizontaldistancedialog.o gccRelease/tmp/moc/moc_humiditydialog.o gccRelease/tmp/moc/moc_modulesdialog.o gccRelease/tmp/moc/moc_pagemenudialog.o gccRelease/tmp/moc/moc_propertydialog.o gccRelease/tmp/moc/moc_realspinbox.o gccRelease/tmp/moc/moc_rundialog.o gccRelease/tmp/moc/moc_slopetooldialog.o gccRelease/tmp/moc/moc_standardwizards.o gccRelease/tmp/moc/moc_sundialog.o gccRelease/tmp/moc/moc_textview.o gccRelease/tmp/moc/moc_textviewdocument.o gccRelease/tmp/moc/moc_unitsconverterdialog.o gccRelease/tmp/moc/moc_unitseditdialog.o gccRelease/tmp/moc/moc_varcheckbox.o gccRelease/tmp/moc/moc_wizarddialog.o gccRelease/Algorithms/Fofem6BarkThickness.o gccRelease/Algorithms/Fofem6Mortality.o gccRelease/Algorithms/Fofem6Species.o gccRelease/Algorithms/Fofem6SpeciesData.o 
	g++ gccRelease/aboutdialog.o gccRelease/appdialog.o gccRelease/appearancedialog.o gccRelease/appfilesystem.o gccRelease/appmessage.o gccRelease/appproperty.o gccRelease/appsiunits.o gccRelease/apptranslator.o gccRelease/appwindow.o gccRelease/attachdialog.o gccRelease/BehavePlus6.o gccRelease/Bp6ChaparralFuel.o gccRelease/Bp6ChaparralFuelChamise.o gccRelease/Bp6ChaparralFuelMixedBrush.o gccRelease/Bp6CrownFire.o gccRelease/Bp6FuelModel10.o gccRelease/Bp6SurfaceFire.o gccRelease/bpcomposecontaindiagram.o gccRelease/bpcomposedoc.o gccRelease/bpcomposefiredirdiagram.o gccRelease/bpcomposefireshapediagram.o gccRelease/bpcomposegraphs.o gccRelease/bpcomposehaulingchart.o gccRelease/bpcomposelogo.o gccRelease/bpcomposepage.o gccRelease/bpcomposetable1.o gccRelease/bpcomposetable2.o gccRelease/bpcomposetable3.o gccRelease/bpcomposeworksheet.o gccRelease/bpdocentry.o gccRelease/bpdocument.o gccRelease/bpfile.o gccRelease/calendardocument.o gccRelease/cdtlib.o gccRelease/composer.o gccRelease/conflictdialog.o gccRelease/contain.o gccRelease/Contain6.o gccRelease/ContainForce6.o gccRelease/ContainResource6.o gccRelease/ContainSim6.o gccRelease/datetime.o gccRelease/docdevicesize.o gccRelease/docpagesize.o gccRelease/docscrollview.o gccRelease/doctabs.o gccRelease/document.o gccRelease/fdfmcdialog.o gccRelease/fileselector.o gccRelease/filesystem.o gccRelease/fuelexportdialog.o gccRelease/fuelinitdialog.o gccRelease/fuelmodel.o gccRelease/fuelmodeldialog.o gccRelease/fuelwizards.o gccRelease/globalposition.o gccRelease/globalsite.o gccRelease/graph.o gccRelease/graphaxle.o gccRelease/graphbar.o gccRelease/graphlimitsdialog.o gccRelease/graphline.o gccRelease/graphmarker.o gccRelease/guidedialog.o gccRelease/helpbrowser.o gccRelease/horizontaldistancedialog.o gccRelease/humiditydialog.o gccRelease/module.o gccRelease/modulesdialog.o gccRelease/moisscenario.o gccRelease/newext.o gccRelease/pagemenudialog.o gccRelease/parser.o gccRelease/platform-windows.o gccRelease/printer.o gccRelease/property.o gccRelease/propertydialog.o gccRelease/randfuel.o gccRelease/randthread.o gccRelease/realspinbox.o gccRelease/requestdialog.o gccRelease/rundialog.o gccRelease/rxvar.o gccRelease/SemFireCrownFirebrandProcessor.o gccRelease/siunits.o gccRelease/slopetooldialog.o gccRelease/standardwizards.o gccRelease/stdafx.o gccRelease/sundialog.o gccRelease/sunview.o gccRelease/textview.o gccRelease/textviewdocument.o gccRelease/toc.o gccRelease/unitsconverterdialog.o gccRelease/unitseditdialog.o gccRelease/varcheckbox.o gccRelease/wizarddialog.o gccRelease/xeqapp.o gccRelease/xeqappparser.o gccRelease/xeqcalc.o gccRelease/xeqcalcmask.o gccRelease/xeqcalcreconfig.o gccRelease/xeqcalcV6Crown.o gccRelease/xeqcalcV6Main.o gccRelease/xeqfile.o gccRelease/xeqtree.o gccRelease/xeqtreeparser.o gccRelease/xeqtreeprint.o gccRelease/xeqvar.o gccRelease/xeqvaritem.o gccRelease/xfblib.o gccRelease/xmlparser.o gccRelease/tmp/moc/moc_aboutdialog.o gccRelease/tmp/moc/moc_appdialog.o gccRelease/tmp/moc/moc_appearancedialog.o gccRelease/tmp/moc/moc_appmessage.o gccRelease/tmp/moc/moc_appwindow.o gccRelease/tmp/moc/moc_attachdialog.o gccRelease/tmp/moc/moc_bpdocument.o gccRelease/tmp/moc/moc_calendardocument.o gccRelease/tmp/moc/moc_conflictdialog.o gccRelease/tmp/moc/moc_document.o gccRelease/tmp/moc/moc_fdfmcdialog.o gccRelease/tmp/moc/moc_fileselector.o gccRelease/tmp/moc/moc_fuelexportdialog.o gccRelease/tmp/moc/moc_fuelinitdialog.o gccRelease/tmp/moc/moc_fuelmodeldialog.o gccRelease/tmp/moc/moc_fuelwizards.o gccRelease/tmp/moc/moc_graphlimitsdialog.o gccRelease/tmp/moc/moc_guidedialog.o gccRelease/tmp/moc/moc_helpbrowser.o gccRelease/tmp/moc/moc_horizontaldistancedialog.o gccRelease/tmp/moc/moc_humiditydialog.o gccRelease/tmp/moc/moc_modulesdialog.o gccRelease/tmp/moc/moc_pagemenudialog.o gccRelease/tmp/moc/moc_propertydialog.o gccRelease/tmp/moc/moc_realspinbox.o gccRelease/tmp/moc/moc_rundialog.o gccRelease/tmp/moc/moc_slopetooldialog.o gccRelease/tmp/moc/moc_standardwizards.o gccRelease/tmp/moc/moc_sundialog.o gccRelease/tmp/moc/moc_textview.o gccRelease/tmp/moc/moc_textviewdocument.o gccRelease/tmp/moc/moc_unitsconverterdialog.o gccRelease/tmp/moc/moc_unitseditdialog.o gccRelease/tmp/moc/moc_varcheckbox.o gccRelease/tmp/moc/moc_wizarddialog.o gccRelease/Algorithms/Fofem6BarkThickness.o gccRelease/Algorithms/Fofem6Mortality.o gccRelease/Algorithms/Fofem6Species.o gccRelease/Algorithms/Fofem6SpeciesData.o  $(Release_Library_Path) $(Release_Libraries) -Wl,-rpath,./ -o gccRelease/BehavePlus6_Project.exe

# Compiles file aboutdialog.cpp for the Release configuration...
-include gccRelease/aboutdialog.d
gccRelease/aboutdialog.o: aboutdialog.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c aboutdialog.cpp $(Release_Include_Path) -o gccRelease/aboutdialog.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM aboutdialog.cpp $(Release_Include_Path) > gccRelease/aboutdialog.d

# Compiles file appdialog.cpp for the Release configuration...
-include gccRelease/appdialog.d
gccRelease/appdialog.o: appdialog.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c appdialog.cpp $(Release_Include_Path) -o gccRelease/appdialog.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM appdialog.cpp $(Release_Include_Path) > gccRelease/appdialog.d

# Compiles file appearancedialog.cpp for the Release configuration...
-include gccRelease/appearancedialog.d
gccRelease/appearancedialog.o: appearancedialog.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c appearancedialog.cpp $(Release_Include_Path) -o gccRelease/appearancedialog.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM appearancedialog.cpp $(Release_Include_Path) > gccRelease/appearancedialog.d

# Compiles file appfilesystem.cpp for the Release configuration...
-include gccRelease/appfilesystem.d
gccRelease/appfilesystem.o: appfilesystem.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c appfilesystem.cpp $(Release_Include_Path) -o gccRelease/appfilesystem.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM appfilesystem.cpp $(Release_Include_Path) > gccRelease/appfilesystem.d

# Compiles file appmessage.cpp for the Release configuration...
-include gccRelease/appmessage.d
gccRelease/appmessage.o: appmessage.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c appmessage.cpp $(Release_Include_Path) -o gccRelease/appmessage.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM appmessage.cpp $(Release_Include_Path) > gccRelease/appmessage.d

# Compiles file appproperty.cpp for the Release configuration...
-include gccRelease/appproperty.d
gccRelease/appproperty.o: appproperty.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c appproperty.cpp $(Release_Include_Path) -o gccRelease/appproperty.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM appproperty.cpp $(Release_Include_Path) > gccRelease/appproperty.d

# Compiles file appsiunits.cpp for the Release configuration...
-include gccRelease/appsiunits.d
gccRelease/appsiunits.o: appsiunits.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c appsiunits.cpp $(Release_Include_Path) -o gccRelease/appsiunits.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM appsiunits.cpp $(Release_Include_Path) > gccRelease/appsiunits.d

# Compiles file apptranslator.cpp for the Release configuration...
-include gccRelease/apptranslator.d
gccRelease/apptranslator.o: apptranslator.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c apptranslator.cpp $(Release_Include_Path) -o gccRelease/apptranslator.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM apptranslator.cpp $(Release_Include_Path) > gccRelease/apptranslator.d

# Compiles file appwindow.cpp for the Release configuration...
-include gccRelease/appwindow.d
gccRelease/appwindow.o: appwindow.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c appwindow.cpp $(Release_Include_Path) -o gccRelease/appwindow.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM appwindow.cpp $(Release_Include_Path) > gccRelease/appwindow.d

# Compiles file attachdialog.cpp for the Release configuration...
-include gccRelease/attachdialog.d
gccRelease/attachdialog.o: attachdialog.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c attachdialog.cpp $(Release_Include_Path) -o gccRelease/attachdialog.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM attachdialog.cpp $(Release_Include_Path) > gccRelease/attachdialog.d

# Compiles file BehavePlus6.cpp for the Release configuration...
-include gccRelease/BehavePlus6.d
gccRelease/BehavePlus6.o: BehavePlus6.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c BehavePlus6.cpp $(Release_Include_Path) -o gccRelease/BehavePlus6.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM BehavePlus6.cpp $(Release_Include_Path) > gccRelease/BehavePlus6.d

# Compiles file Bp6ChaparralFuel.cpp for the Release configuration...
-include gccRelease/Bp6ChaparralFuel.d
gccRelease/Bp6ChaparralFuel.o: Bp6ChaparralFuel.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c Bp6ChaparralFuel.cpp $(Release_Include_Path) -o gccRelease/Bp6ChaparralFuel.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM Bp6ChaparralFuel.cpp $(Release_Include_Path) > gccRelease/Bp6ChaparralFuel.d

# Compiles file Bp6ChaparralFuelChamise.cpp for the Release configuration...
-include gccRelease/Bp6ChaparralFuelChamise.d
gccRelease/Bp6ChaparralFuelChamise.o: Bp6ChaparralFuelChamise.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c Bp6ChaparralFuelChamise.cpp $(Release_Include_Path) -o gccRelease/Bp6ChaparralFuelChamise.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM Bp6ChaparralFuelChamise.cpp $(Release_Include_Path) > gccRelease/Bp6ChaparralFuelChamise.d

# Compiles file Bp6ChaparralFuelMixedBrush.cpp for the Release configuration...
-include gccRelease/Bp6ChaparralFuelMixedBrush.d
gccRelease/Bp6ChaparralFuelMixedBrush.o: Bp6ChaparralFuelMixedBrush.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c Bp6ChaparralFuelMixedBrush.cpp $(Release_Include_Path) -o gccRelease/Bp6ChaparralFuelMixedBrush.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM Bp6ChaparralFuelMixedBrush.cpp $(Release_Include_Path) > gccRelease/Bp6ChaparralFuelMixedBrush.d

# Compiles file Bp6CrownFire.cpp for the Release configuration...
-include gccRelease/Bp6CrownFire.d
gccRelease/Bp6CrownFire.o: Bp6CrownFire.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c Bp6CrownFire.cpp $(Release_Include_Path) -o gccRelease/Bp6CrownFire.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM Bp6CrownFire.cpp $(Release_Include_Path) > gccRelease/Bp6CrownFire.d

# Compiles file Bp6FuelModel10.cpp for the Release configuration...
-include gccRelease/Bp6FuelModel10.d
gccRelease/Bp6FuelModel10.o: Bp6FuelModel10.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c Bp6FuelModel10.cpp $(Release_Include_Path) -o gccRelease/Bp6FuelModel10.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM Bp6FuelModel10.cpp $(Release_Include_Path) > gccRelease/Bp6FuelModel10.d

# Compiles file Bp6SurfaceFire.cpp for the Release configuration...
-include gccRelease/Bp6SurfaceFire.d
gccRelease/Bp6SurfaceFire.o: Bp6SurfaceFire.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c Bp6SurfaceFire.cpp $(Release_Include_Path) -o gccRelease/Bp6SurfaceFire.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM Bp6SurfaceFire.cpp $(Release_Include_Path) > gccRelease/Bp6SurfaceFire.d

# Compiles file bpcomposecontaindiagram.cpp for the Release configuration...
-include gccRelease/bpcomposecontaindiagram.d
gccRelease/bpcomposecontaindiagram.o: bpcomposecontaindiagram.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c bpcomposecontaindiagram.cpp $(Release_Include_Path) -o gccRelease/bpcomposecontaindiagram.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM bpcomposecontaindiagram.cpp $(Release_Include_Path) > gccRelease/bpcomposecontaindiagram.d

# Compiles file bpcomposedoc.cpp for the Release configuration...
-include gccRelease/bpcomposedoc.d
gccRelease/bpcomposedoc.o: bpcomposedoc.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c bpcomposedoc.cpp $(Release_Include_Path) -o gccRelease/bpcomposedoc.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM bpcomposedoc.cpp $(Release_Include_Path) > gccRelease/bpcomposedoc.d

# Compiles file bpcomposefiredirdiagram.cpp for the Release configuration...
-include gccRelease/bpcomposefiredirdiagram.d
gccRelease/bpcomposefiredirdiagram.o: bpcomposefiredirdiagram.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c bpcomposefiredirdiagram.cpp $(Release_Include_Path) -o gccRelease/bpcomposefiredirdiagram.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM bpcomposefiredirdiagram.cpp $(Release_Include_Path) > gccRelease/bpcomposefiredirdiagram.d

# Compiles file bpcomposefireshapediagram.cpp for the Release configuration...
-include gccRelease/bpcomposefireshapediagram.d
gccRelease/bpcomposefireshapediagram.o: bpcomposefireshapediagram.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c bpcomposefireshapediagram.cpp $(Release_Include_Path) -o gccRelease/bpcomposefireshapediagram.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM bpcomposefireshapediagram.cpp $(Release_Include_Path) > gccRelease/bpcomposefireshapediagram.d

# Compiles file bpcomposegraphs.cpp for the Release configuration...
-include gccRelease/bpcomposegraphs.d
gccRelease/bpcomposegraphs.o: bpcomposegraphs.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c bpcomposegraphs.cpp $(Release_Include_Path) -o gccRelease/bpcomposegraphs.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM bpcomposegraphs.cpp $(Release_Include_Path) > gccRelease/bpcomposegraphs.d

# Compiles file bpcomposehaulingchart.cpp for the Release configuration...
-include gccRelease/bpcomposehaulingchart.d
gccRelease/bpcomposehaulingchart.o: bpcomposehaulingchart.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c bpcomposehaulingchart.cpp $(Release_Include_Path) -o gccRelease/bpcomposehaulingchart.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM bpcomposehaulingchart.cpp $(Release_Include_Path) > gccRelease/bpcomposehaulingchart.d

# Compiles file bpcomposelogo.cpp for the Release configuration...
-include gccRelease/bpcomposelogo.d
gccRelease/bpcomposelogo.o: bpcomposelogo.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c bpcomposelogo.cpp $(Release_Include_Path) -o gccRelease/bpcomposelogo.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM bpcomposelogo.cpp $(Release_Include_Path) > gccRelease/bpcomposelogo.d

# Compiles file bpcomposepage.cpp for the Release configuration...
-include gccRelease/bpcomposepage.d
gccRelease/bpcomposepage.o: bpcomposepage.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c bpcomposepage.cpp $(Release_Include_Path) -o gccRelease/bpcomposepage.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM bpcomposepage.cpp $(Release_Include_Path) > gccRelease/bpcomposepage.d

# Compiles file bpcomposetable1.cpp for the Release configuration...
-include gccRelease/bpcomposetable1.d
gccRelease/bpcomposetable1.o: bpcomposetable1.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c bpcomposetable1.cpp $(Release_Include_Path) -o gccRelease/bpcomposetable1.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM bpcomposetable1.cpp $(Release_Include_Path) > gccRelease/bpcomposetable1.d

# Compiles file bpcomposetable2.cpp for the Release configuration...
-include gccRelease/bpcomposetable2.d
gccRelease/bpcomposetable2.o: bpcomposetable2.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c bpcomposetable2.cpp $(Release_Include_Path) -o gccRelease/bpcomposetable2.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM bpcomposetable2.cpp $(Release_Include_Path) > gccRelease/bpcomposetable2.d

# Compiles file bpcomposetable3.cpp for the Release configuration...
-include gccRelease/bpcomposetable3.d
gccRelease/bpcomposetable3.o: bpcomposetable3.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c bpcomposetable3.cpp $(Release_Include_Path) -o gccRelease/bpcomposetable3.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM bpcomposetable3.cpp $(Release_Include_Path) > gccRelease/bpcomposetable3.d

# Compiles file bpcomposeworksheet.cpp for the Release configuration...
-include gccRelease/bpcomposeworksheet.d
gccRelease/bpcomposeworksheet.o: bpcomposeworksheet.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c bpcomposeworksheet.cpp $(Release_Include_Path) -o gccRelease/bpcomposeworksheet.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM bpcomposeworksheet.cpp $(Release_Include_Path) > gccRelease/bpcomposeworksheet.d

# Compiles file bpdocentry.cpp for the Release configuration...
-include gccRelease/bpdocentry.d
gccRelease/bpdocentry.o: bpdocentry.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c bpdocentry.cpp $(Release_Include_Path) -o gccRelease/bpdocentry.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM bpdocentry.cpp $(Release_Include_Path) > gccRelease/bpdocentry.d

# Compiles file bpdocument.cpp for the Release configuration...
-include gccRelease/bpdocument.d
gccRelease/bpdocument.o: bpdocument.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c bpdocument.cpp $(Release_Include_Path) -o gccRelease/bpdocument.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM bpdocument.cpp $(Release_Include_Path) > gccRelease/bpdocument.d

# Compiles file bpfile.cpp for the Release configuration...
-include gccRelease/bpfile.d
gccRelease/bpfile.o: bpfile.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c bpfile.cpp $(Release_Include_Path) -o gccRelease/bpfile.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM bpfile.cpp $(Release_Include_Path) > gccRelease/bpfile.d

# Compiles file calendardocument.cpp for the Release configuration...
-include gccRelease/calendardocument.d
gccRelease/calendardocument.o: calendardocument.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c calendardocument.cpp $(Release_Include_Path) -o gccRelease/calendardocument.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM calendardocument.cpp $(Release_Include_Path) > gccRelease/calendardocument.d

# Compiles file cdtlib.c for the Release configuration...
-include gccRelease/cdtlib.d
gccRelease/cdtlib.o: cdtlib.c
	$(C_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c cdtlib.c $(Release_Include_Path) -o gccRelease/cdtlib.o
	$(C_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM cdtlib.c $(Release_Include_Path) > gccRelease/cdtlib.d

# Compiles file composer.cpp for the Release configuration...
-include gccRelease/composer.d
gccRelease/composer.o: composer.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c composer.cpp $(Release_Include_Path) -o gccRelease/composer.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM composer.cpp $(Release_Include_Path) > gccRelease/composer.d

# Compiles file conflictdialog.cpp for the Release configuration...
-include gccRelease/conflictdialog.d
gccRelease/conflictdialog.o: conflictdialog.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c conflictdialog.cpp $(Release_Include_Path) -o gccRelease/conflictdialog.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM conflictdialog.cpp $(Release_Include_Path) > gccRelease/conflictdialog.d

# Compiles file contain.cpp for the Release configuration...
-include gccRelease/contain.d
gccRelease/contain.o: contain.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c contain.cpp $(Release_Include_Path) -o gccRelease/contain.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM contain.cpp $(Release_Include_Path) > gccRelease/contain.d

# Compiles file Contain6.cpp for the Release configuration...
-include gccRelease/Contain6.d
gccRelease/Contain6.o: Contain6.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c Contain6.cpp $(Release_Include_Path) -o gccRelease/Contain6.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM Contain6.cpp $(Release_Include_Path) > gccRelease/Contain6.d

# Compiles file ContainForce6.cpp for the Release configuration...
-include gccRelease/ContainForce6.d
gccRelease/ContainForce6.o: ContainForce6.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c ContainForce6.cpp $(Release_Include_Path) -o gccRelease/ContainForce6.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM ContainForce6.cpp $(Release_Include_Path) > gccRelease/ContainForce6.d

# Compiles file ContainResource6.cpp for the Release configuration...
-include gccRelease/ContainResource6.d
gccRelease/ContainResource6.o: ContainResource6.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c ContainResource6.cpp $(Release_Include_Path) -o gccRelease/ContainResource6.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM ContainResource6.cpp $(Release_Include_Path) > gccRelease/ContainResource6.d

# Compiles file ContainSim6.cpp for the Release configuration...
-include gccRelease/ContainSim6.d
gccRelease/ContainSim6.o: ContainSim6.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c ContainSim6.cpp $(Release_Include_Path) -o gccRelease/ContainSim6.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM ContainSim6.cpp $(Release_Include_Path) > gccRelease/ContainSim6.d

# Compiles file datetime.cpp for the Release configuration...
-include gccRelease/datetime.d
gccRelease/datetime.o: datetime.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c datetime.cpp $(Release_Include_Path) -o gccRelease/datetime.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM datetime.cpp $(Release_Include_Path) > gccRelease/datetime.d

# Compiles file docdevicesize.cpp for the Release configuration...
-include gccRelease/docdevicesize.d
gccRelease/docdevicesize.o: docdevicesize.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c docdevicesize.cpp $(Release_Include_Path) -o gccRelease/docdevicesize.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM docdevicesize.cpp $(Release_Include_Path) > gccRelease/docdevicesize.d

# Compiles file docpagesize.cpp for the Release configuration...
-include gccRelease/docpagesize.d
gccRelease/docpagesize.o: docpagesize.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c docpagesize.cpp $(Release_Include_Path) -o gccRelease/docpagesize.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM docpagesize.cpp $(Release_Include_Path) > gccRelease/docpagesize.d

# Compiles file docscrollview.cpp for the Release configuration...
-include gccRelease/docscrollview.d
gccRelease/docscrollview.o: docscrollview.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c docscrollview.cpp $(Release_Include_Path) -o gccRelease/docscrollview.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM docscrollview.cpp $(Release_Include_Path) > gccRelease/docscrollview.d

# Compiles file doctabs.cpp for the Release configuration...
-include gccRelease/doctabs.d
gccRelease/doctabs.o: doctabs.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c doctabs.cpp $(Release_Include_Path) -o gccRelease/doctabs.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM doctabs.cpp $(Release_Include_Path) > gccRelease/doctabs.d

# Compiles file document.cpp for the Release configuration...
-include gccRelease/document.d
gccRelease/document.o: document.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c document.cpp $(Release_Include_Path) -o gccRelease/document.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM document.cpp $(Release_Include_Path) > gccRelease/document.d

# Compiles file fdfmcdialog.cpp for the Release configuration...
-include gccRelease/fdfmcdialog.d
gccRelease/fdfmcdialog.o: fdfmcdialog.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c fdfmcdialog.cpp $(Release_Include_Path) -o gccRelease/fdfmcdialog.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM fdfmcdialog.cpp $(Release_Include_Path) > gccRelease/fdfmcdialog.d

# Compiles file fileselector.cpp for the Release configuration...
-include gccRelease/fileselector.d
gccRelease/fileselector.o: fileselector.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c fileselector.cpp $(Release_Include_Path) -o gccRelease/fileselector.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM fileselector.cpp $(Release_Include_Path) > gccRelease/fileselector.d

# Compiles file filesystem.cpp for the Release configuration...
-include gccRelease/filesystem.d
gccRelease/filesystem.o: filesystem.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c filesystem.cpp $(Release_Include_Path) -o gccRelease/filesystem.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM filesystem.cpp $(Release_Include_Path) > gccRelease/filesystem.d

# Compiles file fuelexportdialog.cpp for the Release configuration...
-include gccRelease/fuelexportdialog.d
gccRelease/fuelexportdialog.o: fuelexportdialog.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c fuelexportdialog.cpp $(Release_Include_Path) -o gccRelease/fuelexportdialog.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM fuelexportdialog.cpp $(Release_Include_Path) > gccRelease/fuelexportdialog.d

# Compiles file fuelinitdialog.cpp for the Release configuration...
-include gccRelease/fuelinitdialog.d
gccRelease/fuelinitdialog.o: fuelinitdialog.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c fuelinitdialog.cpp $(Release_Include_Path) -o gccRelease/fuelinitdialog.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM fuelinitdialog.cpp $(Release_Include_Path) > gccRelease/fuelinitdialog.d

# Compiles file fuelmodel.cpp for the Release configuration...
-include gccRelease/fuelmodel.d
gccRelease/fuelmodel.o: fuelmodel.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c fuelmodel.cpp $(Release_Include_Path) -o gccRelease/fuelmodel.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM fuelmodel.cpp $(Release_Include_Path) > gccRelease/fuelmodel.d

# Compiles file fuelmodeldialog.cpp for the Release configuration...
-include gccRelease/fuelmodeldialog.d
gccRelease/fuelmodeldialog.o: fuelmodeldialog.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c fuelmodeldialog.cpp $(Release_Include_Path) -o gccRelease/fuelmodeldialog.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM fuelmodeldialog.cpp $(Release_Include_Path) > gccRelease/fuelmodeldialog.d

# Compiles file fuelwizards.cpp for the Release configuration...
-include gccRelease/fuelwizards.d
gccRelease/fuelwizards.o: fuelwizards.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c fuelwizards.cpp $(Release_Include_Path) -o gccRelease/fuelwizards.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM fuelwizards.cpp $(Release_Include_Path) > gccRelease/fuelwizards.d

# Compiles file globalposition.cpp for the Release configuration...
-include gccRelease/globalposition.d
gccRelease/globalposition.o: globalposition.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c globalposition.cpp $(Release_Include_Path) -o gccRelease/globalposition.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM globalposition.cpp $(Release_Include_Path) > gccRelease/globalposition.d

# Compiles file globalsite.cpp for the Release configuration...
-include gccRelease/globalsite.d
gccRelease/globalsite.o: globalsite.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c globalsite.cpp $(Release_Include_Path) -o gccRelease/globalsite.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM globalsite.cpp $(Release_Include_Path) > gccRelease/globalsite.d

# Compiles file graph.cpp for the Release configuration...
-include gccRelease/graph.d
gccRelease/graph.o: graph.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c graph.cpp $(Release_Include_Path) -o gccRelease/graph.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM graph.cpp $(Release_Include_Path) > gccRelease/graph.d

# Compiles file graphaxle.cpp for the Release configuration...
-include gccRelease/graphaxle.d
gccRelease/graphaxle.o: graphaxle.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c graphaxle.cpp $(Release_Include_Path) -o gccRelease/graphaxle.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM graphaxle.cpp $(Release_Include_Path) > gccRelease/graphaxle.d

# Compiles file graphbar.cpp for the Release configuration...
-include gccRelease/graphbar.d
gccRelease/graphbar.o: graphbar.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c graphbar.cpp $(Release_Include_Path) -o gccRelease/graphbar.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM graphbar.cpp $(Release_Include_Path) > gccRelease/graphbar.d

# Compiles file graphlimitsdialog.cpp for the Release configuration...
-include gccRelease/graphlimitsdialog.d
gccRelease/graphlimitsdialog.o: graphlimitsdialog.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c graphlimitsdialog.cpp $(Release_Include_Path) -o gccRelease/graphlimitsdialog.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM graphlimitsdialog.cpp $(Release_Include_Path) > gccRelease/graphlimitsdialog.d

# Compiles file graphline.cpp for the Release configuration...
-include gccRelease/graphline.d
gccRelease/graphline.o: graphline.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c graphline.cpp $(Release_Include_Path) -o gccRelease/graphline.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM graphline.cpp $(Release_Include_Path) > gccRelease/graphline.d

# Compiles file graphmarker.cpp for the Release configuration...
-include gccRelease/graphmarker.d
gccRelease/graphmarker.o: graphmarker.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c graphmarker.cpp $(Release_Include_Path) -o gccRelease/graphmarker.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM graphmarker.cpp $(Release_Include_Path) > gccRelease/graphmarker.d

# Compiles file guidedialog.cpp for the Release configuration...
-include gccRelease/guidedialog.d
gccRelease/guidedialog.o: guidedialog.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c guidedialog.cpp $(Release_Include_Path) -o gccRelease/guidedialog.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM guidedialog.cpp $(Release_Include_Path) > gccRelease/guidedialog.d

# Compiles file helpbrowser.cpp for the Release configuration...
-include gccRelease/helpbrowser.d
gccRelease/helpbrowser.o: helpbrowser.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c helpbrowser.cpp $(Release_Include_Path) -o gccRelease/helpbrowser.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM helpbrowser.cpp $(Release_Include_Path) > gccRelease/helpbrowser.d

# Compiles file horizontaldistancedialog.cpp for the Release configuration...
-include gccRelease/horizontaldistancedialog.d
gccRelease/horizontaldistancedialog.o: horizontaldistancedialog.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c horizontaldistancedialog.cpp $(Release_Include_Path) -o gccRelease/horizontaldistancedialog.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM horizontaldistancedialog.cpp $(Release_Include_Path) > gccRelease/horizontaldistancedialog.d

# Compiles file humiditydialog.cpp for the Release configuration...
-include gccRelease/humiditydialog.d
gccRelease/humiditydialog.o: humiditydialog.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c humiditydialog.cpp $(Release_Include_Path) -o gccRelease/humiditydialog.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM humiditydialog.cpp $(Release_Include_Path) > gccRelease/humiditydialog.d

# Compiles file module.cpp for the Release configuration...
-include gccRelease/module.d
gccRelease/module.o: module.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c module.cpp $(Release_Include_Path) -o gccRelease/module.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM module.cpp $(Release_Include_Path) > gccRelease/module.d

# Compiles file modulesdialog.cpp for the Release configuration...
-include gccRelease/modulesdialog.d
gccRelease/modulesdialog.o: modulesdialog.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c modulesdialog.cpp $(Release_Include_Path) -o gccRelease/modulesdialog.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM modulesdialog.cpp $(Release_Include_Path) > gccRelease/modulesdialog.d

# Compiles file moisscenario.cpp for the Release configuration...
-include gccRelease/moisscenario.d
gccRelease/moisscenario.o: moisscenario.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c moisscenario.cpp $(Release_Include_Path) -o gccRelease/moisscenario.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM moisscenario.cpp $(Release_Include_Path) > gccRelease/moisscenario.d

# Compiles file newext.cpp for the Release configuration...
-include gccRelease/newext.d
gccRelease/newext.o: newext.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c newext.cpp $(Release_Include_Path) -o gccRelease/newext.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM newext.cpp $(Release_Include_Path) > gccRelease/newext.d

# Compiles file pagemenudialog.cpp for the Release configuration...
-include gccRelease/pagemenudialog.d
gccRelease/pagemenudialog.o: pagemenudialog.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c pagemenudialog.cpp $(Release_Include_Path) -o gccRelease/pagemenudialog.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM pagemenudialog.cpp $(Release_Include_Path) > gccRelease/pagemenudialog.d

# Compiles file parser.cpp for the Release configuration...
-include gccRelease/parser.d
gccRelease/parser.o: parser.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c parser.cpp $(Release_Include_Path) -o gccRelease/parser.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM parser.cpp $(Release_Include_Path) > gccRelease/parser.d

# Compiles file platform-windows.cpp for the Release configuration...
-include gccRelease/platform-windows.d
gccRelease/platform-windows.o: platform-windows.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c platform-windows.cpp $(Release_Include_Path) -o gccRelease/platform-windows.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM platform-windows.cpp $(Release_Include_Path) > gccRelease/platform-windows.d

# Compiles file printer.cpp for the Release configuration...
-include gccRelease/printer.d
gccRelease/printer.o: printer.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c printer.cpp $(Release_Include_Path) -o gccRelease/printer.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM printer.cpp $(Release_Include_Path) > gccRelease/printer.d

# Compiles file property.cpp for the Release configuration...
-include gccRelease/property.d
gccRelease/property.o: property.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c property.cpp $(Release_Include_Path) -o gccRelease/property.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM property.cpp $(Release_Include_Path) > gccRelease/property.d

# Compiles file propertydialog.cpp for the Release configuration...
-include gccRelease/propertydialog.d
gccRelease/propertydialog.o: propertydialog.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c propertydialog.cpp $(Release_Include_Path) -o gccRelease/propertydialog.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM propertydialog.cpp $(Release_Include_Path) > gccRelease/propertydialog.d

# Compiles file randfuel.cpp for the Release configuration...
-include gccRelease/randfuel.d
gccRelease/randfuel.o: randfuel.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c randfuel.cpp $(Release_Include_Path) -o gccRelease/randfuel.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM randfuel.cpp $(Release_Include_Path) > gccRelease/randfuel.d

# Compiles file randthread.cpp for the Release configuration...
-include gccRelease/randthread.d
gccRelease/randthread.o: randthread.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c randthread.cpp $(Release_Include_Path) -o gccRelease/randthread.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM randthread.cpp $(Release_Include_Path) > gccRelease/randthread.d

# Compiles file realspinbox.cpp for the Release configuration...
-include gccRelease/realspinbox.d
gccRelease/realspinbox.o: realspinbox.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c realspinbox.cpp $(Release_Include_Path) -o gccRelease/realspinbox.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM realspinbox.cpp $(Release_Include_Path) > gccRelease/realspinbox.d

# Compiles file requestdialog.cpp for the Release configuration...
-include gccRelease/requestdialog.d
gccRelease/requestdialog.o: requestdialog.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c requestdialog.cpp $(Release_Include_Path) -o gccRelease/requestdialog.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM requestdialog.cpp $(Release_Include_Path) > gccRelease/requestdialog.d

# Compiles file rundialog.cpp for the Release configuration...
-include gccRelease/rundialog.d
gccRelease/rundialog.o: rundialog.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c rundialog.cpp $(Release_Include_Path) -o gccRelease/rundialog.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM rundialog.cpp $(Release_Include_Path) > gccRelease/rundialog.d

# Compiles file rxvar.cpp for the Release configuration...
-include gccRelease/rxvar.d
gccRelease/rxvar.o: rxvar.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c rxvar.cpp $(Release_Include_Path) -o gccRelease/rxvar.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM rxvar.cpp $(Release_Include_Path) > gccRelease/rxvar.d

# Compiles file SemFireCrownFirebrandProcessor.cpp for the Release configuration...
-include gccRelease/SemFireCrownFirebrandProcessor.d
gccRelease/SemFireCrownFirebrandProcessor.o: SemFireCrownFirebrandProcessor.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c SemFireCrownFirebrandProcessor.cpp $(Release_Include_Path) -o gccRelease/SemFireCrownFirebrandProcessor.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM SemFireCrownFirebrandProcessor.cpp $(Release_Include_Path) > gccRelease/SemFireCrownFirebrandProcessor.d

# Compiles file siunits.cpp for the Release configuration...
-include gccRelease/siunits.d
gccRelease/siunits.o: siunits.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c siunits.cpp $(Release_Include_Path) -o gccRelease/siunits.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM siunits.cpp $(Release_Include_Path) > gccRelease/siunits.d

# Compiles file slopetooldialog.cpp for the Release configuration...
-include gccRelease/slopetooldialog.d
gccRelease/slopetooldialog.o: slopetooldialog.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c slopetooldialog.cpp $(Release_Include_Path) -o gccRelease/slopetooldialog.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM slopetooldialog.cpp $(Release_Include_Path) > gccRelease/slopetooldialog.d

# Compiles file standardwizards.cpp for the Release configuration...
-include gccRelease/standardwizards.d
gccRelease/standardwizards.o: standardwizards.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c standardwizards.cpp $(Release_Include_Path) -o gccRelease/standardwizards.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM standardwizards.cpp $(Release_Include_Path) > gccRelease/standardwizards.d

# Compiles file stdafx.cpp for the Release configuration...
-include gccRelease/stdafx.d
gccRelease/stdafx.o: stdafx.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c stdafx.cpp $(Release_Include_Path) -o gccRelease/stdafx.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM stdafx.cpp $(Release_Include_Path) > gccRelease/stdafx.d

# Compiles file sundialog.cpp for the Release configuration...
-include gccRelease/sundialog.d
gccRelease/sundialog.o: sundialog.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c sundialog.cpp $(Release_Include_Path) -o gccRelease/sundialog.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM sundialog.cpp $(Release_Include_Path) > gccRelease/sundialog.d

# Compiles file sunview.cpp for the Release configuration...
-include gccRelease/sunview.d
gccRelease/sunview.o: sunview.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c sunview.cpp $(Release_Include_Path) -o gccRelease/sunview.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM sunview.cpp $(Release_Include_Path) > gccRelease/sunview.d

# Compiles file textview.cpp for the Release configuration...
-include gccRelease/textview.d
gccRelease/textview.o: textview.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c textview.cpp $(Release_Include_Path) -o gccRelease/textview.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM textview.cpp $(Release_Include_Path) > gccRelease/textview.d

# Compiles file textviewdocument.cpp for the Release configuration...
-include gccRelease/textviewdocument.d
gccRelease/textviewdocument.o: textviewdocument.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c textviewdocument.cpp $(Release_Include_Path) -o gccRelease/textviewdocument.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM textviewdocument.cpp $(Release_Include_Path) > gccRelease/textviewdocument.d

# Compiles file toc.cpp for the Release configuration...
-include gccRelease/toc.d
gccRelease/toc.o: toc.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c toc.cpp $(Release_Include_Path) -o gccRelease/toc.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM toc.cpp $(Release_Include_Path) > gccRelease/toc.d

# Compiles file unitsconverterdialog.cpp for the Release configuration...
-include gccRelease/unitsconverterdialog.d
gccRelease/unitsconverterdialog.o: unitsconverterdialog.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c unitsconverterdialog.cpp $(Release_Include_Path) -o gccRelease/unitsconverterdialog.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM unitsconverterdialog.cpp $(Release_Include_Path) > gccRelease/unitsconverterdialog.d

# Compiles file unitseditdialog.cpp for the Release configuration...
-include gccRelease/unitseditdialog.d
gccRelease/unitseditdialog.o: unitseditdialog.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c unitseditdialog.cpp $(Release_Include_Path) -o gccRelease/unitseditdialog.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM unitseditdialog.cpp $(Release_Include_Path) > gccRelease/unitseditdialog.d

# Compiles file varcheckbox.cpp for the Release configuration...
-include gccRelease/varcheckbox.d
gccRelease/varcheckbox.o: varcheckbox.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c varcheckbox.cpp $(Release_Include_Path) -o gccRelease/varcheckbox.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM varcheckbox.cpp $(Release_Include_Path) > gccRelease/varcheckbox.d

# Compiles file wizarddialog.cpp for the Release configuration...
-include gccRelease/wizarddialog.d
gccRelease/wizarddialog.o: wizarddialog.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c wizarddialog.cpp $(Release_Include_Path) -o gccRelease/wizarddialog.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM wizarddialog.cpp $(Release_Include_Path) > gccRelease/wizarddialog.d

# Compiles file xeqapp.cpp for the Release configuration...
-include gccRelease/xeqapp.d
gccRelease/xeqapp.o: xeqapp.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c xeqapp.cpp $(Release_Include_Path) -o gccRelease/xeqapp.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM xeqapp.cpp $(Release_Include_Path) > gccRelease/xeqapp.d

# Compiles file xeqappparser.cpp for the Release configuration...
-include gccRelease/xeqappparser.d
gccRelease/xeqappparser.o: xeqappparser.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c xeqappparser.cpp $(Release_Include_Path) -o gccRelease/xeqappparser.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM xeqappparser.cpp $(Release_Include_Path) > gccRelease/xeqappparser.d

# Compiles file xeqcalc.cpp for the Release configuration...
-include gccRelease/xeqcalc.d
gccRelease/xeqcalc.o: xeqcalc.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c xeqcalc.cpp $(Release_Include_Path) -o gccRelease/xeqcalc.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM xeqcalc.cpp $(Release_Include_Path) > gccRelease/xeqcalc.d

# Compiles file xeqcalcmask.cpp for the Release configuration...
-include gccRelease/xeqcalcmask.d
gccRelease/xeqcalcmask.o: xeqcalcmask.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c xeqcalcmask.cpp $(Release_Include_Path) -o gccRelease/xeqcalcmask.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM xeqcalcmask.cpp $(Release_Include_Path) > gccRelease/xeqcalcmask.d

# Compiles file xeqcalcreconfig.cpp for the Release configuration...
-include gccRelease/xeqcalcreconfig.d
gccRelease/xeqcalcreconfig.o: xeqcalcreconfig.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c xeqcalcreconfig.cpp $(Release_Include_Path) -o gccRelease/xeqcalcreconfig.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM xeqcalcreconfig.cpp $(Release_Include_Path) > gccRelease/xeqcalcreconfig.d

# Compiles file xeqcalcV6Crown.cpp for the Release configuration...
-include gccRelease/xeqcalcV6Crown.d
gccRelease/xeqcalcV6Crown.o: xeqcalcV6Crown.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c xeqcalcV6Crown.cpp $(Release_Include_Path) -o gccRelease/xeqcalcV6Crown.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM xeqcalcV6Crown.cpp $(Release_Include_Path) > gccRelease/xeqcalcV6Crown.d

# Compiles file xeqcalcV6Main.cpp for the Release configuration...
-include gccRelease/xeqcalcV6Main.d
gccRelease/xeqcalcV6Main.o: xeqcalcV6Main.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c xeqcalcV6Main.cpp $(Release_Include_Path) -o gccRelease/xeqcalcV6Main.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM xeqcalcV6Main.cpp $(Release_Include_Path) > gccRelease/xeqcalcV6Main.d

# Compiles file xeqfile.cpp for the Release configuration...
-include gccRelease/xeqfile.d
gccRelease/xeqfile.o: xeqfile.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c xeqfile.cpp $(Release_Include_Path) -o gccRelease/xeqfile.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM xeqfile.cpp $(Release_Include_Path) > gccRelease/xeqfile.d

# Compiles file xeqtree.cpp for the Release configuration...
-include gccRelease/xeqtree.d
gccRelease/xeqtree.o: xeqtree.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c xeqtree.cpp $(Release_Include_Path) -o gccRelease/xeqtree.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM xeqtree.cpp $(Release_Include_Path) > gccRelease/xeqtree.d

# Compiles file xeqtreeparser.cpp for the Release configuration...
-include gccRelease/xeqtreeparser.d
gccRelease/xeqtreeparser.o: xeqtreeparser.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c xeqtreeparser.cpp $(Release_Include_Path) -o gccRelease/xeqtreeparser.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM xeqtreeparser.cpp $(Release_Include_Path) > gccRelease/xeqtreeparser.d

# Compiles file xeqtreeprint.cpp for the Release configuration...
-include gccRelease/xeqtreeprint.d
gccRelease/xeqtreeprint.o: xeqtreeprint.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c xeqtreeprint.cpp $(Release_Include_Path) -o gccRelease/xeqtreeprint.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM xeqtreeprint.cpp $(Release_Include_Path) > gccRelease/xeqtreeprint.d

# Compiles file xeqvar.cpp for the Release configuration...
-include gccRelease/xeqvar.d
gccRelease/xeqvar.o: xeqvar.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c xeqvar.cpp $(Release_Include_Path) -o gccRelease/xeqvar.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM xeqvar.cpp $(Release_Include_Path) > gccRelease/xeqvar.d

# Compiles file xeqvaritem.cpp for the Release configuration...
-include gccRelease/xeqvaritem.d
gccRelease/xeqvaritem.o: xeqvaritem.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c xeqvaritem.cpp $(Release_Include_Path) -o gccRelease/xeqvaritem.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM xeqvaritem.cpp $(Release_Include_Path) > gccRelease/xeqvaritem.d

# Compiles file xfblib.cpp for the Release configuration...
-include gccRelease/xfblib.d
gccRelease/xfblib.o: xfblib.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c xfblib.cpp $(Release_Include_Path) -o gccRelease/xfblib.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM xfblib.cpp $(Release_Include_Path) > gccRelease/xfblib.d

# Compiles file xmlparser.cpp for the Release configuration...
-include gccRelease/xmlparser.d
gccRelease/xmlparser.o: xmlparser.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c xmlparser.cpp $(Release_Include_Path) -o gccRelease/xmlparser.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM xmlparser.cpp $(Release_Include_Path) > gccRelease/xmlparser.d

# Compiles file tmp/moc/moc_aboutdialog.cpp for the Release configuration...
-include gccRelease/tmp/moc/moc_aboutdialog.d
gccRelease/tmp/moc/moc_aboutdialog.o: tmp/moc/moc_aboutdialog.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c tmp/moc/moc_aboutdialog.cpp $(Release_Include_Path) -o gccRelease/tmp/moc/moc_aboutdialog.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM tmp/moc/moc_aboutdialog.cpp $(Release_Include_Path) > gccRelease/tmp/moc/moc_aboutdialog.d

# Compiles file tmp/moc/moc_appdialog.cpp for the Release configuration...
-include gccRelease/tmp/moc/moc_appdialog.d
gccRelease/tmp/moc/moc_appdialog.o: tmp/moc/moc_appdialog.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c tmp/moc/moc_appdialog.cpp $(Release_Include_Path) -o gccRelease/tmp/moc/moc_appdialog.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM tmp/moc/moc_appdialog.cpp $(Release_Include_Path) > gccRelease/tmp/moc/moc_appdialog.d

# Compiles file tmp/moc/moc_appearancedialog.cpp for the Release configuration...
-include gccRelease/tmp/moc/moc_appearancedialog.d
gccRelease/tmp/moc/moc_appearancedialog.o: tmp/moc/moc_appearancedialog.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c tmp/moc/moc_appearancedialog.cpp $(Release_Include_Path) -o gccRelease/tmp/moc/moc_appearancedialog.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM tmp/moc/moc_appearancedialog.cpp $(Release_Include_Path) > gccRelease/tmp/moc/moc_appearancedialog.d

# Compiles file tmp/moc/moc_appmessage.cpp for the Release configuration...
-include gccRelease/tmp/moc/moc_appmessage.d
gccRelease/tmp/moc/moc_appmessage.o: tmp/moc/moc_appmessage.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c tmp/moc/moc_appmessage.cpp $(Release_Include_Path) -o gccRelease/tmp/moc/moc_appmessage.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM tmp/moc/moc_appmessage.cpp $(Release_Include_Path) > gccRelease/tmp/moc/moc_appmessage.d

# Compiles file tmp/moc/moc_appwindow.cpp for the Release configuration...
-include gccRelease/tmp/moc/moc_appwindow.d
gccRelease/tmp/moc/moc_appwindow.o: tmp/moc/moc_appwindow.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c tmp/moc/moc_appwindow.cpp $(Release_Include_Path) -o gccRelease/tmp/moc/moc_appwindow.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM tmp/moc/moc_appwindow.cpp $(Release_Include_Path) > gccRelease/tmp/moc/moc_appwindow.d

# Compiles file tmp/moc/moc_attachdialog.cpp for the Release configuration...
-include gccRelease/tmp/moc/moc_attachdialog.d
gccRelease/tmp/moc/moc_attachdialog.o: tmp/moc/moc_attachdialog.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c tmp/moc/moc_attachdialog.cpp $(Release_Include_Path) -o gccRelease/tmp/moc/moc_attachdialog.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM tmp/moc/moc_attachdialog.cpp $(Release_Include_Path) > gccRelease/tmp/moc/moc_attachdialog.d

# Compiles file tmp/moc/moc_bpdocument.cpp for the Release configuration...
-include gccRelease/tmp/moc/moc_bpdocument.d
gccRelease/tmp/moc/moc_bpdocument.o: tmp/moc/moc_bpdocument.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c tmp/moc/moc_bpdocument.cpp $(Release_Include_Path) -o gccRelease/tmp/moc/moc_bpdocument.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM tmp/moc/moc_bpdocument.cpp $(Release_Include_Path) > gccRelease/tmp/moc/moc_bpdocument.d

# Compiles file tmp/moc/moc_calendardocument.cpp for the Release configuration...
-include gccRelease/tmp/moc/moc_calendardocument.d
gccRelease/tmp/moc/moc_calendardocument.o: tmp/moc/moc_calendardocument.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c tmp/moc/moc_calendardocument.cpp $(Release_Include_Path) -o gccRelease/tmp/moc/moc_calendardocument.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM tmp/moc/moc_calendardocument.cpp $(Release_Include_Path) > gccRelease/tmp/moc/moc_calendardocument.d

# Compiles file tmp/moc/moc_conflictdialog.cpp for the Release configuration...
-include gccRelease/tmp/moc/moc_conflictdialog.d
gccRelease/tmp/moc/moc_conflictdialog.o: tmp/moc/moc_conflictdialog.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c tmp/moc/moc_conflictdialog.cpp $(Release_Include_Path) -o gccRelease/tmp/moc/moc_conflictdialog.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM tmp/moc/moc_conflictdialog.cpp $(Release_Include_Path) > gccRelease/tmp/moc/moc_conflictdialog.d

# Compiles file tmp/moc/moc_document.cpp for the Release configuration...
-include gccRelease/tmp/moc/moc_document.d
gccRelease/tmp/moc/moc_document.o: tmp/moc/moc_document.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c tmp/moc/moc_document.cpp $(Release_Include_Path) -o gccRelease/tmp/moc/moc_document.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM tmp/moc/moc_document.cpp $(Release_Include_Path) > gccRelease/tmp/moc/moc_document.d

# Compiles file tmp/moc/moc_fdfmcdialog.cpp for the Release configuration...
-include gccRelease/tmp/moc/moc_fdfmcdialog.d
gccRelease/tmp/moc/moc_fdfmcdialog.o: tmp/moc/moc_fdfmcdialog.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c tmp/moc/moc_fdfmcdialog.cpp $(Release_Include_Path) -o gccRelease/tmp/moc/moc_fdfmcdialog.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM tmp/moc/moc_fdfmcdialog.cpp $(Release_Include_Path) > gccRelease/tmp/moc/moc_fdfmcdialog.d

# Compiles file tmp/moc/moc_fileselector.cpp for the Release configuration...
-include gccRelease/tmp/moc/moc_fileselector.d
gccRelease/tmp/moc/moc_fileselector.o: tmp/moc/moc_fileselector.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c tmp/moc/moc_fileselector.cpp $(Release_Include_Path) -o gccRelease/tmp/moc/moc_fileselector.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM tmp/moc/moc_fileselector.cpp $(Release_Include_Path) > gccRelease/tmp/moc/moc_fileselector.d

# Compiles file tmp/moc/moc_fuelexportdialog.cpp for the Release configuration...
-include gccRelease/tmp/moc/moc_fuelexportdialog.d
gccRelease/tmp/moc/moc_fuelexportdialog.o: tmp/moc/moc_fuelexportdialog.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c tmp/moc/moc_fuelexportdialog.cpp $(Release_Include_Path) -o gccRelease/tmp/moc/moc_fuelexportdialog.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM tmp/moc/moc_fuelexportdialog.cpp $(Release_Include_Path) > gccRelease/tmp/moc/moc_fuelexportdialog.d

# Compiles file tmp/moc/moc_fuelinitdialog.cpp for the Release configuration...
-include gccRelease/tmp/moc/moc_fuelinitdialog.d
gccRelease/tmp/moc/moc_fuelinitdialog.o: tmp/moc/moc_fuelinitdialog.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c tmp/moc/moc_fuelinitdialog.cpp $(Release_Include_Path) -o gccRelease/tmp/moc/moc_fuelinitdialog.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM tmp/moc/moc_fuelinitdialog.cpp $(Release_Include_Path) > gccRelease/tmp/moc/moc_fuelinitdialog.d

# Compiles file tmp/moc/moc_fuelmodeldialog.cpp for the Release configuration...
-include gccRelease/tmp/moc/moc_fuelmodeldialog.d
gccRelease/tmp/moc/moc_fuelmodeldialog.o: tmp/moc/moc_fuelmodeldialog.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c tmp/moc/moc_fuelmodeldialog.cpp $(Release_Include_Path) -o gccRelease/tmp/moc/moc_fuelmodeldialog.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM tmp/moc/moc_fuelmodeldialog.cpp $(Release_Include_Path) > gccRelease/tmp/moc/moc_fuelmodeldialog.d

# Compiles file tmp/moc/moc_fuelwizards.cpp for the Release configuration...
-include gccRelease/tmp/moc/moc_fuelwizards.d
gccRelease/tmp/moc/moc_fuelwizards.o: tmp/moc/moc_fuelwizards.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c tmp/moc/moc_fuelwizards.cpp $(Release_Include_Path) -o gccRelease/tmp/moc/moc_fuelwizards.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM tmp/moc/moc_fuelwizards.cpp $(Release_Include_Path) > gccRelease/tmp/moc/moc_fuelwizards.d

# Compiles file tmp/moc/moc_graphlimitsdialog.cpp for the Release configuration...
-include gccRelease/tmp/moc/moc_graphlimitsdialog.d
gccRelease/tmp/moc/moc_graphlimitsdialog.o: tmp/moc/moc_graphlimitsdialog.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c tmp/moc/moc_graphlimitsdialog.cpp $(Release_Include_Path) -o gccRelease/tmp/moc/moc_graphlimitsdialog.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM tmp/moc/moc_graphlimitsdialog.cpp $(Release_Include_Path) > gccRelease/tmp/moc/moc_graphlimitsdialog.d

# Compiles file tmp/moc/moc_guidedialog.cpp for the Release configuration...
-include gccRelease/tmp/moc/moc_guidedialog.d
gccRelease/tmp/moc/moc_guidedialog.o: tmp/moc/moc_guidedialog.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c tmp/moc/moc_guidedialog.cpp $(Release_Include_Path) -o gccRelease/tmp/moc/moc_guidedialog.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM tmp/moc/moc_guidedialog.cpp $(Release_Include_Path) > gccRelease/tmp/moc/moc_guidedialog.d

# Compiles file tmp/moc/moc_helpbrowser.cpp for the Release configuration...
-include gccRelease/tmp/moc/moc_helpbrowser.d
gccRelease/tmp/moc/moc_helpbrowser.o: tmp/moc/moc_helpbrowser.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c tmp/moc/moc_helpbrowser.cpp $(Release_Include_Path) -o gccRelease/tmp/moc/moc_helpbrowser.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM tmp/moc/moc_helpbrowser.cpp $(Release_Include_Path) > gccRelease/tmp/moc/moc_helpbrowser.d

# Compiles file tmp/moc/moc_horizontaldistancedialog.cpp for the Release configuration...
-include gccRelease/tmp/moc/moc_horizontaldistancedialog.d
gccRelease/tmp/moc/moc_horizontaldistancedialog.o: tmp/moc/moc_horizontaldistancedialog.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c tmp/moc/moc_horizontaldistancedialog.cpp $(Release_Include_Path) -o gccRelease/tmp/moc/moc_horizontaldistancedialog.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM tmp/moc/moc_horizontaldistancedialog.cpp $(Release_Include_Path) > gccRelease/tmp/moc/moc_horizontaldistancedialog.d

# Compiles file tmp/moc/moc_humiditydialog.cpp for the Release configuration...
-include gccRelease/tmp/moc/moc_humiditydialog.d
gccRelease/tmp/moc/moc_humiditydialog.o: tmp/moc/moc_humiditydialog.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c tmp/moc/moc_humiditydialog.cpp $(Release_Include_Path) -o gccRelease/tmp/moc/moc_humiditydialog.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM tmp/moc/moc_humiditydialog.cpp $(Release_Include_Path) > gccRelease/tmp/moc/moc_humiditydialog.d

# Compiles file tmp/moc/moc_modulesdialog.cpp for the Release configuration...
-include gccRelease/tmp/moc/moc_modulesdialog.d
gccRelease/tmp/moc/moc_modulesdialog.o: tmp/moc/moc_modulesdialog.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c tmp/moc/moc_modulesdialog.cpp $(Release_Include_Path) -o gccRelease/tmp/moc/moc_modulesdialog.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM tmp/moc/moc_modulesdialog.cpp $(Release_Include_Path) > gccRelease/tmp/moc/moc_modulesdialog.d

# Compiles file tmp/moc/moc_pagemenudialog.cpp for the Release configuration...
-include gccRelease/tmp/moc/moc_pagemenudialog.d
gccRelease/tmp/moc/moc_pagemenudialog.o: tmp/moc/moc_pagemenudialog.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c tmp/moc/moc_pagemenudialog.cpp $(Release_Include_Path) -o gccRelease/tmp/moc/moc_pagemenudialog.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM tmp/moc/moc_pagemenudialog.cpp $(Release_Include_Path) > gccRelease/tmp/moc/moc_pagemenudialog.d

# Compiles file tmp/moc/moc_propertydialog.cpp for the Release configuration...
-include gccRelease/tmp/moc/moc_propertydialog.d
gccRelease/tmp/moc/moc_propertydialog.o: tmp/moc/moc_propertydialog.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c tmp/moc/moc_propertydialog.cpp $(Release_Include_Path) -o gccRelease/tmp/moc/moc_propertydialog.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM tmp/moc/moc_propertydialog.cpp $(Release_Include_Path) > gccRelease/tmp/moc/moc_propertydialog.d

# Compiles file tmp/moc/moc_realspinbox.cpp for the Release configuration...
-include gccRelease/tmp/moc/moc_realspinbox.d
gccRelease/tmp/moc/moc_realspinbox.o: tmp/moc/moc_realspinbox.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c tmp/moc/moc_realspinbox.cpp $(Release_Include_Path) -o gccRelease/tmp/moc/moc_realspinbox.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM tmp/moc/moc_realspinbox.cpp $(Release_Include_Path) > gccRelease/tmp/moc/moc_realspinbox.d

# Compiles file tmp/moc/moc_rundialog.cpp for the Release configuration...
-include gccRelease/tmp/moc/moc_rundialog.d
gccRelease/tmp/moc/moc_rundialog.o: tmp/moc/moc_rundialog.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c tmp/moc/moc_rundialog.cpp $(Release_Include_Path) -o gccRelease/tmp/moc/moc_rundialog.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM tmp/moc/moc_rundialog.cpp $(Release_Include_Path) > gccRelease/tmp/moc/moc_rundialog.d

# Compiles file tmp/moc/moc_slopetooldialog.cpp for the Release configuration...
-include gccRelease/tmp/moc/moc_slopetooldialog.d
gccRelease/tmp/moc/moc_slopetooldialog.o: tmp/moc/moc_slopetooldialog.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c tmp/moc/moc_slopetooldialog.cpp $(Release_Include_Path) -o gccRelease/tmp/moc/moc_slopetooldialog.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM tmp/moc/moc_slopetooldialog.cpp $(Release_Include_Path) > gccRelease/tmp/moc/moc_slopetooldialog.d

# Compiles file tmp/moc/moc_standardwizards.cpp for the Release configuration...
-include gccRelease/tmp/moc/moc_standardwizards.d
gccRelease/tmp/moc/moc_standardwizards.o: tmp/moc/moc_standardwizards.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c tmp/moc/moc_standardwizards.cpp $(Release_Include_Path) -o gccRelease/tmp/moc/moc_standardwizards.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM tmp/moc/moc_standardwizards.cpp $(Release_Include_Path) > gccRelease/tmp/moc/moc_standardwizards.d

# Compiles file tmp/moc/moc_sundialog.cpp for the Release configuration...
-include gccRelease/tmp/moc/moc_sundialog.d
gccRelease/tmp/moc/moc_sundialog.o: tmp/moc/moc_sundialog.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c tmp/moc/moc_sundialog.cpp $(Release_Include_Path) -o gccRelease/tmp/moc/moc_sundialog.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM tmp/moc/moc_sundialog.cpp $(Release_Include_Path) > gccRelease/tmp/moc/moc_sundialog.d

# Compiles file tmp/moc/moc_textview.cpp for the Release configuration...
-include gccRelease/tmp/moc/moc_textview.d
gccRelease/tmp/moc/moc_textview.o: tmp/moc/moc_textview.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c tmp/moc/moc_textview.cpp $(Release_Include_Path) -o gccRelease/tmp/moc/moc_textview.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM tmp/moc/moc_textview.cpp $(Release_Include_Path) > gccRelease/tmp/moc/moc_textview.d

# Compiles file tmp/moc/moc_textviewdocument.cpp for the Release configuration...
-include gccRelease/tmp/moc/moc_textviewdocument.d
gccRelease/tmp/moc/moc_textviewdocument.o: tmp/moc/moc_textviewdocument.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c tmp/moc/moc_textviewdocument.cpp $(Release_Include_Path) -o gccRelease/tmp/moc/moc_textviewdocument.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM tmp/moc/moc_textviewdocument.cpp $(Release_Include_Path) > gccRelease/tmp/moc/moc_textviewdocument.d

# Compiles file tmp/moc/moc_unitsconverterdialog.cpp for the Release configuration...
-include gccRelease/tmp/moc/moc_unitsconverterdialog.d
gccRelease/tmp/moc/moc_unitsconverterdialog.o: tmp/moc/moc_unitsconverterdialog.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c tmp/moc/moc_unitsconverterdialog.cpp $(Release_Include_Path) -o gccRelease/tmp/moc/moc_unitsconverterdialog.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM tmp/moc/moc_unitsconverterdialog.cpp $(Release_Include_Path) > gccRelease/tmp/moc/moc_unitsconverterdialog.d

# Compiles file tmp/moc/moc_unitseditdialog.cpp for the Release configuration...
-include gccRelease/tmp/moc/moc_unitseditdialog.d
gccRelease/tmp/moc/moc_unitseditdialog.o: tmp/moc/moc_unitseditdialog.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c tmp/moc/moc_unitseditdialog.cpp $(Release_Include_Path) -o gccRelease/tmp/moc/moc_unitseditdialog.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM tmp/moc/moc_unitseditdialog.cpp $(Release_Include_Path) > gccRelease/tmp/moc/moc_unitseditdialog.d

# Compiles file tmp/moc/moc_varcheckbox.cpp for the Release configuration...
-include gccRelease/tmp/moc/moc_varcheckbox.d
gccRelease/tmp/moc/moc_varcheckbox.o: tmp/moc/moc_varcheckbox.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c tmp/moc/moc_varcheckbox.cpp $(Release_Include_Path) -o gccRelease/tmp/moc/moc_varcheckbox.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM tmp/moc/moc_varcheckbox.cpp $(Release_Include_Path) > gccRelease/tmp/moc/moc_varcheckbox.d

# Compiles file tmp/moc/moc_wizarddialog.cpp for the Release configuration...
-include gccRelease/tmp/moc/moc_wizarddialog.d
gccRelease/tmp/moc/moc_wizarddialog.o: tmp/moc/moc_wizarddialog.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c tmp/moc/moc_wizarddialog.cpp $(Release_Include_Path) -o gccRelease/tmp/moc/moc_wizarddialog.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM tmp/moc/moc_wizarddialog.cpp $(Release_Include_Path) > gccRelease/tmp/moc/moc_wizarddialog.d

# Compiles file Algorithms/Fofem6BarkThickness.cpp for the Release configuration...
-include gccRelease/Algorithms/Fofem6BarkThickness.d
gccRelease/Algorithms/Fofem6BarkThickness.o: Algorithms/Fofem6BarkThickness.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c Algorithms/Fofem6BarkThickness.cpp $(Release_Include_Path) -o gccRelease/Algorithms/Fofem6BarkThickness.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM Algorithms/Fofem6BarkThickness.cpp $(Release_Include_Path) > gccRelease/Algorithms/Fofem6BarkThickness.d

# Compiles file Algorithms/Fofem6Mortality.cpp for the Release configuration...
-include gccRelease/Algorithms/Fofem6Mortality.d
gccRelease/Algorithms/Fofem6Mortality.o: Algorithms/Fofem6Mortality.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c Algorithms/Fofem6Mortality.cpp $(Release_Include_Path) -o gccRelease/Algorithms/Fofem6Mortality.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM Algorithms/Fofem6Mortality.cpp $(Release_Include_Path) > gccRelease/Algorithms/Fofem6Mortality.d

# Compiles file Algorithms/Fofem6Species.cpp for the Release configuration...
-include gccRelease/Algorithms/Fofem6Species.d
gccRelease/Algorithms/Fofem6Species.o: Algorithms/Fofem6Species.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c Algorithms/Fofem6Species.cpp $(Release_Include_Path) -o gccRelease/Algorithms/Fofem6Species.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM Algorithms/Fofem6Species.cpp $(Release_Include_Path) > gccRelease/Algorithms/Fofem6Species.d

# Compiles file Algorithms/Fofem6SpeciesData.cpp for the Release configuration...
-include gccRelease/Algorithms/Fofem6SpeciesData.d
gccRelease/Algorithms/Fofem6SpeciesData.o: Algorithms/Fofem6SpeciesData.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c Algorithms/Fofem6SpeciesData.cpp $(Release_Include_Path) -o gccRelease/Algorithms/Fofem6SpeciesData.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM Algorithms/Fofem6SpeciesData.cpp $(Release_Include_Path) > gccRelease/Algorithms/Fofem6SpeciesData.d

# Creates the intermediate and output folders for each configuration...
.PHONY: create_folders
create_folders:
	mkdir -p gccDebug/source
	mkdir -p gccRelease/source

# Cleans intermediate and output files (objects, libraries, executables)...
.PHONY: clean
clean:
	rm -f gccDebug/*.o
	rm -f gccDebug/*.d
	rm -f gccDebug/*.a
	rm -f gccDebug/*.so
	rm -f gccDebug/*.dll
	rm -f gccDebug/*.exe
	rm -f gccRelease/*.o
	rm -f gccRelease/*.d
	rm -f gccRelease/*.a
	rm -f gccRelease/*.so
	rm -f gccRelease/*.dll
	rm -f gccRelease/*.exe

