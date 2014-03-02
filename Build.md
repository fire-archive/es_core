# How to build es_core

es_core depends on nanomsg, ogre3d and sdl2.

## Ogredeps

Hg clone https://bitbucket.org/binarycrusader/ogredeps.

```
mkdir Build
cd Build
cmake -G "Visual Studio 12 Win64" -DCMAKE_INSTALL_PREFIX=./Run/ ..
```

Build Release Build/INSTALL.vcxproj.

Build Debug Build/INSTALL.vcxproj.

Publish artifacts from Build/Run.

## Ogre3d

Hg clone https://bitbucket.org/sinbad/ogre.

Depends on files from ogredep.

```
bin=>./Build/Run/bin
include=>./Build/Run/include
lib=>./Build/Run/lib
```

Generate project files.

```
mkdir Build
cd Build
cmake -G "Visual Studio 12 Win64" -DOGRE_BUILD_RENDERSYSTEM_D3D11=0 -DOGRE_BUILD_RENDERSYSTEM_D3D9=0 -DOGRE_BUILD_RENDERSYSTEM_GL3PLUS=1 -DOGRE_BUILD_RENDERSYSTEM_GL=1 -DOGRE_DEPENDENCIES_DIR=./Run/ -DOGRE_BUILD_SAMPLES=1 -DOGRE_UNITY_BUILD=1 -DCMAKE_INSTALL_PREFIX=./Run/ ..
```

Build Release Build\INSTALL.vcxproj.

Publish artifacts from Build/Run.

## Nanomsg

Git clone https://github.com/nanomsg/nanomsg.git.

```
mkdir Build
cd Build
cmake -G "Visual Studio 12 Win64" -DCMAKE_INSTALL_PREFIX=./Run/ ..
```

Build Release Build/INSTALL.vcxproj.

Publish Build/Run/.

## Sdl2 

Hg clone https://hg.libsdl.org/SDL.

```
mkdir Build
cd Build
cmake -G "Visual Studio 12 Win64" -DCMAKE_INSTALL_PREFIX=./Run/ ..
```

Build Release Build/INSTALL.vcxproj.

Publish: 
```
Build\Run\bin => bin
Build\Run\include => include
Build\Run\lib => lib
```

## es_core

Git clone https://github.com/fire/es_core.git.

Depends on nanomsg:

```
bin=>Build/Run/bin
include=>Build/Run/include
lib=>Build/Run/lib
```

Depends on ogre3d:
```
bin/Release/*.dll=>Build/Run/bin
bin=>Build/Run/bin
CMake=>Build/Run/CMake
include=>Build/Run/include
lib=>Build/Run/lib
media=>Build/Run/bin/media
```

Depends on SDL2:

```
include=>Build/Run/include
lib=>Build/Run/lib
bin=>Build/Run/bin
```

```
mkdir Build
cd Build
cmake -G "Visual Studio 12 Win64" -DCMAKE_INSTALL_PREFIX=./Run ..
```

Build Build/INSTALL.vcxproj.

