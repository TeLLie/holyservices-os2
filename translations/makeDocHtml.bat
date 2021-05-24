set COMMON_OPTIONS= "paper.type=A4" "admon.graphics=0" "admon.graphics.path=/DocBook/images/" "saxon.character.representation=native"

cd en\doc

java -jar "\Program Files\saxon\saxon9he.jar" -o HolyServices.html HolyServices.xml \DocBook\html\docbook.xsl %COMMON_OPTIONS% "l10n.gentext.language=en"


cd ..\..

cd cz\doc

java -jar "\Program Files\saxon\saxon9he.jar" -o HolyServices.html HolyServices.xml \DocBook\html\docbook.xsl %COMMON_OPTIONS% "l10n.gentext.language=cs"

cd ..\..

cd sk\doc

java -jar "\Program Files\saxon\saxon9he.jar" -o HolyServices.html HolyServices.xml \DocBook\html\docbook.xsl %COMMON_OPTIONS% "l10n.gentext.language=sk"

cd ..\..

