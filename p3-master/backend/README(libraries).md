1. Ajouter restbed comme submodule:

$ git submodule add git@github.com:Corvusoft/restbed.git restbed

$ git submodule update --init --recursive


2. Inclure restbed dans votre projet. Il faut inclure les lignes suivantes dans le CMakeLists.txt de votre dossier lib:

option(BUILD_SSL OFF)

option(BUILD_TESTS OFF)

add_subdirectory(restbed)

target_include_directories(restbed-static PUBLIC restbed/source)



3. Linker restbed à votre projet. Il faut inclure les lignes suivantes dans le CMakeLists.txt de votre dossier src ou bien celui d'un des projets si restbed est utilisé, par exemple, uniquement par le serveur:

target_link_libraries(common PUBLIC restbed-static)


4. Ajouter "pthread" dans le CMakeList.txt de restbed (Path: backend/lib/restbed) -> remplacer à la ligne 65 :  

    if ( BUILD_SSL )
    	target_link_libraries( ${STATIC_LIBRARY_NAME} LINK_PRIVATE ${ssl_LIBRARY_STATIC} ${crypto_LIBRARY_STATIC} ${CMAKE_DL_LIBS} pthread)
    else ( )
        target_link_libraries( ${STATIC_LIBRARY_NAME} ${CMAKE_DL_LIBS} pthread)
    endif ( ) 


5. Vous pouvez par la suite compiler votre projet (mineur ou serveur):

$ ./compile.sh server


6. Penser à recompile à chaque fois que l'on change le code dans project

