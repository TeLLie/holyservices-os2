<?xml version="1.0" encoding="utf-8"?>

<xsl:stylesheet version="1.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:template match="/">
    <html>
    <body>
	<TABLE BORDER="1">
	<TR BGCOLOR="lightgreen"><TH>dátum</TH><TH>úmysel</TH><TH>farník</TH></TR>
	<xsl:for-each select="//OutputService">
		<TR>
			<TD> <xsl:value-of select="@StartWeekDay"/><xsl:text> </xsl:text><xsl:value-of select="@StartDateDM"/><xsl:text> </xsl:text><xsl:value-of select="@StartTimeHM"/></TD>
		<TD> <xsl:value-of select="@Intention"/> </TD>
   		<TD> <xsl:value-of select="@Client"/> </TD>
    	</TR>
	</xsl:for-each>
  	</TABLE>
    </body>
    </html>
</xsl:template>

</xsl:stylesheet> 
