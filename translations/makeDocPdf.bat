set XEPBATCH="c:\kovis\XEP\xep.bat"
set COMMON_OPTIONS= "paper.type=A4" "admon.graphics=1" "admon.graphics.path=/DocBook/images/" "xep.extensions=1" "saxon.character.representation=native"

cd en\doc

java -jar "\Program Files\saxon\saxon9he.jar" -o HolyServices.fo HolyServices.xml \DocBook\fo\docbook.xsl %COMMON_OPTIONS% "l10n.gentext.language=en"

call %XEPBATCH% -fo HolyServices.fo -pdf HolyServices.pdf
cd ..\..

cd cz\doc

java -jar "\Program Files\saxon\saxon9he.jar" -o HolyServices.fo HolyServices.xml \DocBook\fo\docbook.xsl %COMMON_OPTIONS% "l10n.gentext.language=cs"

call %XEPBATCH% -fo HolyServices.fo -pdf HolyServices.pdf

cd ..\..

cd sk\doc

java -jar "\Program Files\saxon\saxon9he.jar" -o HolyServices.fo HolyServices.xml \DocBook\fo\docbook.xsl %COMMON_OPTIONS% "l10n.gentext.language=sk"

call %XEPBATCH% -fo HolyServices.fo -pdf HolyServices.pdf

cd ..\..
