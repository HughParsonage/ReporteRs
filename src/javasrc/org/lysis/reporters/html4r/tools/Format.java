/*
 * This file is part of ReporteRs
 * Copyright (c) 2014, David Gohel All rights reserved.
 * This program is licensed under the GNU GENERAL PUBLIC LICENSE V3.
 * You may obtain a copy of the License at :
 * http://www.gnu.org/licenses/gpl.html
 */

package org.lysis.reporters.html4r.tools;

import java.util.Iterator;
import java.util.LinkedHashMap;
public class Format {
	
	public static String getJSString( LinkedHashMap<String, String> properties ){
		String out = "";
		for (Iterator<String> it1 = properties.keySet().iterator(); it1.hasNext();) {
			String prop = it1.next();
			out += prop + ":" + properties.get(prop) + ";";
		}
		return out;
	}
	public static LinkedHashMap<String, String> getTextProperties(String color,int fontsize, boolean strbold, boolean italic, boolean underlined, String fontfamily, String valign) {
		LinkedHashMap<String, String> runProperties = new LinkedHashMap<String, String>();
		

		runProperties.put("font-size",fontsize+"px");
		runProperties.put("font-family",fontfamily);
				
		if( strbold ){
			runProperties.put("font-weight","bold");
		}
		
		if( italic ){
			runProperties.put("font-style","italic");
		}
		
		if( underlined ){
			runProperties.put("text-decoration","underline");
		}
		runProperties.put("color",color);
		
		if( valign.equals("subscript")){
			runProperties.put("vertical-align","sub");
		} else if( valign.equals("superscript")){
			runProperties.put("vertical-align","super");
		}
		
		return runProperties;
	}

	public static LinkedHashMap<String, String> getParProperties(String textalign,int paddingbottom
			, int paddingtop, int paddingleft, int paddingright) {
		LinkedHashMap<String, String> parProperties = new LinkedHashMap<String, String>();


		if( textalign.equals("left")) parProperties.put("text-align", "left");	
		else if( textalign.equals("center")) parProperties.put("text-align", "center");	
		else if( textalign.equals("right")) parProperties.put("text-align", "right");	
		else if( textalign.equals("justify")) parProperties.put("text-align", "justify");	
		else parProperties.put("text-align", "left");
		
		parProperties.put("margin-top", paddingtop+"px");	
		parProperties.put("margin-bottom", paddingbottom+"px");	
		parProperties.put("margin-right", paddingright+"px");	
		parProperties.put("margin-left", paddingleft+"px");	
 

        return parProperties;

	}


	
	public static LinkedHashMap<String, String> getCellProperties(String borderBottomColor, String borderBottomStyle, int borderBottomWidth
			, String borderLeftColor, String borderLeftStyle, int borderLeftWidth
			, String borderTopColor, String borderTopStyle, int borderTopWidth
			, String borderRightColor, String borderRightStyle, int borderRightWidth, 
			String verticalAlign, int paddingBottom, int paddingTop, int paddingLeft, int paddingRight
			, String backgroundColor
			)  {
		LinkedHashMap<String, String> tcPr = new LinkedHashMap<String, String>();

		tcPr.put("background-color", backgroundColor);	

		tcPr.put("border-bottom-color", borderBottomColor );	
		tcPr.put("border-bottom-style", borderBottomStyle );	
		tcPr.put("border-bottom-width", borderBottomWidth + "px" );	
		tcPr.put("border-top-color", borderTopColor );	
		tcPr.put("border-top-style", borderTopStyle );	
		tcPr.put("border-top-width", borderTopWidth + "px" );	
		tcPr.put("border-right-color", borderRightColor );	
		tcPr.put("border-right-style", borderRightStyle );	
		tcPr.put("border-right-width", borderRightWidth + "px" );	
		tcPr.put("border-left-color", borderLeftColor );	
		tcPr.put("border-left-style", borderLeftStyle );	
		tcPr.put("border-left-width", borderLeftWidth + "px" );	

		if( verticalAlign.equals("center") )
			tcPr.put("vertical-align", "middle" );	
		else tcPr.put("vertical-align", verticalAlign );	
		
	
		tcPr.put("padding-left", paddingLeft + "px" );	
		tcPr.put("padding-right", paddingRight + "px" );	
		tcPr.put("padding-top", paddingTop + "px" );	
		tcPr.put("padding-bottom", paddingBottom + "px" );	

	    return tcPr;
	}
	


}
