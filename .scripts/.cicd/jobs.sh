build_documentation() {
    set -x

    asciidoctor -o index.html documentation.adoc

    printf "\n\033[0;32mreplacing https://cdnjs.cloudflare.com/ajax/libs/mathjax/2.7.9\nwith ${MATHJAX_URL}\033[0m \n"
    sed -i "s@https://cdnjs.cloudflare.com/ajax/libs/mathjax/2.7.9@${MATHJAX_URL}@g" doc/index.html

    # #genarate docbook
    # docker run --rm -v $(pwd)/doc:/documents $IMAGE_REGISTRY_PATH_MAIN/asciidoctor/docker-asciidoctor asciidoctor -b docbook documentation.adoc

    # #genarate pdf
    # docker run --rm -v "$(pwd)/doc:/data" --user $(id -u):$(id -g) $IMAGE_REGISTRY_PATH_MAIN/pandoc/latex -r docbook --toc -V colorlinks -V geometry:margin=25mm -V block-headings documentation.xml -o documentation.pdf

    # #genarate ms-word
    # docker run -v $(pwd)/doc:/data $IMAGE_REGISTRY_PATH_MAIN/pandoc/latex -r docbook -t docx -o documentation.docx documentation.xml
}
