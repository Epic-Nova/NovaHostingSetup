installer: EpicNovaInstaller.o Screens.o build
	g++ -lcurl build/EpicNovaInstaller.o  build/Screens.o -o build/installer 


EpicNovaInstaller.o: EpicNovaInstaller.cpp EpicNovaInstaller.h build
	g++ -c -lcurl EpicNovaInstaller.cpp -o build/EpicNovaInstaller.o 

Screens.o: Screens.cpp Screens.h IncludeFile.h build
	g++ -c -lcurl Screens.cpp -o build/Screens.o 


build: 
	mkdir build

clean:
	rm -r build