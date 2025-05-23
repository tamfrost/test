build_documentation() {
    set -x

    cd doc 

    asciidoctor -o index.html documentation.adoc

    printf "\n\033[0;32mreplacing https://cdnjs.cloudflare.com/ajax/libs/mathjax/2.7.9\nwith ${MATHJAX_URL}\033[0m \n"
    sed -i "s@https://cdnjs.cloudflare.com/ajax/libs/mathjax/2.7.9@${MATHJAX_URL}@g" index.html

    # #genarate docbook
    # docker run --rm -v $(pwd)/doc:/documents $IMAGE_REGISTRY_PATH_MAIN/asciidoctor/docker-asciidoctor asciidoctor -b docbook documentation.adoc

    # #genarate pdf
    # docker run --rm -v "$(pwd)/doc:/data" --user $(id -u):$(id -g) $IMAGE_REGISTRY_PATH_MAIN/pandoc/latex -r docbook --toc -V colorlinks -V geometry:margin=25mm -V block-headings documentation.xml -o documentation.pdf

    # #genarate ms-word
    # docker run -v $(pwd)/doc:/data $IMAGE_REGISTRY_PATH_MAIN/pandoc/latex -r docbook -t docx -o documentation.docx documentation.xml
}

build_wasm() {
# written for dspdf-wasm
    PS4=$(printf "\n\033[1;33mWASM >>\033[0m ")
    set -x

     cd wasm
    /bin/bash build_cicd.sh
    ## TODO: code below using cmake is not working for some reason
    # cd wasm
    # mkdir build
    # cd build
    # cmake -DCMAKE_TOOLCHAIN_FILE="/emsdk/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake" -DBUILD_TYPE=wasm -DCOMMIT_HASH=${CI_COMMIT_SHORT_SHA} .. || exit 1
    # make || exit 1
    # cd ..
    # rm -rf build
}

